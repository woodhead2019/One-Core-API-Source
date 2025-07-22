/*++

Copyright (c) 2024 Shorthorn Project

Module Name:

    istream.c

Abstract:

    This module implements Win32 Shell IStream Interface Functions

Author:

    Skulltrail 10-September-2024

Revision History:

--*/

#include "main.h"
#include "shlobj.h"

WINE_DEFAULT_DEBUG_CHANNEL(shlwapi);

/*************************************************************************
 * IStream_ReadStr   [SHLWAPI.596]
 */
HRESULT WINAPI IStream_ReadStr(IStream *pstm, PWSTR *ppsz)
{
  HRESULT result;
  int readBytes;
  unsigned short length;
  PWSTR str;
  
  *ppsz = NULL;
  result = pstm->lpVtbl->Read(pstm, &length, sizeof(unsigned short), &readBytes);
  if (NT_SUCCESS(result))
  {
    if (readBytes == 2) {
        str = CoTaskMemAlloc((length + 1) * sizeof(WCHAR));
        if (str != NULL) {
            result = pstm->lpVtbl->Read(pstm, str, length * sizeof(WCHAR), &readBytes);
            if (NT_SUCCESS(result)) {
                if (readBytes == length * sizeof(WCHAR)) {
                    str[length] = 0; // Null-terminate the string
                    *ppsz = str;
                    return STATUS_SUCCESS;
                } else
                    result = E_FAIL;
            }
            CoTaskMemFree(str);
        } else
            return E_OUTOFMEMORY;
    } else
      return E_FAIL;
  }
  return result;
}

/*************************************************************************
 * IStream_WriteStr   [SHLWAPI.597]
 */
HRESULT WINAPI IStream_WriteStr(IStream *pstm, PCWSTR psz)
{
  HRESULT result;
  unsigned int slen = lstrlenW(psz);
  unsigned short real_len = slen;
  if (slen >= 65536) { 
    // For some reason, IStream_WriteStr from Windows 7 has a 65,535-character string limitation. We need to play along.
    WARN("IStream_WriteStr length overflow: Win7 implementation only supports 65535 WCHARS, actual length is %i\n");
  }
  real_len = (unsigned short)slen;
  result = pstm->lpVtbl->Write(pstm, &real_len, sizeof(unsigned short), 0);
  if (NT_SUCCESS(result))
  {
      return pstm->lpVtbl->Write(pstm, psz, real_len * sizeof(WCHAR), 0);
  }
  return result;
}

HRESULT WINAPI IStream_Copy(IStream *pstmFrom, IStream *pstmTo, DWORD cb)
{
  TRACE("IStream_Copy(%p, %p, %u)", pstmFrom, pstmTo, cb);

  return pstmFrom->lpVtbl->CopyTo(
             pstmFrom,
             pstmTo,
             UlongToLargeInt(cb),
             NULL,
             NULL);
}