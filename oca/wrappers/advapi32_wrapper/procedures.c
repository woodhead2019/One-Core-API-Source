/*++

Copyright (c) 2026  Shorthorn Project

Module Name:

    hooks.c

Abstract:

    Function to get Addresses of native procedures
	
Author:

    Skulltrail 28-January-2026

Revision History:

--*/

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(procedures);

static HMODULE ghAdvapi32 = NULL;

/* Initialize pointers using GetModuleHandleW (no LoadLibrary) */
BOOL InitNativeProcs(void)
{
    HMODULE h;
    if (ghAdvapi32) return TRUE; /* jÃ¡ inicializado */

    h = GetModuleHandleW(L"advapibase.dll");
    if (!h) return FALSE;
    ghAdvapi32 = h;
	
    pAddMandatoryAce             = (void*)GetProcAddress(ghAdvapi32, "AddMandatoryAce");
    pCredFindBestCredentialA     = (void*)GetProcAddress(ghAdvapi32, "CredFindBestCredentialA");
    pCredFindBestCredentialW     = (void*)GetProcAddress(ghAdvapi32, "CredFindBestCredentialW");
    pCredIsProtectedA            = (void*)GetProcAddress(ghAdvapi32, "CredIsProtectedA");
    pCredIsProtectedW            = (void*)GetProcAddress(ghAdvapi32, "CredIsProtectedW");
    pCredProtectA                = (void*)GetProcAddress(ghAdvapi32, "CredProtectA");
    pCredProtectW                = (void*)GetProcAddress(ghAdvapi32, "CredProtectW");
    pCredUnprotectA              = (void*)GetProcAddress(ghAdvapi32, "CredUnprotectA");
    pCredUnprotectW              = (void*)GetProcAddress(ghAdvapi32, "CredUnprotectW");	
    pCloseThreadWaitChainSession = (void*)GetProcAddress(ghAdvapi32, "CloseThreadWaitChainSession");	
    pEventRegister               = (void*)GetProcAddress(ghAdvapi32, "EventRegister");
    pEventUnregister             = (void*)GetProcAddress(ghAdvapi32, "EventUnregister");
    pEventWrite                  = (void*)GetProcAddress(ghAdvapi32, "EventWrite");
    pEventWriteTransfer          = (void*)GetProcAddress(ghAdvapi32, "EventWriteTransfer");
    pEventWriteString            = (void*)GetProcAddress(ghAdvapi32, "EventWriteString");
    pEventActivityIdControl      = (void*)GetProcAddress(ghAdvapi32, "EventActivityIdControl");
    pEventEnabled                = (void*)GetProcAddress(ghAdvapi32, "EventEnabled");
    pEventProviderEnabled        = (void*)GetProcAddress(ghAdvapi32, "EventProviderEnabled");
    pEventAccessControl          = (void*)GetProcAddress(ghAdvapi32, "EventAccessControl");
    pEventWriteEx                = (void*)GetProcAddress(ghAdvapi32, "EventWriteEx");
    pEventWriteStartScenario     = (void*)GetProcAddress(ghAdvapi32, "EventWriteStartScenario");
    pEventWriteEndScenario       = (void*)GetProcAddress(ghAdvapi32, "EventWriteEndScenario");
    pEnableTraceEx               = (void*)GetProcAddress(ghAdvapi32, "EnableTraceEx");
    pEnableTraceEx2               = (void*)GetProcAddress(ghAdvapi32, "EnableTraceEx2");

    // /* getprocaddress - use exact exported names */
    // pRegGetValueW = (void*) GetProcAddress(ghAdvapi32, "RegGetValueW");
    // pRegNotifyChangeKeyValue = (void*) GetProcAddress(ghAdvapi32, "RegNotifyChangeKeyValue");
    // pConvertStringSecurityDescriptorToSecurityDescriptorW = (void*) GetProcAddress(ghAdvapi32, "ConvertStringSecurityDescriptorToSecurityDescriptorW");
    // pSetNamedSecurityInfoW = (void*) GetProcAddress(ghAdvapi32, "SetNamedSecurityInfoW");
    // pSetSecurityInfo = (void*) GetProcAddress(ghAdvapi32, "SetSecurityInfo");
    // pGetSecurityInfo = (void*) GetProcAddress(ghAdvapi32, "GetSecurityInfo");
    // pGetNamedSecurityInfoW = (void*) GetProcAddress(ghAdvapi32, "GetNamedSecurityInfoW");

    /* It's OK if some pointers are NULL (fallbacks in code may handle) */
    return TRUE;
}