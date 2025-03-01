/*++

Copyright (c) 2021  Shorthorn Project

Module Name:

    hooks.c

Abstract:

    Hooks native functions to implement new features
	
Author:

    Skulltrail 05-December-2021

Revision History:

--*/

#include "main.h"

#include <pshpack1.h>
typedef struct {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    union
    {
        WORD wPlanes; /* For icons */
        WORD xHotspot; /* For cursors */
    };
    union
    {
        WORD wBitCount; /* For icons */
        WORD yHotspot; /* For cursors */
    };
    DWORD dwDIBSize;
    DWORD dwDIBOffset;
} CURSORICONFILEDIRENTRY;

typedef struct
{
    WORD idReserved;
    WORD idType;
    WORD idCount;
    CURSORICONFILEDIRENTRY idEntries[1];
} CURSORICONFILEDIR;
#include <poppack.h>

#define PNG_CHECK_SIG_SIZE 8 /* Check signature size */

/* libpng helpers */
typedef struct {
    png_bytep buffer;
    size_t bufSize;
    size_t currentPos;
} PNG_READER_STATE;

/* This function will be used for reading png data from memory */
static VOID
ReadMemoryPng(
    _Inout_ png_structp png_ptr,
    _Out_ png_bytep data,
    _In_ size_t length)
{
    PNG_READER_STATE *state = png_get_io_ptr(png_ptr);
    if ((state->currentPos + length) > state->bufSize)
    {
        ERR("png read error\n");
        png_error(png_ptr, "read error in ReadMemoryPng");
        return;
    }
    RtlCopyMemory(data, state->buffer + state->currentPos, length);
    state->currentPos += length;
}

static int get_dib_image_size(int width, int height, int depth);

/* Convert PNG raw data to BMP icon data */
static LPBYTE
CURSORICON_ConvertPngToBmpIcon(
    _In_ LPBYTE pngBits,
    _In_ DWORD fileSize,
    _Out_ PDWORD pBmpIconSize)
{
    if (!pngBits || fileSize < PNG_CHECK_SIG_SIZE || !png_check_sig(pngBits, PNG_CHECK_SIG_SIZE))
        return NULL;

    TRACE("pngBits %p fileSize %d\n", pngBits, fileSize);

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        ERR("png_create_read_struct error\n");
        return NULL;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        ERR("png_create_info_struct error\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    /* Set our own read_function */
    PNG_READER_STATE readerState = { pngBits, fileSize, PNG_CHECK_SIG_SIZE };
    png_set_read_fn(png_ptr, &readerState, ReadMemoryPng);
    png_set_sig_bytes(png_ptr, PNG_CHECK_SIG_SIZE);

    /* Read png info */
    png_read_info(png_ptr, info_ptr);

    /* Add translation of some PNG formats and update info */
    int colorType = png_get_color_type(png_ptr, info_ptr);
    if (colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    else if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);
    png_set_scale_16(png_ptr); /* Convert 16-bit channels to 8-bit */
    png_read_update_info(png_ptr, info_ptr);

    /* Get updated png info */
    png_uint_32 width, height;
    int bitDepth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);
    TRACE("width %d, height %d, bitDepth %d, colorType %d\n",
          width, height, bitDepth, colorType);

    int channels = png_get_channels(png_ptr, info_ptr);
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    int imageSize = height * rowbytes;
    TRACE("rowbytes %d, channels %d, imageSize %d\n", rowbytes, channels, imageSize);

    /* Allocate rows data */
    png_bytepp rows = png_malloc(png_ptr, sizeof(png_bytep) * height);
    if (!rows)
    {
        ERR("png_malloc failed\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    for (int i = 0; i < (int)height; i++)
    {
        rows[i] = png_malloc(png_ptr, rowbytes);
        if (!rows[i])
        {
            ERR("png_malloc failed\n");

            /* Clean up */
            while (--i >= 0)
                png_free(png_ptr, rows[i]);
            png_free(png_ptr, rows);
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

            return NULL;
        }
    }

    /* Read png image data */
    png_set_rows(png_ptr, info_ptr, rows);
    png_read_image(png_ptr, rows);
    png_read_end(png_ptr, info_ptr);

    /* After reading the image, you can deal with row pointers */
    LPBYTE imageBytes = HeapAlloc(GetProcessHeap(), 0, imageSize);
    if (imageBytes)
    {
        LPBYTE pb = imageBytes;
        for (int i = height - 1; i >= 0; i--)
        {
            png_bytep row = rows[i];
            for (int j = 0; j < channels * width; j += channels)
            {
                *pb++ = row[j + 2]; /* Red */
                *pb++ = row[j + 1]; /* Green */
                *pb++ = row[j + 0]; /* Blue */
                if (channels == 4)
                    *pb++ = row[j + 3]; /* Alpha */
            }
            pb += (channels * width) % 4;
        }
    }

    /* Clean up */
    for (int i = 0; i < (int)height; i++)
        png_free(png_ptr, rows[i]);
    png_free(png_ptr, rows);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    if (!imageBytes)
    {
        ERR("HeapAlloc failed\n");
        return NULL;
    }

    /* BPP (Bits Per Pixel) */
    WORD bpp = (WORD)(bitDepth * channels);

    /* The byte size of mask bits */
    DWORD maskSize = get_dib_image_size(width, height, 1);

    /* Build BITMAPINFOHEADER */
    BITMAPINFOHEADER info = { sizeof(info) };
    info.biWidth = width;
    info.biHeight = 2 * height;
    info.biPlanes = 1;
    info.biBitCount = bpp;
    info.biCompression = BI_RGB;

    /* Build CURSORICONFILEDIR */
    CURSORICONFILEDIR cifd = { 0, 1, 1 };
    cifd.idEntries[0].bWidth = (BYTE)width;
    cifd.idEntries[0].bHeight = (BYTE)height;
    cifd.idEntries[0].bColorCount = 0; /* No color pallete */
    cifd.idEntries[0].wPlanes = 1; /* Must be 1 */
    cifd.idEntries[0].wBitCount = bpp;
    cifd.idEntries[0].dwDIBSize = (DWORD)(sizeof(info) + imageSize + maskSize);
    cifd.idEntries[0].dwDIBOffset = (DWORD)sizeof(cifd);

    /* Allocate BMP icon data */
    *pBmpIconSize = (DWORD)(sizeof(cifd) + sizeof(info) + imageSize + maskSize);
    LPBYTE pbBmpIcon = HeapAlloc(GetProcessHeap(), 0, *pBmpIconSize);
    if (!pbBmpIcon)
    {
        ERR("HeapAlloc failed\n");
        HeapFree(GetProcessHeap(), 0, imageBytes);
        return NULL;
    }

    /* Store data to pbBmpIcon */
    LPBYTE pb = pbBmpIcon;
    RtlCopyMemory(pb, &cifd, sizeof(cifd));
    pb += sizeof(cifd);
    RtlCopyMemory(pb, &info, sizeof(info));
    pb += sizeof(info);
    RtlCopyMemory(pb, imageBytes, imageSize);
    pb += imageSize;
    RtlFillMemory(pb, maskSize, 0xFF); /* Mask bits for AND operation */

    HeapFree(GetProcessHeap(), 0, imageBytes);
    return pbBmpIcon;
}

static int DIB_GetBitmapInfo( const BITMAPINFOHEADER *header, LONG *width,
                              LONG *height, WORD *bpp, DWORD *compr )
{
    #define CR 13
    #define LF 10
    #define EOFM 26 // DOS End Of File Marker
    #define HighBitDetect 0x89 // Byte with high bit set to test if not 7-bit
    /* wine's definition */
    static const BYTE png_sig_pattern[] = { HighBitDetect, 'P', 'N', 'G', CR, LF, EOFM, LF };
    if (header->biSize == sizeof(BITMAPCOREHEADER))
    {
        const BITMAPCOREHEADER *core = (const BITMAPCOREHEADER *)header;
        *width  = core->bcWidth;
        *height = core->bcHeight;
        *bpp    = core->bcBitCount;
        *compr  = 0;
        return 0;
    }
    else if (header->biSize == sizeof(BITMAPINFOHEADER) ||
             header->biSize == sizeof(BITMAPV4HEADER) ||
             header->biSize == sizeof(BITMAPV5HEADER))
    {
        *width  = header->biWidth;
        *height = header->biHeight;
        *bpp    = header->biBitCount;
        *compr  = header->biCompression;
        return 1;
    }
    if (memcmp(&header->biSize, png_sig_pattern, sizeof(png_sig_pattern)) == 0)
    {
        TRACE("We have a PNG icon\n");
        /* for PNG format details see https://en.wikipedia.org/wiki/PNG */
    }
    else
    {
        ERR("Unknown/wrong size for header of 0x%x\n", header->biSize );
    }
    return -1;
}

VOID
WINAPI
UserSetLastError(IN DWORD dwErrCode)
{
    /*
     * Equivalent of SetLastError in kernel32, but without breaking
     * into the debugger nor checking whether the last old error is
     * the same as the one we are going to set.
     */
    NtCurrentTeb()->LastErrorValue = dwErrCode;
}

VOID
WINAPI
UserSetLastNTError(IN NTSTATUS Status)
{
    /*
     * Equivalent of BaseSetLastNTError in kernel32, but using
     * UserSetLastError: convert from NT to Win32, then set.
     */
    UserSetLastError(RtlNtStatusToDosError(Status));
}

// BOOL 
// WINAPI
// SystemParametersInfoWInternal(
	// UINT uiAction,
	// UINT uiParam,
	// PVOID pvParam,
	// UINT fWinIni)
// {
	// BOOL res;
	// PBOOL realParam;
	// // HACK: Qt6.6.1 after WinRT classes defined crashes due to NONCLIENTMETRICS being on NT6 size, so convert to NT5
	// if ((uiAction == SPI_SETNONCLIENTMETRICS || uiAction == SPI_GETNONCLIENTMETRICS) && ((LPNONCLIENTMETRICSW)pvParam)->cbSize == sizeof(NONCLIENTMETRICSW) + 4) {
		// // Set size
		// ((LPNONCLIENTMETRICSW)pvParam)->cbSize -= 4;
		// res = SystemParametersInfoW(uiAction, sizeof(NONCLIENTMETRICSW), pvParam, fWinIni);
		// ((LPNONCLIENTMETRICSW)pvParam)->cbSize += 4;
		// if (res) {
			 // ((LPNONCLIENTMETRICSW_VISTA)pvParam)->iPaddedBorderWidth = 0;
		// }
		// return res;
	// }	
	// switch(uiAction)
    // {
      // case SPI_GETNONCLIENTMETRICS:
      // {
          // LPNONCLIENTMETRICSW lpnclt = (LPNONCLIENTMETRICSW)pvParam;
		  // lpnclt->cbSize = sizeof(NONCLIENTMETRICSW);
          // return SystemParametersInfoW(uiAction, lpnclt->cbSize, lpnclt, fWinIni);          
      // }
	  // case SPI_SETNONCLIENTMETRICS:
	  // {
          // LPNONCLIENTMETRICSW lpnclt = (LPNONCLIENTMETRICSW)pvParam;
		  // lpnclt->cbSize = sizeof(NONCLIENTMETRICSW);
          // return SystemParametersInfoW(uiAction, lpnclt->cbSize, lpnclt, fWinIni);  
	  // }
	  // case SPI_GETCLIENTAREAANIMATION: // Visual Studio 2012 WPF Designer crashes without this case
		// realParam = pvParam;
		// *realParam = TRUE;
		// return TRUE;  
	  // default:
		// return SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
	// }
	// return SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
// }

BOOL 
WINAPI
SystemParametersInfoWInternal(
	UINT uiAction,
	UINT uiParam,
	PVOID pvParam,
	UINT fWinIni)
{
	BOOL res;
	
	// HACK: Qt6.6.1 after WinRT classes defined crashes due to NONCLIENTMETRICS being on NT6 size, so convert to NT5
	if ((uiAction == SPI_SETNONCLIENTMETRICS || uiAction == SPI_GETNONCLIENTMETRICS) && pvParam && ((LPNONCLIENTMETRICSW)pvParam)->cbSize == sizeof(NONCLIENTMETRICSW) + 4) {
		// Set size
		((LPNONCLIENTMETRICSW)pvParam)->cbSize -= 4;
		res = SystemParametersInfoW(uiAction, sizeof(NONCLIENTMETRICSW), pvParam, fWinIni);
		((LPNONCLIENTMETRICSW)pvParam)->cbSize += 4;
		if (res) {
			 ((LPNONCLIENTMETRICSW_VISTA)pvParam)->iPaddedBorderWidth = 0;
		}
		return res;
	}	
	switch(uiAction)
    {
	  case SPI_GETCLIENTAREAANIMATION:
		/*
			In Visual Studio 2012, the WPF designer crashes without having this. Client area animations simply do not exist
			before Windows Vista, so return TRUE.
			
			TODO: If someone installs the Longhorn 5048 Win32ss and enables DWM, then client area animations actually become
			meaningful. This is why we return TRUE
		*/
		if (!pvParam) {
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
		*(PBOOL)pvParam = TRUE;
		return TRUE;
	  case SPI_GETDISABLEOVERLAPPEDCONTENT:
	  case SPI_GETSYSTEMLANGUAGEBAR:
	  case SPI_GETTHREADLOCALINPUTSETTINGS:
	  case SPI_GETSCREENSAVESECURE: // This exists internally in Server 2003 SP2...
	  case SPI_GETDOCKMOVING:
	  case SPI_GETSNAPSIZING:
	  case SPI_GETWINARRANGING:
		if (!pvParam) {
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
		*(PBOOL)pvParam = FALSE;
		return TRUE;
	  case SPI_GETMESSAGEDURATION:
		if (!pvParam) {
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
		*(PULONG)pvParam = 5;
		return TRUE;
	  case SPI_SETMOUSEWHEELROUTING: // Support Windows 10 mouse wheel options.
		SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
		return TRUE;
	  case SPI_SETCLIENTAREAANIMATION:
	  case SPI_SETMESSAGEDURATION:
	  case SPI_SETDISABLEOVERLAPPEDCONTENT:
	  case SPI_SETSYSTEMLANGUAGEBAR:
	  case SPI_SETTHREADLOCALINPUTSETTINGS:
	  case SPI_SETDOCKMOVING:
	  case SPI_SETSNAPSIZING:
	  case SPI_SETWINARRANGING:
		return TRUE;
	  default:
		return SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
	}
}

BOOL 
WINAPI
SystemParametersInfoAInternal(
	UINT uiAction,
	UINT uiParam,
	PVOID pvParam,
	UINT fWinIni)
{
	BOOL res;
	// HACK: Qt6.6.1 after WinRT classes defined crashes due to NONCLIENTMETRICS being on NT6 size, so convert to NT5
	if ((uiAction == SPI_SETNONCLIENTMETRICS || uiAction == SPI_GETNONCLIENTMETRICS) && ((LPNONCLIENTMETRICSA)pvParam)->cbSize == sizeof(NONCLIENTMETRICSA) + 4) {
		// Set size
		((LPNONCLIENTMETRICSA)pvParam)->cbSize -= 4;
		res = SystemParametersInfoW(uiAction, sizeof(NONCLIENTMETRICSA), pvParam, fWinIni);
		((LPNONCLIENTMETRICSA)pvParam)->cbSize += 4;
		if (res) {
			 ((LPNONCLIENTMETRICSA_VISTA)pvParam)->iPaddedBorderWidth = 0;
		}
		return res;
	}	
    switch(uiAction)
    {
       case SPI_GETNONCLIENTMETRICS:
      {
          LPNONCLIENTMETRICSA lpnclt = (LPNONCLIENTMETRICSA)pvParam;	  
		  lpnclt->cbSize = sizeof(NONCLIENTMETRICSA);       
		  return SystemParametersInfoA(uiAction, lpnclt->cbSize, lpnclt, fWinIni);          
      }
	  case SPI_SETNONCLIENTMETRICS:
	  {
          LPNONCLIENTMETRICSA lpnclt = (LPNONCLIENTMETRICSA)pvParam;	  
		  lpnclt->cbSize = sizeof(NONCLIENTMETRICSA);       
		  return SystemParametersInfoA(uiAction, lpnclt->cbSize, lpnclt, fWinIni); 		  
	  }
	  default:
		return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
  }
  return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni); 			   
}	

/*
 * GetUserObjectSecurity
 *
 * Retrieves security information for user object specified
 * with handle 'hObject'. Descriptor returned in self-relative
 * format.
 *
 * Arguments:
 *  1) hObject - handle to an object to retrieve information for
 *  2) pSecurityInfo - type of information to retrieve
 *  3) pSecurityDescriptor - buffer which receives descriptor
 *  4) dwLength - size, in bytes, of buffer 'pSecurityDescriptor'
 *  5) pdwLengthNeeded - reseives actual size of descriptor
 *
 * Return Vaules:
 *  TRUE on success
 *  FALSE on failure, call GetLastError() for more information
 */
/*
 * @implemented
 */
BOOL
WINAPI
GetUserObjectSecurityInternal(
    IN HANDLE hObject,
    IN PSECURITY_INFORMATION pSecurityInfo,
    OUT PSECURITY_DESCRIPTOR pSecurityDescriptor,
    IN DWORD dwLength,
    OUT PDWORD pdwLengthNeeded
)
{
    NTSTATUS Status;

    Status = NtQuerySecurityObject(hObject,
                              

							  *pSecurityInfo,
                                   pSecurityDescriptor,
                                   dwLength,
                                   pdwLengthNeeded);
    if (!NT_SUCCESS(Status))
    {
		DbgPrint("GetUserObjectSecurity::NtQuerySecurityObject returned Status: 0x%08lx\n", Status);
        UserSetLastNTError(Status);
        return FALSE;
    }

    return TRUE;
}

HANDLE WINAPI LoadImageW(
  _In_opt_  HINSTANCE hinst,
  _In_      LPCWSTR lpszName,
  _In_      UINT uType,
  _In_      int cxDesired,
  _In_      int cyDesired,
  _In_      UINT fuLoad
)
{
    TRACE("hinst 0x%p, name %s, uType 0x%08x, cxDesired %d, cyDesired %d, fuLoad 0x%08x.\n",
        hinst, debugstr_w(lpszName), uType, cxDesired, cyDesired, fuLoad);
    /* Redirect to each implementation */
	if(uType == IMAGE_ICON || uType == IMAGE_CURSOR)
	{
		 return CURSORICON_LoadImageW(hinst, lpszName, cxDesired, cyDesired, fuLoad, uType == IMAGE_ICON);
	}
	
    return LoadImageW(hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);
}