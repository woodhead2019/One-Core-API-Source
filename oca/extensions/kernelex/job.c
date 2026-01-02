/*++

Copyright (c) 2024  Shorthorn Project

Module Name:

    job.c

Abstract:

    Support for the Job Object

Author:

    Skulltrail 15-August-2024

Revision History:

--*/

#include "main.h"

typedef BOOL (WINAPI *PFN_QueryInformationJobObjectNative)(
    HANDLE,
    JOBOBJECTINFOCLASS,
    LPVOID,
    DWORD,
    LPDWORD
);

static PFN_QueryInformationJobObjectNative pQueryInformationJobObjectNative = NULL;
static HMODULE g_kernel32 = NULL;

static BOOL load_kernel32_job_native(void)
{
    if (!g_kernel32)
        g_kernel32 = GetModuleHandleW(L"kernel32.dll");

    if (!g_kernel32)
        return FALSE;

    if (!pQueryInformationJobObjectNative)
        pQueryInformationJobObjectNative =
            (PFN_QueryInformationJobObjectNative)
                GetProcAddress(g_kernel32, "QueryInformationJobObjectNative");

    return (pQueryInformationJobObjectNative != NULL);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsProcessInJobInternal(IN HANDLE ProcessHandle,
               IN HANDLE JobHandle,
               OUT PBOOL Result)
{
    NTSTATUS Status;

    Status = NtIsProcessInJob(ProcessHandle, JobHandle);
    if (NT_SUCCESS(Status))
    {
        *Result = (Status == STATUS_PROCESS_IN_JOB);
        return TRUE;
    }

    BaseSetLastNTError(Status);
    return FALSE;
}

BOOL 
WINAPI 
IsProcessInJob(HANDLE ProcessHandle, HANDLE JobHandle, PBOOL Result) {
    if (JobHandle == NULL && Result != NULL && ProcessHandle == NtCurrentProcess()) {
        //DbgPrint("IsProcessInJob_Internal :: VxKex method Triggered. Remove when not needed.");
        *Result = TRUE;
        return TRUE;
    }
    return IsProcessInJobInternal(ProcessHandle, JobHandle, Result);
}

/*
 * @implemented
 */
BOOL
WINAPI
AssignProcessToJobObjectInternal(IN HANDLE hJob,
                         IN HANDLE hProcess)
{
    NTSTATUS Status;

    Status = NtAssignProcessToJobObject(hJob, hProcess);
    if (!NT_SUCCESS(Status))
    {
        BaseSetLastNTError(Status);
        return FALSE;
    }

    return TRUE;
}

BOOL 
WINAPI 
AssignProcessToJobObject(HANDLE hJob, HANDLE hProcess) {
    BOOL Result = AssignProcessToJobObjectInternal(hJob, hProcess);
    if (Result == FALSE && GetLastError() == ERROR_ACCESS_DENIED) {
        //DbgPrint("AssignProjectToJobObject_Internal :: Detected the use of Nested Job Objects. Lie about it.");
        Result = TRUE;
        SetLastError(0);
    }
    return Result;
}

BOOL 
WINAPI 
QueryInformationJobObject(
	HANDLE hJob,
	JOBOBJECTINFOCLASS JobObjectInformationClass,
	LPVOID lpJobObjectInformation,
	DWORD cbJobObjectInformationLength,
	LPDWORD lpReturnLength
) {
	JOBOBJECT_BASIC_LIMIT_INFORMATION BasicLimitInfo;
	PGROUP_AFFINITY Affinity;	

    //if (!g_kernel32)
        g_kernel32 = GetModuleHandleW(L"kernel32.dll");

    //if (!pQueryInformationJobObjectNative)
        pQueryInformationJobObjectNative =
            (PFN_QueryInformationJobObjectNative)
                GetProcAddress(g_kernel32, "QueryInformationJobObjectNative");

    // // Default fallback behavior
    // if (!load_kernel32_job_native() || !pQueryInformationJobObjectNative) {
        // SetLastError(ERROR_PROC_NOT_FOUND);
        // return FALSE;
    // }	

	// Check for JobObjectGroupInformation or JobObjectGroupInformationEx
	if (JobObjectInformationClass == JobObjectGroupInformation) {
		if (lpReturnLength)
			*lpReturnLength = sizeof(USHORT);
		
		if (cbJobObjectInformationLength < sizeof(USHORT)) {
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return FALSE;
		}

		*(USHORT*)lpJobObjectInformation = 0; // XP does not have processor groups.
		return FALSE;
	} else if (JobObjectInformationClass == JobObjectGroupInformationEx) {
		if (lpReturnLength)
			*lpReturnLength = sizeof(GROUP_AFFINITY);
		// Java does this when the kernel32.dll version is Windows 10 or greater. However, this is needed
		// for Chromium 126 or greater to launch without hitting a DCHECK.
		if (cbJobObjectInformationLength < sizeof(GROUP_AFFINITY)) {
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return FALSE;
		}
        if (!pQueryInformationJobObjectNative(
                hJob,
                JobObjectBasicLimitInformation,
                &BasicLimitInfo,
                sizeof(JOBOBJECT_BASIC_LIMIT_INFORMATION),
                NULL))
        {
            return FALSE;
        }

		Affinity = lpJobObjectInformation;
		Affinity->Group = 0;
		if (BasicLimitInfo.LimitFlags & JOB_OBJECT_LIMIT_AFFINITY) {
			Affinity->Mask = BasicLimitInfo.Affinity;
		} else {
			DWORD_PTR ignored;
			// Uhh... not set to a affinity, so we have to just make a guess and go for it.
			return GetProcessAffinityMask(GetCurrentProcess(), &(Affinity->Mask), &ignored);
		}
		return TRUE;
	}

    return pQueryInformationJobObjectNative(
        hJob,
        JobObjectInformationClass,
        lpJobObjectInformation,
        cbJobObjectInformationLength,
        lpReturnLength
    );
}