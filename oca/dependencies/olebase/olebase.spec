@ stub CoVrfCheckThreadState
@ stub CoVrfGetThreadState
@ stub CoVrfReleaseThreadState
@ stub PropVariantChangeType
@ stdcall BindMoniker(ptr long ptr ptr)
@ stdcall CLIPFORMAT_UserFree(ptr ptr)
@ stdcall CLIPFORMAT_UserMarshal(ptr ptr ptr)
@ stdcall CLIPFORMAT_UserSize(ptr long ptr)
@ stdcall CLIPFORMAT_UserUnmarshal(ptr ptr ptr)
@ stub CLSIDFromOle1Class
@ stdcall CLSIDFromProgID(wstr ptr)
@ stdcall CLSIDFromProgIDEx(wstr ptr)
@ stdcall CLSIDFromString(wstr ptr)
@ stdcall CoAddRefServerProcess()
@ stdcall CoAllowSetForegroundWindow(ptr ptr)
@ stdcall CoBuildVersion()
@ stub CoCancelCall
@ stdcall CoCopyProxy(ptr ptr)
@ stdcall CoCreateFreeThreadedMarshaler(ptr ptr)
@ stdcall CoCreateGuid(ptr)
@ stdcall CoCreateInstance(ptr ptr long ptr ptr)
@ stdcall CoCreateInstanceEx(ptr ptr long ptr long ptr)
@ stub CoCreateObjectInContext
@ stub CoDeactivateObject
@ stub CoDisableCallCancellation
@ stdcall CoDisconnectObject(ptr long)
@ stdcall CoDosDateTimeToFileTime(long long ptr) kernel32.DosDateTimeToFileTime
@ stub CoEnableCallCancellation
@ stdcall CoFileTimeNow(ptr)
@ stdcall CoFileTimeToDosDateTime(ptr ptr ptr) kernel32.FileTimeToDosDateTime
@ stdcall CoFreeAllLibraries()
@ stdcall CoFreeLibrary(long)
@ stdcall CoFreeUnusedLibraries()
@ stdcall CoFreeUnusedLibrariesEx(long long)
@ stub CoGetApartmentID
@ stdcall CoGetCallContext(ptr ptr)
@ stdcall CoGetCallerTID(ptr)
@ stub CoGetCancelObject
@ stdcall CoGetClassObject(ptr long ptr ptr ptr)
@ stub CoGetClassVersion
@ stub CoGetComCatalog
@ stdcall CoGetContextToken(ptr)
@ stdcall CoGetCurrentLogicalThreadId(ptr)
@ stdcall CoGetCurrentProcess()
@ stdcall CoGetDefaultContext(long ptr ptr)
@ stdcall CoGetInstanceFromFile(ptr ptr ptr long long wstr long ptr)
@ stdcall CoGetInstanceFromIStorage(ptr ptr ptr long ptr long ptr)
@ stub CoGetInterceptor
@ stub CoGetInterceptorFromTypeInfo
@ stdcall CoGetInterfaceAndReleaseStream(ptr ptr ptr)
@ stdcall CoGetMalloc(long ptr)
@ stdcall CoGetMarshalSizeMax(ptr ptr ptr long ptr long)
@ stub CoGetModuleType
@ stdcall CoGetObject(wstr ptr ptr ptr)
@ stdcall CoGetObjectContext(ptr ptr)
@ stdcall CoGetPSClsid(ptr ptr)
@ stub CoGetProcessIdentifier
@ stdcall CoGetStandardMarshal(ptr ptr long ptr long ptr)
@ stdcall CoGetState(ptr)
@ stdcall -stub CoGetStdMarshalEx(ptr long ptr)
@ stub CoGetSystemSecurityPermissions
@ stdcall CoGetTreatAsClass(ptr ptr)
@ stdcall CoImpersonateClient()
@ stdcall CoInitialize(ptr)
@ stdcall CoInitializeEx(ptr long)
@ stdcall CoInitializeSecurity(ptr long ptr ptr long long ptr long ptr)
@ stdcall CoInitializeWOW(long long)
@ stub CoInstall
@ stub CoInvalidateRemoteMachineBindings
@ stdcall CoIsHandlerConnected(ptr)
@ stdcall CoIsOle1Class (ptr)
@ stdcall CoLoadLibrary(wstr long)
@ stdcall CoLockObjectExternal(ptr long long)
@ stdcall CoMarshalHresult(ptr long)
@ stdcall CoMarshalInterThreadInterfaceInStream(ptr ptr ptr)
@ stdcall CoMarshalInterface(ptr ptr ptr long ptr long)
@ stub CoPopServiceDomain
@ stub CoPushServiceDomain
@ stub CoQueryAuthenticationServices
@ stdcall CoQueryClientBlanket(ptr ptr ptr ptr ptr ptr ptr)
@ stdcall CoQueryProxyBlanket(ptr ptr ptr ptr ptr ptr ptr ptr)
@ stub CoQueryReleaseObject
@ stub CoReactivateObject
@ stdcall CoRegisterChannelHook(ptr ptr)
@ stdcall CoRegisterClassObject(ptr ptr long long ptr)
@ stdcall CoRegisterInitializeSpy(ptr ptr)
@ stdcall CoRegisterMallocSpy (ptr)
@ stdcall CoRegisterMessageFilter(ptr ptr)
@ stdcall CoRegisterPSClsid(ptr ptr)
@ stub CoRegisterSurrogate
@ stub CoRegisterSurrogateEx
@ stdcall CoReleaseMarshalData(ptr)
@ stdcall CoReleaseServerProcess()
@ stdcall CoResumeClassObjects()
@ stub CoRetireServer
@ stdcall CoRevertToSelf()
@ stdcall CoRevokeClassObject(long)
@ stdcall CoRevokeInitializeSpy(int64)
@ stdcall CoRevokeMallocSpy()
@ stub CoSetCancelObject
@ stdcall CoSetProxyBlanket(ptr long long ptr long long ptr long)
@ stdcall CoSetState(ptr)
@ stdcall CoSuspendClassObjects()
@ stdcall CoSwitchCallContext(ptr ptr)
@ stdcall CoTaskMemAlloc(long)
@ stdcall CoTaskMemFree(ptr)
@ stdcall CoTaskMemRealloc(ptr long)
@ stub CoTestCancel
@ stdcall CoTreatAsClass(ptr ptr)
@ stdcall CoUninitialize()
@ stub CoUnloadingWOW
@ stdcall CoUnmarshalHresult(ptr ptr)
@ stdcall CoUnmarshalInterface(ptr ptr ptr)
@ stdcall CoWaitForMultipleHandles(long long long ptr ptr)
@ stub ComPs_NdrDllCanUnloadNow
@ stub ComPs_NdrDllGetClassObject
@ stub ComPs_NdrDllRegisterProxy
@ stub ComPs_NdrDllUnregisterProxy
@ stdcall CreateAntiMoniker(ptr)
@ stdcall CreateBindCtx(long ptr)
@ stdcall CreateClassMoniker(ptr ptr)
@ stdcall CreateDataAdviseHolder(ptr)
@ stdcall CreateDataCache(ptr ptr ptr ptr)
@ stdcall CreateErrorInfo(ptr)
@ stdcall CreateFileMoniker(wstr ptr)
@ stdcall CreateGenericComposite(ptr ptr ptr)
@ stdcall CreateILockBytesOnHGlobal(ptr long ptr)
@ stdcall CreateItemMoniker(wstr wstr ptr)
@ stub CreateObjrefMoniker
@ stdcall CreateOleAdviseHolder(ptr)
@ stdcall CreatePointerMoniker(ptr ptr)
@ stub CreateStdProgressIndicator
@ stdcall CreateStreamOnHGlobal(ptr long ptr)
@ stub DcomChannelSetHResult
@ stdcall DllDebugObjectRPCHook(long ptr)
@ stdcall DllGetClassObject(ptr ptr ptr)
@ stub DllGetClassObjectWOW
@ stdcall DllRegisterServer()
@ stdcall DoDragDrop(ptr ptr long ptr)
@ stub EnableHookObject
@ stdcall FmtIdToPropStgName(ptr wstr)
@ stdcall FreePropVariantArray(long ptr)
@ stdcall GetClassFile(wstr ptr)
@ stdcall GetConvertStg(ptr)
@ stub GetDocumentBitStg
@ stdcall GetErrorInfo(long ptr)
@ stdcall GetHGlobalFromILockBytes(ptr ptr)
@ stdcall GetHGlobalFromStream(ptr ptr)
@ stub GetHookInterface
@ stdcall GetRunningObjectTable(long ptr)
@ stdcall HACCEL_UserFree(ptr ptr)
@ stdcall HACCEL_UserMarshal(ptr ptr ptr)
@ stdcall HACCEL_UserSize(ptr long ptr)
@ stdcall HACCEL_UserUnmarshal(ptr ptr ptr)
@ stdcall HBITMAP_UserFree(ptr ptr)
@ stdcall HBITMAP_UserMarshal(ptr ptr ptr)
@ stdcall HBITMAP_UserSize(ptr long ptr)
@ stdcall HBITMAP_UserUnmarshal(ptr ptr ptr)
@ stub HBRUSH_UserFree
@ stub HBRUSH_UserMarshal
@ stub HBRUSH_UserSize
@ stub HBRUSH_UserUnmarshal
@ stdcall HDC_UserFree(ptr ptr)
@ stdcall HDC_UserMarshal(ptr ptr ptr)
@ stdcall HDC_UserSize(ptr long ptr)
@ stdcall HDC_UserUnmarshal(ptr ptr ptr)
@ stdcall HENHMETAFILE_UserFree(ptr ptr)
@ stdcall HENHMETAFILE_UserMarshal(ptr ptr ptr)
@ stdcall HENHMETAFILE_UserSize(ptr long ptr)
@ stdcall HENHMETAFILE_UserUnmarshal(ptr ptr ptr)
@ stdcall HGLOBAL_UserFree(ptr ptr)
@ stdcall HGLOBAL_UserMarshal(ptr ptr ptr)
@ stdcall HGLOBAL_UserSize(ptr long ptr)
@ stdcall HGLOBAL_UserUnmarshal(ptr ptr ptr)
@ stdcall HICON_UserFree(ptr ptr)
@ stdcall HICON_UserMarshal(ptr ptr ptr)
@ stdcall HICON_UserSize(ptr long ptr)
@ stdcall HICON_UserUnmarshal(ptr ptr ptr)
@ stdcall HMENU_UserFree(ptr ptr)
@ stdcall HMENU_UserMarshal(ptr ptr ptr)
@ stdcall HMENU_UserSize(ptr long ptr)
@ stdcall HMENU_UserUnmarshal(ptr ptr ptr)
@ stdcall HMETAFILEPICT_UserFree(ptr ptr)
@ stdcall HMETAFILEPICT_UserMarshal(ptr ptr ptr)
@ stdcall HMETAFILEPICT_UserSize(ptr long ptr)
@ stdcall HMETAFILEPICT_UserUnmarshal(ptr ptr ptr)
@ stdcall HMETAFILE_UserFree(ptr ptr)
@ stdcall HMETAFILE_UserMarshal(ptr ptr ptr)
@ stdcall HMETAFILE_UserSize(ptr long ptr)
@ stdcall HMETAFILE_UserUnmarshal(ptr ptr ptr)
@ stdcall HPALETTE_UserFree(ptr ptr)
@ stdcall HPALETTE_UserMarshal(ptr ptr ptr)
@ stdcall HPALETTE_UserSize(ptr long ptr)
@ stdcall HPALETTE_UserUnmarshal(ptr ptr ptr)
@ stdcall HWND_UserFree(ptr ptr)
@ stdcall HWND_UserMarshal(ptr ptr ptr)
@ stdcall HWND_UserSize(ptr long ptr)
@ stdcall HWND_UserUnmarshal(ptr ptr ptr)
@ stub HkOleRegisterObject
@ stdcall IIDFromString(wstr ptr)
@ stdcall IsAccelerator(long long ptr long)
@ stdcall IsEqualGUID(ptr ptr)
@ stub IsValidIid
@ stdcall IsValidInterface(ptr)
@ stub IsValidPtrIn
@ stub IsValidPtrOut
@ stdcall MkParseDisplayName(ptr ptr ptr ptr)
@ stdcall MonikerCommonPrefixWith(ptr ptr ptr)
@ stub MonikerRelativePathTo
@ stdcall OleBuildVersion()
@ stdcall OleConvertIStorageToOLESTREAM(ptr ptr)
@ stub OleConvertIStorageToOLESTREAMEx
@ stdcall OleConvertOLESTREAMToIStorage(ptr ptr ptr)
@ stub OleConvertOLESTREAMToIStorageEx
@ stdcall OleCreate(ptr ptr long ptr ptr ptr ptr)
@ stdcall OleCreateDefaultHandler(ptr ptr ptr ptr)
@ stdcall OleCreateEmbeddingHelper(ptr ptr long ptr ptr ptr)
@ stub OleCreateEx
@ stdcall OleCreateFromData(ptr ptr long ptr ptr ptr ptr)
@ stdcall OleCreateFromDataEx(ptr ptr long long long ptr ptr ptr ptr ptr ptr ptr)
@ stdcall OleCreateFromFile(ptr ptr ptr long ptr ptr ptr ptr)
@ stub OleCreateFromFileEx
@ stdcall OleCreateLink(ptr ptr long ptr ptr ptr ptr)
@ stub OleCreateLinkEx
@ stdcall OleCreateLinkFromData(ptr ptr long ptr ptr ptr ptr)
@ stub OleCreateLinkFromDataEx
@ stdcall OleCreateLinkToFile(ptr ptr long ptr ptr ptr ptr)
@ stub OleCreateLinkToFileEx
@ stdcall OleCreateMenuDescriptor(long ptr)
@ stdcall OleCreateStaticFromData(ptr ptr long ptr ptr ptr ptr)
@ stdcall OleDestroyMenuDescriptor(long)
@ stdcall OleDoAutoConvert(ptr ptr)
@ stdcall OleDraw(ptr long long ptr)
@ stdcall OleDuplicateData(long long long)
@ stdcall OleFlushClipboard()
@ stdcall OleGetAutoConvert(ptr ptr)
@ stdcall OleGetClipboard(ptr)
@ stdcall OleGetIconOfClass(ptr ptr long)
@ stub OleGetIconOfFile
@ stdcall OleInitialize(ptr)
@ stdcall OleInitializeWOW(long long)
@ stdcall OleIsCurrentClipboard(ptr)
@ stdcall OleIsRunning(ptr)
@ stdcall OleLoad(ptr ptr ptr ptr)
@ stdcall OleLoadFromStream(ptr ptr ptr)
@ stdcall OleLockRunning(ptr long long)
@ stdcall OleMetafilePictFromIconAndLabel(long ptr ptr long)
@ stdcall OleNoteObjectVisible(ptr long)
@ stdcall OleQueryCreateFromData(ptr)
@ stdcall OleQueryLinkFromData(ptr)
@ stdcall OleRegEnumFormatEtc(ptr long ptr)
@ stdcall OleRegEnumVerbs(long ptr)
@ stdcall OleRegGetMiscStatus(ptr long ptr)
@ stdcall OleRegGetUserType(long long ptr)
@ stdcall OleRun(ptr)
@ stdcall OleSave(ptr ptr long)
@ stdcall OleSaveToStream(ptr ptr)
@ stdcall OleSetAutoConvert(ptr ptr)
@ stdcall OleSetClipboard(ptr)
@ stdcall OleSetContainedObject(ptr long)
@ stdcall OleSetMenuDescriptor(long long long ptr ptr)
@ stdcall OleTranslateAccelerator(ptr ptr ptr)
@ stdcall OleUninitialize()
@ stub OpenOrCreateStream
@ stdcall ProgIDFromCLSID(ptr ptr)
@ stdcall PropStgNameToFmtId(wstr ptr)
@ stdcall PropSysAllocString(wstr)
@ stdcall PropSysFreeString(wstr)
@ stdcall PropVariantClear(ptr)
@ stdcall PropVariantCopy(ptr ptr)
@ stdcall ReadClassStg(ptr ptr)
@ stdcall ReadClassStm(ptr ptr)
@ stdcall ReadFmtUserTypeStg(ptr ptr ptr)
@ stub ReadOleStg
@ stub ReadStringStream
@ stdcall RegisterDragDrop(long ptr)
@ stdcall ReleaseStgMedium(ptr)
@ stdcall RevokeDragDrop(long)
@ stdcall SNB_UserFree(ptr ptr)
@ stdcall SNB_UserMarshal(ptr ptr ptr)
@ stdcall SNB_UserSize(ptr long ptr)
@ stdcall SNB_UserUnmarshal(ptr ptr ptr)
@ stdcall STGMEDIUM_UserFree(ptr ptr)
@ stdcall STGMEDIUM_UserMarshal(ptr ptr ptr)
@ stdcall STGMEDIUM_UserSize(ptr long ptr)
@ stdcall STGMEDIUM_UserUnmarshal(ptr ptr ptr)
@ stdcall SetConvertStg(ptr long)
@ stub SetDocumentBitStg
@ stdcall SetErrorInfo(long ptr)
@ stdcall StgConvertPropertyToVariant(ptr long ptr ptr)
@ stdcall StgConvertVariantToProperty(ptr long ptr ptr long long ptr)
@ stdcall StgCreateDocfile(wstr long long ptr)
@ stdcall StgCreateDocfileOnILockBytes(ptr long long ptr)
@ stdcall StgCreatePropSetStg(ptr long ptr)
@ stub StgCreatePropStg
@ stdcall StgCreateStorageEx(wstr long long long ptr ptr ptr ptr)
@ stub StgGetIFillLockBytesOnFile
@ stub StgGetIFillLockBytesOnILockBytes
@ stdcall StgIsStorageFile(wstr)
@ stdcall StgIsStorageILockBytes(ptr)
@ stub StgOpenAsyncDocfileOnIFillLockBytes
@ stub StgOpenPropStg
@ stdcall StgOpenStorage(wstr ptr long ptr long ptr)
@ stdcall StgOpenStorageEx(wstr long long long ptr ptr ptr ptr)
@ stub StgOpenStorageOnHandle
@ stdcall StgOpenStorageOnILockBytes(ptr ptr long long long ptr)
@ stub StgPropertyLengthAsVariant
@ stdcall StgSetTimes(wstr ptr ptr ptr )
@ stdcall StringFromCLSID(ptr ptr)
@ stdcall StringFromGUID2(ptr ptr long)
@ stdcall StringFromIID(ptr ptr) StringFromCLSID
@ stub UpdateDCOMSettings
@ stub UtConvertDvtd16toDvtd32
@ stub UtConvertDvtd32toDvtd16
@ stub UtGetDvtd16Info
@ stub UtGetDvtd32Info
@ stdcall WdtpInterfacePointer_UserFree(ptr)
@ stdcall WdtpInterfacePointer_UserMarshal(ptr long ptr ptr ptr)
@ stdcall WdtpInterfacePointer_UserSize(ptr long ptr long ptr)
@ stdcall WdtpInterfacePointer_UserUnmarshal(ptr ptr ptr ptr)
@ stdcall WriteClassStg(ptr ptr)
@ stdcall WriteClassStm(ptr ptr)
@ stdcall WriteFmtUserTypeStg(ptr long ptr)
@ stub WriteOleStg
@ stub WriteStringStream
@ stub UpdateProcessTracing

@ stub ComPs_CStdStubBuffer_AddRef 	
@ stub ComPs_CStdStubBuffer_Connect 			
@ stub ComPs_CStdStubBuffer_CountRefs 			
@ stub ComPs_CStdStubBuffer_DebugServerQueryInterface 			
@ stub ComPs_CStdStubBuffer_DebugServerRelease 			
@ stub ComPs_CStdStubBuffer_Disconnect 			
@ stub ComPs_CStdStubBuffer_Invoke 			
@ stub ComPs_CStdStubBuffer_IsIIDSupported 			
@ stub ComPs_CStdStubBuffer_QueryInterface 			
@ stub ComPs_IUnknown_AddRef_Proxy 			
@ stub ComPs_IUnknown_QueryInterface_Proxy 			
@ stub ComPs_IUnknown_Release_Proxy 			
@ stub ComPs_NdrClientCall2 			
@ stub ComPs_NdrClientCall2_va 			
@ stub ComPs_NdrCStdStubBuffer2_Release 			
@ stub ComPs_NdrCStdStubBuffer_Release 
@ stub ComPs_NdrStubCall2
@ stub ComPs_NdrStubForwardingFunction
@ stub CoRegisterActivationFilter

#for XP x64
@ stdcall -stub -arch=x86_64 CLIPFORMAT_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 CLIPFORMAT_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 CLIPFORMAT_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 CLIPFORMAT_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HACCEL_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HACCEL_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HACCEL_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HACCEL_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HBITMAP_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HBITMAP_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HBITMAP_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HBITMAP_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HBRUSH_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HBRUSH_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HBRUSH_UserSize64(ptr ptr)
@ stdcall -stub -arch=x86_64 HBRUSH_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HDC_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HDC_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HDC_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HDC_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HENHMETAFILE_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HENHMETAFILE_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HENHMETAFILE_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HENHMETAFILE_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HGLOBAL_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HGLOBAL_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HGLOBAL_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HGLOBAL_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HICON_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HICON_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HICON_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HICON_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HMENU_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HMENU_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HMENU_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HMENU_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HMETAFILE_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HMETAFILE_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HMETAFILE_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HMETAFILE_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HMETAFILEPICT_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HMETAFILEPICT_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HMETAFILEPICT_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HMETAFILEPICT_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HPALETTE_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HPALETTE_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HPALETTE_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HPALETTE_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HWND_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 HWND_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 HWND_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 HWND_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 SNB_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 SNB_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 SNB_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 SNB_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 STGMEDIUM_UserFree64(ptr ptr)
@ stdcall -stub -arch=x86_64 STGMEDIUM_UserMarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 STGMEDIUM_UserSize64(ptr long ptr)
@ stdcall -stub -arch=x86_64 STGMEDIUM_UserUnmarshal64(ptr ptr ptr)
@ stdcall -stub -arch=x86_64 WdtpInterfacePointer_UserFree64(ptr)
@ stdcall -stub -arch=x86_64 WdtpInterfacePointer_UserMarshal64(ptr long ptr ptr ptr)
@ stdcall -stub -arch=x86_64 WdtpInterfacePointer_UserSize64(ptr long ptr long ptr)
@ stdcall -stub -arch=x86_64 WdtpInterfacePointer_UserUnmarshal64(ptr ptr ptr ptr)

#For Vista Compatibility
@ stdcall -stub HRGN_UserFree(ptr ptr)
@ stdcall -stub HRGN_UserMarshal(ptr ptr ptr)
@ stdcall -stub HRGN_UserSize(ptr long ptr)
@ stdcall -stub HRGN_UserUnmarshal(ptr ptr ptr)