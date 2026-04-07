/*++

Copyright (c) 2022 Shorthorn Project

Module Name:

    syspams.c

Abstract:

    Implement Touch Screen Support functions

Author:

    Skulltrail 14-February-2022

Revision History:

--*/

#include <main.h>

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

DWORD g_pointerTlsInfo;
DWORD g_MouseInPointerState = -1;

typedef struct _OCA_POINTER_HOOK {
	POINTER_INFO pointerInfo; // must be first!
	HHOOK hook;
} OCA_POINTER_HOOK, *POCA_POINTER_HOOK;

// Converts legacy WM_MOUSE messages to a modern WM_POINTER message.

void SendPointerMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, POINT pt, DWORD time) {
    POINTER_INFO* pointerInfo = TlsGetValue(g_pointerTlsInfo);
    DWORD message = 0;
	
    pointerInfo->pointerType = PT_MOUSE;
	pointerInfo->pointerId = 1;
	pointerInfo->sourceDevice = INVALID_HANDLE_VALUE;
	pointerInfo->historyCount = 1;
	pointerInfo->frameId++;
	pointerInfo->hwndTarget = hwnd;
	pointerInfo->dwTime = time;
	pointerInfo->pointerFlags = POINTER_FLAG_UPDATE | POINTER_MESSAGE_FLAG_PRIMARY;
	
    pointerInfo->ptPixelLocation.x = pt.x;
    pointerInfo->ptPixelLocation.y = pt.y;
	pointerInfo->ptHimetricLocation.x = pt.x;
    pointerInfo->ptHimetricLocation.y = pt.y;
	pointerInfo->ptPixelLocationRaw.x = pt.x;
    pointerInfo->ptPixelLocationRaw.y = pt.y;
	pointerInfo->ptHimetricLocationRaw.x = pt.x;
    pointerInfo->ptHimetricLocationRaw.y = pt.y;
	pointerInfo->ButtonChangeType = 0;
	
    switch (msg) {
        case WM_MOUSEMOVE:
            message = WM_POINTERUPDATE;
			pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_INRANGE;
            break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            message = WM_POINTERDOWN;
            pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_PRIMARY | POINTER_MESSAGE_FLAG_INCONTACT;
			if (msg == WM_LBUTTONDOWN)
			{
				pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_FIRSTBUTTON;
				pointerInfo->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_DOWN;
			}
			else if (msg == WM_RBUTTONDOWN)
			{
				pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_SECONDBUTTON;
				pointerInfo->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_DOWN;
			}
			else if (msg == WM_MBUTTONDOWN)
			{
				pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_THIRDBUTTON;
				pointerInfo->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_DOWN;
			}
			else if (msg == WM_XBUTTONDOWN && LOWORD(wParam) == MK_LBUTTON)
			{
				pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_FIRSTBUTTON;
				pointerInfo->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_DOWN;
			}
			else if (msg == WM_XBUTTONDOWN && LOWORD(wParam) == MK_RBUTTON)
			{
                pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_SECONDBUTTON;
                pointerInfo->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_DOWN;
            }
            else if (msg == WM_XBUTTONDOWN && LOWORD(wParam) == MK_MBUTTON)
            {
                pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_THIRDBUTTON;
                pointerInfo->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_DOWN;
            }
            else if (msg == WM_XBUTTONDOWN && LOWORD(wParam) == MK_XBUTTON1)
            {
                pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_FOURTHBUTTON;
                pointerInfo->ButtonChangeType = POINTER_CHANGE_FOURTHBUTTON_DOWN;
            }
            else if (msg == WM_XBUTTONDOWN && LOWORD(wParam) == MK_XBUTTON2)
            {
                pointerInfo->pointerFlags |= POINTER_MESSAGE_FLAG_FIFTHBUTTON;
                pointerInfo->ButtonChangeType = POINTER_CHANGE_FIFTHBUTTON_DOWN;
            }
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            message = WM_POINTERUP;
            if (msg == WM_LBUTTONUP) pointerInfo->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_UP;
            else if (msg == WM_RBUTTONUP) pointerInfo->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_UP;
            else if (msg == WM_MBUTTONUP) pointerInfo->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_UP;
            else if (msg == WM_XBUTTONUP && LOWORD(wParam) == MK_LBUTTON)
                pointerInfo->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_UP;
            else if (msg == WM_XBUTTONUP && LOWORD(wParam) == MK_RBUTTON)
                pointerInfo->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_UP;
            else if (msg == WM_XBUTTONUP && LOWORD(wParam) == MK_MBUTTON)
                pointerInfo->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_UP;
            else if (msg == WM_XBUTTONUP && LOWORD(wParam) == MK_XBUTTON1)
                pointerInfo->ButtonChangeType = POINTER_CHANGE_FOURTHBUTTON_UP;
            else if (msg == WM_XBUTTONUP && LOWORD(wParam) == MK_XBUTTON2)
                pointerInfo->ButtonChangeType = POINTER_CHANGE_FIFTHBUTTON_UP;
            break;
        case WM_MOUSEWHEEL:
            message = WM_POINTERWHEEL;
            pointerInfo->pointerFlags = HIWORD(wParam);
            break;
        case WM_MOUSEHWHEEL:
            message = WM_POINTERHWHEEL;
            pointerInfo->pointerFlags = HIWORD(wParam);
            break;
    }
	
    if (message) {
        // Send the message to the window
		SendMessageW(hwnd, message, MAKELONG(pointerInfo->pointerId, LOWORD(pointerInfo->pointerFlags)), MAKELONG(pt.x, pt.y));
    }
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// We should only activate this emulation when EnableMouseInPointer is called with parameters TRUE.
    if (g_MouseInPointerState == 1 && nCode >= 0)
    {
        MSG* pMsg = (MSG*)lParam;
		
		// only intercept messages of interest
		switch (pMsg->message) {
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_XBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_MOUSEHWHEEL:
				SendPointerMessage(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam, pMsg->pt, pMsg->time);
		}
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

VOID WINAPI PointerApiThreadStartup(HINSTANCE hinstDLL) {
	OCA_POINTER_HOOK* ptrInfo;

	if (TlsGetValue(g_pointerTlsInfo) != NULL)
		return;

	ptrInfo = malloc(sizeof(OCA_POINTER_HOOK));
	if (ptrInfo) {
		memset(ptrInfo, 0, sizeof(OCA_POINTER_HOOK));
		ptrInfo->hook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hinstDLL, GetCurrentThreadId());
	}
	TlsSetValue(g_pointerTlsInfo, ptrInfo);
}

VOID WINAPI PointerApiThreadShutdown(HINSTANCE hinstDLL) {
	OCA_POINTER_HOOK* ptrInfo = TlsGetValue(g_pointerTlsInfo);
	if (ptrInfo != NULL) {
		UnhookWindowsHookEx(ptrInfo->hook);
		free(ptrInfo);
		TlsSetValue(g_pointerTlsInfo, NULL);
	}
}

void WINAPI FreeUMHandleEntry(PRTL_CRITICAL_SECTION a1, PBOOL a2)
{
  PBOOL resp; // ebx@1
  LONG recursion; // esi@2
  LONG count; // esi@2
  PBOOL v5; // [sp+10h] [bp+Ch]@2

  resp = a2;
  if ( a2[1] )
  {
    recursion = (LONG)(a2 - a1->RecursionCount);
    v5 = a2;
    count = recursion >> 4;
    if ( resp[3] )
      RtlFreeHeap(pUserHeap, 0, (HANDLE)resp[3]);
    *resp = 0;
    resp[1] = 0;
    resp[2] = 0;
    resp[3] = 0;
    *resp = (LONG)v5 + 1;
    --*&a1->DebugInfo->CreatorBackTraceIndexHigh;
    if ( count < a1->LockCount )
      a1->LockCount = count;
  }
}

PBOOL WINAPI UMHandleActiveEntryFromHandle(RTL_CRITICAL_SECTION a1, HANDLE a2)
{
  PBOOL result = FALSE; // eax@2 
  DbgPrint("UMHandleActiveEntryFromHandle is UNIMPLEMENTED\n");  
  return result;
}

BOOL WINAPI UnlockUMHandleList(PRTL_CRITICAL_SECTION CriticalSectionObject)
{
  NTSTATUS status = STATUS_SUCCESS; // eax@1
  BOOL result; // eax@2

  RtlLeaveCriticalSection(CriticalSectionObject);
  if ( status >= 0 )
  {
    result = TRUE;
  }
  else
  {
    SetLastError(status);
    result = FALSE;
  }
  return result;
}

BOOL WINAPI LockUMHandleList(PRTL_CRITICAL_SECTION CriticalSectionObject)
{
  NTSTATUS status = STATUS_SUCCESS; // eax@1
  BOOL result; // eax@2

  RtlEnterCriticalSection(CriticalSectionObject);
  if ( status >= 0 )
  {
    result = TRUE;
  }
  else
  {
    SetLastError(status);
    result = FALSE;
  }
  return result;
}

BOOL WINAPI CloseTouchInputHandle(HTOUCHINPUT a1)
{
    return FALSE;
}

BOOL WINAPI CloseGestureInfoHandle(HGESTUREINFO  hGestureInfo)
{
    return FALSE;
}

BOOL WINAPI RegisterTouchWindow(
  _In_  HWND hWnd,
  _In_  ULONG ulFlags
)
{
	DbgPrint("RegisterTouchWindow is UNIMPLEMENTED\n");  
	return TRUE;
}

BOOL WINAPI UnregisterTouchWindow(
  _In_  HWND hWnd
)
{
	DbgPrint("UnregisterTouchWindow is UNIMPLEMENTED\n");  
	return TRUE;
}

BOOL WINAPI GetTouchInputInfo(
  _In_   HTOUCHINPUT hTouchInput,
  _In_   UINT cInputs,
  _Out_  PTOUCHINPUT pInputs,
  _In_   int cbSize
)
{
	  // BOOL result; // eax@2
	  // if ( cbSize == 40 )
	  // {
		// result = GetTouchInputInfoWorker(hTouchInput, cInputs, pInputs, 0);
	  // }
	  // else
	  // {
		// RtlSetLastWin32Error(87);
		// result = 0;
	  // }
	  // return result;
	return FALSE;
}

BOOL WINAPI IsTouchWindow(
  _In_       HWND hWnd,
  _Out_opt_  PULONG pulFlags
)
{
	DbgPrint("IsTouchWindow is UNIMPLEMENTED\n"); 
	SetLastError(0);
	return FALSE;
}

BOOL WINAPI UnregisterPointerInputTarget(
  _In_  HWND hwnd,
  _In_  POINTER_INPUT_TYPE  pointerType
)
{
	DbgPrint("UnregisterPointerInputTarget is UNIMPLEMENTED\n");  
	return TRUE;
}

BOOL WINAPI PtInRect(const RECT *lprc, POINT pt)
{
  return lprc && pt.x >= lprc->left && pt.x < lprc->right && pt.y >= lprc->top && pt.y < lprc->bottom;
}

BOOL WINAPI GetGestureInfo(
  _In_   HGESTUREINFO hGestureInfo,
  _Out_  PGESTUREINFO pGestureInfo
)
{
	pGestureInfo = NULL;
	DbgPrint("GetGestureInfo is UNIMPLEMENTED\n");  
	return TRUE;
}

BOOL WINAPI SetGestureConfig(
  _In_  HWND hwnd,
  _In_  DWORD dwReserved,
  _In_  UINT cIDs,
  _In_  PGESTURECONFIG pGestureConfig,
  _In_  UINT cbSize
)
{
	pGestureConfig = NULL;
	DbgPrint("SetGestureConfig is UNIMPLEMENTED\n");  
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

BOOL WINAPI GetGestureConfig(
  _In_     HWND hwnd,
  _In_     DWORD dwReserved,
  _In_     DWORD dwFlags,
  _In_     PUINT pcIDs,
  _Inout_  PGESTURECONFIG pGestureConfig,
  _In_     UINT cbSize
)
{
	pGestureConfig = NULL;
	DbgPrint("GetGestureConfig is UNIMPLEMENTED\n");  
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}


BOOL WINAPI GetGestureExtraArgs(
  _In_   HGESTUREINFO hGestureInfo,
  _In_   UINT cbExtraArgs,
  _Out_  PBYTE pExtraArgs
)
{
	pExtraArgs = NULL;
	DbgPrint("GetGestureExtraArgs is UNIMPLEMENTED\n");  	
	return TRUE;
}

BOOL 
WINAPI 
EvaluateProximityToRect(
  _In_  const RECT                                   *controlBoundingBox,
  _In_  const TOUCH_HIT_TESTING_INPUT                *pHitTestingInput,
  _Out_       TOUCH_HIT_TESTING_PROXIMITY_EVALUATION *pProximityEval
)
{
	DbgPrint("EvaluateProximityToRect is UNIMPLEMENTED\n");  	
	return FALSE;
}

BOOL WINAPI GetPointerDevices(UINT32 *DeviceCount, POINTER_DEVICE_INFO *Devices) {
	*DeviceCount = 0;
	return TRUE;
}

BOOL 
WINAPI 
GetPointerType(
  _In_  UINT32             pointerId,
  _Out_ POINTER_INPUT_TYPE *pointerType
)
{
	DbgPrint("GetPointerType is UNIMPLEMENTED\n");		
	*pointerType = PT_MOUSE;
	return TRUE;
}

BOOL 
WINAPI 
RegisterPointerDeviceNotifications(
  _In_ HWND window,
  _In_ BOOL notifyRange
)
{
	DbgPrint("RegisterPointerDeviceNotifications is UNIMPLEMENTED\n");		
	return TRUE;
}

BOOL 
WINAPI 
RegisterTouchHitTestingWindow(
  _In_ HWND  hwnd,
  _In_ ULONG value
)
{
	DbgPrint("RegisterTouchHitTestingWindow is UNIMPLEMENTED\n");		
	return TRUE;
}

__declspec(dllexport)
BOOL WINAPI GetPointerInfo(UINT32 id, POINTER_INFO *info) {
	POINTER_INFO *frame = TlsGetValue(g_pointerTlsInfo);
	
	if (!frame) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	
	if (!id || !info || !frame->frameId || id != frame->pointerId) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	
	memmove(info, frame, sizeof(POINTER_INFO));
	return TRUE;
}

BOOL WINAPI GetPointerTouchInfoHistory(UINT32 pointerId, UINT32 *entriesCount, POINTER_TOUCH_INFO *touchInfo) {
	SetLastError(ERROR_DATATYPE_MISMATCH);
	return FALSE;
}
__declspec(dllexport)
BOOL WINAPI GetPointerInfoHistory(UINT32 pointerId, UINT32 *entriesCount, POINTER_INFO* pointerInfo) {
	if (!entriesCount) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	if (*entriesCount == 0) {
		*entriesCount = 1;
		return TRUE;
	}
	if (!pointerInfo) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	return GetPointerInfo(pointerId, &pointerInfo[0]);
}

BOOL WINAPI GetPointerPenInfoHistory(UINT32 pointerId, UINT32 *entriesCount, POINTER_PEN_INFO *penInfo) {
	SetLastError(ERROR_DATATYPE_MISMATCH);
	return FALSE;
}
BOOL WINAPI GetPointerPenInfo(UINT32 pointerId, POINTER_PEN_INFO *pointerType) {
	GetPointerInfo(pointerId, &(pointerType->pointerInfo));
	pointerType->penFlags = 0;
	pointerType->penMask = 1;
	pointerType->pressure = 0;
	pointerType->tiltX = 0;
	pointerType->tiltY = 0;
	return TRUE;
}
BOOL WINAPI GetPointerFrameTouchInfo(UINT32 PointerID, UINT32 *Pointers, POINTER_TOUCH_INFO *Info) {
	SetLastError(ERROR_DATATYPE_MISMATCH);
	return FALSE;
}
BOOL WINAPI GetPointerFrameTouchInfoHistory(UINT32 PointerID, UINT32 *Entries, UINT32 *Pointers, POINTER_TOUCH_INFO *Info) {
	SetLastError(ERROR_DATATYPE_MISMATCH);
	return FALSE;
}
BOOL WINAPI GetPointerFrameInfo(UINT32 PointerID, UINT32 *Pointers, POINTER_INFO *Info) {
	*Pointers = 1;
	GetPointerInfo(PointerID, Info);
	return TRUE;
}
BOOL WINAPI GetPointerTouchInfo(UINT32 PointerID, POINTER_TOUCH_INFO *Info) {
	SetLastError(ERROR_DATATYPE_MISMATCH);
	return FALSE;
}
BOOL WINAPI GetPointerFrameInfoHistory(UINT32 PointerID, UINT32 *Entries, UINT32 *Pointers, POINTER_INFO *Info) {
	*Entries = 1;
	*Pointers = 1;
	GetPointerInfo(PointerID, Info);
	return TRUE;
}
BOOL WINAPI GetPointerDeviceRects(HANDLE device, RECT *pointerDeviceRect, RECT *displayRect) {
	if (displayRect != 0) {
		displayRect->top = 0;
		displayRect->right = 0;
		displayRect->bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		displayRect->left = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	}
	if (pointerDeviceRect != 0) {
		pointerDeviceRect->top = 0;
		pointerDeviceRect->right = 0;
		pointerDeviceRect->bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		pointerDeviceRect->left = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	}
	return TRUE;
}

HRESULT WINAPI PackTouchHitTestingProximityEvaluation(const TOUCH_HIT_TESTING_INPUT *pHitTestingInput, const TOUCH_HIT_TESTING_PROXIMITY_EVALUATION *pProximityEval)
{
  int proximityEval; // esi@1
  int proximityDistance; // edi@1
  HRESULT result; // eax@4

  proximityEval = pProximityEval->adjustedPoint.x - pHitTestingInput->point.x;
  proximityDistance = pProximityEval->adjustedPoint.y - pHitTestingInput->point.y;
  if ( pProximityEval->score > 0xFFFu || abs(proximityEval) >= 511 || abs(proximityDistance) >= 511 )
    result = 0xFFF00000u;
  else
    result = (proximityDistance & 0x3FF) + (((proximityEval & 0x3FF) + (pProximityEval->score << 10)) << 10);
  return result;
}

__declspec(dllexport)
BOOL WINAPI IsMouseInPointerEnabled() {
	return g_MouseInPointerState == 1;
}

/***********************************************************************
 *		EnableMouseInPointer (USER32.@)
 */
BOOL WINAPI EnableMouseInPointer(BOOL enable)
{
    DbgPrint("EnableMouseInPointer stub\n", enable);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

// needed for photoshop 2025.
BOOL WINAPI GetPointerDeviceProperties(HANDLE device, UINT32 *propertyCount, POINTER_DEVICE_PROPERTY *pointerProperties) {
    if (!propertyCount)
        return FALSE;
    
    *propertyCount = 0;
    return TRUE;
}