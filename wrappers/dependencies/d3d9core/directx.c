/*
 * IDirect3D9 implementation
 *
 * Copyright 2002 Jason Edmeades
 * Copyright 2005 Oliver Stieber
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
 */

#include "windows.h"
#include "d3d9_private.h"

WINE_DEFAULT_DEBUG_CHANNEL(d3d9);

static inline struct d3d9 *impl_from_IDirect3D9Ex(IDirect3D9Ex *iface)
{
    return CONTAINING_RECORD(iface, struct d3d9, IDirect3D9Ex_iface);
}

static HRESULT WINAPI d3d9_QueryInterface(IDirect3D9Ex *iface, REFIID riid, void **out)
{
	return S_OK;
}

static ULONG WINAPI d3d9_AddRef(IDirect3D9Ex *iface)
{
    return 0;
}

static ULONG WINAPI d3d9_Release(IDirect3D9Ex *iface)
{
    return 0;
}

static HRESULT WINAPI d3d9_RegisterSoftwareDevice(IDirect3D9Ex *iface, void *init_function)
{
	return S_OK;
}

static UINT WINAPI d3d9_GetAdapterCount(IDirect3D9Ex *iface)
{
    return 0;
}

static HRESULT WINAPI d3d9_GetAdapterIdentifier(IDirect3D9Ex *iface, UINT adapter,
        DWORD flags, D3DADAPTER_IDENTIFIER9 *identifier)
{
	return S_OK;
}

static UINT WINAPI d3d9_GetAdapterModeCount(IDirect3D9Ex *iface, UINT adapter, D3DFORMAT format)
{
    return 0;
}

static HRESULT WINAPI d3d9_EnumAdapterModes(IDirect3D9Ex *iface, UINT adapter,
        D3DFORMAT format, UINT mode_idx, D3DDISPLAYMODE *mode)
{
    return S_OK;
}

static HRESULT WINAPI d3d9_GetAdapterDisplayMode(IDirect3D9Ex *iface, UINT adapter, D3DDISPLAYMODE *mode)
{
    return S_OK;
}

static HRESULT WINAPI d3d9_CheckDeviceType(IDirect3D9Ex *iface, UINT adapter, D3DDEVTYPE device_type,
        D3DFORMAT display_format, D3DFORMAT backbuffer_format, BOOL windowed)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_CheckDeviceFormat(IDirect3D9Ex *iface, UINT adapter, D3DDEVTYPE device_type,
        D3DFORMAT adapter_format, DWORD usage, D3DRESOURCETYPE resource_type, D3DFORMAT format)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_CheckDeviceMultiSampleType(IDirect3D9Ex *iface, UINT adapter, D3DDEVTYPE device_type,
        D3DFORMAT format, BOOL windowed, D3DMULTISAMPLE_TYPE multisample_type, DWORD *levels)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_CheckDepthStencilMatch(IDirect3D9Ex *iface, UINT adapter, D3DDEVTYPE device_type,
        D3DFORMAT adapter_format, D3DFORMAT rt_format, D3DFORMAT ds_format)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_CheckDeviceFormatConversion(IDirect3D9Ex *iface, UINT adapter,
        D3DDEVTYPE device_type, D3DFORMAT src_format, D3DFORMAT dst_format)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_GetDeviceCaps(IDirect3D9Ex *iface, UINT adapter, D3DDEVTYPE device_type, D3DCAPS9 *caps)
{
	return S_OK;
}

static HMONITOR WINAPI d3d9_GetAdapterMonitor(IDirect3D9Ex *iface, UINT adapter)
{
    struct d3d9 *d3d9 = impl_from_IDirect3D9Ex(iface);
    struct wined3d_output_desc desc;
    unsigned int output_idx;
    HRESULT hr;

    TRACE("iface %p, adapter %u.\n", iface, adapter);

    output_idx = adapter;
    if (output_idx >= d3d9->wined3d_output_count)
        return NULL;

    wined3d_mutex_lock();
    hr = wined3d_output_get_desc(d3d9->wined3d_outputs[output_idx], &desc);
    wined3d_mutex_unlock();

    if (FAILED(hr))
    {
        WARN("Failed to get output desc, hr %#x.\n", hr);
        return NULL;
    }

    return desc.monitor;
}

static HRESULT WINAPI DECLSPEC_HOTPATCH d3d9_CreateDevice(IDirect3D9Ex *iface, UINT adapter,
        D3DDEVTYPE device_type, HWND focus_window, DWORD flags, D3DPRESENT_PARAMETERS *parameters,
        IDirect3DDevice9 **device)
{
	return S_OK;
}

static UINT WINAPI d3d9_GetAdapterModeCountEx(IDirect3D9Ex *iface,
        UINT adapter, const D3DDISPLAYMODEFILTER *filter)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_EnumAdapterModesEx(IDirect3D9Ex *iface,
        UINT adapter, const D3DDISPLAYMODEFILTER *filter, UINT mode_idx, D3DDISPLAYMODEEX *mode)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_GetAdapterDisplayModeEx(IDirect3D9Ex *iface,
        UINT adapter, D3DDISPLAYMODEEX *mode, D3DDISPLAYROTATION *rotation)
{
	return S_OK;
}

static HRESULT WINAPI DECLSPEC_HOTPATCH d3d9_CreateDeviceEx(IDirect3D9Ex *iface,
        UINT adapter, D3DDEVTYPE device_type, HWND focus_window, DWORD flags,
        D3DPRESENT_PARAMETERS *parameters, D3DDISPLAYMODEEX *mode, IDirect3DDevice9Ex **device)
{
	return S_OK;
}

static HRESULT WINAPI d3d9_GetAdapterLUID(IDirect3D9Ex *iface, UINT adapter, LUID *luid)
{
	return S_OK;
}

static const struct IDirect3D9ExVtbl d3d9_vtbl =
{
    /* IUnknown */
    d3d9_QueryInterface,
    d3d9_AddRef,
    d3d9_Release,
    /* IDirect3D9 */
    d3d9_RegisterSoftwareDevice,
    d3d9_GetAdapterCount,
    d3d9_GetAdapterIdentifier,
    d3d9_GetAdapterModeCount,
    d3d9_EnumAdapterModes,
    d3d9_GetAdapterDisplayMode,
    d3d9_CheckDeviceType,
    d3d9_CheckDeviceFormat,
    d3d9_CheckDeviceMultiSampleType,
    d3d9_CheckDepthStencilMatch,
    d3d9_CheckDeviceFormatConversion,
    d3d9_GetDeviceCaps,
    d3d9_GetAdapterMonitor,
    d3d9_CreateDevice,
    /* IDirect3D9Ex */
    d3d9_GetAdapterModeCountEx,
    d3d9_EnumAdapterModesEx,
    d3d9_GetAdapterDisplayModeEx,
    d3d9_CreateDeviceEx,
    d3d9_GetAdapterLUID,
};

BOOL d3d9_init(struct d3d9 *d3d9, BOOL extended)
{
    return TRUE;
}
