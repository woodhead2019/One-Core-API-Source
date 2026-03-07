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

WINE_DEFAULT_DEBUG_CHANNEL(hooks);

HRESULT 
WINAPI 
CoWaitForMultipleHandlesHook(
    DWORD dwFlags,
    DWORD dwTimeout,
    ULONG cHandles,
    LPHANDLE pHandles,
    LPDWORD lpdwIndex)
{
    DWORD filteredFlags;

    /* Remove APENAS as flags não suportadas */
    filteredFlags = dwFlags &
        ~(COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES);

    return CoWaitForMultipleHandles(
        filteredFlags,
        dwTimeout,
        cHandles,
        pHandles,
        lpdwIndex);
}