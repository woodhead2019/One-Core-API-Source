/*++

Copyright (c) 2024  Shorthorn Project

Module Name:

    shfileop.c

Abstract:

    This module implements Interface Functions for Shell File Operations

Author:

    Skulltrail 24-October-2024

Revision History:

--*/

#include <main.h>
 
WINE_DEFAULT_DEBUG_CHANNEL(shell);


/***********************************************************************
 *    DisplayNameOfW [SHELL32.757] (Vista+)
 */
HRESULT 
WINAPI
DisplayNameOfW(
    _In_ IShellFolder *psf,
    _In_ LPCITEMIDLIST pidl,
    _In_ DWORD dwFlags,
    _Out_ LPWSTR pszBuf,
    _In_ UINT cchBuf)
{
    STRRET sr;	
    HRESULT hr;
	
	hr = psf->lpVtbl->GetDisplayNameOf(psf, pidl, dwFlags, &sr);
    *pszBuf = UNICODE_NULL;	
    if (FAILED(hr))
        return hr;
    return StrRetToBufW(&sr, pidl, pszBuf, cchBuf);
}
