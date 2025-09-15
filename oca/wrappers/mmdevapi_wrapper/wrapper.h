// wrapper.h
#ifndef WRAPPER_H
#define WRAPPER_H

#define NOMINMAX
#include <windows.h>
#include <objbase.h>
#include <objidl.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <dsound.h>
#include <functiondiscoverykeys_devpkey.h>
#include <ksmedia.h>
#include <devicetopology.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <string>
#include <vector>

using std::wstring;
using std::vector;

extern LONG g_objects;
extern HINSTANCE g_hInstance;

class MyDeviceEnumerator;
class MyDeviceCollection;
class MyDevice;
class MyAudioClient;
class MyRenderClient;
class MyCaptureClient;
class MyPropertyStore;
class MyAudioClock;
class MyDeviceTopology;
class MyAudioClockAdjustment;
class MyAudioEndpointVolume;
class MyAudioSessionManager2;
class MyAudioSessionControl;
class MySimpleAudioVolume;
class MyAudioSessionEnumerator;
class MyAudioSession;
class MyAudioStreamVolume;

extern MyDeviceEnumerator* g_enumerator;

#ifndef AUDCLNT_STREAMFLAGS_CROSSPROCESS
#define AUDCLNT_STREAMFLAGS_CROSSPROCESS 0x00010000
#endif
#ifndef AUDCLNT_STREAMFLAGS_LOOPBACK
#define AUDCLNT_STREAMFLAGS_LOOPBACK 0x00020000
#endif
#ifndef AUDCLNT_STREAMFLAGS_EVENTCALLBACK
#define AUDCLNT_STREAMFLAGS_EVENTCALLBACK 0x00040000
#endif
#ifndef AUDCLNT_STREAMFLAGS_NOPERSIST
#define AUDCLNT_STREAMFLAGS_NOPERSIST 0x00080000
#endif
#ifndef AUDCLNT_STREAMFLAGS_RATEADJUST
#define AUDCLNT_STREAMFLAGS_RATEADJUST 0x00100000
#endif
#ifndef AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED
#define AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED 0x10000000
#endif
#ifndef AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE
#define AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE 0x20000000
#endif
#ifndef AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED
#define AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED 0x40000000
#endif
#ifndef AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
#define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY 0x08000000
#endif
#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTSRC
#define AUDCLNT_STREAMFLAGS_AUTOCONVERTSRC 0x10000000
#endif
#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
#endif

class MyAudioSession : public IUnknown {
private:
    LONG ref;
public:
    GUID guid;
    vector<MyAudioClient*> clients;
    float volume;
    bool mute;
    wstring displayName;
    wstring iconPath;
    AudioSessionState state;
    vector<IAudioSessionEvents*> events;
    MyAudioSession();
    ~MyAudioSession();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    void UpdateVolumes();
    void SetState(AudioSessionState newState);
};

class MyDeviceEnumerator : public IMMDeviceEnumerator {
private:
    IUnknown* m_pUnkOuter;
    IUnknown* m_pUnkMarshal;
    BYTE* loopBuf;
    UINT32 loopBytes;
    UINT64 loopReadPos;
    UINT64 loopWritePos;
    UINT32 loopPaddingFrames;
    UINT64 loopPositionFrames;
    WAVEFORMATEXTENSIBLE loopFormat;
    CRITICAL_SECTION loopCS;
    HANDLE loopEvent;
    LONG ref;
    vector<IMMNotificationClient*> clients;
    vector<MyAudioSession*> sessions;
    float masterVolume;
    bool masterMute;
    vector<IAudioEndpointVolumeCallback*> volumeCallbacks;
    CRITICAL_SECTION volumeCS;
    MyAudioClient* loopMicClient;
    IAudioCaptureClient* loopMicCapture;
    HANDLE pumpThread;
    HANDLE loopMicEvent;
    volatile bool loopbackRunning;
    int loopbackCount;
    friend class MyAudioClient;
    friend class MyRenderClient;
    friend class MyCaptureClient;
    friend class MyAudioClock;
    friend class MyAudioEndpointVolume;
    friend class MySimpleAudioVolume;
    friend class MyAudioSessionControl;
    friend class MyAudioSessionManager2;
public:
    MyDeviceEnumerator(IUnknown* pUnkOuter = nullptr);
    ~MyDeviceEnumerator();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    HRESULT NonDelegatingQueryInterface(REFIID iid, void** ppv);
    ULONG NonDelegatingAddRef();
    ULONG NonDelegatingRelease();
    virtual HRESULT __stdcall EnumAudioEndpoints(EDataFlow dataFlow, DWORD dwStateMask, IMMDeviceCollection** ppDevices);
    virtual HRESULT __stdcall GetDefaultAudioEndpoint(EDataFlow dataFlow, ERole role, IMMDevice** ppEndpoint);
    virtual HRESULT __stdcall GetDevice(const wchar_t* pwstrId, IMMDevice** ppDevice);
    virtual HRESULT __stdcall RegisterEndpointNotificationCallback(IMMNotificationClient* pClient);
    virtual HRESULT __stdcall UnregisterEndpointNotificationCallback(IMMNotificationClient* pClient);
    MyAudioSession* GetSession(const GUID& guid, bool create = true);
    void RemoveSession(MyAudioSession* s);
    void UpdateAllVolumes();
    void NotifyVolumeChange(const GUID* context);
    static DWORD WINAPI PumpFromMicToLoop(LPVOID param);
};

class MyDeviceCollection : public IMMDeviceCollection {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    vector<IMMDevice*> devices;
public:
    MyDeviceCollection(const vector<IMMDevice*>& devs);
    ~MyDeviceCollection();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetCount(UINT* pcDevices);
    virtual HRESULT __stdcall Item(UINT nDevice, IMMDevice** ppDevice);
};

class MyDevice : public IMMDevice, public IMMEndpoint {
private:
    LONG ref;
    GUID deviceGuid;
    wstring name;
    wstring id;
public:
    EDataFlow flow;
    MyDevice(EDataFlow f, const GUID& g, const wstring& n);
    ~MyDevice();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall Activate(REFIID iid, DWORD dwClsCtx, PROPVARIANT* pActivationParams, void** ppInterface);
    virtual HRESULT __stdcall OpenPropertyStore(DWORD stgmAccess, IPropertyStore** ppProperties);
    virtual HRESULT __stdcall GetId(LPWSTR* ppstrId);
    virtual HRESULT __stdcall GetState(DWORD* pdwState);
    virtual HRESULT __stdcall GetDataFlow(EDataFlow* pDataFlow);
};

class MyAudioClient : public IAudioClient3 {
private:
    IUnknown* m_pUnkOuter;
    IUnknown* m_pUnkMarshal;
    LONG ref;
    EDataFlow flow;
    GUID deviceGuid;
    WAVEFORMATEXTENSIBLE format;
    UINT32 bufferFrames;
    UINT32 blockAlign;
    UINT32 rate;
    REFERENCE_TIME bufferDuration;
    IDirectSound8* ds;
    IDirectSoundCapture8* dsc;
    IDirectSoundBuffer8* dsBuffer;
    IDirectSoundCaptureBuffer8* dscBuffer;
    IDirectSoundNotify* dsNotify;
    DWORD bufferBytes;
    UINT64 lastPos;
    HANDLE hEvent;
    HANDLE hThread;
    HANDLE notifyEvent;
    bool started;
    bool isEventDriven;
    bool locked;
    void* lockP1;
    void* lockP2;
    DWORD lockL1;
    DWORD lockL2;
    CRITICAL_SECTION cs;
    static DWORD WINAPI MonitorThread(LPVOID lpParam);
    HRESULT UpdatePositions(UINT32* padding);
    friend class MyRenderClient;
    friend class MyCaptureClient;
    friend class MyAudioClock;
    friend class MyAudioStreamVolume;
    UINT32 currentPaddingFrames;
    DWORD prevPos;
    bool positionsInitialized;
    HRESULT FillSilence();
    AUDCLNT_SHAREMODE shareMode;
    UINT32 periodFrames;
    bool isLoopback;
    bool lowLatencyShared;
    HRESULT InternalInitialize(AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags, REFERENCE_TIME hnsBufferDuration, REFERENCE_TIME hnsPeriodicity, const WAVEFORMATEX* pFormat, const GUID* AudioSessionGuid);
    UINT64 devicePositionFrames;
    GUID sessionGuid;
    MyAudioSession* session;
    vector<float> channelVolumes;
public:
    MyAudioClient(EDataFlow f, const GUID& g, IUnknown* pUnkOuter = nullptr);
    ~MyAudioClient();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    HRESULT NonDelegatingQueryInterface(REFIID iid, void** ppv);
    ULONG NonDelegatingAddRef();
    ULONG NonDelegatingRelease();
    virtual HRESULT __stdcall Initialize(AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags, REFERENCE_TIME hnsBufferDuration, REFERENCE_TIME hnsPeriodicity, const WAVEFORMATEX* pFormat, const GUID* AudioSessionGuid);
    virtual HRESULT __stdcall GetBufferSize(UINT32* pNumBufferFrames);
    virtual HRESULT __stdcall GetStreamLatency(REFERENCE_TIME* phnsLatency);
    virtual HRESULT __stdcall GetCurrentPadding(UINT32* pNumPaddingFrames);
    virtual HRESULT __stdcall IsFormatSupported(AUDCLNT_SHAREMODE ShareMode, const WAVEFORMATEX* pFormat, WAVEFORMATEX** ppClosestMatch);
    virtual HRESULT __stdcall GetMixFormat(WAVEFORMATEX** ppDeviceFormat);
    virtual HRESULT __stdcall GetDevicePeriod(REFERENCE_TIME* phnsDefaultDevicePeriod, REFERENCE_TIME* phnsMinimumDevicePeriod);
    virtual HRESULT __stdcall Start();
    virtual HRESULT __stdcall Stop();
    virtual HRESULT __stdcall Reset();
    virtual HRESULT __stdcall SetEventHandle(HANDLE eventHandle);
    virtual HRESULT __stdcall GetService(REFIID riid, void** ppv);
    virtual HRESULT __stdcall IsOffloadCapable(AUDIO_STREAM_CATEGORY Category, BOOL* pbOffloadCapable);
    virtual HRESULT __stdcall SetClientProperties(const AudioClientProperties* pProperties);
    virtual HRESULT __stdcall GetBufferSizeLimits(const WAVEFORMATEX* pFormat, BOOL bEventDriven, REFERENCE_TIME* phnsMinBufferDuration, REFERENCE_TIME* phnsMaxBufferDuration);
    virtual HRESULT __stdcall GetSharedModeEnginePeriod(const WAVEFORMATEX* pFormat, UINT32* pDefaultPeriodInFrames, UINT32* pFundamentalPeriodInFrames, UINT32* pMinPeriodInFrames, UINT32* pMaxPeriodInFrames);
    virtual HRESULT __stdcall GetCurrentSharedModeEnginePeriod(WAVEFORMATEX** ppFormat, UINT32* pCurrentPeriodInFrames);
    virtual HRESULT __stdcall InitializeSharedAudioStream(DWORD StreamFlags, UINT32 PeriodInFrames, const WAVEFORMATEX* pFormat, const GUID* AudioSessionGuid);
    void UpdateVolume();
};

class MyRenderClient : public IAudioRenderClient {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyAudioClient* parent;
    BYTE* tempBuffer;
    bool usingTemp;
public:
    MyRenderClient(MyAudioClient* p);
    ~MyRenderClient();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetBuffer(UINT32 NumFramesRequested, BYTE** ppData);
    virtual HRESULT __stdcall ReleaseBuffer(UINT32 NumFramesWritten, DWORD dwFlags);
};

class MyCaptureClient : public IAudioCaptureClient {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyAudioClient* parent;
    BYTE* tempBuffer;
    bool usingTemp;
public:
    MyCaptureClient(MyAudioClient* p);
    ~MyCaptureClient();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetBuffer(BYTE** ppData, UINT32* pNumFramesToRead, DWORD* pdwFlags, UINT64* pu64DevicePosition, UINT64* pu64QPCPosition);
    virtual HRESULT __stdcall ReleaseBuffer(UINT32 NumFramesRead);
    virtual HRESULT __stdcall GetNextPacketSize(UINT32* pNumFramesInNextPacket);
};

class MyPropertyStore : public IPropertyStore {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    wstring deviceName;
    EDataFlow flow;
    WAVEFORMATEXTENSIBLE mixFormat;
    wstring endpointGuid;
public:
    MyPropertyStore(const wstring& n, EDataFlow f, const wstring& guidStr);
    ~MyPropertyStore();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetCount(DWORD* cProps);
    virtual HRESULT __stdcall GetAt(DWORD iProp, PROPERTYKEY* pkey);
    virtual HRESULT __stdcall GetValue(const PROPERTYKEY& key, PROPVARIANT* pv);
    virtual HRESULT __stdcall SetValue(const PROPERTYKEY& key, const PROPVARIANT& propvar);
    virtual HRESULT __stdcall Commit();
};

class MyAudioClock : public IAudioClock {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyAudioClient* parent;
public:
    MyAudioClock(MyAudioClient* p);
    ~MyAudioClock();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetFrequency(UINT64* pu64Frequency);
    virtual HRESULT __stdcall GetPosition(UINT64* pu64Position, UINT64* pu64QPCPosition);
    virtual HRESULT __stdcall GetCharacteristics(DWORD* pdwCharacteristics);
};

class MyAudioClockAdjustment : public IAudioClockAdjustment {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyAudioClient* parent;
public:
    MyAudioClockAdjustment(MyAudioClient* p);
    ~MyAudioClockAdjustment();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall SetSampleRate(FLOAT fSampleRate);
};

class MyDeviceTopology : public IDeviceTopology {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    wstring deviceId;
public:
    MyDeviceTopology(const wstring& id);
    ~MyDeviceTopology();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetConnectorCount(UINT* pCount);
    virtual HRESULT __stdcall GetConnector(UINT nIndex, IConnector** ppConnector);
    virtual HRESULT __stdcall GetSubunitCount(UINT* pCount);
    virtual HRESULT __stdcall GetSubunit(UINT nIndex, ISubunit** ppSubunit);
    virtual HRESULT __stdcall GetPartById(UINT nId, IPart** ppPart);
    virtual HRESULT __stdcall GetDeviceId(LPWSTR* ppwstrDeviceId);
    virtual HRESULT __stdcall GetSignalPath(IPart* pIPartFrom, IPart* pIPartTo, BOOL bRejectPending, IPartsList** ppParts);
};

class MyAudioEndpointVolume : public IAudioEndpointVolume {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyDevice* device;
public:
    MyAudioEndpointVolume(MyDevice* d);
    ~MyAudioEndpointVolume();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall RegisterControlChangeNotify(IAudioEndpointVolumeCallback* pNotify);
    virtual HRESULT __stdcall UnregisterControlChangeNotify(IAudioEndpointVolumeCallback* pNotify);
    virtual HRESULT __stdcall GetChannelCount(UINT* pnChannelCount);
    virtual HRESULT __stdcall SetMasterVolumeLevel(FLOAT fLevelDB, const GUID* pguidEventContext);
    virtual HRESULT __stdcall SetMasterVolumeLevelScalar(FLOAT fLevel, const GUID* pguidEventContext);
    virtual HRESULT __stdcall GetMasterVolumeLevel(FLOAT* pfLevelDB);
    virtual HRESULT __stdcall GetMasterVolumeLevelScalar(FLOAT* pfLevel);
    virtual HRESULT __stdcall SetChannelVolumeLevel(UINT nChannel, FLOAT fLevelDB, const GUID* pguidEventContext);
    virtual HRESULT __stdcall SetChannelVolumeLevelScalar(UINT nChannel, FLOAT fLevel, const GUID* pguidEventContext);
    virtual HRESULT __stdcall GetChannelVolumeLevel(UINT nChannel, FLOAT* pfLevelDB);
    virtual HRESULT __stdcall GetChannelVolumeLevelScalar(UINT nChannel, FLOAT* pfLevel);
    virtual HRESULT __stdcall SetMute(BOOL bMute, const GUID* pguidEventContext);
    virtual HRESULT __stdcall GetMute(BOOL* pbMute);
    virtual HRESULT __stdcall GetVolumeRange(FLOAT* pfLevelMinDB, FLOAT* pfLevelMaxDB, FLOAT* pfVolumeIncrementDB);
    virtual HRESULT __stdcall GetVolumeStepInfo(UINT* pnStep, UINT* pnStepCount);
    virtual HRESULT __stdcall VolumeStepUp(const GUID* pguidEventContext);
    virtual HRESULT __stdcall VolumeStepDown(const GUID* pguidEventContext);
    virtual HRESULT __stdcall QueryHardwareSupport(DWORD* pdwHardwareSupportMask);
    virtual HRESULT __stdcall GetVolumeRangeChannel(UINT iChannel, FLOAT* pfMin, FLOAT* pfMax, FLOAT* pfInc);
};

class MyAudioSessionManager2 : public IAudioSessionManager2 {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyDevice* device;
    vector<IAudioSessionNotification*> sessionNotifications;
public:
    MyAudioSessionManager2(MyDevice* d);
    ~MyAudioSessionManager2();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetAudioSessionControl(const GUID* AudioSessionGuid, DWORD StreamFlags, IAudioSessionControl** SessionControl);
    virtual HRESULT __stdcall GetSimpleAudioVolume(const GUID* AudioSessionGuid, DWORD StreamFlags, ISimpleAudioVolume** AudioVolume);
    virtual HRESULT __stdcall GetSessionEnumerator(IAudioSessionEnumerator** SessionEnumerator);
    virtual HRESULT __stdcall RegisterSessionNotification(IAudioSessionNotification* SessionNotification);
    virtual HRESULT __stdcall UnregisterSessionNotification(IAudioSessionNotification* SessionNotification);
    virtual HRESULT __stdcall RegisterDuckNotification(LPCWSTR SessionID, IAudioVolumeDuckNotification* duckNotification);
    virtual HRESULT __stdcall UnregisterDuckNotification(IAudioVolumeDuckNotification* duckNotification);
};

class MyAudioSessionEnumerator : public IAudioSessionEnumerator {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    vector<IAudioSessionControl*> controls;
public:
    MyAudioSessionEnumerator(const vector<MyAudioSession*>& sessions);
    ~MyAudioSessionEnumerator();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetCount(int* SessionCount);
    virtual HRESULT __stdcall GetSession(int SessionNum, IAudioSessionControl** Session);
};

class MyAudioSessionControl : public IAudioSessionControl2 {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyAudioSession* session;
public:
    MyAudioSessionControl(MyAudioSession* s);
    ~MyAudioSessionControl();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall GetState(AudioSessionState* pRetVal);
    virtual HRESULT __stdcall GetDisplayName(LPWSTR* pRetVal);
    virtual HRESULT __stdcall SetDisplayName(LPCWSTR Value, const GUID* EventContext);
    virtual HRESULT __stdcall GetIconPath(LPWSTR* pRetVal);
    virtual HRESULT __stdcall SetIconPath(LPCWSTR Value, const GUID* EventContext);
    virtual HRESULT __stdcall GetGroupingParam(GUID* pRetVal);
    virtual HRESULT __stdcall SetGroupingParam(const GUID* Override, const GUID* EventContext);
    virtual HRESULT __stdcall RegisterAudioSessionNotification(IAudioSessionEvents* NewNotifications);
    virtual HRESULT __stdcall UnregisterAudioSessionNotification(IAudioSessionEvents* NewNotifications);
    virtual HRESULT __stdcall GetSessionIdentifier(LPWSTR* pRetVal);
    virtual HRESULT __stdcall GetSessionInstanceIdentifier(LPWSTR* pRetVal);
    virtual HRESULT __stdcall GetProcessId(DWORD* pRetVal);
    virtual HRESULT __stdcall IsSystemSoundsSession();
    virtual HRESULT __stdcall SetDuckingPreference(BOOL optOut);
};

class MySimpleAudioVolume : public ISimpleAudioVolume {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyAudioSession* session;
public:
    MySimpleAudioVolume(MyAudioSession* s);
    ~MySimpleAudioVolume();
    virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
    virtual HRESULT __stdcall SetMasterVolume(float fLevel, const GUID* EventContext);
    virtual HRESULT __stdcall GetMasterVolume(float* pfLevel);
    virtual HRESULT __stdcall SetMute(BOOL bMute, const GUID* EventContext);
    virtual HRESULT __stdcall GetMute(BOOL* pbMute);
};

class MyAudioStreamVolume : public IAudioStreamVolume {
private:
    LONG ref;
    IUnknown* m_pUnkMarshal;
    MyAudioClient* parent;
public:
    MyAudioStreamVolume(MyAudioClient* p);
    ~MyAudioStreamVolume();

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IAudioStreamVolume
    virtual HRESULT STDMETHODCALLTYPE GetChannelCount(UINT32* pdwCount);
    virtual HRESULT STDMETHODCALLTYPE SetChannelVolume(UINT32 dwIndex, const float fLevel);
    virtual HRESULT STDMETHODCALLTYPE GetChannelVolume(UINT32 dwIndex, float* pfLevel);
    virtual HRESULT STDMETHODCALLTYPE SetAllVolumes(UINT32 dwCount, const float* pfVolumes);
    virtual HRESULT STDMETHODCALLTYPE GetAllVolumes(UINT32 dwCount, float* pfVolumes);
};

#endif

