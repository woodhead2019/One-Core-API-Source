/*++

Copyright (c) 2024  Shorthorn Project

Module Name:

    hooks.c

Abstract:

    Hook native functions 

Author:

    Skulltrail 20-September-2024

Revision History:

--*/

#include <wine/config.h>

#include <ntstatus.h>
#define WIN32_NO_STATUS

#include <wine/debug.h>

#include <windef.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <winnls.h>
#include <windowsx.h>
#include <undocuser.h>
#include <uxtheme.h>

#define uxthemeName L"uxtheme.dll"

typedef HTHEME (WINAPI *PFNOPENTHEMEDATA)(
	HWND  hWndParent,
	LPCWSTR hInstance
);

HTHEME 
WINAPI 
OpenThemeDataInternal(
  HWND    hwnd,
  LPCWSTR pszClassList
)
{
    PFNOPENTHEMEDATA pOpenThemeDataNative = NULL;
    HMODULE hMod = NULL;
	
	hMod = GetModuleHandleW(uxthemeName);
		
    if (!hMod)
        hMod = LoadLibraryW(uxthemeName);
	
	if(!hMod)
		return NULL;

    if (hMod)
        pOpenThemeDataNative = (PFNOPENTHEMEDATA)GetProcAddress(hMod, "OpenThemeDataNative");

    if (!pOpenThemeDataNative)
        return NULL; /* não encontrado */
	
    if(pszClassList != NULL && wcscmp(pszClassList, L"TASKDIALOG") == 0){
        return pOpenThemeDataNative(hwnd, L"HEADER");
    }
	
	return pOpenThemeDataNative(hwnd, pszClassList);
}