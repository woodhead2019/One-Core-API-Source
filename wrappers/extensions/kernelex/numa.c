/*++

Copyright (c) 2018  Shorthorn Project

Module Name:

    numa.c

Abstract:

    This module implements Win32 Non Uniform Memory Architecture
    information APIs.

Author:

    Skulltrail 18-April-2018

Revision History:

--*/

#include "main.h"

static BOOL (WINAPI *pGetNumaNodeProcessorMask)(UCHAR, PULONGLONG);

BOOL
WINAPI
GetNumaNodeProcessorMask(
    UCHAR Node,
    PULONGLONG ProcessorMask
    )

{
    NTSTATUS Status;
    ULONG ReturnedSize;
    SYSTEM_NUMA_INFORMATION Map;

	Status = NtQuerySystemInformation(SystemNumaProcessorMap,
                                      &Map,
                                      sizeof(Map),
                                      &ReturnedSize);
	if (!NT_SUCCESS(Status)) {
		BaseSetLastNTError(Status);
		return FALSE;
	}

	if (Node > Map.HighestNodeNumber) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	*ProcessorMask = Map.ActiveProcessorsAffinityMask[Node];
	return TRUE;
}

BOOL 
WINAPI 
GetNumaAvailableMemoryNodeEx(
    USHORT Node, 
    PULONGLONG AvailableBytes
)
{
    if (Node > 0xFF) DbgPrint("GetNumaAvailableMemoryNodeEx:: node %i does not fit in a UCHAR!", Node);

    return GetNumaAvailableMemoryNode(Node, AvailableBytes);
}

BOOL 
WINAPI 
GetNumaNodeProcessorMaskEx(
	USHORT Node, 
	PGROUP_AFFINITY ProcessorMask
)
{
	ULONGLONG mask;
	if(GetNumaNodeProcessorMask(Node, &mask)){
		ProcessorMask->Group = 0;
		ProcessorMask->Mask = mask;
		ProcessorMask->Reserved[0] = 0;
		return TRUE;
	}else{
		return FALSE;
	}	
}

/***********************************************************************
 *           GetNumaProcessorNodeEx (KERNEL32.@)
 */
BOOL WINAPI GetNumaProcessorNodeEx(PPROCESSOR_NUMBER Processor, PUSHORT NodeNumber)
{
	if(!Processor)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	return GetNumaProcessorNode(Processor->Number, (PUCHAR)NodeNumber);
}

/***********************************************************************
 *           GetNumaProximityNode (KERNEL32.@)
 */
BOOL WINAPI GetNumaProximityNode(ULONG  proximity_id, PUCHAR node_number)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

/***********************************************************************
 *           GetNumaProximityNodeEx (KERNEL32.@)
 */
BOOL WINAPI GetNumaProximityNodeEx(ULONG  proximity_id, PUSHORT node_number)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}