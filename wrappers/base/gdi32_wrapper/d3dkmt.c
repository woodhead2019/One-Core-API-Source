/*
 * Copyright 2009 Henri Verbeet for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#include <main.h>
#include <ntstrsafe.h>
//#include <d3dnthal.h>
#undef _WIN32
#include <ddrawi.h>
#include <ddrawgdi.h>

#define MAX_GDI_HANDLES  16384
#define FIRST_GDI_HANDLE 32
#define MAX_FONT_HANDLES  256

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
        ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
        ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
#endif /* MAKEFOURCC */

#define D3DKMT_MAX_ADAPTER_NAME_LENGTH 32

typedef struct _D3DKMT_CHECKSHAREDRESOURCEACCESS {
  D3DKMT_HANDLE hResource;
  UINT          ClientPid;
} D3DKMT_CHECKSHAREDRESOURCEACCESS;

typedef BOOL (WINAPI *pD3DKMTCreateDCFromMemory)(
    D3DKMT_CREATEDCFROMMEMORY*);
	
typedef BOOL (WINAPI *pD3DKMTDestroyDCFromMemory)(
    D3DKMT_DESTROYDCFROMMEMORY*);	

/*Hack, i don't know how require these funcions really*/
DWORD
APIENTRY
DdEntry28(
    _In_ HANDLE hDirectDraw,
    _Inout_ PDD_GETAVAILDRIVERMEMORYDATA puGetAvailDriverMemoryData);
	
DWORD
APIENTRY
DdEntry6(PDD_GETDRIVERSTATEDATA pdata);	
	
HANDLE
APIENTRY
DdEntry15(HDC hdc);	

BOOL
APIENTRY
DdEntry1(
    _In_ HANDLE hDirectDrawLocal,
    _In_ HANDLE hSurfColor,
    _In_ HANDLE hSurfZ,
    _Inout_ D3DNTHAL_CONTEXTCREATEDATA *pdcci);

DEFINE_DEVPROPKEY(DEVPROPKEY_GPU_LUID, 0x60b193cb, 0x5276, 0x4d0f, 0x96, 0xfc, 0xf1, 0x73, 0xab, 0xad, 0x3e, 0xc6, 2); 

WINE_DEFAULT_DEBUG_CHANNEL(gdi);

struct graphics_driver
{
    struct list                entry;
    HMODULE                    module;  /* module handle */
    const struct gdi_dc_funcs *funcs;
};
 
static HWND (WINAPI *pGetDesktopWindow)(void);
static INT (WINAPI *pGetSystemMetrics)(INT);
//static DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);  

static CRITICAL_SECTION gdi_section;

static struct list d3dkmt_adapters = LIST_INIT( d3dkmt_adapters );
static struct list d3dkmt_devices = LIST_INIT( d3dkmt_devices );

static CRITICAL_SECTION driver_section;
static CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &driver_section,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": driver_section") }
};
static CRITICAL_SECTION driver_section = { &critsect_debug, -1, 0, 0, 0, 0 };

struct d3dkmt_adapter
{
    D3DKMT_HANDLE handle;               /* Kernel mode graphics adapter handle */
    struct list entry;                  /* List entry */
};

struct d3dkmt_device
{
    D3DKMT_HANDLE handle;               /* Kernel mode graphics device handle*/
    struct list entry;                  /* List entry */
};

static struct graphics_driver *display_driver;

#define WINE_GDI_DRIVER_VERSION 48

/**********************************************************************
 *	     create_driver
 *
 * Allocate and fill the driver structure for a given module.
 */
static struct graphics_driver *create_driver( HMODULE module )
{
    static const struct gdi_dc_funcs empty_funcs;
    const struct gdi_dc_funcs *funcs = NULL;
    struct graphics_driver *driver;

    if (!(driver = HeapAlloc( GetProcessHeap(), 0, sizeof(*driver)))) return NULL;
    driver->module = module;

    if (module)
    {
        const struct gdi_dc_funcs * (CDECL *wine_get_gdi_driver)( unsigned int version );

        if ((wine_get_gdi_driver = (void *)GetProcAddress( module, "wine_get_gdi_driver" )))
            funcs = wine_get_gdi_driver( WINE_GDI_DRIVER_VERSION );
    }
    if (!funcs) funcs = &empty_funcs;
    driver->funcs = funcs;
    return driver;
}


/***********************************************************************
 *           __wine_set_display_driver    (GDI32.@)
 */
void CDECL __wine_set_display_driver( HMODULE module )
{
    struct graphics_driver *driver;
    HMODULE user32;

    if (!(driver = create_driver( module )))
    {
        ERR( "Could not create graphics driver\n" );
        ExitProcess(1);
    }
    if (InterlockedCompareExchangePointer( (void **)&display_driver, driver, NULL ))
        HeapFree( GetProcessHeap(), 0, driver );

    user32 = LoadLibraryA( "user32.dll" );
    pGetSystemMetrics = (void *)GetProcAddress( user32, "GetSystemMetrics" );
    //pSetThreadDpiAwarenessContext = (void *)GetProcAddress( user32, "SetThreadDpiAwarenessContext" );
}

/**********************************************************************
 *	     get_display_driver
 *
 * Special case for loading the display driver: get the name from the config file
 */
static const struct gdi_dc_funcs *get_display_driver(void)
{
    if (!display_driver)
    {
        HMODULE user32 = LoadLibraryA( "user32.dll" );
        pGetDesktopWindow = (void *)GetProcAddress( user32, "GetDesktopWindow" );

        if (!pGetDesktopWindow() || !display_driver)
        {
            WARN( "failed to load the display driver, falling back to null driver\n" );
            __wine_set_display_driver( 0 );
        }
    }
    return display_driver->funcs;
}

static HANDLE get_display_device_init_mutex( void )
{
    static const WCHAR init_mutex[] = {'d','i','s','p','l','a','y','_','d','e','v','i','c','e','_','i','n','i','t',0};
    HANDLE mutex = CreateMutexW( NULL, FALSE, init_mutex );

    WaitForSingleObject( mutex, INFINITE );
    return mutex;
}

static void release_display_device_init_mutex( HANDLE mutex )
{
    ReleaseMutex( mutex );
    CloseHandle( mutex );
}

/******************************************************************************
 *		D3DKMTCloseAdapter [GDI32.@]
 */
NTSTATUS WINAPI D3DKMTCloseAdapter( const D3DKMT_CLOSEADAPTER *desc )
{
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    struct d3dkmt_adapter *adapter;

    TRACE("(%p)\n", desc);

    if (!desc || !desc->hAdapter)
        return STATUS_INVALID_PARAMETER;

    EnterCriticalSection( &driver_section );
    LIST_FOR_EACH_ENTRY( adapter, &d3dkmt_adapters, struct d3dkmt_adapter, entry )
    {
        if (adapter->handle == desc->hAdapter)
        {
            list_remove( &adapter->entry );
            heap_free( adapter );
            status = STATUS_SUCCESS;
            break;
        }
    }
    LeaveCriticalSection( &driver_section );

    return status;
}

#define BMF_TOPDOWN   0x0001
#define BMF_NOZEROINIT   0x0002

/* bitmap object */

typedef struct tagBITMAPOBJ
{
    //struct gdi_obj_header obj;
    DIBSECTION            dib;
    SIZE                  size;   /* For SetBitmapDimension() */
    RGBQUAD              *color_table;  /* DIB color table if <= 8bpp (always 1 << bpp in size) */
} BITMAPOBJ;

const RGBQUAD *get_default_color_table( int bpp )
{
    static const RGBQUAD table_1[2] =
    {
        { 0x00, 0x00, 0x00 }, { 0xff, 0xff, 0xff }
    };
    static const RGBQUAD table_4[16] =
    {
        { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x80 }, { 0x00, 0x80, 0x00 }, { 0x00, 0x80, 0x80 },
        { 0x80, 0x00, 0x00 }, { 0x80, 0x00, 0x80 }, { 0x80, 0x80, 0x00 }, { 0x80, 0x80, 0x80 },
        { 0xc0, 0xc0, 0xc0 }, { 0x00, 0x00, 0xff }, { 0x00, 0xff, 0x00 }, { 0x00, 0xff, 0xff },
        { 0xff, 0x00, 0x00 }, { 0xff, 0x00, 0xff }, { 0xff, 0xff, 0x00 }, { 0xff, 0xff, 0xff },
    };
    static const RGBQUAD table_8[256] =
    {
        /* first and last 10 entries are the default system palette entries */
        { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x80 }, { 0x00, 0x80, 0x00 }, { 0x00, 0x80, 0x80 },
        { 0x80, 0x00, 0x00 }, { 0x80, 0x00, 0x80 }, { 0x80, 0x80, 0x00 }, { 0xc0, 0xc0, 0xc0 },
        { 0xc0, 0xdc, 0xc0 }, { 0xf0, 0xca, 0xa6 }, { 0x00, 0x20, 0x40 }, { 0x00, 0x20, 0x60 },
        { 0x00, 0x20, 0x80 }, { 0x00, 0x20, 0xa0 }, { 0x00, 0x20, 0xc0 }, { 0x00, 0x20, 0xe0 },
        { 0x00, 0x40, 0x00 }, { 0x00, 0x40, 0x20 }, { 0x00, 0x40, 0x40 }, { 0x00, 0x40, 0x60 },
        { 0x00, 0x40, 0x80 }, { 0x00, 0x40, 0xa0 }, { 0x00, 0x40, 0xc0 }, { 0x00, 0x40, 0xe0 },
        { 0x00, 0x60, 0x00 }, { 0x00, 0x60, 0x20 }, { 0x00, 0x60, 0x40 }, { 0x00, 0x60, 0x60 },
        { 0x00, 0x60, 0x80 }, { 0x00, 0x60, 0xa0 }, { 0x00, 0x60, 0xc0 }, { 0x00, 0x60, 0xe0 },
        { 0x00, 0x80, 0x00 }, { 0x00, 0x80, 0x20 }, { 0x00, 0x80, 0x40 }, { 0x00, 0x80, 0x60 },
        { 0x00, 0x80, 0x80 }, { 0x00, 0x80, 0xa0 }, { 0x00, 0x80, 0xc0 }, { 0x00, 0x80, 0xe0 },
        { 0x00, 0xa0, 0x00 }, { 0x00, 0xa0, 0x20 }, { 0x00, 0xa0, 0x40 }, { 0x00, 0xa0, 0x60 },
        { 0x00, 0xa0, 0x80 }, { 0x00, 0xa0, 0xa0 }, { 0x00, 0xa0, 0xc0 }, { 0x00, 0xa0, 0xe0 },
        { 0x00, 0xc0, 0x00 }, { 0x00, 0xc0, 0x20 }, { 0x00, 0xc0, 0x40 }, { 0x00, 0xc0, 0x60 },
        { 0x00, 0xc0, 0x80 }, { 0x00, 0xc0, 0xa0 }, { 0x00, 0xc0, 0xc0 }, { 0x00, 0xc0, 0xe0 },
        { 0x00, 0xe0, 0x00 }, { 0x00, 0xe0, 0x20 }, { 0x00, 0xe0, 0x40 }, { 0x00, 0xe0, 0x60 },
        { 0x00, 0xe0, 0x80 }, { 0x00, 0xe0, 0xa0 }, { 0x00, 0xe0, 0xc0 }, { 0x00, 0xe0, 0xe0 },
        { 0x40, 0x00, 0x00 }, { 0x40, 0x00, 0x20 }, { 0x40, 0x00, 0x40 }, { 0x40, 0x00, 0x60 },
        { 0x40, 0x00, 0x80 }, { 0x40, 0x00, 0xa0 }, { 0x40, 0x00, 0xc0 }, { 0x40, 0x00, 0xe0 },
        { 0x40, 0x20, 0x00 }, { 0x40, 0x20, 0x20 }, { 0x40, 0x20, 0x40 }, { 0x40, 0x20, 0x60 },
        { 0x40, 0x20, 0x80 }, { 0x40, 0x20, 0xa0 }, { 0x40, 0x20, 0xc0 }, { 0x40, 0x20, 0xe0 },
        { 0x40, 0x40, 0x00 }, { 0x40, 0x40, 0x20 }, { 0x40, 0x40, 0x40 }, { 0x40, 0x40, 0x60 },
        { 0x40, 0x40, 0x80 }, { 0x40, 0x40, 0xa0 }, { 0x40, 0x40, 0xc0 }, { 0x40, 0x40, 0xe0 },
        { 0x40, 0x60, 0x00 }, { 0x40, 0x60, 0x20 }, { 0x40, 0x60, 0x40 }, { 0x40, 0x60, 0x60 },
        { 0x40, 0x60, 0x80 }, { 0x40, 0x60, 0xa0 }, { 0x40, 0x60, 0xc0 }, { 0x40, 0x60, 0xe0 },
        { 0x40, 0x80, 0x00 }, { 0x40, 0x80, 0x20 }, { 0x40, 0x80, 0x40 }, { 0x40, 0x80, 0x60 },
        { 0x40, 0x80, 0x80 }, { 0x40, 0x80, 0xa0 }, { 0x40, 0x80, 0xc0 }, { 0x40, 0x80, 0xe0 },
        { 0x40, 0xa0, 0x00 }, { 0x40, 0xa0, 0x20 }, { 0x40, 0xa0, 0x40 }, { 0x40, 0xa0, 0x60 },
        { 0x40, 0xa0, 0x80 }, { 0x40, 0xa0, 0xa0 }, { 0x40, 0xa0, 0xc0 }, { 0x40, 0xa0, 0xe0 },
        { 0x40, 0xc0, 0x00 }, { 0x40, 0xc0, 0x20 }, { 0x40, 0xc0, 0x40 }, { 0x40, 0xc0, 0x60 },
        { 0x40, 0xc0, 0x80 }, { 0x40, 0xc0, 0xa0 }, { 0x40, 0xc0, 0xc0 }, { 0x40, 0xc0, 0xe0 },
        { 0x40, 0xe0, 0x00 }, { 0x40, 0xe0, 0x20 }, { 0x40, 0xe0, 0x40 }, { 0x40, 0xe0, 0x60 },
        { 0x40, 0xe0, 0x80 }, { 0x40, 0xe0, 0xa0 }, { 0x40, 0xe0, 0xc0 }, { 0x40, 0xe0, 0xe0 },
        { 0x80, 0x00, 0x00 }, { 0x80, 0x00, 0x20 }, { 0x80, 0x00, 0x40 }, { 0x80, 0x00, 0x60 },
        { 0x80, 0x00, 0x80 }, { 0x80, 0x00, 0xa0 }, { 0x80, 0x00, 0xc0 }, { 0x80, 0x00, 0xe0 },
        { 0x80, 0x20, 0x00 }, { 0x80, 0x20, 0x20 }, { 0x80, 0x20, 0x40 }, { 0x80, 0x20, 0x60 },
        { 0x80, 0x20, 0x80 }, { 0x80, 0x20, 0xa0 }, { 0x80, 0x20, 0xc0 }, { 0x80, 0x20, 0xe0 },
        { 0x80, 0x40, 0x00 }, { 0x80, 0x40, 0x20 }, { 0x80, 0x40, 0x40 }, { 0x80, 0x40, 0x60 },
        { 0x80, 0x40, 0x80 }, { 0x80, 0x40, 0xa0 }, { 0x80, 0x40, 0xc0 }, { 0x80, 0x40, 0xe0 },
        { 0x80, 0x60, 0x00 }, { 0x80, 0x60, 0x20 }, { 0x80, 0x60, 0x40 }, { 0x80, 0x60, 0x60 },
        { 0x80, 0x60, 0x80 }, { 0x80, 0x60, 0xa0 }, { 0x80, 0x60, 0xc0 }, { 0x80, 0x60, 0xe0 },
        { 0x80, 0x80, 0x00 }, { 0x80, 0x80, 0x20 }, { 0x80, 0x80, 0x40 }, { 0x80, 0x80, 0x60 },
        { 0x80, 0x80, 0x80 }, { 0x80, 0x80, 0xa0 }, { 0x80, 0x80, 0xc0 }, { 0x80, 0x80, 0xe0 },
        { 0x80, 0xa0, 0x00 }, { 0x80, 0xa0, 0x20 }, { 0x80, 0xa0, 0x40 }, { 0x80, 0xa0, 0x60 },
        { 0x80, 0xa0, 0x80 }, { 0x80, 0xa0, 0xa0 }, { 0x80, 0xa0, 0xc0 }, { 0x80, 0xa0, 0xe0 },
        { 0x80, 0xc0, 0x00 }, { 0x80, 0xc0, 0x20 }, { 0x80, 0xc0, 0x40 }, { 0x80, 0xc0, 0x60 },
        { 0x80, 0xc0, 0x80 }, { 0x80, 0xc0, 0xa0 }, { 0x80, 0xc0, 0xc0 }, { 0x80, 0xc0, 0xe0 },
        { 0x80, 0xe0, 0x00 }, { 0x80, 0xe0, 0x20 }, { 0x80, 0xe0, 0x40 }, { 0x80, 0xe0, 0x60 },
        { 0x80, 0xe0, 0x80 }, { 0x80, 0xe0, 0xa0 }, { 0x80, 0xe0, 0xc0 }, { 0x80, 0xe0, 0xe0 },
        { 0xc0, 0x00, 0x00 }, { 0xc0, 0x00, 0x20 }, { 0xc0, 0x00, 0x40 }, { 0xc0, 0x00, 0x60 },
        { 0xc0, 0x00, 0x80 }, { 0xc0, 0x00, 0xa0 }, { 0xc0, 0x00, 0xc0 }, { 0xc0, 0x00, 0xe0 },
        { 0xc0, 0x20, 0x00 }, { 0xc0, 0x20, 0x20 }, { 0xc0, 0x20, 0x40 }, { 0xc0, 0x20, 0x60 },
        { 0xc0, 0x20, 0x80 }, { 0xc0, 0x20, 0xa0 }, { 0xc0, 0x20, 0xc0 }, { 0xc0, 0x20, 0xe0 },
        { 0xc0, 0x40, 0x00 }, { 0xc0, 0x40, 0x20 }, { 0xc0, 0x40, 0x40 }, { 0xc0, 0x40, 0x60 },
        { 0xc0, 0x40, 0x80 }, { 0xc0, 0x40, 0xa0 }, { 0xc0, 0x40, 0xc0 }, { 0xc0, 0x40, 0xe0 },
        { 0xc0, 0x60, 0x00 }, { 0xc0, 0x60, 0x20 }, { 0xc0, 0x60, 0x40 }, { 0xc0, 0x60, 0x60 },
        { 0xc0, 0x60, 0x80 }, { 0xc0, 0x60, 0xa0 }, { 0xc0, 0x60, 0xc0 }, { 0xc0, 0x60, 0xe0 },
        { 0xc0, 0x80, 0x00 }, { 0xc0, 0x80, 0x20 }, { 0xc0, 0x80, 0x40 }, { 0xc0, 0x80, 0x60 },
        { 0xc0, 0x80, 0x80 }, { 0xc0, 0x80, 0xa0 }, { 0xc0, 0x80, 0xc0 }, { 0xc0, 0x80, 0xe0 },
        { 0xc0, 0xa0, 0x00 }, { 0xc0, 0xa0, 0x20 }, { 0xc0, 0xa0, 0x40 }, { 0xc0, 0xa0, 0x60 },
        { 0xc0, 0xa0, 0x80 }, { 0xc0, 0xa0, 0xa0 }, { 0xc0, 0xa0, 0xc0 }, { 0xc0, 0xa0, 0xe0 },
        { 0xc0, 0xc0, 0x00 }, { 0xc0, 0xc0, 0x20 }, { 0xc0, 0xc0, 0x40 }, { 0xc0, 0xc0, 0x60 },
        { 0xc0, 0xc0, 0x80 }, { 0xc0, 0xc0, 0xa0 }, { 0xf0, 0xfb, 0xff }, { 0xa4, 0xa0, 0xa0 },
        { 0x80, 0x80, 0x80 }, { 0x00, 0x00, 0xff }, { 0x00, 0xff, 0x00 }, { 0x00, 0xff, 0xff },
        { 0xff, 0x00, 0x00 }, { 0xff, 0x00, 0xff }, { 0xff, 0xff, 0x00 }, { 0xff, 0xff, 0xff },
    };

    switch (bpp)
    {
    case 1: return table_1;
    case 4: return table_4;
    case 8: return table_8;
    default: return NULL;
    }
}


/***********************************************************************
 *           D3DKMTCreateDCFromMemory    (GDI32.@)
 */
NTSTATUS WINAPI D3DKMTCreateDCFromMemory( D3DKMT_CREATEDCFROMMEMORY *desc )
{
	pD3DKMTCreateDCFromMemory pD3dCreateDC;
	
    pD3dCreateDC = (pD3DKMTCreateDCFromMemory) GetProcAddress(
                            GetModuleHandle(TEXT("gdibase")),
                            "D3DKMTCreateDCFromMemory");	

	if(pD3dCreateDC){
		return pD3dCreateDC(desc);
	}else{
		// const struct d3dddi_format_info
		// {
			// D3DDDIFORMAT format;
			// unsigned int bit_count;
			// DWORD compression;
			// unsigned int palette_size;
			// DWORD mask_r, mask_g, mask_b;
		// } *format = NULL;
		// BITMAPINFO *bmpInfo = NULL;
		// BITMAPV5HEADER *bmpHeader = NULL;
		// HBITMAP bitmap;
		// unsigned int i;
		// HDC dc;	
		// static const struct d3dddi_format_info format_info[] =
		// {
			// { D3DDDIFMT_R8G8B8,   24, BI_RGB,       0,   0x00000000, 0x00000000, 0x00000000 },
			// { D3DDDIFMT_A8R8G8B8, 32, BI_RGB,       0,   0x00000000, 0x00000000, 0x00000000 },
			// { D3DDDIFMT_X8R8G8B8, 32, BI_RGB,       0,   0x00000000, 0x00000000, 0x00000000 },
			// { D3DDDIFMT_R5G6B5,   16, BI_BITFIELDS, 0,   0x0000f800, 0x000007e0, 0x0000001f },
			// { D3DDDIFMT_X1R5G5B5, 16, BI_BITFIELDS, 0,   0x00007c00, 0x000003e0, 0x0000001f },
			// { D3DDDIFMT_A1R5G5B5, 16, BI_BITFIELDS, 0,   0x00007c00, 0x000003e0, 0x0000001f },
			// { D3DDDIFMT_A4R4G4B4, 16, BI_BITFIELDS, 0,   0x00000f00, 0x000000f0, 0x0000000f },
			// { D3DDDIFMT_X4R4G4B4, 16, BI_BITFIELDS, 0,   0x00000f00, 0x000000f0, 0x0000000f },
			// { D3DDDIFMT_P8,       8,  BI_RGB,       256, 0x00000000, 0x00000000, 0x00000000 },
		// };
		
		// DbgPrint("D3DKMTCreateDCFromMemory:: calling function\n");

		// if (!desc) return STATUS_INVALID_PARAMETER;

		// if (!desc->pMemory) return STATUS_INVALID_PARAMETER;

		// for (i = 0; i < sizeof(format_info) / sizeof(*format_info); ++i)
		// {
			// if (format_info[i].format == desc->Format)
			// {
				// format = &format_info[i];
				// break;
			// }
		// }
		// if (!format) return STATUS_INVALID_PARAMETER;

		// if (desc->Width > (UINT_MAX & ~3) / (format->bit_count / 8) ||
			// !desc->Pitch || desc->Pitch < (((desc->Width * format->bit_count + 31) >> 3) & ~3) ||
			// !desc->Height || desc->Height > UINT_MAX / desc->Pitch) return STATUS_INVALID_PARAMETER;

		// if (!desc->hDeviceDc || !(dc = CreateCompatibleDC( desc->hDeviceDc ))) return STATUS_INVALID_PARAMETER;

		// if (!(bmpInfo = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*bmpInfo) + (format->palette_size * sizeof(RGBQUAD)) ))) goto error;
		// if (!(bmpHeader = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*bmpHeader) ))) goto error;

		
		// bmpHeader->bV5Size        = sizeof(*bmpHeader);
		// bmpHeader->bV5Width       = desc->Width;
		// bmpHeader->bV5Height      = desc->Height;
		// bmpHeader->bV5SizeImage   = desc->Pitch;
		// bmpHeader->bV5Planes      = 1;
		// bmpHeader->bV5BitCount    = format->bit_count;
		// bmpHeader->bV5Compression = BI_BITFIELDS;
		// bmpHeader->bV5RedMask     = format->mask_r;
		// bmpHeader->bV5GreenMask   = format->mask_g;
		// bmpHeader->bV5BlueMask    = format->mask_b;

		// bmpInfo->bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
		// bmpInfo->bmiHeader.biWidth        = desc->Width;
		// bmpInfo->bmiHeader.biHeight       = -(LONG)desc->Height;
		// bmpInfo->bmiHeader.biPlanes       = 1;
		// bmpInfo->bmiHeader.biBitCount     = format->bit_count;
		// bmpInfo->bmiHeader.biCompression  = format->compression;
		// bmpInfo->bmiHeader.biClrUsed      = format->palette_size;
		// bmpInfo->bmiHeader.biClrImportant = format->palette_size;

		// if (desc->pColorTable)
		// {
			// for (i = 0; i < format->palette_size; ++i)
			// {
				 // bmpInfo->bmiColors[i].rgbRed   = desc->pColorTable[i].peRed;
				 // bmpInfo->bmiColors[i].rgbGreen = desc->pColorTable[i].peGreen;
				 // bmpInfo->bmiColors[i].rgbBlue = desc->pColorTable[i].peBlue;
				 // bmpInfo->bmiColors[i].rgbReserved = 0;
			// }
		// }

		// if (!(bitmap = CreateBitmap(desc->Width, desc->Height, 1, format->bit_count, desc->pMemory))) goto error;

		// desc->hDc = dc;
		// desc->hBitmap = bitmap;
		// SelectObject( dc, bitmap );
		// DbgPrint("D3DKMTCreateDCFromMemory:: return STATUS_SUCCESS\n");
		// return STATUS_SUCCESS;

	// error:
		// if (bmpInfo)  HeapFree( GetProcessHeap(), 0, bmpInfo );
		// if (bmpHeader) HeapFree( GetProcessHeap(), 0, bmpHeader );

		// DeleteDC( dc );
		// return STATUS_INVALID_PARAMETER;	
    const struct d3dddi_format_info
    {
        D3DDDIFORMAT format;
        unsigned int bit_count;
        DWORD compression;
        unsigned int palette_size;
        DWORD mask_r, mask_g, mask_b;
    } *format = NULL;
    BITMAPOBJ *bmp = NULL;
    HBITMAP bitmap;
    unsigned int i;
    HDC dc;

    static const struct d3dddi_format_info format_info[] =
    {
        { D3DDDIFMT_R8G8B8,   24, BI_RGB,       0,   0x00000000, 0x00000000, 0x00000000 },
        { D3DDDIFMT_A8R8G8B8, 32, BI_RGB,       0,   0x00000000, 0x00000000, 0x00000000 },
        { D3DDDIFMT_X8R8G8B8, 32, BI_RGB,       0,   0x00000000, 0x00000000, 0x00000000 },
        { D3DDDIFMT_R5G6B5,   16, BI_BITFIELDS, 0,   0x0000f800, 0x000007e0, 0x0000001f },
        { D3DDDIFMT_X1R5G5B5, 16, BI_BITFIELDS, 0,   0x00007c00, 0x000003e0, 0x0000001f },
        { D3DDDIFMT_A1R5G5B5, 16, BI_BITFIELDS, 0,   0x00007c00, 0x000003e0, 0x0000001f },
        { D3DDDIFMT_A4R4G4B4, 16, BI_BITFIELDS, 0,   0x00000f00, 0x000000f0, 0x0000000f },
        { D3DDDIFMT_X4R4G4B4, 16, BI_BITFIELDS, 0,   0x00000f00, 0x000000f0, 0x0000000f },
        { D3DDDIFMT_P8,       8,  BI_RGB,       256, 0x00000000, 0x00000000, 0x00000000 },
    };

    if (!desc) return STATUS_INVALID_PARAMETER;

    TRACE("memory %p, format %#x, width %u, height %u, pitch %u, device dc %p, color table %p.\n",
          desc->pMemory, desc->Format, desc->Width, desc->Height,
          desc->Pitch, desc->hDeviceDc, desc->pColorTable);

    if (!desc->pMemory) return STATUS_INVALID_PARAMETER;

    for (i = 0; i < ARRAY_SIZE( format_info ); ++i)
    {
        if (format_info[i].format == desc->Format)
        {
            format = &format_info[i];
            break;
        }
    }
    if (!format) return STATUS_INVALID_PARAMETER;

    if (desc->Width > (UINT_MAX & ~3) / (format->bit_count / 8) ||
        !desc->Pitch || desc->Pitch < (((desc->Width * format->bit_count + 31) >> 3) & ~3) ||
        !desc->Height || desc->Height > UINT_MAX / desc->Pitch) return STATUS_INVALID_PARAMETER;

    if (!desc->hDeviceDc || !(dc = CreateCompatibleDC( desc->hDeviceDc ))) return STATUS_INVALID_PARAMETER;
        return STATUS_INVALID_PARAMETER;

    if (!(bmp = calloc( 1, sizeof(*bmp) ))) goto error;

    bmp->dib.dsBm.bmWidth      = desc->Width;
    bmp->dib.dsBm.bmHeight     = desc->Height;
    bmp->dib.dsBm.bmWidthBytes = desc->Pitch;
    bmp->dib.dsBm.bmPlanes     = 1;
    bmp->dib.dsBm.bmBitsPixel  = format->bit_count;
    bmp->dib.dsBm.bmBits       = desc->pMemory;

    bmp->dib.dsBmih.biSize         = sizeof(bmp->dib.dsBmih);
    bmp->dib.dsBmih.biWidth        = desc->Width;
    bmp->dib.dsBmih.biHeight       = -(LONG)desc->Height;
    bmp->dib.dsBmih.biPlanes       = 1;
    bmp->dib.dsBmih.biBitCount     = format->bit_count;
    bmp->dib.dsBmih.biCompression  = format->compression;
    bmp->dib.dsBmih.biClrUsed      = format->palette_size;
    bmp->dib.dsBmih.biClrImportant = format->palette_size;

    bmp->dib.dsBitfields[0] = format->mask_r;
    bmp->dib.dsBitfields[1] = format->mask_g;
    bmp->dib.dsBitfields[2] = format->mask_b;

    if (format->palette_size)
    {
        if (!(bmp->color_table = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, format->palette_size * sizeof(*bmp->color_table) )))
            goto error;
        if (desc->pColorTable)
        {
            for (i = 0; i < format->palette_size; ++i)
            {
                bmp->color_table[i].rgbRed      = desc->pColorTable[i].peRed;
                bmp->color_table[i].rgbGreen    = desc->pColorTable[i].peGreen;
                bmp->color_table[i].rgbBlue     = desc->pColorTable[i].peBlue;
                bmp->color_table[i].rgbReserved = 0;
            }
        }
        else
        {
            memcpy( bmp->color_table, get_default_color_table( format->bit_count ),
                    format->palette_size * sizeof(*bmp->color_table) );
        }
    }

    if (!(bitmap = CreateBitmap(desc->Width, desc->Height, 1, format->bit_count, desc->pMemory))) goto error;

    desc->hDc = dc;
    desc->hBitmap = bitmap;
    SelectObject( dc, bitmap );
    return STATUS_SUCCESS;

error:
    if (bmp) HeapFree( GetProcessHeap(), 0, bmp->color_table );
    HeapFree( GetProcessHeap(), 0, bmp );	
    DeleteDC( dc );
    return STATUS_INVALID_PARAMETER;
		
	}
}


/***********************************************************************
 *           D3DKMTDestroyDCFromMemory    (GDI32.@)
 */
NTSTATUS WINAPI D3DKMTDestroyDCFromMemory( const D3DKMT_DESTROYDCFROMMEMORY *desc )
{
	pD3DKMTDestroyDCFromMemory pD3dDestroyDC;
	
    pD3dDestroyDC = (pD3DKMTDestroyDCFromMemory) GetProcAddress(
                            GetModuleHandle(TEXT("gdibase")),
                            "D3DKMTDestroyDCFromMemory");	

	if(pD3dDestroyDC){
		return pD3dDestroyDC(desc);
	}else{
		if (!desc) return STATUS_INVALID_PARAMETER;

		TRACE("dc %p, bitmap %p.\n", desc->hDc, desc->hBitmap);

		if (GetObjectType( desc->hDc ) != OBJ_MEMDC ||
			GetObjectType( desc->hBitmap ) != OBJ_BITMAP) return STATUS_INVALID_PARAMETER;
		DeleteObject( desc->hBitmap );
		DeleteDC( desc->hDc );

		return STATUS_SUCCESS;	
	}
}

NTSTATUS 
APIENTRY 
D3DKMTCheckMonitorPowerState(
  _In_ const D3DKMT_CHECKMONITORPOWERSTATE *pData
)
{
	HMONITOR mon;
	BOOL state;
	mon = MonitorFromWindow( NULL,MONITOR_DEFAULTTONULL );	
	GetDevicePowerState(mon, &state);
	if(state){
		return STATUS_SUCCESS;
	}else{
		return STATUS_INVALID_PARAMETER;
	}		
}

/******************************************************************************
 *		D3DKMTOpenAdapterFromHdc [GDI32.@]
 */
NTSTATUS 
WINAPI 
D3DKMTOpenAdapterFromHdc( D3DKMT_OPENADAPTERFROMHDC *pData )
{
    return STATUS_NO_MEMORY;
}

/******************************************************************************
 *		D3DKMTEscape [GDI32.@]
 */
NTSTATUS 
WINAPI 
D3DKMTEscape( const D3DKMT_ESCAPE *pData )
{
    return STATUS_NO_MEMORY;
}

// /******************************************************************************
 // *		D3DKMTOpenAdapterFromGdiDisplayName [GDI32.@]
 // */
// NTSTATUS WINAPI D3DKMTOpenAdapterFromGdiDisplayName( D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME *desc )
// {
    // static const WCHAR displayW[] = {'\\','\\','.','\\','D','I','S','P','L','A','Y'};
    // static const WCHAR state_flagsW[] = {'S','t','a','t','e','F','l','a','g','s',0};
    // static const WCHAR video_value_fmtW[] = {'\\','D','e','v','i','c','e','\\',
                                             // 'V','i','d','e','o','%','d',0};
    // static const WCHAR video_keyW[] = {'H','A','R','D','W','A','R','E','\\',
                                       // 'D','E','V','I','C','E','M','A','P','\\',
                                       // 'V','I','D','E','O','\\',0};
    // static const WCHAR gpu_idW[] = {'G','P','U','I','D',0};
    // WCHAR *end, key_nameW[MAX_PATH], bufferW[MAX_PATH];
    // HDEVINFO devinfo = INVALID_HANDLE_VALUE;
    // NTSTATUS status = STATUS_UNSUCCESSFUL;
    // static D3DKMT_HANDLE handle_start = 0;
    // struct d3dkmt_adapter *adapter;
    // SP_DEVINFO_DATA device_data;
    // DWORD size, state_flags;
    // DEVPROPTYPE type;
    // HANDLE mutex;
    // LUID luid;
    // int index;

    // TRACE("(%p)\n", desc);

    // if (!desc)
        // return STATUS_UNSUCCESSFUL;

    // TRACE("DeviceName: %s\n", wine_dbgstr_w( desc->DeviceName ));
    // if (strncmpiW( desc->DeviceName, displayW, ARRAY_SIZE(displayW) ))
        // return STATUS_UNSUCCESSFUL;

    // index = strtolW( desc->DeviceName + ARRAY_SIZE(displayW), &end, 10 ) - 1;
    // if (*end)
        // return STATUS_UNSUCCESSFUL;

    // adapter = heap_alloc( sizeof( *adapter ) );
    // if (!adapter)
        // return STATUS_NO_MEMORY;

    // /* Get adapter LUID from SetupAPI */
    // mutex = get_display_device_init_mutex();

    // size = sizeof( bufferW );
    // sDbgPrintW( key_nameW, video_value_fmtW, index );
    // if (RegGetValueW( HKEY_LOCAL_MACHINE, video_keyW, key_nameW, RRF_RT_REG_SZ, NULL, bufferW, &size ))
        // goto done;

    // /* Strip \Registry\Machine\ prefix and retrieve Wine specific data set by the display driver */
    // lstrcpyW( key_nameW, bufferW + 18 );
    // size = sizeof( state_flags );
    // if (RegGetValueW( HKEY_CURRENT_CONFIG, key_nameW, state_flagsW, RRF_RT_REG_DWORD, NULL,
                      // &state_flags, &size ))
        // goto done;

    // if (!(state_flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
        // goto done;

    // size = sizeof( bufferW );
    // if (RegGetValueW( HKEY_CURRENT_CONFIG, key_nameW, gpu_idW, RRF_RT_REG_SZ, NULL, bufferW, &size ))
        // goto done;

    // devinfo = SetupDiCreateDeviceInfoList( &GUID_DEVCLASS_DISPLAY, NULL );
    // device_data.cbSize = sizeof( device_data );
    // SetupDiOpenDeviceInfoW( devinfo, bufferW, NULL, 0, &device_data );
    // if (!SetupDiGetDevicePropertyW( devinfo, &device_data, &DEVPROPKEY_GPU_LUID, &type,
                                    // (BYTE *)&luid, sizeof( luid ), NULL, 0))
        // goto done;

    // EnterCriticalSection( &driver_section );
    // /* D3DKMT_HANDLE is UINT, so we can't use pointer as handle */
    // adapter->handle = ++handle_start;
    // list_add_tail( &d3dkmt_adapters, &adapter->entry );
    // LeaveCriticalSection( &driver_section );

    // desc->hAdapter = handle_start;
    // desc->AdapterLuid = luid;
    // desc->VidPnSourceId = index;
    // status = STATUS_SUCCESS;

// done:
    // SetupDiDestroyDeviceInfoList( devinfo );
    // release_display_device_init_mutex( mutex );
    // if (status != STATUS_SUCCESS)
        // heap_free( adapter );
    // return status;
// }

// /******************************************************************************
 // *		D3DKMTCreateDevice [GDI32.@]
 // */
// NTSTATUS WINAPI D3DKMTCreateDevice( D3DKMT_CREATEDEVICE *desc )
// {
    // static D3DKMT_HANDLE handle_start = 0;
    // struct d3dkmt_adapter *adapter;
    // struct d3dkmt_device *device;
    // BOOL found = FALSE;

    // TRACE("(%p)\n", desc);

    // if (!desc)
        // return STATUS_INVALID_PARAMETER;

    // EnterCriticalSection( &driver_section );
    // LIST_FOR_EACH_ENTRY( adapter, &d3dkmt_adapters, struct d3dkmt_adapter, entry )
    // {
        // if (adapter->handle == desc->hAdapter)
        // {
            // found = TRUE;
            // break;
        // }
    // }
    // LeaveCriticalSection( &driver_section );

    // if (!found)
        // return STATUS_INVALID_PARAMETER;

    // if (desc->Flags.LegacyMode || desc->Flags.RequestVSync || desc->Flags.DisableGpuTimeout)
        // FIXME("Flags unsupported.\n");

    // device = heap_alloc_zero( sizeof( *device ) );
    // if (!device)
        // return STATUS_NO_MEMORY;

    // EnterCriticalSection( &driver_section );
    // device->handle = ++handle_start;
    // list_add_tail( &d3dkmt_devices, &device->entry );
    // LeaveCriticalSection( &driver_section );

    // desc->hDevice = device->handle;
    // return STATUS_SUCCESS;
// }

NTSTATUS 
WINAPI 
D3DKMTCreateDevice(D3DKMT_CREATEDEVICE *pCreateDevice) {
    HDC hdcScreen;
	DDRAWI_DIRECTDRAW_GBL directDrawGlobal;
	
	directDrawGlobal.hDD = 0;

    // Validar parâmetro de entrada
    if (!pCreateDevice) {
        return STATUS_INVALID_PARAMETER; // STATUS_INVALID_PARAMETER
    }

    // Obter HDC da tela principal
    hdcScreen = GetDC(NULL);
    if (!hdcScreen) {
        DbgPrint("Erro: Falha ao obter HDC da tela\n");
        return STATUS_DEVICE_CONFIGURATION_ERROR; // STATUS_DEVICE_CONFIGURATION_ERROR
    }

    // Inicializar estrutura temporária para DirectDraw
    //ZeroMemory(directDrawGlobal, sizeof(directDrawGlobal));

    // Criar um objeto DirectDraw com DdCreateDirectDrawObject
    if (!GdiEntry1(&directDrawGlobal, hdcScreen)) {
        DbgPrint("Erro: Falha ao criar dispositivo DirectDraw\n");
        ReleaseDC(NULL, hdcScreen);
        return STATUS_UNSUCCESSFUL; // STATUS_UNSUCCESSFUL
    }

    // Preencher os dados da estrutura de saída
    pCreateDevice->hDevice = (D3DKMT_HANDLE)directDrawGlobal.hDD; // Handle do dispositivo DirectDraw
    pCreateDevice->pCommandBuffer = NULL;
    pCreateDevice->CommandBufferSize = 0;
    pCreateDevice->pAllocationList = NULL;
    pCreateDevice->AllocationListSize = 0;
    pCreateDevice->pPatchLocationList = NULL;
    pCreateDevice->PatchLocationListSize = 0;

    // Configurar flags para modo Legacy, VSync, etc. (Simulação no XP)
    if (pCreateDevice->Flags.LegacyMode) {
        DbgPrint("Modo Legacy ativado\n");
    }
    if (pCreateDevice->Flags.RequestVSync) {
        DbgPrint("VSync solicitado (não suportado no XP via DdCreateDirectDrawObject)\n");
    }
    if (pCreateDevice->Flags.DisableGpuTimeout) {
        DbgPrint("Timeout da GPU desativado (sem efeito no XP)\n");
    }

    DbgPrint("Dispositivo criado com sucesso! Handle: %p\n", pCreateDevice->hDevice);

    // Liberar recursos
    ReleaseDC(NULL, hdcScreen);
 
    return STATUS_SUCCESS; // STATUS_SUCCESS
}


/******************************************************************************
 *		D3DKMTSetVidPnSourceOwner [GDI32.@]
 */
NTSTATUS WINAPI D3DKMTSetVidPnSourceOwner( const D3DKMT_SETVIDPNSOURCEOWNER *desc )
{
    TRACE("(%p)\n", desc);

    if (!get_display_driver()->pD3DKMTSetVidPnSourceOwner)
        return STATUS_PROCEDURE_NOT_FOUND;

    if (!desc || !desc->hDevice || (desc->VidPnSourceCount && (!desc->pType || !desc->pVidPnSourceId)))
        return STATUS_INVALID_PARAMETER;

    /* Store the VidPN source ownership info in the graphics driver because
     * the graphics driver needs to change ownership sometimes. For example,
     * when a new window is moved to a VidPN source with an exclusive owner,
     * such an exclusive owner will be released before showing the new window */
    return get_display_driver()->pD3DKMTSetVidPnSourceOwner( desc );
}

/******************************************************************************
 *		D3DKMTDestroyDevice [GDI32.@]
 */
NTSTATUS WINAPI D3DKMTDestroyDevice( const D3DKMT_DESTROYDEVICE *desc )
{
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    D3DKMT_SETVIDPNSOURCEOWNER set_owner_desc;
    struct d3dkmt_device *device;

    TRACE("(%p)\n", desc);

    if (!desc || !desc->hDevice)
        return STATUS_INVALID_PARAMETER;

    EnterCriticalSection( &driver_section );
    LIST_FOR_EACH_ENTRY( device, &d3dkmt_devices, struct d3dkmt_device, entry )
    {
        if (device->handle == desc->hDevice)
        {
            memset( &set_owner_desc, 0, sizeof(set_owner_desc) );
            set_owner_desc.hDevice = desc->hDevice;
            D3DKMTSetVidPnSourceOwner( &set_owner_desc );
            list_remove( &device->entry );
            heap_free( device );
            status = STATUS_SUCCESS;
            break;
        }
    }
    LeaveCriticalSection( &driver_section );

    return status;
}

/******************************************************************************
 *		D3DKMTQueryStatistics [GDI32.@]
 */
NTSTATUS WINAPI D3DKMTQueryStatistics(D3DKMT_QUERYSTATISTICS *stats)
{
    FIXME("(%p): stub\n", stats);
    return STATUS_SUCCESS;
}

/******************************************************************************
 *		D3DKMTSetQueuedLimit [GDI32.@]
 */
NTSTATUS WINAPI D3DKMTSetQueuedLimit( D3DKMT_SETQUEUEDLIMIT *desc )
{
    FIXME( "(%p): stub\n", desc );
    return STATUS_NOT_IMPLEMENTED;
}

/******************************************************************************
 *		D3DKMTCheckVidPnExclusiveOwnership [GDI32.@]
 */
NTSTATUS WINAPI D3DKMTCheckVidPnExclusiveOwnership( const D3DKMT_CHECKVIDPNEXCLUSIVEOWNERSHIP *desc )
{
    TRACE("(%p)\n", desc);

    if (!get_display_driver()->pD3DKMTCheckVidPnExclusiveOwnership)
        return STATUS_PROCEDURE_NOT_FOUND;

    if (!desc || !desc->hAdapter)
        return STATUS_INVALID_PARAMETER;

    return get_display_driver()->pD3DKMTCheckVidPnExclusiveOwnership( desc );
}

const WCHAR displayName[32] = L"\\\\.\\DISPLAY1\0";

/******************************************************************************
 *           NtGdiDdDDIOpenAdapterFromLuid    (win32u.@)
 */
NTSTATUS WINAPI D3DKMTOpenAdapterFromLuid( D3DKMT_OPENADAPTERFROMLUID *desc )
{
	// DEVMODEW devMode;
	// WCHAR gdiDisplayName[D3DKMT_MAX_ADAPTER_NAME_LENGTH] = {0};
	// LUID luid = desc->AdapterLuid;
	// HRESULT hr;
	
	// if (!desc) {
	// return STATUS_INVALID_PARAMETER;
	// }

	// hr = StringCchDbgPrintW(
				// gdiDisplayName,
				// D3DKMT_MAX_ADAPTER_NAME_LENGTH,
				// L"\\\\.\\DISPLAY%08X%08X",
				// luid.HighPart,
				// luid.LowPart
				// );

	// if (FAILED(hr)) {
		// return STATUS_INVALID_PARAMETER;
	// }
	
	// ZeroMemory(&devMode, sizeof(DEVMODEW));
	// devMode.dmSize = sizeof(DEVMODEW);

	// if (!EnumDisplaySettingsW(gdiDisplayName, ENUM_CURRENT_SETTINGS, &devMode)) {
		// return STATUS_NOT_FOUND;
	// }


	// desc->hAdapter = (D3DKMT_HANDLE)1;

	// return STATUS_SUCCESS;	
	
    NTSTATUS Status;
    D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME adapter;
    if (!desc)
        return STATUS_INVALID_PARAMETER;
    ZeroMemory(&adapter, sizeof(D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME));
    wcscpy(adapter.DeviceName, displayName);

    // Dynamically load d3dkmt.dll, then call D3DKMTOpenAdapterFromGdiDisplayName. TODO: figure out how
    Status = D3DKMTOpenAdapterFromGdiDisplayName(&adapter);
    if (FAILED(Status))
        return Status;
    desc->hAdapter = adapter.hAdapter;
    return Status;
}

NTSTATUS
WINAPI 
D3DKMTQueryVideoMemoryInfo(
  D3DKMT_QUERYVIDEOMEMORYINFO *desc
)
{
    DWORD Ret = FALSE;
    DD_GETAVAILDRIVERMEMORYDATA Data;
    ZeroMemory(&Data, sizeof(Data));

	//We always want know total memory avaliable.
    Data.DDSCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

    if (D3DKMT_MEMORY_SEGMENT_GROUP_LOCAL == desc->MemorySegmentGroup)
    {
        Data.DDSCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
    }
    else if (D3DKMT_MEMORY_SEGMENT_GROUP_NON_LOCAL == desc->MemorySegmentGroup)
    {
        Data.DDSCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM;
    }

    if (Data.DDSCaps.dwCaps != 0)
    {
        Ret = DdEntry28(DdEntry15(CreateCompatibleDC(NULL)), (DD_GETAVAILDRIVERMEMORYDATA*)&Data);
        desc->Budget = Data.dwTotal;
        desc->CurrentUsage = Data.dwTotal - Data.dwFree;
        desc->CurrentReservation = 0;
        desc->AvailableForReservation = 100;
    }

    return Ret;	
}

NTSTATUS
WINAPI
D3DKMTCheckSharedResourceAccess(
  const D3DKMT_CHECKSHAREDRESOURCEACCESS *unnamedParam1
)
{
	if(unnamedParam1->ClientPid == GetCurrentProcessId())
		return STATUS_SUCCESS;
	else
		return STATUS_INVALID_PARAMETER;
}

NTSTATUS
WINAPI
D3DKMTSetProcessSchedulingPriorityClass(
    HANDLE                         unnamedParam1,
    D3DKMT_SCHEDULINGPRIORITYCLASS unnamedParam2
)
{
	return STATUS_NOT_IMPLEMENTED;
}

// Implementação de D3DKMTCheckExclusiveOwnership
HRESULT 
WINAPI
D3DKMTCheckExclusiveOwnership() {
    // Carrega a biblioteca GDI32.dll
	DD_GETDRIVERSTATEDATA stateData = { 0 };
	DWORD result;

    // Configura a estrutura DDHAL_GETDRIVERSTATEDATA
    
    stateData.dwFlags = 0; // Pode variar dependendo do driver
    stateData.dwhContext = 0;

    // Chama a função NtGdiDdGetDriverState - DdEntry6
    result = DdEntry6(&stateData);

    // Libera a biblioteca
    //FreeLibrary(hGdi32);

    // Verifica o resultado
    if (result != DD_OK) {
        return E_FAIL;
    }

    // Retorna o resultado da consulta
    return stateData.ddRVal;
}

//Função simulada de D3DKMTCheckOcclusion
HRESULT 
WINAPI 
D3DKMTCheckOcclusion(
	D3DKMT_CHECKOCCLUSION* pCheckOcclusion
	) 
{
	RECT windowRect;
	RECT monitorRect;
	RECT intersectionRect;
	
    if (!pCheckOcclusion || !pCheckOcclusion->hWnd) {
        return E_INVALIDARG;
    }

    // Verifica se a janela está visível
    if (!IsWindowVisible(pCheckOcclusion->hWnd)) {
        return S_FALSE; // Janela não está visível
    }

    // Obtém o retângulo da janela em coordenadas de tela
    if (!GetWindowRect(pCheckOcclusion->hWnd, &windowRect)) {
        return E_FAIL;
    }

    // Obtém o retângulo do monitor principal
    monitorRect.left = 0;
    monitorRect.left = 0;
    monitorRect.left = GetSystemMetrics(SM_CXSCREEN);
    monitorRect.left = GetSystemMetrics(SM_CYSCREEN);

    // Verifica se há interseção entre o retângulo da janela e o do monitor
    intersectionRect;
    if (!IntersectRect(&intersectionRect, &windowRect, &monitorRect)) {
        return S_FALSE; // Janela está fora da tela (oculta)
    }

    return S_OK; // Janela está visível
}

// NTSTATUS WINAPI D3DKMTCheckOcclusion(const D3DKMT_CHECKOCCLUSION *unnamedParam1) {
    // HDC dc = GetDC(unnamedParam1->hWnd);
    // RECT rect;
	// int result;

    // if (!unnamedParam1)
        // return STATUS_INVALID_PARAMETER;	
	
    // if (dc) {
        // result = GetClipBox(dc, &rect);
        // ReleaseDC((HWND)dc);
        // return result == NULLREGION ? STATUS_GRAPHICS_PRESENT_OCCLUDED : STATUS_SUCCESS;
    // }
    // return 0x80070000 + GetLastError(); // uhh k then
// }

HRESULT D3DKMTCreateContext(D3DKMT_CREATECONTEXT* pCreateContext) {
    D3DNTHAL_CONTEXTCREATEDATA dcci = { 0 };
	BOOL result;
	
	if (!pCreateContext || !pCreateContext->hDevice) {
        return E_INVALIDARG;
    }    

    // Prepara a estrutura D3DNTHAL_CONTEXTCREATEDATA
    dcci.lpDDLcl = (PDD_DIRECTDRAW_LOCAL)pCreateContext->hDevice;
    dcci.lpDDSLcl = (PDD_SURFACE_LOCAL)pCreateContext->pPrivateDriverData;
    dcci.dwPID = GetCurrentProcessId(); // Obtém o PID do processo atual

    // Chama a função NtGdiD3DContextCreate
    result = DdEntry1(
        (HANDLE)pCreateContext->hDevice,
        (HANDLE)pCreateContext->NodeOrdinal,  // Aqui o NodeOrdinal pode ser tratado como cor
        (HANDLE)pCreateContext->EngineAffinity, // Afinidade do Z-buffer
        &dcci
    );

    // Verifica o resultado
    if (!result || dcci.ddrval != DD_OK) {
        return E_FAIL;
    }

    // Retorna o handle do contexto criado
    pCreateContext->hContext = (D3DKMT_HANDLE)dcci.dwhContext;

    return S_OK;
}

// Implementação da função D3DKMTGetScanLine
// HRESULT D3DKMTGetScanLine(D3DKMT_GETSCANLINE* pGetScanLine) {
	// DDHAL_GETSCANLINEDATA scanlineData = { 0 };
	
    // if (!pGetScanLine || !pGetScanLine->hAdapter) {
        // return E_INVALIDARG;
    // }

    // // Prepara a estrutura para consulta do estado do scanline
    
    // scanlineData.dwSurfaceHandle = (DWORD)pGetScanLine->hDevice;

    // stateData.dwContext = (DWORD)pGetScanLine->hAdapter;
    // stateData.dwFlags = 0x200; // Flag personalizada para consulta de scanline
    // stateData.lpvData = &scanlineData;
    // stateData.ddRVal = DD_OK;

    // // Chama a função NtGdiDdGetDriverState
    // DWORD result = pNtGdiDdGetDriverState(&stateData);

    // // Libera a biblioteca
    // FreeLibrary(hGdi32);

    // // Verifica o resultado da chamada
    // if (result != DD_OK || stateData.ddRVal != DD_OK) {
        // return E_FAIL;
    // }

    // // Atualiza os valores retornados
    // pGetScanLine->InVerticalBlank = scanlineData.bInVerticalBlank;
    // pGetScanLine->ScanLine = scanlineData.dwScanLine;

    // return S_OK;
// }

NTSTATUS WINAPI D3DKMTGetScanLine(D3DKMT_GETSCANLINE *unnamedParam1) {
    if (!unnamedParam1)
        return STATUS_INVALID_PARAMETER;
    unnamedParam1->InVerticalBlank = FALSE;
    unnamedParam1->ScanLine = 405;
    return STATUS_SUCCESS;
}

NTSTATUS WINAPI D3DKMTSharedPrimaryLockNotification(
    const D3DKMT_SHAREDPRIMARYLOCKNOTIFICATION *unnamedParam1
)
{
	return STATUS_SUCCESS;
}

NTSTATUS WINAPI D3DKMTSharedPrimaryUnLockNotification(
    const D3DKMT_SHAREDPRIMARYUNLOCKNOTIFICATION *unnamedParam1
)
{
	return STATUS_SUCCESS;
}

NTSTATUS WINAPI D3DKMTReleaseProcessVidPnSourceOwners(
    HANDLE unnamedParam1
)
{
	return STATUS_SUCCESS;
}
