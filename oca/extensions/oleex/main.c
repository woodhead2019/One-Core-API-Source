/*++

Copyright (c) 2023 Shorthorn Project

Module Name:

    main.c

Abstract:

    This module implements COM Main functions APIs

Author:

    Skulltrail 12-October-2023

Revision History:

--*/

#define WIN32_NO_STATUS

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(ole32);

typedef interface IUnknown IActivationFilter;

IActivationFilter globalActivationFilter = {0};

HRESULT 
WINAPI
CoDisconnectContext(
  _In_ DWORD dwTimeout
)
{
	DbgPrint("CoDisconnectContext called\n");
	return S_OK;
}

/***********************************************************************
 *      CoGetActivationState (ole32.@)
 */
HRESULT WINAPI CoGetActivationState(GUID guid, DWORD unknown, DWORD *unknown2)
{
	DbgPrint("CoGetActivationState called\n");
    return E_NOTIMPL;
}

/***********************************************************************
 *      CoGetCallState (ole32.@)
 */
HRESULT WINAPI CoGetCallState(int unknown, PULONG unknown2)
{
	DbgPrint("CoGetCallState called\n");	
    return E_NOTIMPL;
}

HRESULT WINAPI CoRegisterActivationFilter(IActivationFilter *pActivationFilter)
{
  IActivationFilter *activationFilter; // rax
  
  DbgPrint("CoRegisterActivationFilter called\n");

  if ( !pActivationFilter )
    return 0x80070057;
  activationFilter = (IActivationFilter *)_InterlockedCompareExchange64(
                                            (signed __int64*)&globalActivationFilter,
                                            (signed __int64)pActivationFilter,
                                            0i64);
  if ( !activationFilter || activationFilter == pActivationFilter )
    return 0;
  else
    return 0x80004021;
}