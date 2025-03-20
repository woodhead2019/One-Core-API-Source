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
LPDIRECT3DDEVICE9EX_INT IDirect3DDevice9ToImpl(LPDIRECT3DDEVICE9EX  iface)
{
    if (NULL == iface)
        return NULL;

    return (LPDIRECT3DDEVICE9EX_INT)((ULONG_PTR)iface - FIELD_OFFSET(DIRECT3DDEVICE9EX_INT, lpVtbl));
}

HRESULT CreateD3D9SwapChainEx(OUT LPDIRECT3DSWAPCHAIN9EX *ppDirect3D9SwapChainEx, IN LPDIRECT3DSWAPCHAIN9 ppDirect3D9SwapChain)
{
    LPDIRECT3DSWAPCHAIN9_INT pThisBaseDevice;
	
    if (ppDirect3D9SwapChain == 0)
        return DDERR_INVALIDPARAMS;

    if (AlignedAlloc((LPVOID *)&pThisBaseDevice, sizeof(DIRECT3DDEVICE9EX_INT)) != S_OK)
        return DDERR_OUTOFMEMORY;

    if (pThisBaseDevice == 0)
        return DDERR_OUTOFMEMORY;	

    pThisBaseDevice->lpVtbl = &Direct3DSwapChain9Ex_Vtbl;
    pThisBaseDevice->lRealD3D9SwapChain = ppDirect3D9SwapChain;
    //pThisBaseDevice->dwProcessId = GetCurrentThreadId();
    //pThisBaseDevice->lRefCnt = 1;

    //InitializeCriticalSection(&pThisBaseDevice->CriticalSection);

    *ppDirect3D9SwapChainEx = (LPDIRECT3DSWAPCHAIN9EX)&pThisBaseDevice->lpVtbl;

    return D3D_OK;
}

// static HRESULT InvalidCall(LPDIRECT3DDEVICE9_INT This, LPSTR ErrorMsg)
// {
    // DPRINT1("%s",ErrorMsg);
    // UNLOCK_D3DDEVICE9();
    // return D3DERR_INVALIDCALL;
// }

/* IDirect3DDevice9: IUnknown implementation */
HRESULT WINAPI IDirect3DDevice9Impl_QueryInterface(LPDIRECT3DDEVICE9EX  iface, REFIID riid, void** ppvObject)
{
    LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);

    DbgPrint("IDirect3DDevice9Impl_QueryInterface called\n");	

    if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_IDirect3DDevice9) || IsEqualGUID(riid, &IID_IDirect3DDevice9Ex))
    {
        IUnknown_AddRef(iface);
        *ppvObject = &This->lpVtbl;
        return D3D_OK;
    }

    *ppvObject = NULL;
	return IDirect3DDevice9_QueryInterface(This->lRealD3D9DeviceEx, riid, ppvObject);
}

ULONG WINAPI IDirect3DDevice9Impl_AddRef(LPDIRECT3DDEVICE9EX  iface)
{
    LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
    // ULONG ref = InterlockedIncrement(&This->lRefCnt);
	
	DbgPrint("IDirect3DDevice9Impl_AddRef called\n");	

    // return ref;
	return IDirect3DDevice9_AddRef(This->lRealD3D9DeviceEx);
}

ULONG WINAPI IDirect3DDevice9Impl_Release(LPDIRECT3DDEVICE9EX  iface)
{
    LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
    ULONG Ref;   
	
	DbgPrint("IDirect3DDevice9Impl_Release called\n");

    Ref = IDirect3DDevice9_Release(This->lRealD3D9DeviceEx);
    if (Ref == 0) {
        AlignedFree(This);
    }
    return Ref;	
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
}

/* IDirect3DDevice9 public interface */
HRESULT WINAPI IDirect3DDevice9Impl_TestCooperativeLevel(LPDIRECT3DDEVICE9EX  iface)
{
	HRESULT Ret;
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_TestCooperativeLevel called\n");
    Ret =  IDirect3DDevice9_TestCooperativeLevel(This->lRealD3D9DeviceEx);
		if(Ret == D3D_OK){
		DbgPrint("IDirect3DDevice9Impl_TestCooperativeLevel::IDirect3DDevice9_TestCooperativeLevel sucessful\n");
	}		
	
	return Ret;
}

/*++
* @name IDirect3DDevice9::GetAvailableTextureMem
* @implemented
*
* The function IDirect3DDevice9Impl_GetAvailableTextureMem returns a pointer to the IDirect3D9 object
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
UINT WINAPI IDirect3DDevice9Impl_GetAvailableTextureMem(LPDIRECT3DDEVICE9EX  iface)
{
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);	
		
	DbgPrint("IDirect3DDevice9Impl_GetAvailableTextureMem called\n");
	
	return IDirect3DDevice9_GetAvailableTextureMem(This->lRealD3D9DeviceEx);	
}

HRESULT WINAPI IDirect3DDevice9Impl_EvictManagedResources(LPDIRECT3DDEVICE9EX  iface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_EvictManagedResources called\n");	
	
    return IDirect3DDevice9_EvictManagedResources(This->lRealD3D9DeviceEx);
}

/*++
* @name IDirect3DDevice9::GetDirect3D
* @implemented
*
* The function IDirect3DDevice9Impl_GetDirect3D returns a pointer to the IDirect3D9 object
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
HRESULT WINAPI IDirect3DDevice9Impl_GetDirect3D(LPDIRECT3DDEVICE9EX  iface, IDirect3D9** ppD3D9)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetDirect3D called\n");		
	
	return IDirect3DDevice9_GetDirect3D(This->lRealD3D9DeviceEx, ppD3D9);	
}

/*++
* @name IDirect3DDevice9::GetDeviceCaps
* @implemented
*
* The function IDirect3DDevice9Impl_GetDeviceCaps fills the pCaps argument with the
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
HRESULT WINAPI IDirect3DDevice9Impl_GetDeviceCaps(LPDIRECT3DDEVICE9EX  iface, D3DCAPS9* pCaps)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	HRESULT hr;
	
	DbgPrint("IDirect3DDevice9Impl_GetDeviceCaps called\n");
	
	hr = IDirect3DDevice9_GetDeviceCaps(This->lRealD3D9DeviceEx, pCaps);	
	
	return hr;
}

/*++
* @name IDirect3DDevice9::GetDisplayMode
* @implemented
*
* The function IDirect3DDevice9Impl_GetDisplayMode fills the pMode argument with the
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
HRESULT WINAPI IDirect3DDevice9Impl_GetDisplayMode(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DDISPLAYMODE* pMode)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetDisplayMode called\n");
	
	return IDirect3DDevice9_GetDisplayMode(This->lRealD3D9DeviceEx, iSwapChain, pMode);
}

/*++
* @name IDirect3DDevice9::GetCreationParameters
* @implemented
*
* The function IDirect3DDevice9Impl_GetCreationParameters fills the pParameters argument with the
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
HRESULT WINAPI IDirect3DDevice9Impl_GetCreationParameters(LPDIRECT3DDEVICE9EX  iface, D3DDEVICE_CREATION_PARAMETERS* pParameters)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetCreationParameters called\n");
	
	return IDirect3DDevice9_GetCreationParameters(This->lRealD3D9DeviceEx, pParameters);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetCursorProperties(LPDIRECT3DDEVICE9EX  iface, UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);

    DbgPrint("IDirect3DDevice9Impl_SetCursorProperties called\n");	

	return IDirect3DDevice9_SetCursorProperties(This->lRealD3D9DeviceEx, XHotSpot, YHotSpot, pCursorBitmap);
}

VOID WINAPI IDirect3DDevice9Impl_SetCursorPosition(LPDIRECT3DDEVICE9EX  iface, int X, int Y, DWORD Flags)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetCursorPosition called\n");	

    IDirect3DDevice9_SetCursorPosition(This->lRealD3D9DeviceEx, X, Y, Flags);
}

BOOL WINAPI IDirect3DDevice9Impl_ShowCursor(LPDIRECT3DDEVICE9EX  iface, BOOL bShow)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_ShowCursor called\n");
	
	return IDirect3DDevice9_ShowCursor(This->lRealD3D9DeviceEx, bShow);
}

/*++
* @name IDirect3DDevice9::CreateAdditionalSwapChain
* @implemented
*
* The function IDirect3DDevice9Impl_CreateAdditionalSwapChain creates a swap chain object,
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
HRESULT WINAPI IDirect3DDevice9Impl_CreateAdditionalSwapChain(LPDIRECT3DDEVICE9EX  iface, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** ppSwapChain)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	LPDIRECT3DSWAPCHAIN9 RealSwapChainAdditional;
    HRESULT Ret;
	
	DbgPrint("IDirect3DDevice9Impl_CreateAdditionalSwapChain called\n");
	
	Ret = IDirect3DDevice9_CreateAdditionalSwapChain(This->lRealD3D9DeviceEx, pPresentationParameters, &RealSwapChainAdditional);

	if(Ret == D3D_OK){
	   DbgPrint("IDirect3DDevice9Impl_CreateAdditionalSwapChain::IDirect3DDevice9_CreateAdditionalSwapChain sucessful\n");
       Ret = CreateD3D9SwapChainEx((LPDIRECT3DSWAPCHAIN9EX*)ppSwapChain, RealSwapChainAdditional);
	}
	
	if(Ret == D3D_OK){
		DbgPrint("IDirect3DDevice9Impl_CreateAdditionalSwapChain::CreateD3D9DeviceEx sucessful\n");
	}	

    return Ret;
}

/*++
* @name IDirect3DDevice9::GetSwapChain
* @implemented
*
* The function IDirect3DDevice9Impl_GetSwapChain returns a pointer to a swap chain object.
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
HRESULT WINAPI IDirect3DDevice9Impl_GetSwapChain(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, IDirect3DSwapChain9** ppSwapChain)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetSwapChain called\n");
	
	Ret = IDirect3DDevice9_GetSwapChain(This->lRealD3D9DeviceEx, iSwapChain, &SwapChainProxyInterface);

	if(Ret == D3D_OK){
	   DbgPrint("IDirect3DDevice9Impl_CreateAdditionalSwapChain::IDirect3DDevice9_CreateAdditionalSwapChain sucessful\n"); 	
       Ret = CreateD3D9SwapChainEx((LPDIRECT3DSWAPCHAIN9EX*)ppSwapChain, SwapChainProxyInterface);
	}
	
	if(Ret == D3D_OK){
		DbgPrint("IDirect3DDevice9Impl_CreateAdditionalSwapChain::CreateD3D9DeviceEx sucessful\n");
	}	

    return Ret;	
}

/*++
* @name IDirect3DDevice9::GetNumberOfSwapChains
* @implemented
*
* The function IDirect3DDevice9Impl_GetNumberOfSwapChains returns the number of swap chains
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
UINT WINAPI IDirect3DDevice9Impl_GetNumberOfSwapChains(LPDIRECT3DDEVICE9EX  iface)
{
    // UINT NumSwapChains;

    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // NumSwapChains = This->NumAdaptersInDevice;

    // UNLOCK_D3DDEVICE9();
    // return NumSwapChains;
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetNumberOfSwapChains called\n");
	
	return IDirect3DDevice9_GetNumberOfSwapChains(This->lRealD3D9DeviceEx);
}

HRESULT WINAPI IDirect3DDevice9Impl_Reset(LPDIRECT3DDEVICE9EX  iface, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	HRESULT Ret;
	
	DbgPrint("IDirect3DDevice9Impl_Reset called\n");

    Ret =  IDirect3DDevice9_Reset(This->lRealD3D9DeviceEx, pPresentationParameters);
	
	if(Ret == D3D_OK){
		DbgPrint("IDirect3DDevice9Impl_Reset::IDirect3DDevice9_Reset sucessful\n");
	}		

	return Ret;
}

/*++
* @name IDirect3DDevice9::Present
* @implemented
*
* The function IDirect3DDevice9Impl_Present displays the content of the next
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
HRESULT WINAPI IDirect3DDevice9Impl_Present(LPDIRECT3DDEVICE9EX  iface, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    // UINT i;
    // UINT iNumSwapChains;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // iNumSwapChains = IDirect3DDevice9Impl_GetNumberOfSwapChains(iface);
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
        
        // IDirect3DDevice9Impl_GetSwapChain(iface, i, &pSwapChain);
        // hResult = IDirect3DSwapChain9_Present(pSwapChain, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, 0);

        // if (FAILED(hResult))
        // {
            // UNLOCK_D3DDEVICE9();
            // return hResult;
        // }
    // }

    // UNLOCK_D3DDEVICE9();
    // return D3D_OK;
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_Reset called\n");

	return IDirect3DDevice9_Present(This->lRealD3D9DeviceEx, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

/*++
* @name IDirect3DDevice9::GetBackBuffer
* @implemented
*
* The function IDirect3DDevice9Impl_GetBackBuffer retrieves the back buffer
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
HRESULT WINAPI IDirect3DDevice9Impl_GetBackBuffer(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
    // HRESULT hResult;
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Impl_GetSwapChain(iface, iSwapChain, &pSwapChain);
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetBackBuffer called\n");

	return IDirect3DDevice9_GetBackBuffer(This->lRealD3D9DeviceEx, iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

/*++
* @name IDirect3DDevice9::GetRasterStatus
* @implemented
*
* The function IDirect3DDevice9Impl_GetRasterStatus retrieves raster information
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
HRESULT WINAPI IDirect3DDevice9Impl_GetRasterStatus(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
    // HRESULT hResult;
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Impl_GetSwapChain(iface, iSwapChain, &pSwapChain);
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetRasterStatus called\n");

	return IDirect3DDevice9_GetRasterStatus(This->lRealD3D9DeviceEx, iSwapChain, pRasterStatus);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetDialogBoxMode(LPDIRECT3DDEVICE9EX  iface, BOOL bEnableDialogs)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);

    return IDirect3DDevice9_SetDialogBoxMode(This->lRealD3D9DeviceEx, bEnableDialogs);
}

/*++
* @name IDirect3DDevice9::SetGammaRamp
* @implemented
*
* The function IDirect3DDevice9Impl_SetGammaRamp sets the gamma correction ramp values
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
VOID WINAPI IDirect3DDevice9Impl_SetGammaRamp(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetGammaRamp called\n");

	IDirect3DDevice9_SetGammaRamp(This->lRealD3D9DeviceEx, iSwapChain, Flags, pRamp);
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // Direct3DSwapChain9_INT* pSwapChain_INT;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Impl_GetSwapChain(iface, iSwapChain, &pSwapChain);
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
* The function IDirect3DDevice9Impl_GetGammaRamp retrieves the gamma correction ramp values
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
VOID WINAPI IDirect3DDevice9Impl_GetGammaRamp(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // Direct3DSwapChain9_INT* pSwapChain_INT;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Impl_GetSwapChain(iface, iSwapChain, &pSwapChain);
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetGammaRamp called\n");

	IDirect3DDevice9_GetGammaRamp(This->lRealD3D9DeviceEx, iSwapChain, pRamp);
}

/*++
* @name IDirect3DDevice9::CreateTexture
* @implemented
*
* The function IDirect3DDevice9Impl_CreateTexture creates a D3D9 texture.
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
HRESULT WINAPI IDirect3DDevice9Impl_CreateTexture(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateTexture called\n");

	return IDirect3DDevice9_CreateTexture(This->lRealD3D9DeviceEx, Width, Height, Levels, Usage, Format, Pool, ppTexture, NULL);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateVolumeTexture(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateVolumeTexture called\n");

    return IDirect3DDevice9_CreateVolumeTexture(This->lRealD3D9DeviceEx, Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, NULL);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateCubeTexture(LPDIRECT3DDEVICE9EX  iface, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateCubeTexture called\n");

    return IDirect3DDevice9_CreateCubeTexture(This->lRealD3D9DeviceEx, EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, NULL);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateVertexBuffer(LPDIRECT3DDEVICE9EX  iface, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateVertexBuffer called\n");

    return IDirect3DDevice9_CreateVertexBuffer(This->lRealD3D9DeviceEx, Length, Usage, FVF, Pool, ppVertexBuffer, NULL);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateIndexBuffer(LPDIRECT3DDEVICE9EX  iface, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateIndexBuffer called\n");

    return IDirect3DDevice9_CreateIndexBuffer(This->lRealD3D9DeviceEx, Length, Usage, Format, Pool, ppIndexBuffer, NULL);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateRenderTarget(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateRenderTarget called\n");

    return IDirect3DDevice9_CreateRenderTarget(This->lRealD3D9DeviceEx, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, NULL);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateDepthStencilSurface called\n");
	
    return IDirect3DDevice9_CreateDepthStencilSurface(This->lRealD3D9DeviceEx, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, NULL);
}

HRESULT WINAPI IDirect3DDevice9Impl_UpdateSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_UpdateSurface called\n");

    return IDirect3DDevice9_UpdateSurface(This->lRealD3D9DeviceEx, pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT WINAPI IDirect3DDevice9Impl_UpdateTexture(LPDIRECT3DDEVICE9EX  iface, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_UpdateTexture called\n");
	
    return IDirect3DDevice9_UpdateTexture(This->lRealD3D9DeviceEx, pSourceTexture, pDestinationTexture);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetRenderTargetData(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetRenderTargetData called\n");

    return IDirect3DDevice9_GetRenderTargetData(This->lRealD3D9DeviceEx, pRenderTarget, pDestSurface);
}

/*++
* @name IDirect3DDevice9::GetFrontBufferData
* @implemented
*
* The function IDirect3DDevice9Impl_GetFrontBufferData copies the content of
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
HRESULT WINAPI IDirect3DDevice9Impl_GetFrontBufferData(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
    // HRESULT hResult;
    // IDirect3DSwapChain9* pSwapChain = NULL;
    // LPDIRECT3DDEVICE9_INT This = IDirect3DDevice9ToImpl(iface);
    // LOCK_D3DDEVICE9();

    // IDirect3DDevice9Impl_GetSwapChain(iface, iSwapChain, &pSwapChain);
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetRenderTargetData called\n");
	
	return IDirect3DDevice9_GetFrontBufferData(This->lRealD3D9DeviceEx, iSwapChain, pDestSurface);
}

HRESULT WINAPI IDirect3DDevice9Impl_StretchRect(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_StretchRect called\n");

    return IDirect3DDevice9_StretchRect(This->lRealD3D9DeviceEx, pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT WINAPI IDirect3DDevice9Impl_ColorFill(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
 	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_ColorFill called\n");
 
	return IDirect3DDevice9_ColorFill(This->lRealD3D9DeviceEx, pSurface, pRect, color);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateOffscreenPlainSurface(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
 	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateOffscreenPlainSurface called\n");

    return IDirect3DDevice9_CreateOffscreenPlainSurface(This->lRealD3D9DeviceEx, Width, Height, Format, Pool, ppSurface, NULL);
}

/* Convert a IDirect3D9 pointer safely to the internal implementation struct */
/*static LPD3D9HALDEVICE IDirect3DDevice9ToImpl(LPDIRECT3DDEVICE9EX  iface)
{
    if (NULL == iface)
        return NULL;

    return (LPD3D9HALDEVICE)((ULONG_PTR)iface - FIELD_OFFSET(D3D9HALDEVICE, PureDevice.BaseDevice.lpVtbl));
}*/

/* IDirect3DDevice9 public interface */
HRESULT WINAPI IDirect3DDevice9Impl_SetRenderTarget(LPDIRECT3DDEVICE9EX  iface, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetRenderTarget called\n");
	
    return IDirect3DDevice9_SetRenderTarget(This->lRealD3D9DeviceEx, RenderTargetIndex, pRenderTarget);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetRenderTarget(LPDIRECT3DDEVICE9EX  iface, DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetRenderTarget called\n");	
	
    return IDirect3DDevice9_GetRenderTarget(This->lRealD3D9DeviceEx, RenderTargetIndex, ppRenderTarget);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pNewZStencil)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetDepthStencilSurface called\n");		
	
    return IDirect3DDevice9_SetDepthStencilSurface(This->lRealD3D9DeviceEx, pNewZStencil);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9** ppZStencilSurface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetDepthStencilSurface called\n");	
	
    return IDirect3DDevice9_GetDepthStencilSurface(This->lRealD3D9DeviceEx, ppZStencilSurface);
}

HRESULT WINAPI IDirect3DDevice9Impl_BeginScene(LPDIRECT3DDEVICE9EX  iface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_BeginScene called\n");		
	
    return IDirect3DDevice9_BeginScene(This->lRealD3D9DeviceEx);
}

HRESULT WINAPI IDirect3DDevice9Impl_EndScene(LPDIRECT3DDEVICE9EX  iface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_EndScene called\n");	
	
    return IDirect3DDevice9_EndScene(This->lRealD3D9DeviceEx);
}

HRESULT WINAPI IDirect3DDevice9Impl_Clear(LPDIRECT3DDEVICE9EX  iface, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_Clear called\n");		
	
    return IDirect3DDevice9_Clear(This->lRealD3D9DeviceEx, Count, pRects, Flags, Color, Z, Stencil);
} 

HRESULT WINAPI IDirect3DDevice9Impl_SetTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetTransform called\n");	
	
    return IDirect3DDevice9_SetTransform(This->lRealD3D9DeviceEx, State, pMatrix);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetTransform called\n");		
	
    return IDirect3DDevice9_GetTransform(This->lRealD3D9DeviceEx, State, pMatrix);
}

HRESULT WINAPI IDirect3DDevice9Impl_MultiplyTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_MultiplyTransform called\n");		
	
    return IDirect3DDevice9_MultiplyTransform(This->lRealD3D9DeviceEx, State, pMatrix);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetViewport(LPDIRECT3DDEVICE9EX  iface, CONST D3DVIEWPORT9* pViewport)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetViewport called\n");		
	
    return IDirect3DDevice9_SetViewport(This->lRealD3D9DeviceEx, pViewport);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetViewport(LPDIRECT3DDEVICE9EX  iface, D3DVIEWPORT9* pViewport)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetViewport called\n");	
	
    return IDirect3DDevice9_GetViewport(This->lRealD3D9DeviceEx, pViewport);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetMaterial(LPDIRECT3DDEVICE9EX  iface, CONST D3DMATERIAL9* pMaterial)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetMaterial called\n");	
	
    return IDirect3DDevice9_SetMaterial(This->lRealD3D9DeviceEx, pMaterial);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetMaterial(LPDIRECT3DDEVICE9EX  iface, D3DMATERIAL9* pMaterial)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetMaterial called\n");	
	
    return IDirect3DDevice9_GetMaterial(This->lRealD3D9DeviceEx, pMaterial);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetLight(LPDIRECT3DDEVICE9EX  iface, DWORD Index, CONST D3DLIGHT9* pLight)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetLight called\n");	
	
    return IDirect3DDevice9_SetLight(This->lRealD3D9DeviceEx, Index, pLight);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetLight(LPDIRECT3DDEVICE9EX  iface, DWORD Index, D3DLIGHT9* pLight)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetLight called\n");		
	
    return IDirect3DDevice9_GetLight(This->lRealD3D9DeviceEx, Index, pLight);
}

HRESULT WINAPI IDirect3DDevice9Impl_LightEnable(LPDIRECT3DDEVICE9EX  iface, DWORD Index, BOOL Enable)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);

	DbgPrint("IDirect3DDevice9Impl_LightEnable called\n");		
	
    return IDirect3DDevice9_LightEnable(This->lRealD3D9DeviceEx, Index, Enable);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetLightEnable(LPDIRECT3DDEVICE9EX  iface, DWORD Index, BOOL* pEnable)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetLightEnable called\n");		
	
    return IDirect3DDevice9_GetLightEnable(This->lRealD3D9DeviceEx, Index, pEnable);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetClipPlane(LPDIRECT3DDEVICE9EX  iface, DWORD Index, CONST float* pPlane)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetClipPlane called\n");	
	
    return IDirect3DDevice9_SetClipPlane(This->lRealD3D9DeviceEx, Index, pPlane);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetClipPlane(LPDIRECT3DDEVICE9EX  iface, DWORD Index, float* pPlane)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetClipPlane called\n");	
	
    return IDirect3DDevice9_GetClipPlane(This->lRealD3D9DeviceEx, Index, pPlane);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD Value)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetRenderState called\n");		
	
    return IDirect3DDevice9_SetRenderState(This->lRealD3D9DeviceEx, State, Value);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD* pValue)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetRenderState called\n");		
	
    return IDirect3DDevice9_GetRenderState(This->lRealD3D9DeviceEx, State, pValue);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateStateBlock(LPDIRECT3DDEVICE9EX  iface, D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateStateBlock called\n");	
	
    return IDirect3DDevice9_CreateStateBlock(This->lRealD3D9DeviceEx, Type, ppSB);
}

HRESULT WINAPI IDirect3DDevice9Impl_BeginStateBlock(LPDIRECT3DDEVICE9EX  iface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_BeginStateBlock called\n");		
	
    return IDirect3DDevice9_BeginStateBlock(This->lRealD3D9DeviceEx);
}

HRESULT WINAPI IDirect3DDevice9Impl_EndStateBlock(LPDIRECT3DDEVICE9EX  iface, IDirect3DStateBlock9** ppSB)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_EndStateBlock called\n");	
	
    return IDirect3DDevice9_EndStateBlock(This->lRealD3D9DeviceEx, ppSB);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetClipStatus(LPDIRECT3DDEVICE9EX  iface, CONST D3DCLIPSTATUS9* pClipStatus)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetClipStatus called\n");	
	
    return IDirect3DDevice9_SetClipStatus(This->lRealD3D9DeviceEx, pClipStatus);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetClipStatus(LPDIRECT3DDEVICE9EX  iface, D3DCLIPSTATUS9* pClipStatus)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetClipStatus called\n");	
	
    return IDirect3DDevice9_GetClipStatus(This->lRealD3D9DeviceEx, pClipStatus);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetTexture(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetTexture called\n");		
	
    return IDirect3DDevice9_GetTexture(This->lRealD3D9DeviceEx, Stage, ppTexture);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetTexture(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetTexture called\n");	
	
    return IDirect3DDevice9_SetTexture(This->lRealD3D9DeviceEx, Stage, pTexture);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetTextureStageState(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetTextureStageState called\n");	
	
    return IDirect3DDevice9_GetTextureStageState(This->lRealD3D9DeviceEx, Stage, Type, pValue);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetTextureStageState(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetTextureStageState called\n");	
	
    return IDirect3DDevice9_SetTextureStageState(This->lRealD3D9DeviceEx, Stage, Type, Value);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetSamplerState(LPDIRECT3DDEVICE9EX  iface, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetSamplerState called\n");		
	
    return IDirect3DDevice9_GetSamplerState(This->lRealD3D9DeviceEx, Sampler, Type, pValue);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetSamplerState(LPDIRECT3DDEVICE9EX  iface, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetSamplerState called\n");	
	
    return IDirect3DDevice9_SetSamplerState(This->lRealD3D9DeviceEx, Sampler, Type, Value);
}

HRESULT WINAPI IDirect3DDevice9Impl_ValidateDevice(LPDIRECT3DDEVICE9EX  iface, DWORD* pNumPasses)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_ValidateDevice called\n");	
	
    return IDirect3DDevice9_ValidateDevice(This->lRealD3D9DeviceEx, pNumPasses);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetPaletteEntries(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetPaletteEntries called\n");	
	
    return IDirect3DDevice9_SetPaletteEntries(This->lRealD3D9DeviceEx, PaletteNumber, pEntries);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetPaletteEntries(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetPaletteEntries called\n");	
	
    return IDirect3DDevice9_GetPaletteEntries(This->lRealD3D9DeviceEx, PaletteNumber, pEntries);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetCurrentTexturePalette(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetCurrentTexturePalette called\n");	
	
    return IDirect3DDevice9_SetCurrentTexturePalette(This->lRealD3D9DeviceEx, PaletteNumber);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetCurrentTexturePalette(LPDIRECT3DDEVICE9EX  iface, UINT* pPaletteNumber)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetCurrentTexturePalette called\n");	
	
    return IDirect3DDevice9_GetCurrentTexturePalette(This->lRealD3D9DeviceEx, pPaletteNumber);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetScissorRect(LPDIRECT3DDEVICE9EX  iface, CONST RECT* pRect)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetScissorRect called\n");		
	
    return IDirect3DDevice9_SetScissorRect(This->lRealD3D9DeviceEx, pRect);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetScissorRect(LPDIRECT3DDEVICE9EX  iface, RECT* pRect)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetScissorRect called\n");	
	
    return IDirect3DDevice9_GetScissorRect(This->lRealD3D9DeviceEx, pRect);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface, BOOL bSoftware)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetSoftwareVertexProcessing called\n");	
	
    return IDirect3DDevice9_SetSoftwareVertexProcessing(This->lRealD3D9DeviceEx, bSoftware);
}

BOOL WINAPI IDirect3DDevice9Impl_GetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetSoftwareVertexProcessing called\n");		
	
    return IDirect3DDevice9_GetSoftwareVertexProcessing(This->lRealD3D9DeviceEx);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetNPatchMode(LPDIRECT3DDEVICE9EX  iface, float nSegments)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetNPatchMode called\n");	
	
    return IDirect3DDevice9_SetNPatchMode(This->lRealD3D9DeviceEx, nSegments);
}

float WINAPI IDirect3DDevice9Impl_GetNPatchMode(LPDIRECT3DDEVICE9EX  iface)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetNPatchMode called\n");	
	
    return IDirect3DDevice9_GetNPatchMode(This->lRealD3D9DeviceEx);
}

HRESULT WINAPI IDirect3DDevice9Impl_DrawPrimitive(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_DrawPrimitive called\n");	
	
    return IDirect3DDevice9_DrawPrimitive(This->lRealD3D9DeviceEx, PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_DrawIndexedPrimitive(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_DrawIndexedPrimitive called\n");	
	
    return IDirect3DDevice9_DrawIndexedPrimitive(This->lRealD3D9DeviceEx, PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_DrawPrimitiveUP(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_DrawPrimitiveUP called\n");		
	
    return IDirect3DDevice9_DrawPrimitiveUP(This->lRealD3D9DeviceEx, PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT WINAPI IDirect3DDevice9Impl_DrawIndexedPrimitiveUP(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_DrawIndexedPrimitiveUP called\n");	
	
    return IDirect3DDevice9_DrawIndexedPrimitiveUP(This->lRealD3D9DeviceEx, PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT WINAPI IDirect3DDevice9Impl_ProcessVertices(LPDIRECT3DDEVICE9EX  iface, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_ProcessVertices called\n");	
	
    return IDirect3DDevice9_ProcessVertices(This->lRealD3D9DeviceEx, SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateVertexDeclaration called\n");		
	
    return IDirect3DDevice9_CreateVertexDeclaration(This->lRealD3D9DeviceEx, pVertexElements, ppDecl);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexDeclaration9* pDecl)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetVertexDeclaration called\n");		
	
    return IDirect3DDevice9_SetVertexDeclaration(This->lRealD3D9DeviceEx, pDecl);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexDeclaration9** ppDecl)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetVertexDeclaration called\n");	
	
    return IDirect3DDevice9_GetVertexDeclaration(This->lRealD3D9DeviceEx, ppDecl);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetFVF(LPDIRECT3DDEVICE9EX  iface, DWORD FVF)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetFVF called\n");		
	
    return IDirect3DDevice9_SetFVF(This->lRealD3D9DeviceEx, FVF);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetFVF(LPDIRECT3DDEVICE9EX  iface, DWORD* pFVF)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetFVF called\n");	
	
    return IDirect3DDevice9_GetFVF(This->lRealD3D9DeviceEx, pFVF);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateVertexShader(LPDIRECT3DDEVICE9EX  iface, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	HRESULT hr;
	
	DbgPrint("IDirect3DDevice9Impl_CreateVertexShader called\n");	
	
	hr = IDirect3DDevice9_CreateVertexShader(This->lRealD3D9DeviceEx, pFunction, ppShader);
	
	DbgPrint("IDirect3DDevice9Impl_CreateVertexShader::IDirect3DDevice9_CreateVertexShader hr %#x. result\n", hr);
	
    return hr;
}

HRESULT WINAPI IDirect3DDevice9Impl_SetVertexShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexShader9* pShader)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetVertexShader called\n");	
	
    return IDirect3DDevice9_SetVertexShader(This->lRealD3D9DeviceEx, pShader);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetVertexShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexShader9** ppShader)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetVertexShader called\n");	
	
    return IDirect3DDevice9_GetVertexShader(This->lRealD3D9DeviceEx, ppShader);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetVertexShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetVertexShaderConstantF called\n");		
	
    return IDirect3DDevice9_SetVertexShaderConstantF(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetVertexShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetVertexShaderConstantF called\n");	
	
    return IDirect3DDevice9_GetVertexShaderConstantF(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetVertexShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetVertexShaderConstantI called\n");	
	
    return IDirect3DDevice9_SetVertexShaderConstantI(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetVertexShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetVertexShaderConstantI called\n");		
	
    return IDirect3DDevice9_GetVertexShaderConstantI(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetVertexShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetVertexShaderConstantB called\n");		
	
    return IDirect3DDevice9_SetVertexShaderConstantB(This->lRealD3D9DeviceEx, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetVertexShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetVertexShaderConstantB called\n");		
	
    return IDirect3DDevice9_GetVertexShaderConstantB(This->lRealD3D9DeviceEx, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetStreamSource(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetStreamSource called\n");		
	
    return IDirect3DDevice9_SetStreamSource(This->lRealD3D9DeviceEx, StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetStreamSource(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetStreamSource called\n");	
	
    return IDirect3DDevice9_GetStreamSource(This->lRealD3D9DeviceEx, StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetStreamSourceFreq(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber,UINT Setting)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetStreamSourceFreq called\n");	
	
    return IDirect3DDevice9_SetStreamSourceFreq(This->lRealD3D9DeviceEx, StreamNumber, Setting);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetStreamSourceFreq(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, UINT* pSetting)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetStreamSourceFreq called\n");	
	
    return IDirect3DDevice9_GetStreamSourceFreq(This->lRealD3D9DeviceEx, StreamNumber, pSetting);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetIndices(LPDIRECT3DDEVICE9EX  iface, IDirect3DIndexBuffer9* pIndexData)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetIndices called\n");		
	
    return IDirect3DDevice9_SetIndices(This->lRealD3D9DeviceEx, pIndexData);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetIndices(LPDIRECT3DDEVICE9EX  iface, IDirect3DIndexBuffer9** ppIndexData)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetIndices called\n");	
	
    return IDirect3DDevice9_GetIndices(This->lRealD3D9DeviceEx, ppIndexData);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreatePixelShader(LPDIRECT3DDEVICE9EX  iface, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	//DbgPrint("IDirect3DDevice9Impl_CreatePixelShader called\n");	
	
    return IDirect3DDevice9_CreatePixelShader(This->lRealD3D9DeviceEx, pFunction, ppShader);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetPixelShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DPixelShader9* pShader)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetPixelShader called\n");	
	
    return IDirect3DDevice9_SetPixelShader(This->lRealD3D9DeviceEx, pShader);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetPixelShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DPixelShader9** ppShader)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetPixelShader called\n");	
	
    return IDirect3DDevice9_GetPixelShader(This->lRealD3D9DeviceEx, ppShader);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetPixelShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetPixelShaderConstantF called\n");		
	
    return IDirect3DDevice9_SetPixelShaderConstantF(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetPixelShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetPixelShaderConstantF called\n");	
	
    return IDirect3DDevice9_GetPixelShaderConstantF(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4fCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetPixelShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface); 
	
	DbgPrint("IDirect3DDevice9Impl_SetPixelShaderConstantI called\n");		
	
	return IDirect3DDevice9_SetPixelShaderConstantI(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetPixelShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetPixelShaderConstantI called\n");	
	
    return IDirect3DDevice9_GetPixelShaderConstantI(This->lRealD3D9DeviceEx, StartRegister, pConstantData, Vector4iCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_SetPixelShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_SetPixelShaderConstantB called\n");	
	
    return IDirect3DDevice9_SetPixelShaderConstantB(This->lRealD3D9DeviceEx, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_GetPixelShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_GetPixelShaderConstantB called\n");	
	
    return IDirect3DDevice9_GetPixelShaderConstantB(This->lRealD3D9DeviceEx, StartRegister, pConstantData, BoolCount);
}

HRESULT WINAPI IDirect3DDevice9Impl_DrawRectPatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_DrawRectPatch called\n");		
	
    return IDirect3DDevice9_DrawRectPatch(This->lRealD3D9DeviceEx, Handle, pNumSegs, pRectPatchInfo);
}

HRESULT WINAPI IDirect3DDevice9Impl_DrawTriPatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_DrawTriPatch called\n");	
	
    return IDirect3DDevice9_DrawTriPatch(This->lRealD3D9DeviceEx, Handle, pNumSegs, pTriPatchInfo);
}

HRESULT WINAPI IDirect3DDevice9Impl_DeletePatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_DeletePatch called\n");		
	
    return IDirect3DDevice9_DeletePatch(This->lRealD3D9DeviceEx, Handle);
}

HRESULT WINAPI IDirect3DDevice9Impl_CreateQuery(LPDIRECT3DDEVICE9EX  iface, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	
	DbgPrint("IDirect3DDevice9Impl_CreateQuery called\n");	
	
    return IDirect3DDevice9_CreateQuery(This->lRealD3D9DeviceEx, Type, ppQuery);
}

static HRESULT WINAPI IDirect3DDevice9Impl_SetConvolutionMonoKernel(LPDIRECT3DDEVICE9EX iface,
        UINT width, UINT height, float *rows, float *columns)
{
    FIXME("iface %p, width %u, height %u, rows %p, columns %p stub!\n",
            iface, width, height, rows, columns);
			
	DbgPrint("IDirect3DDevice9Impl_SetConvolutionMonoKernel called\n");				

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9Impl_ComposeRects(LPDIRECT3DDEVICE9EX iface,
        IDirect3DSurface9 *src_surface, IDirect3DSurface9 *dst_surface, IDirect3DVertexBuffer9 *src_descs,
        UINT rect_count, IDirect3DVertexBuffer9 *dst_descs, D3DCOMPOSERECTSOP operation, INT offset_x, INT offset_y)
{
    FIXME("iface %p, src_surface %p, dst_surface %p, src_descs %p, rect_count %u, "
            "dst_descs %p, operation %#x, offset_x %u, offset_y %u stub!\n",
            iface, src_surface, dst_surface, src_descs, rect_count,
            dst_descs, operation, offset_x, offset_y);
			
	DbgPrint("IDirect3DDevice9Impl_ComposeRects called\n");				

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9Impl_PresentEx(LPDIRECT3DDEVICE9EX iface,
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);	
	
	DbgPrint("IDirect3DDevice9Impl_PresentEx called\n");	
	
	return IDirect3DDevice9_Present(This->lRealD3D9DeviceEx, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

static HRESULT WINAPI IDirect3DDevice9Impl_GetGPUThreadPriority(LPDIRECT3DDEVICE9EX iface, INT *priority)
{
    FIXME("iface %p, priority %p stub!\n", iface, priority);
	
	DbgPrint("IDirect3DDevice9Impl_GetGPUThreadPriority called\n");	

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9Impl_SetGPUThreadPriority(LPDIRECT3DDEVICE9EX iface, INT priority)
{
    FIXME("iface %p, priority %d stub!\n", iface, priority);
	
	DbgPrint("IDirect3DDevice9Impl_SetGPUThreadPriority called\n");	

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9Impl_WaitForVBlank(LPDIRECT3DDEVICE9EX iface, UINT swapchain_idx)
{
    FIXME("iface %p, swapchain_idx %u stub!\n", iface, swapchain_idx);
	
	DbgPrint("IDirect3DDevice9Impl_WaitForVBlank called\n");	

    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9Impl_CheckResourceResidency(LPDIRECT3DDEVICE9EX iface,
        IDirect3DResource9 **resources, UINT32 resource_count)
{
    FIXME("iface %p, resources %p, resource_count %u stub!\n",
            iface, resources, resource_count);
	
	DbgPrint("IDirect3DDevice9Impl_CheckResourceResidency called\n");	

    return E_NOTIMPL;
}

static UINT MaxLatency;

static HRESULT WINAPI IDirect3DDevice9Impl_SetMaximumFrameLatency(LPDIRECT3DDEVICE9EX iface, UINT max_latency)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);

    // TRACE("iface %p, max_latency %u.\n", iface, max_latency);

    // if (max_latency > 30)
        // return D3DERR_INVALIDCALL;
	
	MaxLatency = max_latency;
	
	DbgPrint("IDirect3DDevice9Impl_SetMaximumFrameLatency called\n");	

    // wined3d_mutex_lock();
    // wined3d_device_set_max_frame_latency(device->wined3d_device, max_latency);
    // wined3d_mutex_unlock();

    return S_OK;
}

static HRESULT WINAPI IDirect3DDevice9Impl_GetMaximumFrameLatency(LPDIRECT3DDEVICE9EX iface, UINT *max_latency)
{
    // struct d3d9_device *device = impl_from_IDirect3DDevice9Ex(iface);

    // TRACE("iface %p, max_latency %p.\n", iface, max_latency);
	
	*max_latency = MaxLatency;
	
	DbgPrint("IDirect3DDevice9Impl_GetMaximumFrameLatency called\n");	

    // wined3d_mutex_lock();
    // *max_latency = wined3d_device_get_max_frame_latency(device->wined3d_device);
    // wined3d_mutex_unlock();

    return S_OK;
}

static HRESULT WINAPI IDirect3DDevice9Impl_CheckDeviceState(LPDIRECT3DDEVICE9EX iface, HWND dst_window)
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);	
	
	DbgPrint("IDirect3DDevice9Impl_CheckDeviceState called\n");	

    // Verifica o estado do dispositivo
	hr = IDirect3DDevice9_TestCooperativeLevel(This->lRealD3D9DeviceEx);	

	return hr;
}

static HRESULT WINAPI IDirect3DDevice9Impl_CreateRenderTargetEx(LPDIRECT3DDEVICE9EX iface,
        UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality,
        BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD usage)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);	
	
	DbgPrint("IDirect3DDevice9Impl_CreateRenderTargetEx called\n");		

    // Verifica o estado do dispositivo
	return IDirect3DDevice9_CreateRenderTarget(This->lRealD3D9DeviceEx, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, NULL);	
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

static HRESULT WINAPI IDirect3DDevice9Impl_CreateOffscreenPlainSurfaceEx(LPDIRECT3DDEVICE9EX iface,
        UINT width, UINT height, D3DFORMAT format, D3DPOOL pool, IDirect3DSurface9 **surface,
        HANDLE *shared_handle, DWORD usage)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI IDirect3DDevice9Impl_CreateDepthStencilSurfaceEx(LPDIRECT3DDEVICE9EX iface,
        UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality,
        BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD usage)
{
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);	
	
	DbgPrint("IDirect3DDevice9Impl_CreateDepthStencilSurfaceEx called\n");		

	return IDirect3DDevice9_CreateDepthStencilSurface(This->lRealD3D9DeviceEx, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, NULL);
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

static HRESULT WINAPI IDirect3DDevice9Impl_ResetEx(LPDIRECT3DDEVICE9EX iface,
        D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *mode)
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

	// LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
	// HRESULT Ret;
	
	// DbgPrint("IDirect3DDevice9Impl_ResetEx called\n");

    // Ret =  IDirect3DDevice9_Reset(This->lRealD3D9DeviceEx, pPresentationParameters);
	
	// if(Ret == D3D_OK){
		// DbgPrint("IDirect3DDevice9Impl_ResetEx::IDirect3DDevice9_Reset sucessful\n");
	// }	

	// return Ret;	
	return D3D_OK;
}

static HRESULT WINAPI IDirect3DDevice9Impl_GetDisplayModeEx(LPDIRECT3DDEVICE9EX iface,
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
	LPDIRECT3DDEVICE9EX_INT This = IDirect3DDevice9ToImpl(iface);
    
    if (!mode || mode->Size != sizeof(D3DDISPLAYMODEEX)) 
        return D3DERR_INVALIDCALL;
    
    pMode.Width = mode->Width;
    pMode.Height = mode->Height;
    pMode.RefreshRate = mode->RefreshRate;
    pMode.Format = mode->Format;
    
    hr = IDirect3DDevice9_GetDisplayMode(This->lRealD3D9DeviceEx, iSwapChain, &pMode);		

    if (SUCCEEDED(hr)) {       
        mode->Width = pMode.Width;
        mode->Height = pMode.Height;
        mode->RefreshRate = pMode.RefreshRate;
        mode->Format = pMode.Format;
        mode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		
		if(rotation){
			*rotation = 0xdeadbeef;
		}		
    }
    
    return hr; 
}

IDirect3DDevice9ExVtbl_INT Direct3DDevice9Ex_Vtbl =
{
    /* IUnknown */
    IDirect3DDevice9Impl_QueryInterface,
    IDirect3DDevice9Impl_AddRef,
    IDirect3DDevice9Impl_Release,
    /* IDirect3DDevice9 public */
    IDirect3DDevice9Impl_TestCooperativeLevel,
    IDirect3DDevice9Impl_GetAvailableTextureMem,
    IDirect3DDevice9Impl_EvictManagedResources,
    IDirect3DDevice9Impl_GetDirect3D,
    IDirect3DDevice9Impl_GetDeviceCaps,
    IDirect3DDevice9Impl_GetDisplayMode,
    IDirect3DDevice9Impl_GetCreationParameters,
    IDirect3DDevice9Impl_SetCursorProperties,
    IDirect3DDevice9Impl_SetCursorPosition,
    IDirect3DDevice9Impl_ShowCursor,
    IDirect3DDevice9Impl_CreateAdditionalSwapChain,
    IDirect3DDevice9Impl_GetSwapChain,
    IDirect3DDevice9Impl_GetNumberOfSwapChains,
    IDirect3DDevice9Impl_Reset,
    IDirect3DDevice9Impl_Present,
    IDirect3DDevice9Impl_GetBackBuffer,
    IDirect3DDevice9Impl_GetRasterStatus,
    IDirect3DDevice9Impl_SetDialogBoxMode,
    IDirect3DDevice9Impl_SetGammaRamp,
    IDirect3DDevice9Impl_GetGammaRamp,
    IDirect3DDevice9Impl_CreateTexture,
    IDirect3DDevice9Impl_CreateVolumeTexture,
    IDirect3DDevice9Impl_CreateCubeTexture,
    IDirect3DDevice9Impl_CreateVertexBuffer,
    IDirect3DDevice9Impl_CreateIndexBuffer,
    IDirect3DDevice9Impl_CreateRenderTarget,
    IDirect3DDevice9Impl_CreateDepthStencilSurface,
    IDirect3DDevice9Impl_UpdateSurface,
    IDirect3DDevice9Impl_UpdateTexture,
    IDirect3DDevice9Impl_GetRenderTargetData,
    IDirect3DDevice9Impl_GetFrontBufferData,
    IDirect3DDevice9Impl_StretchRect,
    IDirect3DDevice9Impl_ColorFill,
    IDirect3DDevice9Impl_CreateOffscreenPlainSurface,
    IDirect3DDevice9Impl_SetRenderTarget,
    IDirect3DDevice9Impl_GetRenderTarget,
    IDirect3DDevice9Impl_SetDepthStencilSurface,
    IDirect3DDevice9Impl_GetDepthStencilSurface,
    IDirect3DDevice9Impl_BeginScene,
    IDirect3DDevice9Impl_EndScene,
    IDirect3DDevice9Impl_Clear,
    IDirect3DDevice9Impl_SetTransform,
    IDirect3DDevice9Impl_GetTransform,
    IDirect3DDevice9Impl_MultiplyTransform,
    IDirect3DDevice9Impl_SetViewport,
    IDirect3DDevice9Impl_GetViewport,
    IDirect3DDevice9Impl_SetMaterial,
    IDirect3DDevice9Impl_GetMaterial,
    IDirect3DDevice9Impl_SetLight,
    IDirect3DDevice9Impl_GetLight,
    IDirect3DDevice9Impl_LightEnable,
    IDirect3DDevice9Impl_GetLightEnable,
    IDirect3DDevice9Impl_SetClipPlane,
    IDirect3DDevice9Impl_GetClipPlane,
    IDirect3DDevice9Impl_SetRenderState,
    IDirect3DDevice9Impl_GetRenderState,
    IDirect3DDevice9Impl_CreateStateBlock,
    IDirect3DDevice9Impl_BeginStateBlock,
    IDirect3DDevice9Impl_EndStateBlock,
    IDirect3DDevice9Impl_SetClipStatus,
    IDirect3DDevice9Impl_GetClipStatus,
    IDirect3DDevice9Impl_GetTexture,
    IDirect3DDevice9Impl_SetTexture,
    IDirect3DDevice9Impl_GetTextureStageState,
    IDirect3DDevice9Impl_SetTextureStageState,
    IDirect3DDevice9Impl_GetSamplerState,
    IDirect3DDevice9Impl_SetSamplerState,
    IDirect3DDevice9Impl_ValidateDevice,
    IDirect3DDevice9Impl_SetPaletteEntries,
    IDirect3DDevice9Impl_GetPaletteEntries,
    IDirect3DDevice9Impl_SetCurrentTexturePalette,
    IDirect3DDevice9Impl_GetCurrentTexturePalette,
    IDirect3DDevice9Impl_SetScissorRect,
    IDirect3DDevice9Impl_GetScissorRect,
    IDirect3DDevice9Impl_SetSoftwareVertexProcessing,
    IDirect3DDevice9Impl_GetSoftwareVertexProcessing,
    IDirect3DDevice9Impl_SetNPatchMode,
    IDirect3DDevice9Impl_GetNPatchMode,
    IDirect3DDevice9Impl_DrawPrimitive,
    IDirect3DDevice9Impl_DrawIndexedPrimitive,
    IDirect3DDevice9Impl_DrawPrimitiveUP,
    IDirect3DDevice9Impl_DrawIndexedPrimitiveUP,
    IDirect3DDevice9Impl_ProcessVertices,
    IDirect3DDevice9Impl_CreateVertexDeclaration,
    IDirect3DDevice9Impl_SetVertexDeclaration,
    IDirect3DDevice9Impl_GetVertexDeclaration,
    IDirect3DDevice9Impl_SetFVF,
    IDirect3DDevice9Impl_GetFVF,
    IDirect3DDevice9Impl_CreateVertexShader,
    IDirect3DDevice9Impl_SetVertexShader,
    IDirect3DDevice9Impl_GetVertexShader,
    IDirect3DDevice9Impl_SetVertexShaderConstantF,
    IDirect3DDevice9Impl_GetVertexShaderConstantF,
    IDirect3DDevice9Impl_SetVertexShaderConstantI,
    IDirect3DDevice9Impl_GetVertexShaderConstantI,
    IDirect3DDevice9Impl_SetVertexShaderConstantB,
    IDirect3DDevice9Impl_GetVertexShaderConstantB,
    IDirect3DDevice9Impl_SetStreamSource,
    IDirect3DDevice9Impl_GetStreamSource,
    IDirect3DDevice9Impl_SetStreamSourceFreq,
    IDirect3DDevice9Impl_GetStreamSourceFreq,
    IDirect3DDevice9Impl_SetIndices,
    IDirect3DDevice9Impl_GetIndices,
    IDirect3DDevice9Impl_CreatePixelShader,
    IDirect3DDevice9Impl_SetPixelShader,
    IDirect3DDevice9Impl_GetPixelShader,
    IDirect3DDevice9Impl_SetPixelShaderConstantF,
    IDirect3DDevice9Impl_GetPixelShaderConstantF,
    IDirect3DDevice9Impl_SetPixelShaderConstantI,
    IDirect3DDevice9Impl_GetPixelShaderConstantI,
    IDirect3DDevice9Impl_SetPixelShaderConstantB,
    IDirect3DDevice9Impl_GetPixelShaderConstantB,
    IDirect3DDevice9Impl_DrawRectPatch,
    IDirect3DDevice9Impl_DrawTriPatch,
    IDirect3DDevice9Impl_DeletePatch,
    IDirect3DDevice9Impl_CreateQuery,
    /* LPDIRECT3DDEVICE9EX */
    IDirect3DDevice9Impl_SetConvolutionMonoKernel,
    IDirect3DDevice9Impl_ComposeRects,
    IDirect3DDevice9Impl_PresentEx,
    IDirect3DDevice9Impl_GetGPUThreadPriority,
    IDirect3DDevice9Impl_SetGPUThreadPriority,
    IDirect3DDevice9Impl_WaitForVBlank,
    IDirect3DDevice9Impl_CheckResourceResidency,
    IDirect3DDevice9Impl_SetMaximumFrameLatency,
    IDirect3DDevice9Impl_GetMaximumFrameLatency,
    IDirect3DDevice9Impl_CheckDeviceState,
    IDirect3DDevice9Impl_CreateRenderTargetEx,
    IDirect3DDevice9Impl_CreateOffscreenPlainSurfaceEx,
    IDirect3DDevice9Impl_CreateDepthStencilSurfaceEx,
    IDirect3DDevice9Impl_ResetEx,
    IDirect3DDevice9Impl_GetDisplayModeEx	
};