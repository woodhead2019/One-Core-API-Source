#include <wine/config.h>

#include <ntstatus.h>
#define WIN32_NO_STATUS

#include <wine/debug.h>

#define WIN32_NO_STATUS
#define _INC_WINDOWS
#define COM_NO_WINDOWS_H
#include <stdarg.h>
#include <windef.h>
#include <winbase.h>
#include <winreg.h>
#include <wingdi.h>
#include <winnls.h>
#include <winuser.h>
#include <setupapi.h>
#include <ddraw.h>
#include <initguid.h>
#include <devguid.h>
#include <strsafe.h>
#include <ntsecapi.h>
#include <bcrypt.h>
#include <math.h>
#include <d3dkmthk.h>
#include <limits.h>
#include <rtlfuncs.h>
#include <list.h>
#include "wine/heap.h"
#include <devpropdef.h>
#include <unicode.h>
#include <strsafe.h>
#include <ddrawint.h>
#include <d3dhal.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// Purpose: Video present source unique identification number descriptor type
//

typedef UINT D3DDDI_VIDEO_PRESENT_SOURCE_ID;

//typedef UINT D3DKMT_HANDLE;

typedef struct _D3DKMT_CHECKMONITORPOWERSTATE {
  D3DKMT_HANDLE                  hAdapter;
  D3DDDI_VIDEO_PRESENT_SOURCE_ID VidPnSourceId;
} D3DKMT_CHECKMONITORPOWERSTATE;

typedef enum _D3DKMT_SCHEDULINGPRIORITYCLASS {
  D3DKMT_SCHEDULINGPRIORITYCLASS_IDLE,
  D3DKMT_SCHEDULINGPRIORITYCLASS_BELOW_NORMAL,
  D3DKMT_SCHEDULINGPRIORITYCLASS_NORMAL,
  D3DKMT_SCHEDULINGPRIORITYCLASS_ABOVE_NORMAL,
  D3DKMT_SCHEDULINGPRIORITYCLASS_HIGH,
  D3DKMT_SCHEDULINGPRIORITYCLASS_REALTIME
} D3DKMT_SCHEDULINGPRIORITYCLASS;

typedef struct _D3DDDI_CREATECONTEXTFLAGS {
	union {
		struct {
			UINT NullRendering : 1;
			UINT InitialData : 1;
			UINT Reserved : 30;
		};
		UINT   Value;
	};
} D3DDDI_CREATECONTEXTFLAGS;

typedef enum _D3DKMT_CLIENTHINT
{
	D3DKMT_CLIENTHINT_UNKNOWN = 0,
	D3DKMT_CLIENTHINT_OPENGL = 1,
	D3DKMT_CLIENTHINT_CDD = 2,       // Internal   ;internal
	D3DKMT_CLIENTHINT_DX7 = 7,
	D3DKMT_CLIENTHINT_DX8 = 8,
	D3DKMT_CLIENTHINT_DX9 = 9,
	D3DKMT_CLIENTHINT_DX10 = 10,
} D3DKMT_CLIENTHINT;

typedef struct _D3DKMT_CREATECONTEXT {
  D3DKMT_HANDLE             hDevice;
  UINT                      NodeOrdinal;
  UINT                      EngineAffinity;
  D3DDDI_CREATECONTEXTFLAGS Flags;
  VOID                      *pPrivateDriverData;
  UINT                      PrivateDriverDataSize;
  D3DKMT_CLIENTHINT         ClientHint;
  D3DKMT_HANDLE             hContext;
  VOID                      *pCommandBuffer;
  UINT                      CommandBufferSize;
  D3DDDI_ALLOCATIONLIST     *pAllocationList;
  UINT                      AllocationListSize;
  D3DDDI_PATCHLOCATIONLIST  *pPatchLocationList;
  UINT                      PatchLocationListSize;
  VOID                      *CommandBuffer;
} D3DKMT_CREATECONTEXT;

typedef struct _D3DKMT_SHAREDPRIMARYLOCKNOTIFICATION {
  LUID                           AdapterLuid;
  D3DDDI_VIDEO_PRESENT_SOURCE_ID VidPnSourceId;
  RECTL                          LockRect;
} D3DKMT_SHAREDPRIMARYLOCKNOTIFICATION;

typedef struct _D3DKMT_SHAREDPRIMARYUNLOCKNOTIFICATION {
  LUID                           AdapterLuid;
  D3DDDI_VIDEO_PRESENT_SOURCE_ID VidPnSourceId;
} D3DKMT_SHAREDPRIMARYUNLOCKNOTIFICATION;

typedef struct _D3DKMT_GETSCANLINE {
    HANDLE hAdapter;       // Handle do adaptador gráfico
    HANDLE hDevice;        // Handle do dispositivo gráfico
    BOOL InVerticalBlank;  // Indica se está no intervalo de blanking vertical
    UINT ScanLine;         // A posição atual do scanline
} D3DKMT_GETSCANLINE;

typedef struct _D3DNTHAL_CONTEXTCREATEDATA {
  union {
    PDD_DIRECTDRAW_GLOBAL lpDDGbl;
    PDD_DIRECTDRAW_LOCAL lpDDLcl;
  };
  union {
    PDD_SURFACE_LOCAL lpDDS;
    PDD_SURFACE_LOCAL lpDDSLcl;
  };
  union {
    PDD_SURFACE_LOCAL lpDDSZ;
    PDD_SURFACE_LOCAL lpDDSZLcl;
  };
  DWORD dwPID;
  ULONG_PTR dwhContext;
  HRESULT ddrval;
} D3DNTHAL_CONTEXTCREATEDATA, *LPD3DNTHAL_CONTEXTCREATEDATA;

// typedef struct _DDHAL_GETSCANLINEDATA {
  // LPDDRAWI_DIRECTDRAW_GBL lpDD;
  // DWORD dwScanLine;
  // HRESULT ddRVal;
  // LPDDHAL_GETSCANLINE GetScanLine;
// } DDHAL_GETSCANLINEDATA;

struct bitblt_coords
{
    int  log_x;     /* original position and size, in logical coords */
    int  log_y;
    int  log_width;
    int  log_height;
    int  x;         /* mapped position and size, in device coords */
    int  y;
    int  width;
    int  height;
    RECT visrect;   /* rectangle clipped to the visible part, in device coords */
    DWORD layout;   /* DC layout */
};

typedef struct gdi_physdev
{
     const struct gdi_dc_funcs *funcs;
     struct gdi_physdev        *next;
     HDC                        hdc;
} *PHYSDEV;

/* Undocumented structure filled in by GetFontRealizationInfo */
struct font_realization_info
{
    DWORD size;        /* could be 16 or 24 */
    DWORD flags;       /* 1 for bitmap fonts, 3 for scalable fonts */
    DWORD cache_num;   /* keeps incrementing - num of fonts that have been created allowing for caching?? */
    DWORD instance_id; /* identifies a realized font instance */
    DWORD file_count;  /* file font count */
    WORD  face_index;  /* face index in case of font collections */
    WORD  simulations; /* 0 bit - bold simulation, 1 bit - oblique simulation */
};

typedef struct tagDC
{
    HDC          hSelf;            /* Handle to this DC */
    struct gdi_physdev nulldrv;    /* physdev for the null driver */
    PHYSDEV      physDev;          /* current top of the physdev stack */
    DWORD        thread;           /* thread owning the DC */
    LONG         refcount;         /* thread refcount */
    LONG         dirty;            /* dirty flag */
    LONG         disabled;         /* get_dc_ptr() will return NULL.  Controlled by DCHF_(DISABLE|ENABLE)DC */
    INT          saveLevel;
    struct tagDC *saved_dc;
    DWORD_PTR    dwHookData;
    DCHOOKPROC   hookProc;         /* DC hook */
    BOOL         bounds_enabled:1; /* bounds tracking is enabled */
    BOOL         path_open:1;      /* path is currently open (only for saved DCs) */

    POINT        wnd_org;          /* Window origin */
    SIZE         wnd_ext;          /* Window extent */
    POINT        vport_org;        /* Viewport origin */
    SIZE         vport_ext;        /* Viewport extent */
    SIZE         virtual_res;      /* Initially HORZRES,VERTRES. Changed by SetVirtualResolution */
    SIZE         virtual_size;     /* Initially HORZSIZE,VERTSIZE. Changed by SetVirtualResolution */
    RECT         vis_rect;         /* visible rectangle in screen coords */
    RECT         device_rect;      /* rectangle for the whole device */
    int          pixel_format;     /* pixel format (for memory DCs) */
    UINT         aa_flags;         /* anti-aliasing flags to pass to GetGlyphOutline for current font */
    FLOAT        miterLimit;

    int           flags;
    DWORD         layout;
    HRGN          hClipRgn;      /* Clip region */
    HRGN          hMetaRgn;      /* Meta region */
    HRGN          hVisRgn;       /* Visible region */
    HRGN          region;        /* Total DC region (intersection of clip and visible) */
    HPEN          hPen;
    HBRUSH        hBrush;
    HFONT         hFont;
    HBITMAP       hBitmap;
    HPALETTE      hPalette;

    struct gdi_path *path;

    UINT          font_code_page;
    WORD          ROPmode;
    WORD          polyFillMode;
    WORD          stretchBltMode;
    WORD          relAbsMode;
    WORD          backgroundMode;
    COLORREF      backgroundColor;
    COLORREF      textColor;
    COLORREF      dcBrushColor;
    COLORREF      dcPenColor;
    POINT         brush_org;

    DWORD         mapperFlags;       /* Font mapper flags */
    WORD          textAlign;         /* Text alignment from SetTextAlign() */
    INT           charExtra;         /* Spacing from SetTextCharacterExtra() */
    INT           breakExtra;        /* breakTotalExtra / breakCount */
    INT           breakRem;          /* breakTotalExtra % breakCount */
    INT           MapMode;
    INT           GraphicsMode;      /* Graphics mode */
    ABORTPROC     pAbortProc;        /* AbortProc for Printing */
    POINT         cur_pos;           /* Current position */
    INT           ArcDirection;
    XFORM         xformWorld2Wnd;    /* World-to-window transformation */
    XFORM         xformWorld2Vport;  /* World-to-viewport transformation */
    XFORM         xformVport2World;  /* Inverse of the above transformation */
    BOOL          vport2WorldValid;  /* Is xformVport2World valid? */
    RECT          bounds;            /* Current bounding rect */
} DC;

struct gdi_handle_entry
{
    void                       *obj;         /* pointer to the object-specific data */
    const struct gdi_obj_funcs *funcs;       /* type-specific functions */
    struct hdc_list            *hdcs;        /* list of HDCs interested in this object */
    WORD                        generation;  /* generation count for reusing handle values */
    WORD                        type;        /* object type (one of the OBJ_* constants) */
    WORD                        selcount;    /* number of times the object is selected in a DC */
    WORD                        system : 1;  /* system object flag */
    WORD                        deleted : 1; /* whether DeleteObject has been called on this object */
};

static inline PHYSDEV get_physdev_entry_point( PHYSDEV dev, size_t offset )
{
     while (!((void **)dev->funcs)[offset / sizeof(void *)]) dev = dev->next;
     return dev;
}

#define GET_DC_PHYSDEV(dc,func) \
    get_physdev_entry_point( (dc)->physDev, FIELD_OFFSET(struct gdi_dc_funcs,func))
	
#define D3DKMT_QUERYSTATISTICS_SEGMENT_PREFERENCE_MAX 5

struct gdi_dc_funcs
{
    INT      (*pAbortDoc)(PHYSDEV);
    BOOL     (*pAbortPath)(PHYSDEV);
    BOOL     (*pAlphaBlend)(PHYSDEV,struct bitblt_coords*,PHYSDEV,struct bitblt_coords*,BLENDFUNCTION);
    BOOL     (*pAngleArc)(PHYSDEV,INT,INT,DWORD,FLOAT,FLOAT);
    BOOL     (*pArc)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pArcTo)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pBeginPath)(PHYSDEV);
    DWORD    (*pBlendImage)(PHYSDEV,BITMAPINFO*,const struct gdi_image_bits*,struct bitblt_coords*,struct bitblt_coords*,BLENDFUNCTION);
    BOOL     (*pChord)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pCloseFigure)(PHYSDEV);
    BOOL     (*pCreateCompatibleDC)(PHYSDEV,PHYSDEV*);
    BOOL     (*pCreateDC)(PHYSDEV*,LPCWSTR,LPCWSTR,LPCWSTR,const DEVMODEW*);
    BOOL     (*pDeleteDC)(PHYSDEV);
    BOOL     (*pDeleteObject)(PHYSDEV,HGDIOBJ);
    DWORD    (*pDeviceCapabilities)(LPSTR,LPCSTR,LPCSTR,WORD,LPSTR,LPDEVMODEA);
    BOOL     (*pEllipse)(PHYSDEV,INT,INT,INT,INT);
    INT      (*pEndDoc)(PHYSDEV);
    INT      (*pEndPage)(PHYSDEV);
    BOOL     (*pEndPath)(PHYSDEV);
    BOOL     (*pEnumFonts)(PHYSDEV,LPLOGFONTW,FONTENUMPROCW,LPARAM);
    INT      (*pEnumICMProfiles)(PHYSDEV,ICMENUMPROCW,LPARAM);
    INT      (*pExcludeClipRect)(PHYSDEV,INT,INT,INT,INT);
    INT      (*pExtDeviceMode)(LPSTR,HWND,LPDEVMODEA,LPSTR,LPSTR,LPDEVMODEA,LPSTR,DWORD);
    INT      (*pExtEscape)(PHYSDEV,INT,INT,LPCVOID,INT,LPVOID);
    BOOL     (*pExtFloodFill)(PHYSDEV,INT,INT,COLORREF,UINT);
    INT      (*pExtSelectClipRgn)(PHYSDEV,HRGN,INT);
    BOOL     (*pExtTextOut)(PHYSDEV,INT,INT,UINT,const RECT*,LPCWSTR,UINT,const INT*);
    BOOL     (*pFillPath)(PHYSDEV);
    BOOL     (*pFillRgn)(PHYSDEV,HRGN,HBRUSH);
    BOOL     (*pFlattenPath)(PHYSDEV);
    BOOL     (*pFontIsLinked)(PHYSDEV);
    BOOL     (*pFrameRgn)(PHYSDEV,HRGN,HBRUSH,INT,INT);
    BOOL     (*pGdiComment)(PHYSDEV,UINT,const BYTE*);
    UINT     (*pGetBoundsRect)(PHYSDEV,RECT*,UINT);
    BOOL     (*pGetCharABCWidths)(PHYSDEV,UINT,UINT,LPABC);
    BOOL     (*pGetCharABCWidthsI)(PHYSDEV,UINT,UINT,WORD*,LPABC);
    BOOL     (*pGetCharWidth)(PHYSDEV,UINT,UINT,LPINT);
    INT      (*pGetDeviceCaps)(PHYSDEV,INT);
    BOOL     (*pGetDeviceGammaRamp)(PHYSDEV,LPVOID);
    DWORD    (*pGetFontData)(PHYSDEV,DWORD,DWORD,LPVOID,DWORD);
    BOOL     (*pGetFontRealizationInfo)(PHYSDEV,void*);
    DWORD    (*pGetFontUnicodeRanges)(PHYSDEV,LPGLYPHSET);
    DWORD    (*pGetGlyphIndices)(PHYSDEV,LPCWSTR,INT,LPWORD,DWORD);
    DWORD    (*pGetGlyphOutline)(PHYSDEV,UINT,UINT,LPGLYPHMETRICS,DWORD,LPVOID,const MAT2*);
    BOOL     (*pGetICMProfile)(PHYSDEV,LPDWORD,LPWSTR);
    DWORD    (*pGetImage)(PHYSDEV,BITMAPINFO*,struct gdi_image_bits*,struct bitblt_coords*);
    DWORD    (*pGetKerningPairs)(PHYSDEV,DWORD,LPKERNINGPAIR);
    COLORREF (*pGetNearestColor)(PHYSDEV,COLORREF);
    UINT     (*pGetOutlineTextMetrics)(PHYSDEV,UINT,LPOUTLINETEXTMETRICW);
    COLORREF (*pGetPixel)(PHYSDEV,INT,INT);
    UINT     (*pGetSystemPaletteEntries)(PHYSDEV,UINT,UINT,LPPALETTEENTRY);
    UINT     (*pGetTextCharsetInfo)(PHYSDEV,LPFONTSIGNATURE,DWORD);
    BOOL     (*pGetTextExtentExPoint)(PHYSDEV,LPCWSTR,INT,LPINT);
    BOOL     (*pGetTextExtentExPointI)(PHYSDEV,const WORD*,INT,LPINT);
    INT      (*pGetTextFace)(PHYSDEV,INT,LPWSTR);
    BOOL     (*pGetTextMetrics)(PHYSDEV,TEXTMETRICW*);
    BOOL     (*pGradientFill)(PHYSDEV,TRIVERTEX*,ULONG,void*,ULONG,ULONG);
    INT      (*pIntersectClipRect)(PHYSDEV,INT,INT,INT,INT);
    BOOL     (*pInvertRgn)(PHYSDEV,HRGN);
    BOOL     (*pLineTo)(PHYSDEV,INT,INT);
    BOOL     (*pModifyWorldTransform)(PHYSDEV,const XFORM*,DWORD);
    BOOL     (*pMoveTo)(PHYSDEV,INT,INT);
    INT      (*pOffsetClipRgn)(PHYSDEV,INT,INT);
    BOOL     (*pOffsetViewportOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pOffsetWindowOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pPaintRgn)(PHYSDEV,HRGN);
    BOOL     (*pPatBlt)(PHYSDEV,struct bitblt_coords*,DWORD);
    BOOL     (*pPie)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT);
    BOOL     (*pPolyBezier)(PHYSDEV,const POINT*,DWORD);
    BOOL     (*pPolyBezierTo)(PHYSDEV,const POINT*,DWORD);
    BOOL     (*pPolyDraw)(PHYSDEV,const POINT*,const BYTE *,DWORD);
    BOOL     (*pPolyPolygon)(PHYSDEV,const POINT*,const INT*,UINT);
    BOOL     (*pPolyPolyline)(PHYSDEV,const POINT*,const DWORD*,DWORD);
    BOOL     (*pPolygon)(PHYSDEV,const POINT*,INT);
    BOOL     (*pPolyline)(PHYSDEV,const POINT*,INT);
    BOOL     (*pPolylineTo)(PHYSDEV,const POINT*,INT);
    DWORD    (*pPutImage)(PHYSDEV,HRGN,BITMAPINFO*,const struct gdi_image_bits*,struct bitblt_coords*,struct bitblt_coords*,DWORD);
    UINT     (*pRealizeDefaultPalette)(PHYSDEV);
    UINT     (*pRealizePalette)(PHYSDEV,HPALETTE,BOOL);
    BOOL     (*pRectangle)(PHYSDEV,INT,INT,INT,INT);
    HDC      (*pResetDC)(PHYSDEV,const DEVMODEW*);
    BOOL     (*pRestoreDC)(PHYSDEV,INT);
    BOOL     (*pRoundRect)(PHYSDEV,INT,INT,INT,INT,INT,INT);
    INT      (*pSaveDC)(PHYSDEV);
    BOOL     (*pScaleViewportExtEx)(PHYSDEV,INT,INT,INT,INT,SIZE*);
    BOOL     (*pScaleWindowExtEx)(PHYSDEV,INT,INT,INT,INT,SIZE*);
    HBITMAP  (*pSelectBitmap)(PHYSDEV,HBITMAP);
    HBRUSH   (*pSelectBrush)(PHYSDEV,HBRUSH,const struct brush_pattern*);
    BOOL     (*pSelectClipPath)(PHYSDEV,INT);
    HFONT    (*pSelectFont)(PHYSDEV,HFONT,UINT*);
    HPALETTE (*pSelectPalette)(PHYSDEV,HPALETTE,BOOL);
    HPEN     (*pSelectPen)(PHYSDEV,HPEN,const struct brush_pattern*);
    INT      (*pSetArcDirection)(PHYSDEV,INT);
    COLORREF (*pSetBkColor)(PHYSDEV,COLORREF);
    INT      (*pSetBkMode)(PHYSDEV,INT);
    UINT     (*pSetBoundsRect)(PHYSDEV,RECT*,UINT);
    COLORREF (*pSetDCBrushColor)(PHYSDEV, COLORREF);
    COLORREF (*pSetDCPenColor)(PHYSDEV, COLORREF);
    INT      (*pSetDIBitsToDevice)(PHYSDEV,INT,INT,DWORD,DWORD,INT,INT,UINT,UINT,LPCVOID,BITMAPINFO*,UINT);
    VOID     (*pSetDeviceClipping)(PHYSDEV,HRGN);
    BOOL     (*pSetDeviceGammaRamp)(PHYSDEV,LPVOID);
    DWORD    (*pSetLayout)(PHYSDEV,DWORD);
    INT      (*pSetMapMode)(PHYSDEV,INT);
    DWORD    (*pSetMapperFlags)(PHYSDEV,DWORD);
    COLORREF (*pSetPixel)(PHYSDEV,INT,INT,COLORREF);
    INT      (*pSetPolyFillMode)(PHYSDEV,INT);
    INT      (*pSetROP2)(PHYSDEV,INT);
    INT      (*pSetRelAbs)(PHYSDEV,INT);
    INT      (*pSetStretchBltMode)(PHYSDEV,INT);
    UINT     (*pSetTextAlign)(PHYSDEV,UINT);
    INT      (*pSetTextCharacterExtra)(PHYSDEV,INT);
    COLORREF (*pSetTextColor)(PHYSDEV,COLORREF);
    BOOL     (*pSetTextJustification)(PHYSDEV,INT,INT);
    BOOL     (*pSetViewportExtEx)(PHYSDEV,INT,INT,SIZE*);
    BOOL     (*pSetViewportOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pSetWindowExtEx)(PHYSDEV,INT,INT,SIZE*);
    BOOL     (*pSetWindowOrgEx)(PHYSDEV,INT,INT,POINT*);
    BOOL     (*pSetWorldTransform)(PHYSDEV,const XFORM*);
    INT      (*pStartDoc)(PHYSDEV,const DOCINFOW*);
    INT      (*pStartPage)(PHYSDEV);
    BOOL     (*pStretchBlt)(PHYSDEV,struct bitblt_coords*,PHYSDEV,struct bitblt_coords*,DWORD);
    INT      (*pStretchDIBits)(PHYSDEV,INT,INT,INT,INT,INT,INT,INT,INT,const void*,BITMAPINFO*,UINT,DWORD);
    BOOL     (*pStrokeAndFillPath)(PHYSDEV);
    BOOL     (*pStrokePath)(PHYSDEV);
    BOOL     (*pUnrealizePalette)(HPALETTE);
    BOOL     (*pWidenPath)(PHYSDEV);
    NTSTATUS (CDECL *pD3DKMTCheckVidPnExclusiveOwnership)(const D3DKMT_CHECKVIDPNEXCLUSIVEOWNERSHIP *);
    NTSTATUS (CDECL *pD3DKMTSetVidPnSourceOwner)(const D3DKMT_SETVIDPNSOURCEOWNER *);	
    struct opengl_funcs * (*wine_get_wgl_driver)(PHYSDEV,UINT);

    /* priority order for the driver on the stack */
    UINT       priority;
};

typedef struct _REALIZATION_INFO
{
    DWORD  iTechnology;
    DWORD  iUniq;
    DWORD  iFontFileId;
} REALIZATION_INFO, *PREALIZATION_INFO;

typedef struct _D3DKMT_CHECKSHAREDRESOURCEACCESS {
  D3DKMT_HANDLE hResource;
  UINT          ClientPid;
} D3DKMT_CHECKSHAREDRESOURCEACCESS;

void *get_any_obj_ptr( HGDIOBJ handle, WORD *type );

DC *get_dc_ptr( HDC hdc );

void release_dc_ptr( DC *dc );

BOOL
WINAPI
GetDevicePowerState(IN HANDLE hDevice,
                    OUT BOOL *pfOn);
					
BOOL 
WINAPI 
SetupDiGetDevicePropertyW(
	HDEVINFO devinfo, 
	PSP_DEVINFO_DATA device_data,
	DEVPROPKEY *prop_key, 
	DEVPROPTYPE *prop_type, 
	BYTE *prop_buff,
    DWORD prop_buff_size, 
	DWORD *required_size, 
	DWORD flags
);

LSTATUS
WINAPI
RegGetValueW(
  _In_ HKEY hkey,
  _In_opt_ LPCWSTR lpSubKey,
  _In_opt_ LPCWSTR lpValue,
  _In_ DWORD dwFlags,
  _Out_opt_ LPDWORD pdwType,
  _When_((dwFlags & 0x7F) == RRF_RT_REG_SZ || (dwFlags & 0x7F) == RRF_RT_REG_EXPAND_SZ ||
    (dwFlags & 0x7F) == (RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ) || *pdwType == REG_SZ ||
    *pdwType == REG_EXPAND_SZ, _Post_z_)
    _When_((dwFlags & 0x7F) == RRF_RT_REG_MULTI_SZ || *pdwType == REG_MULTI_SZ, _Post_ _NullNull_terminated_)
      _Out_writes_bytes_to_opt_(*pcbData,*pcbData) PVOID pvData,
  _Inout_opt_ LPDWORD pcbData);					
  
BOOL
WINAPI
GdiRealizationInfo(
	HDC hdc,
    PREALIZATION_INFO pri
);

typedef BOOL (WINAPI *pD3DKMTCreateDCFromMemory)(
    D3DKMT_CREATEDCFROMMEMORY*);
	
typedef BOOL (WINAPI *pD3DKMTDestroyDCFromMemory)(
    D3DKMT_DESTROYDCFROMMEMORY*);	
	
typedef HBITMAP (WINAPI *pCreateBitmapFromDxSurface)(
    HDC, UINT32, UINT32, ULONG, HANDLE);	