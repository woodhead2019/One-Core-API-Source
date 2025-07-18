/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS ReactX
 * FILE:            dll/directx/d3d9/d3d9_device.h
 * PURPOSE:         d3d9.dll internal device structures
 * PROGRAMERS:      Gregor Brunmar <gregor (dot) brunmar (at) home (dot) se>
 */
#ifndef _D3D9_DEVICE_H_
#define _D3D9_DEVICE_H_

#include "d3d9_common.h"
#include <d3d9.h>
#include <d3d9types.h>
#include "d3d9_private.h"
#include "d3d9_swapchain.h"
#include "d3d9_surface.h"

#if !defined(__cplusplus) || defined(CINTERFACE)
typedef struct _IDirect3DDevice9Vtbl_INT
{
    struct IDirect3DDevice9Vtbl PublicInterface;
} IDirect3DDevice9Vtbl_INT;

typedef struct _IDirect3DDevice9ExVtbl_INT
{
    struct IDirect3DDevice9ExVtbl PublicInterface;
} IDirect3DDevice9ExVtbl_INT;
#endif

typedef struct _D3D9HeapTexture
{
/* 0x0000 */    DWORD dwUnknown00;
/* 0x0004 */    DWORD dwUnknown04;  // 0x400
/* 0x0008 */    LPDWORD pUnknown08; // malloc(dwUnknown04 * 2)
} D3D9HeapTexture;

typedef struct _D3D9ResourceManager
{
#ifdef D3D_DEBUG_INFO
/* N/A    - 0x0000 */   DDSURFACEDESC SurfaceDesc[8];
#endif
/* 0x0000 - 0x0160 */   struct _Direct3DDevice9_INT* pBaseDevice;
/* 0x0004 - 0x0164 */   DWORD dwUnknown0004;
/* 0x0008 - 0x0168 */   DWORD dwUnknown0008;
/* 0x000c - 0x016c */   DWORD MaxSimultaneousTextures;
/* 0x0010 - 0x0170 */   DWORD dwUnknown0010;
/* 0x0014 - 0x0174 */   D3D9HeapTexture* pTextureHeap;
} D3D9ResourceManager;

typedef struct _Direct3DDevice9_INT
{
/* 0x0000 */    struct _IDirect3DDevice9ExVtbl_INT* lpVtbl;
/* 0x0004 */    CRITICAL_SECTION CriticalSection;
#ifdef D3D_DEBUG_INFO
/* N/A    - 0x001c */   DWORD dbg0004;
/* N/A    - 0x0020 */   DWORD dbg0008;
/* N/A    - 0x0024 */   DWORD dbg000c;
/* N/A    - 0x0028 */   DWORD dbg0010;
/* N/A    - 0x002c */   DWORD dbg0014;
/* N/A    - 0x0030 */   DWORD dbg0018;
/* N/A    - 0x0034 */   DWORD dbg001c;
/* N/A    - 0x0038 */   DWORD dbg0020;
/* N/A    - 0x003c */   DWORD dbg0024;
/* N/A    - 0x0040 */   DWORD dbg0028;
/* N/A    - 0x0044 */   DWORD dbg002c;
/* N/A    - 0x0048 */   DWORD dbg0030;
/* N/A    - 0x004c */   DWORD dbg0034;
/* N/A    - 0x0050 */   DWORD dbg0038;
#endif
/* 0x001c - 0x0054 */   BOOL bLockDevice;
/* 0x0020 - 0x0058 */   DWORD dwProcessId;
/* 0x0024 - 0x005c */   IUnknown* pUnknown;
/* 0x0028 - 0x0060 */   DWORD dwDXVersion;
/* 0x002c - 0x0064 */   DWORD unknown000011;
/* 0x0030 - 0x0068 */   LONG lRefCnt;
/* 0x0034 - 0x006c */   DWORD unknown000013;
/* 0x0038 - 0x0070 */   D3D9ResourceManager* pResourceManager;
/* 0x003c - 0x0074 */   HWND hWnd;
/* 0x0040 - 0x0078 */   DWORD AdjustedBehaviourFlags;
/* 0x0044 - 0x007c */   DWORD BehaviourFlags;
/* 0x0048 - 0x0080 */   D3D9BaseSurface* pUnknown0010;
/* 0x004c - 0x0084 */   DWORD NumAdaptersInDevice;
/* 0x0050 - 0x0088 */   D3DDISPLAYMODE CurrentDisplayMode[D3D9_INT_MAX_NUM_ADAPTERS];
/* 0x0110 - 0x0148 */   DWORD AdapterIndexInGroup[D3D9_INT_MAX_NUM_ADAPTERS]; 
/* 0x0140 - 0x0178 */   D3D9_DEVICEDATA DeviceData[D3D9_INT_MAX_NUM_ADAPTERS];
/* 0x1df0 - 0x1e28 */   LPDIRECT3DSWAPCHAIN9_INT pSwapChains[D3D9_INT_MAX_NUM_ADAPTERS];
/* 0x1e20 - 0x1e58 */   LPDIRECT3DSWAPCHAIN9_INT pSwapChains2[D3D9_INT_MAX_NUM_ADAPTERS];
/* 0x1e50 */    D3D9BaseSurface* pRenderTargetList;
/* 0x1e54 */    DWORD unknown001941;
/* 0x1e58 */    DWORD unknown001942;
/* 0x1e5c */    DWORD unknown001943;
/* 0x1e60 */    D3D9BaseSurface* pUnknown001944;
/* 0x1e64 */    D3DDEVTYPE DeviceType;
/* 0x1e68 */    LPDIRECT3D9_INT pDirect3D9;
/* 0x1e6c */    D3D9DriverSurface* pDriverSurfaceList;
/* 0x1e70 */    DWORD unknown001948;
/* 0x1e74 */    HANDLE hDX10UMDriver;
/* 0x1e78 */    HANDLE hDX10UMDriverInst;
} DIRECT3DDEVICE9_INT, FAR* LPDIRECT3DDEVICE9_INT;

/* Helper functions */
LPDIRECT3DDEVICE9_INT IDirect3DDevice9ToImpl(LPDIRECT3DDEVICE9EX  iface);

/* IUnknown interface */
HRESULT WINAPI IDirect3DDevice9Base_QueryInterface(LPDIRECT3DDEVICE9EX  iface, REFIID riid, void** ppvObject);
ULONG WINAPI IDirect3DDevice9Base_AddRef(LPDIRECT3DDEVICE9EX  iface);
ULONG WINAPI IDirect3DDevice9Base_Release(LPDIRECT3DDEVICE9EX  iface);

/* IDirect3DDevice9 public interface */
HRESULT WINAPI IDirect3DDevice9Base_TestCooperativeLevel(LPDIRECT3DDEVICE9EX  iface);
UINT WINAPI IDirect3DDevice9Base_GetAvailableTextureMem(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Base_EvictManagedResources(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Base_GetDirect3D(LPDIRECT3DDEVICE9EX  iface, IDirect3D9** ppD3D9);
HRESULT WINAPI IDirect3DDevice9Base_GetDeviceCaps(LPDIRECT3DDEVICE9EX  iface, D3DCAPS9* pCaps);
HRESULT WINAPI IDirect3DDevice9Base_GetDisplayMode(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DDISPLAYMODE* pMode);
HRESULT WINAPI IDirect3DDevice9Base_GetCreationParameters(LPDIRECT3DDEVICE9EX  iface, D3DDEVICE_CREATION_PARAMETERS* pParameters);
HRESULT WINAPI IDirect3DDevice9Base_SetCursorProperties(LPDIRECT3DDEVICE9EX  iface, UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap);
VOID WINAPI IDirect3DDevice9Base_SetCursorPosition(LPDIRECT3DDEVICE9EX  iface, int X, int Y, DWORD Flags);
BOOL WINAPI IDirect3DDevice9Base_ShowCursor(LPDIRECT3DDEVICE9EX  iface, BOOL bShow);
HRESULT WINAPI IDirect3DDevice9Base_CreateAdditionalSwapChain(LPDIRECT3DDEVICE9EX  iface, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** ppSwapChain);
HRESULT WINAPI IDirect3DDevice9Base_GetSwapChain(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, IDirect3DSwapChain9** ppSwapChain);
UINT WINAPI IDirect3DDevice9Base_GetNumberOfSwapChains(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Base_Reset(LPDIRECT3DDEVICE9EX  iface, D3DPRESENT_PARAMETERS* pPresentationParameters);
HRESULT WINAPI IDirect3DDevice9Base_Present(LPDIRECT3DDEVICE9EX  iface, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
HRESULT WINAPI IDirect3DDevice9Base_GetBackBuffer(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
HRESULT WINAPI IDirect3DDevice9Base_GetRasterStatus(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);
HRESULT WINAPI IDirect3DDevice9Base_SetDialogBoxMode(LPDIRECT3DDEVICE9EX  iface, BOOL bEnableDialogs);
VOID WINAPI IDirect3DDevice9Base_SetGammaRamp(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp);
VOID WINAPI IDirect3DDevice9Base_GetGammaRamp(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, D3DGAMMARAMP* pRamp);
HRESULT WINAPI IDirect3DDevice9Base_CreateTexture(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
HRESULT WINAPI IDirect3DDevice9Base_CreateVolumeTexture(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle);
HRESULT WINAPI IDirect3DDevice9Base_CreateCubeTexture(LPDIRECT3DDEVICE9EX  iface, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle);
HRESULT WINAPI IDirect3DDevice9Base_CreateVertexBuffer(LPDIRECT3DDEVICE9EX  iface, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle);
HRESULT WINAPI IDirect3DDevice9Base_CreateIndexBuffer(LPDIRECT3DDEVICE9EX  iface, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle);
HRESULT WINAPI IDirect3DDevice9Base_CreateRenderTarget(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
HRESULT WINAPI IDirect3DDevice9Base_CreateDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
HRESULT WINAPI IDirect3DDevice9Base_UpdateSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint);
HRESULT WINAPI IDirect3DDevice9Base_UpdateTexture(LPDIRECT3DDEVICE9EX  iface, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture);
HRESULT WINAPI IDirect3DDevice9Base_GetRenderTargetData(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);
HRESULT WINAPI IDirect3DDevice9Base_GetFrontBufferData(LPDIRECT3DDEVICE9EX  iface, UINT iSwapChain, IDirect3DSurface9* pDestSurface);
HRESULT WINAPI IDirect3DDevice9Base_StretchRect(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter);
HRESULT WINAPI IDirect3DDevice9Base_ColorFill(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color);
HRESULT WINAPI IDirect3DDevice9Base_CreateOffscreenPlainSurface(LPDIRECT3DDEVICE9EX  iface, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
/* IDirect3D9Device public interface */
HRESULT WINAPI IDirect3DDevice9Pure_SetRenderTarget(LPDIRECT3DDEVICE9EX  iface, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
HRESULT WINAPI IDirect3DDevice9Pure_GetRenderTarget(LPDIRECT3DDEVICE9EX  iface, DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
HRESULT WINAPI IDirect3DDevice9Pure_SetDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9* pNewZStencil);
HRESULT WINAPI IDirect3DDevice9Pure_GetDepthStencilSurface(LPDIRECT3DDEVICE9EX  iface, IDirect3DSurface9** ppZStencilSurface);
HRESULT WINAPI IDirect3DDevice9Pure_BeginScene(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Pure_EndScene(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Pure_Clear(LPDIRECT3DDEVICE9EX  iface, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
HRESULT WINAPI IDirect3DDevice9Pure_SetTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
HRESULT WINAPI IDirect3DDevice9Pure_GetTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
HRESULT WINAPI IDirect3DDevice9Pure_MultiplyTransform(LPDIRECT3DDEVICE9EX  iface, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
HRESULT WINAPI IDirect3DDevice9Pure_SetViewport(LPDIRECT3DDEVICE9EX  iface, CONST D3DVIEWPORT9* pViewport);
HRESULT WINAPI IDirect3DDevice9Pure_GetViewport(LPDIRECT3DDEVICE9EX  iface, D3DVIEWPORT9* pViewport);
HRESULT WINAPI IDirect3DDevice9Pure_SetMaterial(LPDIRECT3DDEVICE9EX  iface, CONST D3DMATERIAL9* pMaterial);
HRESULT WINAPI IDirect3DDevice9Pure_GetMaterial(LPDIRECT3DDEVICE9EX  iface, D3DMATERIAL9* pMaterial);
HRESULT WINAPI IDirect3DDevice9Pure_SetLight(LPDIRECT3DDEVICE9EX  iface, DWORD Index, CONST D3DLIGHT9* pLight);
HRESULT WINAPI IDirect3DDevice9Pure_GetLight(LPDIRECT3DDEVICE9EX  iface, DWORD Index, D3DLIGHT9* pLight);
HRESULT WINAPI IDirect3DDevice9Pure_LightEnable(LPDIRECT3DDEVICE9EX  iface, DWORD Index, BOOL Enable);
HRESULT WINAPI IDirect3DDevice9Pure_GetLightEnable(LPDIRECT3DDEVICE9EX  iface, DWORD Index, BOOL* pEnable);
HRESULT WINAPI IDirect3DDevice9Pure_SetClipPlane(LPDIRECT3DDEVICE9EX  iface, DWORD Index, CONST float* pPlane);
HRESULT WINAPI IDirect3DDevice9Pure_GetClipPlane(LPDIRECT3DDEVICE9EX  iface, DWORD Index, float* pPlane);
HRESULT WINAPI IDirect3DDevice9Pure_SetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD Value);
HRESULT WINAPI IDirect3DDevice9Pure_GetRenderState(LPDIRECT3DDEVICE9EX  iface, D3DRENDERSTATETYPE State, DWORD* pValue);
HRESULT WINAPI IDirect3DDevice9Pure_CreateStateBlock(LPDIRECT3DDEVICE9EX  iface, D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB);
HRESULT WINAPI IDirect3DDevice9Pure_BeginStateBlock(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Pure_EndStateBlock(LPDIRECT3DDEVICE9EX  iface, IDirect3DStateBlock9** ppSB);
HRESULT WINAPI IDirect3DDevice9Pure_SetClipStatus(LPDIRECT3DDEVICE9EX  iface, CONST D3DCLIPSTATUS9* pClipStatus);
HRESULT WINAPI IDirect3DDevice9Pure_GetClipStatus(LPDIRECT3DDEVICE9EX  iface, D3DCLIPSTATUS9* pClipStatus);
HRESULT WINAPI IDirect3DDevice9Pure_GetTexture(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, IDirect3DBaseTexture9** ppTexture);
HRESULT WINAPI IDirect3DDevice9Pure_SetTexture(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, IDirect3DBaseTexture9* pTexture);
HRESULT WINAPI IDirect3DDevice9Pure_GetTextureStageState(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue);
HRESULT WINAPI IDirect3DDevice9Pure_SetTextureStageState(LPDIRECT3DDEVICE9EX  iface, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
HRESULT WINAPI IDirect3DDevice9Pure_GetSamplerState(LPDIRECT3DDEVICE9EX  iface, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue);
HRESULT WINAPI IDirect3DDevice9Pure_SetSamplerState(LPDIRECT3DDEVICE9EX  iface, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
HRESULT WINAPI IDirect3DDevice9Pure_ValidateDevice(LPDIRECT3DDEVICE9EX  iface, DWORD* pNumPasses);
HRESULT WINAPI IDirect3DDevice9Pure_SetPaletteEntries(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber, CONST PALETTEENTRY* pEntries);
HRESULT WINAPI IDirect3DDevice9Pure_GetPaletteEntries(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber, PALETTEENTRY* pEntries);
HRESULT WINAPI IDirect3DDevice9Pure_SetCurrentTexturePalette(LPDIRECT3DDEVICE9EX  iface, UINT PaletteNumber);
HRESULT WINAPI IDirect3DDevice9Pure_GetCurrentTexturePalette(LPDIRECT3DDEVICE9EX  iface, UINT* pPaletteNumber);
HRESULT WINAPI IDirect3DDevice9Pure_SetScissorRect(LPDIRECT3DDEVICE9EX  iface, CONST RECT* pRect);
HRESULT WINAPI IDirect3DDevice9Pure_GetScissorRect(LPDIRECT3DDEVICE9EX  iface, RECT* pRect);
HRESULT WINAPI IDirect3DDevice9Pure_SetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface, BOOL bSoftware);
BOOL WINAPI IDirect3DDevice9Pure_GetSoftwareVertexProcessing(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Pure_SetNPatchMode(LPDIRECT3DDEVICE9EX  iface, float nSegments);
float WINAPI IDirect3DDevice9Pure_GetNPatchMode(LPDIRECT3DDEVICE9EX  iface);
HRESULT WINAPI IDirect3DDevice9Pure_DrawPrimitive(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
HRESULT WINAPI IDirect3DDevice9Pure_DrawIndexedPrimitive(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
HRESULT WINAPI IDirect3DDevice9Pure_DrawPrimitiveUP(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
HRESULT WINAPI IDirect3DDevice9Pure_DrawIndexedPrimitiveUP(LPDIRECT3DDEVICE9EX  iface, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
HRESULT WINAPI IDirect3DDevice9Pure_ProcessVertices(LPDIRECT3DDEVICE9EX  iface, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags);
HRESULT WINAPI IDirect3DDevice9Pure_CreateVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl);
HRESULT WINAPI IDirect3DDevice9Pure_SetVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexDeclaration9* pDecl);
HRESULT WINAPI IDirect3DDevice9Pure_GetVertexDeclaration(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexDeclaration9** ppDecl);
HRESULT WINAPI IDirect3DDevice9Pure_SetFVF(LPDIRECT3DDEVICE9EX  iface, DWORD FVF);
HRESULT WINAPI IDirect3DDevice9Pure_GetFVF(LPDIRECT3DDEVICE9EX  iface, DWORD* pFVF);
HRESULT WINAPI IDirect3DDevice9Pure_CreateVertexShader(LPDIRECT3DDEVICE9EX  iface, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader);
HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexShader9* pShader);
HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DVertexShader9** ppShader);
HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, float* pConstantData, UINT Vector4fCount);
HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, int* pConstantData, UINT Vector4iCount);
HRESULT WINAPI IDirect3DDevice9Pure_SetVertexShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount);
HRESULT WINAPI IDirect3DDevice9Pure_GetVertexShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
HRESULT WINAPI IDirect3DDevice9Pure_SetStreamSource(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
HRESULT WINAPI IDirect3DDevice9Pure_GetStreamSource(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride);
HRESULT WINAPI IDirect3DDevice9Pure_SetStreamSourceFreq(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber,UINT Setting);
HRESULT WINAPI IDirect3DDevice9Pure_GetStreamSourceFreq(LPDIRECT3DDEVICE9EX  iface, UINT StreamNumber, UINT* pSetting);
HRESULT WINAPI IDirect3DDevice9Pure_SetIndices(LPDIRECT3DDEVICE9EX  iface, IDirect3DIndexBuffer9* pIndexData);
HRESULT WINAPI IDirect3DDevice9Pure_GetIndices(LPDIRECT3DDEVICE9EX  iface, IDirect3DIndexBuffer9** ppIndexData);
HRESULT WINAPI IDirect3DDevice9Pure_CreatePixelShader(LPDIRECT3DDEVICE9EX  iface, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader);
HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DPixelShader9* pShader);
HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShader(LPDIRECT3DDEVICE9EX  iface, IDirect3DPixelShader9** ppShader);
HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShaderConstantF(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, float* pConstantData, UINT Vector4fCount);
HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShaderConstantI(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, int* pConstantData, UINT Vector4iCount);
HRESULT WINAPI IDirect3DDevice9Pure_SetPixelShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount);
HRESULT WINAPI IDirect3DDevice9Pure_GetPixelShaderConstantB(LPDIRECT3DDEVICE9EX  iface, UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
HRESULT WINAPI IDirect3DDevice9Pure_DrawRectPatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo);
HRESULT WINAPI IDirect3DDevice9Pure_DrawTriPatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo);
HRESULT WINAPI IDirect3DDevice9Pure_DeletePatch(LPDIRECT3DDEVICE9EX  iface, UINT Handle);
HRESULT WINAPI IDirect3DDevice9Pure_CreateQuery(LPDIRECT3DDEVICE9EX  iface, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery);

/* IDirect3DDevice9 private interface */
VOID WINAPI IDirect3DDevice9Base_Destroy(LPDIRECT3DDEVICE9EX  iface);
VOID WINAPI IDirect3DDevice9Base_VirtualDestructor(LPDIRECT3DDEVICE9EX  iface);

#if !defined(__cplusplus) || defined(CINTERFACE)
extern IDirect3DDevice9ExVtbl_INT Direct3DDevice9Ex_Vtbl;
extern IDirect3DSwapChain9ExVtbl Direct3DSwapChain9Ex_Vtbl;
#endif

typedef struct _D3D9PUREDEVICE
{
/* 0x0000 - 0x0000 */   DIRECT3DDEVICE9_INT BaseDevice;
} D3D9PUREDEVICE, FAR *LPD3D9PUREDEVICE;

typedef struct _D3D9HALDEVICE
{
/* 0x0000 */    D3D9PUREDEVICE PureDevice;
} D3D9HALDEVICE, FAR *LPD3D9HALDEVICE;

#endif /* _D3D9_DEVICE_H_ */
