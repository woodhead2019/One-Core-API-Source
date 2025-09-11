#include "d3d9ex_wrapper.h"
#include <windows.h>
#include <cassert>
#include <string>

typedef IDirect3D9* (WINAPI *Direct3DCreate9_t)(UINT SDKVersion);
static Direct3DCreate9_t Direct3DCreate9_original = nullptr;
static HMODULE hOriginalD3D9 = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        char systemDir[MAX_PATH];
        if (GetSystemDirectoryA(systemDir, MAX_PATH) == 0) {
            return FALSE;
        }
        std::string d3d9Path = std::string(systemDir) + "\\d3d9.dll";
        hOriginalD3D9 = LoadLibraryA(d3d9Path.c_str());
        if (!hOriginalD3D9) {
            return FALSE;
        }
        Direct3DCreate9_original = reinterpret_cast<Direct3DCreate9_t>(GetProcAddress(hOriginalD3D9, "Direct3DCreate9"));
        if (!Direct3DCreate9_original) {
            FreeLibrary(hOriginalD3D9);
            hOriginalD3D9 = nullptr;
            return FALSE;
        }
        break;
    }
    case DLL_PROCESS_DETACH:
        if (hOriginalD3D9) {
            FreeLibrary(hOriginalD3D9);
            hOriginalD3D9 = nullptr;
        }
        Direct3DCreate9_original = nullptr;
        break;
    }
    return TRUE;
}

extern "C" HRESULT WINAPI Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D) {
    if (!Direct3DCreate9_original || !ppD3D) return D3DERR_NOTAVAILABLE;
    *ppD3D = nullptr;
    IDirect3D9* pD3D9 = Direct3DCreate9_original(SDKVersion);
    if (!pD3D9) return D3DERR_NOTAVAILABLE;

    MyDirect3D9Ex* pWrapper = new MyDirect3D9Ex(pD3D9);
    if (!pWrapper) {
        pD3D9->Release();
        return E_OUTOFMEMORY;
    }
    *ppD3D = pWrapper;
    return D3D_OK;
}

MyDirect3D9Ex::MyDirect3D9Ex(IDirect3D9* pD3D9) : m_pD3D9(pD3D9), m_refCount(1) {
    assert(pD3D9 != nullptr);
}

MyDirect3D9Ex::~MyDirect3D9Ex() {
    if (m_pD3D9) m_pD3D9->Release();
}

HRESULT MyDirect3D9Ex::QueryInterface(REFIID riid, void** ppvObj) {
    if (!ppvObj) return E_POINTER;
    *ppvObj = nullptr;
    if (riid == IID_IUnknown || riid == IID_IDirect3D9 || riid == IID_IDirect3D9Ex) {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }
    return m_pD3D9->QueryInterface(riid, ppvObj);
}

ULONG MyDirect3D9Ex::AddRef() {
    return InterlockedIncrement(&m_refCount);
}

ULONG MyDirect3D9Ex::Release() {
    ULONG count = InterlockedDecrement(&m_refCount);
    if (count == 0) delete this;
    return count;
}

HRESULT MyDirect3D9Ex::RegisterSoftwareDevice(void* pInitializeFunction) { return m_pD3D9->RegisterSoftwareDevice(pInitializeFunction); }
UINT MyDirect3D9Ex::GetAdapterCount() { return m_pD3D9->GetAdapterCount(); }
HRESULT MyDirect3D9Ex::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) { return m_pD3D9->GetAdapterIdentifier(Adapter, Flags, pIdentifier); }
UINT MyDirect3D9Ex::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) { return m_pD3D9->GetAdapterModeCount(Adapter, Format); }
HRESULT MyDirect3D9Ex::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) { return m_pD3D9->EnumAdapterModes(Adapter, Format, Mode, pMode); }
HRESULT MyDirect3D9Ex::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) { return m_pD3D9->GetAdapterDisplayMode(Adapter, pMode); }
HRESULT MyDirect3D9Ex::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) { return m_pD3D9->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed); }
HRESULT MyDirect3D9Ex::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) { return m_pD3D9->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat); }
HRESULT MyDirect3D9Ex::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) { return m_pD3D9->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels); }
HRESULT MyDirect3D9Ex::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) { return m_pD3D9->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat); }
HRESULT MyDirect3D9Ex::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) { return m_pD3D9->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat); }
HRESULT MyDirect3D9Ex::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) { return m_pD3D9->GetDeviceCaps(Adapter, DeviceType, pCaps); }
HMONITOR MyDirect3D9Ex::GetAdapterMonitor(UINT Adapter) { return m_pD3D9->GetAdapterMonitor(Adapter); }
HRESULT MyDirect3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) { return m_pD3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface); }

UINT MyDirect3D9Ex::GetAdapterModeCountEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter) {
    return pFilter ? m_pD3D9->GetAdapterModeCount(Adapter, pFilter->Format) : 0;
}

HRESULT MyDirect3D9Ex::EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode) {
    if (!pFilter || !pMode) return D3DERR_INVALIDCALL;
    D3DDISPLAYMODE mode;
    HRESULT hr = m_pD3D9->EnumAdapterModes(Adapter, pFilter->Format, Mode, &mode);
    if (SUCCEEDED(hr)) {
        pMode->Width = mode.Width;
        pMode->Height = mode.Height;
        pMode->RefreshRate = mode.RefreshRate;
        pMode->Format = mode.Format;
        pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
        pMode->Size = sizeof(D3DDISPLAYMODEEX);
    }
    return hr;
}

HRESULT MyDirect3D9Ex::GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) {
    if (!pMode) return D3DERR_INVALIDCALL;
    D3DDISPLAYMODE mode;
    HRESULT hr = m_pD3D9->GetAdapterDisplayMode(Adapter, &mode);
    if (SUCCEEDED(hr)) {
        pMode->Width = mode.Width;
        pMode->Height = mode.Height;
        pMode->RefreshRate = mode.RefreshRate;
        pMode->Format = mode.Format;
        pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
        pMode->Size = sizeof(D3DDISPLAYMODEEX);
        if (pRotation) *pRotation = D3DDISPLAYROTATION_IDENTITY;
    }
    return hr;
}

HRESULT MyDirect3D9Ex::CreateDeviceEx(
    UINT Adapter,
    D3DDEVTYPE DeviceType,
    HWND hFocusWindow,
    DWORD BehaviorFlags,
    D3DPRESENT_PARAMETERS* pPresentationParameters,
    D3DDISPLAYMODEEX* pFullscreenDisplayMode,
    IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
    if (!ppReturnedDeviceInterface) {
        return D3DERR_INVALIDCALL;
    }

    return m_pD3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, (IDirect3DDevice9**)ppReturnedDeviceInterface);
}

HRESULT MyDirect3D9Ex::GetAdapterLUID(UINT Adapter, LUID* pLUID) {
    if (!pLUID) return D3DERR_INVALIDCALL;
    D3DADAPTER_IDENTIFIER9 identifier;
    HRESULT hr = m_pD3D9->GetAdapterIdentifier(Adapter, 0, &identifier);
    if (SUCCEEDED(hr)) {
        pLUID->LowPart = identifier.DeviceId;
        pLUID->HighPart = identifier.VendorId;
    }
    return hr;
}

MyDirect3DDevice9Ex::MyDirect3DDevice9Ex(IDirect3DDevice9* pDevice) : m_pDevice(pDevice), m_refCount(1), m_gpuThreadPriority(0), m_maxFrameLatency(3) {
    assert(pDevice != nullptr);
}

MyDirect3DDevice9Ex::~MyDirect3DDevice9Ex() {
    if (m_pDevice) m_pDevice->Release();
}

HRESULT MyDirect3DDevice9Ex::QueryInterface(REFIID riid, void** ppvObj) {
    if (!ppvObj) return E_POINTER;
    *ppvObj = nullptr;
    if (riid == IID_IUnknown || riid == IID_IDirect3DDevice9 || riid == IID_IDirect3DDevice9Ex) {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG MyDirect3DDevice9Ex::AddRef() {
    return InterlockedIncrement(&m_refCount);
}

ULONG MyDirect3DDevice9Ex::Release() {
    ULONG count = InterlockedDecrement(&m_refCount);
    if (count == 0) delete this;
    return count;
}

HRESULT MyDirect3DDevice9Ex::TestCooperativeLevel()
{
    HRESULT hr = m_pDevice->TestCooperativeLevel();
    if (hr == D3DERR_DEVICELOST)
    {
        return D3D_OK;
    }
    return hr;
}
UINT MyDirect3DDevice9Ex::GetAvailableTextureMem() { return m_pDevice->GetAvailableTextureMem(); }
HRESULT MyDirect3DDevice9Ex::EvictManagedResources() { return m_pDevice->EvictManagedResources(); }
HRESULT MyDirect3DDevice9Ex::GetDirect3D(IDirect3D9** ppD3D9) { return m_pDevice->GetDirect3D(ppD3D9); }
HRESULT MyDirect3DDevice9Ex::GetDeviceCaps(D3DCAPS9* pCaps) { return m_pDevice->GetDeviceCaps(pCaps); }
HRESULT MyDirect3DDevice9Ex::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) { return m_pDevice->GetDisplayMode(iSwapChain, pMode); }
HRESULT MyDirect3DDevice9Ex::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) { return m_pDevice->GetCreationParameters(pParameters); }
HRESULT MyDirect3DDevice9Ex::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) { return m_pDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap); }
void MyDirect3DDevice9Ex::SetCursorPosition(int X, int Y, DWORD Flags) { m_pDevice->SetCursorPosition(X, Y, Flags); }
BOOL MyDirect3DDevice9Ex::ShowCursor(BOOL bShow) { return m_pDevice->ShowCursor(bShow); }
HRESULT MyDirect3DDevice9Ex::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) { return m_pDevice->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain); }
HRESULT MyDirect3DDevice9Ex::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) { return m_pDevice->GetSwapChain(iSwapChain, pSwapChain); }
UINT MyDirect3DDevice9Ex::GetNumberOfSwapChains() { return m_pDevice->GetNumberOfSwapChains(); }
HRESULT MyDirect3DDevice9Ex::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    return m_pDevice->Reset(pPresentationParameters);
}
HRESULT MyDirect3DDevice9Ex::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) { return m_pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion); }
HRESULT MyDirect3DDevice9Ex::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) { return m_pDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer); }
HRESULT MyDirect3DDevice9Ex::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) { return m_pDevice->GetRasterStatus(iSwapChain, pRasterStatus); }
HRESULT MyDirect3DDevice9Ex::SetDialogBoxMode(BOOL bEnableDialogs) { return m_pDevice->SetDialogBoxMode(bEnableDialogs); }
void MyDirect3DDevice9Ex::SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp) { m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp); }
void MyDirect3DDevice9Ex::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) { m_pDevice->GetGammaRamp(iSwapChain, pRamp); }
HRESULT MyDirect3DDevice9Ex::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) { return m_pDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) { return m_pDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) { return m_pDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) { return m_pDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) { return m_pDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return m_pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return m_pDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestPoint) { return m_pDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint); }
HRESULT MyDirect3DDevice9Ex::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) { return m_pDevice->UpdateTexture(pSourceTexture, pDestinationTexture); }
HRESULT MyDirect3DDevice9Ex::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) { return m_pDevice->GetRenderTargetData(pRenderTarget, pDestSurface); }
HRESULT MyDirect3DDevice9Ex::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) { return m_pDevice->GetFrontBufferData(iSwapChain, pDestSurface); }
HRESULT MyDirect3DDevice9Ex::StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) { return m_pDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter); }
HRESULT MyDirect3DDevice9Ex::ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color) { return m_pDevice->ColorFill(pSurface, pRect, color); }
HRESULT MyDirect3DDevice9Ex::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return m_pDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle); }
HRESULT MyDirect3DDevice9Ex::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) { return m_pDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget); }
HRESULT MyDirect3DDevice9Ex::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) { return m_pDevice->GetRenderTarget(RenderTargetIndex, ppRenderTarget); }
HRESULT MyDirect3DDevice9Ex::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) { return m_pDevice->SetDepthStencilSurface(pNewZStencil); }
HRESULT MyDirect3DDevice9Ex::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) { return m_pDevice->GetDepthStencilSurface(ppZStencilSurface); }
HRESULT MyDirect3DDevice9Ex::BeginScene() { return m_pDevice->BeginScene(); }
HRESULT MyDirect3DDevice9Ex::EndScene() { return m_pDevice->EndScene(); }
HRESULT MyDirect3DDevice9Ex::Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { return m_pDevice->Clear(Count, pRects, Flags, Color, Z, Stencil); }
HRESULT MyDirect3DDevice9Ex::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) { return m_pDevice->SetTransform(State, pMatrix); }
HRESULT MyDirect3DDevice9Ex::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) { return m_pDevice->GetTransform(State, pMatrix); }
HRESULT MyDirect3DDevice9Ex::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) { return m_pDevice->MultiplyTransform(State, pMatrix); }
HRESULT MyDirect3DDevice9Ex::SetViewport(const D3DVIEWPORT9* pViewport) { return m_pDevice->SetViewport(pViewport); }
HRESULT MyDirect3DDevice9Ex::GetViewport(D3DVIEWPORT9* pViewport) { return m_pDevice->GetViewport(pViewport); }
HRESULT MyDirect3DDevice9Ex::SetMaterial(const D3DMATERIAL9* pMaterial) { return m_pDevice->SetMaterial(pMaterial); }
HRESULT MyDirect3DDevice9Ex::GetMaterial(D3DMATERIAL9* pMaterial) { return m_pDevice->GetMaterial(pMaterial); }
HRESULT MyDirect3DDevice9Ex::SetLight(DWORD Index, const D3DLIGHT9* pLight) { return m_pDevice->SetLight(Index, pLight); }
HRESULT MyDirect3DDevice9Ex::GetLight(DWORD Index, D3DLIGHT9* pLight) { return m_pDevice->GetLight(Index, pLight); }
HRESULT MyDirect3DDevice9Ex::LightEnable(DWORD Index, BOOL Enable) { return m_pDevice->LightEnable(Index, Enable); }
HRESULT MyDirect3DDevice9Ex::GetLightEnable(DWORD Index, BOOL* pEnable) { return m_pDevice->GetLightEnable(Index, pEnable); }
HRESULT MyDirect3DDevice9Ex::SetClipPlane(DWORD Index, const float* pPlane) { return m_pDevice->SetClipPlane(Index, pPlane); }
HRESULT MyDirect3DDevice9Ex::GetClipPlane(DWORD Index, float* pPlane) { return m_pDevice->GetClipPlane(Index, pPlane); }
HRESULT MyDirect3DDevice9Ex::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { return m_pDevice->SetRenderState(State, Value); }
HRESULT MyDirect3DDevice9Ex::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { return m_pDevice->GetRenderState(State, pValue); }
HRESULT MyDirect3DDevice9Ex::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) { return m_pDevice->CreateStateBlock(Type, ppSB); }
HRESULT MyDirect3DDevice9Ex::BeginStateBlock() { return m_pDevice->BeginStateBlock(); }
HRESULT MyDirect3DDevice9Ex::EndStateBlock(IDirect3DStateBlock9** ppSB) { return m_pDevice->EndStateBlock(ppSB); }
HRESULT MyDirect3DDevice9Ex::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus) { return m_pDevice->SetClipStatus(pClipStatus); }
HRESULT MyDirect3DDevice9Ex::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) { return m_pDevice->GetClipStatus(pClipStatus); }
HRESULT MyDirect3DDevice9Ex::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) { return m_pDevice->GetTexture(Stage, ppTexture); }
HRESULT MyDirect3DDevice9Ex::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) { return m_pDevice->SetTexture(Stage, pTexture); }
HRESULT MyDirect3DDevice9Ex::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) { return m_pDevice->GetTextureStageState(Stage, Type, pValue); }
HRESULT MyDirect3DDevice9Ex::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) { return m_pDevice->SetTextureStageState(Stage, Type, Value); }
HRESULT MyDirect3DDevice9Ex::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) { return m_pDevice->GetSamplerState(Sampler, Type, pValue); }
HRESULT MyDirect3DDevice9Ex::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { return m_pDevice->SetSamplerState(Sampler, Type, Value); }
HRESULT MyDirect3DDevice9Ex::ValidateDevice(DWORD* pNumPasses) { return m_pDevice->ValidateDevice(pNumPasses); }
HRESULT MyDirect3DDevice9Ex::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) { return m_pDevice->SetPaletteEntries(PaletteNumber, pEntries); }
HRESULT MyDirect3DDevice9Ex::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) { return m_pDevice->GetPaletteEntries(PaletteNumber, pEntries); }
HRESULT MyDirect3DDevice9Ex::SetCurrentTexturePalette(UINT PaletteNumber) { return m_pDevice->SetCurrentTexturePalette(PaletteNumber); }
HRESULT MyDirect3DDevice9Ex::GetCurrentTexturePalette(UINT* PaletteNumber) { return m_pDevice->GetCurrentTexturePalette(PaletteNumber); }
HRESULT MyDirect3DDevice9Ex::SetScissorRect(const RECT* pRect) { return m_pDevice->SetScissorRect(pRect); }
HRESULT MyDirect3DDevice9Ex::GetScissorRect(RECT* pRect) { return m_pDevice->GetScissorRect(pRect); }
HRESULT MyDirect3DDevice9Ex::SetSoftwareVertexProcessing(BOOL bSoftware) { return m_pDevice->SetSoftwareVertexProcessing(bSoftware); }
BOOL MyDirect3DDevice9Ex::GetSoftwareVertexProcessing() { return m_pDevice->GetSoftwareVertexProcessing(); }
HRESULT MyDirect3DDevice9Ex::SetNPatchMode(float nSegments) { return m_pDevice->SetNPatchMode(nSegments); }
float MyDirect3DDevice9Ex::GetNPatchMode() { return m_pDevice->GetNPatchMode(); }
HRESULT MyDirect3DDevice9Ex::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { return m_pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); }
HRESULT MyDirect3DDevice9Ex::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) { return m_pDevice->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount); }
HRESULT MyDirect3DDevice9Ex::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return m_pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride); }
HRESULT MyDirect3DDevice9Ex::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return m_pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); }
HRESULT MyDirect3DDevice9Ex::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) { return m_pDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags); }
HRESULT MyDirect3DDevice9Ex::CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) { return m_pDevice->CreateVertexDeclaration(pVertexElements, ppDecl); }
HRESULT MyDirect3DDevice9Ex::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) { return m_pDevice->SetVertexDeclaration(pDecl); }
HRESULT MyDirect3DDevice9Ex::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) { return m_pDevice->GetVertexDeclaration(ppDecl); }
HRESULT MyDirect3DDevice9Ex::SetFVF(DWORD FVF) { return m_pDevice->SetFVF(FVF); }
HRESULT MyDirect3DDevice9Ex::GetFVF(DWORD* pFVF) { return m_pDevice->GetFVF(pFVF); }
HRESULT MyDirect3DDevice9Ex::CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader) { return m_pDevice->CreateVertexShader(pFunction, ppShader); }
HRESULT MyDirect3DDevice9Ex::SetVertexShader(IDirect3DVertexShader9* pShader) { return m_pDevice->SetVertexShader(pShader); }
HRESULT MyDirect3DDevice9Ex::GetVertexShader(IDirect3DVertexShader9** ppShader) { return m_pDevice->GetVertexShader(ppShader); }
HRESULT MyDirect3DDevice9Ex::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) { return m_pDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT MyDirect3DDevice9Ex::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return m_pDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT MyDirect3DDevice9Ex::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) { return m_pDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT MyDirect3DDevice9Ex::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return m_pDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT MyDirect3DDevice9Ex::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) { return m_pDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT MyDirect3DDevice9Ex::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return m_pDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT MyDirect3DDevice9Ex::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) { return m_pDevice->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride); }
HRESULT MyDirect3DDevice9Ex::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride) { return m_pDevice->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride); }
HRESULT MyDirect3DDevice9Ex::SetStreamSourceFreq(UINT StreamNumber, UINT Setting) { return m_pDevice->SetStreamSourceFreq(StreamNumber, Setting); }
HRESULT MyDirect3DDevice9Ex::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) { return m_pDevice->GetStreamSourceFreq(StreamNumber, pSetting); }
HRESULT MyDirect3DDevice9Ex::SetIndices(IDirect3DIndexBuffer9* pIndexData) { return m_pDevice->SetIndices(pIndexData); }
HRESULT MyDirect3DDevice9Ex::GetIndices(IDirect3DIndexBuffer9** ppIndexData) { return m_pDevice->GetIndices(ppIndexData); }
HRESULT MyDirect3DDevice9Ex::CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader) { return m_pDevice->CreatePixelShader(pFunction, ppShader); }
HRESULT MyDirect3DDevice9Ex::SetPixelShader(IDirect3DPixelShader9* pShader) { return m_pDevice->SetPixelShader(pShader); }
HRESULT MyDirect3DDevice9Ex::GetPixelShader(IDirect3DPixelShader9** ppShader) { return m_pDevice->GetPixelShader(ppShader); }
HRESULT MyDirect3DDevice9Ex::SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) { return m_pDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT MyDirect3DDevice9Ex::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return m_pDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT MyDirect3DDevice9Ex::SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) { return m_pDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT MyDirect3DDevice9Ex::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return m_pDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT MyDirect3DDevice9Ex::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) { return m_pDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT MyDirect3DDevice9Ex::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return m_pDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT MyDirect3DDevice9Ex::DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo) { return m_pDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo); }
HRESULT MyDirect3DDevice9Ex::DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo) { return m_pDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo); }
HRESULT MyDirect3DDevice9Ex::DeletePatch(UINT Handle) { return m_pDevice->DeletePatch(Handle); }
HRESULT MyDirect3DDevice9Ex::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) { return m_pDevice->CreateQuery(Type, ppQuery); }
HRESULT MyDirect3DDevice9Ex::SetConvolutionMonoKernel(UINT width, UINT height, float* rows, float* columns) { return D3DERR_NOTAVAILABLE; }
HRESULT MyDirect3DDevice9Ex::ComposeRects(IDirect3DSurface9* pSrc, IDirect3DSurface9* pDst, IDirect3DVertexBuffer9* pSrcRectDescs, UINT NumRects, IDirect3DVertexBuffer9* pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset) {
    if (!pSrc || !pDst || !pSrcRectDescs || !pDstRectDescs || NumRects == 0) return D3DERR_INVALIDCALL;
    if (Operation != D3DCOMPOSERECTS_COPY) return D3DERR_NOTAVAILABLE;

    struct ComposRectDesc {
        USHORT X, Y, Width, Height;
    };
    ComposRectDesc* pSrcDescs = nullptr;
    ComposRectDesc* pDstDescs = nullptr;
    HRESULT hr = pSrcRectDescs->Lock(0, NumRects * sizeof(ComposRectDesc), reinterpret_cast<void**>(&pSrcDescs), D3DLOCK_READONLY);
    if (FAILED(hr)) return hr;
    hr = pDstRectDescs->Lock(0, NumRects * sizeof(ComposRectDesc), reinterpret_cast<void**>(&pDstDescs), D3DLOCK_READONLY);
    if (FAILED(hr)) {
        pSrcRectDescs->Unlock();
        return hr;
    }

    D3DSURFACE_DESC dstDesc;
    pDst->GetDesc(&dstDesc);

    for (UINT i = 0; i < NumRects; ++i) {
        RECT src = { pSrcDescs[i].X, pSrcDescs[i].Y, pSrcDescs[i].X + pSrcDescs[i].Width, pSrcDescs[i].Y + pSrcDescs[i].Height };
        RECT dst = { pDstDescs[i].X + Xoffset, pDstDescs[i].Y + Yoffset, pDstDescs[i].X + Xoffset + pDstDescs[i].Width, pDstDescs[i].Y + Yoffset + pDstDescs[i].Height };

        if (dst.left < 0) dst.left = 0;
        if (dst.top < 0) dst.top = 0;
        if (dst.right > static_cast<LONG>(dstDesc.Width)) dst.right = static_cast<LONG>(dstDesc.Width);
        if (dst.bottom > static_cast<LONG>(dstDesc.Height)) dst.bottom = static_cast<LONG>(dstDesc.Height);

        LONG dstWidth = dst.right - dst.left;
        LONG dstHeight = dst.bottom - dst.top;
        src.right = src.left + dstWidth;
        src.bottom = src.top + dstHeight;

        hr = m_pDevice->StretchRect(pSrc, &src, pDst, &dst, D3DTEXF_POINT);
        if (FAILED(hr)) break;
    }

    pSrcRectDescs->Unlock();
    pDstRectDescs->Unlock();
    return hr;
}

HRESULT MyDirect3DDevice9Ex::PresentEx(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags) {
    if (dwFlags & D3DPRESENT_DONOTWAIT) {
        HRESULT hr = m_pDevice->TestCooperativeLevel();
        if (hr == D3DERR_DEVICELOST) {
            return D3DERR_DEVICELOST;
        }
    }
    return m_pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT MyDirect3DDevice9Ex::GetGPUThreadPriority(INT* pPriority) {
    if (!pPriority) return D3DERR_INVALIDCALL;
    *pPriority = m_gpuThreadPriority;
    return D3D_OK;
}

HRESULT MyDirect3DDevice9Ex::SetGPUThreadPriority(INT Priority) {
    if (Priority < -7 || Priority > 7) return D3DERR_INVALIDCALL;
    m_gpuThreadPriority = Priority;
    return D3D_OK;
}

HRESULT MyDirect3DDevice9Ex::WaitForVBlank(UINT iSwapChain) {
    return SUCCEEDED(m_pDevice->TestCooperativeLevel()) ? D3D_OK : D3DERR_DEVICELOST;
}

HRESULT MyDirect3DDevice9Ex::CheckResourceResidency(IDirect3DResource9** pResourceArray, UINT32 NumResources) { return D3D_OK; }

HRESULT MyDirect3DDevice9Ex::SetMaximumFrameLatency(UINT MaxLatency) {
    if (MaxLatency == 0 || MaxLatency > 16) return D3DERR_INVALIDCALL;
    m_maxFrameLatency = MaxLatency;
    return D3D_OK;
}

HRESULT MyDirect3DDevice9Ex::GetMaximumFrameLatency(UINT* pMaxLatency) {
    if (!pMaxLatency) return D3DERR_INVALIDCALL;
    *pMaxLatency = m_maxFrameLatency;
    return D3D_OK;
}

HRESULT MyDirect3DDevice9Ex::CheckDeviceState(HWND hDestinationWindow) {
    return SUCCEEDED(m_pDevice->TestCooperativeLevel()) ? D3D_OK : D3DERR_DEVICELOST;
}

HRESULT MyDirect3DDevice9Ex::CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) {
    return m_pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT MyDirect3DDevice9Ex::CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) {
    return m_pDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT MyDirect3DDevice9Ex::CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) {
    return m_pDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT MyDirect3DDevice9Ex::ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode)
{
    if (!pPresentationParameters || !pPresentationParameters->hDeviceWindow || !IsWindow(pPresentationParameters->hDeviceWindow)) {
        return D3DERR_INVALIDCALL;
    }

    if (pFullscreenDisplayMode) {
        pPresentationParameters->BackBufferWidth  = pFullscreenDisplayMode->Width;
        pPresentationParameters->BackBufferHeight = pFullscreenDisplayMode->Height;
        pPresentationParameters->BackBufferFormat = pFullscreenDisplayMode->Format;
        pPresentationParameters->FullScreen_RefreshRateInHz = pFullscreenDisplayMode->RefreshRate;
    }

    return m_pDevice->Reset(pPresentationParameters);
}

HRESULT MyDirect3DDevice9Ex::GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) {
    if (!pMode) return D3DERR_INVALIDCALL;
    D3DDISPLAYMODE mode;
    HRESULT hr = m_pDevice->GetDisplayMode(iSwapChain, &mode);
    if (SUCCEEDED(hr)) {
        pMode->Width = mode.Width;
        pMode->Height = mode.Height;
        pMode->RefreshRate = mode.RefreshRate;
        pMode->Format = mode.Format;
        pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
        pMode->Size = sizeof(D3DDISPLAYMODEEX);
        if (pRotation) *pRotation = D3DDISPLAYROTATION_IDENTITY;
    }
    return hr;
}