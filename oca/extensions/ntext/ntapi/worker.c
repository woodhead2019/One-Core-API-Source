/*++

Copyright (c) 2026 Shorthorn Project

Module Name:

    wnf.c

Abstract:

    Implement functions for Worker Factory (Threapool stuff)

Author:

    Skulltrail 24-February-2026

Revision History:

--*/

#include <main.h>

NTSTATUS
NTAPI
NtCreateWorkerFactory(
	_Out_ PHANDLE WorkerFactoryHandleReturn,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_ HANDLE CompletionPortHandle,
	_In_ HANDLE WorkerProcessHandle,
	_In_ PVOID StartRoutine,
	_In_opt_ PVOID StartParameter,
	_In_opt_ ULONG MaxThreadCount,
	_In_opt_ SIZE_T StackReserve,
	_In_opt_ SIZE_T StackCommit
	)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS
NTAPI
NtQueryInformationWorkerFactory(
    __in HANDLE WorkerFactoryHandle,
    __in WORKERFACTORYINFOCLASS WorkerFactoryInformationClass,
    __out_bcount(WorkerFactoryInformationLength) PVOID WorkerFactoryInformation,
    __in ULONG WorkerFactoryInformationLength,
    __out_opt PULONG ReturnLength
    )
{
	return STATUS_NOT_IMPLEMENTED;
}	