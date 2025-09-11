/*++

Copyright (c) 2017  Shorthorn Project

Module Name:

    sysinfo.c

Abstract:

    This module implements System Information APIs for Win32

Author:

    Skulltrail 11-April-2017

Revision History:

--*/

#include <main.h>

static BOOL (WINAPI *pSetSystemFileCacheSize)(SIZE_T, SIZE_T,int Flags);

/*
* @unimplemented - need implementation
*/
BOOL 
WINAPI 
GetOSProductNameW(
	PCWSTR Source, 
	ULONG var, 
	ULONG parameter
)
{
	Source = L"Microsoft Windows Codename \"Longhorn\" Professional Version 2003 Copyright ";
	return TRUE;
}

/*
* @unimplemented - need implementation
*/
BOOL 
WINAPI 
GetOSProductNameA(
	PCSTR Source, 
	ULONG var, 
	ULONG parameter
)
{
	Source = "Microsoft Windows Codename \"Longhorn\" Professional Version 2003 Copyright ";
	return TRUE;
}

/*
 * @implemented - new
 */
BOOL 
WINAPI 
GetProductInfo(
  _In_   DWORD dwOSMajorVersion,
  _In_   DWORD dwOSMinorVersion,
  _In_   DWORD dwSpMajorVersion,
  _In_   DWORD dwSpMinorVersion,
  _Out_  PDWORD pdwReturnedProductType
)
{
	return RtlGetProductInfo(dwOSMajorVersion, 
							 dwOSMinorVersion,
                             dwSpMajorVersion, 
							 dwSpMinorVersion, 
							 pdwReturnedProductType);
}

BOOL 
WINAPI 
SetSystemFileCacheSize(
	SIZE_T MinimumFileCacheSize, 
	SIZE_T MaximumFileCacheSize, 
	int Flags
)
{
	NTSTATUS Status; 
	BOOL result; 
	SYSTEM_FILECACHE_INFORMATION SystemInformation; 
	
	SystemInformation.MinimumWorkingSet = MinimumFileCacheSize;
	SystemInformation.MaximumWorkingSet = MaximumFileCacheSize;
	SystemInformation.Flags = Flags;
	
	Status = NtSetSystemInformation(SystemFileCacheInformationEx, &SystemInformation, sizeof(SYSTEM_FILECACHE_INFORMATION));
	if ( !NT_SUCCESS(Status) )
	{
		BaseSetLastNTError(Status);
		result = FALSE;
	}
	else
	{
		result = TRUE;
	}
	return result;	
}

// Required for Firefox 133.
// On most systems, and under most conditions, each CPU Set ID will map directly to a single home logical processor.
// This means that CPU sets can be partially emulated by it being a more complicated GetLogicalProcessorInformation.
// Note that it does not support processor groups just yet.
BOOL WINAPI GetSystemCpuSetInformation(
    PSYSTEM_CPU_SET_INFORMATION  Information,
    ULONG                        BufferLength,
    PULONG                       ReturnedLength,
    HANDLE                       Process,
    ULONG                        Flags
) {
    // Get the number of cores in the system.
	PSYSTEM_CPU_SET_INFORMATION currentCpuSet;
	SYSTEM_INFO sysInfo;
	DWORD requiredLength;
	KAFFINITY processAffinity;
	KAFFINITY systemAffinity;
	int i;
	
	GetSystemInfo(&sysInfo);
	if (!GetProcessAffinityMask(Process, &processAffinity, &systemAffinity))
		return FALSE;
	
	requiredLength = sizeof(SYSTEM_CPU_SET_INFORMATION) * sysInfo.dwNumberOfProcessors;
	
    if (ReturnedLength)
        *ReturnedLength = requiredLength;
    
    if (BufferLength < requiredLength) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }
    
    if (!Information) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    
	memset(Information, 0, requiredLength);
	
	for (i = 0; i < sysInfo.dwNumberOfProcessors; i++) {
        currentCpuSet = &Information[i];

        currentCpuSet->Size = sizeof(SYSTEM_CPU_SET_INFORMATION);
        currentCpuSet->Type = CpuSetInformation;

        currentCpuSet->CpuSet.Id                  = i;
        currentCpuSet->CpuSet.Group               = 0;
        currentCpuSet->CpuSet.LogicalProcessorIndex = (UCHAR)i;
        currentCpuSet->CpuSet.CoreIndex           = (UCHAR)i;  // simplificado
        currentCpuSet->CpuSet.LastLevelCacheIndex = 0;
        currentCpuSet->CpuSet.NumaNodeIndex       = 0;
        currentCpuSet->CpuSet.EfficiencyClass     = 0;

        // Flags (bitfields + AllFlags union)
        currentCpuSet->CpuSet.AllFlags = 0;

        if ((systemAffinity & ((KAFFINITY)1 << i)) != 0)
            currentCpuSet->CpuSet.Allocated = 1;

        if ((processAffinity & ((KAFFINITY)1 << i)) != 0)
            currentCpuSet->CpuSet.AllocatedToTargetProcess = 1;

        // Exemplo: nunca setamos Parked ou RealTime aqui
        currentCpuSet->CpuSet.Parked   = 0;
        currentCpuSet->CpuSet.RealTime = 0;

        currentCpuSet->CpuSet.AllocationTag = i;
	}
	
    return TRUE;
}

// rtworkq.dll requires this. If we actually return safe mode, then it intentionally fails to start up rtworkq.dll.
BOOLEAN WINAPI GetOsSafeBootMode(PBOOLEAN isSafeMode) {
    if (!isSafeMode)
        return FALSE;

    *isSafeMode = FALSE;
    return TRUE;
}