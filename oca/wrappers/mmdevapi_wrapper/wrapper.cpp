// wrapper.cpp
#include <initguid.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <strsafe.h>
#include <new>
#include <cstdint>
#include "wrapper.h"
#pragma comment(lib, "propsys.lib")
using std::vector;
using std::wstring;
HINSTANCE g_hInstance = NULL;
MyDeviceEnumerator* g_enumerator = nullptr;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_hInstance = hModule;
        DisableThreadLibraryCalls(hModule);
        g_enumerator = new MyDeviceEnumerator();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (g_enumerator && g_objects == 1) {
            delete g_enumerator;
            g_enumerator = nullptr;
        }
        break;
    }
    return TRUE;
}
const GUID CLSID_MMDeviceEnumerator = { 0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e} };
const GUID IID_IMMDeviceEnumerator = { 0xa95664d2, 0x9614, 0x4f35, {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6} };
const GUID IID_IMMDeviceCollection = { 0x0bd7a1be, 0x7a1a, 0x44db, {0x83, 0x97, 0xcc, 0x53, 0x92, 0x38, 0x7b, 0x5e} };
const GUID IID_IMMDevice = { 0xd666063f, 0x1587, 0x4e43, {0x81, 0xf1, 0xb9, 0x48, 0xe8, 0x07, 0x36, 0x3f} };
const GUID IID_IAudioClient = { 0x1cb9ad4c, 0xdbfa, 0x4c32, {0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2} };
const GUID IID_IMMEndpoint = { 0x1be09788, 0x6894, 0x4089, {0x85, 0x86, 0x9a, 0x2a, 0x6c, 0x26, 0x5a, 0xc5} };
const GUID IID_IAudioClient2 = { 0x726778cd, 0xf60a, 0x4eda, {0x82, 0xde, 0xe4, 0x76, 0x10, 0xcd, 0x78, 0xaa} };
const GUID IID_IAudioClient3 = { 0x7ed4ee07, 0x8e67, 0x4cd4, {0x8c, 0x1a, 0x2b, 0x7a, 0x59, 0x87, 0xad, 0x42} };
const GUID IID_IAudioClock = { 0xcd63314f, 0x3fba, 0x4a1b, {0x81, 0x2c, 0xef, 0x96, 0x35, 0x87, 0x28, 0xe7} };
const GUID IID_IAudioClock2 = { 0x6f49ff73, 0x6727, 0x49ac, {0xa0, 0x08, 0xd9, 0x8c, 0xf5, 0xe7, 0x00, 0x48} };
const GUID IID_IDeviceTopology = { 0x2a07407e, 0x6497, 0x4a18, {0x97, 0x87, 0x32, 0xf7, 0x9b, 0xd0, 0xd9, 0x8f} };
const GUID IID_IAudioClockAdjustment = { 0xf6e4c0a0, 0x0d25, 0x4d84, {0xaa, 0x3c, 0xaf, 0x33, 0x5d, 0x87, 0x74, 0x60} };
const GUID IID_IAudioEndpointVolume = { 0x5cdf2c82, 0x841e, 0x4546, {0x97, 0x22, 0x0c, 0xf7, 0x40, 0x78, 0x22, 0x9a} };
const GUID IID_IAudioSessionManager = { 0xbfa971f1, 0x4d5e, 0x40bb, {0x93, 0x5e, 0x96, 0x70, 0x39, 0xbf, 0xbe, 0xe4} };
const GUID IID_IAudioSessionManager2 = { 0x77aa99a0, 0x1bd6, 0x484f, {0x8b, 0xc7, 0x2c, 0x65, 0x4c, 0x9a, 0x9b, 0x6f} };
const GUID IID_IAudioSessionControl = { 0xf4b1a599, 0x7266, 0x4319, {0xa8, 0xca, 0xe7, 0x0a, 0xcb, 0x11, 0xe8, 0xcd} };
const GUID IID_IAudioSessionControl2 = { 0xbfb7ff88, 0x7239, 0x4fc9, {0x8f, 0xa2, 0x07, 0xc9, 0x50, 0xbe, 0x9c, 0x6d} };
const GUID IID_ISimpleAudioVolume = { 0x87ce5498, 0x68d6, 0x44de, {0x9f, 0xf3, 0x6d, 0x32, 0xd1, 0xa1, 0x8f, 0x9e} };
const GUID IID_IAudioSessionEnumerator = { 0xe2f5bb11, 0x0570, 0x40ca, {0xac, 0xdd, 0x3a, 0x0a, 0x12, 0x77, 0xde, 0xe8} };
const GUID IID_IAudioSessionNotification = { 0x641dd20b, 0x4d41, 0x49cc, {0xab, 0xa3, 0x17, 0x4b, 0x94, 0x77, 0xbb, 0x08} };
const GUID IID_IAudioSessionEvents = { 0x24918acc, 0x64b3, 0x37c1, {0x8c, 0xa9, 0x74, 0xa6, 0x6e, 0x99, 0x57, 0xa8} };
const GUID IID_IAudioRenderClient = { 0xf294acfc, 0x3146, 0x4483, {0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2} };
const GUID IID_IAudioCaptureClient = { 0xc8adbd64, 0xe71e, 0x48a0, {0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17} };
const GUID IID_IAudioStreamVolume = { 0x93014887, 0x242d, 0x4068, {0x8a, 0x15, 0xcf, 0x5e, 0x93, 0xb9, 0x0c, 0xfe} };
const GUID IID_IActivateAudioInterfaceAsyncOperation = { 0x72a22d78, 0xcde4, 0x431d, {0xb8, 0xcc, 0x84, 0x3a, 0x71, 0x19, 0x9b, 0x6d} };
const GUID IID_IActivateAudioInterfaceCompletionHandler = { 0x41d949ab, 0x9862, 0x444a, {0x80, 0xf6, 0xc2, 0x61, 0x33, 0x4d, 0xa5, 0xeb} };
const GUID IID_IConnector = { 0x9c2c4058, 0x23f5, 0x41de, {0x87, 0x7a, 0xdf, 0x3a, 0xf2, 0x36, 0xa0, 0x9e} };
LONG g_objects = 0;
#define DEVICE_STATE_ACTIVE 0x00000001UL
#define DEVICE_STATE_DISABLED 0x00000002UL
#define DEVICE_STATE_NOTPRESENT 0x00000004UL
#define DEVICE_STATE_UNPLUGGED 0x00000008UL
const DWORD DEVICE_STATE_MASK_ALL = DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED | DEVICE_STATE_NOTPRESENT | DEVICE_STATE_UNPLUGGED;
struct DeviceInfo {
    GUID guid;
    wstring desc;
};
BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext) {
    vector<DeviceInfo>* devices = static_cast<vector<DeviceInfo>*>(lpContext);
    DeviceInfo info;
    if (lpGuid) info.guid = *lpGuid;
    else ZeroMemory(&info.guid, sizeof(GUID));
    info.desc = lpcstrDescription;
    devices->push_back(info);
    return TRUE;
}
static bool IsFloatFormat(const WAVEFORMATEXTENSIBLE& fmt) {
    return (fmt.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT && fmt.Format.wBitsPerSample == 32);
}
static void ResampleFloat(const float* src, UINT32 srcChannels, UINT32 srcFrames, float* dest, UINT32 destChannels, UINT32 destFrames, double ratio) {
    if (srcChannels == destChannels) {
        for (UINT32 i = 0; i < destFrames; ++i) {
            double pos = i / ratio;
            UINT32 ipos = static_cast<UINT32>(pos);
            float frac = static_cast<float>(pos - ipos);
            if (ipos >= srcFrames - 1) {
                frac = 0.0f;
                ipos = srcFrames - 1;
            }
            for (UINT32 c = 0; c < srcChannels; ++c) {
                dest[i * destChannels + c] = src[ipos * srcChannels + c] * (1.0f - frac) + src[(ipos + 1) * srcChannels + c] * frac;
            }
        }
    }
    else if (srcChannels == 1 && destChannels == 2) {
        for (UINT32 i = 0; i < destFrames; ++i) {
            double pos = i / ratio;
            UINT32 ipos = static_cast<UINT32>(pos);
            float frac = static_cast<float>(pos - ipos);
            if (ipos >= srcFrames - 1) {
                frac = 0.0f;
                ipos = srcFrames - 1;
            }
            float sample = src[ipos] * (1.0f - frac) + src[ipos + 1] * frac;
            dest[i * 2] = sample;
            dest[i * 2 + 1] = sample;
        }
    }
    else if (srcChannels == 2 && destChannels == 1) {
        for (UINT32 i = 0; i < destFrames; ++i) {
            double pos = i / ratio;
            UINT32 ipos = static_cast<UINT32>(pos);
            float frac = static_cast<float>(pos - ipos);
            if (ipos >= srcFrames - 1) {
                frac = 0.0f;
                ipos = srcFrames - 1;
            }
            float sample1 = src[ipos * 2] * (1.0f - frac) + src[(ipos + 1) * 2] * frac;
            float sample2 = src[ipos * 2 + 1] * (1.0f - frac) + src[(ipos + 1) * 2 + 1] * frac;
            dest[i] = (sample1 + sample2) * 0.5f;
        }
    }
    else {
        memset(dest, 0, destFrames * destChannels * sizeof(float));
    }
}
static void ConvertToFloat(const BYTE* src, const WAVEFORMATEXTENSIBLE& srcFmt, float* destFloat, UINT32 frames) {
    UINT32 channels = srcFmt.Format.nChannels;
    UINT32 samples = frames * channels;
    UINT32 bits = srcFmt.Format.wBitsPerSample;
    if (IsFloatFormat(srcFmt)) {
        memcpy(destFloat, src, frames * srcFmt.Format.nBlockAlign);
    }
    else if (bits == 8) {
        for (UINT32 i = 0; i < samples; ++i) {
            destFloat[i] = (static_cast<float>(src[i]) - 128.0f) / 128.0f;
        }
    }
    else if (bits == 16) {
        const int16_t* s = reinterpret_cast<const int16_t*>(src);
        for (UINT32 i = 0; i < samples; ++i) {
            destFloat[i] = static_cast<float>(s[i]) / 32768.0f;
        }
    }
    else if (bits == 24) {
        for (UINT32 i = 0; i < samples; ++i) {
            int32_t val = static_cast<int32_t>(src[i * 3]) | (static_cast<int32_t>(src[i * 3 + 1]) << 8) | (static_cast<int32_t>(static_cast<int8_t>(src[i * 3 + 2])) << 16);
            destFloat[i] = static_cast<float>(val) / 8388608.0f;
        }
    }
    else if (bits == 32) {
        const int32_t* s = reinterpret_cast<const int32_t*>(src);
        for (UINT32 i = 0; i < samples; ++i) {
            destFloat[i] = static_cast<float>(s[i]) / 2147483648.0f;
        }
    }
}
static void ConvertFromFloat(const float* fSrc, const WAVEFORMATEXTENSIBLE& destFmt, BYTE* dest, UINT32 frames) {
    UINT32 channels = destFmt.Format.nChannels;
    UINT32 samples = frames * channels;
    UINT32 bits = destFmt.Format.wBitsPerSample;
    if (IsFloatFormat(destFmt)) {
        memcpy(dest, fSrc, frames * destFmt.Format.nBlockAlign);
    }
    else if (bits == 8) {
        for (UINT32 i = 0; i < samples; ++i) {
            float v = std::max(std::min(fSrc[i], 1.0f), -1.0f);
            dest[i] = static_cast<BYTE>(v * 127.0f + 128.0f);
        }
    }
    else if (bits == 16) {
        int16_t* d = reinterpret_cast<int16_t*>(dest);
        for (UINT32 i = 0; i < samples; ++i) {
            float v = std::max(std::min(fSrc[i], 1.0f), -1.0f);
            d[i] = static_cast<int16_t>(v * 32767.0f);
        }
    }
    else if (bits == 24) {
        for (UINT32 i = 0; i < samples; ++i) {
            float v = std::max(std::min(fSrc[i], 1.0f), -1.0f);
            int32_t val = static_cast<int32_t>(v * 8388607.5f - 0.5f);
            dest[i * 3] = val & 0xFF;
            dest[i * 3 + 1] = (val >> 8) & 0xFF;
            dest[i * 3 + 2] = (val >> 16) & 0xFF;
        }
    }
    else if (bits == 32) {
        int32_t* d = reinterpret_cast<int32_t*>(dest);
        for (UINT32 i = 0; i < samples; ++i) {
            float v = std::max(std::min(fSrc[i], 1.0f), -1.0f);
            d[i] = static_cast<int32_t>(v * 2147483647.5f - 0.5f);
        }
    }
}
static void ApplyChannelVolumes(BYTE* data, const WAVEFORMATEXTENSIBLE& fmt, UINT32 frames, const vector<float>& volumes) {
    UINT32 channels = fmt.Format.nChannels;
    if (channels != volumes.size()) return;
    bool allOne = true;
    for (float v : volumes) {
        if (v != 1.0f) {
            allOne = false;
            break;
        }
    }
    if (allOne) return;
    UINT32 samples = frames * channels;
    float* fData = new float[samples];
    if (fData == nullptr) return;
    ConvertToFloat(data, fmt, fData, frames);
    for (UINT32 i = 0; i < frames; ++i) {
        for (UINT32 c = 0; c < channels; ++c) {
            fData[i * channels + c] *= volumes[c];
        }
    }
    ConvertFromFloat(fData, fmt, data, frames);
    delete[] fData;
}
MyAudioSession::MyAudioSession() : ref(1), volume(1.0f), mute(false), state(AudioSessionStateActive) {
    displayName = L"";
    iconPath = L"%SystemRoot%\\system32\\sndvol32.exe,-100"; // XP speaker icon
}
MyAudioSession::~MyAudioSession() {
    for (auto e : events) e->Release();
}
HRESULT __stdcall MyAudioSession::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown) {
        *ppv = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioSession::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioSession::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) {
        g_enumerator->RemoveSession(this);
        delete this;
    }
    return newRef;
}
void MyAudioSession::UpdateVolumes() {
    for (auto c : clients) {
        c->UpdateVolume();
    }
}
void MyAudioSession::SetState(AudioSessionState newState) {
    state = newState;
    for (auto e : events) {
        e->OnStateChanged(state);
    }
}
MyDeviceEnumerator::MyDeviceEnumerator(IUnknown* pUnkOuter) : m_pUnkOuter(pUnkOuter ? pUnkOuter : static_cast<IUnknown*>(this)), m_pUnkMarshal(nullptr), loopBuf(nullptr), loopBytes(0), loopReadPos(0ULL), loopWritePos(0ULL), loopPaddingFrames(0), loopPositionFrames(0ULL), ref(1), masterVolume(1.0f), masterMute(false), loopMicClient(nullptr), loopMicCapture(nullptr), pumpThread(nullptr), loopMicEvent(nullptr), loopbackRunning(false), loopbackCount(0) {
    if (m_pUnkOuter == static_cast<IUnknown*>(this)) {
        CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    }
    InterlockedIncrement(&g_objects);
    InitializeCriticalSection(&loopCS);
    loopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ZeroMemory(&loopFormat, sizeof(loopFormat));
    InitializeCriticalSection(&volumeCS);
}
MyDeviceEnumerator::~MyDeviceEnumerator() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    for (auto p : clients) p->Release();
    vector<MyAudioSession*> temp_sessions = sessions;
    sessions.clear();
    for (auto s : temp_sessions) s->Release();
    for (auto cb : volumeCallbacks) cb->Release();
    if (loopbackRunning) {
        loopbackRunning = false;
        if (pumpThread) {
            WaitForSingleObject(pumpThread, INFINITE);
            CloseHandle(pumpThread);
        }
    }
    if (loopMicEvent) CloseHandle(loopMicEvent);
    if (loopBuf) delete[] loopBuf;
    DeleteCriticalSection(&loopCS);
    DeleteCriticalSection(&volumeCS);
    CloseHandle(loopEvent);
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyDeviceEnumerator::QueryInterface(REFIID iid, void** ppv) {
    if (m_pUnkOuter != static_cast<IUnknown*>(this)) {
        return m_pUnkOuter->QueryInterface(iid, ppv);
    }
    else {
        return NonDelegatingQueryInterface(iid, ppv);
    }
}
ULONG __stdcall MyDeviceEnumerator::AddRef() {
    if (m_pUnkOuter != static_cast<IUnknown*>(this)) {
        return m_pUnkOuter->AddRef();
    }
    else {
        return NonDelegatingAddRef();
    }
}
ULONG __stdcall MyDeviceEnumerator::Release() {
    if (m_pUnkOuter != static_cast<IUnknown*>(this)) {
        return m_pUnkOuter->Release();
    }
    else {
        return NonDelegatingRelease();
    }
}
HRESULT MyDeviceEnumerator::NonDelegatingQueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IMMDeviceEnumerator) {
        *ppv = static_cast<IMMDeviceEnumerator*>(this);
        NonDelegatingAddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG MyDeviceEnumerator::NonDelegatingAddRef() {
    return InterlockedIncrement(&ref);
}
ULONG MyDeviceEnumerator::NonDelegatingRelease() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyDeviceEnumerator::EnumAudioEndpoints(EDataFlow dataFlow, DWORD dwStateMask, IMMDeviceCollection** ppDevices) {
    if (ppDevices == NULL) return E_POINTER;
    *ppDevices = NULL;
    if (dataFlow < eRender || dataFlow > eAll) return E_INVALIDARG;
    if (dwStateMask & ~DEVICE_STATE_MASK_ALL) return E_INVALIDARG;
    if ((dwStateMask & DEVICE_STATE_ACTIVE) == 0) {
        *ppDevices = new MyDeviceCollection(vector<IMMDevice*>());
        return S_OK;
    }
    vector<IMMDevice*> immDevices;
    if (dataFlow == eRender || dataFlow == eAll) {
        vector<DeviceInfo> renderDevices;
        DirectSoundEnumerateW(DSEnumCallback, &renderDevices);
        if (renderDevices.empty()) {
            DeviceInfo dummy;
            ZeroMemory(&dummy.guid, sizeof(GUID));
            dummy.desc = L"Primary Sound Driver";
            renderDevices.push_back(dummy);
        }
        immDevices.reserve(immDevices.size() + renderDevices.size());
        for (const auto& info : renderDevices) {
            immDevices.push_back(new MyDevice(eRender, info.guid, info.desc));
        }
    }
    if (dataFlow == eCapture || dataFlow == eAll) {
        vector<DeviceInfo> captureDevices;
        DirectSoundCaptureEnumerateW(DSEnumCallback, &captureDevices);
        if (captureDevices.empty()) {
            DeviceInfo dummy;
            ZeroMemory(&dummy.guid, sizeof(GUID));
            dummy.desc = L"Primary Sound Capture Driver";
            captureDevices.push_back(dummy);
        }
        immDevices.reserve(immDevices.size() + captureDevices.size());
        for (const auto& info : captureDevices) {
            immDevices.push_back(new MyDevice(eCapture, info.guid, info.desc));
        }
    }
    *ppDevices = new MyDeviceCollection(immDevices);
    return S_OK;
}
HRESULT __stdcall MyDeviceEnumerator::GetDefaultAudioEndpoint(EDataFlow dataFlow, ERole role, IMMDevice** ppEndpoint) {
    if (ppEndpoint == NULL) return E_POINTER;
    *ppEndpoint = NULL;
    if (dataFlow != eRender && dataFlow != eCapture) return E_INVALIDARG;
    if (role < eConsole || role > eCommunications) return E_INVALIDARG;
    vector<DeviceInfo> devs;
    if (dataFlow == eRender) {
        DirectSoundEnumerateW(DSEnumCallback, &devs);
    }
    else {
        DirectSoundCaptureEnumerateW(DSEnumCallback, &devs);
    }
    if (devs.empty()) {
        DeviceInfo dummy;
        ZeroMemory(&dummy.guid, sizeof(GUID));
        dummy.desc = (dataFlow == eRender) ? L"Primary Sound Driver" : L"Primary Sound Capture Driver";
        devs.push_back(dummy);
    }
    DeviceInfo& info = devs[0];
    wstring name = info.desc;
    GUID g = info.guid;
    *ppEndpoint = new MyDevice(dataFlow, g, name);
    return S_OK;
}
HRESULT __stdcall MyDeviceEnumerator::GetDevice(const wchar_t* pwstrId, IMMDevice** ppDevice) {
    if (ppDevice == NULL) return E_POINTER;
    *ppDevice = NULL;
    if (pwstrId == NULL) return E_POINTER;
    bool isRender = wcsncmp(pwstrId, L"dsound-render-", 14) == 0;
    bool isCapture = wcsncmp(pwstrId, L"dsound-capture-", 15) == 0;
    vector<DeviceInfo> devs;
    if (isRender || isCapture) {
        const wchar_t* guidStr = pwstrId + (isRender ? 14 : 15);
        GUID guid;
        HRESULT hr = CLSIDFromString(guidStr, &guid);
        if (FAILED(hr)) return hr;
        if (isRender) DirectSoundEnumerateW(DSEnumCallback, &devs);
        else DirectSoundCaptureEnumerateW(DSEnumCallback, &devs);
        for (const auto& info : devs) {
            if (IsEqualGUID(info.guid, guid)) {
                *ppDevice = new MyDevice(isRender ? eRender : eCapture, guid, info.desc);
                return S_OK;
            }
        }
        return E_NOTFOUND;
    }
    const wchar_t* l = wcschr(pwstrId, L'{');
    const wchar_t* r = wcschr(pwstrId, L'}');
    if (l && r && r > l) {
        wstring guidSub(l, static_cast<size_t>(r - l + 1));
        GUID guid;
        if (SUCCEEDED(CLSIDFromString(guidSub.c_str(), &guid))) {
            // search both render and capture sets
            DirectSoundEnumerateW(DSEnumCallback, &devs);
            for (const auto& info : devs) {
                if (IsEqualGUID(info.guid, guid)) {
                    *ppDevice = new MyDevice(eRender, guid, info.desc);
                    return S_OK;
                }
            }
            DirectSoundCaptureEnumerateW(DSEnumCallback, &devs);
            for (const auto& info : devs) {
                if (IsEqualGUID(info.guid, guid)) {
                    *ppDevice = new MyDevice(eCapture, guid, info.desc);
                    return S_OK;
                }
            }
            // If GUID found in string but not matched to enumerated DirectSound GUIDs,
            // still create a device with that GUID and a generic name to avoid failing activation
            wstring generic = (wcsstr(pwstrId, L"render") ? L"Speakers (virtual)" : L"Microphone (virtual)");
            *ppDevice = new MyDevice(wcsstr(pwstrId, L"render") ? eRender : eCapture, guid, generic);
            return S_OK;
        }
    }
    DirectSoundEnumerateW(DSEnumCallback, &devs);
    for (const auto& info : devs) {
        if (_wcsicmp(info.desc.c_str(), pwstrId) == 0 || wcsstr(info.desc.c_str(), pwstrId) != nullptr || wcsstr(pwstrId, info.desc.c_str()) != nullptr) {
            *ppDevice = new MyDevice(eRender, info.guid, info.desc);
            return S_OK;
        }
    }
    DirectSoundCaptureEnumerateW(DSEnumCallback, &devs);
    for (const auto& info : devs) {
        if (_wcsicmp(info.desc.c_str(), pwstrId) == 0 || wcsstr(info.desc.c_str(), pwstrId) != nullptr || wcsstr(pwstrId, info.desc.c_str()) != nullptr) {
            *ppDevice = new MyDevice(eCapture, info.guid, info.desc);
            return S_OK;
        }
    }
    return E_NOTFOUND;
}
HRESULT __stdcall MyDeviceEnumerator::RegisterEndpointNotificationCallback(IMMNotificationClient* pClient) {
    if (pClient == NULL) return E_POINTER;
    auto it = std::find(clients.begin(), clients.end(), pClient);
    if (it != clients.end()) return S_OK;
    pClient->AddRef();
    clients.push_back(pClient);
    return S_OK;
}
HRESULT __stdcall MyDeviceEnumerator::UnregisterEndpointNotificationCallback(IMMNotificationClient* pClient) {
    if (pClient == NULL) return E_POINTER;
    auto it = std::find(clients.begin(), clients.end(), pClient);
    if (it == clients.end()) return S_OK;
    (*it)->Release();
    clients.erase(it);
    return S_OK;
}
MyAudioSession* MyDeviceEnumerator::GetSession(const GUID& guid, bool create) {
    for (auto s : sessions) {
        if (IsEqualGUID(s->guid, guid)) return s;
    }
    if (!create) return nullptr;
    MyAudioSession* s = new MyAudioSession();
    s->guid = guid;
    s->volume = 1.0f;
    s->mute = false;
    s->state = AudioSessionStateActive;
    sessions.push_back(s);
    return s;
}
void MyDeviceEnumerator::RemoveSession(MyAudioSession* s) {
    auto it = std::find(sessions.begin(), sessions.end(), s);
    if (it != sessions.end()) sessions.erase(it);
}
void MyDeviceEnumerator::UpdateAllVolumes() {
    for (auto s : sessions) {
        s->UpdateVolumes();
    }
}
void MyDeviceEnumerator::NotifyVolumeChange(const GUID* context) {
    AUDIO_VOLUME_NOTIFICATION_DATA data = {};
    data.guidEventContext = context ? *context : GUID_NULL;
    data.bMuted = masterMute;
    data.fMasterVolume = masterVolume;
    data.nChannels = 1;
    data.afChannelVolumes[0] = masterVolume;
    for (auto cb : volumeCallbacks) {
        cb->OnNotify(&data);
    }
}
DWORD WINAPI MyDeviceEnumerator::PumpFromMicToLoop(LPVOID param) {
    CoInitialize(NULL);
    MyDeviceEnumerator* self = static_cast<MyDeviceEnumerator*>(param);
    while (self->loopbackRunning) {
        WaitForSingleObject(self->loopMicEvent, 50);
        while (true) {
            BYTE* data;
            UINT32 frames;
            DWORD flags;
            UINT64 devPos, qpc;
            HRESULT hr = self->loopMicCapture->GetBuffer(&data, &frames, &flags, &devPos, &qpc);
            if (hr == AUDCLNT_S_BUFFER_EMPTY || FAILED(hr) || frames == 0) break;
            EnterCriticalSection(&self->loopCS);
            UINT32 bytes = frames * self->loopFormat.Format.nBlockAlign;
            UINT64 spaceBytes = (self->loopBytes + self->loopReadPos - self->loopWritePos) % self->loopBytes;
            if (bytes > spaceBytes) {
                self->loopReadPos = (self->loopReadPos + (bytes - spaceBytes)) % self->loopBytes;
            }
            UINT32 pos = static_cast<UINT32>(self->loopWritePos % self->loopBytes);
            UINT32 bytes1 = std::min(bytes, self->loopBytes - pos);
            memcpy(self->loopBuf + pos, data, bytes1);
            if (bytes1 < bytes) memcpy(self->loopBuf, data + bytes1, bytes - bytes1);
            self->loopWritePos = (self->loopWritePos + bytes) % self->loopBytes;
            self->loopPaddingFrames += frames;
            self->loopPositionFrames += frames;
            SetEvent(self->loopEvent);
            LeaveCriticalSection(&self->loopCS);
            self->loopMicCapture->ReleaseBuffer(frames);
        }
    }
    CoUninitialize();
    return 0;
}
MyDeviceCollection::MyDeviceCollection(const vector<IMMDevice*>& devs) : ref(1), m_pUnkMarshal(nullptr), devices(devs) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
}
MyDeviceCollection::~MyDeviceCollection() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    for (auto d : devices) d->Release();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyDeviceCollection::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IMMDeviceCollection) {
        *ppv = static_cast<IMMDeviceCollection*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyDeviceCollection::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyDeviceCollection::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyDeviceCollection::GetCount(UINT* pcDevices) {
    if (pcDevices == NULL) return E_POINTER;
    *pcDevices = static_cast<UINT>(devices.size());
    return S_OK;
}
HRESULT __stdcall MyDeviceCollection::Item(UINT nDevice, IMMDevice** ppDevice) {
    if (ppDevice == NULL) return E_POINTER;
    if (nDevice >= devices.size()) {
        *ppDevice = NULL;
        return E_INVALIDARG;
    }
    *ppDevice = devices[nDevice];
    (*ppDevice)->AddRef();
    return S_OK;
}
MyDevice::MyDevice(EDataFlow f, const GUID& g, const wstring& n) : ref(1), flow(f), deviceGuid(g), name(n) {
    wchar_t guidBuf[39];
    StringFromGUID2(deviceGuid, guidBuf, 39);
    id = L"dsound-" + (f == eRender ? wstring(L"render-") : wstring(L"capture-")) + wstring(guidBuf);
    InterlockedIncrement(&g_objects);
}
MyDevice::~MyDevice() {
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyDevice::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IMMDevice) {
        *ppv = static_cast<IMMDevice*>(this);
    }
    else if (iid == IID_IMMEndpoint) {
        *ppv = static_cast<IMMEndpoint*>(this);
    }
    else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}
ULONG __stdcall MyDevice::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyDevice::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyDevice::Activate(REFIID iid, DWORD dwClsCtx, PROPVARIANT* pActivationParams, void** ppInterface) {
    if (ppInterface == NULL) return E_POINTER;
    *ppInterface = NULL;
    if (iid == IID_IAudioClient || iid == IID_IAudioClient2 || iid == IID_IAudioClient3) {
        MyAudioClient* ac = new MyAudioClient(flow, deviceGuid);
        if (ac == nullptr) return E_OUTOFMEMORY;
        HRESULT hr = ac->NonDelegatingQueryInterface(iid, ppInterface);
        ac->NonDelegatingRelease();
        return hr;
    }
    else if (iid == IID_IDeviceTopology) {
        MyDeviceTopology* dt = new MyDeviceTopology(id);
        if (dt == nullptr) return E_OUTOFMEMORY;
        HRESULT hr = dt->QueryInterface(iid, ppInterface);
        dt->Release();
        return hr;
    }
    else if (iid == IID_IAudioEndpointVolume) {
        MyAudioEndpointVolume* vol = new MyAudioEndpointVolume(this);
        if (vol == nullptr) return E_OUTOFMEMORY;
        HRESULT hr = vol->QueryInterface(iid, ppInterface);
        vol->Release();
        return hr;
    }
    else if (iid == IID_IAudioSessionManager || iid == IID_IAudioSessionManager2) {
        MyAudioSessionManager2* mgr = new MyAudioSessionManager2(this);
        if (mgr == nullptr) return E_OUTOFMEMORY;
        HRESULT hr = mgr->QueryInterface(iid, ppInterface);
        mgr->Release();
        return hr;
    }
    return E_NOINTERFACE;
}
HRESULT __stdcall MyDevice::OpenPropertyStore(DWORD stgmAccess, IPropertyStore** ppProperties) {
    if (ppProperties == NULL) return E_POINTER;
    *ppProperties = NULL;
    wchar_t guidBuf[39];
    StringFromGUID2(deviceGuid, guidBuf, 39);
    *ppProperties = new MyPropertyStore(name, flow, guidBuf);
    if (*ppProperties == NULL) return E_OUTOFMEMORY;
    return S_OK;
}
HRESULT __stdcall MyDevice::GetId(LPWSTR* ppstrId) {
    if (ppstrId == NULL) return E_POINTER;
    size_t len = id.length() + 1;
    *ppstrId = (LPWSTR)CoTaskMemAlloc(len * sizeof(wchar_t));
    if (!*ppstrId) return E_OUTOFMEMORY;
    wcscpy_s(*ppstrId, len, id.c_str());
    return S_OK;
}
HRESULT __stdcall MyDevice::GetState(DWORD* pdwState) {
    if (pdwState == NULL) return E_POINTER;
    *pdwState = DEVICE_STATE_ACTIVE;
    return S_OK;
}
HRESULT __stdcall MyDevice::GetDataFlow(EDataFlow* pDataFlow) {
    if (pDataFlow == NULL) return E_POINTER;
    *pDataFlow = flow;
    return S_OK;
}
MyAudioClient::MyAudioClient(EDataFlow f, const GUID& g, IUnknown* pUnkOuter) : m_pUnkOuter(pUnkOuter ? pUnkOuter : static_cast<IUnknown*>(this)), m_pUnkMarshal(nullptr), ref(1), flow(f), deviceGuid(g), started(false), isEventDriven(false), hEvent(NULL), hThread(NULL), notifyEvent(NULL), locked(false), lastPos(0ULL), bufferFrames(0), blockAlign(0), rate(0), bufferBytes(0), currentPaddingFrames(0), prevPos(0), positionsInitialized(false), shareMode(AUDCLNT_SHAREMODE_SHARED), periodFrames(0), isLoopback(false), lowLatencyShared(false), devicePositionFrames(0ULL), session(nullptr), channelVolumes(), requestedPeriodFrames(0), sampleRateRatio(1.0) {
    if (m_pUnkOuter == static_cast<IUnknown*>(this)) {
        CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    }
    InterlockedIncrement(&g_objects);
    InitializeCriticalSection(&cs);
    ZeroMemory(&format, sizeof(format));
    ZeroMemory(&sessionGuid, sizeof(GUID));
}
MyAudioClient::~MyAudioClient() {
    if (started) Stop();
    if (hThread) {
        started = false;
        SetEvent(hEvent);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = NULL;
    }
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    if (notifyEvent) {
        CloseHandle(notifyEvent);
        notifyEvent = NULL;
    }
    if (dsNotify) {
        dsNotify->Release();
        dsNotify = nullptr;
    }
    if (dsBuffer) {
        dsBuffer->Release();
        dsBuffer = nullptr;
    }
    if (dscBuffer) {
        dscBuffer->Release();
        dscBuffer = nullptr;
    }
    if (ds) {
        ds->Release();
        ds = nullptr;
    }
    if (dsc) {
        dsc->Release();
        dsc = nullptr;
    }
    if (session) {
        auto it = std::find(session->clients.begin(), session->clients.end(), this);
        if (it != session->clients.end()) session->clients.erase(it);
        session->Release();
    }
    DeleteCriticalSection(&cs);
    if (isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        g_enumerator->loopbackCount--;
        if (g_enumerator->loopbackCount == 0) {
            g_enumerator->loopbackRunning = false;
            LeaveCriticalSection(&g_enumerator->loopCS);
            if (g_enumerator->pumpThread) {
                SetEvent(g_enumerator->loopMicEvent);
                WaitForSingleObject(g_enumerator->pumpThread, INFINITE);
                CloseHandle(g_enumerator->pumpThread);
                g_enumerator->pumpThread = NULL;
            }
            if (g_enumerator->loopBuf) {
                delete[] g_enumerator->loopBuf;
                g_enumerator->loopBuf = nullptr;
                g_enumerator->loopBytes = 0;
            }
        }
        else {
            LeaveCriticalSection(&g_enumerator->loopCS);
        }
    }
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioClient::QueryInterface(REFIID iid, void** ppv) {
    if (m_pUnkOuter != static_cast<IUnknown*>(this)) {
        return m_pUnkOuter->QueryInterface(iid, ppv);
    }
    else {
        return NonDelegatingQueryInterface(iid, ppv);
    }
}
ULONG __stdcall MyAudioClient::AddRef() {
    if (m_pUnkOuter != static_cast<IUnknown*>(this)) {
        return m_pUnkOuter->AddRef();
    }
    else {
        return NonDelegatingAddRef();
    }
}
ULONG __stdcall MyAudioClient::Release() {
    if (m_pUnkOuter != static_cast<IUnknown*>(this)) {
        return m_pUnkOuter->Release();
    }
    else {
        return NonDelegatingRelease();
    }
}
HRESULT MyAudioClient::NonDelegatingQueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioClient || iid == IID_IAudioClient2 || iid == IID_IAudioClient3) {
        *ppv = static_cast<IAudioClient3*>(this);
        NonDelegatingAddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG MyAudioClient::NonDelegatingAddRef() {
    return InterlockedIncrement(&ref);
}
ULONG MyAudioClient::NonDelegatingRelease() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT MyAudioClient::FillSilence() {
    if (flow != eRender || !dsBuffer) return S_OK;
    void* pData = NULL;
    DWORD dataLen = 0;
    HRESULT hr = dsBuffer->Lock(0, 0, &pData, &dataLen, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    if (FAILED(hr)) return hr;
    BYTE silence = (format.Format.wBitsPerSample == 8) ? 128 : 0;
    memset(pData, (format.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) ? 0 : silence, dataLen);
    dsBuffer->Unlock(pData, dataLen, NULL, 0);
    return S_OK;
}
HRESULT MyAudioClient::InternalInitialize(AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags, REFERENCE_TIME hnsBufferDuration, REFERENCE_TIME hnsPeriodicity, const WAVEFORMATEX* pFormat, const GUID* AudioSessionGuid) {
    if (pFormat == NULL) return E_POINTER;
    if (ShareMode != AUDCLNT_SHAREMODE_SHARED && ShareMode != AUDCLNT_SHAREMODE_EXCLUSIVE) return E_INVALIDARG;
    DWORD ignored_flags = AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
    StreamFlags &= ~ignored_flags;
    DWORD valid_flags = AUDCLNT_STREAMFLAGS_CROSSPROCESS | AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED | AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE | AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED;
    if (StreamFlags & ~valid_flags) return E_INVALIDARG;
    if (pFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        format = *(WAVEFORMATEXTENSIBLE*)pFormat;
    }
    else {
        format.Format = *pFormat;
        format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
        format.Samples.wValidBitsPerSample = pFormat->wBitsPerSample;
    }
    rate = pFormat->nSamplesPerSec;
    blockAlign = pFormat->nBlockAlign;
    channelVolumes.resize(format.Format.nChannels, 1.0f);
    if (StreamFlags & AUDCLNT_STREAMFLAGS_LOOPBACK) {
        if (flow != eRender) return AUDCLNT_E_WRONG_ENDPOINT_TYPE;
        if (ShareMode != AUDCLNT_SHAREMODE_SHARED) return AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED;
        isLoopback = true;
        flow = eCapture;
        GUID nullGuid = { 0 };
        deviceGuid = nullGuid;
    }
    if (ShareMode == AUDCLNT_SHAREMODE_EXCLUSIVE) {
        if (hnsPeriodicity == 0) hnsPeriodicity = hnsBufferDuration / 2;
        if (hnsBufferDuration % hnsPeriodicity != 0) return AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED;
        bufferDuration = hnsBufferDuration;
        periodFrames = static_cast<UINT32>(((hnsPeriodicity * static_cast<REFERENCE_TIME>(rate)) + 5000000LL) / 10000000LL);
    }
    else {
        if (hnsBufferDuration == 0) hnsBufferDuration = 1000000;
        lowLatencyShared = (hnsPeriodicity != 0);
        if (lowLatencyShared) {
            if (hnsBufferDuration < hnsPeriodicity) return AUDCLNT_E_BUFFER_SIZE_ERROR;
            periodFrames = requestedPeriodFrames;
        }
        else {
            periodFrames = 0;
        }
        bufferDuration = hnsBufferDuration;
    }
    if (bufferDuration == 0) return AUDCLNT_E_BUFFER_SIZE_ERROR;
    bufferFrames = static_cast<UINT32>(((bufferDuration * static_cast<REFERENCE_TIME>(rate)) + 5000000LL) / 10000000LL);
    if (bufferFrames == 0) return AUDCLNT_E_BUFFER_SIZE_ERROR;
    if (flow == eRender && !isLoopback) {
        bufferFrames *= 4;
    }
    bufferBytes = bufferFrames * blockAlign;
    WAVEFORMATEX dsFormat = *pFormat;
    bool isExtensible = (pFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE);
    if (isExtensible) {
        WAVEFORMATEXTENSIBLE* ex = (WAVEFORMATEXTENSIBLE*)pFormat;
        if (ex->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) dsFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        else if (ex->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) dsFormat.wFormatTag = WAVE_FORMAT_PCM;
        dsFormat.cbSize = 0;
    }
    HRESULT hr;
    if (flow == eRender && !isLoopback) {
        hr = CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound8, (void**)&ds);
        if (FAILED(hr)) return hr;
        hr = ds->Initialize(IsEqualGUID(deviceGuid, GUID_NULL) ? NULL : &deviceGuid);
        if (FAILED(hr)) return hr;
        DWORD coopLevel = (ShareMode == AUDCLNT_SHAREMODE_SHARED) ? DSSCL_NORMAL : DSSCL_EXCLUSIVE;
        ds->SetCooperativeLevel(GetDesktopWindow(), coopLevel);
        IDirectSoundBuffer* temp = NULL;
        if (ShareMode == AUDCLNT_SHAREMODE_EXCLUSIVE) {
            IDirectSoundBuffer* primary = NULL;
            DSBUFFERDESC pdsbd = { sizeof(DSBUFFERDESC), DSBCAPS_PRIMARYBUFFER, 0, 0, NULL, {0} };
            hr = ds->CreateSoundBuffer(&pdsbd, &primary, NULL);
            if (SUCCEEDED(hr)) {
                hr = primary->SetFormat(&dsFormat);
                primary->Release();
            }
            if (FAILED(hr)) return hr;
        }
        DSBUFFERDESC dsbd = { sizeof(DSBUFFERDESC), DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME, bufferBytes, 0, NULL, {0} };
        dsbd.lpwfxFormat = &dsFormat;
        hr = ds->CreateSoundBuffer(&dsbd, &temp, NULL);
        if (FAILED(hr) && isExtensible) {
            dsbd.lpwfxFormat = const_cast<WAVEFORMATEX*>(pFormat);
            hr = ds->CreateSoundBuffer(&dsbd, &temp, NULL);
        }
        if (FAILED(hr)) return hr;
        hr = temp->QueryInterface(IID_IDirectSoundBuffer8, (void**)&dsBuffer);
        temp->Release();
        if (FAILED(hr)) return hr;
        hr = dsBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&dsNotify);
        if (FAILED(hr)) dsNotify = NULL;
        FillSilence();
    }
    else if (flow == eCapture && !isLoopback) {
        hr = CoCreateInstance(CLSID_DirectSoundCapture8, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSoundCapture8, (void**)&dsc);
        if (FAILED(hr)) return hr;
        hr = dsc->Initialize(IsEqualGUID(deviceGuid, GUID_NULL) ? NULL : &deviceGuid);
        if (FAILED(hr)) return hr;
        IDirectSoundCaptureBuffer* temp = NULL;
        DSCBUFFERDESC dscbd = { sizeof(DSCBUFFERDESC), 0, bufferBytes, 0, NULL, 0, NULL };
        dscbd.lpwfxFormat = &dsFormat;
        hr = dsc->CreateCaptureBuffer(&dscbd, &temp, NULL);
        if (FAILED(hr) && isExtensible) {
            dscbd.lpwfxFormat = const_cast<WAVEFORMATEX*>(pFormat);
            hr = dsc->CreateCaptureBuffer(&dscbd, &temp, NULL);
        }
        if (FAILED(hr)) return hr;
        hr = temp->QueryInterface(IID_IDirectSoundCaptureBuffer8, (void**)&dscBuffer);
        temp->Release();
        if (FAILED(hr)) return hr;
        hr = dscBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&dsNotify);
        if (FAILED(hr)) dsNotify = NULL;
    }
    else if (isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        g_enumerator->loopbackCount++;
        if (g_enumerator->loopBytes == 0) {
            g_enumerator->loopFormat = format;
            g_enumerator->loopBytes = bufferBytes * 4;
            g_enumerator->loopBuf = new BYTE[g_enumerator->loopBytes];
            if (g_enumerator->loopBuf == nullptr) {
                LeaveCriticalSection(&g_enumerator->loopCS);
                return E_OUTOFMEMORY;
            }
            g_enumerator->loopReadPos = 0ULL;
            g_enumerator->loopWritePos = 0ULL;
            g_enumerator->loopPaddingFrames = 0;
            g_enumerator->loopPositionFrames = 0ULL;
            vector<DeviceInfo> devs;
            DirectSoundCaptureEnumerateW(DSEnumCallback, &devs);
            GUID micGuid = { 0 };
            if (!devs.empty()) {
                micGuid = devs[0].guid;
            }
            g_enumerator->loopMicClient = new MyAudioClient(eCapture, micGuid);
            hr = g_enumerator->loopMicClient->InternalInitialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, hnsBufferDuration, hnsPeriodicity, pFormat, nullptr);
            if (FAILED(hr)) {
                delete g_enumerator->loopMicClient;
                g_enumerator->loopMicClient = nullptr;
                delete[] g_enumerator->loopBuf;
                g_enumerator->loopBuf = nullptr;
                g_enumerator->loopBytes = 0;
                LeaveCriticalSection(&g_enumerator->loopCS);
                return hr;
            }
            g_enumerator->loopMicEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (g_enumerator->loopMicEvent == NULL) {
                g_enumerator->loopMicClient->Release();
                g_enumerator->loopMicClient = nullptr;
                delete[] g_enumerator->loopBuf;
                g_enumerator->loopBuf = nullptr;
                g_enumerator->loopBytes = 0;
                LeaveCriticalSection(&g_enumerator->loopCS);
                return E_OUTOFMEMORY;
            }
            hr = g_enumerator->loopMicClient->SetEventHandle(g_enumerator->loopMicEvent);
            if (FAILED(hr)) {
                CloseHandle(g_enumerator->loopMicEvent);
                g_enumerator->loopMicEvent = nullptr;
                g_enumerator->loopMicClient->Release();
                g_enumerator->loopMicClient = nullptr;
                delete[] g_enumerator->loopBuf;
                g_enumerator->loopBuf = nullptr;
                g_enumerator->loopBytes = 0;
                LeaveCriticalSection(&g_enumerator->loopCS);
                return hr;
            }
            hr = g_enumerator->loopMicClient->GetService(IID_IAudioCaptureClient, (void**)&g_enumerator->loopMicCapture);
            if (FAILED(hr)) {
                CloseHandle(g_enumerator->loopMicEvent);
                g_enumerator->loopMicEvent = nullptr;
                g_enumerator->loopMicClient->Release();
                g_enumerator->loopMicClient = nullptr;
                delete[] g_enumerator->loopBuf;
                g_enumerator->loopBuf = nullptr;
                g_enumerator->loopBytes = 0;
                LeaveCriticalSection(&g_enumerator->loopCS);
                return hr;
            }
            g_enumerator->loopbackRunning = true;
            g_enumerator->pumpThread = CreateThread(NULL, 0, MyDeviceEnumerator::PumpFromMicToLoop, g_enumerator, 0, NULL);
            if (g_enumerator->pumpThread == NULL) {
                g_enumerator->loopbackRunning = false;
                g_enumerator->loopMicCapture->Release();
                g_enumerator->loopMicCapture = nullptr;
                CloseHandle(g_enumerator->loopMicEvent);
                g_enumerator->loopMicEvent = nullptr;
                g_enumerator->loopMicClient->Release();
                g_enumerator->loopMicClient = nullptr;
                delete[] g_enumerator->loopBuf;
                g_enumerator->loopBuf = nullptr;
                g_enumerator->loopBytes = 0;
                LeaveCriticalSection(&g_enumerator->loopCS);
                return E_OUTOFMEMORY;
            }
            g_enumerator->loopMicClient->Start();
        }
        LeaveCriticalSection(&g_enumerator->loopCS);
    }
    currentPaddingFrames = 0;
    prevPos = 0;
    positionsInitialized = false;
    this->shareMode = ShareMode;
    if (AudioSessionGuid) sessionGuid = *AudioSessionGuid;
    else sessionGuid = GUID_NULL;
    session = g_enumerator->GetSession(sessionGuid, true);
    session->AddRef();
    session->clients.push_back(this);
    UpdateVolume();
    return S_OK;
}
HRESULT __stdcall MyAudioClient::Initialize(AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags, REFERENCE_TIME hnsBufferDuration, REFERENCE_TIME hnsPeriodicity, const WAVEFORMATEX* pFormat, const GUID* AudioSessionGuid) {
    if (rate != 0) return AUDCLNT_E_ALREADY_INITIALIZED;
    lowLatencyShared = false;
    return InternalInitialize(ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid);
}
HRESULT __stdcall MyAudioClient::GetBufferSize(UINT32* pNumBufferFrames) {
    if (pNumBufferFrames == NULL) return E_POINTER;
    if (bufferFrames == 0) return AUDCLNT_E_NOT_INITIALIZED;
    *pNumBufferFrames = bufferFrames;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetStreamLatency(REFERENCE_TIME* phnsLatency) {
    if (phnsLatency == NULL) return E_POINTER;
    if (rate == 0) return AUDCLNT_E_NOT_INITIALIZED;
    *phnsLatency = (shareMode == AUDCLNT_SHAREMODE_SHARED) ? bufferDuration / 2 : bufferDuration / 4;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetCurrentPadding(UINT32* pNumPaddingFrames) {
    if (pNumPaddingFrames == NULL) return E_POINTER;
    if (rate == 0) {
        *pNumPaddingFrames = 0;
        return S_OK;
    }
    if (isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        double ratio = (double)rate / g_enumerator->loopFormat.Format.nSamplesPerSec;
        *pNumPaddingFrames = static_cast<UINT32>(g_enumerator->loopPaddingFrames * ratio + 0.5);
        LeaveCriticalSection(&g_enumerator->loopCS);
        return S_OK;
    }
    EnterCriticalSection(&cs);
    UINT32 padding;
    HRESULT hr = UpdatePositions(&padding);
    LeaveCriticalSection(&cs);
    if (FAILED(hr)) return hr;
    *pNumPaddingFrames = padding;
    return S_OK;
}
HRESULT MyAudioClient::UpdatePositions(UINT32* padding) {
    EnterCriticalSection(&cs);
    if (rate == 0) {
        *padding = 0;
        LeaveCriticalSection(&cs);
        return S_OK;
    }
    if (isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        if (g_enumerator->loopBuf == nullptr) {
            LeaveCriticalSection(&g_enumerator->loopCS);
            LeaveCriticalSection(&cs);
            return AUDCLNT_E_DEVICE_INVALIDATED;
        }
        double ratio = (double)rate / g_enumerator->loopFormat.Format.nSamplesPerSec;
        *padding = static_cast<UINT32>(g_enumerator->loopPaddingFrames * ratio + 0.5);
        devicePositionFrames = static_cast<UINT64>(g_enumerator->loopPositionFrames * ratio + 0.5);
        LeaveCriticalSection(&g_enumerator->loopCS);
        LeaveCriticalSection(&cs);
        return S_OK;
    }
    if ((flow == eRender && dsBuffer == nullptr) || (flow == eCapture && dscBuffer == nullptr)) {
        *padding = 0;
        LeaveCriticalSection(&cs);
        return S_OK;
    }
    DWORD curPos = 0, curSafe = 0;
    HRESULT hr;
    if (flow == eRender) {
        hr = dsBuffer->GetCurrentPosition(&curPos, &curSafe);
    }
    else {
        hr = dscBuffer->GetCurrentPosition(&curPos, &curSafe);
        curPos = curSafe;
    }
    if (FAILED(hr)) {
        LeaveCriticalSection(&cs);
        return hr;
    }
    if (!positionsInitialized) {
        prevPos = curPos;
        positionsInitialized = true;
        *padding = currentPaddingFrames;
        LeaveCriticalSection(&cs);
        return S_OK;
    }
    DWORD deltaBytes = (curPos >= prevPos) ? (curPos - prevPos) : (bufferBytes - prevPos + curPos);
    UINT32 deltaFrames = deltaBytes / blockAlign;
    devicePositionFrames += deltaFrames;
    if (flow == eRender) {
        currentPaddingFrames = (deltaFrames > currentPaddingFrames) ? 0 : currentPaddingFrames - deltaFrames;
    }
    else {
        currentPaddingFrames = std::min(currentPaddingFrames + deltaFrames, bufferFrames);
    }
    prevPos = curPos;
    *padding = currentPaddingFrames;
    LeaveCriticalSection(&cs);
    return S_OK;
}
HRESULT __stdcall MyAudioClient::IsFormatSupported(AUDCLNT_SHAREMODE ShareMode, const WAVEFORMATEX* pFormat, WAVEFORMATEX** ppClosestMatch) {
    if (pFormat == NULL) return E_POINTER;
    if (ppClosestMatch) *ppClosestMatch = NULL;
    if (ShareMode != AUDCLNT_SHAREMODE_SHARED && ShareMode != AUDCLNT_SHAREMODE_EXCLUSIVE) return E_INVALIDARG;
    if (pFormat->nSamplesPerSec > 200000) return AUDCLNT_E_UNSUPPORTED_FORMAT;
    if (pFormat->wFormatTag != WAVE_FORMAT_PCM && pFormat->wFormatTag != WAVE_FORMAT_IEEE_FLOAT && pFormat->wFormatTag != WAVE_FORMAT_EXTENSIBLE) return AUDCLNT_E_UNSUPPORTED_FORMAT;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetMixFormat(WAVEFORMATEX** ppDeviceFormat) {
    if (ppDeviceFormat == NULL) return E_POINTER;
    WAVEFORMATEXTENSIBLE* wfex = static_cast<WAVEFORMATEXTENSIBLE*>(CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE)));
    if (!wfex) return E_OUTOFMEMORY;
    wfex->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfex->Format.cbSize = 22;
    wfex->Format.nChannels = 2;
    wfex->Format.nSamplesPerSec = 48000;
    wfex->Format.wBitsPerSample = 32;
    wfex->Format.nBlockAlign = 8;
    wfex->Format.nAvgBytesPerSec = 384000;
    wfex->Samples.wValidBitsPerSample = 32;
    wfex->dwChannelMask = 3;
    wfex->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    *ppDeviceFormat = reinterpret_cast<WAVEFORMATEX*>(wfex);
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetDevicePeriod(REFERENCE_TIME* phnsDefaultDevicePeriod, REFERENCE_TIME* phnsMinimumDevicePeriod) {
    if (phnsDefaultDevicePeriod == NULL && phnsMinimumDevicePeriod == NULL) return E_POINTER;
    if (phnsDefaultDevicePeriod != NULL) *phnsDefaultDevicePeriod = 100000;
    if (phnsMinimumDevicePeriod != NULL) *phnsMinimumDevicePeriod = 30000;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::Start() {
    if (rate == 0) return AUDCLNT_E_NOT_INITIALIZED;
    if (started) return AUDCLNT_E_NOT_STOPPED;
    if (flow == eRender && !isLoopback) {
        if (!dsBuffer) return AUDCLNT_E_NOT_INITIALIZED;
        dsBuffer->SetCurrentPosition(0);
        HRESULT hr = dsBuffer->Play(0, 0, DSBPLAY_LOOPING);
        if (FAILED(hr)) return hr;
    }
    else if (flow == eCapture && !isLoopback) {
        if (!dscBuffer) return AUDCLNT_E_NOT_INITIALIZED;
        HRESULT hr = dscBuffer->Start(DSCBSTART_LOOPING);
        if (FAILED(hr)) return hr;
    }
    else if (isLoopback) {
        started = true;
        positionsInitialized = false;
        if (isEventDriven) {
            hThread = CreateThread(NULL, 0, MonitorThread, this, 0, NULL);
        }
        return S_OK;
    }
    started = true;
    positionsInitialized = false;
    if (dsNotify && (shareMode == AUDCLNT_SHAREMODE_EXCLUSIVE || lowLatencyShared) && isEventDriven) {
        notifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (notifyEvent) {
            DWORD periodBytes = periodFrames * blockAlign;
            UINT32 numPositions = static_cast<UINT32>(std::ceil(static_cast<double>(bufferBytes) / periodBytes));
            numPositions = std::max(numPositions, static_cast<UINT32>(2));
            numPositions = std::min(numPositions, static_cast<UINT32>(64));
            DSBPOSITIONNOTIFY pos[64];
            for (UINT32 i = 0; i < numPositions; i++) {
                pos[i].dwOffset = (i * periodBytes) % bufferBytes;
                pos[i].hEventNotify = notifyEvent;
            }
            HRESULT hr = dsNotify->SetNotificationPositions(numPositions, pos);
            if (FAILED(hr)) {
                DSBPOSITIONNOTIFY pos2[2];
                pos2[0].dwOffset = 0;
                pos2[0].hEventNotify = notifyEvent;
                pos2[1].dwOffset = bufferBytes / 2;
                pos2[1].hEventNotify = notifyEvent;
                hr = dsNotify->SetNotificationPositions(2, pos2);
                if (FAILED(hr)) {
                    CloseHandle(notifyEvent);
                    notifyEvent = NULL;
                }
            }
        }
    }
    if (isEventDriven) {
        hThread = CreateThread(NULL, 0, MonitorThread, this, 0, NULL);
    }
    return S_OK;
}
HRESULT __stdcall MyAudioClient::Stop() {
    if (rate == 0) return AUDCLNT_E_NOT_INITIALIZED;
    if (!started) return AUDCLNT_E_NOT_STOPPED;
    started = false;
    if (hEvent) SetEvent(hEvent);
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = NULL;
    }
    if (flow == eRender && !isLoopback) {
        if (dsBuffer) dsBuffer->Stop();
    }
    else if (flow == eCapture && !isLoopback) {
        if (dscBuffer) dscBuffer->Stop();
    }
    return S_OK;
}
HRESULT __stdcall MyAudioClient::Reset() {
    if (rate == 0) return AUDCLNT_E_NOT_INITIALIZED;
    if (started) return AUDCLNT_E_NOT_STOPPED;
    Stop();
    lastPos = 0ULL;
    currentPaddingFrames = 0;
    prevPos = 0;
    positionsInitialized = false;
    if (isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        g_enumerator->loopReadPos = g_enumerator->loopWritePos;
        g_enumerator->loopPaddingFrames = 0;
        LeaveCriticalSection(&g_enumerator->loopCS);
    }
    else if (flow == eRender && dsBuffer) {
        dsBuffer->SetCurrentPosition(0);
        FillSilence();
    }
    return S_OK;
}
HRESULT __stdcall MyAudioClient::SetEventHandle(HANDLE eventHandle) {
    if (rate == 0) return AUDCLNT_E_NOT_INITIALIZED;
    if (eventHandle == NULL) return E_INVALIDARG;
    hEvent = eventHandle;
    isEventDriven = true;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetService(REFIID riid, void** ppv) {
    if (rate == 0) return AUDCLNT_E_NOT_INITIALIZED;
    if (ppv == NULL) return E_POINTER;
    *ppv = NULL;
    if (riid == IID_IAudioRenderClient && flow == eRender && !isLoopback) {
        try {
            *ppv = new MyRenderClient(this);
        }
        catch (std::bad_alloc&) {
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }
    else if (riid == IID_IAudioCaptureClient && flow == eCapture) {
        try {
            *ppv = new MyCaptureClient(this);
        }
        catch (std::bad_alloc&) {
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }
    else if (riid == IID_IAudioClock) {
        *ppv = new MyAudioClock(this);
        return S_OK;
    }
    else if (riid == IID_IAudioClock2) {
        *ppv = new MyAudioClock2(this);
        return S_OK;
    }
    else if (riid == IID_IAudioClockAdjustment) {
        if (lowLatencyShared) {
            *ppv = new MyAudioClockAdjustment(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    else if (riid == IID_IAudioSessionControl || riid == IID_IAudioSessionControl2) {
        if (session == nullptr) {
            session = g_enumerator->GetSession(sessionGuid, true);
            session->AddRef();
            session->clients.push_back(this);
        }
        MyAudioSessionControl* ctrl = new MyAudioSessionControl(session);
        HRESULT hr = ctrl->QueryInterface(riid, ppv);
        ctrl->Release();
        return hr;
    }
    else if (riid == IID_ISimpleAudioVolume) {
        if (session == nullptr) {
            session = g_enumerator->GetSession(sessionGuid, true);
            session->AddRef();
            session->clients.push_back(this);
        }
        *ppv = new MySimpleAudioVolume(session);
        return S_OK;
    }
    else if (riid == IID_IAudioStreamVolume) {
        *ppv = new MyAudioStreamVolume(this);
        return S_OK;
    }
    return E_NOINTERFACE;
}
DWORD WINAPI MyAudioClient::MonitorThread(LPVOID lpParam) {
    CoInitialize(NULL);
    MyAudioClient* self = reinterpret_cast<MyAudioClient*>(lpParam);
    while (self->started) {
        DWORD waitResult = WAIT_TIMEOUT;
        if (self->isLoopback) {
            if (g_enumerator->loopEvent) {
                waitResult = WaitForSingleObject(g_enumerator->loopEvent, 50);
            }
            else {
                Sleep(5);
                waitResult = WAIT_OBJECT_0;
            }
        }
        else if (self->notifyEvent) {
            waitResult = WaitForSingleObject(self->notifyEvent, 50);
        }
        else {
            Sleep(5);
            waitResult = WAIT_OBJECT_0;
        }
        if (!self->started) break;
        if (waitResult == WAIT_OBJECT_0) {
            EnterCriticalSection(&self->cs);
            UINT32 pad;
            self->UpdatePositions(&pad);
            LeaveCriticalSection(&self->cs);
            if (self->hEvent) {
                bool shouldSignal = (self->flow == eRender && !self->isLoopback) ? (pad < self->bufferFrames) : (pad > 0);
                if (shouldSignal) {
                    SetEvent(self->hEvent);
                }
            }
        }
    }
    CoUninitialize();
    return 0;
}
HRESULT __stdcall MyAudioClient::IsOffloadCapable(AUDIO_STREAM_CATEGORY Category, BOOL* pbOffloadCapable) {
    if (pbOffloadCapable == NULL) return E_POINTER;
    *pbOffloadCapable = FALSE;
    if (flow != eRender) return S_OK;
    if (Category != 1 && Category != 2 && Category != 11 && Category != 10 && Category != 7) {
        return S_OK;
    }
    IDirectSound8* tempDs = NULL;
    HRESULT hr = DirectSoundCreate8(IsEqualGUID(deviceGuid, GUID_NULL) ? NULL : &deviceGuid, &tempDs, NULL);
    if (FAILED(hr)) return S_OK;
    DSCAPS caps;
    caps.dwSize = sizeof(DSCAPS);
    hr = tempDs->GetCaps(&caps);
    tempDs->Release();
    if (FAILED(hr)) return S_OK;
    *pbOffloadCapable = (caps.dwFlags & DSCAPS_EMULDRIVER) == 0;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::SetClientProperties(const AudioClientProperties* pProperties) {
    if (pProperties == NULL) return E_POINTER;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetBufferSizeLimits(const WAVEFORMATEX* pFormat, BOOL bEventDriven, REFERENCE_TIME* phnsMinBufferDuration, REFERENCE_TIME* phnsMaxBufferDuration) {
    if (pFormat == NULL || phnsMinBufferDuration == NULL || phnsMaxBufferDuration == NULL) return E_POINTER;
    *phnsMinBufferDuration = 30000;
    *phnsMaxBufferDuration = 10000000;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetSharedModeEnginePeriod(const WAVEFORMATEX* pFormat, UINT32* pDefaultPeriodInFrames, UINT32* pFundamentalPeriodInFrames, UINT32* pMinPeriodInFrames, UINT32* pMaxPeriodInFrames) {
    if (pFormat == NULL || pDefaultPeriodInFrames == NULL || pFundamentalPeriodInFrames == NULL || pMinPeriodInFrames == NULL || pMaxPeriodInFrames == NULL) return E_POINTER;
    UINT32 r = pFormat->nSamplesPerSec ? pFormat->nSamplesPerSec : (rate ? rate : 48000);
    *pDefaultPeriodInFrames = r / 100;
    *pFundamentalPeriodInFrames = r / 1000;
    *pMinPeriodInFrames = r / 1000;
    *pMaxPeriodInFrames = r / 10;
    return S_OK;
}
HRESULT __stdcall MyAudioClient::GetCurrentSharedModeEnginePeriod(WAVEFORMATEX** ppFormat, UINT32* pCurrentPeriodInFrames) {
    if (ppFormat == NULL || pCurrentPeriodInFrames == NULL) return E_POINTER;
    WAVEFORMATEXTENSIBLE* wfex = static_cast<WAVEFORMATEXTENSIBLE*>(CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE)));
    if (!wfex) return E_OUTOFMEMORY;
    if (format.Format.nSamplesPerSec == 0) {
        wfex->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
        wfex->Format.cbSize = 22;
        wfex->Format.nChannels = 2;
        wfex->Format.nSamplesPerSec = 48000;
        wfex->Format.wBitsPerSample = 32;
        wfex->Format.nBlockAlign = 8;
        wfex->Format.nAvgBytesPerSec = 384000;
        wfex->Samples.wValidBitsPerSample = 32;
        wfex->dwChannelMask = 3;
        wfex->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        *pCurrentPeriodInFrames = 480;
    }
    else {
        *wfex = format;
        *pCurrentPeriodInFrames = requestedPeriodFrames ? requestedPeriodFrames : periodFrames;
    }
    *ppFormat = reinterpret_cast<WAVEFORMATEX*>(wfex);
    return S_OK;
}
HRESULT __stdcall MyAudioClient::InitializeSharedAudioStream(DWORD StreamFlags, UINT32 PeriodInFrames, const WAVEFORMATEX* pFormat, const GUID* AudioSessionGuid) {
    lowLatencyShared = true;
    REFERENCE_TIME hnsPeriodicity = static_cast<REFERENCE_TIME>(PeriodInFrames) * 10000000LL / pFormat->nSamplesPerSec;
    REFERENCE_TIME hnsBufferDuration = hnsPeriodicity * 4;
    requestedPeriodFrames = PeriodInFrames;
    return InternalInitialize(AUDCLNT_SHAREMODE_SHARED, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid);
}
void MyAudioClient::UpdateVolume() {
    if (flow != eRender || !dsBuffer || !g_enumerator || !session || isLoopback) return;
    float effective = g_enumerator->masterVolume * session->volume;
    bool muted = g_enumerator->masterMute || session->mute;
    long dsVol = muted || effective <= 0.0001f ? DSBVOLUME_MIN : static_cast<long>(2000.0f * log10f(effective));
    dsBuffer->SetVolume(dsVol);
}
HRESULT MyAudioClient::GetPosition(UINT64* pu64Position, UINT64* pu64QPCPosition) {
    if (pu64Position == NULL) return E_POINTER;
    EnterCriticalSection(&cs);
    if (rate == 0) {
        *pu64Position = 0;
        if (pu64QPCPosition) *pu64QPCPosition = 0;
        LeaveCriticalSection(&cs);
        return S_OK;
    }
    UINT64 rawPosition;
    if (isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        double ratio = (double)rate / g_enumerator->loopFormat.Format.nSamplesPerSec;
        rawPosition = static_cast<UINT64>(g_enumerator->loopPositionFrames * ratio + 0.5);
        LeaveCriticalSection(&g_enumerator->loopCS);
    }
    else {
        UINT32 pad;
        HRESULT hr = UpdatePositions(&pad);
        if (FAILED(hr)) {
            LeaveCriticalSection(&cs);
            return hr;
        }
        rawPosition = devicePositionFrames;
    }
    *pu64Position = static_cast<UINT64>(static_cast<double>(rawPosition) * sampleRateRatio);
    LeaveCriticalSection(&cs);
    if (pu64QPCPosition) {
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(pu64QPCPosition));
    }
    return S_OK;
}
MyRenderClient::MyRenderClient(MyAudioClient* p) : ref(1), m_pUnkMarshal(nullptr), parent(p), tempBuffer(new BYTE[p->bufferBytes]), usingTemp(false) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    parent->NonDelegatingAddRef();
}
MyRenderClient::~MyRenderClient() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    delete[] tempBuffer;
    parent->NonDelegatingRelease();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyRenderClient::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioRenderClient) {
        *ppv = static_cast<IAudioRenderClient*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyRenderClient::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyRenderClient::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyRenderClient::GetBuffer(UINT32 NumFramesRequested, BYTE** ppData) {
    if (ppData == NULL) return E_POINTER;
    *ppData = NULL;
    if (parent->locked) return AUDCLNT_E_OUT_OF_ORDER;
    if (NumFramesRequested == 0) return S_OK;
    UINT32 padding;
    HRESULT hr = parent->GetCurrentPadding(&padding);
    if (FAILED(hr)) return hr;
    UINT32 available = parent->bufferFrames - padding;
    if (NumFramesRequested > available) return AUDCLNT_E_BUFFER_TOO_LARGE;
    UINT32 bytes = NumFramesRequested * parent->blockAlign;
    EnterCriticalSection(&parent->cs);
    hr = parent->dsBuffer->Lock(static_cast<DWORD>(parent->lastPos % parent->bufferBytes), bytes, &parent->lockP1, &parent->lockL1, &parent->lockP2, &parent->lockL2, 0);
    if (FAILED(hr)) {
        LeaveCriticalSection(&parent->cs);
        return AUDCLNT_E_DEVICE_INVALIDATED;
    }
    usingTemp = (parent->lockP2 != NULL);
    *ppData = usingTemp ? tempBuffer : static_cast<BYTE*>(parent->lockP1);
    parent->locked = true;
    LeaveCriticalSection(&parent->cs);
    return S_OK;
}
HRESULT __stdcall MyRenderClient::ReleaseBuffer(UINT32 NumFramesWritten, DWORD dwFlags) {
    if (NumFramesWritten == 0) {
        if (!parent->locked) return S_OK;
        EnterCriticalSection(&parent->cs);
        if (parent->dsBuffer) {
            parent->dsBuffer->Unlock(parent->lockP1, parent->lockL1, parent->lockP2, parent->lockL2);
        }
        parent->lockP1 = nullptr;
        parent->lockP2 = nullptr;
        parent->lockL1 = parent->lockL2 = 0;
        parent->locked = false;
        LeaveCriticalSection(&parent->cs);
        return S_OK;
    }
    if (!parent->locked) return AUDCLNT_E_OUT_OF_ORDER;
    UINT32 bytes = NumFramesWritten * parent->blockAlign;
    EnterCriticalSection(&parent->cs);
    if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT) {
        BYTE silence = (parent->format.Format.wBitsPerSample == 8) ? 128 : 0;
        void* dest = usingTemp ? tempBuffer : parent->lockP1;
        memset(dest, (parent->format.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) ? 0 : silence, bytes);
    }
    else {
        BYTE* dataToApply = usingTemp ? tempBuffer : static_cast<BYTE*>(parent->lockP1);
        ApplyChannelVolumes(dataToApply, parent->format, NumFramesWritten, parent->channelVolumes);
    }
    if (usingTemp) {
        DWORD bytes1 = std::min<DWORD>(bytes, parent->lockL1);
        memcpy(parent->lockP1, tempBuffer, bytes1);
        if (bytes > bytes1 && parent->lockP2) memcpy(parent->lockP2, tempBuffer + bytes1, bytes - bytes1);
    }
    parent->dsBuffer->Unlock(parent->lockP1, std::min<DWORD>(bytes, parent->lockL1), parent->lockP2, (bytes > parent->lockL1 && parent->lockP2) ? (bytes - parent->lockL1) : 0);
    parent->lastPos += bytes;
    parent->currentPaddingFrames = std::min(parent->currentPaddingFrames + NumFramesWritten, parent->bufferFrames);
    if (NumFramesWritten > 0 && parent->session == nullptr && parent->flow == eRender && !parent->isLoopback) {
        parent->session = g_enumerator->GetSession(parent->sessionGuid, true);
        parent->session->AddRef();
        parent->session->clients.push_back(parent);
        parent->UpdateVolume();
    }
    parent->locked = false;
    LeaveCriticalSection(&parent->cs);
    return S_OK;
}
MyCaptureClient::MyCaptureClient(MyAudioClient* p) : ref(1), m_pUnkMarshal(nullptr), parent(p), tempBuffer(new BYTE[p->bufferBytes * 4]), usingTemp(false) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    parent->NonDelegatingAddRef();
}
MyCaptureClient::~MyCaptureClient() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    delete[] tempBuffer;
    parent->NonDelegatingRelease();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyCaptureClient::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioCaptureClient) {
        *ppv = static_cast<IAudioCaptureClient*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyCaptureClient::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyCaptureClient::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyCaptureClient::GetBuffer(BYTE** ppData, UINT32* pNumFramesToRead, DWORD* pdwFlags, UINT64* pu64DevicePosition, UINT64* pu64QPCPosition) {
    if (ppData == NULL || pNumFramesToRead == NULL) return E_POINTER;
    if (pdwFlags) *pdwFlags = 0;
    if (pu64DevicePosition) *pu64DevicePosition = 0;
    if (pu64QPCPosition) *pu64QPCPosition = 0;
    if (parent->isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        UINT32 loopPadFrames = g_enumerator->loopPaddingFrames;
        if (loopPadFrames == 0) {
            *ppData = NULL;
            *pNumFramesToRead = 0;
            LeaveCriticalSection(&g_enumerator->loopCS);
            return S_OK;
        }
        double ratio = (double)parent->rate / g_enumerator->loopFormat.Format.nSamplesPerSec;
        UINT32 frames = static_cast<UINT32>(loopPadFrames * ratio + 0.5);
        UINT32 maxBytes = parent->bufferBytes * 4;
        if (frames * parent->blockAlign > maxBytes) {
            frames = maxBytes / parent->blockAlign;
        }
        UINT32 loopBlockAlign = g_enumerator->loopFormat.Format.nBlockAlign;
        UINT32 loopBytesToRead = loopPadFrames * loopBlockAlign;
        BYTE* rawData = new BYTE[loopBytesToRead];
        if (rawData == nullptr) {
            LeaveCriticalSection(&g_enumerator->loopCS);
            return E_OUTOFMEMORY;
        }
        UINT32 pos = static_cast<UINT32>(g_enumerator->loopReadPos % g_enumerator->loopBytes);
        UINT32 bytes1 = std::min(loopBytesToRead, g_enumerator->loopBytes - pos);
        memcpy(rawData, g_enumerator->loopBuf + pos, bytes1);
        if (bytes1 < loopBytesToRead) memcpy(rawData + bytes1, g_enumerator->loopBuf, loopBytesToRead - bytes1);
        UINT32 loopChannels = g_enumerator->loopFormat.Format.nChannels;
        float* fRaw = new float[loopPadFrames * loopChannels];
        if (fRaw == nullptr) {
            delete[] rawData;
            LeaveCriticalSection(&g_enumerator->loopCS);
            return E_OUTOFMEMORY;
        }
        ConvertToFloat(rawData, g_enumerator->loopFormat, fRaw, loopPadFrames);
        delete[] rawData;
        UINT32 parentChannels = parent->format.Format.nChannels;
        float* fDest = new float[frames * parentChannels];
        if (fDest == nullptr) {
            delete[] fRaw;
            LeaveCriticalSection(&g_enumerator->loopCS);
            return E_OUTOFMEMORY;
        }
        ResampleFloat(fRaw, loopChannels, loopPadFrames, fDest, parentChannels, frames, ratio);
        delete[] fRaw;
        ConvertFromFloat(fDest, parent->format, tempBuffer, frames);
        delete[] fDest;
        ApplyChannelVolumes(tempBuffer, parent->format, frames, parent->channelVolumes);
        *ppData = tempBuffer;
        *pNumFramesToRead = frames;
        if (pu64DevicePosition) *pu64DevicePosition = static_cast<UINT64>(g_enumerator->loopPositionFrames * ratio + 0.5) - frames;
        if (pu64QPCPosition) QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(pu64QPCPosition));
        parent->locked = true;
        LeaveCriticalSection(&g_enumerator->loopCS);
        return S_OK;
    }
    UINT32 padding;
    parent->GetCurrentPadding(&padding);
    if (padding == 0) {
        *ppData = NULL;
        *pNumFramesToRead = 0;
        return S_OK;
    }
    UINT32 bytes = padding * parent->blockAlign;
    EnterCriticalSection(&parent->cs);
    HRESULT hr = parent->dscBuffer->Lock(static_cast<DWORD>(parent->lastPos % parent->bufferBytes), bytes, &parent->lockP1, &parent->lockL1, &parent->lockP2, &parent->lockL2, 0);
    if (FAILED(hr)) {
        LeaveCriticalSection(&parent->cs);
        return AUDCLNT_E_DEVICE_INVALIDATED;
    }
    usingTemp = (parent->lockP2 != NULL);
    if (usingTemp) {
        memcpy(tempBuffer, parent->lockP1, parent->lockL1);
        if (parent->lockP2) memcpy(tempBuffer + parent->lockL1, parent->lockP2, parent->lockL2);
        *ppData = tempBuffer;
    }
    else {
        *ppData = static_cast<BYTE*>(parent->lockP1);
    }
    BYTE* dataToApply = usingTemp ? tempBuffer : static_cast<BYTE*>(parent->lockP1);
    ApplyChannelVolumes(dataToApply, parent->format, padding, parent->channelVolumes);
    *pNumFramesToRead = padding;
    if (pu64DevicePosition) *pu64DevicePosition = parent->devicePositionFrames - padding;
    if (pu64QPCPosition) QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(pu64QPCPosition));
    parent->locked = true;
    LeaveCriticalSection(&parent->cs);
    return S_OK;
}
HRESULT __stdcall MyCaptureClient::ReleaseBuffer(UINT32 NumFramesRead) {
    if (!parent->locked) return AUDCLNT_E_OUT_OF_ORDER;
    UINT32 bytes = NumFramesRead * parent->blockAlign;
    if (parent->isLoopback) {
        EnterCriticalSection(&g_enumerator->loopCS);
        double ratio = (double)g_enumerator->loopFormat.Format.nSamplesPerSec / parent->rate;
        UINT32 loopFramesReleased = static_cast<UINT32>(NumFramesRead * ratio + 0.5f);
        g_enumerator->loopReadPos += loopFramesReleased * g_enumerator->loopFormat.Format.nBlockAlign;
        g_enumerator->loopPaddingFrames = (loopFramesReleased > g_enumerator->loopPaddingFrames) ? 0 : g_enumerator->loopPaddingFrames - loopFramesReleased;
        LeaveCriticalSection(&g_enumerator->loopCS);
        parent->locked = false;
        return S_OK;
    }
    EnterCriticalSection(&parent->cs);
    DWORD bytes1 = std::min<DWORD>(bytes, parent->lockL1);
    DWORD bytes2 = (bytes > parent->lockL1 && parent->lockP2) ? (bytes - parent->lockL1) : 0;
    parent->dscBuffer->Unlock(parent->lockP1, bytes1, parent->lockP2, bytes2);
    parent->lastPos += bytes;
    parent->currentPaddingFrames = (NumFramesRead > parent->currentPaddingFrames) ? 0 : parent->currentPaddingFrames - NumFramesRead;
    parent->locked = false;
    LeaveCriticalSection(&parent->cs);
    return S_OK;
}
HRESULT __stdcall MyCaptureClient::GetNextPacketSize(UINT32* pNumFramesInNextPacket) {
    if (pNumFramesInNextPacket == NULL) return E_POINTER;
    return parent->GetCurrentPadding(pNumFramesInNextPacket);
}
static uint32_t fnv1a32_from_wstring(const std::wstring& s, uint32_t seed) {
    const uint32_t FNV_PRIME = 16777619u;
    uint32_t h = 2166136261u ^ seed;
    for (wchar_t c : s) {
        h ^= static_cast<uint32_t>(c & 0xFFFF);
        h *= FNV_PRIME;
        h ^= static_cast<uint32_t>((c >> 16) & 0xFFFF);
        h *= FNV_PRIME;
    }
    return h;
}
static GUID MakeDeterministicGuidFromString(const std::wstring& s) {
    GUID g;
    uint32_t a = fnv1a32_from_wstring(s, 0x12345678);
    uint32_t b = fnv1a32_from_wstring(s, 0x87654321);
    uint32_t c = fnv1a32_from_wstring(s, 0xdeadbeef);
    uint32_t d = fnv1a32_from_wstring(s, 0xfeedface);
    g.Data1 = a;
    g.Data2 = static_cast<unsigned short>(b & 0xFFFF);
    g.Data3 = static_cast<unsigned short>((b >> 16) & 0xFFFF);
    g.Data4[0] = static_cast<unsigned char>((c >> 24) & 0xFF);
    g.Data4[1] = static_cast<unsigned char>((c >> 16) & 0xFF);
    g.Data4[2] = static_cast<unsigned char>((c >> 8) & 0xFF);
    g.Data4[3] = static_cast<unsigned char>(c & 0xFF);
    g.Data4[4] = static_cast<unsigned char>((d >> 24) & 0xFF);
    g.Data4[5] = static_cast<unsigned char>((d >> 16) & 0xFF);
    g.Data4[6] = static_cast<unsigned char>((d >> 8) & 0xFF);
    g.Data4[7] = static_cast<unsigned char>(d & 0xFF);
    return g;
}
MyPropertyStore::MyPropertyStore(const wstring& n, EDataFlow f, const wstring& guidStr) : ref(1), m_pUnkMarshal(nullptr), deviceName(n), flow(f), endpointGuid(guidStr) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    mixFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    mixFormat.Format.cbSize = 22;
    mixFormat.Format.nChannels = 2;
    mixFormat.Format.nSamplesPerSec = 48000;
    mixFormat.Format.wBitsPerSample = 32;
    mixFormat.Format.nBlockAlign = 8;
    mixFormat.Format.nAvgBytesPerSec = 384000;
    mixFormat.Samples.wValidBitsPerSample = 32;
    mixFormat.dwChannelMask = 3;
    mixFormat.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    GUID parsed = GUID_NULL;
    if (guidStr.size() > 0) {
        HRESULT hr = CLSIDFromString(guidStr.c_str(), &parsed);
        if (FAILED(hr) || IsEqualGUID(parsed, GUID_NULL)) {
            parsed = GUID_NULL;
        }
    }
    if (!IsEqualGUID(parsed, GUID_NULL)) {
        containerId = parsed;
    }
    else {
        wstring seed = deviceName + L"|" + endpointGuid + L"|" + (flow == eRender ? L"render" : L"capture");
        containerId = MakeDeterministicGuidFromString(seed);
    }
}
MyPropertyStore::~MyPropertyStore() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyPropertyStore::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IPropertyStore) {
        *ppv = static_cast<IPropertyStore*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyPropertyStore::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyPropertyStore::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyPropertyStore::GetCount(DWORD* cProps) {
    if (cProps == NULL) return E_POINTER;
    *cProps = 12;
    return S_OK;
}
HRESULT __stdcall MyPropertyStore::GetAt(DWORD iProp, PROPERTYKEY* pkey) {
    if (pkey == NULL) return E_POINTER;
    switch (iProp) {
    case 0: *pkey = PKEY_Device_FriendlyName; break;
    case 1: *pkey = PKEY_Device_DeviceDesc; break;
    case 2: *pkey = PKEY_DeviceInterface_FriendlyName; break;
    case 3: *pkey = PKEY_AudioEngine_DeviceFormat; break;
    case 4: *pkey = PKEY_AudioEndpoint_FormFactor; break;
    case 5: *pkey = PKEY_AudioEndpoint_GUID; break;
    case 6: *pkey = PKEY_DeviceClass_IconPath; break;
    case 7: *pkey = PKEY_Device_InstanceId; break;
    case 8: *pkey = PKEY_AudioEndpoint_Supports_EventDriven_Mode; break;
    case 9: *pkey = PKEY_Device_Driver; break;
    case 10: *pkey = PKEY_Device_ContainerId; break;
    case 11: *pkey = PKEY_Device_EnumeratorName; break;
    default: return E_INVALIDARG;
    }
    return S_OK;
}
HRESULT __stdcall MyPropertyStore::GetValue(const PROPERTYKEY& key, PROPVARIANT* pv) {
    if (pv == NULL) return E_POINTER;
    PropVariantInit(pv);
    if (IsEqualGUID(key.fmtid, PKEY_Device_FriendlyName.fmtid) && key.pid == PKEY_Device_FriendlyName.pid) {
        wstring friendly = (flow == eRender ? L"Speakers (" : L"Microphone (") + deviceName + L")";
        return InitPropVariantFromString(friendly.c_str(), pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_Device_DeviceDesc.fmtid) && key.pid == PKEY_Device_DeviceDesc.pid) {
        wstring desc = (flow == eRender ? L"Speakers" : L"Microphone");
        return InitPropVariantFromString(desc.c_str(), pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_DeviceInterface_FriendlyName.fmtid) && key.pid == PKEY_DeviceInterface_FriendlyName.pid) {
        return InitPropVariantFromString(deviceName.c_str(), pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_AudioEngine_DeviceFormat.fmtid) && key.pid == PKEY_AudioEngine_DeviceFormat.pid) {
        pv->vt = VT_BLOB;
        pv->blob.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
        pv->blob.pBlobData = static_cast<BYTE*>(CoTaskMemAlloc(pv->blob.cbSize));
        if (!pv->blob.pBlobData) {
            PropVariantClear(pv);
            return E_OUTOFMEMORY;
        }
        memcpy(pv->blob.pBlobData, &mixFormat, pv->blob.cbSize);
        return S_OK;
    }
    else if (IsEqualGUID(key.fmtid, PKEY_AudioEndpoint_FormFactor.fmtid) && key.pid == PKEY_AudioEndpoint_FormFactor.pid) {
        return InitPropVariantFromUInt32((flow == eRender) ? 1 : 4, pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_AudioEndpoint_GUID.fmtid) && key.pid == PKEY_AudioEndpoint_GUID.pid) {
        return InitPropVariantFromString(endpointGuid.c_str(), pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_DeviceClass_IconPath.fmtid) && key.pid == PKEY_DeviceClass_IconPath.pid) {
        return InitPropVariantFromString(L"%SystemRoot%\\system32\\shell32.dll,-236", pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_Device_InstanceId.fmtid) && key.pid == PKEY_Device_InstanceId.pid) {
        wstring instanceId;
        if (!endpointGuid.empty()) {
            instanceId = L"Endpoint\\" + endpointGuid;
        }
        else {
            instanceId = L"EndpointName\\" + deviceName;
        }
        return InitPropVariantFromString(instanceId.c_str(), pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_AudioEndpoint_Supports_EventDriven_Mode.fmtid) && key.pid == PKEY_AudioEndpoint_Supports_EventDriven_Mode.pid) {
        return InitPropVariantFromBoolean(TRUE, pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_Device_Driver.fmtid) && key.pid == PKEY_Device_Driver.pid) {
        wstring driver = L"{4d36e96c-e325-11ce-bfc1-08002be10318}\\0000";
        return InitPropVariantFromString(driver.c_str(), pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_Device_ContainerId.fmtid) && key.pid == PKEY_Device_ContainerId.pid) {
        return InitPropVariantFromCLSID(containerId, pv);
    }
    else if (IsEqualGUID(key.fmtid, PKEY_Device_EnumeratorName.fmtid) && key.pid == PKEY_Device_EnumeratorName.pid) {
        wstring enumName = L"Audio";
        return InitPropVariantFromString(enumName.c_str(), pv);
    }
    PropVariantClear(pv);
    return STG_E_ACCESSDENIED;
}
HRESULT __stdcall MyPropertyStore::SetValue(const PROPERTYKEY& key, const PROPVARIANT& propvar) {
    return S_OK;
}
HRESULT __stdcall MyPropertyStore::Commit() {
    return S_OK;
}
MyAudioClock::MyAudioClock(MyAudioClient* p) : ref(1), m_pUnkMarshal(nullptr), parent(p) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    parent->NonDelegatingAddRef();
}
MyAudioClock::~MyAudioClock() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    parent->NonDelegatingRelease();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioClock::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioClock) {
        *ppv = static_cast<IAudioClock*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioClock::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioClock::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyAudioClock::GetFrequency(UINT64* pu64Frequency) {
    if (pu64Frequency == NULL) return E_POINTER;
    *pu64Frequency = parent->rate;
    return S_OK;
}
HRESULT __stdcall MyAudioClock::GetPosition(UINT64* pu64Position, UINT64* pu64QPCPosition) {
    return parent->GetPosition(pu64Position, pu64QPCPosition);
}
HRESULT __stdcall MyAudioClock::GetCharacteristics(DWORD* pdwCharacteristics) {
    if (pdwCharacteristics == NULL) return E_POINTER;
    *pdwCharacteristics = AUDIOCLOCK_CHARACTERISTIC_FIXED_FREQ;
    return S_OK;
}
MyAudioClock2::MyAudioClock2(MyAudioClient* p) : ref(1), m_pUnkMarshal(nullptr), parent(p) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    parent->NonDelegatingAddRef();
}
MyAudioClock2::~MyAudioClock2() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    parent->NonDelegatingRelease();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioClock2::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioClock2) {
        *ppv = static_cast<IAudioClock2*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioClock2::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioClock2::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyAudioClock2::GetDevicePosition(UINT64* pu64DevicePosition, UINT64* pu64QPCPosition) {
    if (pu64DevicePosition == NULL) return E_POINTER;
    return parent->GetPosition(pu64DevicePosition, pu64QPCPosition);
}
MyAudioClockAdjustment::MyAudioClockAdjustment(MyAudioClient* p) : ref(1), m_pUnkMarshal(nullptr), parent(p) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    parent->NonDelegatingAddRef();
}
MyAudioClockAdjustment::~MyAudioClockAdjustment() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    parent->NonDelegatingRelease();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioClockAdjustment::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioClockAdjustment) {
        *ppv = static_cast<IAudioClockAdjustment*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioClockAdjustment::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioClockAdjustment::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyAudioClockAdjustment::SetSampleRate(FLOAT fSampleRate) {
    parent->sampleRateRatio = fSampleRate;
    return S_OK;
}
MyDeviceTopology::MyDeviceTopology(const wstring& id) : ref(1), m_pUnkMarshal(nullptr), deviceId(id) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
}
MyDeviceTopology::~MyDeviceTopology() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    InterlockedDecrement(&g_objects);
}

MyConnector::MyConnector(const std::wstring& deviceId) : ref(1), m_pUnkMarshal(nullptr), connectedDeviceId(deviceId) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
}
MyConnector::~MyConnector() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    InterlockedDecrement(&g_objects);
}

HRESULT __stdcall MyConnector::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IConnector) {
        *ppv = static_cast<IConnector*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyConnector::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyConnector::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}

// IConnector methods - minimal implementations
HRESULT __stdcall MyConnector::GetType(ConnectorType* pType) {
    if (pType == NULL) return E_POINTER;
    // Return a reasonable default
    *pType = ConnectorType::Physical_Internal;
    return S_OK;
}
HRESULT __stdcall MyConnector::GetDataFlow(DataFlow* pFlow) {
    if (pFlow == NULL) return E_POINTER;
    return E_NOTIMPL;
}
HRESULT __stdcall MyConnector::ConnectTo(IConnector* pConnectTo) {
    return E_NOTIMPL;
}
HRESULT __stdcall MyConnector::Disconnect() {
    return E_NOTIMPL;
}
HRESULT __stdcall MyConnector::IsConnected(BOOL* pbConnected) {
    if (pbConnected == NULL) return E_POINTER;
    *pbConnected = FALSE;
    return S_OK;
}
HRESULT __stdcall MyConnector::GetConnectedTo(IConnector** ppConTo) {
    if (ppConTo == NULL) return E_POINTER;
    *ppConTo = NULL;
    return E_NOTFOUND;
}
HRESULT __stdcall MyConnector::GetConnectorIdConnectedTo(LPWSTR* ppwstrConnectorId) {
    if (ppwstrConnectorId == NULL) return E_POINTER;
    *ppwstrConnectorId = NULL;
    return E_NOTFOUND;
}
HRESULT __stdcall MyConnector::GetDeviceIdConnectedTo(LPWSTR* ppwstrDeviceId) {
    if (ppwstrDeviceId == NULL) return E_POINTER;
    if (connectedDeviceId.empty()) {
        *ppwstrDeviceId = NULL;
        return E_NOTFOUND;
    }
    size_t len = connectedDeviceId.length() + 1;
    *ppwstrDeviceId = (LPWSTR)CoTaskMemAlloc(len * sizeof(wchar_t));
    if (!*ppwstrDeviceId) return E_OUTOFMEMORY;
    wcscpy_s(*ppwstrDeviceId, len, connectedDeviceId.c_str());
    return S_OK;
}

HRESULT __stdcall MyDeviceTopology::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IDeviceTopology) {
        *ppv = static_cast<IDeviceTopology*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyDeviceTopology::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyDeviceTopology::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyDeviceTopology::GetConnectorCount(UINT* pCount) {
    if (pCount == NULL) return E_POINTER;
    *pCount = 1; // we expose one connector for cubeb to query
    return S_OK;
}
HRESULT __stdcall MyDeviceTopology::GetConnector(UINT nIndex, IConnector** ppConnector) {
    if (ppConnector == NULL) return E_POINTER;
    *ppConnector = NULL;
    if (nIndex != 0) return E_INVALIDARG;
    MyConnector* conn = new MyConnector(deviceId);
    if (conn == nullptr) return E_OUTOFMEMORY;
    HRESULT hr = conn->QueryInterface(IID_IConnector, (void**)ppConnector);
    conn->Release();
    return hr;
}
HRESULT __stdcall MyDeviceTopology::GetSubunitCount(UINT* pCount) {
    if (pCount == NULL) return E_POINTER;
    *pCount = 0;
    return S_OK;
}
HRESULT __stdcall MyDeviceTopology::GetSubunit(UINT nIndex, ISubunit** ppSubunit) {
    if (ppSubunit == NULL) return E_POINTER;
    *ppSubunit = NULL;
    return E_NOTIMPL;
}
HRESULT __stdcall MyDeviceTopology::GetPartById(UINT nId, IPart** ppPart) {
    if (ppPart == NULL) return E_POINTER;
    *ppPart = NULL;
    return E_NOTIMPL;
}
HRESULT __stdcall MyDeviceTopology::GetDeviceId(LPWSTR* ppwstrDeviceId) {
    if (ppwstrDeviceId == NULL) return E_POINTER;
    *ppwstrDeviceId = static_cast<LPWSTR>(CoTaskMemAlloc((deviceId.length() + 1) * sizeof(wchar_t)));
    if (!*ppwstrDeviceId) return E_OUTOFMEMORY;
    wcscpy_s(*ppwstrDeviceId, deviceId.length() + 1, deviceId.c_str());
    return S_OK;
}
HRESULT __stdcall MyDeviceTopology::GetSignalPath(IPart* pIPartFrom, IPart* pIPartTo, BOOL bRejectPending, IPartsList** ppParts) {
    if (ppParts == NULL) return E_POINTER;
    *ppParts = NULL;
    return E_NOTIMPL;
}
MyAudioEndpointVolume::MyAudioEndpointVolume(MyDevice* d) : ref(1), m_pUnkMarshal(nullptr), device(d) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    device->AddRef();
}
MyAudioEndpointVolume::~MyAudioEndpointVolume() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    device->Release();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioEndpointVolume::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioEndpointVolume) {
        *ppv = static_cast<IAudioEndpointVolume*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioEndpointVolume::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioEndpointVolume::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyAudioEndpointVolume::RegisterControlChangeNotify(IAudioEndpointVolumeCallback* pNotify) {
    if (pNotify == NULL) return E_POINTER;
    EnterCriticalSection(&g_enumerator->volumeCS);
    auto it = std::find(g_enumerator->volumeCallbacks.begin(), g_enumerator->volumeCallbacks.end(), pNotify);
    if (it == g_enumerator->volumeCallbacks.end()) {
        pNotify->AddRef();
        g_enumerator->volumeCallbacks.push_back(pNotify);
    }
    LeaveCriticalSection(&g_enumerator->volumeCS);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::UnregisterControlChangeNotify(IAudioEndpointVolumeCallback* pNotify) {
    if (pNotify == NULL) return E_POINTER;
    EnterCriticalSection(&g_enumerator->volumeCS);
    auto it = std::find(g_enumerator->volumeCallbacks.begin(), g_enumerator->volumeCallbacks.end(), pNotify);
    if (it == g_enumerator->volumeCallbacks.end()) {
        LeaveCriticalSection(&g_enumerator->volumeCS);
        return S_OK;
    }
    (*it)->Release();
    g_enumerator->volumeCallbacks.erase(it);
    LeaveCriticalSection(&g_enumerator->volumeCS);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::GetChannelCount(UINT* pnChannelCount) {
    if (pnChannelCount == NULL) return E_POINTER;
    *pnChannelCount = 2;
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::SetMasterVolumeLevel(FLOAT fLevelDB, const GUID* pguidEventContext) {
    if (device->flow == eCapture) return S_OK;
    EnterCriticalSection(&g_enumerator->volumeCS);
    g_enumerator->masterVolume = powf(10.0f, fLevelDB / 20.0f);
    g_enumerator->UpdateAllVolumes();
    LeaveCriticalSection(&g_enumerator->volumeCS);
    g_enumerator->NotifyVolumeChange(pguidEventContext);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::SetMasterVolumeLevelScalar(FLOAT fLevel, const GUID* pguidEventContext) {
    if (device->flow == eCapture) return S_OK;
    if (fLevel < 0.0f || fLevel > 1.0f) return E_INVALIDARG;
    EnterCriticalSection(&g_enumerator->volumeCS);
    g_enumerator->masterVolume = fLevel;
    g_enumerator->UpdateAllVolumes();
    LeaveCriticalSection(&g_enumerator->volumeCS);
    g_enumerator->NotifyVolumeChange(pguidEventContext);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::GetMasterVolumeLevel(FLOAT* pfLevelDB) {
    if (pfLevelDB == NULL) return E_POINTER;
    EnterCriticalSection(&g_enumerator->volumeCS);
    *pfLevelDB = (g_enumerator->masterVolume <= 0.0f) ? -96.0f : 20.0f * log10f(g_enumerator->masterVolume);
    LeaveCriticalSection(&g_enumerator->volumeCS);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::GetMasterVolumeLevelScalar(FLOAT* pfLevel) {
    if (pfLevel == NULL) return E_POINTER;
    EnterCriticalSection(&g_enumerator->volumeCS);
    *pfLevel = g_enumerator->masterVolume;
    LeaveCriticalSection(&g_enumerator->volumeCS);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::SetChannelVolumeLevel(UINT nChannel, FLOAT fLevelDB, const GUID* pguidEventContext) {
    return SetMasterVolumeLevel(fLevelDB, pguidEventContext);
}
HRESULT __stdcall MyAudioEndpointVolume::SetChannelVolumeLevelScalar(UINT nChannel, FLOAT fLevel, const GUID* pguidEventContext) {
    return SetMasterVolumeLevelScalar(fLevel, pguidEventContext);
}
HRESULT __stdcall MyAudioEndpointVolume::GetChannelVolumeLevel(UINT nChannel, FLOAT* pfLevelDB) {
    return GetMasterVolumeLevel(pfLevelDB);
}
HRESULT __stdcall MyAudioEndpointVolume::GetChannelVolumeLevelScalar(UINT nChannel, FLOAT* pfLevel) {
    return GetMasterVolumeLevelScalar(pfLevel);
}
HRESULT __stdcall MyAudioEndpointVolume::SetMute(BOOL bMute, const GUID* pguidEventContext) {
    if (device->flow == eCapture) return S_OK;
    EnterCriticalSection(&g_enumerator->volumeCS);
    g_enumerator->masterMute = bMute;
    g_enumerator->UpdateAllVolumes();
    LeaveCriticalSection(&g_enumerator->volumeCS);
    g_enumerator->NotifyVolumeChange(pguidEventContext);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::GetMute(BOOL* pbMute) {
    if (pbMute == NULL) return E_POINTER;
    EnterCriticalSection(&g_enumerator->volumeCS);
    *pbMute = g_enumerator->masterMute;
    LeaveCriticalSection(&g_enumerator->volumeCS);
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::GetVolumeRange(FLOAT* pfLevelMinDB, FLOAT* pfLevelMaxDB, FLOAT* pfVolumeIncrementDB) {
    if (pfLevelMinDB == NULL || pfLevelMaxDB == NULL || pfVolumeIncrementDB == NULL) return E_POINTER;
    *pfLevelMinDB = -96.0f;
    *pfLevelMaxDB = 0.0f;
    *pfVolumeIncrementDB = 0.1f;
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::GetVolumeStepInfo(UINT* pnStep, UINT* pnStepCount) {
    if (pnStep == NULL || pnStepCount == NULL) return E_POINTER;
    *pnStep = 10;
    *pnStepCount = 100;
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::VolumeStepUp(const GUID* pguidEventContext) {
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::VolumeStepDown(const GUID* pguidEventContext) {
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::QueryHardwareSupport(DWORD* pdwHardwareSupportMask) {
    if (pdwHardwareSupportMask == NULL) return E_POINTER;
    *pdwHardwareSupportMask = ENDPOINT_HARDWARE_SUPPORT_VOLUME | ENDPOINT_HARDWARE_SUPPORT_MUTE;
    return S_OK;
}
HRESULT __stdcall MyAudioEndpointVolume::GetVolumeRangeChannel(UINT iChannel, FLOAT* pfMin, FLOAT* pfMax, FLOAT* pfInc) {
    if (pfMin == NULL || pfMax == NULL || pfInc == NULL) return E_POINTER;
    *pfMin = -96.0f;
    *pfMax = 0.0f;
    *pfInc = 0.1f;
    return S_OK;
}
MyAudioSessionManager2::MyAudioSessionManager2(MyDevice* d) : ref(1), m_pUnkMarshal(nullptr), device(d), sessionNotifications() {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    device->AddRef();
}
MyAudioSessionManager2::~MyAudioSessionManager2() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    for (auto n : sessionNotifications) n->Release();
    device->Release();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioSessionManager2::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioSessionManager || iid == IID_IAudioSessionManager2) {
        *ppv = static_cast<IAudioSessionManager2*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioSessionManager2::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioSessionManager2::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyAudioSessionManager2::GetAudioSessionControl(const GUID* AudioSessionGuid, DWORD StreamFlags, IAudioSessionControl** SessionControl) {
    if (SessionControl == NULL) return E_POINTER;
    *SessionControl = NULL;
    GUID guid = AudioSessionGuid ? *AudioSessionGuid : GUID_NULL;
    MyAudioSession* s = g_enumerator->GetSession(guid, true);
    MyAudioSessionControl* ctrl = new MyAudioSessionControl(s);
    HRESULT hr = ctrl->QueryInterface(IID_IAudioSessionControl, reinterpret_cast<void**>(SessionControl));
    ctrl->Release();
    return hr;
}
HRESULT __stdcall MyAudioSessionManager2::GetSimpleAudioVolume(const GUID* AudioSessionGuid, DWORD StreamFlags, ISimpleAudioVolume** AudioVolume) {
    if (AudioVolume == NULL) return E_POINTER;
    *AudioVolume = NULL;
    GUID guid = AudioSessionGuid ? *AudioSessionGuid : GUID_NULL;
    MyAudioSession* s = g_enumerator->GetSession(guid, true);
    *AudioVolume = new MySimpleAudioVolume(s);
    if (*AudioVolume == nullptr) return E_OUTOFMEMORY;
    return S_OK;
}
HRESULT __stdcall MyAudioSessionManager2::GetSessionEnumerator(IAudioSessionEnumerator** SessionEnumerator) {
    if (SessionEnumerator == NULL) return E_POINTER;
    *SessionEnumerator = NULL;
    *SessionEnumerator = new MyAudioSessionEnumerator(g_enumerator->sessions);
    if (*SessionEnumerator == nullptr) return E_OUTOFMEMORY;
    return S_OK;
}
HRESULT __stdcall MyAudioSessionManager2::RegisterSessionNotification(IAudioSessionNotification* SessionNotification) {
    if (SessionNotification == NULL) return E_POINTER;
    auto it = std::find(sessionNotifications.begin(), sessionNotifications.end(), SessionNotification);
    if (it == sessionNotifications.end()) {
        SessionNotification->AddRef();
        sessionNotifications.push_back(SessionNotification);
    }
    return S_OK;
}
HRESULT __stdcall MyAudioSessionManager2::UnregisterSessionNotification(IAudioSessionNotification* SessionNotification) {
    if (SessionNotification == NULL) return E_POINTER;
    auto it = std::find(sessionNotifications.begin(), sessionNotifications.end(), SessionNotification);
    if (it == sessionNotifications.end()) return S_OK;
    (*it)->Release();
    sessionNotifications.erase(it);
    return S_OK;
}
HRESULT __stdcall MyAudioSessionManager2::RegisterDuckNotification(LPCWSTR SessionID, IAudioVolumeDuckNotification* duckNotification) {
    return S_OK;
}
HRESULT __stdcall MyAudioSessionManager2::UnregisterDuckNotification(IAudioVolumeDuckNotification* duckNotification) {
    return S_OK;
}
MyAudioSessionEnumerator::MyAudioSessionEnumerator(const vector<MyAudioSession*>& sessions) : ref(1), m_pUnkMarshal(nullptr) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    controls.reserve(sessions.size());
    for (auto s : sessions) {
        controls.push_back(new MyAudioSessionControl(s));
    }
}
MyAudioSessionEnumerator::~MyAudioSessionEnumerator() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    for (auto c : controls) c->Release();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioSessionEnumerator::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioSessionEnumerator) {
        *ppv = static_cast<IAudioSessionEnumerator*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioSessionEnumerator::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioSessionEnumerator::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyAudioSessionEnumerator::GetCount(int* SessionCount) {
    if (SessionCount == NULL) return E_POINTER;
    *SessionCount = static_cast<int>(controls.size());
    return S_OK;
}
HRESULT __stdcall MyAudioSessionEnumerator::GetSession(int SessionNum, IAudioSessionControl** Session) {
    if (Session == NULL) return E_POINTER;
    if (SessionNum < 0 || SessionNum >= static_cast<int>(controls.size())) return E_INVALIDARG;
    *Session = controls[SessionNum];
    (*Session)->AddRef();
    return S_OK;
}
MyAudioSessionControl::MyAudioSessionControl(MyAudioSession* s) : ref(1), m_pUnkMarshal(nullptr), session(s) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    session->AddRef();
}
MyAudioSessionControl::~MyAudioSessionControl() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    session->Release();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MyAudioSessionControl::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioSessionControl || iid == IID_IAudioSessionControl2) {
        *ppv = static_cast<IAudioSessionControl2*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MyAudioSessionControl::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MyAudioSessionControl::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MyAudioSessionControl::GetState(AudioSessionState* pRetVal) {
    if (pRetVal == NULL) return E_POINTER;
    *pRetVal = session->state;
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::GetDisplayName(LPWSTR* pRetVal) {
    if (pRetVal == NULL) return E_POINTER;
    *pRetVal = static_cast<LPWSTR>(CoTaskMemAlloc((session->displayName.length() + 1) * sizeof(wchar_t)));
    if (!*pRetVal) return E_OUTOFMEMORY;
    wcscpy_s(*pRetVal, session->displayName.length() + 1, session->displayName.c_str());
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::SetDisplayName(LPCWSTR Value, const GUID* EventContext) {
    session->displayName = Value ? Value : L"";
    for (auto e : session->events) {
        e->OnDisplayNameChanged(session->displayName.c_str(), EventContext);
    }
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::GetIconPath(LPWSTR* pRetVal) {
    if (pRetVal == NULL) return E_POINTER;
    *pRetVal = static_cast<LPWSTR>(CoTaskMemAlloc((session->iconPath.length() + 1) * sizeof(wchar_t)));
    if (!*pRetVal) return E_OUTOFMEMORY;
    wcscpy_s(*pRetVal, session->iconPath.length() + 1, session->iconPath.c_str());
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::SetIconPath(LPCWSTR Value, const GUID* EventContext) {
    session->iconPath = Value ? Value : L"";
    for (auto e : session->events) {
        e->OnIconPathChanged(session->iconPath.c_str(), EventContext);
    }
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::GetGroupingParam(GUID* pRetVal) {
    if (pRetVal == NULL) return E_POINTER;
    *pRetVal = GUID_NULL;
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::SetGroupingParam(const GUID* Override, const GUID* EventContext) {
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::RegisterAudioSessionNotification(IAudioSessionEvents* NewNotifications) {
    if (NewNotifications == NULL) return E_POINTER;
    auto it = std::find(session->events.begin(), session->events.end(), NewNotifications);
    if (it == session->events.end()) {
        NewNotifications->AddRef();
        session->events.push_back(NewNotifications);
    }
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::UnregisterAudioSessionNotification(IAudioSessionEvents* NewNotifications) {
    if (NewNotifications == NULL) return E_POINTER;
    auto it = std::find(session->events.begin(), session->events.end(), NewNotifications);
    if (it == session->events.end()) return S_OK;
    (*it)->Release();
    session->events.erase(it);
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::GetSessionIdentifier(LPWSTR* pRetVal) {
    if (pRetVal == NULL) return E_POINTER;
    wchar_t buf[39];
    StringFromGUID2(session->guid, buf, 39);
    *pRetVal = static_cast<LPWSTR>(CoTaskMemAlloc((wcslen(buf) + 1) * sizeof(wchar_t)));
    if (!*pRetVal) return E_OUTOFMEMORY;
    wcscpy_s(*pRetVal, wcslen(buf) + 1, buf);
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::GetSessionInstanceIdentifier(LPWSTR* pRetVal) {
    return GetSessionIdentifier(pRetVal);
}
HRESULT __stdcall MyAudioSessionControl::GetProcessId(DWORD* pRetVal) {
    if (pRetVal == NULL) return E_POINTER;
    *pRetVal = GetCurrentProcessId();
    return S_OK;
}
HRESULT __stdcall MyAudioSessionControl::IsSystemSoundsSession() {
    return S_FALSE;
}
HRESULT __stdcall MyAudioSessionControl::SetDuckingPreference(BOOL optOut) {
    return S_OK;
}
MySimpleAudioVolume::MySimpleAudioVolume(MyAudioSession* s) : ref(1), m_pUnkMarshal(nullptr), session(s) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    session->AddRef();
}
MySimpleAudioVolume::~MySimpleAudioVolume() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    session->Release();
    InterlockedDecrement(&g_objects);
}
HRESULT __stdcall MySimpleAudioVolume::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_ISimpleAudioVolume) {
        *ppv = static_cast<ISimpleAudioVolume*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG __stdcall MySimpleAudioVolume::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG __stdcall MySimpleAudioVolume::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT __stdcall MySimpleAudioVolume::SetMasterVolume(float fLevel, const GUID* EventContext) {
    if (fLevel < 0.0f || fLevel > 1.0f) return E_INVALIDARG;
    session->volume = fLevel;
    session->UpdateVolumes();
    for (auto e : session->events) {
        e->OnSimpleVolumeChanged(fLevel, session->mute, EventContext);
    }
    return S_OK;
}
HRESULT __stdcall MySimpleAudioVolume::GetMasterVolume(float* pfLevel) {
    if (pfLevel == NULL) return E_POINTER;
    *pfLevel = session->volume;
    return S_OK;
}
HRESULT __stdcall MySimpleAudioVolume::SetMute(BOOL bMute, const GUID* EventContext) {
    session->mute = bMute;
    session->UpdateVolumes();
    for (auto e : session->events) {
        e->OnSimpleVolumeChanged(session->volume, bMute, EventContext);
    }
    return S_OK;
}
HRESULT __stdcall MySimpleAudioVolume::GetMute(BOOL* pbMute) {
    if (pbMute == NULL) return E_POINTER;
    *pbMute = session->mute;
    return S_OK;
}
MyAudioStreamVolume::MyAudioStreamVolume(MyAudioClient* p) : ref(1), m_pUnkMarshal(nullptr), parent(p) {
    CoCreateFreeThreadedMarshaler(static_cast<IUnknown*>(this), &m_pUnkMarshal);
    InterlockedIncrement(&g_objects);
    parent->NonDelegatingAddRef();
}
MyAudioStreamVolume::~MyAudioStreamVolume() {
    if (m_pUnkMarshal) m_pUnkMarshal->Release();
    parent->NonDelegatingRelease();
    InterlockedDecrement(&g_objects);
}
HRESULT STDMETHODCALLTYPE MyAudioStreamVolume::QueryInterface(REFIID iid, void** ppv) {
    if (ppv == NULL) return E_POINTER;
    if (iid == IID_IUnknown || iid == IID_IAudioStreamVolume) {
        *ppv = static_cast<IAudioStreamVolume*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == IID_IMarshal || iid == IID_IMarshal2 || iid == IID_IProvideClassInfo || iid == IID_IProvideClassInfo2 || iid == IID_IPersist) {
        if (m_pUnkMarshal) {
            return m_pUnkMarshal->QueryInterface(iid, ppv);
        }
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
ULONG STDMETHODCALLTYPE MyAudioStreamVolume::AddRef() {
    return InterlockedIncrement(&ref);
}
ULONG STDMETHODCALLTYPE MyAudioStreamVolume::Release() {
    ULONG newRef = InterlockedDecrement(&ref);
    if (newRef == 0) delete this;
    return newRef;
}
HRESULT STDMETHODCALLTYPE MyAudioStreamVolume::GetChannelCount(UINT32* pdwCount) {
    if (pdwCount == NULL) return E_POINTER;
    *pdwCount = static_cast<DWORD>(parent->channelVolumes.size());
    return S_OK;
}
HRESULT STDMETHODCALLTYPE MyAudioStreamVolume::SetChannelVolume(UINT32 dwIndex, const float fLevel) {
    if (dwIndex >= parent->channelVolumes.size()) return E_INVALIDARG;
    if (fLevel < 0.0f || fLevel > 1.0f) return E_INVALIDARG;
    parent->channelVolumes[dwIndex] = fLevel;
    return S_OK;
}
HRESULT STDMETHODCALLTYPE MyAudioStreamVolume::GetChannelVolume(UINT32 dwIndex, float* pfLevel) {
    if (pfLevel == NULL) return E_POINTER;
    if (dwIndex >= parent->channelVolumes.size()) return E_INVALIDARG;
    *pfLevel = parent->channelVolumes[dwIndex];
    return S_OK;
}
HRESULT STDMETHODCALLTYPE MyAudioStreamVolume::SetAllVolumes(UINT32 dwCount, const float* pfVolumes) {
    if (dwCount != parent->channelVolumes.size() || pfVolumes == NULL) return E_INVALIDARG;
    for (DWORD i = 0; i < dwCount; ++i) {
        if (pfVolumes[i] < 0.0f || pfVolumes[i] > 1.0f) return E_INVALIDARG;
    }
    std::copy(pfVolumes, pfVolumes + dwCount, parent->channelVolumes.begin());
    return S_OK;
}
HRESULT STDMETHODCALLTYPE MyAudioStreamVolume::GetAllVolumes(UINT32 dwCount, float* pfVolumes) {
    if (dwCount != parent->channelVolumes.size() || pfVolumes == NULL) return E_INVALIDARG;
    std::copy(parent->channelVolumes.begin(), parent->channelVolumes.end(), pfVolumes);
    return S_OK;
}
class MyClassFactory : public IClassFactory {
private:
    LONG ref;
public:
    MyClassFactory() : ref(1) {
        InterlockedIncrement(&g_objects);
    }
    ~MyClassFactory() {
        InterlockedDecrement(&g_objects);
    }
    HRESULT __stdcall QueryInterface(REFIID iid, void** ppv) {
        if (ppv == NULL) return E_POINTER;
        if (iid == IID_IUnknown || iid == IID_IClassFactory) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() {
        return InterlockedIncrement(&ref);
    }
    ULONG __stdcall Release() {
        ULONG newRef = InterlockedDecrement(&ref);
        if (newRef == 0) delete this;
        return newRef;
    }
    HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) {
        if (pUnkOuter && riid != IID_IUnknown) return CLASS_E_NOAGGREGATION;
        return g_enumerator->QueryInterface(riid, ppv);
    }
    HRESULT __stdcall LockServer(BOOL fLock) {
        if (fLock) InterlockedIncrement(&g_objects);
        else InterlockedDecrement(&g_objects);
        return S_OK;
    }
};
class MyActivateAudioInterfaceAsyncOperation : public IActivateAudioInterfaceAsyncOperation {
private:
    LONG ref;
    HRESULT activateResult;
    IUnknown* activatedInterface;
    IActivateAudioInterfaceCompletionHandler* handler;
    HANDLE thread;
    wchar_t* deviceInterfacePath;
    IID riid;
    PROPVARIANT activationParams;
public:
    MyActivateAudioInterfaceAsyncOperation(LPCWSTR path, REFIID r, PROPVARIANT* params, IActivateAudioInterfaceCompletionHandler* h) : ref(1), activateResult(E_FAIL), activatedInterface(NULL), handler(h), thread(NULL), deviceInterfacePath(NULL) {
        deviceInterfacePath = _wcsdup(path);
        riid = r;
        if (params) {
            activationParams = *params;
        }
        else {
            PropVariantInit(&activationParams);
        }
        handler->AddRef();
    }
    ~MyActivateAudioInterfaceAsyncOperation() {
        if (deviceInterfacePath) free(deviceInterfacePath);
        if (activatedInterface) activatedInterface->Release();
        handler->Release();
        if (thread) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
        }
    }
    HRESULT __stdcall QueryInterface(REFIID iid, void** ppv) {
        if (ppv == NULL) return E_POINTER;
        if (iid == IID_IUnknown || iid == IID_IActivateAudioInterfaceAsyncOperation) {
            *ppv = static_cast<IActivateAudioInterfaceAsyncOperation*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() {
        return InterlockedIncrement(&ref);
    }
    ULONG __stdcall Release() {
        ULONG newRef = InterlockedDecrement(&ref);
        if (newRef == 0) delete this;
        return newRef;
    }
    HRESULT __stdcall GetActivateResult(HRESULT* ar, IUnknown** ai) {
        if (ar) *ar = activateResult;
        if (ai) {
            *ai = activatedInterface;
            if (activatedInterface) activatedInterface->AddRef();
        }
        return S_OK;
    }
    void Start() {
        AddRef();
        thread = CreateThread(NULL, 0, ActivateThread, this, 0, NULL);
    }
    static DWORD WINAPI ActivateThread(LPVOID param) {
        CoInitialize(NULL);
        MyActivateAudioInterfaceAsyncOperation* self = static_cast<MyActivateAudioInterfaceAsyncOperation*>(param);
        IMMDevice* dev = NULL;
        HRESULT hr = g_enumerator->GetDevice(self->deviceInterfacePath, &dev);
        if (SUCCEEDED(hr)) {
            void* intf = NULL;
            hr = dev->Activate(self->riid, CLSCTX_INPROC_SERVER, &self->activationParams, &intf);
            self->activatedInterface = static_cast<IUnknown*>(intf);
            self->activateResult = hr;
            dev->Release();
        }
        else {
            self->activateResult = hr;
        }
        self->handler->ActivateCompleted(self);
        CoUninitialize();
        self->Release();
        return 0;
    }
};
extern "C" {
    STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
        if (ppv == NULL) return E_POINTER;
        if (!IsEqualCLSID(rclsid, CLSID_MMDeviceEnumerator)) return CLASS_E_CLASSNOTAVAILABLE;
        MyClassFactory* factory = new MyClassFactory();
        if (factory == nullptr) return E_OUTOFMEMORY;
        HRESULT hr = factory->QueryInterface(riid, ppv);
        factory->Release();
        return hr;
    }
    STDAPI DllCanUnloadNow() {
        return (g_objects == 0) ? S_OK : S_FALSE;
    }
    STDAPI DllRegisterServer() {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(g_hInstance, path, MAX_PATH);
        HKEY hKey = NULL;
        LONG res = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{BCDE0395-E52F-467C-8E3D-C4579291692E}", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (res == ERROR_SUCCESS) {
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)L"MMDeviceEnumerator Class", (DWORD)((wcslen(L"MMDeviceEnumerator Class") + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
        res = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{BCDE0395-E52F-467C-8E3D-C4579291692E}\\InprocServer32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (res == ERROR_SUCCESS) {
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)path, (DWORD)((wcslen(path) + 1) * sizeof(wchar_t)));
            RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ, (BYTE*)L"both", (DWORD)((wcslen(L"both") + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
        res = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{06CCA63E-9941-441B-B004-39F999ADA412}", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (res == ERROR_SUCCESS) {
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)L"MMEndpointManager Class", (DWORD)((wcslen(L"MMEndpointManager Class") + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
        res = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{06CCA63E-9941-441B-B004-39F999ADA412}\\InprocServer32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (res == ERROR_SUCCESS) {
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)path, (DWORD)((wcslen(path) + 1) * sizeof(wchar_t)));
            RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ, (BYTE*)L"both", (DWORD)((wcslen(L"both") + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
        res = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{E2F7A62A-862B-40AE-BBC2-5C0CA9A5B7E1}", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (res == ERROR_SUCCESS) {
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)L"ActivateAudioInterfaceWorker class", (DWORD)((wcslen(L"ActivateAudioInterfaceWorker class") + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
        res = RegCreateKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{E2F7A62A-862B-40AE-BBC2-5C0CA9A5B7E1}\\InprocServer32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
        if (res == ERROR_SUCCESS) {
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)path, (DWORD)((wcslen(path) + 1) * sizeof(wchar_t)));
            RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ, (BYTE*)L"free", (DWORD)((wcslen(L"free") + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
        return S_OK;
    }
    STDAPI DllUnregisterServer() {
        RegDeleteKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{BCDE0395-E52F-467C-8E3D-C4579291692E}\\InprocServer32");
        RegDeleteKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{BCDE0395-E52F-467C-8E3D-C4579291692E}");
        RegDeleteKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{06CCA63E-9941-441B-B004-39F999ADA412}\\InprocServer32");
        RegDeleteKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{06CCA63E-9941-441B-B004-39F999ADA412}");
        RegDeleteKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{E2F7A62A-862B-40AE-BBC2-5C0CA9A5B7E1}\\InprocServer32");
        RegDeleteKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Classes\\CLSID\\{E2F7A62A-862B-40AE-BBC2-5C0CA9A5B7E1}");
        return S_OK;
    }
    STDAPI ActivateAudioInterfaceAsync(LPCWSTR deviceInterfacePath, REFIID riid, PROPVARIANT* activationParams, IActivateAudioInterfaceCompletionHandler* completionHandler, IActivateAudioInterfaceAsyncOperation** asyncOp) {
        if (deviceInterfacePath == NULL || completionHandler == NULL || asyncOp == NULL) return E_POINTER;
        *asyncOp = NULL;
        MyActivateAudioInterfaceAsyncOperation* op = new MyActivateAudioInterfaceAsyncOperation(deviceInterfacePath, riid, activationParams, completionHandler);
        if (op == NULL) return E_OUTOFMEMORY;
        *asyncOp = op;
        op->AddRef();
        op->Start();
        return S_OK;
    }
}