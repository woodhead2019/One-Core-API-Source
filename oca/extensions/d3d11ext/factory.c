/*
 * Copyright 2020 Nikolay Sivov for CodeWeavers
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
#include "main.h"
#include "dxgi.h"
#include "inspectable.h"
#include "d3dcommon.h"
#include "d3d11.h"

WINE_DEFAULT_DEBUG_CHANNEL(d3d11);

HRESULT WINAPI CreateDirect3D11DeviceFromDXGIDevice(
  IDXGIDevice  *dxgiDevice,
  IInspectable **graphicsDevice
)
{
	return E_FAIL;
}

HRESULT WINAPI CreateDirect3D11SurfaceFromDXGISurface(IDXGISurface *dgxiSurface, IInspectable **graphicsSurface)
{
  return E_NOTIMPL;
}

HRESULT WINAPI D3D11On12CreateDevice(
    IUnknown *pDevice,
    UINT Flags,
    const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    IUnknown *const *ppCommandQueues,
    UINT NumQueues,
    UINT NodeMask,
    ID3D11Device **ppDevice,
    ID3D11DeviceContext **ppImmediateContext,
    D3D_FEATURE_LEVEL *pChosenFeatureLevel)
{
  return E_NOTIMPL;
}