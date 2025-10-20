/*++

Copyright (c) 2017 Shorthorn Project

Module Name:

    console.c

Abstract:

    This module implements Win32 console functions

Author:

    Skulltrail 30-April-2017

Revision History:

--*/

#include "main.h"
#include "csrss.h"
#include <conmsg.h>

BOOL
WINAPI
DECLSPEC_HOTPATCH
SetConsoleFont(IN HANDLE hConsoleOutput,
               IN DWORD nFont);

// /*--------------------------------------------------------------
 // *  SetConsoleHistoryInfo
 // *
 // * @implemented
 // */
// BOOL
// WINAPI
// SetConsoleHistoryInfo(
	// IN PCONSOLE_HISTORY_INFO lpConsoleHistoryInfo
// )
// {
    // CSR_API_MESSAGE_KERNEL Request;
    // ULONG CsrRequest = MAKE_CSR_API(GET_HISTORY_INFO, CSR_CONSOLE);
    // NTSTATUS Status;
    // if (lpConsoleHistoryInfo->cbSize != sizeof(CONSOLE_HISTORY_INFO))
    // {
        // SetLastError(ERROR_INVALID_PARAMETER);
        // return FALSE;
    // }
    // Request.Data.SetHistoryInfo.HistoryBufferSize      = lpConsoleHistoryInfo->HistoryBufferSize;
    // Request.Data.SetHistoryInfo.NumberOfHistoryBuffers = lpConsoleHistoryInfo->NumberOfHistoryBuffers;
    // Request.Data.SetHistoryInfo.dwFlags                = lpConsoleHistoryInfo->dwFlags;
    // Status = CsrClientCallServer((PCSR_API_MESSAGE)&Request, NULL, CsrRequest, sizeof(CSR_API_MESSAGE));
    // if (!NT_SUCCESS(Status) || !NT_SUCCESS(Status = Request.Status))
    // {
        // SetLastError(Status);
        // return FALSE;
    // }
    // return TRUE;
// }

// /*--------------------------------------------------------------
 // *  GetConsoleHistoryInfo
 // *
 // * @implemented - new
 // */
// BOOL
// WINAPI
// GetConsoleHistoryInfo(
	// PCONSOLE_HISTORY_INFO lpConsoleHistoryInfo
// )
// {
    // CSR_API_MESSAGE_KERNEL Request;
    // ULONG CsrRequest = MAKE_CSR_API(GET_HISTORY_INFO, CSR_CONSOLE);
    // NTSTATUS Status;
    // if (lpConsoleHistoryInfo->cbSize != sizeof(CONSOLE_HISTORY_INFO))
    // {
        // SetLastError(ERROR_INVALID_PARAMETER);
        // return FALSE;
    // }
    // Status = CsrClientCallServer((PCSR_API_MESSAGE)&Request, NULL, CsrRequest, sizeof(CSR_API_MESSAGE));
    // if (!NT_SUCCESS(Status) || !NT_SUCCESS(Status = Request.Status))
    // {
        // SetLastError(Status);
        // return FALSE;
    // }
    // lpConsoleHistoryInfo->HistoryBufferSize      = Request.Data.GetHistoryInfo.HistoryBufferSize;
    // lpConsoleHistoryInfo->NumberOfHistoryBuffers = Request.Data.GetHistoryInfo.NumberOfHistoryBuffers;
    // lpConsoleHistoryInfo->dwFlags                = Request.Data.GetHistoryInfo.dwFlags;
    // return TRUE;
// }

/*--------------------------------------------------------------
 *  SetConsoleHistoryInfo - To Implement on winsrv
 *
 * @unimplemented - Requires csrsrv support to implement, which does not exist in NT5-compatible csrsrv
 */
BOOL
WINAPI
SetConsoleHistoryInfo(
    IN PCONSOLE_HISTORY_INFO lpConsoleHistoryInfo
)
{
    if (!lpConsoleHistoryInfo || lpConsoleHistoryInfo->cbSize != sizeof(CONSOLE_HISTORY_INFO))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return TRUE;
}

/*--------------------------------------------------------------
 *  GetConsoleHistoryInfo - To Implement on winsrv
 *
 * @unimplemented - Requires csrsrv support to implement, which does not exist in NT5-compatible csrsrv
 */
BOOL
WINAPI
GetConsoleHistoryInfo(
    PCONSOLE_HISTORY_INFO lpConsoleHistoryInfo
)
{
    if (!lpConsoleHistoryInfo || lpConsoleHistoryInfo->cbSize != sizeof(CONSOLE_HISTORY_INFO))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    lpConsoleHistoryInfo->HistoryBufferSize      = 0;
    lpConsoleHistoryInfo->NumberOfHistoryBuffers = 0;
    lpConsoleHistoryInfo->dwFlags                = 0;
    return TRUE;
}

BOOL 
WINAPI 
GetConsoleScreenBufferInfoEx(
    HANDLE hConsoleOutput, 
    CONSOLE_SCREEN_BUFFER_INFOEX *ConsoleScreenBufferInfoEx
)
{
  CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo; 
  BOOL isLonghorn;
  if (!ConsoleScreenBufferInfoEx) {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  isLonghorn = ConsoleScreenBufferInfoEx->cbSize < sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

  if (!GetConsoleScreenBufferInfo(hConsoleOutput, &ConsoleScreenBufferInfo))
    return FALSE;
  
  ConsoleScreenBufferInfoEx->dwSize.X = ConsoleScreenBufferInfo.dwSize.X;
  ConsoleScreenBufferInfoEx->dwSize.Y = ConsoleScreenBufferInfo.dwSize.Y;
  ConsoleScreenBufferInfoEx->dwMaximumWindowSize.X = ConsoleScreenBufferInfo.dwMaximumWindowSize.X;
  ConsoleScreenBufferInfoEx->dwMaximumWindowSize.Y = ConsoleScreenBufferInfo.dwMaximumWindowSize.Y;
  ConsoleScreenBufferInfoEx->dwCursorPosition.X = ConsoleScreenBufferInfo.dwCursorPosition.X;
  ConsoleScreenBufferInfoEx->dwCursorPosition.Y = ConsoleScreenBufferInfo.dwCursorPosition.Y;
  ConsoleScreenBufferInfoEx->srWindow.Left = ConsoleScreenBufferInfo.srWindow.Left;
  ConsoleScreenBufferInfoEx->srWindow.Right = ConsoleScreenBufferInfo.srWindow.Right;
  ConsoleScreenBufferInfoEx->srWindow.Top = ConsoleScreenBufferInfo.srWindow.Top;
  ConsoleScreenBufferInfoEx->srWindow.Bottom = ConsoleScreenBufferInfo.srWindow.Bottom;
  ConsoleScreenBufferInfoEx->wAttributes = ConsoleScreenBufferInfo.wAttributes;
  
  // New attributes introduced in Vista:
  ConsoleScreenBufferInfoEx->wPopupAttributes = ConsoleScreenBufferInfo.wAttributes;
  if (!isLonghorn) // This attribute is only avaliable starting in Longhorn Post Reset
    ConsoleScreenBufferInfoEx->bFullscreenSupported = TRUE;
  
  return TRUE;
}

/*--------------------------------------------------------------
 *  SetConsoleScreenBufferInfoEx
 *
 * @implemented - new
 */
 BOOL 
WINAPI 
SetConsoleScreenBufferInfoEx(
    HANDLE hConsoleOutput, 
    CONSOLE_SCREEN_BUFFER_INFOEX *ConsoleScreenBufferInfoEx
)
{
    // The winsrv API only supports the SrvSetScreenBufferInfo opcode API starting from LH 5048.
    // TODO: detect Longhorn 5048 winsrv and dynamically adapt.
    if (!ConsoleScreenBufferInfoEx) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    
    if (!SetConsoleScreenBufferSize(hConsoleOutput, ConsoleScreenBufferInfoEx->dwSize))
        return FALSE;
    if (!SetConsoleWindowInfo(hConsoleOutput, TRUE, &ConsoleScreenBufferInfoEx->srWindow))
        return FALSE;
    if (!SetConsoleTextAttribute(hConsoleOutput, ConsoleScreenBufferInfoEx->wAttributes))
        return FALSE;
    if (!SetConsoleCursorPosition(hConsoleOutput, ConsoleScreenBufferInfoEx->dwCursorPosition))
        return FALSE;
    
    return TRUE;
}
// BOOL 
// WINAPI 
// SetConsoleScreenBufferInfoEx(
    // HANDLE hConsoleOutput, 
    // CONSOLE_SCREEN_BUFFER_INFOEX *lpConsoleScreenBufferInfoEx
// )
// {
  // // CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo; 
  // // if (!ConsoleScreenBufferInfoEx) {
    // // SetLastError(ERROR_INVALID_PARAMETER);
    // // return FALSE;
  // // }
  
  // // ConsoleScreenBufferInfo.dwSize.X = ConsoleScreenBufferInfoEx->dwSize.X;
  // // ConsoleScreenBufferInfo.dwSize.Y = ConsoleScreenBufferInfoEx->dwSize.Y;
  // // ConsoleScreenBufferInfo.dwMaximumWindowSize.X = ConsoleScreenBufferInfoEx->dwMaximumWindowSize.X;
  // // ConsoleScreenBufferInfo.dwMaximumWindowSize.Y = ConsoleScreenBufferInfoEx->dwMaximumWindowSize.Y;
  // // ConsoleScreenBufferInfo.dwCursorPosition.X = ConsoleScreenBufferInfoEx->dwCursorPosition.X;
  // // ConsoleScreenBufferInfo.dwCursorPosition.Y = ConsoleScreenBufferInfoEx->dwCursorPosition.Y;
  // // ConsoleScreenBufferInfo.srWindow.Left = ConsoleScreenBufferInfoEx->srWindow.Left;
  // // ConsoleScreenBufferInfo.srWindow.Right = ConsoleScreenBufferInfoEx->srWindow.Right;
  // // ConsoleScreenBufferInfo.srWindow.Top = ConsoleScreenBufferInfoEx->srWindow.Top;
  // // ConsoleScreenBufferInfo.srWindow.Bottom = ConsoleScreenBufferInfoEx->srWindow.Bottom;
  // // ConsoleScreenBufferInfo.wAttributes = ConsoleScreenBufferInfoEx->wAttributes;
  
  // // return SetConsoleScreenBufferInfo(hConsoleOutput, &ConsoleScreenBufferInfo);
    // DbgPrint("SetConsoleScreenBufferInfoEx(0x%p, 0x%p) UNIMPLEMENTED!\n", hConsoleOutput, lpConsoleScreenBufferInfoEx);
    // SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    // return FALSE;  
// }

DWORD 
WINAPI 
GetConsoleOriginalTitleA(
  _Out_  LPTSTR lpConsoleTitle,
  _In_   DWORD nSize
)
{
	return GetConsoleTitleA(lpConsoleTitle, nSize);
}

DWORD 
WINAPI 
GetConsoleOriginalTitleW(
  _Out_  LPWSTR lpConsoleTitle,
  _In_   DWORD nSize
)
{
	return GetConsoleTitleW(lpConsoleTitle, nSize);
}

BOOL
WINAPI
GetCurrentConsoleFontEx(
	IN HANDLE hConsoleOutput,
    IN BOOL bMaximumWindow,
    OUT PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx
)
{
	CONSOLE_FONT_INFO lpConsoleCurrentFont;
	
	lpConsoleCurrentFontEx->cbSize = sizeof(CONSOLE_FONT_INFOEX);
	
	if(GetCurrentConsoleFont(hConsoleOutput, bMaximumWindow, &lpConsoleCurrentFont)){
		lpConsoleCurrentFontEx->nFont = lpConsoleCurrentFont.nFont;
		lpConsoleCurrentFontEx->dwFontSize = lpConsoleCurrentFont.dwFontSize;
		lpConsoleCurrentFontEx->FontWeight = 400;
		wcscpy(lpConsoleCurrentFontEx->FaceName, L"Raster Fonts");
		lpConsoleCurrentFontEx->FontFamily = FF_DONTCARE;
		return TRUE;
	}else{
		return FALSE;
	}
}

BOOL 
WINAPI 
SetCurrentConsoleFontEx(
	HANDLE hConsoleOutput, 
	BOOL bMaximumWindow, 
	PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx
)
{
	return SetConsoleFont(hConsoleOutput, lpConsoleCurrentFontEx->nFont);
}

/******************************************************************************
 *	ClosePseudoConsole   (kernelbase.@)
 */
void WINAPI ClosePseudoConsole( HPCON handle )
{
    struct pseudo_console *pseudo_console = handle;

    if (!pseudo_console) return;
    if (pseudo_console->signal) CloseHandle( pseudo_console->signal );
    if (pseudo_console->process)
    {
        WaitForSingleObject( pseudo_console->process, INFINITE );
        CloseHandle( pseudo_console->process );
    }
    if (pseudo_console->reference) CloseHandle( pseudo_console->reference );
}