/*++

Copyright (c) 2026 Shorthorn Project

Module Name:

    main.c

Abstract:

    This module implements HTTP functions APIs stubs

Author:

    Skulltrail 21-May-2026

Revision History:

--*/

#include "precomp.h"
#include <winhttp.h>

#define NDEBUG
#include <wine/debug.h>

HINSTANCE hInstance = NULL;

WINE_DEFAULT_DEBUG_CHANNEL(winhttp);

typedef struct _WINHTTP_PROXY_RESULT_ENTRY
{
    BOOL            fProxy;
    BOOL            fBypass;
    INTERNET_SCHEME ProxyScheme;
    PWSTR           pwszProxy;
    INTERNET_PORT   ProxyPort;
} WINHTTP_PROXY_RESULT_ENTRY;

typedef struct _WINHTTP_PROXY_RESULT
{
    DWORD cEntries;
    WINHTTP_PROXY_RESULT_ENTRY *pEntries;
} WINHTTP_PROXY_RESULT;

BOOL
WINAPI
DllMain(HINSTANCE hinstDLL,
        DWORD fdwReason,
        LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        hInstance = hinstDLL;
    }

    return TRUE;
}
		
DWORD WINAPI WinHttpWebSocketClose( HINTERNET hsocket, USHORT status, void *reason, DWORD len ) {
    return WinHttpCloseHandle(hsocket);
}

DWORD WINAPI WinHttpWebSocketReceive(HINTERNET hWebSocket, PVOID pvBuffer, DWORD dwBufferLength, DWORD *pdwBytesRead, enum WINHTTP_WEB_SOCKET_BUFFER_TYPE *peBufferType) {
	return WinHttpReceiveResponse(hWebSocket, pvBuffer);
}

DWORD WINAPI WinHttpWebSocketSend(HINTERNET hWebSocket, enum WINHTTP_WEB_SOCKET_BUFFER_TYPE *eBufferType, PVOID pvBuffer, DWORD dwBufferLength) {
	return WinHttpSendRequest(hWebSocket, ((LPCWSTR)pvBuffer), dwBufferLength, pvBuffer, 0, 0, 0);	
}

HINTERNET WINAPI WinHttpWebSocketCompleteUpgrade(HINTERNET hRequest, DWORD_PTR pContext) {
	return WinHttpSetStatusCallback(hRequest, NULL, WINHTTP_CALLBACK_FLAG_SENDREQUEST_COMPLETE, 0);
}


/***********************************************************************
 *          WinHttpCreateProxyResolver (winhttp.@)
 */
DWORD WINAPI WinHttpCreateProxyResolver( HINTERNET hsession, HINTERNET *hresolver )
{
    FIXME("%p, %p\n", hsession, hresolver);
    return ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR;
}

/***********************************************************************
 *          WinHttpFreeProxyResult (winhttp.@)
 */
void WINAPI WinHttpFreeProxyResult( WINHTTP_PROXY_RESULT *result )
{
    FIXME("%p\n", result);
}


/***********************************************************************
 *          WinHttpGetProxyForUrlEx (winhttp.@)
 */
DWORD WINAPI WinHttpGetProxyForUrlEx( HINTERNET hresolver, const WCHAR *url, WINHTTP_AUTOPROXY_OPTIONS *options,
                                      DWORD_PTR ctx )
{
    FIXME( "%p, %s, %p, %Ix\n", hresolver, debugstr_w(url), options, ctx );
    return ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR;
}


/***********************************************************************
 *          WinHttpGetProxyResult (winhttp.@)
 */
DWORD WINAPI WinHttpGetProxyResult( HINTERNET hresolver, WINHTTP_PROXY_RESULT *result )
{
    FIXME("%p, %p\n", hresolver, result);
    return ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR;
}

DWORD WINAPI WinHttpWebSocketShutdown( HINTERNET hsocket, USHORT status, void *reason, DWORD len )
{
    return ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR;
}

WINHTTPAPI DWORD WinHttpWebSocketQueryCloseStatus(
  HINTERNET hWebSocket,
  USHORT    *pusStatus,
  PVOID     pvReason,
  DWORD     dwReasonLength,
  DWORD     *pdwReasonLengthConsumed
)
{
    return ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR;
}