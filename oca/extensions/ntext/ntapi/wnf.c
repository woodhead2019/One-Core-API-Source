/*++

Copyright (c) 2026 One-Core-API Project

Module Name:

    wnf.c

Abstract:

    Implement functions for Windows Notification Facility (WNF)

Author:

    nixxoq 5-February-2026

Revision History:

--*/

#define NODEBUG

#include "main.h"

#define WNF_POWR_BATTERY_STATE 0x41C6023F103003B5ULL

/*
WNF Internal Definitions
*/
typedef struct _WNF_STATE_NAME
{
    ULONG Data[2];
} WNF_STATE_NAME, *PWNF_STATE_NAME;

typedef const WNF_STATE_NAME *PCWNF_STATE_NAME;
typedef ULONG WNF_CHANGE_STAMP, *PWNF_CHANGE_STAMP;

typedef struct _WNF_TYPE_ID {
    GUID TypeId;
} WNF_TYPE_ID, *PWNF_TYPE_ID;

typedef const WNF_TYPE_ID *PCWNF_TYPE_ID;
typedef ULONG LOGICAL;

typedef struct _WNF_BATTERY_STATE_DATA
{
    ULONG BatteryPresent;
    ULONG Charging;
    ULONG Percentage;
} WNF_BATTERY_STATE_DATA;

#ifndef SystemBatteryState
#define SystemBatteryState 5
#endif

// https://www.geoffchappell.com/studies/windows/win32/ntdll/api/native.htm
// https://chromium.googlesource.com/external/github.com/DynamoRIO/drmemory/+/refs/heads/master/wininc/ntexapi.h#276
NTSTATUS
NTAPI
NtQueryWnfStateData(
    _In_ PWNF_STATE_NAME StateName,
    _In_opt_ PVOID TypeId,
    _In_opt_ const VOID *ExplicitScope,
    _Out_ PWNF_CHANGE_STAMP ChangeStamp,
    _Out_writes_bytes_to_opt_(*BufferSize, *BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize)
{
    ULONGLONG Name;
    ULONG RequiredSize = 0;
    PVOID SourcePointer = NULL;
    WNF_BATTERY_STATE_DATA BatteryData;
    NTSTATUS Status;

    if (!StateName || !BufferSize)
    {
        return STATUS_INVALID_PARAMETER;
    }

    Name = *(ULONGLONG *)StateName;

    DbgPrint(
        "NtQueryWnfStateData called! ([%08x,%08x], %p, %p, %p, %p, %p)\n", StateName->Data[0], StateName->Data[1],
        TypeId, ExplicitScope, ChangeStamp, Buffer, BufferSize);

    // Set change stamp using system time as a reference
    if (ChangeStamp)
    {
        LARGE_INTEGER SystemTime;
        NtQuerySystemTime(&SystemTime);
        *ChangeStamp = (WNF_CHANGE_STAMP)SystemTime.LowPart;
    }

    Status = STATUS_SUCCESS;

    switch (Name)
    {
        case WNF_POWR_BATTERY_STATE:
        {
            SYSTEM_BATTERY_STATE BatteryState;
            Status = NtQuerySystemInformation(
                (SYSTEM_INFORMATION_CLASS)SystemBatteryState, &BatteryState, sizeof(BatteryState), NULL);

            if (NT_SUCCESS(Status))
            {
                BatteryData.BatteryPresent = BatteryState.BatteryPresent;
                BatteryData.Charging = BatteryState.Charging;

                if (BatteryState.MaxCapacity > 0)
                {
                    BatteryData.Percentage = (BatteryState.RemainingCapacity * 100) / BatteryState.MaxCapacity;
                }
                else
                {
                    BatteryData.Percentage = 100;
                }

                SourcePointer = &BatteryData;
                RequiredSize = sizeof(WNF_BATTERY_STATE_DATA);
            }
            break;
        }

        default:
            // TODO: Add more WNF state names
            DbgPrint("NtQueryWnfStateData called! Unknown WNF State Name: 0x%I64X\n", Name);
            RequiredSize = 0;
            break;
    }

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    if (*BufferSize < RequiredSize)
    {
        *BufferSize = RequiredSize;
        return STATUS_BUFFER_TOO_SMALL;
    }

    if (RequiredSize > 0 && Buffer && SourcePointer)
    {
        RtlCopyMemory(Buffer, SourcePointer, RequiredSize);
    }

    *BufferSize = RequiredSize;

    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
NtUpdateWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_reads_bytes_opt_(Length) const VOID *Buffer,
    _In_opt_ ULONG Length,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_opt_ const PVOID ExplicitScope,
    _In_ WNF_CHANGE_STAMP MatchingChangeStamp,
    _In_ LOGICAL CheckStamp)
{
    ULONGLONG Name;

    if (!StateName)
    {
        return STATUS_INVALID_PARAMETER;
    }

    Name = *(ULONGLONG *)StateName;

    DbgPrint(
        "NtUpdateWnfStateData called! ([%08x,%08x], Buf %p, Len %u, TypeId %p, Stamp %u, Check %u)\n",
        StateName->Data[0], StateName->Data[1], Buffer, Length, TypeId, MatchingChangeStamp, CheckStamp);

    // We don't actually store data, as we trying to emulate WNF
    // So yeah, it's a stub function that always (perhaps) return STATUS_SUCCESS
    return STATUS_SUCCESS;
};
