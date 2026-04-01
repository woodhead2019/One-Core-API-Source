/*
 * Copyright 2009 Henri Verbeet for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#include <wine/config.h>

#include <ntstatus.h>
#define WIN32_NO_STATUS

#include <wine/debug.h>
#include <wincred.h>
#include <winbase.h>
#include <secext.h>
#include <security.h>
#include <stdio.h>
#include <windows.h>
#include <ntstatus.h>
#include <ntsecapi.h>
#include <wincred.h>
#include <winnetwk.h>
#include <rtlfuncs.h>

// You can disable the warning for the function.
#pragma warning( push )
#pragma warning( disable : 4047 )

DWORD SecTlsIP;
DWORD state;
state = 0;
SecTlsIP = 0;

WINE_DEFAULT_DEBUG_CHANNEL(bcrypt);

//
// Possible forms of the username returned from CredMan
//

typedef enum _CredParsedUserNameType
{
    parsedUsernameInvalid = 0,
    parsedUsernameUpn,
    parsedUsernameNt4Style,
    parsedUsernameCertificate,
    parsedUsernameNonQualified
} CredParsedUserNameType;

typedef SECURITY_STATUS (WINAPI *SASL_GET_CONTEXT_OPTION)(
    PCtxtHandle,
    ULONG,
    PVOID,
    ULONG,
    PULONG);

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpv)
{
    TRACE("fdwReason %u\n", fdwReason);

    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hInstDLL);
            break;
    }

    return TRUE;
}

SECURITY_STATUS WINAPI SecpSetCallFlags(int a1)
{
	SECURITY_STATUS result; // eax@3
    result = 0x80090300u;
	return result;
}

SECURITY_STATUS WINAPI SecpSetIPAddress(const void *ipNumber, ULONG number)
{
  PVOID receive; // edi@3
  SECURITY_STATUS result; // eax@4
  HLOCAL local; // eax@6

  if ( state & 0x20 || number > 0x20 )
  {
    result = 0x80090300u;
  }
  else
  {
    receive = TlsGetValue(SecTlsIP);
    if ( receive )
    {
LABEL_4:
      memcpy(receive, ipNumber, number);
      return 0;
    }
    local = LocalAlloc(0x40u, 0x20u);
    receive = local;
    if ( local )
    {
      TlsSetValue(SecTlsIP, local);
      goto LABEL_4;
    }
    result = 0x80090300u;
  }
  return result;
}


static
SECURITY_STATUS
SaslGetContextOption(
    PCtxtHandle ContextHandle,
    ULONG Option,
    PVOID Value,
    ULONG Size,
    PULONG Needed OPTIONAL
    )
{
	SASL_GET_CONTEXT_OPTION saslGetContextOption;
	
    saslGetContextOption = (SASL_GET_CONTEXT_OPTION) GetProcAddress(
                            GetModuleHandle(TEXT("securbase")),
                            "SaslGetContextOption");
    if (NULL == saslGetContextOption) 
    {
		return S_FALSE;
	}else{
		return saslGetContextOption(ContextHandle,
									Option,
									Value,
									Size,
									Needed);
	}	    
}

NTSTATUS
WINAPI
I_NetNameCanonicalize(
    IN  LPTSTR  ServerName OPTIONAL,
    IN  LPTSTR  Name,
    OUT LPTSTR  Outbuf,
    IN  DWORD   OutbufLen,
    IN  DWORD   NameType,
    IN  DWORD   Flags
    );

BOOL NetpIsUserNameValid(LPWSTR *UserName)
{
  LPTSTR Outbuf[129]; // [esp+0h] [ebp-208h] BYREF

  return UserName
      && *UserName
      && I_NetNameCanonicalize(0, UserName, Outbuf, 0x202u, 1u, 0) == 0;
}

NTSTATUS WINAPI CredParseUserNameWithType(
        LPWSTR *UserName,
        PUNICODE_STRING pUserName,
        PUNICODE_STRING pDomainName,
        DWORD *pParseType)
{
    LPWSTR DomainName = NULL;
    DWORD ParsedType;
    CRED_MARSHAL_TYPE MarshaledCredential;
    PVOID IgnoredCredential = NULL;
    
    if (!UserName || wcslen(UserName) > CREDUI_MAX_USERNAME_LENGTH)
        return STATUS_INVALID_ACCOUNT_NAME;
    
    DomainName = wcsrchr(UserName, L'\\');
    if (DomainName) {
        *DomainName = L'\0';
        DomainName++;
        ParsedType = 2;
        goto Finish;
    }
    
    if (wcsrchr(UserName, L'@')) {
        PWSTR Terminator = UserName;
        ParsedType = 1;
        
        while (*Terminator != 0)
            Terminator++;
        
        if ((Terminator - UserName) >= 4) {
            if (UserName[0] == L'@' && UserName[1] == L'@') {
                Terminator = wcsrchr(UserName + 2, L'@');
                if (Terminator != NULL) {
                    *Terminator = L'\0';
                    DomainName = Terminator + 1;
                }
            }
        }        
        
        goto Finish;
    }
    
    if (!NetpIsUserNameValid(UserName))
        return STATUS_INVALID_ACCOUNT_NAME;
    
    ParsedType = 4;
Finish:
    if (CredUnmarshalCredentialW(UserName, &MarshaledCredential, &IgnoredCredential)) {
        if (MarshaledCredential == CertCredential)
            ParsedType = 3;
        if (IgnoredCredential)
            CredFree(IgnoredCredential);
    }
    
    if (pUserName)
        RtlInitUnicodeString(pUserName, UserName);
    if (pDomainName)
        RtlInitUnicodeString(pDomainName, DomainName);
    
    *pParseType = ParsedType;
    return STATUS_SUCCESS;
}