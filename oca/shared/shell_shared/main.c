/*++

Copyright (c) 2025  Shorthorn Project

Module Name:

    main.c

Abstract:

    This module main functions for Shell. 

Author:

    Skulltrail 31-October-2025

Revision History:

--*/

#include "main.h"
 
WINE_DEFAULT_DEBUG_CHANNEL(shell);

HINSTANCE shell32_hInstance = 0;

WNDPROC lpPrevWndFunc;

// HRESULT GetDllGetClassObjectProc(){
    // HMODULE hShell32 = NULL;	
	
	// if (pfnDllGetClassObjectNative == NULL) {
		// hShell32 = GetModuleHandleW(shellName);
		// if (!hShell32)
			// hShell32 = LoadLibraryW(shellName);

		// if (hShell32) {
			// FARPROC proc;
			// proc = GetProcAddress(hShell32, "DllGetClassObjectNative");
			// if (proc)
				// pfnDllGetClassObjectNative = (PFN_DllGetClassObject_Native)proc;
			// else {
				// /* fallback: função padrão */
				// proc = GetProcAddress(hShell32, "DllGetClassObject");
				// if (proc)
					// pfnDllGetClassObjectNative = (PFN_DllGetClassObject_Native)proc;
			// }
		// }

		// if (pfnDllGetClassObjectNative == NULL) {
			// OutputDebugStringW(L"DllGetClassObject: não encontrou função nativa em shell32.dll\n");
			// return CLASS_E_CLASSNOTAVAILABLE;
		// }				
				
	// }
	
	// return S_OK;
// }

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpv)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
			shell32_hInstance = hInstDLL;
			//GetDllGetClassObjectProc();
			DisableThreadLibraryCalls(shell32_hInstance);
            break;
    }

    return TRUE;
}

/************************************************************************/

BOOL WINAPI StrRetToStrNW(LPWSTR dest, DWORD len, LPSTRRET src, const ITEMIDLIST *pidl)
{

    if (!dest)
        return FALSE;

    switch (src->uType)
    {
        case STRRET_WSTR:
            lstrcpynW(dest, src->pOleStr, len);
            CoTaskMemFree(src->pOleStr);
            break;
        case STRRET_CSTR:
            if (!MultiByteToWideChar(CP_ACP, 0, src->cStr, -1, dest, len) && len)
                dest[len-1] = 0;
            break;
        case STRRET_OFFSET:
            if (!MultiByteToWideChar(CP_ACP, 0, ((LPCSTR)&pidl->mkid)+src->uOffset, -1, dest, len)
                    && len)
                dest[len-1] = 0;
            break;
        default:
            FIXME("unknown type %u!\n", src->uType);
            if (len)
                *dest = '\0';
            return FALSE;
    }
    return TRUE;
}

LRESULT 
WINAPI 
NotificationWindowCallback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  LRESULT result; // eax@2
  unsigned __int16 cursorPosition; // [sp+0h] [bp-Ch]@7 MAPDST

  if ( Msg == 1024 )
  {
    if ( lParam == 516 )
    {
      GetCursorPos((LPPOINT)&cursorPosition);
      result = CallWindowProcW(lpPrevWndFunc, hWnd, 0x400u, cursorPosition | (cursorPosition << 16), 123);
    }
    else if ( lParam == 517 )
    {
      result = 0;
    }
    else
    {
      result = CallWindowProcW(lpPrevWndFunc, hWnd, 0x400u, wParam, lParam);
    }
  }
  else
  {
    result = DefWindowProcW(hWnd, Msg, wParam, lParam);
  }
  return result;
}

HWND globalWindow;
HWND hWnd;

signed int  Internal_Shell_NotifyIcon(PNOTIFYICONDATA lpdata, DWORD dwMessage, __int16 flags)
{
  void *allocation; // edi@1
  HWND findWindow; // eax@7
  HWND otherWindow; // ST28_4@14
  DWORD localMessage; // [sp-8h] [bp-38h]@5
  struct _NOTIFYICONDATAA *lpDataPointer; // [sp-4h] [bp-34h]@3
  WNDCLASSW WndClass; // [sp+8h] [bp-28h]@14

  allocation = malloc(0x10u);
  wsprintfW((LPWSTR)allocation, L"%x", lpdata->hWnd);
  if ( dwMessage )
  {
    if ( dwMessage != 2 )
    {
      free(allocation);
      lpdata->hWnd = globalWindow;
      lpDataPointer = (struct _NOTIFYICONDATAA *)lpdata;
      if ( flags != 1 )
        return Shell_NotifyIconW(dwMessage, (PNOTIFYICONDATAW)lpdata);
      localMessage = dwMessage;
      return Shell_NotifyIconA(localMessage, lpDataPointer);
    }
    findWindow = FindWindowW((LPCWSTR)allocation, 0);
    globalWindow = findWindow;
    if ( findWindow )
    {
      DestroyWindow(findWindow);
      findWindow = globalWindow;
    }
    lpdata->hWnd = findWindow;
    free(allocation);
    lpDataPointer = (struct _NOTIFYICONDATAA *)lpdata;
    localMessage = 2;
  }
  else
  {
    if ( hWnd || lpPrevWndFunc )
    {
      free(allocation);
      return 183;
    }
    otherWindow = lpdata->hWnd;
    WndClass.hInstance = (HINSTANCE)268435456;
    WndClass.lpszClassName = (LPCWSTR)allocation;
    WndClass.lpfnWndProc = NotificationWindowCallback;
    WndClass.style = 0;
    WndClass.hIcon = 0;
    WndClass.hCursor = 0;
    WndClass.lpszMenuName = 0;
    WndClass.hbrBackground = 0;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    lpPrevWndFunc = (WNDPROC)GetWindowLongW(otherWindow, -4);
    hWnd = lpdata->hWnd;
    RegisterClassW(&WndClass);
    globalWindow = CreateWindowExW(0, (LPCWSTR)allocation, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, (HINSTANCE)0x10000000, 0);
    lpdata->hWnd = globalWindow;
    free(allocation);
    lpDataPointer = (struct _NOTIFYICONDATAA *)lpdata;
    localMessage = 0;
  }
  if ( flags == 1 )
    return Shell_NotifyIconA(localMessage, lpDataPointer);
  return Shell_NotifyIconW(localMessage, (PNOTIFYICONDATAW)lpDataPointer);
}

BOOL __stdcall Shell_NotifyIconInternal(DWORD dwMessage, PNOTIFYICONDATA lpdata)
{
  return Internal_Shell_NotifyIcon(lpdata, dwMessage, 0);
}

BOOL __stdcall Shell_NotifyIconInternalA(DWORD dwMessage, PNOTIFYICONDATAA lpData)
{
  return Internal_Shell_NotifyIcon((PNOTIFYICONDATA)lpData, dwMessage, 1);
}

BOOL __stdcall Shell_NotifyIconInternalW(DWORD dwMessage, PNOTIFYICONDATAW lpData)
{
  return Internal_Shell_NotifyIcon((PNOTIFYICONDATA)lpData, dwMessage, 2);
}