/*++

Copyright (c) 2025 Shorthorn Project

Module Name:

    main.c

Abstract:

    Implement DirectWrite wrapper functions

Author:

    Skulltrail 11-September-2025

Revision History:

--*/

#define COBJMACROS

#include <stdarg.h>
#include <math.h>

#include "windef.h"
#include "winbase.h"
#include "winuser.h"

#include "initguid.h"

#include "dwrite_private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(dwrite);

HMODULE dwrite_module = 0;
static IDWriteFactory7 *shared_factory;
static void release_shared_factory(IDWriteFactory7 *factory);

#define NtCurrentPeb() (NtCurrentTeb()->Peb)

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        dwrite_module = hinstDLL;
        DisableThreadLibraryCalls( hinstDLL );
        break;
    case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}

typedef HRESULT (WINAPI *DirectWriteCreation)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);

BOOL HasExportedFunction(LPCWSTR filePath, LPCSTR targetFunction) {
    HANDLE hFile;
    HANDLE hMapping;
    LPVOID baseAddress;
    PIMAGE_NT_HEADERS nt_headers;
    ULONG exportSize;
    PIMAGE_EXPORT_DIRECTORY exportDir;
    DWORD *nameRVAs;
    DWORD i;
    PIMAGE_DOS_HEADER dos_header;
    WORD machine;

    // Verifica extensão ".exe"
    {
        const WCHAR *ext = wcsrchr(filePath, L'.');
        if (!ext || lstrcmpiW(ext, L".exe") != 0) {
            return FALSE;
        }
    }

    hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    hMapping = CreateFileMappingW(hFile, NULL, PAGE_READONLY | SEC_IMAGE, 0, 0, NULL);
    if (!hMapping) {
        CloseHandle(hFile);
        return FALSE;
    }

    baseAddress = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!baseAddress) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return FALSE;
    }

    // Verifica assinatura do DOS Header
    dos_header = (PIMAGE_DOS_HEADER)baseAddress;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        goto cleanup;
    }

    // Verifica assinatura do NT Header
    nt_headers = (PIMAGE_NT_HEADERS)RtlImageNtHeader(baseAddress);
    if (!nt_headers || nt_headers->Signature != IMAGE_NT_SIGNATURE) {
        goto cleanup;
    }

    machine = nt_headers->FileHeader.Machine;
    if (machine != IMAGE_FILE_MACHINE_I386 && machine != IMAGE_FILE_MACHINE_AMD64) {
        goto cleanup;
    }

    exportSize = 0;
    exportDir = (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(
        baseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &exportSize);

    if (!exportDir || exportSize == 0) {
        goto cleanup;
    }

    nameRVAs = (DWORD *)((BYTE *)baseAddress + exportDir->AddressOfNames);
    for (i = 0; i < exportDir->NumberOfNames; i++) {
        char *funcName = (char *)((BYTE *)baseAddress + nameRVAs[i]);
        if (lstrcmpiA(funcName, targetFunction) == 0) {
            UnmapViewOfFile(baseAddress);
            CloseHandle(hMapping);
            CloseHandle(hFile);
            return TRUE;
        }
    }

cleanup:
    UnmapViewOfFile(baseAddress);
    CloseHandle(hMapping);
    CloseHandle(hFile);
    return FALSE;
}

BOOL ShouldUseDWriteCore() {
	PPEB Peb;
	LPCWSTR lpApplicationName;
	
	Peb = NtCurrentPeb();	
	
	lpApplicationName = Peb->ProcessParameters->ImagePathName.Buffer;
	
    if (!IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE)) {
        // A system without SSE2 can not use DirectWriteCore.
        return FALSE;
    }

    if (GetModuleHandleW(L"Qt6Core.dll")) {
        // Applications using Qt 6.8 or higher requires Windows 10 DirectWrite interfaces, or everything will appear as a block.
        return TRUE;
    }
    
	if(HasExportedFunction(lpApplicationName, "IsSandboxedProcess") && HasExportedFunction(lpApplicationName, "GetHandleVerifier") && !HasExportedFunction(lpApplicationName, "g_originals")){
		return TRUE;
	}
    
    return FALSE;
}

HRESULT 
WINAPI 
DWriteCreateFactory(DWRITE_FACTORY_TYPE factoryType, REFIID iid, IUnknown **factory) {
	LPCWSTR dllName;
	HANDLE hnd;
	DirectWriteCreation api;
    
	if(ShouldUseDWriteCore()){
		// Load dwritecore.dll, which will be used everywhere else.
		dllName = L"dwritecore.dll";
	}else{
		dllName = L"dwrgdi.dll";
	}

	// Try load the DLL.
	hnd = GetModuleHandleW(dllName);
	if(!hnd){
		hnd = LoadLibraryW(dllName);
	}
	
	if (!hnd) 
		return HRESULT_FROM_WIN32(GetLastError());

	api = (DirectWriteCreation) GetProcAddress(hnd, "DWriteCreateFactory");
	if (!api)
		api = (DirectWriteCreation) GetProcAddress(hnd, "DWriteCoreCreateFactory"); // Try DWriteCore instead.

	if (!api) {
		FreeLibrary(hnd);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	return api(factoryType, iid, factory);
}