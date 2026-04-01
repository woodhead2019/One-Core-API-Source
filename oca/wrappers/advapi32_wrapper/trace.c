/*++

Copyright (c) 2026  Shorthorn Project

Module Name:

    trace.c

Abstract:

    Trace information related functions

Author:

    Skulltrail 20-March-2026

Revision History:

--*/

#include "main.h"

/******************************************************************************
 *     TraceSetInformation   (sechost.@)
 */
ULONG WINAPI TraceSetInformation( TRACEHANDLE handle, TRACE_INFO_CLASS class, void *info, ULONG len )
{
    FIXME("%s %d %p %ld: stub\n", wine_dbgstr_longlong(handle), class, info, len);
    return ERROR_CALL_NOT_IMPLEMENTED;
}

ULONG 
WINAPI 
EnableTraceEx(
  _In_      LPCGUID ProviderId,
  _In_opt_  LPCGUID SourceId,
  _In_      TRACEHANDLE TraceHandle,
  _In_      ULONG IsEnabled,
  _In_      UCHAR Level,
  _In_      ULONGLONG MatchAnyKeyword,
  _In_      ULONGLONG MatchAllKeyword,
  _In_      ULONG EnableProperty,
  _In_opt_  PEVENT_FILTER_DESCRIPTOR EnableFilterDesc
)
{
	if(pEnableTraceEx){
		pEnableTraceEx(ProviderId, SourceId, TraceHandle, IsEnabled, Level, MatchAnyKeyword, MatchAllKeyword, EnableProperty, EnableFilterDesc);
	}	
	return ERROR_SUCCESS;	
}

ULONG 
WINAPI
EnableTraceEx2(
  _In_     CONTROLTRACE_ID          TraceId,
  _In_     LPCGUID                  ProviderId,
  _In_     ULONG                    ControlCode,
  _In_     UCHAR                    Level,
  _In_     ULONGLONG                MatchAnyKeyword,
  _In_     ULONGLONG                MatchAllKeyword,
  _In_     ULONG                    Timeout,
  _In_opt_ PENABLE_TRACE_PARAMETERS EnableParameters
)
{
	if(pEnableTraceEx2){
		pEnableTraceEx2(TraceId, ProviderId, ControlCode, Level, MatchAnyKeyword, MatchAllKeyword, Timeout, EnableParameters);
	}
	
	return ERROR_SUCCESS;
}

ULONG WINAPI EnumerateTraceGuidsEx(
  TRACE_QUERY_INFO_CLASS TraceQueryInfoClass,
  PVOID                  InBuffer,
  ULONG                  InBufferSize,
  PVOID                  OutBuffer,
  ULONG                  OutBufferSize,
  PULONG                 ReturnLength
) {
    // TODO implement it on top of EnumerateTraceGuids
    return STATUS_NOT_IMPLEMENTED;
}