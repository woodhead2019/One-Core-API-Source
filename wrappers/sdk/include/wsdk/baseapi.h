/*
 * Copyright 2009 Henri Verbeet for CodeWeavers
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
 *
 */

#include "config.h"
#include <assert.h>
#include <evntprov.h>
#include <winnls.h>
#include <Ntsecapi.h>
#include <evntprov.h>
#include <ntdef.h>

#define MAX_HW_COUNTERS 16

#define RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO    0x01000000
#define RTL_CRITICAL_SECTION_FLAG_DYNAMIC_SPIN     0x02000000
#define RTL_CRITICAL_SECTION_FLAG_STATIC_INIT      0x04000000
#define RTL_CRITICAL_SECTION_FLAG_RESOURCE_TYPE    0x08000000
#define RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO 0x10000000
#define RTL_CRITICAL_SECTION_ALL_FLAG_BITS         0xFF000000
#define RTL_CRITICAL_SECTION_FLAG_RESERVED \
    (RTL_CRITICAL_SECTION_ALL_FLAG_BITS & \
    (~(RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO | \
       RTL_CRITICAL_SECTION_FLAG_DYNAMIC_SPIN | \
       RTL_CRITICAL_SECTION_FLAG_STATIC_INIT | \
       RTL_CRITICAL_SECTION_FLAG_RESOURCE_TYPE | \
       RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO)))

typedef ULONG LOGICAL;

typedef DWORD TP_VERSION, *PTP_VERSION;

typedef DWORD TP_WAIT_RESULT;

typedef PVOID* PALPC_HANDLE;

typedef PVOID ALPC_HANDLE;

typedef HANDLE 	DLL_DIRECTORY_COOKIE;

typedef struct _TP_TIMER TP_TIMER, *PTP_TIMER;

typedef struct _TP_CLEANUP_GROUP TP_CLEANUP_GROUP, *PTP_CLEANUP_GROUP;

typedef struct _TP_CALLBACK_INSTANCE TP_CALLBACK_INSTANCE, *PTP_CALLBACK_INSTANCE;

typedef VOID (*PTP_TIMER_CALLBACK)(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER Timer);

typedef struct _TP_WAIT TP_WAIT, *PTP_WAIT;

typedef TP_CALLBACK_ENVIRON_V1 TP_CALLBACK_ENVIRON, *PTP_CALLBACK_ENVIRON;

typedef VOID (*PTP_WAIT_CALLBACK)(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult);

typedef VOID (WINAPI *PBAD_MEMORY_CALLBACK_ROUTINE)(VOID);

typedef enum _NORM_FORM { 
  NormalizationOther  = 0,
  NormalizationC      = 0x1,
  NormalizationD      = 0x2,
  NormalizationKC     = 0x5,
  NormalizationKD     = 0x6
} NORM_FORM;

typedef BOOLEAN (*PSECURE_MEMORY_CACHE_CALLBACK)(
    _In_  PVOID Addr,
    _In_  SIZE_T Range
);

typedef struct _TP_IO{
	void * 	dummy;
}TP_IO,*PTP_IO;

typedef VOID (CALLBACK *PTP_WIN32_IO_CALLBACK)(PTP_CALLBACK_INSTANCE,PVOID,PVOID,ULONG,ULONG_PTR,PTP_IO);

typedef PVOID (WINAPI *PDELAYLOAD_FAILURE_SYSTEM_ROUTINE)(LPCSTR, LPCSTR);

typedef struct _WIN32_MEMORY_RANGE_ENTRY {
  PVOID  VirtualAddress;
  SIZE_T NumberOfBytes;
} WIN32_MEMORY_RANGE_ENTRY, *PWIN32_MEMORY_RANGE_ENTRY;

typedef struct _CLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE {
  PVOID pValue;
  DWORD ValueLength;
} CLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE, *PCLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE;

typedef struct _CLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE {
  DWORD64 Version;
  PWSTR   Name;
} CLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE, *PCLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE;

typedef struct _CLAIM_SECURITY_ATTRIBUTE_V1 {
  PWSTR Name;
  WORD  ValueType;
  WORD  Reserved;
  DWORD Flags;
  DWORD ValueCount;
  union {
    PLONG64                                      pInt64;
    PDWORD64                                     pUint64;
    PWSTR                                        *ppString;
    PCLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE         pFqbn;
    PCLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE pOctetString;
  } Values;
} CLAIM_SECURITY_ATTRIBUTE_V1, *PCLAIM_SECURITY_ATTRIBUTE_V1;

typedef struct _CLAIM_SECURITY_ATTRIBUTES_INFORMATION {
  WORD  Version;
  WORD  Reserved;
  DWORD AttributeCount;
  union {
    PCLAIM_SECURITY_ATTRIBUTE_V1 pAttributeV1;
  } Attribute;
} CLAIM_SECURITY_ATTRIBUTES_INFORMATION, *PCLAIM_SECURITY_ATTRIBUTES_INFORMATION;

typedef struct _TP_POOL_STACK_INFORMATION
{
   SIZE_T StackReserve;
   SIZE_T StackCommit;
} TP_POOL_STACK_INFORMATION,*PTP_POOL_STACK_INFORMATION;

typedef enum  { 
  PMCCounter,
  MaxHardwareCounterType
} HARDWARE_COUNTER_TYPE;

typedef struct _HARDWARE_COUNTER_DATA {
  HARDWARE_COUNTER_TYPE Type;
  DWORD                 Reserved;
  DWORD64               Value;
} HARDWARE_COUNTER_DATA, *PHARDWARE_COUNTER_DATA;

typedef struct _PERFORMANCE_DATA {
  WORD                  Size;
  BYTE                  Version;
  BYTE                  HwCountersCount;
  DWORD                 ContextSwitchCount;
  DWORD64               WaitReasonBitMap;
  DWORD64               CycleTime;
  DWORD                 RetryCount;
  DWORD                 Reserved;
  HARDWARE_COUNTER_DATA HwCounters[MAX_HW_COUNTERS];
} PERFORMANCE_DATA, *PPERFORMANCE_DATA;

#define FLS_MAX_SLOT	128

typedef struct _FLS_DATA_INFO
{
	LIST_ENTRY FlsLink;
	PVOID FlsSlot[FLS_MAX_SLOT];
} FLS_DATA_INFO;

typedef struct _DELAYLOAD_PROC_DESCRIPTOR
{
     ULONG ImportDescribedByName;
     union {
         LPCSTR Name;
         ULONG Ordinal;
     } Description;
} DELAYLOAD_PROC_DESCRIPTOR, *PDELAYLOAD_PROC_DESCRIPTOR;

typedef struct _DELAYLOAD_INFO
{
     ULONG Size;
     PCIMAGE_DELAYLOAD_DESCRIPTOR DelayloadDescriptor;
     PIMAGE_THUNK_DATA ThunkAddress;
     LPCSTR TargetDllName;
     DELAYLOAD_PROC_DESCRIPTOR TargetApiDescriptor;
     PVOID TargetModuleBase;
     PVOID Unused;
     ULONG LastError;
} DELAYLOAD_INFO, *PDELAYLOAD_INFO;
typedef PVOID (WINAPI *PDELAYLOAD_FAILURE_DLL_CALLBACK)(ULONG, PDELAYLOAD_INFO);

typedef enum
{
    VmPrefetchInformation,
    VmPagePriorityInformation,
    VmCfgCallTargetInformation,
    VmPageDirtyStateInformation,
    VmImageHotPatchInformation,
    VmPhysicalContiguityInformation,
    VmVirtualMachinePrepopulateInformation,
    VmRemoveFromWorkingSetInformation,
} VIRTUAL_MEMORY_INFORMATION_CLASS, *PVIRTUAL_MEMORY_INFORMATION_CLASS;

typedef struct _CONTEXT_CHUNK
{
    LONG Offset;
    ULONG Length;
} CONTEXT_CHUNK, *PCONTEXT_CHUNK;

typedef struct _CONTEXT_EX
{
    CONTEXT_CHUNK All;
    CONTEXT_CHUNK Legacy;
    CONTEXT_CHUNK XState;
#ifdef _WIN64
    ULONG64 align;
#endif
} CONTEXT_EX, *PCONTEXT_EX;

typedef struct _MEMORY_RANGE_ENTRY
{
    PVOID  VirtualAddress;
    SIZE_T NumberOfBytes;
} MEMORY_RANGE_ENTRY, *PMEMORY_RANGE_ENTRY;

/* unimplemented*/
ULONG WINAPI EtwEventRegister(
  _In_      LPCGUID ProviderId,
  _In_opt_  PENABLECALLBACK EnableCallback,
  _In_opt_  PVOID CallbackContext,
  _Out_     PREGHANDLE RegHandle
);

/* unimplemented*/
ULONG WINAPI EtwEventUnregister(
  _In_  REGHANDLE RegHandle
);

/* unimplemented*/
ULONG WINAPI EtwEventWrite(
  _In_      REGHANDLE RegHandle,
  _In_      PCEVENT_DESCRIPTOR EventDescriptor,
  _In_      ULONG UserDataCount,
  _In_opt_  PEVENT_DATA_DESCRIPTOR UserData
);

ULONG WINAPI EtwEventWriteEndScenario(
    REGHANDLE RegHandle,
    PCEVENT_DESCRIPTOR EventDescriptor,
    ULONG UserDataCount,
    PEVENT_DATA_DESCRIPTOR UserData);


ULONG WINAPI EtwEventWriteStartScenario(
    REGHANDLE RegHandle,
    PCEVENT_DESCRIPTOR EventDescriptor,
    ULONG UserDataCount,
    PEVENT_DATA_DESCRIPTOR UserData);


/* unimplemented*/
BOOLEAN WINAPI EtwEventEnabled(
  IN REGHANDLE RegHandle,
  IN PCEVENT_DESCRIPTOR EventDescriptor);

BOOL WINAPI RtlGetFileMUIPath(
  _In_         DWORD dwFlags,
  _In_         PCWSTR pcwszFilePath,
  _Inout_opt_  PWSTR pwszLanguage,
  _Inout_      PULONG pcchLanguage,
  _Out_opt_    PWSTR pwszFileMUIPath,
  _Inout_      PULONG pcchFileMUIPath,
  _Inout_      PULONGLONG pululEnumerator
);

NTSTATUS WINAPI RtlGetUILanguageInfo(DWORD dwFlags, 
									 PCZZWSTR pwmszLanguage, 
									 PZZWSTR pwszFallbackLanguages,
									 PDWORD pcchFallbackLanguages,
									 PDWORD pdwAttributes);

NTSYSAPI NTSTATUS  WINAPI RtlGetSystemPreferredUILanguages(DWORD,ULONG,ULONG*,WCHAR*,ULONG*);

HANDLE WINAPI RtlGetCurrentTransaction();

BOOL WINAPI RtlSetCurrentTransaction();

NTSYSAPI
NTSTATUS
NTAPI
RtlAddMandatoryAce(
    _Inout_ PACL Acl,
    _In_ ULONG Revision,
    _In_ ULONG Flags,
    _In_ ULONG MandatoryFlags,
    _In_ UCHAR AceType,
    _In_ PSID LabelSid); 		
							
ULONG WINAPI EtwEventActivityIdControl(
  _In_     ULONG ControlCode,
  _Inout_  LPGUID ActivityId
);

NTSTATUS 
WINAPI 
RtlQueryActivationContextApplicationSettings( 
	DWORD flags, 
	HANDLE handle, 
	const WCHAR *ns,
    const WCHAR *settings, 
	WCHAR *buffer,
    SIZE_T size, 
	SIZE_T *written 
);

NTSTATUS WINAPI RtlFlsAlloc(PFLS_CALLBACK_FUNCTION lpCallback, 
							DWORD *lpCallbackPointer);

NTSTATUS WINAPI RtlFlsFree(DWORD dwFlsIndex);
							  
NTSTATUS WINAPI RtlReadThreadProfilingData(
	HANDLE PerformanceDataHandle, 
	DWORD Flags, 
	PPERFORMANCE_DATA PerformanceData
);							  

NTSTATUS 
NTAPI 
NtQuerySystemInformationEx(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation, 
	ULONG QueryInformationLength, 
	PVOID SystemInformatiom, 
	ULONG BufferLength, 
	PULONG ReturnLength
);	

NTSTATUS NTAPI RtlSleepConditionVariableSRW( 	
		IN OUT PRTL_CONDITION_VARIABLE  	ConditionVariable,
		IN OUT PRTL_SRWLOCK  	SRWLock,
		IN PLARGE_INTEGER TimeOut  	OPTIONAL,
		IN ULONG  	Flags 
); 	

VOID NTAPI RtlInitializeSRWLock(OUT PRTL_SRWLOCK SRWLock);

NTSTATUS
NTAPI
RtlSleepConditionVariableCS(IN OUT PRTL_CONDITION_VARIABLE ConditionVariable,
                            IN OUT PRTL_CRITICAL_SECTION CriticalSection,
                            IN const LARGE_INTEGER * TimeOut OPTIONAL); 

NTSTATUS WINAPI RtlGetUserPreferredUILanguages(DWORD dwFlags, 
											   BOOL verification,
											   PULONG pulNumLanguages, 
											   PZZWSTR pwszLanguagesBuffer, 
											   PULONG pcchLanguagesBuffer);
											   
typedef void (CALLBACK *PTP_IO_CALLBACK)(PTP_CALLBACK_INSTANCE,void*,void*,IO_STATUS_BLOCK*,PTP_IO);

/* Thread Pool */
VOID NTAPI TpDisassociateCallback(__inout PTP_CALLBACK_INSTANCE Instance); 
VOID NTAPI TpStartAsyncIoOperation(__inout PTP_IO Io);
VOID NTAPI TpCancelAsyncIoOperation (__inout PTP_IO Io);
VOID NTAPI TpCallbackUnloadDllOnCompletion (__inout PTP_CALLBACK_INSTANCE Instance, __in PVOID DllHandle);
NTSTATUS NTAPI TpAllocPool(TP_POOL **,PVOID);
NTSTATUS NTAPI TpAllocTimer(TP_TIMER **,PTP_TIMER_CALLBACK,PVOID,TP_CALLBACK_ENVIRON *);
NTSTATUS NTAPI TpAllocWait(TP_WAIT **,PTP_WAIT_CALLBACK,PVOID,TP_CALLBACK_ENVIRON *);
NTSTATUS NTAPI TpAllocWork(TP_WORK **,PTP_WORK_CALLBACK,PVOID,TP_CALLBACK_ENVIRON *);
NTSTATUS NTAPI TpSimpleTryPost(PTP_SIMPLE_CALLBACK,PVOID,TP_CALLBACK_ENVIRON *);
NTSTATUS NTAPI TpAllocCleanupGroup(TP_CLEANUP_GROUP **);
NTSTATUS NTAPI TpAllocIoCompletion(TP_IO **,HANDLE,PTP_IO_CALLBACK,void *,TP_CALLBACK_ENVIRON *);

VOID NTAPI TpCallbackReleaseMutexOnCompletion( 
		__inout PTP_CALLBACK_INSTANCE  	Instance,
		__in HANDLE  	Mutex 
); 
VOID NTAPI TpCallbackReleaseSemaphoreOnCompletion( 	
		__inout PTP_CALLBACK_INSTANCE  	Instance,
		__in HANDLE  	Semaphore,
		__in LONG  	ReleaseCount 
) ;

NTSTATUS WINAPI RtlAddSIDToBoundaryDescriptor(
  _Inout_  HANDLE *BoundaryDescriptor,
  _In_     PSID RequiredSid
);

NTSTATUS WINAPI TpSetPoolStackInformation(
  _Inout_  PTP_POOL ptpp,
  _In_     PTP_POOL_STACK_INFORMATION ptpsi
);

NTSTATUS NTAPI 	TpQueryPoolStackInformation (
  __in PTP_POOL Pool, 
  __out PTP_POOL_STACK_INFORMATION PoolStackInformation
);

NTSTATUS 
NTAPI 
NtCreatePrivateNamespace(
  __out PHANDLE NamespaceHandle,
  __in ACCESS_MASK DesiredAccess,
  __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
  __in PVOID BoundaryDescriptor
);

NTSTATUS 
NTAPI
NtRemoveIoCompletionEx(
    __in HANDLE IoCompletionHandle,
    __out FILE_IO_COMPLETION_INFORMATION *IoCompletionInformation,
    __in ULONG Count,
    __out PVOID NumEntriesRemoved,
    __in_opt PLARGE_INTEGER Timeout,
    __in BOOLEAN Alertable
);

NTSTATUS 
NTAPI
NtOpenPrivateNamespace(
  __out PHANDLE NamespaceHandle,
  __in ACCESS_MASK DesiredAccess,
  __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
  __in PVOID BoundaryDescriptor
);

NTSTATUS NTAPI 	NtReplacePartitionUnit(__in PUNICODE_STRING TargetInstancePath, 
										__in PUNICODE_STRING SpareInstancePath,
										__in ULONG Flags);
								
NTSTATUS NTAPI RtlCheckTokenCapability(
  _In_opt_  HANDLE TokenHandle,
  _In_      PSID CapabilitySidToCheck,
  _Out_     PBOOL HasCapability
);

NTSTATUS 
NTAPI 
RtlCheckTokenMembershipEx(
	HANDLE TokenHandle, 
	PSID SidToCheck, 
	DWORD Flas, 
	PBOOL IsMember
);

NTSTATUS 
NTAPI 
NtSetInformationVirtualMemory(
	HANDLE hProcess,
	BOOL access,
	ULONG_PTR NumberOfEntries, 
	PMEMORY_RANGE_ENTRY VirtualAddresses, 
	PULONG Flags,
	DWORD other
);

PVOID NTAPI LdrResolveDelayLoadedAPI(
  _In_        PVOID ParentModuleBase,
  _In_        PCIMAGE_DELAYLOAD_DESCRIPTOR DelayloadDescriptor,
  _In_opt_    PDELAYLOAD_FAILURE_DLL_CALLBACK FailureDllHook,
  _In_opt_    PDELAYLOAD_FAILURE_SYSTEM_ROUTINE FailureSystemHook,
  _Out_       PIMAGE_THUNK_DATA ThunkAddress,
  _Reserved_  ULONG Flags
);

NTSTATUS NTAPI RtlAddResourceAttributeAce(PACL pAcl, 
										  DWORD dwAceRevision, 
										  DWORD AceFlags, 
										  DWORD AccessMask, 
										  PSID pSid, 
										  PCLAIM_SECURITY_ATTRIBUTES_INFORMATION  pAttributeInfo, 
										  PDWORD pReturnLength
);

NTSTATUS WINAPI LdrResolveDelayLoadsFromDll(
  _In_        PVOID ParentBase,
  _In_        LPCSTR TargetDllName,
  _Reserved_  ULONG Flags
);

//PTEB NtCurrentTeb(void);

VOID NTAPI RtlProcessFlsData( void *teb_fls_data, ULONG flags );

NTSTATUS WINAPI NtUnmapViewOfSectionEx(HANDLE handle, PVOID MemoryCache, ULONG number);

NTSTATUS NTAPI RtlQueryThreadProfiling(HANDLE HANDLE, PBOOLEAN Enabled);

NTSTATUS NTAPI RtlCopyContext(PCONTEXT Destination, DWORD ContextFlags, PCONTEXT Source);

NTSTATUS NTAPI RtlDisableThreadProfiling(HANDLE PerformanceDataHandle);

NTSTATUS NTAPI NtDeletePrivateNamespace(__in HANDLE NamespaceHandle);

NTSYSAPI NTSTATUS  WINAPI RtlIsNormalizedString(ULONG,const WCHAR*,INT,BOOLEAN*);

NTSYSAPI NTSTATUS  WINAPI RtlNormalizeString(ULONG,const WCHAR*,INT,WCHAR*,INT*);

BOOL WINAPI RtlDeregisterSecureMemoryCacheCallback(_In_  PRTL_SECURE_MEMORY_CACHE_CALLBACK pfnCallBack);

NTSYSAPI NTSTATUS WINAPI RtlRegisterSecureMemoryCacheCallback(_In_  PRTL_SECURE_MEMORY_CACHE_CALLBACK pfnCallBack);

NTSTATUS NTAPI RtlEnableThreadProfiling(HANDLE ThreadHandle, DWORD Flags, DWORD64 HardwareCounters, HANDLE PerformanceDataHandle);

NTSTATUS NTAPI RtlAddIntegrityLabelToBoundaryDescriptor(HANDLE *BoundaryDescriptor, PSID IntegrityLabel);

VOID NTAPI RtlWakeConditionVariable(IN OUT PRTL_CONDITION_VARIABLE ConditionVariable); 

VOID NTAPI RtlGetCurrentProcessorNumberEx(_Out_  PPROCESSOR_NUMBER ProcNumber);

void * NTAPI RtlLocateLegacyContext( CONTEXT_EX *context_ex, ULONG *length );

NTSTATUS NTAPI RtlInitializeExtendedContext( void *context, ULONG context_flags, CONTEXT_EX **context_ex );

NTSTATUS NTAPI RtlGetExtendedContextLength(DWORD flags, LPDWORD ContextFlags);

ULONG64 NTAPI RtlGetEnabledExtendedFeatures(_In_  ULONG64 FeatureMask);

void * WINAPI RtlLocateExtendedFeature( CONTEXT_EX *context_ex, ULONG feature_id, ULONG *length );

void NTAPI RtlSetExtendedFeaturesMask( CONTEXT_EX *context_ex, ULONG64 feature_mask );

VOID NTAPI 	TpCallbackSetEventOnCompletion (__inout PTP_CALLBACK_INSTANCE Instance, __in HANDLE Event);

BOOL NTAPI WinSqmIsOptedInEx(ULONG number);

HANDLE NTAPI RtlCreateBoundaryDescriptor(LSA_UNICODE_STRING *string, ULONG Flags);	

NTSTATUS NTAPI RtlUnsubscribeWnfStateChangeNotification(PVOID RegistrationHandle);

DWORD NTAPI RtlGetSystemTimePrecise();

typedef enum _RTL_UMS_THREAD_INFO_CLASS
{
    UmsThreadInvalidInfoClass,
    UmsThreadUserContext,
    UmsThreadPriority,
    UmsThreadAffinity,
    UmsThreadTeb,
    UmsThreadIsSuspended,
    UmsThreadIsTerminated,
    UmsThreadMaxInfoClass
} RTL_UMS_THREAD_INFO_CLASS, *PRTL_UMS_THREAD_INFO_CLASS;

typedef enum _RTL_UMS_SCHEDULER_REASON
{
    UmsSchedulerStartup,
    UmsSchedulerThreadBlocked,
    UmsSchedulerThreadYield,
} RTL_UMS_SCHEDULER_REASON, *PRTL_UMS_SCHEDULER_REASON;

typedef void (CALLBACK *PRTL_UMS_SCHEDULER_ENTRY_POINT)(RTL_UMS_SCHEDULER_REASON,ULONG_PTR,PVOID);

typedef enum _PROCESS_MITIGATION_POLICY
{
    ProcessDEPPolicy,
    ProcessASLRPolicy,
    ProcessDynamicCodePolicy,
    ProcessStrictHandleCheckPolicy,
    ProcessSystemCallDisablePolicy,
    ProcessMitigationOptionsMask,
    ProcessExtensionPointDisablePolicy,
    ProcessControlFlowGuardPolicy,
    ProcessSignaturePolicy,
    ProcessFontDisablePolicy,
    ProcessImageLoadPolicy,
    ProcessSystemCallFilterPolicy,
    ProcessPayloadRestrictionPolicy,
    ProcessChildProcessPolicy,
    ProcessSideChannelIsolationPolicy,
    MaxProcessMitigationPolicy
} PROCESS_MITIGATION_POLICY, *PPROCESS_MITIGATION_POLICY;

typedef enum _PROCESS_INFORMATION_CLASS
{
    ProcessMemoryPriority,
    ProcessMemoryExhaustionInfo,
    ProcessAppMemoryInfo,
    ProcessInPrivateInfo,
    ProcessPowerThrottling,
    ProcessReservedValue1,
    ProcessTelemetryCoverageInfo,
    ProcessProtectionLevelInfo,
    ProcessLeapSecondInfo,
    ProcessMachineTypeInfo,
    ProcessInformationClassMax
} PROCESS_INFORMATION_CLASS;

typedef struct _MEMORY_PRIORITY_INFORMATION
{
    ULONG MemoryPriority;
} MEMORY_PRIORITY_INFORMATION, *PMEMORY_PRIORITY_INFORMATION;

typedef struct _FLS_CALLBACK_INFO
{
	PFLS_CALLBACK_FUNCTION CallbackFunc;
	RTL_SRWLOCK FlsCbLock;
} FLS_CALLBACK_INFO;

typedef DWORD WINAPI RTL_RUN_ONCE_INIT_FN(PRTL_RUN_ONCE, PVOID, PVOID*);
typedef RTL_RUN_ONCE_INIT_FN *PRTL_RUN_ONCE_INIT_FN;

typedef enum _THREAD_INFORMATION_CLASS
{
    ThreadMemoryPriority,
    ThreadAbsoluteCpuPriority,
    ThreadDynamicCodePolicy,
    ThreadPowerThrottling,
    ThreadInformationClassMax
} THREAD_INFORMATION_CLASS;

ULONG WINAPI EtwEventUnregister(
  _In_  REGHANDLE RegHandle
);

ULONG
WINAPI
EtwEventRegister(
  IN LPCGUID ProviderId,
  IN PENABLECALLBACK EnableCallback OPTIONAL,
  IN PVOID CallbackContext OPTIONAL,
  OUT PREGHANDLE RegHandle);

BOOLEAN WINAPI EtwEventEnabled(
  _In_  REGHANDLE RegHandle,
  _In_  PCEVENT_DESCRIPTOR EventDescriptor
);

ULONG
WINAPI
EtwEventWrite(
  IN REGHANDLE RegHandle,
  IN PCEVENT_DESCRIPTOR EventDescriptor,
  IN ULONG UserDataCount,
  IN PEVENT_DATA_DESCRIPTOR UserData);

NTSTATUS
NTAPI
RtlSetEnvironmentStrings(
    IN PWCHAR NewEnvironment,
    IN SIZE_T NewEnvironmentSize
    );

NTSTATUS NTAPI 
LdrFindResourceEx_U(
    IN ULONG Flags,
    IN PVOID DllHandle,
    IN PLDR_RESOURCE_INFO ResourceIdPath,
    IN ULONG ResourceIdPathLength,
    OUT PIMAGE_RESOURCE_DATA_ENTRY *ResourceDataEntry
);

BOOLEAN 
WINAPI 
RtlGetProductInfo(
	DWORD dwOSMajorVersion, 
	DWORD dwOSMinorVersion, 
	DWORD dwSpMajorVersion,
    DWORD dwSpMinorVersion, 
	PDWORD pdwReturnedProductType
);

NTSTATUS 
WINAPI 
RtlQueryUnbiasedInterruptTime(
	ULONGLONG *time
);

LONG 
WINAPI 
RtlCompareUnicodeStrings( 
	const WCHAR *s1, 
	SIZE_T len1, 
	const WCHAR *s2, 
	SIZE_T len2,
    BOOLEAN case_insensitive 
);

NTSTATUS 
WINAPI 
RtlWaitOnAddress( 
	const void *addr, 
	const void *cmp, 
	SIZE_T size,
    const LARGE_INTEGER *timeout 
);

#ifdef _M_IX86
typedef struct DECLSPEC_ALIGN(16) _M128A {
  ULONGLONG Low;
  LONGLONG High;
} M128A, *PM128A;
#endif

typedef struct _XSAVE_FORMAT {
    WORD ControlWord;        /* 000 */
    WORD StatusWord;         /* 002 */
    BYTE TagWord;            /* 004 */
    BYTE Reserved1;          /* 005 */
    WORD ErrorOpcode;        /* 006 */
    DWORD ErrorOffset;       /* 008 */
    WORD ErrorSelector;      /* 00c */
    WORD Reserved2;          /* 00e */
    DWORD DataOffset;        /* 010 */
    WORD DataSelector;       /* 014 */
    WORD Reserved3;          /* 016 */
    DWORD MxCsr;             /* 018 */
    DWORD MxCsr_Mask;        /* 01c */
    M128A FloatRegisters[8]; /* 020 */
    M128A XmmRegisters[16];  /* 0a0 */
    BYTE Reserved4[96];      /* 1a0 */
} XSAVE_FORMAT, *PXSAVE_FORMAT;

NTSTATUS WINAPI NtOpenKeyEx( HANDLE *key, ACCESS_MASK access, const OBJECT_ATTRIBUTES *attr, ULONG options );
								  
NTSYSAPI NTSTATUS  WINAPI LdrGetDllPath(PCWSTR,ULONG,PWSTR*,PWSTR*);								  
NTSYSAPI void      WINAPI RtlReleasePath(PWSTR);
NTSYSAPI NTSTATUS  WINAPI LdrAddDllDirectory(const UNICODE_STRING*,void**);
NTSYSAPI NTSTATUS  WINAPI LdrSetDefaultDllDirectories(ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlSetSearchPathMode(ULONG);
NTSYSAPI NTSTATUS  WINAPI LdrRemoveDllDirectory(void*);
NTSYSAPI NTSTATUS  WINAPI RtlIdnToAscii(DWORD,const WCHAR*,INT,WCHAR*,INT*);
NTSYSAPI NTSTATUS  WINAPI RtlIdnToNameprepUnicode(DWORD,const WCHAR*,INT,WCHAR*,INT*);
NTSYSAPI NTSTATUS  WINAPI RtlIdnToUnicode(DWORD,const WCHAR*,INT,WCHAR*,INT*);
NTSYSAPI NTSTATUS  WINAPI RtlGetProcessPreferredUILanguages(DWORD,ULONG*,WCHAR*,ULONG*);
NTSYSAPI NTSTATUS  WINAPI RtlSetThreadPreferredUILanguages(DWORD,PCZZWSTR,ULONG*);
NTSYSAPI NTSTATUS  WINAPI RtlSetProcessPreferredUILanguages(DWORD,PCZZWSTR,ULONG*);
NTSYSAPI NTSTATUS  WINAPI RtlDosPathNameToNtPathName_U_WithStatus(PCWSTR,PUNICODE_STRING,PWSTR*,PRTL_RELATIVE_NAME_U);
NTSYSAPI NTSTATUS  WINAPI NtFilterToken(HANDLE,ULONG,TOKEN_GROUPS*,TOKEN_PRIVILEGES*,TOKEN_GROUPS*,HANDLE*);
NTSYSAPI NTSTATUS  WINAPI RtlConvertToAutoInheritSecurityObject(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR*,GUID*,BOOL,PGENERIC_MAPPING);
NTSYSAPI NTSTATUS  WINAPI NtGetNlsSectionPtr(ULONG,ULONG,void*,void**,SIZE_T*);
NTSYSAPI NTSTATUS WINAPI RtlFlsGetValue( ULONG index, void **data );
NTSYSAPI NTSTATUS WINAPI RtlFlsSetValue( ULONG index, void *data );
NTSTATUS NTAPI RtlUnicodeToUTF8N( char *dst, DWORD dstlen, DWORD *reslen, const WCHAR *src, DWORD srclen );
NTSTATUS NTAPI RtlGetExtendedContextLength2( ULONG context_flags, ULONG *length, ULONG64 compaction_mask );
NTSTATUS NTAPI RtlInitializeExtendedContext2( void *context, ULONG context_flags, CONTEXT_EX **context_ex, ULONG64 compaction_mask );
ULONG64 NTAPI RtlGetExtendedFeaturesMask( CONTEXT_EX *context_ex );
VOID NTAPI RtlWakeConditionVariable(_Inout_ RTL_CONDITION_VARIABLE *ConditionVariable);
VOID NTAPI RtlWakeAllConditionVariable(_Inout_ RTL_CONDITION_VARIABLE *ConditionVariable);
void NTAPI RtlInitializeConditionVariable( RTL_CONDITION_VARIABLE *variable);
VOID WINAPI RtlDeleteBoundaryDescriptor(_In_  HANDLE BoundaryDescriptor);
VOID WINAPI RtlAcquireSRWLockExclusive(IN OUT PRTL_SRWLOCK SRWLock);
VOID NTAPI RtlReleaseSRWLockExclusive(IN OUT PRTL_SRWLOCK SRWLock);
VOID NTAPI RtlReleaseSRWLockShared(IN OUT PRTL_SRWLOCK SRWLock);
VOID NTAPI RtlAcquireSRWLockShared(IN OUT PRTL_SRWLOCK SRWLock);


#ifdef __i386__
#define CONTEXT_I386_XSTATE             (CONTEXT_i386 | 0x0040)
#define CONTEXT_XSTATE CONTEXT_I386_XSTATE
#endif  /* __i386__ */

#ifdef __x86_64__
#define CONTEXT_AMD64_XSTATE          (CONTEXT_AMD64 | 0x0040)
#define CONTEXT_XSTATE CONTEXT_AMD64_XSTATE
#endif /* __x86_64__ */


NTSTATUS
NTAPI
RtlInitializeCriticalSectionEx(
    _Out_ PRTL_CRITICAL_SECTION CriticalSection,
    _In_ ULONG SpinCount,
    _In_ ULONG Flags);