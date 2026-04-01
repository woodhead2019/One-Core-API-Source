/*++

Copyright (c) 2021  Shorthorn Project

Module Name:

    eventlog.c

Abstract:

    Event Log Handle functions

Author:

    Skulltrail 18-July-2021

Revision History:

--*/

#include "main.h"

ULONG 
WINAPI
EventWriteEndScenario(
    REGHANDLE RegHandle,
    PCEVENT_DESCRIPTOR EventDescriptor,
    ULONG UserDataCount,
    PEVENT_DATA_DESCRIPTOR UserData)
{
	if(pEventWriteEndScenario){
		pEventWriteEndScenario(RegHandle, EventDescriptor, UserDataCount, UserData);
	}		
	//return EtwEventWriteEndScenario(RegHandle, EventDescriptor, UserDataCount, UserData);
	return ERROR_SUCCESS;
}

ULONG 
WINAPI
EventWriteStartScenario(
    REGHANDLE RegHandle,
    PCEVENT_DESCRIPTOR EventDescriptor,
    ULONG UserDataCount,
    PEVENT_DATA_DESCRIPTOR UserData)
{
	if(pEventWriteStartScenario){
		pEventWriteStartScenario(RegHandle, EventDescriptor, UserDataCount, UserData);
	}	
	//return EtwEventWriteStartScenario(RegHandle, EventDescriptor, UserDataCount, UserData);
	return ERROR_SUCCESS;
}

/* unimplemented*/
ULONG 
WINAPI 
EventWriteEx(
  _In_      REGHANDLE RegHandle,
  _In_      PCEVENT_DESCRIPTOR EventDescriptor,
  _In_      ULONG64 Filter,
  _In_      ULONG Flags,
  _In_opt_  LPCGUID ActivityId,
  _In_      LPCGUID RelatedActivityId,
  _In_      ULONG UserDataCount,
  _In_opt_  PEVENT_DATA_DESCRIPTOR UserData
)
{
	if(pEventWriteEx){
		pEventWriteEx(RegHandle, EventDescriptor, Filter, Flags, ActivityId, RelatedActivityId, UserDataCount, UserData);
	}	
	return ERROR_SUCCESS;
}

/* unimplemented*/
ULONG 
WINAPI 
EventAccessControl(
  _In_  LPGUID Guid,
  _In_  ULONG Operation,
  _In_  PSID Sid,
  _In_  ULONG Rights,
  _In_  BOOLEAN AllowOrDeny
)
{
	if(pEventAccessControl){
		pEventAccessControl(Guid, Operation, Sid, Rights, AllowOrDeny);
	}
	
	return ERROR_SUCCESS;
}

/*
 * @implemented
 */
NTSTATUS
NTAPI
ElfReportEventAndSourceW(
	IN HANDLE hEventLog,
    IN ULONG Time,
    IN PUNICODE_STRING ComputerName,
    IN USHORT EventType,
    IN USHORT EventCategory,
    IN ULONG EventID,
    IN PSID UserSID,
    IN PUNICODE_STRING SourceName,
    IN USHORT NumStrings,
    IN ULONG DataSize,
    IN PUNICODE_STRING* Strings,
    IN PVOID Data,
    IN USHORT Flags,
    IN OUT PULONG RecordNumber,
    IN OUT PULONG TimeWritten)
{
	ELF_REPORT_EVENT_AND_SOURCE elfReportEventAndSource;
	
    elfReportEventAndSource = (ELF_REPORT_EVENT_AND_SOURCE) GetProcAddress(
                            GetModuleHandle(TEXT("advapibase.dll")),
                            "ElfReportEventAndSourceW");
    if(elfReportEventAndSource){
		return (NTSTATUS)elfReportEventAndSource(hEventLog, 
											Time,
											ComputerName,
											EventType,
											EventCategory,
											EventID,
											UserSID,
											SourceName,
											NumStrings,
											DataSize,
											Strings,
											Data,
											Flags,
											RecordNumber,
											TimeWritten);
	}
	
	return STATUS_NOT_IMPLEMENTED;
}	

BOOL 
WINAPI
EvtReport(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
	return TRUE;
}

ULONG 
WINAPI 
EvtRegisterPublisher(int a1, HMODULE hModule, int a3, int a4)
{
	return ERROR_SUCCESS;
}