/*++

Copyright (c) 2017 Shorthorn Project

Module Name:

    dllmain.c

Abstract:

    This module initialize Win32 API Base

Author:

    Skulltrail 20-March-2017

Revision History:

--*/


#include "main.h"

static BOOL DllInitialized = FALSE;
PPEB Peb;
HMODULE kernel32_handle = NULL;
ULONG BaseDllTag;

extern BOOL RegInitialize(VOID);
extern BOOL RegCleanup(VOID);
void InitializeCriticalForLocaleInfo();
void init_locale(void);

// Code from EAZY BLACK
NTSTATUS WINAPI BasepChromeSandboxWorkaround() {
	NTSTATUS Status;
    HANDLE BaseNamedObjectsHandle = NULL;
    HANDLE DirectoryHandle = NULL;
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING DirectoryName;
    UNICODE_STRING BaseNamedObjectsPath;
    SECURITY_DESCRIPTOR SecurityDescriptor;
	WCHAR BaseNamedObjectsWchar[40];
	WCHAR BaseNamedObjectsUntrustedWchar[40];
	
    BYTE UntrustedSidBuffer[] = {1, 1, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0};
    BYTE SaclBuffer[sizeof(UntrustedSidBuffer) + sizeof(ACL) + sizeof(ACE_HEADER) + sizeof(ACCESS_MASK)];
    PSID UntrustedSid;
    PACL Sacl;
	DWORD SessionId;
	
	DbgPrint("BasepChromeSandboxWorkaround called\n");
	
	if (!ProcessIdToSessionId(GetCurrentProcessId(), &SessionId))
        return HRESULT_FROM_WIN32(GetLastError());
    
    if (SessionId == 0) { // XP
        RtlInitUnicodeString(&BaseNamedObjectsPath, L"\\BaseNamedObjects"); // &BaseNamedObjectsWchar);
        RtlInitUnicodeString(&DirectoryName, L"\\BaseNamedObjects\\Untrusted"); //&BaseNamedObjectsUntrustedWchar);
    } else {
        swprintf(BaseNamedObjectsWchar, L"\\Sessions\\%i\\BaseNamedObjects", SessionId);
        swprintf(BaseNamedObjectsUntrustedWchar, L"\\Sessions\\%i\\BaseNamedObjects\\Untrusted", SessionId);
        RtlInitUnicodeString(&BaseNamedObjectsPath, BaseNamedObjectsWchar);
        RtlInitUnicodeString(&DirectoryName, BaseNamedObjectsUntrustedWchar);
    }
	
    //RtlInitUnicodeString(&BaseNamedObjectsPath, BaseNamedObjectsWchar);
    InitializeObjectAttributes(
        &ObjectAttributes,
        &BaseNamedObjectsPath,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
	
    Status = NtOpenDirectoryObject(
        &BaseNamedObjectsHandle,
        DIRECTORY_ALL_ACCESS & ~(STANDARD_RIGHTS_REQUIRED),
        &ObjectAttributes);
	
    if (!NT_SUCCESS(Status)) {
        DbgPrint("NtOpenDirectoryObject(%ws) failed: %i\n", &BaseNamedObjectsWchar, Status);
        return STATUS_SUCCESS;
    }
	
    //RtlInitUnicodeString(&DirectoryName, BaseNamedObjectsUntrustedWchar);
	
    UntrustedSid = (PSID)UntrustedSidBuffer;
    Sacl = (PACL)SaclBuffer;
    Status = RtlCreateAcl(Sacl, sizeof(SaclBuffer), ACL_REVISION);
	if (!NT_SUCCESS(Status)) goto end;
    Status = RtlAddMandatoryAce(Sacl, ACL_REVISION, 0, (ULONG)UntrustedSid, SYSTEM_MANDATORY_LABEL_ACE_TYPE, 0);
    if (!NT_SUCCESS(Status)) goto end;
    Status = RtlCreateSecurityDescriptor(&SecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
    if (!NT_SUCCESS(Status)) goto end;
    Status = RtlSetDaclSecurityDescriptor(&SecurityDescriptor, TRUE, NULL, FALSE);
    if (!NT_SUCCESS(Status)) goto end;
    Status = RtlSetSaclSecurityDescriptor(&SecurityDescriptor, TRUE, Sacl, FALSE);
    if (!NT_SUCCESS(Status)) goto end;
	
	InitializeObjectAttributes(
        &ObjectAttributes,
        &DirectoryName,
        OBJ_CASE_INSENSITIVE | OBJ_OPENIF,
        NULL,
        &SecurityDescriptor
    );

    Status = NtCreateDirectoryObject(
        &DirectoryHandle,
        DIRECTORY_ALL_ACCESS,
        &ObjectAttributes);
	
end:
    if (BaseNamedObjectsHandle != NULL)
        NtClose(BaseNamedObjectsHandle);
	
	return Status;
}

BOOL
WINAPI
BaseDllInitialize(
	HANDLE hDll,
    DWORD dwReason,
    LPVOID lpReserved)
{
	DWORD bufferSize = (MAX_PATH + 30) * sizeof(WCHAR); // MAX_PATH plus 30 characters to have room for all valid setups.
	LPWSTR EnvVar;
	NTSTATUS Status;

    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            /* Insert more dll attach stuff here! */
			kernel32_handle = GetModuleHandleW(L"kernel32");
			InitializeCriticalForLocaleInfo();
			
			//Initialize Locale
			init_locale();
			
			// The NT equalivent of DisableThreadLibraryCalls.() Remove this line when you add THREAD_ATTACH/THREAD_DETACH.
			LdrDisableThreadCalloutsForDll(hDll);
			
			EnvVar = (LPWSTR)HeapAlloc(GetProcessHeap(), 8, bufferSize);
			
			if (EnvVar) {
				// Inject Vista-only LocalAppData and ProgramData environment variables. We have to do this seperately
				// from AppData because some applications store the same files in AppData and LocalAppData.
				if (GetEnvironmentVariableW(L"LOCALAPPDATA", EnvVar, bufferSize) == 0
					|| EnvVar[0] == '\\' // old OCA versions don't put the \, overwrite them anyway.
				) {
					if (GetEnvironmentVariableW(L"USERPROFILE", EnvVar, bufferSize) > 0) {
						// This is the closest thing to LocalAppData.
						wcscat(EnvVar, L"\\Local Settings\\Application Data");
						SetEnvironmentVariableW(L"LOCALAPPDATA", EnvVar);
					}
				}
				
				if (GetEnvironmentVariableW(L"PROGRAMDATA", EnvVar, bufferSize) == 0
					|| EnvVar[0] == '%' // old OCA versions don't put it in the form C:\ProgramData so overwrite it.
				) {
					GetEnvironmentVariableW(L"SYSTEMDRIVE", EnvVar, bufferSize);
					wcscat(EnvVar, L"\\ProgramData");
					SetEnvironmentVariableW(L"PROGRAMDATA", EnvVar);
				}
				
				HeapFree(GetProcessHeap(), 0, EnvVar);
			}
			
            Status = BasepChromeSandboxWorkaround();
            if (!NT_SUCCESS(Status))
                DbgPrint("BasepChromeSandboxWorkaround failed with status %i\n", Status);
			
            DllInitialized = TRUE;				
            break;
        }
        case DLL_PROCESS_DETACH:
        {
			DllInitialized = FALSE;
            break;
        }
        default:
            break;
    }

    return TRUE;
}

/***********************************************************************
 *          QuirkIsEnabled3 (kernelex.@)
 */
BOOL WINAPI QuirkIsEnabled3(void *unk1, void *unk2)
{
    static int once;

    if (!once++)
        DbgPrint("(%p, %p) stub!\n", unk1, unk2);
	
    return FALSE;
}