/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS ReactX
 * FILE:            dll/directx/d3d9/d3d9.c
 * PURPOSE:         d3d9.dll implementation
 * PROGRAMERS:      Magnus Olsen <greatlrd (at) reactos (dot) org>
 *                  Gregor Brunmar <gregor (dot) brunmar (at) home (dot) se>
 */

#include <d3d9.h>
#include "d3d9_helpers.h"
#include "d3d9_create.h"
#include <debug.h>
#include <reactos/debug.h>

#define DEBUG_MESSAGE_BUFFER_SIZE   512
#define D3D9b_SDK_VERSION 31 //Need be moved to d3d9.h

WINE_DEFAULT_DEBUG_CHANNEL(d3d9);

LPDIRECT3D9 ProxyInterface = 0;
LPDIRECT3DDEVICE9 DeviceProxyInterface = 0;
LPDIRECT3DBASETEXTURE9 BaseTextureProxyInterface = 0;
LPDIRECT3DSWAPCHAIN9 SwapChainProxyInterface = 0;
LPDIRECT3DTEXTURE9 TextureProxyInterface = 0;

typedef IDirect3D9* (WINAPI *LPDIRECT3DCREATE9)(UINT);

static LPCSTR D3dError_WrongSdkVersion =
    "D3D ERROR: D3D header version mismatch.\n"
    "The application was compiled against and will only work with "
    "D3D_SDK_VERSION (%d), but the currently installed runtime is "
    "version (%d).\n"
    "Recompile the application against the appropriate SDK for the installed runtime.\n"
    "\n";

// HRESULT WINAPI Direct3DShaderValidatorCreate9(void)
// {
    // UNIMPLEMENTED;
    // return 0;
// }

// HRESULT WINAPI PSGPError(void)
// {
    // UNIMPLEMENTED;
    // return 0;
// }

// HRESULT WINAPI PSGPSampleTexture(void)
// {
    // UNIMPLEMENTED;
    // return 0;
// }

// HRESULT WINAPI DebugSetLevel(void)
// {
    // UNIMPLEMENTED;
    // return 0;
// }

// HRESULT WINAPI DebugSetMute(DWORD dw1)
// {
    // UNIMPLEMENTED;
    // return 0;
// }

// IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion)
// {
    // HINSTANCE hDebugDll;
    // DWORD LoadDebugDll;
    // DWORD LoadDebugDllSize;
    // LPDIRECT3D9 D3D9Obj = 0;
    // LPDIRECT3DCREATE9 DebugDirect3DCreate9 = 0;
    // CHAR DebugMessageBuffer[DEBUG_MESSAGE_BUFFER_SIZE];
    // UINT NoDebugSDKVersion = SDKVersion & ~DX_D3D9_DEBUG;

    // LoadDebugDllSize = sizeof(LoadDebugDll);
    // if (ReadRegistryValue(REG_DWORD, "LoadDebugRuntime", (LPBYTE)&LoadDebugDll, &LoadDebugDllSize))
    // {
        // if (0 != LoadDebugDll)
        // {
            // hDebugDll = LoadLibraryA("d3d9d.dll");

            // if (0 != hDebugDll)
            // {
                // DebugDirect3DCreate9 = (LPDIRECT3DCREATE9)GetProcAddress(hDebugDll, "Direct3DCreate9");

                // return DebugDirect3DCreate9(SDKVersion);
            // }
        // }
    // }

    // if (NoDebugSDKVersion != D3D_SDK_VERSION && NoDebugSDKVersion != D3D9b_SDK_VERSION)
    // {
        // if (SDKVersion & DX_D3D9_DEBUG)
        // {
            // HRESULT hResult;
            // hResult = SafeFormatString(DebugMessageBuffer, DEBUG_MESSAGE_BUFFER_SIZE, D3dError_WrongSdkVersion, NoDebugSDKVersion, D3D_SDK_VERSION);
            // if (SUCCEEDED(hResult))
                // OutputDebugStringA(DebugMessageBuffer);
        // }

        // return NULL;
    // }

    // CreateD3D9(&D3D9Obj, SDKVersion);

    // return D3D9Obj;
// }

HRESULT CreateD3D9Ex(OUT LPDIRECT3D9EX *ppDirect3D9, UINT SDKVersion, LPDIRECT3D9 d3dObj, BOOL pIsD3d9Ex)
{
    LPDIRECT3D9EX_INT pDirect3D9;

    if (d3dObj == 0)
        return DDERR_INVALIDPARAMS;

    if (AlignedAlloc((LPVOID *)&pDirect3D9, sizeof(DIRECT3D9_INT)) != S_OK)
        return DDERR_OUTOFMEMORY;

    if (pDirect3D9 == 0)
        return DDERR_OUTOFMEMORY;

    pDirect3D9->lpVtbl = &Direct3D9Ex_Vtbl;
    pDirect3D9->dwProcessId = GetCurrentThreadId();
    pDirect3D9->lRefCnt = 1;
    pDirect3D9->lRealD3D9 = d3dObj;
    pDirect3D9->SDKVersion = SDKVersion;
    pDirect3D9->lpInt = pDirect3D9;
    pDirect3D9->IsD3d9Ex = pIsD3d9Ex;

    InitializeCriticalSection(&pDirect3D9->d3d9_cs);

    // if (FALSE == GetDisplayDeviceInfo(pDirect3D9))
    // {
        // DPRINT1("Could not create Direct3D9 object");
        // AlignedFree(pDirect3D9);
        // return DDERR_GENERIC;
    // }

    *ppDirect3D9 = (LPDIRECT3D9EX)&pDirect3D9->lpVtbl;

    return D3D_OK;
}

IDirect3D9* WINAPI Direct3DCreate9Internal(UINT SDKVersion)
{
    LPDIRECT3D9EX D3d9ExObj = 0;
	LPDIRECT3D9 D3dObj;
	
	//ProxyInterface = Direct3DCreate9(D3D_SDK_VERSION);
	D3dObj = Direct3DCreate9(D3D_SDK_VERSION);

    CreateD3D9Ex(&D3d9ExObj, SDKVersion, D3dObj, FALSE);
	
	return (IDirect3D9*)D3d9ExObj;
    //return ProxyInterface;
}

HRESULT WINAPI Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **d3d9Ex)
{
    LPDIRECT3D9EX D3d9ExObj = 0;
	LPDIRECT3D9 D3dObj;
	
	//ProxyInterface = Direct3DCreate9(D3D_SDK_VERSION);
	D3dObj = Direct3DCreate9(D3D_SDK_VERSION);
	
	DbgPrint("Direct3DCreate9Ex called\n");

    CreateD3D9Ex(&D3d9ExObj, SDKVersion, D3dObj, TRUE);
	
	*d3d9Ex = D3d9ExObj;

    return D3D_OK;
    //return ProxyInterface;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

// /***********************************************************************
 // *              D3DPERF_SetOptions (D3D9.@)
 // *
 // */
// void WINAPI D3DPERF_SetOptions(DWORD options)
// {
  // FIXME("(%#x) : stub\n", options);
// }