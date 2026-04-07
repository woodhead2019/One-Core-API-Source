/*++

Copyright (c) 2025 Shorthorn Project

Module Name:

    d3dkmt.c

Abstract:

    Implement GDI Font Info and Managemet Transfer Emulation Functions

Author:

    Skulltrail 11-Febraoy-2021
	
Environment:

    User mode only.	

Revision History:

--*/
#include <main.h>

WINE_DEFAULT_DEBUG_CHANNEL(font);

HDC currentHdcFont;

#define FIRST_FONT_HANDLE 1
#define MAX_FONT_HANDLES  256

#define MS_MAKE_TAG( _x1, _x2, _x3, _x4 ) \
          ( ( (FT_ULong)_x4 << 24 ) |     \
            ( (FT_ULong)_x3 << 16 ) |     \
            ( (FT_ULong)_x2 <<  8 ) |     \
            (FT_ULong)_x1 )

#define MS_TTCF_TAG MS_MAKE_TAG('t', 't', 'c', 'f')

typedef struct {
    GLYPHMETRICS gm;
    ABC          abc;  /* metrics of the unrotated char */
    BOOL         init;
} GM;

typedef struct {
    FLOAT eM11, eM12;
    FLOAT eM21, eM22;
} FMAT2;

typedef struct {
    DWORD hash;
    LOGFONTW lf;
    FMAT2 matrix;
    BOOL can_use_bitmap;
} FONT_DESC;

typedef struct tagGdiFont GdiFont;

struct tagGdiFont {
    struct list entry;
    struct list unused_entry;
    unsigned int refcount;
    GM **gm;
    DWORD gmsize;
    OUTLINETEXTMETRICW *potm;
    DWORD total_kern_pairs;
    KERNINGPAIR *kern_pairs;
    struct list child_fonts;

    /* the following members can be accessed without locking, they are never modified after creation */
    FT_Face ft_face;
    struct font_mapping *mapping;
    LPWSTR name;
    int charset;
    int codepage;
    BOOL fake_italic;
    BOOL fake_bold;
    BYTE underline;
    BYTE strikeout;
    INT orientation;
    FONT_DESC font_desc;
    LONG aveWidth, ppem;
    double scale_y;
    SHORT yMax;
    SHORT yMin;
    DWORD ntmFlags;
    DWORD aa_flags;
    UINT ntmCellHeight, ntmAvgWidth;
    FONTSIGNATURE fs;
    GdiFont *base_font;
    VOID *GSUB_Table;
    const VOID *vert_feature;
    ULONG ttc_item_offset; /* 0 if font is not a part of TrueType collection */
    DWORD cache_num;
    DWORD instance_id;
    struct font_fileinfo *fileinfo;
};

struct font_fileinfo {
    FILETIME writetime;
    LARGE_INTEGER size;
    WCHAR path[1];
};

struct font_handle_entry
{
    void *obj;
    WORD  generation; /* generation count for reusing handle values */
};

static struct font_handle_entry font_handles[MAX_FONT_HANDLES];
static struct font_handle_entry *next_free;
static struct font_handle_entry *next_unused = font_handles;

static inline struct font_handle_entry *handle_entry( DWORD handle )
{
    unsigned int idx = LOWORD(handle) - FIRST_FONT_HANDLE;

    if (idx < MAX_FONT_HANDLES)
    {
        if (!HIWORD( handle ) || HIWORD( handle ) == font_handles[idx].generation)
            return &font_handles[idx];
    }
    return NULL;
}

LSTATUS GetFontFileFromRegistry(
    LPCWSTR faceName,
    LPCWSTR *outPath /* to be freed by caller */
)
{
    HKEY hKey;
    DWORD i = 0;
    WCHAR valueName[256];
    WCHAR data[MAX_PATH];
    WCHAR outDir[MAX_PATH];
    DWORD valueSize, dataSize, type;
    LSTATUS status;
    DWORD face_len;
    LPWSTR str;

    if ((status = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
            0,
            STANDARD_RIGHTS_READ | KEY_QUERY_VALUE,
            &hKey)) != ERROR_SUCCESS)
        return status;

    face_len = lstrlenW(faceName);

    while (1)
    {
        valueSize = sizeof(valueName) / sizeof(WCHAR);
        dataSize  = sizeof(data);

        status = RegEnumValueW(
                hKey,
                i++,
                valueName,
                &valueSize,
                NULL,
                &type,
                (LPBYTE)&data,
                &dataSize);

        if (status != ERROR_SUCCESS) {
            if (status == ERROR_NO_MORE_ITEMS)
                break;
            if (status == ERROR_MORE_DATA) /* Font display name or value is too long */
                continue;
            break;
        }

        if (type != REG_SZ)
            continue;

        if (face_len > 0 &&
            wcsncmp(valueName, faceName, face_len) == 0 &&
            (valueName[face_len] == L'\0' || valueName[face_len] == L' ' || valueName[face_len] == L'('))
        {
            if (wcschr(data, L':') != NULL) {
                lstrcpyW(outDir, data);
            } else {
                GetWindowsDirectoryW(outDir, MAX_PATH);
                lstrcatW(outDir, L"\\Fonts\\");
                lstrcatW(outDir, data);
            }
            break;
        }
    }

    RegCloseKey(hKey);
    if (status != ERROR_SUCCESS)
        return status;

    str = malloc((lstrlenW(outDir) + 1) * sizeof(WCHAR));
    if (str == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    lstrcpyW(str, outDir);
    *outPath = str;
    return ERROR_SUCCESS;
}

BOOL 
WINAPI 
GetFontRealizationInfo(HDC hdc, struct font_realization_info *info)
{
    REALIZATION_INFO reinfo;
    HFONT font;

    if (!GdiRealizationInfo(hdc, &reinfo))
        return FALSE;

    if (!(font = GetCurrentObject(hdc, OBJ_FONT)))
        return FALSE;

    info->flags      = reinfo.iTechnology;
    info->cache_num  = 0;
    info->instance_id = (DWORD)font;

    if (info->size >= 20)
        info->file_count = 1;
    if (info->size >= 22)
        info->face_index = reinfo.iFontFileId;
    if (info->size >= 24)
        info->simulations = 0;

    return TRUE;
}

BOOL WINAPI GetFontFileInfo(DWORD instance_id, DWORD file_index, struct font_fileinfo *info, SIZE_T size, SIZE_T *needed)
{
    LSTATUS status;
    LPWSTR filePath = NULL;
    WIN32_FILE_ATTRIBUTE_DATA data = { 0 };
    DWORD requiredSize;
    HFONT font;
    LOGFONTW logfont = { 0 };

    if (!handle_entry(instance_id))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    /* instance_id здесь — это HFONT */
    font = (HFONT)instance_id;
    if (!GetObjectW(font, sizeof(logfont), &logfont))
        return FALSE;

    status = GetFontFileFromRegistry(logfont.lfFaceName, &filePath);
    if (status != ERROR_SUCCESS)
    {
        SetLastError(status);
        return FALSE;
    }

    requiredSize = sizeof(*info) + lstrlenW(filePath) * sizeof(WCHAR);

    if (needed)
        *needed = requiredSize;

    if (size < requiredSize)
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        free(filePath);
        return FALSE;
    }

    if (!GetFileAttributesExW(filePath, GetFileExInfoStandard, &data))
    {
        free(filePath);
        return FALSE;
    }

    info->writetime = data.ftLastWriteTime;
    info->size.LowPart  = data.nFileSizeLow;
    info->size.HighPart = data.nFileSizeHigh;
    lstrcpyW(info->path, filePath);

    free(filePath);
    return TRUE;
}

static DWORD get_font_data( GdiFont *font, DWORD table, DWORD offset, LPVOID buf, DWORD cbData)
{
    FT_Face ft_face = font->ft_face;
    FT_ULong len;
    FT_Error err;

    if (!FT_IS_SFNT(ft_face)) return GDI_ERROR;

    if(!buf)
        len = 0;
    else
        len = cbData;

    /* if font is a member of TTC, 'ttcf' tag allows reading from beginning of TTC file,
       0 tag means to read from start of collection member data. */
    if (font->ttc_item_offset)
    {
        if (table == MS_TTCF_TAG)
            table = 0;
        else if (table == 0)
            offset += font->ttc_item_offset;
    }

    table = RtlUlongByteSwap( table );  /* MS tags differ in endianness from FT ones */

    /* make sure value of len is the value freetype says it needs */
    if (buf && len)
    {
        FT_ULong needed = 0;
        err = FT_Load_Sfnt_Table(ft_face, table, offset, NULL, &needed);
        if( !err && needed < len) len = needed;
    }
    err = FT_Load_Sfnt_Table(ft_face, table, offset, (FT_Byte*)buf, &len);
    if (err)
    {
        table = RtlUlongByteSwap( table );
        TRACE("Can't find table %s\n", debugstr_an((char*)&table, 4));
        return GDI_ERROR;
    }
    return len;
}

/*************************************************************************
 *             GetFontFileData   (GDI32.@)
 */
BOOL WINAPI GetFontFileData( DWORD instance_id, DWORD file_index, UINT64 offset, void *buff, DWORD buff_size )
{
    struct font_handle_entry *entry;
    GdiFont *font;
    DWORD tag = 0;
    DWORD size;
    //DWORD ret;

    entry = handle_entry( instance_id );
    if (!entry)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    font = (GdiFont *)entry->obj;

    if (font->ttc_item_offset)
        tag = MS_TTCF_TAG;

    /* getting size of font file */
    size = get_font_data(font, tag, 0, NULL, 0);
    if (size == GDI_ERROR)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    /* check boundaries
    if (offset > (UINT64)size ||
        (UINT64)buff_size > (UINT64)size - offset)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    ret = get_font_data(font, tag, (DWORD)offset, buff, buff_size);
*/
    return (size != GDI_ERROR);
}