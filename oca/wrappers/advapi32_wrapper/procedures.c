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
    if (ghAdvapi32)
        return TRUE;

    ghAdvapi32 = GetModuleHandleW(L"advapibase.dll");
    if (!ghAdvapi32)
        ghAdvapi32 = GetModuleHandleW(L"advapi32.dll");

    if (!ghAdvapi32)
        return FALSE;
	
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
    pEnableTraceEx2              = (void*)GetProcAddress(ghAdvapi32, "EnableTraceEx2");
    pGetThreadWaitChain  		 = (void*)GetProcAddress(ghAdvapi32, "GetThreadWaitChain");	
    pInitiateShutdownA 			 = (void*)GetProcAddress(ghAdvapi32, "InitiateShutdownA");
    pInitiateShutdownW 			 = (void*)GetProcAddress(ghAdvapi32, "InitiateShutdownW");
    pNotifyServiceStatusChangeA  = (void*)GetProcAddress(ghAdvapi32, "NotifyServiceStatusChangeA");
    pNotifyServiceStatusChangeW  = (void*)GetProcAddress(ghAdvapi32, "NotifyServiceStatusChangeW");
    pOpenThreadWaitChainSession  = (void*)GetProcAddress(ghAdvapi32, "OpenThreadWaitChainSession");	
    pPerfOpenQueryHandle = (void*)GetProcAddress(ghAdvapi32, "PerfOpenQueryHandle");
    pPerfCloseQueryHandle = (void*)GetProcAddress(ghAdvapi32, "PerfCloseQueryHandle");

    pPerfAddCounters = (void*)GetProcAddress(ghAdvapi32, "PerfAddCounters");
    pPerfDeleteCounters = (void*)GetProcAddress(ghAdvapi32, "PerfDeleteCounters");

    pPerfCreateInstance = (void*)GetProcAddress(ghAdvapi32, "PerfCreateInstance");
    pPerfDeleteInstance = (void*)GetProcAddress(ghAdvapi32, "PerfDeleteInstance");

    pPerfEnumerateCounterSet = (void*)GetProcAddress(ghAdvapi32, "PerfEnumerateCounterSet");
    pPerfEnumerateCounterSetInstances =
        (void*)GetProcAddress(ghAdvapi32, "PerfEnumerateCounterSetInstances");

    pPerfQueryCounterData = (void*)GetProcAddress(ghAdvapi32, "PerfQueryCounterData");
    pPerfQueryCounterInfo = (void*)GetProcAddress(ghAdvapi32, "PerfQueryCounterInfo");
    pPerfQueryInstance = (void*)GetProcAddress(ghAdvapi32, "PerfQueryInstance");

    pPerfQueryCounterSetRegistrationInfo =
        (void*)GetProcAddress(ghAdvapi32, "PerfQueryCounterSetRegistrationInfo");

    pPerfSetCounterRefValue =
        (void*)GetProcAddress(ghAdvapi32, "PerfSetCounterRefValue");
    pPerfSetCounterSetInfo =
        (void*)GetProcAddress(ghAdvapi32, "PerfSetCounterSetInfo");

    pPerfSetULongCounterValue =
        (void*)GetProcAddress(ghAdvapi32, "PerfSetULongCounterValue");
    pPerfSetULongLongCounterValue =
        (void*)GetProcAddress(ghAdvapi32, "PerfSetULongLongCounterValue");

    pPerfIncrementULongCounterValue =
        (void*)GetProcAddress(ghAdvapi32, "PerfIncrementULongCounterValue");
    pPerfIncrementULongLongCounterValue =
        (void*)GetProcAddress(ghAdvapi32, "PerfIncrementULongLongCounterValue");

    pPerfDecrementULongCounterValue =
        (void*)GetProcAddress(ghAdvapi32, "PerfDecrementULongCounterValue");
    pPerfDecrementULongLongCounterValue =
        (void*)GetProcAddress(ghAdvapi32, "PerfDecrementULongLongCounterValue");
    pPerfStartProvider =
        (void*)GetProcAddress(ghAdvapi32, "PerfStartProvider");
    pPerfStartProviderEx =
        (void*)GetProcAddress(ghAdvapi32, "PerfStartProviderEx");
    pPerfStopProvider =
        (void*)GetProcAddress(ghAdvapi32, "PerfStopProvider");	
		
    pProcessIdleTasksW =
        (void*)GetProcAddress(ghAdvapi32, "ProcessIdleTasksW");	

    pQuerySecurityAccessMask      = (void*)GetProcAddress(ghAdvapi32, "QuerySecurityAccessMask");
    pSetSecurityAccessMask        = (void*)GetProcAddress(ghAdvapi32, "SetSecurityAccessMask");
    pSetUserFileEncryptionKeyEx   = (void*)GetProcAddress(ghAdvapi32, "SetUserFileEncryptionKeyEx");
    pTreeSetNamedSecurityInfoW    = (void*)GetProcAddress(ghAdvapi32, "TreeSetNamedSecurityInfoW");

    pRegCopyTreeA                 = (void*)GetProcAddress(ghAdvapi32, "RegCopyTreeA");
    pRegCopyTreeW                 = (void*)GetProcAddress(ghAdvapi32, "RegCopyTreeW");
    pRegDeleteTreeA               = (void*)GetProcAddress(ghAdvapi32, "RegDeleteTreeA");
    pRegDeleteTreeW               = (void*)GetProcAddress(ghAdvapi32, "RegDeleteTreeW");
    pRegDeleteKeyValueA           = (void*)GetProcAddress(ghAdvapi32, "RegDeleteKeyValueA");
    pRegDeleteKeyValueW           = (void*)GetProcAddress(ghAdvapi32, "RegDeleteKeyValueW");
    pRegSetKeyValueA              = (void*)GetProcAddress(ghAdvapi32, "RegSetKeyValueA");
    pRegSetKeyValueW              = (void*)GetProcAddress(ghAdvapi32, "RegSetKeyValueW");
    pRegRenameKey                 = (void*)GetProcAddress(ghAdvapi32, "RegRenameKey");
    pRegLoadAppKeyA               = (void*)GetProcAddress(ghAdvapi32, "RegLoadAppKeyA");
    pRegLoadAppKeyW               = (void*)GetProcAddress(ghAdvapi32, "RegLoadAppKeyW");
    pRegLoadMUIStringA            = (void*)GetProcAddress(ghAdvapi32, "RegLoadMUIStringA");
    pRegLoadMUIStringW            = (void*)GetProcAddress(ghAdvapi32, "RegLoadMUIStringW");

    pRegCreateKeyTransactedA      = (void*)GetProcAddress(ghAdvapi32, "RegCreateKeyTransactedA");
    pRegCreateKeyTransactedW      = (void*)GetProcAddress(ghAdvapi32, "RegCreateKeyTransactedW");
    pRegOpenKeyTransactedA        = (void*)GetProcAddress(ghAdvapi32, "RegOpenKeyTransactedA");
    pRegOpenKeyTransactedW        = (void*)GetProcAddress(ghAdvapi32, "RegOpenKeyTransactedW");
    pRegDeleteKeyTransactedA      = (void*)GetProcAddress(ghAdvapi32, "RegDeleteKeyTransactedA");
    pRegDeleteKeyTransactedW      = (void*)GetProcAddress(ghAdvapi32, "RegDeleteKeyTransactedW");		

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