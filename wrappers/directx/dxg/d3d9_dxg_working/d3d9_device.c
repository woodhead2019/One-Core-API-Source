/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS ReactX
 * FILE:            dll/directx/d3d9/d3d9_device.c
 * PURPOSE:         d3d9.dll internal device methods
 * PROGRAMERS:      Gregor Brunmar <gregor (dot) brunmar (at) home (dot) se>
 */
#include "d3d9_device.h"
#include "d3d9_helpers.h"
#include "adapter.h"
#include <debug.h>
#include "d3d9_create.h"
#include "d3d9_mipmap.h"
#include <reactos/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(d3d9_device);

#define LOCK_D3DDEVICE9()     if (This->bLockDevice) EnterCriticalSection(&This->CriticalSection);
#define UNLOCK_D3DDEVICE9()   if (This->bLockDevice) LeaveCriticalSection(&This->CriticalSection);

/* Convert a IDirect3DDevice9 pointer safely to the internal implementation struct */
LPDIRECT3DDEVICE9_INT IDirect3DDevice9ToImpl(LPDIRECT3DDEVICE9EX  iface)
{
    if (NULL == iface)
        return NULL;

    return (LPDIRECT3DDEVICE9_INT)((ULONG_PTR)iface - FIELD_OFFSET(DIRECT3DDEVICE9_INT, lpVtbl));
}

// static HRESULT InvalidCall(LPDIRECT3DDEVICE9_INT This, LPSTR ErrorMsg)
// {
    // DPRINT1("%s",ErrorMsg);
    // UNLOCK_D3DDEVICE9();
    // return D3DERR_INVALIDCALL;
// }

/* IDirect3DDevice9: IUnknown implementation */
HRESULT WINAPI IDirect3DDevice9Base_QueryInterface(LPDIRECT3DDEVICE9EX  iface, REFIID riid, void** ppvObject)
{
    LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);

    if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_IDirect3DDevice9))
    {
        IUnknown_AddRef(iface);
        *ppvObject = &This->lpVtbl;
        return D3D_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
	//return IDirect3DDevice9_QueryInterface(DeviceProxyInterface, riid, ppvObject);
}

ULONG WINAPI IDirect3DDevice9Base_AddRef(LPDIRECT3DDEVICE9EX  iface)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // ULONG ref = InterlockedIncrement(&This->lRefCnt);

    // return ref;
	return IDirect3DDevice9_AddRef(DeviceProxyInterface);
}

ULONG WINAPI IDirect3DDevice9Base_Release(LPDIRECT3DDEVICE9EX  iface)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // ULONG ref = InterlockedDecrement(&This->lRefCnt);

    // if (ref == 0)
    // {
        // DWORD iAdapter;

        // EnterCriticalSection(&This->CriticalSection);
        
        // /* TODO: Free resources here */
        // for (iAdapter = 0; iAdapter < This->NumAdaptersInDevice; iAdapter++)
        // {
            // DestroyD3D9DeviceData(&This->DeviceData[iAdapter]);
        // }
        // This->lpVtbl->VirtualDestructor(iface);

        // LeaveCriticalSection(&This->CriticalSection);
        // AlignedFree(This);
    // }

    // return ref;
	return IDirect3DDevice9_Release(DeviceProxyInterface);	
}

/* IDirect3DDevice9 public interface */
HRESULT WINAPI IDirect3DDevice9Base_TestCooperativeLevel(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_TestCooperativeLevel(DeviceProxyInterface);
}

/*++
* @name IDirect3DDevice9::GetAvailableTextureMem
* @implemented
*
* The function IDirect3DDevice9Base_GetAvailableTextureMem returns a pointer to the IDirect3D9 object
* that created this device.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice()
*
* @return UINT
* The method returns an estimated the currently available texture memory in bytes rounded
* to the nearest MB. Applications should NOT use this as an exact number.
*
*/
UINT WINAPI IDirect3DDevice9Base_GetAvailableTextureMem(LPDIRECT3DDEVICE9EX  iface)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
	return IDirect3DDevice9_GetAvailableTextureMem(DeviceProxyInterface);	
}

HRESULT WINAPI IDirect3DDevice9Base_EvictManagedResources(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_EvictManagedResources(DeviceProxyInterface);
}

/*++
* @name IDirect3DDevice9::GetDirect3D
* @implemented
*
* The function IDirect3DDevice9Base_GetDirect3D returns a pointer to the IDirect3D9 object
* that created this device.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice()
*
* @param IDirect3D9** ppD3D9
* Pointer to a IDirect3D9* to receive the IDirect3D9 object pointer.
*
* @return HRESULT
* If the method successfully fills the ppD3D9 structure, the return value is D3D_OK.
* If ppD3D9 is a bad pointer, the return value will be D3DERR_INVALIDCALL.
*
*/
HRESULT WINAPI IDirect3DDevice9Base_GetDirect3D(LPDIRECT3DDEVICE9EX  iface, IDirect3D9** ppD3D9)
{
    // IDirect3D9* pDirect3D9;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // if (NULL == ppD3D9)
    // {
        // DPRINT1("Invalid ppD3D9 parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // pDirect3D9 = (IDirect3D9*)&This->pDirect3D9->lpVtbl;
    // IDirect3D9_AddRef(pDirect3D9);
    // *ppD3D9 = pDirect3D9;

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;	
	return IDirect3DDevice9_GetDirect3D(DeviceProxyInterface, ppD3D9);	
}

/*++
* @name IDirect3DDevice9::GetDeviceCaps
* @implemented
*
* The function IDirect3DDevice9Base_GetDeviceCaps fills the pCaps argument with the
* capabilities of the device.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3D9 object returned from Direct3DCreate9()
*
* @param D3DCAPS9* pCaps
* Pointer to a D3DCAPS9 structure to be filled with the device's capabilities.
*
* @return HRESULT
* If the method successfully fills the pCaps structure, the return value is D3D_OK.
* If pCaps is a bad pointer the return value will be D3DERR_INVALIDCALL.
*
*/
HRESULT WINAPI IDirect3DDevice9Base_GetDeviceCaps(LPDIRECT3DDEVICE9EX  iface, D3DCAPS9* pCaps)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // if (NULL == pCaps)
    // {
        // DPRINT1("Invalid pCaps parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // GetAdapterCaps(&This->pDirect3D9->DisplayAdapters[0], This->DeviceData[0].DeviceType, pCaps);

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;
	return IDirect3DDevice9_GetDeviceCaps(DeviceProxyInterface, pCaps);
}

/*++
* @name IDirect3DDevice9::GetDisplayMode
* @implemented
*
* The function IDirect3DDevice9Base_GetDisplayMode fills the pMode argument with the
* display mode for the specified swap chain.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3D9 object returned from Direct3DCreate9()
*
* @param UINT iSwapChain
* Swap chain index to get object for.
* The maximum value for this is the value returned by IDirect3DDevice9::GetNumberOfSwapChains() - 1.
*
* @param D3DDISPLAYMODE* pMode
* Pointer to a D3DDISPLAYMODE structure to be filled with the current swap chain's display mode information.
*
* @return HRESULT
* If the method successfully fills the pMode structure, the return value is D3D_OK.
* If iSwapChain is out of range or pMode is a bad pointer, the return value will be D3DERR_INVALIDCALL.
*
*/
HRESULT WINAPI IDirect3DDevice9Base_GetDisplayMode(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // if (iSwapChain >= IDirect3DDevice9_GetNumberOfSwapChains(iface))
    // {
        // DPRINT1("Invalid iSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // if (NULL == pMode)
    // {
        // DPRINT1("Invalid pMode parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // pMode->Width = This->DeviceData[iSwapChain].DriverCaps.dwDisplayWidth;
    // pMode->Height = This->DeviceData[iSwapChain].DriverCaps.dwDisplayHeight;
    // pMode->Format = This->DeviceData[iSwapChain].DriverCaps.RawDisplayFormat;
    // pMode->RefreshRate = This->DeviceData[iSwapChain].DriverCaps.dwRefreshRate;

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;
	return IDirect3DDevice9_GetDisplayMode(DeviceProxyInterface, iSwapChain, pMode);
}

/*++
* @name IDirect3DDevice9::GetCreationParameters
* @implemented
*
* The function IDirect3DDevice9Base_GetCreationParameters fills the pParameters argument with the
* parameters the device was created with.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3D9 object returned from Direct3DCreate9()
*
* @param D3DDEVICE_CREATION_PARAMETERS* pParameters
* Pointer to a D3DDEVICE_CREATION_PARAMETERS structure to be filled with the creation parameter
* information for this device.
*
* @return HRESULT
* If the method successfully fills the pParameters structure, the return value is D3D_OK.
* If pParameters is a bad pointer, the return value will be D3DERR_INVALIDCALL.
*
*/
HRESULT WINAPI IDirect3DDevice9Base_GetCreationParameters(LPDIRECT3DDEVICE9EX  iface, D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // if (NULL == pParameters)
    // {
        // DPRINT1("Invalid pParameters parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // pParameters->AdapterOrdinal = This->AdapterIndexInGroup[0];
    // pParameters->DeviceType = This->DeviceType;
    // pParameters->hFocusWindow = This->hWnd;
    // pParameters->BehaviorFlags = This->BehaviourFlags;

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;
	return IDirect3DDevice9_GetCreationParameters(DeviceProxyInterface, pParameters);
}

HRESULT WINAPI IDirect3DDevice9Base_SetCursorProperties(LPDIRECT3DDEVICE9EX  iface, UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
    // UNIMPLEMENTED;

    // return D3D_OK;
	return IDirect3DDevice9_SetCursorProperties(DeviceProxyInterface, XHotSpot, YHotSpot, pCursorBitmap);
}

VOID WINAPI IDirect3DDevice9Base_SetCursorPosition(LPDIRECT3DDEVICE9EX  iface, int X, int Y, DWORD Flags)
{
    IDirect3DDevice9_SetCursorPosition(DeviceProxyInterface, X, Y, Flags);
}

BOOL WINAPI IDirect3DDevice9Base_ShowCursor(LPDIRECT3DDEVICE9EX  iface, BOOL bShow)
{
	return IDirect3DDevice9_ShowCursor(DeviceProxyInterface, bShow);
}

/*++
* @name IDirect3DDevice9::CreateAdditionalSwapChain
* @implemented
*
* The function IDirect3DDevice9Base_CreateAdditionalSwapChain creates a swap chain object,
* useful when rendering multiple views.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice()
*
* @param D3DPRESENT_PARAMETERS* pPresentationParameters
* Pointer to a D3DPRESENT_PARAMETERS structure describing the parameters for the swap chain
* to be created.
*
* @param IDirect3DSwapChain9** ppSwapChain
* Pointer to a IDirect3DSwapChain9* to receive the swap chain object pointer.
*
* @return HRESULT
* If the method successfully fills the ppSwapChain structure, the return value is D3D_OK.
* If iSwapChain is out of range or ppSwapChain is a bad pointer, the return value
* will be D3DERR_INVALIDCALL. Also D3DERR_OUTOFVIDEOMEMORY can be returned if allocation
* of the new swap chain object failed.
*
*/
HRESULT WINAPI IDirect3DDevice9Base_CreateAdditionalSwapChain(LPDIRECT3DDEVICE9EX  iface, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** ppSwapChain)
{
    // UINT iSwapChain;
    // IDirect3DSwapChain9* pSwapChain;
    // Direct3DSwapChain9_INT* pSwapChain_INT;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // if (NULL == ppSwapChain)
    // {
        // DPRINT1("Invalid ppSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // *ppSwapChain = NULL;
    // iSwapChain = IDirect3DDevice9_GetNumberOfSwapChains(iface) + 1;

    // pSwapChain_INT = CreateDirect3DSwapChain9(RT_EXTERNAL, This, iSwapChain);
    // if (NULL == pSwapChain_INT)
    // {
        // DPRINT1("Out of memory");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_OUTOFVIDEOMEMORY;
    // }

    // Direct3DSwapChain9_Init(pSwapChain_INT, pPresentationParameters);

    // This->pSwapChains[iSwapChain] = pSwapChain_INT;
    // pSwapChain = (IDirect3DSwapChain9*)&pSwapChain_INT->lpVtbl;
    // IDirect3DSwapChain9_AddRef(pSwapChain);
    // *ppSwapChain = pSwapChain;

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;
	return IDirect3DDevice9_CreateAdditionalSwapChain(DeviceProxyInterface, pPresentationParameters, ppSwapChain);
}

HRESULT CreateD3D9SwapChainEx(OUT LPDIRECT3DSWAPCHAIN9EX *ppDirect3D9SwapChain)
{
    LPDIRECT3DSWAPCHAIN9_INT pThisBaseDevice;
	
    if (ppDirect3D9SwapChain == 0)
        return DDERR_INVALIDPARAMS;

    if (AlignedAlloc((LPVOID *)&pThisBaseDevice, sizeof(DIRECT3DDEVICE9_INT)) != S_OK)
        return DDERR_OUTOFMEMORY;

    if (pThisBaseDevice == 0)
        return DDERR_OUTOFMEMORY;	

    pThisBaseDevice->lpVtbl = &Direct3DSwapChain9Ex_Vtbl;
    //pThisBaseDevice->dwProcessId = GetCurrentThreadId();
    //pThisBaseDevice->lRefCnt = 1;

    //InitializeCriticalSection(&pThisBaseDevice->CriticalSection);

    *ppDirect3D9SwapChain = (LPDIRECT3DSWAPCHAIN9EX)&pThisBaseDevice->lpVtbl;

    return D3D_OK;
}

/*++
* @name IDirect3DDevice9::GetSwapChain
* @implemented
*
* The function IDirect3DDevice9Base_GetSwapChain returns a pointer to a swap chain object.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice()
*
* @param UINT iSwapChain
* Swap chain index to get object for.
* The maximum value for this is the value returned by IDirect3DDevice9::GetNumberOfSwapChains() - 1.
*
* @param IDirect3DSwapChain9** ppSwapChain
* Pointer to a IDirect3DSwapChain9* to receive the swap chain object pointer.
*
* @return HRESULT
* If the method successfully fills the ppSwapChain structure, the return value is D3D_OK.
* If iSwapChain is out of range or ppSwapChain is a bad pointer, the return value
* will be D3DERR_INVALIDCALL.
*
*/
HRESULT WINAPI IDirect3DDevice9Base_GetSwapChain(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, IDirect3DSwapChain9** ppSwapChain)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // if (NULL == ppSwapChain)
    // {
        // DPRINT1("Invalid ppSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // *ppSwapChain = NULL;

    // if (iSwapChain >= IDirect3DDevice9_GetNumberOfSwapChains(iface))
    // {
        // DPRINT1("Invalid iSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // if (This->pSwapChains[iSwapChain] != NULL)
    // {
        // IDirect3DSwapChain9* pSwapChain = (IDirect3DSwapChain9*)&This->pSwapChains[iSwapChain]->lpVtbl;
        // IDirect3DSwapChain9_AddRef(pSwapChain);
        // *ppSwapChain = pSwapChain;
    // }
    // else
    // {
        // *ppSwapChain = NULL;
    // }

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;
    HRESULT Ret;
	
	Ret = IDirect3DDevice9_GetSwapChain(DeviceProxyInterface, iSwapChain, &SwapChainProxyInterface);

	// if(Ret == D3D_OK){
       // Ret = CreateD3D9SwapChainEx((LPDIRECT3DSWAPCHAIN9EX*)ppSwapChain);
	// }

    return Ret;	
}

/*++
* @name IDirect3DDevice9::GetNumberOfSwapChains
* @implemented
*
* The function IDirect3DDevice9Base_GetNumberOfSwapChains returns the number of swap chains
* created by IDirect3D9::CreateDevice().
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice().
*
* @return UINT
* Returns the number of swap chains created by IDirect3D9::CreateDevice().
*
* NOTE: An application can create additional swap chains using the
*       IDirect3DDevice9::CreateAdditionalSwapChain() method.
*
*/
UINT WINAPI IDirect3DDevice9Base_GetNumberOfSwapChains(LPDIRECT3DDEVICE9EX  iface)
{
    // UINT NumSwapChains;

    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // NumSwapChains = This->NumAdaptersInDevice;

    // UNLOCK_D3DDEVICE9();
    // return NumSwapChains;
	return IDirect3DDevice9_GetNumberOfSwapChains(DeviceProxyInterface);
}

HRESULT WINAPI IDirect3DDevice9Base_Reset(LPDIRECT3DDEVICE9EX  iface, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    return IDirect3DDevice9_Reset(DeviceProxyInterface, pPresentationParameters);
}

/*++
* @name IDirect3DDevice9::Present
* @implemented
*
* The function IDirect3DDevice9Base_Present displays the content of the next
* back buffer in sequence for the device.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice().
*
* @param CONST RECT* pSourceRect
* A pointer to a RECT structure representing an area of the back buffer to display where
* NULL means the whole back buffer. This parameter MUST be NULL unless the back buffer
* was created with the D3DSWAPEFFECT_COPY flag.
*
* @param CONST RECT* pDestRect
* A pointer to a RECT structure representing an area of the back buffer where the content
* will be displayed where NULL means the whole back buffer starting at (0,0).
* This parameter MUST be NULL unless the back buffer was created with the D3DSWAPEFFECT_COPY flag.
*
* @param HWND hDestWindowOverride
* A destination window where NULL means the window specified in the hWndDeviceWindow of the
* D3DPRESENT_PARAMETERS structure.
*
* @param CONST RGNDATA* pDirtyRegion
* A pointer to a RGNDATA structure representing an area of the back buffer to display where
* NULL means the whole back buffer. This parameter MUST be NULL unless the back buffer
* was created with the D3DSWAPEFFECT_COPY flag. This is an optimization region only.
*
* @return HRESULT
* If the method successfully displays the back buffer content, the return value is D3D_OK.
* If no swap chains are available, the return value will be D3DERR_INVALIDCALL.
*/
HRESULT WINAPI IDirect3DDevice9Base_Present(LPDIRECT3DDEVICE9EX  iface, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    // UINT i;
    // UINT iNumSwapChains;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // iNumSwapChains = IDirect3DDevice9Base_GetNumberOfSwapChains(iface);
    // if (0 == iNumSwapChains)
    // {
        // DPRINT1("Not enough swap chains, Present() fails");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // for (i = 0; i < iNumSwapChains; i++)
    // {
        // HRESULT hResult;
        // IDirect3DSwapChain9* pSwapChain;
        
        // IDirect3DDevice9Base_GetSwapChain(iface, i, &pSwapChain);
        // hResult = IDirect3DSwapChain9_Present(pSwapChain, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, 0);

        // if (FAILED(hResult))
        // {
            // UNLOCK_D3DDEVICE9();
            // return hResult;
        // }
    // }

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;
	return IDirect3DDevice9_Present(DeviceProxyInterface, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

/*++
* @name IDirect3DDevice9::GetBackBuffer
* @implemented
*
* The function IDirect3DDevice9Base_GetBackBuffer retrieves the back buffer
* for the specified swap chain.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice().
*
* @param UINT iSwapChain
* Swap chain index to get object for.
* The maximum value for this is the value returned by IDirect3DDevice9::GetNumberOfSwapChains() - 1.
*
* @param UINT iBackBuffer
* Back buffer index to get object for.
* The maximum value for this is the the total number of back buffers - 1, as indexing starts at 0.
*
* @param IDirect3DSurface9** ppBackBuffer
* Pointer to a IDirect3DSurface9* to receive the back buffer object
*
* @return HRESULT
* If the method successfully sets the ppBackBuffer pointer, the return value is D3D_OK.
* If iSwapChain or iBackBuffer is out of range, Type is invalid or ppBackBuffer is a bad pointer,
* the return value will be D3DERR_INVALIDCALL.
*/
HRESULT WINAPI IDirect3DDevice9Base_GetBackBuffer(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
    // HRESULT hResult;
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Base_GetSwapChain(iface, iSwapChain, &pSwapChain);
    // if (NULL == pSwapChain)
    // {
        // DPRINT1("Invalid iSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // if (NULL == ppBackBuffer)
    // {
        // DPRINT1("Invalid ppBackBuffer parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // hResult = IDirect3DSwapChain9_GetBackBuffer(pSwapChain, iBackBuffer, Type, ppBackBuffer);

    // UNLOCK_D3DDEVICE9();
    // return hResult;
	return IDirect3DDevice9_GetBackBuffer(DeviceProxyInterface, iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

/*++
* @name IDirect3DDevice9::GetRasterStatus
* @implemented
*
* The function IDirect3DDevice9Base_GetRasterStatus retrieves raster information
* of the monitor for the specified swap chain.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice().
*
* @param UINT iSwapChain
* Swap chain index to get object for.
* The maximum value for this is the value returned by IDirect3DDevice9::GetNumberOfSwapChains() - 1.
*
* @param D3DRASTER_STATUS* pRasterStatus
* Pointer to a D3DRASTER_STATUS to receive the raster information
*
* @return HRESULT
* If the method successfully fills the pRasterStatus structure, the return value is D3D_OK.
* If iSwapChain is out of range or pRasterStatus is a bad pointer, the return value
* will be D3DERR_INVALIDCALL.
*/
HRESULT WINAPI IDirect3DDevice9Base_GetRasterStatus(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
    // HRESULT hResult;
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Base_GetSwapChain(iface, iSwapChain, &pSwapChain);
    // if (NULL == pSwapChain)
    // {
        // DPRINT1("Invalid iSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // if (NULL == pRasterStatus)
    // {
        // DPRINT1("Invalid pRasterStatus parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // hResult = IDirect3DSwapChain9_GetRasterStatus(pSwapChain, pRasterStatus);

    // UNLOCK_D3DDEVICE9();
    // return hResult;
	return IDirect3DDevice9_GetRasterStatus(DeviceProxyInterface, iSwapChain, pRasterStatus);
}

HRESULT WINAPI IDirect3DDevice9Base_SetDialogBoxMode(LPDIRECT3DDEVICE9EX  iface, BOOL bEnableDialogs)
{
    return IDirect3DDevice9_SetDialogBoxMode(DeviceProxyInterface, bEnableDialogs);
}

/*++
* @name IDirect3DDevice9::SetGammaRamp
* @implemented
*
* The function IDirect3DDevice9Base_SetGammaRamp sets the gamma correction ramp values
* for the specified swap chain.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice().
*
* @param UINT iSwapChain
* Swap chain index to get object for.
* The maximum value for this is the value returned by IDirect3DDevice9::GetNumberOfSwapChains() - 1.
*
* @param UINT Flags
* Can be on of the following:
* D3DSGR_CALIBRATE - Detects if a gamma calibrator is installed and if so modifies the values to correspond to
*                    the monitor and system settings before sending them to the display device.
* D3DSGR_NO_CALIBRATION - The gamma calibrations values are sent directly to the display device without
*                         any modification.
*
* @param CONST D3DGAMMARAMP* pRamp
* Pointer to a D3DGAMMARAMP representing the gamma correction ramp values to be set.
*
*/
VOID WINAPI IDirect3DDevice9Base_SetGammaRamp(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	IDirect3DDevice9_SetGammaRamp(DeviceProxyInterface, iSwapChain, Flags, pRamp);
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // Direct3DSwapChain9_INT* pSwapChain_INT;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Base_GetSwapChain(iface, iSwapChain, &pSwapChain);
    // if (NULL == pSwapChain)
    // {
        // DPRINT1("Invalid iSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return;
    // }

    // if (NULL == pRamp)
    // {
        // DPRINT1("Invalid pRamp parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return;
    // }

    // pSwapChain_INT = IDirect3DSwapChain9ToImpl(pSwapChain);
    // Direct3DSwapChain9_SetGammaRamp(pSwapChain_INT, Flags, pRamp);

    // UNLOCK_D3DDEVICE9();
}

/*++
* @name IDirect3DDevice9::GetGammaRamp
* @implemented
*
* The function IDirect3DDevice9Base_GetGammaRamp retrieves the gamma correction ramp values
* for the specified swap chain.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice().
*
* @param UINT iSwapChain
* Swap chain index to get object for.
* The maximum value for this is the value returned by IDirect3DDevice9::GetNumberOfSwapChains() - 1.
*
* @param D3DGAMMARAMP* pRamp
* Pointer to a D3DGAMMARAMP to receive the gamma correction ramp values.
*
*/
VOID WINAPI IDirect3DDevice9Base_GetGammaRamp(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // Direct3DSwapChain9_INT* pSwapChain_INT;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Base_GetSwapChain(iface, iSwapChain, &pSwapChain);
    // if (NULL == pSwapChain)
    // {
        // DPRINT1("Invalid iSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return;
    // }

    // if (NULL == pRamp)
    // {
        // DPRINT1("Invalid pRamp parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return;
    // }

    // pSwapChain_INT = IDirect3DSwapChain9ToImpl(pSwapChain);
    // Direct3DSwapChain9_GetGammaRamp(pSwapChain_INT, pRamp);

    // UNLOCK_D3DDEVICE9();
	IDirect3DDevice9_GetGammaRamp(DeviceProxyInterface, iSwapChain, pRamp);
}

/*++
* @name IDirect3DDevice9::CreateTexture
* @implemented
*
* The function IDirect3DDevice9Base_CreateTexture creates a D3D9 texture.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice()
*
* @param UINT Width
* Desired width of the texture
*
* @param UINT Height
* Desired height of the texture
*
* @param UINT Levels
* Number of mip-maps. If Levels are zero, mip-maps down to size 1x1 will be generated.
*
* @param DWORD Usage
* Valid combinations of the D3DUSAGE constants.
*
* @param D3DFORMAT Format
* One of the D3DFORMAT enum members for the surface format.
*
* @param D3DPOOL Pool
* One of the D3DPOOL enum members for where the texture should be placed.
*
* @param IDirect3DTexture9** ppTexture
* Return parameter for the created texture
*
* @param HANDLE* pSharedHandle
* Set to NULL, shared resources are not implemented yet
*
* @return HRESULT
* Returns D3D_OK if everything went well.
*
*/
HRESULT WINAPI IDirect3DDevice9Base_CreateTexture(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
    // HRESULT hResult;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // if (NULL == This)
        // return InvalidCall(This, "Invalid 'this' parameter specified");

    // if (NULL == ppTexture)
        // return InvalidCall(This, "Invalid ppTexture parameter specified");

    // *ppTexture = NULL;

    // if (D3DFMT_UNKNOWN == Format)
        // return InvalidCall(This, "Invalid Format parameter specified, D3DFMT_UNKNOWN is not a valid Format");

    // if (NULL != pSharedHandle)
    // {
        // UNIMPLEMENTED;;
        // return InvalidCall(This, "Invalid pSharedHandle parameter specified, only NULL is supported at the moment");
    // }

    // hResult = CreateD3D9MipMap(This, Width, Height, Levels, Usage, Format, Pool, ppTexture);
    // if (FAILED(hResult))
        // DPRINT1("Failed to create texture");

    // UNLOCK_D3DDEVICE9();
    // return hResult;
	return IDirect3DDevice9_CreateTexture(DeviceProxyInterface, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT WINAPI IDirect3DDevice9Base_CreateVolumeTexture(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
    return IDirect3DDevice9_CreateVolumeTexture(DeviceProxyInterface, Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT WINAPI IDirect3DDevice9Base_CreateCubeTexture(LPDIRECT3DDEVICE9EX  iface, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
    return IDirect3DDevice9_CreateCubeTexture(DeviceProxyInterface, EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT WINAPI IDirect3DDevice9Base_CreateVertexBuffer(LPDIRECT3DDEVICE9EX  iface, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
    return IDirect3DDevice9_CreateVertexBuffer(DeviceProxyInterface, Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT WINAPI IDirect3DDevice9Base_CreateIndexBuffer(LPDIRECT3DDEVICE9EX  iface, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
    return IDirect3DDevice9_CreateIndexBuffer(DeviceProxyInterface, Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT WINAPI IDirect3DDevice9Base_CreateRenderTarget(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return IDirect3DDevice9_CreateRenderTarget(DeviceProxyInterface, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT WINAPI IDirect3DDevice9Base_CreateDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return IDirect3DDevice9_CreateDepthStencilSurface(DeviceProxyInterface, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT WINAPI IDirect3DDevice9Base_UpdateSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
    return IDirect3DDevice9_UpdateSurface(DeviceProxyInterface, pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT WINAPI IDirect3DDevice9Base_UpdateTexture(LPDIRECT3DDEVICE9EX  iface, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
    return IDirect3DDevice9_UpdateTexture(DeviceProxyInterface, pSourceTexture, pDestinationTexture);
}

HRESULT WINAPI IDirect3DDevice9Base_GetRenderTargetData(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
    return IDirect3DDevice9_GetRenderTargetData(DeviceProxyInterface, pRenderTarget, pDestSurface);
}

/*++
* @name IDirect3DDevice9::GetFrontBufferData
* @implemented
*
* The function IDirect3DDevice9Base_GetFrontBufferData copies the content of
* the display device's front buffer in a system memory surface buffer.
*
* @param LPDIRECT3D iface
* Pointer to the IDirect3DDevice9 object returned from IDirect3D9::CreateDevice().
*
* @param UINT iSwapChain
* Swap chain index to get object for.
* The maximum value for this is the value returned by IDirect3DDevice9::GetNumberOfSwapChains() - 1.
*
* @param IDirect3DSurface9* pDestSurface
* Pointer to a IDirect3DSurface9 to receive front buffer content
*
* @return HRESULT
* If the method successfully fills the pDestSurface buffer, the return value is D3D_OK.
* If iSwapChain is out of range or pDestSurface is a bad pointer, the return value
* will be D3DERR_INVALIDCALL.
*/
HRESULT WINAPI IDirect3DDevice9Base_GetFrontBufferData(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
    // HRESULT hResult;
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Base_GetSwapChain(iface, iSwapChain, &pSwapChain);
    // if (NULL == pSwapChain)
    // {
        // DPRINT1("Invalid iSwapChain parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // if (NULL == pDestSurface)
    // {
        // DPRINT1("Invalid pDestSurface parameter specified");
        // UNLOCK_D3DDEVICE9();
        // return D3DERR_INVALIDCALL;
    // }

    // hResult = IDirect3DSwapChain9_GetFrontBufferData(pSwapChain, pDestSurface);

    // UNLOCK_D3DDEVICE9();
    // return hResult;
	return IDirect3DDevice9_GetFrontBufferData(DeviceProxyInterface, iSwapChain, pDestSurface);
}

HRESULT WINAPI IDirect3DDevice9Base_StretchRect(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
    return IDirect3DDevice9_StretchRect(DeviceProxyInterface, pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT WINAPI IDirect3DDevice9Base_ColorFill(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
    return IDirect3DDevice9_ColorFill(DeviceProxyInterface, pSurface, pRect, color);
}

HRESULT WINAPI IDirect3DDevice9Base_CreateOffscreenPlainSurface(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return IDirect3DDevice9_CreateOffscreenPlainSurface(DeviceProxyInterface, Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

/* Convert a IDirect3D9 pointer safely to the internal implementation struct */
/*static LPD3D9HALDEVICE IDirect3DDevice9ToImpl(LPDIRECT3DDEVICE9EX  iface)
{
    if (NULL == iface)
        return NULL;

    return (LPD3D9HALDEVICE)((ULONG_PTR)iface - FIELD_OFFSET(D3D9HALDEVICE, PureDevice.BaseDevice.lpVtbl));
}*/

/* IDirect3DDevice9 public interface */
HRESULT WINAPI IDirect3DDevice9HAL_GetTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
    return IDirect3DDevice9_GetTransform(DeviceProxyInterface, State, pMatrix);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetMaterial(LPDIRECT3DDEVICE9EX  iface, D3DMATERIAL9* pMaterial)
{
    return IDirect3DDevice9_GetMaterial(DeviceProxyInterface, pMaterial);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetLight(LPDIRECT3DDEVICE9EX  iface, DWORD Index, D3DLIGHT9* pLight)
{
    return IDirect3DDevice9_GetLight(DeviceProxyInterface, Index, pLight);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetLightEnable(LPDIRECT3DDEVICE9EX  iface, DWORD Index, BOOL* pEnable)
{
    return IDirect3DDevice9_GetLightEnable(DeviceProxyInterface, Index, pEnable);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetClipPlane(LPDIRECT3DDEVICE9EX  iface, DWORD Index, float* pPlane)
{
    return IDirect3DDevice9_GetClipPlane(DeviceProxyInterface, Index, pPlane);
}

HRESULT WINAPI IDirect3DDevice9HAL_SetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD Value)
{
    return IDirect3DDevice9_SetRenderState(DeviceProxyInterface, State, Value);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD* pValue)
{
    return IDirect3DDevice9_GetRenderState(DeviceProxyInterface, State, pValue);
}

HRESULT WINAPI IDirect3DDevice9HAL_SetClipStatus(LPDIRECT3DDEVICE9EX  iface, CONST D3DCLIPSTATUS9* pClipStatus)
{
    return IDirect3DDevice9_SetClipStatus(DeviceProxyInterface, pClipStatus);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetClipStatus(LPDIRECT3DDEVICE9EX  iface, D3DCLIPSTATUS9* pClipStatus)
{
    return IDirect3DDevice9_GetClipStatus(DeviceProxyInterface, pClipStatus);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetTextureStageState(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
    return IDirect3DDevice9_GetTextureStageState(DeviceProxyInterface, Stage, Type, pValue);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetSamplerState(LPDIRECT3DDEVICE9EX  iface, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
    return IDirect3DDevice9_GetSamplerState(DeviceProxyInterface, Sampler, Type, pValue);
}

HRESULT WINAPI IDirect3DDevice9HAL_ValidateDevice(LPDIRECT3DDEVICE9EX  iface, DWORD* pNumPasses)
{
    return IDirect3DDevice9_ValidateDevice(DeviceProxyInterface, pNumPasses);
}

HRESULT WINAPI IDirect3DDevice9HAL_SetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface, BOOL bSoftware)
{
    return IDirect3DDevice9_SetSoftwareVertexProcessing(DeviceProxyInterface, bSoftware);
}

BOOL WINAPI IDirect3DDevice9HAL_GetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_GetSoftwareVertexProcessing(DeviceProxyInterface);
}

HRESULT WINAPI IDirect3DDevice9HAL_ProcessVertices(LPDIRECT3DDEVICE9EX  iface, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
    return IDirect3DDevice9_ProcessVertices(DeviceProxyInterface, SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetVertexShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexShader9** ppShader)
{
    return IDirect3DDevice9_GetVertexShader(DeviceProxyInterface, ppShader);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetPixelShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DPixelShader9** ppShader)
{
    return IDirect3DDevice9_GetPixelShader(DeviceProxyInterface, ppShader);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetPixelShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return IDirect3DDevice9_GetPixelShaderConstantF(DeviceProxyInterface, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetPixelShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return IDirect3DDevice9_GetPixelShaderConstantI(DeviceProxyInterface, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9HAL_GetPixelShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return IDirect3DDevice9_GetPixelShaderConstantB(DeviceProxyInterface, StartRegister, pConstantData, BoolCount);
}

/* IDirect3DDevice9 public interface */
HRESULT WINAPI IDirect3DDevice9Pure_SetRenderTarget(LPDIRECT3DDEVICE9EX  iface, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
    return IDirect3DDevice9_SetRenderTarget(DeviceProxyInterface, RenderTargetIndex, pRenderTarget);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetRenderTarget(LPDIRECT3DDEVICE9EX  iface, DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget)
{
    return IDirect3DDevice9_GetRenderTarget(DeviceProxyInterface, RenderTargetIndex, ppRenderTarget);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pNewZStencil)
{
    return IDirect3DDevice9_SetDepthStencilSurface(DeviceProxyInterface, pNewZStencil);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9** ppZStencilSurface)
{
    return IDirect3DDevice9_GetDepthStencilSurface(DeviceProxyInterface, ppZStencilSurface);
}

HRESULT WINAPI IDirect3DDevice9Pure_BeginScene(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_BeginScene(DeviceProxyInterface);
}

HRESULT WINAPI IDirect3DDevice9Pure_EndScene(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_EndScene(DeviceProxyInterface);
}

HRESULT WINAPI IDirect3DDevice9Pure_Clear(LPDIRECT3DDEVICE9EX  iface, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    return IDirect3DDevice9_Clear(DeviceProxyInterface, Count, pRects, Flags, Color, Z, Stencil);
} 

HRESULT WINAPI IDirect3DDevice9Pure_SetTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
    return IDirect3DDevice9_SetTransform(DeviceProxyInterface, State, pMatrix);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
    return IDirect3DDevice9_GetTransform(DeviceProxyInterface, State, pMatrix);
}

HRESULT WINAPI IDirect3DDevice9Pure_MultiplyTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
    return IDirect3DDevice9_MultiplyTransform(DeviceProxyInterface, State, pMatrix);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetViewport(LPDIRECT3DDEVICE9EX  iface, CONST D3DVIEWPORT9* pViewport)
{
    return IDirect3DDevice9_SetViewport(DeviceProxyInterface, pViewport);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetViewport(LPDIRECT3DDEVICE9EX  iface, D3DVIEWPORT9* pViewport)
{
    return IDirect3DDevice9_GetViewport(DeviceProxyInterface, pViewport);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetMaterial(LPDIRECT3DDEVICE9EX  iface, CONST D3DMATERIAL9* pMaterial)
{
    return IDirect3DDevice9_SetMaterial(DeviceProxyInterface, pMaterial);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetMaterial(LPDIRECT3DDEVICE9EX  iface, D3DMATERIAL9* pMaterial)
{
    return IDirect3DDevice9_GetMaterial(DeviceProxyInterface, pMaterial);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetLight(LPDIRECT3DDEVICE9EX  iface, DWORD Index, CONST D3DLIGHT9* pLight)
{
    return IDirect3DDevice9_SetLight(DeviceProxyInterface, Index, pLight);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetLight(LPDIRECT3DDEVICE9EX  iface, DWORD Index, D3DLIGHT9* pLight)
{
    return IDirect3DDevice9_GetLight(DeviceProxyInterface, Index, pLight);
}

HRESULT WINAPI IDirect3DDevice9Pure_LightEnable(LPDIRECT3DDEVICE9EX  iface, DWORD Index, BOOL Enable)
{
    return IDirect3DDevice9_LightEnable(DeviceProxyInterface, Index, Enable);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetLightEnable(LPDIRECT3DDEVICE9EX  iface, DWORD Index, BOOL* pEnable)
{
    return IDirect3DDevice9_GetLightEnable(DeviceProxyInterface, Index, pEnable);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetClipPlane(LPDIRECT3DDEVICE9EX  iface, DWORD Index, CONST float* pPlane)
{
    return IDirect3DDevice9_SetClipPlane(DeviceProxyInterface, Index, pPlane);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetClipPlane(LPDIRECT3DDEVICE9EX  iface, DWORD Index, float* pPlane)
{
    return IDirect3DDevice9_GetClipPlane(DeviceProxyInterface, Index, pPlane);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD Value)
{
    return IDirect3DDevice9_SetRenderState(DeviceProxyInterface, State, Value);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD* pValue)
{
    return IDirect3DDevice9_GetRenderState(DeviceProxyInterface, State, pValue);
}

HRESULT WINAPI IDirect3DDevice9Pure_CreateStateBlock(LPDIRECT3DDEVICE9EX  iface, D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
    return IDirect3DDevice9_CreateStateBlock(DeviceProxyInterface, Type, ppSB);
}

HRESULT WINAPI IDirect3DDevice9Pure_BeginStateBlock(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_BeginStateBlock(DeviceProxyInterface);
}

HRESULT WINAPI IDirect3DDevice9Pure_EndStateBlock(LPDIRECT3DDEVICE9EX  iface, IDirect3DStateBlock9** ppSB)
{
    return IDirect3DDevice9_EndStateBlock(DeviceProxyInterface, ppSB);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetClipStatus(LPDIRECT3DDEVICE9EX  iface, CONST D3DCLIPSTATUS9* pClipStatus)
{
    return IDirect3DDevice9_SetClipStatus(DeviceProxyInterface, pClipStatus);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetClipStatus(LPDIRECT3DDEVICE9EX  iface, D3DCLIPSTATUS9* pClipStatus)
{
    return IDirect3DDevice9_GetClipStatus(DeviceProxyInterface, pClipStatus);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetTexture(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
    return IDirect3DDevice9_GetTexture(DeviceProxyInterface, Stage, ppTexture);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetTexture(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
    return IDirect3DDevice9_SetTexture(DeviceProxyInterface, Stage, pTexture);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetTextureStageState(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
    return IDirect3DDevice9_GetTextureStageState(DeviceProxyInterface, Stage, Type, pValue);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetTextureStageState(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    return IDirect3DDevice9_SetTextureStageState(DeviceProxyInterface, Stage, Type, Value);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetSamplerState(LPDIRECT3DDEVICE9EX  iface, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
    return IDirect3DDevice9_GetSamplerState(DeviceProxyInterface, Sampler, Type, pValue);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetSamplerState(LPDIRECT3DDEVICE9EX  iface, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    return IDirect3DDevice9_SetSamplerState(DeviceProxyInterface, Sampler, Type, Value);
}

HRESULT WINAPI IDirect3DDevice9Pure_ValidateDevice(LPDIRECT3DDEVICE9EX  iface, DWORD* pNumPasses)
{
    return IDirect3DDevice9_ValidateDevice(DeviceProxyInterface, pNumPasses);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetPaletteEntries(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
    return IDirect3DDevice9_SetPaletteEntries(DeviceProxyInterface, PaletteNumber, pEntries);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetPaletteEntries(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber, PALETTEENTRY* pEntries)
{
    return IDirect3DDevice9_GetPaletteEntries(DeviceProxyInterface, PaletteNumber, pEntries);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetCurrentTexturePalette(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber)
{
    return IDirect3DDevice9_SetCurrentTexturePalette(DeviceProxyInterface, PaletteNumber);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetCurrentTexturePalette(LPDIRECT3DDEVICE9EX  iface, UINT* pPaletteNumber)
{
    return IDirect3DDevice9_GetCurrentTexturePalette(DeviceProxyInterface, pPaletteNumber);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetScissorRect(LPDIRECT3DDEVICE9EX  iface, CONST RECT* pRect)
{
    return IDirect3DDevice9_SetScissorRect(DeviceProxyInterface, pRect);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetScissorRect(LPDIRECT3DDEVICE9EX  iface, RECT* pRect)
{
    return IDirect3DDevice9_GetScissorRect(DeviceProxyInterface, pRect);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface, BOOL bSoftware)
{
    return IDirect3DDevice9_SetSoftwareVertexProcessing(DeviceProxyInterface, bSoftware);
}

BOOL WINAPI IDirect3DDevice9Pure_GetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_GetSoftwareVertexProcessing(DeviceProxyInterface);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetNPatchMode(LPDIRECT3DDEVICE9EX  iface, float nSegments)
{
    return IDirect3DDevice9_SetNPatchMode(DeviceProxyInterface, nSegments);
}

float WINAPI IDirect3DDevice9Pure_GetNPatchMode(LPDIRECT3DDEVICE9EX  iface)
{
    return IDirect3DDevice9_GetNPatchMode(DeviceProxyInterface);
}

HRESULT WINAPI IDirect3DDevice9Pure_DrawPrimitive(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    return IDirect3DDevice9_DrawPrimitive(DeviceProxyInterface, PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_DrawIndexedPrimitive(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    return IDirect3DDevice9_DrawIndexedPrimitive(DeviceProxyInterface, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_DrawPrimitiveUP(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return IDirect3DDevice9_DrawPrimitiveUP(DeviceProxyInterface, PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT WINAPI IDirect3DDevice9Pure_DrawIndexedPrimitiveUP(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return IDirect3DDevice9_DrawIndexedPrimitiveUP(DeviceProxyInterface, PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT WINAPI IDirect3DDevice9Pure_ProcessVertices(LPDIRECT3DDEVICE9EX  iface, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
    return IDirect3DDevice9_ProcessVertices(DeviceProxyInterface, SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT WINAPI IDirect3DDevice9Pure_CreateVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
    return IDirect3DDevice9_CreateVertexDeclaration(DeviceProxyInterface, pVertexElements, ppDecl);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexDeclaration9* pDecl)
{
    return IDirect3DDevice9_SetVertexDeclaration(DeviceProxyInterface, pDecl);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexDeclaration9** ppDecl)
{
    return IDirect3DDevice9_GetVertexDeclaration(DeviceProxyInterface, ppDecl);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetFVF(LPDIRECT3DDEVICE9EX  iface, DWORD FVF)
{
    return IDirect3DDevice9_SetFVF(DeviceProxyInterface, FVF);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetFVF(LPDIRECT3DDEVICE9EX  iface, DWORD* pFVF)
{
    return IDirect3DDevice9_GetFVF(DeviceProxyInterface, pFVF);
}

HRESULT WINAPI IDirect3DDevice9Pure_CreateVertexShader(LPDIRECT3DDEVICE9EX  iface, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
    return IDirect3DDevice9_CreateVertexShader(DeviceProxyInterface, pFunction, ppShader);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexShader9* pShader)
{
    return IDirect3DDevice9_SetVertexShader(DeviceProxyInterface, pShader);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexShader9** ppShader)
{
    return IDirect3DDevice9_GetVertexShader(DeviceProxyInterface, ppShader);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return IDirect3DDevice9_SetVertexShaderConstantF(DeviceProxyInterface, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return IDirect3DDevice9_GetVertexShaderConstantF(DeviceProxyInterface, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return IDirect3DDevice9_SetVertexShaderConstantI(DeviceProxyInterface, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return IDirect3DDevice9_GetVertexShaderConstantI(DeviceProxyInterface, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    return IDirect3DDevice9_SetVertexShaderConstantB(DeviceProxyInterface, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return IDirect3DDevice9_GetVertexShaderConstantB(DeviceProxyInterface, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetStreamSource(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    return IDirect3DDevice9_SetStreamSource(DeviceProxyInterface, StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetStreamSource(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
    return IDirect3DDevice9_GetStreamSource(DeviceProxyInterface, StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetStreamSourceFreq(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber,UINT Setting)
{
    return IDirect3DDevice9_SetStreamSourceFreq(DeviceProxyInterface, StreamNumber, Setting);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetStreamSourceFreq(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, UINT* pSetting)
{
    return IDirect3DDevice9_GetStreamSourceFreq(DeviceProxyInterface, StreamNumber, pSetting);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetIndices(LPDIRECT3DDEVICE9EX  iface, IDirect3DIndexBuffer9* pIndexData)
{
    return IDirect3DDevice9_SetIndices(DeviceProxyInterface, pIndexData);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetIndices(LPDIRECT3DDEVICE9EX  iface, IDirect3DIndexBuffer9** ppIndexData)
{
    return IDirect3DDevice9_GetIndices(DeviceProxyInterface, ppIndexData);
}

HRESULT WINAPI IDirect3DDevice9Pure_CreatePixelShader(LPDIRECT3DDEVICE9EX  iface, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
    return IDirect3DDevice9_CreatePixelShader(DeviceProxyInterface, pFunction, ppShader);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DPixelShader9* pShader)
{
    return IDirect3DDevice9_SetPixelShader(DeviceProxyInterface, pShader);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DPixelShader9** ppShader)
{
    return IDirect3DDevice9_GetPixelShader(DeviceProxyInterface, ppShader);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    return IDirect3DDevice9_SetPixelShaderConstantF(DeviceProxyInterface, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return IDirect3DDevice9_GetPixelShaderConstantF(DeviceProxyInterface, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    return IDirect3DDevice9_SetPixelShaderConstantI(DeviceProxyInterface, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return IDirect3DDevice9_GetPixelShaderConstantI(DeviceProxyInterface, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    return IDirect3DDevice9_SetPixelShaderConstantB(DeviceProxyInterface, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return IDirect3DDevice9_GetPixelShaderConstantB(DeviceProxyInterface, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Pure_DrawRectPatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
    return IDirect3DDevice9_DrawRectPatch(DeviceProxyInterface, Handle, pNumSegs, pRectPatchInfo);
}

HRESULT WINAPI IDirect3DDevice9Pure_DrawTriPatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
    return IDirect3DDevice9_DrawTriPatch(DeviceProxyInterface, Handle, pNumSegs, pTriPatchInfo);
}

HRESULT WINAPI IDirect3DDevice9Pure_DeletePatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle)
{
    return IDirect3DDevice9_DeletePatch(DeviceProxyInterface, Handle);
}

HRESULT WINAPI IDirect3DDevice9Pure_CreateQuery(LPDIRECT3DDEVICE9EX  iface, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
    return IDirect3DDevice9_CreateQuery(DeviceProxyInterface, Type, ppQuery);
}

static HRESULT WINAPI IDirect3DDevice9HAL_SetConvolutionMonoKernel(LPDIRECT3DDEVICE9EX iface,
        UINT width, UINT height, float *rows, float *columns)
{
    FIXME("iface %p, width %u, height %u, rows %p, columns %p stub!\n",
            iface, width, height, rows, columns);

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9HAL_ComposeRects(LPDIRECT3DDEVICE9EX iface,
        IDirect3DSurface9 *src_surface, IDirect3DSurface9 *dst_surface, IDirect3DVertexBuffer9 *src_descs,
        UINT rect_count, IDirect3DVertexBuffer9 *dst_descs, D3DCOMPOSERECTSOP operation, INT offset_x, INT offset_y)
{
    FIXME("iface %p, src_surface %p, dst_surface %p, src_descs %p, rect_count %u, "
            "dst_descs %p, operation %#x, offset_x %u, offset_y %u stub!\n",
            iface, src_surface, dst_surface, src_descs, rect_count,
            dst_descs, operation, offset_x, offset_y);

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9HAL_PresentEx(LPDIRECT3DDEVICE9EX iface,
        const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride,
        const RGNDATA *pDirtyRegion, DWORD flags)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);
    // struct d3d9_swapchain *swapchain;
    // unsigned int i;
    // HRESULT hr;

    // TRACE("iface %p, src_rect %s, dst_rect %s, dst_window_override %p, dirty_region %p, flags %#lx.\n",
            // iface, wine_dbgstr_rect(src_rect), wine_dbgstr_rect(dst_rect),
            // dst_window_override, dirty_region, flags);

    // if (device->device_state != D3D9_DEVICE_STATE_OK)
        // return S_PRESENT_OCCLUDED;

    // if (dirty_region)
        // FIXME("Ignoring dirty_region %p.\n", dirty_region);

    // wined3d_mutex_lock();
    // for (i = 0; i < device->implicit_swapchain_count; ++i)
    // {
        // swapchain = wined3d_swapchain_get_parent(device->implicit_swapchains[i]);
        // if (FAILED(hr = wined3d_swapchain_present(swapchain->wined3d_swapchain,
                // src_rect, dst_rect, dst_window_override, swapchain->swap_interval, flags)))
        // {
            // wined3d_mutex_unlock();
            // return hr;
        // }
    // }
    // wined3d_mutex_unlock();
	
	return IDirect3DDevice9Base_Present(iface, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

static HRESULT WINAPI IDirect3DDevice9HAL_GetGPUThreadPriority(LPDIRECT3DDEVICE9EX iface, INT *priority)
{
    FIXME("iface %p, priority %p stub!\n", iface, priority);

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9HAL_SetGPUThreadPriority(LPDIRECT3DDEVICE9EX iface, INT priority)
{
    FIXME("iface %p, priority %d stub!\n", iface, priority);

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9HAL_WaitForVBlank(LPDIRECT3DDEVICE9EX iface, UINT swapchain_idx)
{
    FIXME("iface %p, swapchain_idx %u stub!\n", iface, swapchain_idx);

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9HAL_CheckResourceResidency(LPDIRECT3DDEVICE9EX iface,
        IDirect3DResource9 **resources, UINT32 resource_count)
{
    FIXME("iface %p, resources %p, resource_count %u stub!\n",
            iface, resources, resource_count);

    return E_NOTIMPL;
}

static UINT MaxLatency;

static HRESULT WINAPI IDirect3DDevice9HAL_SetMaximumFrameLatency(LPDIRECT3DDEVICE9EX iface, UINT max_latency)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);

    // TRACE("iface %p, max_latency %u.\n", iface, max_latency);

    // if (max_latency > 30)
        // return D3DERR_INVALIDCALL;
	
	MaxLatency = max_latency;

    // wined3d_mutex_lock();
    // wined3d_device_set_max_frame_latency(device->wined3d_device, max_latency);
    // wined3d_mutex_unlock();

    return S_OK;
}

static HRESULT WINAPI IDirect3DDevice9HAL_GetMaximumFrameLatency(LPDIRECT3DDEVICE9EX iface, UINT *max_latency)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);

    // TRACE("iface %p, max_latency %p.\n", iface, max_latency);
	
	*max_latency = MaxLatency;

    // wined3d_mutex_lock();
    // *max_latency = wined3d_device_get_max_frame_latency(device->wined3d_device);
    // wined3d_mutex_unlock();

    return S_OK;
}

static HRESULT WINAPI IDirect3DDevice9HAL_CheckDeviceState(LPDIRECT3DDEVICE9EX iface, HWND dst_window)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);
    // struct wined3d_swapchain_desc swapchain_desc;

    // TRACE("iface %p, dst_window %p.\n", iface, dst_window);

    // wined3d_mutex_lock();
    // wined3d_swapchain_get_desc(device->implicit_swapchains[0], &swapchain_desc);
    // wined3d_mutex_unlock();

    // if (swapchain_desc.windowed)
        // return D3D_OK;

    // /* FIXME: This is actually supposed to check if any other device is in
     // * fullscreen mode. */
    // if (dst_window != swapchain_desc.device_window)
        // return device->device_state == D3D9_DEVICE_STATE_OK ? S_PRESENT_OCCLUDED : D3D_OK;

    // return device->device_state == D3D9_DEVICE_STATE_OK ? D3D_OK : S_PRESENT_OCCLUDED;
	HRESULT hr;

    // Verifica o estado do dispositivo
    hr = IDirect3DDevice9_TestCooperativeLevel(iface);

    switch (hr)
    {
    case D3D_OK:
        // O dispositivo está funcionando corretamente
        return S_OK;

    case D3DERR_DEVICELOST:
        // O dispositivo foi perdido e não pode ser restaurado ainda
        return D3DERR_DEVICELOST;

    case D3DERR_DEVICENOTRESET:
        // O dispositivo foi perdido, mas pode ser restaurado
        return D3DERR_DEVICENOTRESET;

    case D3DERR_DRIVERINTERNALERROR:
        // Erro interno do driver
        return D3DERR_DRIVERINTERNALERROR;

    default:
        // Outros erros
        return hr;
    }
}

static HRESULT WINAPI IDirect3DDevice9HAL_CreateRenderTargetEx(LPDIRECT3DDEVICE9EX iface,
        UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality,
        BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD usage)
{
	
	return IDirect3DDevice9Base_CreateRenderTarget(iface, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);	
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);
    // unsigned int access = WINED3D_RESOURCE_ACCESS_GPU;

    // TRACE("iface %p, width %u, height %u, format %#x, multisample_type %#x, multisample_quality %lu, "
            // "lockable %#x, surface %p, shared_handle %p, usage %#lx.\n",
            // iface, width, height, format, multisample_type, multisample_quality,
            // lockable, surface, shared_handle, usage);

    // *surface = NULL;

    // if (usage & (D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_RENDERTARGET))
    // {
        // WARN("Invalid usage %#lx.\n", usage);
        // return D3DERR_INVALIDCALL;
    // }

    // if (shared_handle)
        // FIXME("Resource sharing not implemented, *shared_handle %p.\n", *shared_handle);

    // if (lockable)
        // access |= WINED3D_RESOURCE_ACCESS_MAP_R | WINED3D_RESOURCE_ACCESS_MAP_W;

    // return d3d9_device_create_surface(device, 0, wined3dformat_from_d3dformat(format),
            // wined3d_multisample_type_from_d3d(multisample_type), multisample_quality,
            // usage & WINED3DUSAGE_MASK, WINED3D_BIND_RENDER_TARGET, access, width, height, NULL, surface);
}

static HRESULT WINAPI IDirect3DDevice9HAL_CreateOffscreenPlainSurfaceEx(LPDIRECT3DDEVICE9EX iface,
        UINT width, UINT height, D3DFORMAT format, D3DPOOL pool, IDirect3DSurface9 **surface,
        HANDLE *shared_handle, DWORD usage)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9HAL_CreateDepthStencilSurfaceEx(LPDIRECT3DDEVICE9EX iface,
        UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality,
        BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD usage)
{
	return IDirect3DDevice9Base_CreateDepthStencilSurface(iface, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);
    // DWORD flags = 0;

    // TRACE("iface %p, width %u, height %u, format %#x, multisample_type %#x, multisample_quality %lu, "
            // "discard %#x, surface %p, shared_handle %p, usage %#lx.\n",
            // iface, width, height, format, multisample_type, multisample_quality,
            // discard, surface, shared_handle, usage);

    // if (usage & (D3DUSAGE_DEPTHSTENCIL | D3DUSAGE_RENDERTARGET))
    // {
        // WARN("Invalid usage %#lx.\n", usage);
        // return D3DERR_INVALIDCALL;
    // }

    // if (shared_handle)
        // FIXME("Resource sharing not implemented, *shared_handle %p.\n", *shared_handle);

    // if (discard)
        // flags |= WINED3D_TEXTURE_CREATE_DISCARD;

    // *surface = NULL;
    // return d3d9_device_create_surface(device, flags, wined3dformat_from_d3dformat(format),
            // wined3d_multisample_type_from_d3d(multisample_type), multisample_quality, usage & WINED3DUSAGE_MASK,
            // WINED3D_BIND_DEPTH_STENCIL, WINED3D_RESOURCE_ACCESS_GPU, width, height, NULL, surface);
}

static HRESULT WINAPI IDirect3DDevice9HAL_ResetEx(LPDIRECT3DDEVICE9EX iface,
        D3DPRESENT_PARAMETERS *present_parameters, D3DDISPLAYMODEEX *mode)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);

    // TRACE("iface %p, present_parameters %p, mode %p.\n", iface, present_parameters, mode);

    // if (!present_parameters->Windowed == !mode)
    // {
        // WARN("Mode can be passed if and only if Windowed is FALSE.\n");
        // return D3DERR_INVALIDCALL;
    // }

    // if (mode && (mode->Width != present_parameters->BackBufferWidth
            // || mode->Height != present_parameters->BackBufferHeight))
    // {
        // WARN("Mode and back buffer mismatch (mode %ux%u, backbuffer %ux%u).\n",
                // mode->Width, mode->Height,
                // present_parameters->BackBufferWidth, present_parameters->BackBufferHeight);
        // return D3DERR_INVALIDCALL;
    // }

    // return d3d9_device_reset(device, present_parameters, mode);
	return IDirect3DDevice9Base_Reset(iface, present_parameters);
}

static HRESULT WINAPI IDirect3DDevice9HAL_GetDisplayModeEx(LPDIRECT3DDEVICE9EX iface,
        UINT iSwapChain, D3DDISPLAYMODEEX *mode, D3DDISPLAYROTATION *rotation)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);
    // struct wined3d_display_mode wined3d_mode;
    // HRESULT hr;

    // TRACE("iface %p, swapchain_idx %u, mode %p, rotation %p.\n",
            // iface, swapchain_idx, mode, rotation);

    // if (mode->Size != sizeof(*mode))
        // return D3DERR_INVALIDCALL;

    // wined3d_mutex_lock();
    // hr = wined3d_device_get_display_mode(device->wined3d_device, swapchain_idx, &wined3d_mode,
            // (enum wined3d_display_rotation *)rotation);
    // wined3d_mutex_unlock();

    // if (SUCCEEDED(hr))
    // {
        // mode->Width = wined3d_mode.width;
        // mode->Height = wined3d_mode.height;
        // mode->RefreshRate = wined3d_mode.refresh_rate;
        // mode->Format = d3dformat_from_wined3dformat(wined3d_mode.format_id);
        // mode->ScanLineOrdering = d3dscanlineordering_from_wined3d(wined3d_mode.scanline_ordering);
    // }

    // return hr;
    D3DDISPLAYMODE pMode;
    HRESULT hr;
    
    if (!mode || mode->Size != sizeof(D3DDISPLAYMODEEX)) 
        return D3DERR_INVALIDCALL;
    
    pMode.Width = mode->Width;
    pMode.Height = mode->Height;
    pMode.RefreshRate = mode->RefreshRate;
    pMode.Format = mode->Format;
    
    hr = IDirect3DDevice9Base_GetDisplayMode(iface, iSwapChain, &pMode);
    if (SUCCEEDED(hr)) {       
        mode->Width = pMode.Width;
        mode->Height = pMode.Height;
        mode->RefreshRate = pMode.RefreshRate;
        mode->Format = pMode.Format;
        mode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
    }
    
    return hr; 
}

IDirect3DDevice9ExVtbl_INT Direct3DDevice9Ex_Vtbl =
{
    /* IUnknown */
    IDirect3DDevice9Base_QueryInterface,
    IDirect3DDevice9Base_AddRef,
    IDirect3DDevice9Base_Release,
    /* IDirect3DDevice9 public */
    IDirect3DDevice9Base_TestCooperativeLevel,
    IDirect3DDevice9Base_GetAvailableTextureMem,
    IDirect3DDevice9Base_EvictManagedResources,
    IDirect3DDevice9Base_GetDirect3D,
    IDirect3DDevice9Base_GetDeviceCaps,
    IDirect3DDevice9Base_GetDisplayMode,
    IDirect3DDevice9Base_GetCreationParameters,
    IDirect3DDevice9Base_SetCursorProperties,
    IDirect3DDevice9Base_SetCursorPosition,
    IDirect3DDevice9Base_ShowCursor,
    IDirect3DDevice9Base_CreateAdditionalSwapChain,
    IDirect3DDevice9Base_GetSwapChain,
    IDirect3DDevice9Base_GetNumberOfSwapChains,
    IDirect3DDevice9Base_Reset,
    IDirect3DDevice9Base_Present,
    IDirect3DDevice9Base_GetBackBuffer,
    IDirect3DDevice9Base_GetRasterStatus,
    IDirect3DDevice9Base_SetDialogBoxMode,
    IDirect3DDevice9Base_SetGammaRamp,
    IDirect3DDevice9Base_GetGammaRamp,
    IDirect3DDevice9Base_CreateTexture,
    IDirect3DDevice9Base_CreateVolumeTexture,
    IDirect3DDevice9Base_CreateCubeTexture,
    IDirect3DDevice9Base_CreateVertexBuffer,
    IDirect3DDevice9Base_CreateIndexBuffer,
    IDirect3DDevice9Base_CreateRenderTarget,
    IDirect3DDevice9Base_CreateDepthStencilSurface,
    IDirect3DDevice9Base_UpdateSurface,
    IDirect3DDevice9Base_UpdateTexture,
    IDirect3DDevice9Base_GetRenderTargetData,
    IDirect3DDevice9Base_GetFrontBufferData,
    IDirect3DDevice9Base_StretchRect,
    IDirect3DDevice9Base_ColorFill,
    IDirect3DDevice9Base_CreateOffscreenPlainSurface,
    IDirect3DDevice9Pure_SetRenderTarget,
    IDirect3DDevice9Pure_GetRenderTarget,
    IDirect3DDevice9Pure_SetDepthStencilSurface,
    IDirect3DDevice9Pure_GetDepthStencilSurface,
    IDirect3DDevice9Pure_BeginScene,
    IDirect3DDevice9Pure_EndScene,
    IDirect3DDevice9Pure_Clear,
    IDirect3DDevice9Pure_SetTransform,
    IDirect3DDevice9HAL_GetTransform,
    IDirect3DDevice9Pure_MultiplyTransform,
    IDirect3DDevice9Pure_SetViewport,
    IDirect3DDevice9Pure_GetViewport,
    IDirect3DDevice9Pure_SetMaterial,
    IDirect3DDevice9HAL_GetMaterial,
    IDirect3DDevice9Pure_SetLight,
    IDirect3DDevice9HAL_GetLight,
    IDirect3DDevice9Pure_LightEnable,
    IDirect3DDevice9HAL_GetLightEnable,
    IDirect3DDevice9Pure_SetClipPlane,
    IDirect3DDevice9HAL_GetClipPlane,
    IDirect3DDevice9HAL_SetRenderState,
    IDirect3DDevice9HAL_GetRenderState,
    IDirect3DDevice9Pure_CreateStateBlock,
    IDirect3DDevice9Pure_BeginStateBlock,
    IDirect3DDevice9Pure_EndStateBlock,
    IDirect3DDevice9HAL_SetClipStatus,
    IDirect3DDevice9HAL_GetClipStatus,
    IDirect3DDevice9Pure_GetTexture,
    IDirect3DDevice9Pure_SetTexture,
    IDirect3DDevice9HAL_GetTextureStageState,
    IDirect3DDevice9Pure_SetTextureStageState,
    IDirect3DDevice9HAL_GetSamplerState,
    IDirect3DDevice9Pure_SetSamplerState,
    IDirect3DDevice9HAL_ValidateDevice,
    IDirect3DDevice9Pure_SetPaletteEntries,
    IDirect3DDevice9Pure_GetPaletteEntries,
    IDirect3DDevice9Pure_SetCurrentTexturePalette,
    IDirect3DDevice9Pure_GetCurrentTexturePalette,
    IDirect3DDevice9Pure_SetScissorRect,
    IDirect3DDevice9Pure_GetScissorRect,
    IDirect3DDevice9HAL_SetSoftwareVertexProcessing,
    IDirect3DDevice9HAL_GetSoftwareVertexProcessing,
    IDirect3DDevice9Pure_SetNPatchMode,
    IDirect3DDevice9Pure_GetNPatchMode,
    IDirect3DDevice9Pure_DrawPrimitive,
    IDirect3DDevice9Pure_DrawIndexedPrimitive,
    IDirect3DDevice9Pure_DrawPrimitiveUP,
    IDirect3DDevice9Pure_DrawIndexedPrimitiveUP,
    IDirect3DDevice9HAL_ProcessVertices,
    IDirect3DDevice9Pure_CreateVertexDeclaration,
    IDirect3DDevice9Pure_SetVertexDeclaration,
    IDirect3DDevice9Pure_GetVertexDeclaration,
    IDirect3DDevice9Pure_SetFVF,
    IDirect3DDevice9Pure_GetFVF,
    IDirect3DDevice9Pure_CreateVertexShader,
    IDirect3DDevice9Pure_SetVertexShader,
    IDirect3DDevice9HAL_GetVertexShader,
    IDirect3DDevice9Pure_SetVertexShaderConstantF,
    IDirect3DDevice9Pure_GetVertexShaderConstantF,
    IDirect3DDevice9Pure_SetVertexShaderConstantI,
    IDirect3DDevice9Pure_GetVertexShaderConstantI,
    IDirect3DDevice9Pure_SetVertexShaderConstantB,
    IDirect3DDevice9Pure_GetVertexShaderConstantB,
    IDirect3DDevice9Pure_SetStreamSource,
    IDirect3DDevice9Pure_GetStreamSource,
    IDirect3DDevice9Pure_SetStreamSourceFreq,
    IDirect3DDevice9Pure_GetStreamSourceFreq,
    IDirect3DDevice9Pure_SetIndices,
    IDirect3DDevice9Pure_GetIndices,
    IDirect3DDevice9Pure_CreatePixelShader,
    IDirect3DDevice9Pure_SetPixelShader,
    IDirect3DDevice9HAL_GetPixelShader,
    IDirect3DDevice9Pure_SetPixelShaderConstantF,
    IDirect3DDevice9HAL_GetPixelShaderConstantF,
    IDirect3DDevice9Pure_SetPixelShaderConstantI,
    IDirect3DDevice9HAL_GetPixelShaderConstantI,
    IDirect3DDevice9Pure_SetPixelShaderConstantB,
    IDirect3DDevice9HAL_GetPixelShaderConstantB,
    IDirect3DDevice9Pure_DrawRectPatch,
    IDirect3DDevice9Pure_DrawTriPatch,
    IDirect3DDevice9Pure_DeletePatch,
    IDirect3DDevice9Pure_CreateQuery,
    /* LPDIRECT3DDEVICE9EX */
    IDirect3DDevice9HAL_SetConvolutionMonoKernel,
    IDirect3DDevice9HAL_ComposeRects,
    IDirect3DDevice9HAL_PresentEx,
    IDirect3DDevice9HAL_GetGPUThreadPriority,
    IDirect3DDevice9HAL_SetGPUThreadPriority,
    IDirect3DDevice9HAL_WaitForVBlank,
    IDirect3DDevice9HAL_CheckResourceResidency,
    IDirect3DDevice9HAL_SetMaximumFrameLatency,
    IDirect3DDevice9HAL_GetMaximumFrameLatency,
    IDirect3DDevice9HAL_CheckDeviceState,
    IDirect3DDevice9HAL_CreateRenderTargetEx,
    IDirect3DDevice9HAL_CreateOffscreenPlainSurfaceEx,
    IDirect3DDevice9HAL_CreateDepthStencilSurfaceEx,
    IDirect3DDevice9HAL_ResetEx,
    IDirect3DDevice9HAL_GetDisplayModeEx	
};