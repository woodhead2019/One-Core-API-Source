/*++

Copyright (c) 2026  Shorthorn Project

Module Name:

    reg.c

Abstract:

    Workaround functions for known issues

Author:

    Skulltrail 28-January-2026

Revision History:

--*/

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(workarounds);

static void apply_restrictions(DWORD dwFlags, DWORD dwType, DWORD cbData, PLONG ret)
{
    /* Check if the type is restricted by the passed flags */
    if (*ret == ERROR_SUCCESS || *ret == ERROR_MORE_DATA)
    {
        DWORD dwMask = 0;

        switch (dwType)
        {
        case REG_NONE: dwMask = RRF_RT_REG_NONE; break;
        case REG_SZ: dwMask = RRF_RT_REG_SZ; break;
        case REG_EXPAND_SZ: dwMask = RRF_RT_REG_EXPAND_SZ; break;
        case REG_MULTI_SZ: dwMask = RRF_RT_REG_MULTI_SZ; break;
        case REG_BINARY: dwMask = RRF_RT_REG_BINARY; break;
        case REG_DWORD: dwMask = RRF_RT_REG_DWORD; break;
        case REG_QWORD: dwMask = RRF_RT_REG_QWORD; break;
        }

        if (dwFlags & dwMask)
        {
            /* Type is not restricted, check for size mismatch */
            if (dwType == REG_BINARY)
            {
                DWORD cbExpect = 0;

                if ((dwFlags & RRF_RT_ANY) == RRF_RT_DWORD)
                    cbExpect = 4;
                else if ((dwFlags & RRF_RT_ANY) == RRF_RT_QWORD)
                    cbExpect = 8;

                if (cbExpect && cbData != cbExpect)
                    *ret = ERROR_DATATYPE_MISMATCH;
            }
        } else *ret = ERROR_UNSUPPORTED_TYPE;
    }
}

static inline BOOL is_string(DWORD type)
{
    return (type == REG_SZ) || (type == REG_EXPAND_SZ) || (type == REG_MULTI_SZ);
}

LSTATUS Py_RegGetValueW(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValue,
    DWORD dwFlags, LPDWORD pdwType, PVOID pvData,
    LPDWORD pcbData)
{
    DWORD dwType, cbData = (pvData && pcbData) ? *pcbData : 0;
    PVOID pvBuf = NULL;
    LONG ret;

    if (pvData && !pcbData)
        return ERROR_INVALID_PARAMETER;

    if ((dwFlags & RRF_RT_REG_EXPAND_SZ) && !(dwFlags & (RRF_NOEXPAND | RRF_RT_REG_SZ)) &&
        ((dwFlags & RRF_RT_ANY) != RRF_RT_ANY))
        return ERROR_INVALID_PARAMETER;

    if ((dwFlags & RRF_WOW64_MASK) == RRF_WOW64_MASK)
        return ERROR_INVALID_PARAMETER;

    if (pszSubKey && pszSubKey[0])
    {
        REGSAM samDesired = KEY_QUERY_VALUE;

        if (dwFlags & RRF_WOW64_MASK)
            samDesired |= (dwFlags & RRF_SUBKEY_WOW6432KEY) ? KEY_WOW64_32KEY : KEY_WOW64_64KEY;

        ret = RegOpenKeyExW(hKey, pszSubKey, 0, samDesired, &hKey);
        if (ret != ERROR_SUCCESS) return ret;
    }

    ret = RegQueryValueExW(hKey, pszValue, NULL, &dwType, pvData, &cbData);

    /* If the value is a string, we need to read in the whole value to be able
     * to know exactly how many bytes are needed after expanding the string and
     * ensuring that it is null-terminated. */
    if (is_string(dwType) &&
        (ret == ERROR_MORE_DATA ||
            (ret == ERROR_SUCCESS && dwType == REG_EXPAND_SZ && !(dwFlags & RRF_NOEXPAND)) ||
            (ret == ERROR_SUCCESS && (cbData < sizeof(WCHAR) || (pvData && *((WCHAR *)pvData + cbData / sizeof(WCHAR) - 1))))))
    {
        do {
            HeapFree(GetProcessHeap(), 0, pvBuf);

            pvBuf = HeapAlloc(GetProcessHeap(), 0, cbData + sizeof(WCHAR));
            if (!pvBuf)
            {
                ret = ERROR_NOT_ENOUGH_MEMORY;
                break;
            }

            if (ret == ERROR_MORE_DATA || !pvData)
                ret = RegQueryValueExW(hKey, pszValue, NULL,
                    &dwType, pvBuf, &cbData);
            else
            {
                /* Even if cbData was large enough we have to copy the
                 * string since ExpandEnvironmentStrings can't handle
                 * overlapping buffers. */
                CopyMemory(pvBuf, pvData, cbData);
            }
        } while (ret == ERROR_MORE_DATA);

        if (ret == ERROR_SUCCESS)
        {
            /* Ensure null termination */
            if (cbData < sizeof(WCHAR) || *((WCHAR *)pvBuf + cbData / sizeof(WCHAR) - 1))
            {
                *((WCHAR *)pvBuf + cbData / sizeof(WCHAR)) = 0;
                cbData = sizeof(WCHAR);
            }

            /* Recheck dwType in case it changed since the first call */
            if (dwType == REG_EXPAND_SZ && !(dwFlags & RRF_NOEXPAND))
            {
                cbData = ExpandEnvironmentStringsW(pvBuf, pvData,
                    pcbData ? *pcbData : 0) * sizeof(WCHAR);
                dwType = REG_SZ;
                if (pvData && cbData > *pcbData)
                    ret = ERROR_MORE_DATA;
            } else if (pvData)
            {
                if (cbData > *pcbData)
                    ret = ERROR_MORE_DATA;
                else
                    CopyMemory(pvData, pvBuf, cbData);
            }
        }

        HeapFree(GetProcessHeap(), 0, pvBuf);
    }

    if (pszSubKey && pszSubKey[0])
        RegCloseKey(hKey);

    apply_restrictions(dwFlags, dwType, cbData, &ret);

    if (pvData && ret != ERROR_SUCCESS && (dwFlags & RRF_ZEROONFAILURE))
        ZeroMemory(pvData, *pcbData);

    if (pdwType) *pdwType = dwType;
    if (pcbData) *pcbData = cbData;

    return ret;
}