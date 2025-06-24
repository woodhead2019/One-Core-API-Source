/*++

Copyright (c) 2018 Shorthorn Project

Module Name:

    ntapi.c

Abstract:

    Implement User Mode Nt functions

Author:

    Skulltrail 06-March-2018

Revision History:

--*/

#include <main.h>

static NTSTATUS NTAPI NtLoadKeyToFixOffice2013Installer(POBJECT_ATTRIBUTES TargetKey, POBJECT_ATTRIBUTES SourceFile) {
    return NtLoadKey(TargetKey, SourceFile);
}
NTSTATUS 
NTAPI
NtLoadKeyEx(
    IN POBJECT_ATTRIBUTES TargetKey,
    IN POBJECT_ATTRIBUTES SourceFile,
    IN ULONG Flags,
    IN HANDLE TrustClassKey
) 
{
	DbgPrint("NtOpenKeyEx:: parameters ignored %i , %p\n", Flags, TrustClassKey);
    return NtLoadKeyToFixOffice2013Installer(TargetKey, SourceFile);
}

NTSTATUS 
NTAPI
NtQueryOpenSubKeysEx(
	IN POBJECT_ATTRIBUTES TargetKey,
	IN ULONG BufferLength,
	IN PVOID Buffer,
	IN PULONG RequiredSize
)
{
	return NtQueryOpenSubKeys(TargetKey, RequiredSize);
}

NTSTATUS 
WINAPI 
NtUnmapViewOfSectionEx(
	HANDLE handle, 
	PVOID MemoryCache, 
	ULONG number)
{
	return NtUnmapViewOfSection(handle, MemoryCache);	
}

NTSTATUS
NTAPI
NtCreateMutantEx(
  OUT PHANDLE             MutantHandle,
  IN ACCESS_MASK          DesiredAccess,
  IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
  IN BOOLEAN              InitialOwner, 
  IN ULONG 				  Flags
)
{
	return NtCreateMutant(MutantHandle, DesiredAccess, ObjectAttributes, InitialOwner);
}

NTSTATUS 
NTAPI 
NtCreateIoCompletionEx(
	__out PHANDLE IoCompletionHandle, 
	__in ACCESS_MASK DesiredAccess, 
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes, 
	__in_opt ULONG Count
)
{
	return NtCreateIoCompletion(IoCompletionHandle, DesiredAccess, ObjectAttributes, Count);
} 

// NTSTATUS 
// NTAPI
// NtRemoveIoCompletionEx(
    // __in HANDLE IoCompletionHandle,
    // __out FILE_IO_COMPLETION_INFORMATION *IoCompletionInformation,
    // __in ULONG Count,
    // __out ULONG *NumEntriesRemoved,
    // __in_opt PLARGE_INTEGER Timeout,
    // __in BOOLEAN Alertable
// )
// {
	// PVOID CompletionKey;
	// PVOID CompletionValue;
	// IO_STATUS_BLOCK IoStatusBlock;
	// NTSTATUS Status;
	// ULONG i = 0;
	
    // for (;;)
    // {
        // while (i < Count)
        // {
			// Status = NtRemoveIoCompletion(IoCompletionHandle,
										 // &CompletionKey,
										 // &CompletionValue,
										 // &IoStatusBlock,
										  // Timeout);
                // if (NT_SUCCESS(Status))
                // {
                    // IoCompletionInformation[i].KeyContext             = CompletionKey;
                    // IoCompletionInformation[i].ApcContext             = CompletionValue;
                    // IoCompletionInformation[i].IoStatusBlock.Information = IoStatusBlock.Information;
                    // IoCompletionInformation[i].IoStatusBlock.Status    = IoStatusBlock.Status;
                // }
            // if (Status != STATUS_SUCCESS) break;
            // ++i;
        // }
        // if (i || Status != STATUS_PENDING)
        // {
            // if (Status == STATUS_PENDING) Status = STATUS_SUCCESS;
            // break;
        // }
        // Status = NtWaitForSingleObject( IoCompletionHandle, Alertable, Timeout );
        // if (Status != WAIT_OBJECT_0) break;
    // }
    // *NumEntriesRemoved = i ? i : 1;
    // return Status;
// }

NTSTATUS 
NTAPI 
NtCreateSemaphoreEx(
  IN OPTIONAL   HANDLE lpSemaphoreAttributes,
  IN       LONG                  lInitialCount,
  IN       LONG                  lMaximumCount,
  IN OPTIONAL  LPCTSTR               lpName,
  IN 	   DWORD                 dwFlags,
  IN       DWORD                 dwDesiredAccess
)
{
	return STATUS_SUCCESS;
}

NTSTATUS 
NTAPI 
NtCreateKeyTransacted(
	__out PHANDLE KeyHandle, 
	__in ACCESS_MASK DesiredAccess, 
	__in POBJECT_ATTRIBUTES ObjectAttributes, 
	ULONG TitleIndex, 
	__in_opt PUNICODE_STRING Class, 
	__in ULONG CreateOptions, 
	__in HANDLE TransactionHandle, 
	__out_opt PULONG Disposition)
{
	return NtCreateKey(KeyHandle, 
					   DesiredAccess, 
					   ObjectAttributes, 
					   TitleIndex, 
					   Class, 
					   CreateOptions, 
					   Disposition);
}

NTSTATUS 
NTAPI 
NtOpenKeyTransacted(
	__out PHANDLE KeyHandle, 
	__in ACCESS_MASK DesiredAccess, 
	__in POBJECT_ATTRIBUTES ObjectAttributes, 
	__in HANDLE TransactionHandle)
{
	return NtOpenKey(KeyHandle, 
					 DesiredAccess, 
					 ObjectAttributes);
}

/* Section Unimplemented*/
NTSTATUS
NTAPI
NtOpenPrivateNamespace(
  __out PHANDLE NamespaceHandle,
  __in ACCESS_MASK DesiredAccess,
  __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
  __in PVOID BoundaryDescriptor
)
{
	return STATUS_SUCCESS;
}

NTSTATUS 
NTAPI 
NtSetInformationVirtualMemory(HANDLE hProcess,
							  BOOL access,
							  ULONG_PTR NumberOfEntries, 
							  PMEMORY_RANGE_ENTRY VirtualAddresses, 
							  PULONG Flags,
							  DWORD other
)
{
	return STATUS_SUCCESS;
}	

NTSTATUS 
NTAPI 	
NtReplacePartitionUnit(__in PUNICODE_STRING TargetInstancePath, 
					   __in PUNICODE_STRING SpareInstancePath,
					   __in ULONG Flags)
{
	return STATUS_SUCCESS;
}

NTSTATUS 
NTAPI 
NtDeletePrivateNamespace(
  __in HANDLE NamespaceHandle
)
{
	return STATUS_SUCCESS;
}

NTSTATUS 
NTAPI 
NtCreatePrivateNamespace(
  __out PHANDLE NamespaceHandle,
  __in ACCESS_MASK DesiredAccess,
  __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
  __in PVOID BoundaryDescriptor
)
{
	return STATUS_SUCCESS;
}

NTSTATUS 
NTAPI
NtTraceControl(
	ULONG FunctionCode, 
	PVOID InBuffer, 
	ULONG InBufferLen, 
	PVOID OutBuffer, 
	ULONG OutBufferLen, 
	PULONG ReturnLength
)
{
	return STATUS_UNSUCCESSFUL;
}

VOID 
NTAPI 
NtFlushProcessWriteBuffers()	
{
	;
}

NTAPI 
NtSetSystemPowerState( 	
	IN POWER_ACTION  	SystemAction,
	IN SYSTEM_POWER_STATE  	MinSystemState,
	IN ULONG  	Flags 
)
{
	return STATUS_SUCCESS;
} 	

NTSTATUS 
WINAPI 
NtCreateThreadEx(PHANDLE hThread,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    HANDLE ProcessHandle,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    ULONG CreateFlags,
    ULONG StackZeroBits,
    ULONG SizeOfStackCommit,
    ULONG SizeOfStackReserve,
    LPVOID lpBytesBuffer)
{
    NTSTATUS status = RtlCreateUserThread(ProcessHandle, 
										  NULL, 
										  CreateFlags & 1, 
										  0, 
										  0, 
										  0, 
										  lpStartAddress, 
										  lpParameter, 
										  hThread, 
										  NULL);
    return status;
}

NTSTATUS 
NTAPI
NtCreateTransaction(
  _Out_    PHANDLE            TransactionHandle,
  _In_     ACCESS_MASK        DesiredAccess,
  _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
  _In_opt_ LPGUID             Uow,
  _In_opt_ HANDLE             TmHandle,
  _In_opt_ ULONG              CreateOptions,
  _In_opt_ ULONG              IsolationLevel,
  _In_opt_ ULONG              IsolationFlags,
  _In_opt_ PLARGE_INTEGER     Timeout,
  _In_opt_ PUNICODE_STRING    Description
)
{
	*TransactionHandle = NULL;
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS 
NTAPI
NtQueryInformationTransaction(
  _In_      HANDLE                        TransactionHandle,
  _In_      TRANSACTION_INFORMATION_CLASS TransactionInformationClass,
  _Out_     PVOID                         TransactionInformation,
  _In_      ULONG                         TransactionInformationLength,
  _Out_opt_ PULONG                        ReturnLength
)
{
	TransactionInformation = NULL;
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS 
NTAPI
NtSetInformationTransaction(
  _In_ HANDLE                        TransactionHandle,
  _In_ TRANSACTION_INFORMATION_CLASS TransactionInformationClass,
  _In_ PVOID                         TransactionInformation,
  _In_ ULONG                         TransactionInformationLength
)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS 
NTAPI
NtRollbackTransaction(
  _In_ HANDLE  TransactionHandle,
  _In_ BOOLEAN Wait
)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS 
NTAPI
NtCommitTransaction(
  _In_ HANDLE  TransactionHandle,
  _In_ BOOLEAN Wait
)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS 
NTAPI
NtOpenTransaction(
  _Out_    PHANDLE            TransactionHandle,
  _In_     ACCESS_MASK        DesiredAccess,
  _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
  _In_     LPGUID             Uow,
  _In_opt_ HANDLE             TmHandle
)
{
	*TransactionHandle = NULL;
	return STATUS_NOT_IMPLEMENTED;
}

static 
NTSTATUS 
NTAPI 
NtOpenKeyToFixOffice2013Installer(PHANDLE retkey, ACCESS_MASK access, const OBJECT_ATTRIBUTES *attr) {
    return NtOpenKey( retkey, access, attr);
}

NTSTATUS 
WINAPI 
NtOpenKeyEx( 
	PHANDLE retkey, 
	ACCESS_MASK access, 
	const OBJECT_ATTRIBUTES *attr, 
	ULONG options 
)
{
	DbgPrint("NtOpenKeyEx:: parameters ignored %i , %p\n", options, retkey);
    return NtOpenKeyToFixOffice2013Installer( retkey, access, attr);
}

/**************************************************************************
 *           NtCancelIoFileEx   (NTDLL.@)
 */
NTSTATUS 
WINAPI 
NtCancelIoFileEx( 
	HANDLE handle, 
	IO_STATUS_BLOCK *io, 
	IO_STATUS_BLOCK *io_status 
)
{
	/*Ignoring io parameter for now*/
	UNREFERENCED_PARAMETER(io);
	return NtCancelIoFile(handle, io_status);
}

/***********************************************************************
 *             NtRemoveIoCompletionEx (NTDLL.@)
 */
NTSTATUS WINAPI NtRemoveIoCompletionEx( HANDLE handle, FILE_IO_COMPLETION_INFORMATION *info, ULONG count,
                                        ULONG *written, LARGE_INTEGER *timeout, BOOLEAN alertable )
{
    NTSTATUS status;
    ULONG i = 0;
	PVOID CompletionKey = 0;
	PVOID CompletionValue = 0;
	PIO_STATUS_BLOCK IoStatusBlock = {0};

    for (;;)
    {
        while (i < count)
        {
			status = NtRemoveIoCompletion(handle, CompletionKey, CompletionValue, IoStatusBlock, timeout);			
            if(status == STATUS_SUCCESS)
			{
				info[i].KeyContext             = CompletionKey;
				info[i].ApcContext             = CompletionValue;
				info[i].IoStatusBlock.Information = IoStatusBlock->Information;
				info[i].IoStatusBlock.Status    = IoStatusBlock->Status;   
			}				
			if (status != STATUS_SUCCESS) break;
            ++i;
        }
        if (i || status != STATUS_PENDING)
        {
            if (status == STATUS_PENDING) status = STATUS_SUCCESS;
            break;
        }
        status = NtWaitForSingleObject( handle, alertable, timeout );
        if (status != WAIT_OBJECT_0) break;
    }
    *written = i ? i : 1;
    return status;
}

// NTSTATUS
// NTAPI
// NtGetNextThread (
    // __in HANDLE ProcessHandle,
    // __in HANDLE ThreadHandle,
    // __in ACCESS_MASK DesiredAccess,
    // __in ULONG HandleAttributes,
    // __in ULONG Flags,
    // __out PHANDLE NewThreadHandle
    // )
// {
    // HANDLE hSnapshot;
    // THREADENTRY32 te32;
    // DWORD currentThreadId;
    // DWORD nextThreadId;
    // BOOL found;
    // BOOL hasThread;
    // HANDLE hThread;
	// CLIENT_ID ClientId;
	// OBJECT_ATTRIBUTES Obja;
	// NTSTATUS Status;

    // currentThreadId = 0;
    // nextThreadId = 0;
    // found = FALSE;
	
	// UNREFERENCED_PARAMETER(ProcessHandle);
	// UNREFERENCED_PARAMETER(ThreadHandle);
	// UNREFERENCED_PARAMETER(DesiredAccess);
	// UNREFERENCED_PARAMETER(HandleAttributes);
	// UNREFERENCED_PARAMETER(Flags);

    // hSnapshot = LdrCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    // if (hSnapshot == INVALID_HANDLE_VALUE) {
        // return STATUS_UNSUCCESSFUL;
    // }

    // te32.dwSize = sizeof(THREADENTRY32);
    // hasThread = LdrThread32First(hSnapshot, &te32);

    // while (hasThread) {
        // if (te32.th32OwnerProcessID == HandleToUlong(NtCurrentTeb()->ClientId.UniqueProcess) && te32.th32ThreadID > currentThreadId) {
            // if (!found || te32.th32ThreadID < nextThreadId) {
                // nextThreadId = te32.th32ThreadID;
                // found = TRUE;
            // }
        // }
        // hasThread = LdrThread32Next(hSnapshot, &te32);
    // }

    // //CloseHandle(hSnapshot);
	// NtClose(hSnapshot);

    // if (!found) {
        // return STATUS_NO_MORE_ENTRIES;
    // }

	// ClientId.UniqueThread = (HANDLE)LongToHandle(te32.th32ThreadID);
	// ClientId.UniqueProcess = (HANDLE)NULL;

	// InitializeObjectAttributes(&Obja,
								// NULL,
								// 0,
								// NULL,
								// NULL
								// );
	// Status = NtOpenThread(
						// &hThread,
						// (ACCESS_MASK)THREAD_QUERY_INFORMATION,
						// &Obja,
						// &ClientId
						// );
	// if ( !NT_SUCCESS(Status) ) {
		// return Status;
	// }

    // // hThread = OpenThread(DesiredAccess, (HandleAttributes & OBJ_INHERIT) != 0, nextThreadId);
    // // if (hThread == NULL) {
        // // return STATUS_ACCESS_DENIED;
    // // }

    // *NewThreadHandle = hThread;
    // return STATUS_SUCCESS;
// }

// NTSTATUS 
// NTAPI
// NtGetNextProcess(
    // HANDLE ProcessHandle,
    // ACCESS_MASK DesiredAccess,
    // ULONG HandleAttributes,
    // ULONG Flags,
    // PHANDLE NewProcessHandle
// ) {
    // HANDLE hSnapshot;
    // PROCESSENTRY32W pe32;
    // DWORD currentPid;
    // DWORD nextPid;
    // BOOL found;
    // BOOL hasProcess;
    // HANDLE hProcess;
	// CLIENT_ID cid;
	// OBJECT_ATTRIBUTES objAttr;
	// NTSTATUS status;

    // currentPid = 0;
    // nextPid = 0;
    // found = FALSE;
	
	// UNREFERENCED_PARAMETER(ProcessHandle);
	// UNREFERENCED_PARAMETER(Flags);

    // hSnapshot = LdrCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // if (hSnapshot == INVALID_HANDLE_VALUE) {
        // return STATUS_UNSUCCESSFUL;
    // }

    // pe32.dwSize = sizeof(PROCESSENTRY32);
    // hasProcess = LdrProcess32First(hSnapshot, &pe32);

    // while (hasProcess) {
        // if (pe32.th32ProcessID > currentPid) {
            // if (!found || pe32.th32ProcessID < nextPid) {
                // nextPid = pe32.th32ProcessID;
                // found = TRUE;
            // }
        // }
        // hasProcess = LdrProcess32Next(hSnapshot, &pe32);
    // }

    // NtClose(hSnapshot);

    // if (!found) {
        // return STATUS_NO_MORE_ENTRIES;
    // }


    // // Abre o novo processo usando NtOpenProcess
    // cid.UniqueProcess = (HANDLE)(ULONG_PTR)nextPid;
    // cid.UniqueThread = NULL;
    // InitializeObjectAttributes(&objAttr, NULL, HandleAttributes, NULL, NULL);

    // status = NtOpenProcess(&hProcess, DesiredAccess, &objAttr, &cid);
    // if (status != STATUS_SUCCESS)
        // return STATUS_ACCESS_DENIED;

    // *NewProcessHandle = hProcess;
    // return STATUS_SUCCESS;
// }