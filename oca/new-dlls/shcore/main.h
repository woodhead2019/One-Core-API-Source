/*++

Copyright (c) 2025 Shorthorn Project

Module Name:

    main.h

Abstract:

    This module implements Win32 Shell IStream Interface Functions

Author:

    Skulltrail 13-June-2025

Revision History:

--*/

#include <stdarg.h>

#define COBJMACROS

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "initguid.h"
#include "ocidl.h"
#include "shellscalingapi.h"
#include "shlwapi.h"
#include "featurestagingapi.h"
#include "shcore.h"

#include "wine/debug.h"
#include "wine/heap.h"

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <string.h>

#define COBJMACROS
#define NONAMELESSUNION

#include <ntsecapi.h>
#include "winerror.h"
#include "winnls.h"
#define NO_SHLWAPI_REG
#define NO_SHLWAPI_PATH
#include "wine/debug.h"
#include <stdio.h>
#include <math.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

ULARGE_INTEGER inline UlongToLargeInt(int i) {
    ULARGE_INTEGER li;
    li.QuadPart = i;
    return li;
}