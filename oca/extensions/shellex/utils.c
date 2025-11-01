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

void remove_extended_prefix(const char* input, char* output, size_t output_size) {
    const char* prefix = "\\\\?\\";
    size_t prefix_len = strlen(prefix);

    if (strncmp(input, prefix, prefix_len) == 0) {
        // Remove o prefixo
        strncpy(output, input + prefix_len, output_size - 1);
        output[output_size - 1] = '\0'; // Garantir terminação nula
    } else {
        // Copia normalmente se não tiver o prefixo
        strncpy(output, input, output_size - 1);
        output[output_size - 1] = '\0';
    }
}

void remove_extended_prefix_w(LPCWSTR input, LPWSTR output, size_t output_size) {
    const LPCWSTR prefix = L"\\\\?\\";
    const size_t prefix_len = 4;
    const LPCWSTR unc_prefix = L"UNC\\";

    if (wcsncmp(input, prefix, prefix_len) == 0) {
        // Trata caso \\?\UNC\... → \\server\share
        if (wcsncmp(input + prefix_len, unc_prefix, 4) == 0) {
            // Copia "\\server\share" (começando após "UNC\")
            wcsncpy(output, L"\\\\", output_size - 1);
            output[output_size - 1] = L'\0';
            wcsncat(output, input + prefix_len + 4, output_size - wcslen(output) - 1);
        } else {
            // Caminho normal: apenas pula o \\?\
            wcsncpy(output, input + prefix_len, output_size - 1);
            output[output_size - 1] = L'\0';
        }
    } else {
        // Sem prefixo, copia normalmente
        wcsncpy(output, input, output_size - 1);
        output[output_size - 1] = L'\0';
    }
}