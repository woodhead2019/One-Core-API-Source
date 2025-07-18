@ stdcall CLSIDFromProgID(wstr ptr) ole32.CLSIDFromProgID
@ stdcall CLSIDFromString(wstr ptr) ole32.CLSIDFromString
@ stdcall CoAddRefServerProcess() ole32.CoAddRefServerProcess
@ stub CoAllowUnmarshalerCLSID ;(long) ole.CoAllowUnmarshalerCLSID
@ stdcall CoCancelCall(long long) ole32.CoCancelCall
@ stdcall CoCopyProxy(ptr ptr) ole32.CoCopyProxy
@ stdcall CoCreateFreeThreadedMarshaler(ptr ptr) ole32.CoCreateFreeThreadedMarshaler
@ stdcall CoCreateGuid(ptr) ole32.CoCreateGuid
@ stdcall CoCreateInstance(ptr ptr long ptr ptr) ole32.CoCreateInstance
@ stdcall CoCreateInstanceEx(ptr ptr long ptr long ptr) ole32.CoCreateInstanceEx
@ stdcall CoCreateInstanceFromApp(ptr ptr long ptr long ptr) combase.CoCreateInstanceFromApp
@ stdcall CoDecodeProxy(long int64 ptr) combase.CoDecodeProxy
@ stdcall CoDecrementMTAUsage(ptr) combase.CoDecrementMTAUsage
@ stdcall CoDisableCallCancellation(ptr) ole32.CoDisableCallCancellation
@ stdcall CoDisconnectContext(long) ole32.CoDisconnectContext
@ stdcall CoDisconnectObject(ptr long) ole32.CoDisconnectObject
@ stdcall CoEnableCallCancellation(ptr) ole32.CoEnableCallCancellation
@ stdcall CoFreeUnusedLibraries() ole32.CoFreeUnusedLibraries
@ stdcall CoFreeUnusedLibrariesEx(long long) ole32.CoFreeUnusedLibrariesEx
@ stdcall CoGetApartmentType(ptr ptr) combase.CoGetApartmentType
@ stdcall CoGetCallContext(ptr ptr) ole32.CoGetCallContext
@ stdcall CoGetCallerTID(ptr) ole32.CoGetCallerTID
@ stdcall CoGetCancelObject(long ptr ptr) ole32.CoGetCancelObject
@ stdcall CoGetClassObject(ptr long ptr ptr ptr) ole32.CoGetClassObject
@ stdcall CoGetContextToken(ptr) ole32.CoGetContextToken
@ stdcall CoGetCurrentLogicalThreadId(ptr) ole32.CoGetCurrentLogicalThreadId
@ stdcall CoGetCurrentProcess() ole32.CoGetCurrentProcess
@ stdcall CoGetDefaultContext(long ptr ptr) ole32.CoGetDefaultContext
@ stdcall CoGetInterfaceAndReleaseStream(ptr ptr ptr) ole32.CoGetInterfaceAndReleaseStream
@ stdcall CoGetMalloc(long ptr) ole32.CoGetMalloc
@ stdcall CoGetMarshalSizeMax(ptr ptr ptr long ptr long) ole32.CoGetMarshalSizeMax
@ stdcall CoGetObjectContext(ptr ptr) ole32.CoGetObjectContext
@ stdcall CoGetPSClsid(ptr ptr) ole32.CoGetPSClsid
@ stdcall CoGetStandardMarshal(ptr ptr long ptr long ptr) ole32.CoGetStandardMarshal
@ stdcall CoGetStdMarshalEx(ptr long ptr) ole32.CoGetStdMarshalEx
@ stdcall CoGetTreatAsClass(ptr ptr) ole32.CoGetTreatAsClass
@ stdcall CoImpersonateClient() ole32.CoImpersonateClient
@ stdcall CoIncrementMTAUsage(ptr) combase.CoIncrementMTAUsage
@ stdcall CoInitializeEx(ptr long) ole32.CoInitializeEx
@ stdcall CoInitializeSecurity(ptr long ptr ptr long long ptr long ptr) ole32.CoInitializeSecurity
@ stdcall CoInvalidateRemoteMachineBindings(str) ole32.CoInvalidateRemoteMachineBindings
@ stdcall CoIsHandlerConnected(ptr) ole32.CoIsHandlerConnected
@ stdcall CoLockObjectExternal(ptr long long) ole32.CoLockObjectExternal
@ stdcall CoMarshalHresult(ptr long) ole32.CoMarshalHresult
@ stdcall CoMarshalInterThreadInterfaceInStream(ptr ptr ptr) ole32.CoMarshalInterThreadInterfaceInStream
@ stdcall CoMarshalInterface(ptr ptr ptr long ptr long) ole32.CoMarshalInterface
@ stdcall CoQueryAuthenticationServices(ptr ptr) ole.CoQueryAuthenticationServices
@ stdcall CoQueryClientBlanket(ptr ptr ptr ptr ptr ptr ptr) ole32.CoQueryClientBlanket
@ stdcall CoQueryProxyBlanket(ptr ptr ptr ptr ptr ptr ptr ptr) ole32.CoQueryProxyBlanket
@ stdcall CoRegisterClassObject(ptr ptr long long ptr) ole32.CoRegisterClassObject
@ stdcall CoRegisterPSClsid(ptr ptr) ole32.CoRegisterPSClsid
@ stdcall CoRegisterSurrogate(ptr) ole32.CoRegisterSurrogate
@ stdcall CoReleaseMarshalData(ptr) ole32.CoReleaseMarshalData
@ stdcall CoReleaseServerProcess() ole32.CoReleaseServerProcess
@ stdcall CoResumeClassObjects() ole32.CoResumeClassObjects
@ stdcall CoRevertToSelf() ole32.CoRevertToSelf
@ stdcall CoRevokeClassObject(long) ole32.CoRevokeClassObject
@ stdcall CoSetCancelObject(ptr) ole32.CoSetCancelObject
@ stdcall CoSetProxyBlanket(ptr long long ptr long long ptr long) ole32.CoSetProxyBlanket
@ stdcall CoSuspendClassObjects() ole32.CoSuspendClassObjects
@ stdcall CoSwitchCallContext(ptr ptr) ole32.CoSwitchCallContext
@ stdcall CoTaskMemAlloc(long) ole32.CoTaskMemAlloc
@ stdcall CoTaskMemFree(ptr) ole32.CoTaskMemFree
@ stdcall CoTaskMemRealloc(ptr long) ole32.CoTaskMemRealloc
@ stdcall CoTestCancel() ole32.CoTestCancel
@ stdcall CoUninitialize() ole32.CoUninitialize
@ stdcall CoUnmarshalHresult(ptr ptr) ole32.CoUnmarshalHresult
@ stdcall CoUnmarshalInterface(ptr ptr ptr) ole32.CoUnmarshalInterface
@ stdcall CoWaitForMultipleHandles(long long long ptr ptr) ole32.CoWaitForMultipleHandles
@ stub CoWaitForMultipleObjects ;(long long long ptr ptr) combase.CoWaitForMultipleObjects
@ stdcall CreateStreamOnHGlobal(ptr long ptr) ole32.CreateStreamOnHGlobal
@ stdcall FreePropVariantArray(long ptr) ole32.FreePropVariantArray
@ stdcall GetHGlobalFromStream(ptr ptr) ole32.GetHGlobalFromStream
@ stdcall IIDFromString(wstr ptr) ole32.IIDFromString
@ stdcall ProgIDFromCLSID(ptr ptr) ole32.ProgIDFromCLSID
@ stdcall PropVariantClear(ptr) ole32.PropVariantClear
@ stdcall PropVariantCopy(ptr ptr) ole32.PropVariantCopy
@ stdcall StringFromCLSID(ptr ptr) ole32.StringFromCLSID
@ stdcall StringFromGUID2(ptr ptr long) ole32.StringFromGUID2
@ stdcall StringFromIID(ptr ptr) ole32.StringFromIID
