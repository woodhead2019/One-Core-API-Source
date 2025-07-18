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

#include <main.h>

WINE_DEFAULT_DEBUG_CHANNEL(user32);

HANDLE section = NULL;
static BOOL (WINAPI *pIsSETEnabled)();

UINT_PTR 
WINAPI 
SetCoalescableTimer(
  _In_opt_  HWND hwnd,
  _In_      UINT_PTR nIDEvent,
  _In_      UINT uElapse,
  _In_opt_  TIMERPROC lpTimerFunc,
  _In_      ULONG uToleranceDelay
)
{
	return SetTimer(hwnd, nIDEvent, uElapse, lpTimerFunc);
}

BOOL WINAPI IsSETEnabled()
{
	HMODULE huserbase = GetModuleHandleA("userbase.dll");
	pIsSETEnabled = (void *)GetProcAddress(huserbase, "IsSETEnabled");
	if(pIsSETEnabled){
		return pIsSETEnabled();
	}else{
		return FALSE;
	}
}

BOOL WINAPI HangrepRegisterPort(HANDLE Port)
{
	return TRUE;	
}

BOOL WINAPI HangrepUnregisterPort(HANDLE Port)
{
	return TRUE;	
}

HANDLE WINAPI ThemeGetCurrentSection(HANDLE Port)
{
	return section;	
}

BOOL WINAPI ThemeSetCurrentSection(HANDLE Section)
{
	section = Section;
	return TRUE;
}

/**********************************************************************
 * RegisterSuspendResumeNotification (USER32.@)
 */
HPOWERNOTIFY WINAPI RegisterSuspendResumeNotification(HANDLE recipient, DWORD flags)
{
    FIXME("%p, %#lx: stub.\n", recipient, flags);
    return (HPOWERNOTIFY)0xdeadbeef;
}

/**********************************************************************
 * UnregisterSuspendResumeNotification (USER32.@)
 */
BOOL WINAPI UnregisterSuspendResumeNotification(HPOWERNOTIFY handle)
{
    FIXME("%p: stub.\n", handle);
    return TRUE;
}

BOOL WINAPI GetPointerDevice(HANDLE device, POINTER_DEVICE_INFO *dev) {
	return FALSE;
}

BOOL WINAPI SkipPointerFrameMessages(UINT32 ID) {
	return TRUE;
}