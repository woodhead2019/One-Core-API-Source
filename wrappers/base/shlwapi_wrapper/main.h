/*++

Copyright (c) 2024 Shorthorn Project

Module Name:

    istream.c

Abstract:

    This module is the main header of Shwlapi Wrapper

Author:

    Skulltrail 10-September-2024

Revision History:

--*/

#include <wine/config.h>

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <stdarg.h>
#include <string.h>

#define COBJMACROS
#define NONAMELESSUNION

#include "windef.h"
#include "winbase.h"
#include <ntsecapi.h>
#include "winerror.h"
#include "winnls.h"
#define NO_SHLWAPI_REG
#define NO_SHLWAPI_PATH
#include <winuser.h>
#include "shlwapi.h"
#include "wine/debug.h"
#include <stdio.h>
#include <math.h>

#define IDS_BYTES_FORMAT 64

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

/* FIXME: This value is stored in the header, but we hard-code it to 0x1000. */
#define LIMIT_TO_USE_SMALL_BLOCK 0x1000

#define STGM_ACCESS_MODE(stgm)   ((stgm)&0x0000f)
#define STGM_SHARE_MODE(stgm)    ((stgm)&0x000f0)
#define STGM_CREATE_MODE(stgm)   ((stgm)&0x0f000)

#define ILSkip(pidl, cb) ((PUIDLIST_RELATIVE)VOID_OFFSET((pidl), (cb)))
#define ILNext(pidl) ILSkip(pidl, (pidl)->mkid.cb) 

#define VOID_OFFSET(pv, cb) ((void*)(((BYTE*)(pv))+(cb)))

/* Structure for formatting byte strings */
typedef struct tagSHLWAPI_BYTEFORMATS
{
  LONGLONG dLimit;
  double   dDivisor;
  double   dNormaliser;
  int      nDecimals;
  WCHAR     wPrefix;
} SHLWAPI_BYTEFORMATS;

typedef enum  { 
  SFBS_FLAGS_ROUND_TO_NEAREST_DISPLAYED_DIGIT     = 0x00000001,
  SFBS_FLAGS_TRUNCATE_UNDISPLAYED_DECIMAL_DIGITS  = 0x00000002
} SFBS_FLAGS;

HRESULT 
WINAPI 
PSStrFormatByteSizeEx(
        ULONGLONG  ull,
        SFBS_FLAGS flags,
  _Out_ PWSTR      pszBuf,
        UINT       cchBuf
);

BOOL WINAPI PathMatchSpecA(_In_ LPCSTR, _In_ LPCSTR);
BOOL WINAPI PathMatchSpecW(_In_ LPCWSTR, _In_ LPCWSTR);

HRESULT
WINAPI
PathCreateFromUrlW(
  _In_ LPCWSTR pszUrl,
  _Out_writes_to_(*pcchPath, *pcchPath) LPWSTR pszPath,
  _Inout_ LPDWORD pcchPath,
  DWORD dwFlags);

BOOL WINAPI PathRemoveFileSpecA(_Inout_ LPSTR);
BOOL WINAPI PathRemoveFileSpecW(_Inout_ LPWSTR);

ULARGE_INTEGER inline UlongToLargeInt(int i) {
    ULARGE_INTEGER li;
    li.QuadPart = i;
    return li;
}
