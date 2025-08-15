/*++

Copyright (c) 2025 Shorthorn Project

Module Name:

    enclave.c

Abstract:

    This module implements Windows Enclave (Trusted Execution) APIs

Author:

    Skulltrail 15-August-2025

Revision History:

--*/

#include <main.h>

WINE_DEFAULT_DEBUG_CHANNEL(enclave);

BOOL WINAPI IsEnclaveTypeSupported(DWORD EnclaveType) {
    return FALSE;
}