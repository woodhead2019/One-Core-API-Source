/*++

Copyright (c) 2017 Shorthorn Project

Module Name:

    mem.c

Abstract:

    This module contains the Win32 Global Memory Management APIs

Author:

    Skulltrail 19-March-2017

Revision History:

--*/

#include "main.h"

#define DIV 1024

SIZE_T
WINAPI
GetLargePageMinimum (
    VOID
)
{
    return (SIZE_T) SharedUserData->LargePageMinimum;
}

BOOL 
WINAPI 
GetPhysicallyInstalledSystemMemory(
  _Out_  PULONGLONG TotalMemoryInKilobytes
)
{
	MEMORYSTATUSEX memory;
	memory.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memory);
	*TotalMemoryInKilobytes = memory.ullTotalPhys/DIV;
	return TRUE;
}

// BOOL 
// WINAPI 
// AllocateUserPhysicalPagesNuma(
	// HANDLE hProcess, 
	// PULONG_PTR NumberOfPages, 
	// PULONG_PTR PageArray, 
	// DWORD nndPreferred
// )
// {
  // DWORD_PTR ThreadAffinitiyMask;
  // NTSTATUS Status; 
  // HANDLE CurrentThread; 
  // ULONGLONG ProcessorMask; 
  
  // DbgPrint("AllocateUserPhysicalPagesNuma called\n");

  // ThreadAffinitiyMask = 0;
  // if ( nndPreferred != -1 )
  // {
    // if ( !GetNumaNodeProcessorMask(nndPreferred, &ProcessorMask) )
      // return AllocateUserPhysicalPages(hProcess, NumberOfPages, PageArray);
    // ThreadAffinitiyMask = ProcessorMask;
    // CurrentThread = GetCurrentThread();
    // ThreadAffinitiyMask = SetThreadAffinityMask(CurrentThread, ThreadAffinitiyMask);
    // if ( !ThreadAffinitiyMask )
      // return FALSE;
  // }
  // Status = NtAllocateUserPhysicalPages(hProcess, NumberOfPages, PageArray);
  // if ( ThreadAffinitiyMask )
  // {
    // CurrentThread = GetCurrentThread();
    // SetThreadAffinityMask(CurrentThread, ThreadAffinitiyMask);
  // }
  // if ( NT_SUCCESS(Status) )
    // return TRUE;
  // BaseSetLastNTError(Status);
  // return FALSE;
// }

BOOL 
WINAPI 
AllocateUserPhysicalPagesNuma(
    HANDLE hProcess, 
    PULONG_PTR NumberOfPages, 
    PULONG_PTR PageArray, 
    DWORD nndPreferred
)
{
  DWORD_PTR AffinityMask = 0;
  NTSTATUS Status; 
  ULONGLONG ProcessorMask; 

  if (nndPreferred != NUMA_NO_PREFERRED_NODE)
  {
    DbgPrint("AllocateUserPhysicalPagesNuma called on NUMA node %i\n", nndPreferred);
    if (!GetNumaNodeProcessorMask(nndPreferred, &ProcessorMask))
        return AllocateUserPhysicalPages(hProcess, NumberOfPages, PageArray);
    
    AffinityMask = SetThreadAffinityMask(GetCurrentThread(), ProcessorMask);
    if (!AffinityMask)
      return FALSE;
  }
  
  Status = NtAllocateUserPhysicalPages(hProcess, NumberOfPages, PageArray);
  
  if (AffinityMask)
    SetThreadAffinityMask(GetCurrentThread(), AffinityMask);
  
  if (NT_SUCCESS(Status))
    return TRUE;
  
  BaseSetLastNTError(Status);
  return FALSE;
}

LPVOID 
WINAPI 
VirtualAllocExNuma(
	HANDLE ProcessHandle, 
	LPVOID BaseAddress, 
	SIZE_T AllocationSize, 
	DWORD AllocationType, 
	DWORD Protect, 
	DWORD nndPreferred
)
{
	if ( nndPreferred != -1 )
      AllocationType |= nndPreferred + 1;
    return VirtualAllocEx(ProcessHandle,
						  BaseAddress,
						  AllocationSize,
						  AllocationType,
						  Protect);
}

/***********************************************************************
 *             PrefetchVirtualMemory   (kernelex.@)
 */
BOOL 
WINAPI 
DECLSPEC_HOTPATCH 
PrefetchVirtualMemory( 
	HANDLE _hProcess, 
	ULONG_PTR _uNumberOfEntries,
    WIN32_MEMORY_RANGE_ENTRY *_pVirtualAddresses, 
	ULONG _fFlags 
)
{
	UNREFERENCED_PARAMETER(_hProcess);
	UNREFERENCED_PARAMETER(_uNumberOfEntries);
	UNREFERENCED_PARAMETER(_pVirtualAddresses);
	UNREFERENCED_PARAMETER(_fFlags);

	// 假装自己预取成功
	return TRUE;
}

// /***********************************************************************
 // *             DiscardVirtualMemory   (kernelex.@)
 // */
// DWORD WINAPI DECLSPEC_HOTPATCH DiscardVirtualMemory( void *addr, SIZE_T size )
// {
    // NTSTATUS status;
    // LPVOID ret = addr;

    // status = NtAllocateVirtualMemory( GetCurrentProcess(), &ret, 0, &size, MEM_RESET, PAGE_NOACCESS );
    // return RtlNtStatusToDosError( status );
// }

DWORD
WINAPI
OfferVirtualMemory(
	_Inout_updates_(_uSize) PVOID _pVirtualAddress,
	_In_ SIZE_T _uSize,
	_In_ OFFER_PRIORITY _ePriority
)
{
	// 低版本系统不支持这个机制，所以暂时假装内存充足，不触发回收
	MEMORY_BASIC_INFORMATION _Info;
	
	UNREFERENCED_PARAMETER(_ePriority);
	if (VirtualQuery(_pVirtualAddress, &_Info, sizeof(_Info)) == 0)
		return GetLastError();

	if (_Info.State != MEM_COMMIT)
		return ERROR_INVALID_PARAMETER;


	if ((char*)_pVirtualAddress + _uSize > (char*)_Info.BaseAddress + _Info.RegionSize)
		return ERROR_INVALID_PARAMETER;

	return ERROR_SUCCESS;
}

DWORD
WINAPI
ReclaimVirtualMemory(
	_In_reads_(_uSize) void const* _pVirtualAddress,
	_In_ SIZE_T _uSize
)
{
	MEMORY_BASIC_INFORMATION _Info;
	if (VirtualQuery(_pVirtualAddress, &_Info, sizeof(_Info)) == 0)
		return GetLastError();

	if (_Info.State != MEM_COMMIT)
		return ERROR_INVALID_PARAMETER;


	if ((char*)_pVirtualAddress + _uSize > (char*)_Info.BaseAddress + _Info.RegionSize)
		return ERROR_INVALID_PARAMETER;
			
	return ERROR_SUCCESS;
}

static ULONG OfferVirtualMemoryInternal(
	IN	PVOID			VirtualAddress,
	IN	SIZE_T			Size,
	IN	OFFER_PRIORITY	Priority,
	IN	BOOL			DiscardMemory)
{
	NTSTATUS Status;
	MEMORY_BASIC_INFORMATION BasicInformation;
	PVOID VirtualAllocResult;
	ULONG OldProtect;

	//
	// Parameter validation.
	//

	if (!VirtualAddress || !Size) {
		return ERROR_INVALID_PARAMETER;
	}

	if ((ULONG_PTR) VirtualAddress & 0xFFF) {
		// The virtual address must be page-aligned.
		return ERROR_INVALID_PARAMETER;
	}

	if (Size & 0xFFF) {
		// The size must be a multiple of the page size.
		return ERROR_INVALID_PARAMETER;
	}

	ASSERT (Priority && Priority < VMOfferPriorityMaximum);

	//
	// Check to see if the memory region provided is valid.
	// The entire region must be readable, writable, and committed.
	//

	Status = NtQueryVirtualMemory(
		NtCurrentProcess(),
		VirtualAddress,
		MemoryBasicInformation,
		&BasicInformation,
		sizeof(BasicInformation),
		NULL);

	if (!NT_SUCCESS(Status)) {
		return RtlNtStatusToDosError(Status);
	}

	if (BasicInformation.RegionSize < Size ||
		BasicInformation.Protect != PAGE_READWRITE ||
		BasicInformation.State != MEM_COMMIT) {

		Status = STATUS_INVALID_PAGE_PROTECTION;
		return RtlNtStatusToDosError(Status);
	}

	//
	// Tell the kernel that we won't be needing the contents of this memory
	// anymore.
	//

	VirtualAllocResult = VirtualAlloc(
		VirtualAddress,
		Size,
		MEM_RESET,
		PAGE_READWRITE);

	if (VirtualAllocResult != VirtualAddress) {
		return GetLastError();
	}

	if (DiscardMemory) {
		VirtualUnlock(VirtualAddress, Size);
	} else {
		// If OfferVirtualMemory was called, then make those pages
		// inaccessible.
		VirtualProtect(VirtualAddress, Size, PAGE_NOACCESS, &OldProtect);
	}

	return ERROR_SUCCESS;
}

// ULONG WINAPI OfferVirtualMemory(
	// IN	PVOID			VirtualAddress,
	// IN	SIZE_T			Size,
	// IN	OFFER_PRIORITY	Priority)
// {
	// DbgPrint(
		// L"OfferVirtualMemory called\r\n\r\n"
		// L"VirtualAddress: 0x%p\r\n"
		// L"Size:           %lu",
		// VirtualAddress,
		// Size);

	// if (!Priority || Priority >= VMOfferPriorityMaximum) {
		// return ERROR_INVALID_PARAMETER;
	// }

	// return OfferVirtualMemoryInternal(VirtualAddress, Size, Priority, FALSE);
// }

DWORD
WINAPI
DiscardVirtualMemory(
    _Inout_updates_(_uSize) PVOID _pVirtualAddress,
    _In_ SIZE_T _uSize
)
{
    // if (const auto _pfnDiscardVirtualMemory = try_get_DiscardVirtualMemory())
    // {
        // return _pfnDiscardVirtualMemory(_pVirtualAddress, _uSize);
    // }

    VirtualAlloc(_pVirtualAddress, _uSize, MEM_RESET, PAGE_NOACCESS);
    return ERROR_SUCCESS;
}

PVOID
WINAPI
VirtualAlloc2(
        _In_opt_ HANDLE Process,
        _In_opt_ PVOID BaseAddress,
        _In_ SIZE_T Size,
        _In_ ULONG AllocationType,
        _In_ ULONG PageProtection,
        _Inout_updates_opt_(ParameterCount) MEM_EXTENDED_PARAMETER* ExtendedParameters,
        _In_ ULONG ParameterCount
        )
{
		ULONG i;
        // if (const auto pVirtualAlloc2 = try_get_VirtualAlloc2())
        // {
            // return pVirtualAlloc2(Process, BaseAddress, Size, AllocationType, PageProtection, ExtendedParameters, ParameterCount);
        // }

        //尝试搜索 MemExtendedParameterNumaNode
        if (ExtendedParameters && ParameterCount)
        {
            for (i = 0; i != ParameterCount; ++i)
            {
                if (ExtendedParameters[i].Type == MemExtendedParameterNumaNode)
                {
                    return VirtualAllocExNuma(Process, BaseAddress, Size, AllocationType, PageProtection, ExtendedParameters[i].ULong);
                }
            }
        }

        //尽力了，只能调用VirtualAllocEx。
        return VirtualAllocEx(Process, BaseAddress, Size, AllocationType, PageProtection);
}