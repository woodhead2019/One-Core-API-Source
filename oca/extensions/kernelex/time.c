/*++

Copyright (c) 2017 Shorthorn Project

Module Name:

    time.c

Abstract:

    This module implements Time functions for the Win32 APIs.

Author:

    Skulltrail 06-May-2017

Revision History:

--*/

#include <main.h>

WINE_DEFAULT_DEBUG_CHANNEL(kernelex);

static    LARGE_INTEGER         TIME_qpcFreq;

/******************************************************************************
 *           QueryInterruptTime  (kernelex.@)
 */
void WINAPI DECLSPEC_HOTPATCH QueryInterruptTime( ULONGLONG *time )
{
    ULONG high, low;

    do
    {
        high = SharedUserData->InterruptTime.High1Time;
        low = SharedUserData->InterruptTime.LowPart;
    }
    while (high != SharedUserData->InterruptTime.High2Time);
    *time = (ULONGLONG)high << 32 | low;
}


/******************************************************************************
 *           QueryInterruptTimePrecise  (kernelex.@)
 */
void WINAPI DECLSPEC_HOTPATCH QueryInterruptTimePrecise( ULONGLONG *time )
{
    static int once;
    if (!once++) FIXME( "(%p) semi-stub\n", time );

    QueryInterruptTime( time );
}


/***********************************************************************
 *           QueryUnbiasedInterruptTimePrecise  (kernelex.@)
 */
void WINAPI DECLSPEC_HOTPATCH QueryUnbiasedInterruptTimePrecise( ULONGLONG *time )
{
    static int once;
    if (!once++) FIXME( "(%p): semi-stub.\n", time );

    RtlQueryUnbiasedInterruptTime( time );
}

/***********************************************************************
 *           QueryUnbiasedInterruptTime   (KERNEL32.@)
 */
BOOL 
WINAPI 
QueryUnbiasedInterruptTime(ULONGLONG *time)
{
    if (!time) return FALSE;
    RtlQueryUnbiasedInterruptTime(time);
    return TRUE;
}

/**************************************************************************
 * 				timeGetTime    [MMSYSTEM.607]
 * 				timeGetTime    [WINMM.@]
 */
DWORD WINAPI timeGetTime(void)
{
    LARGE_INTEGER perfCount;
#if defined(COMMENTOUTPRIORTODELETING)
    DWORD       count;

    /* FIXME: releasing the win16 lock here is a temporary hack (I hope)
     * that lets mciavi.drv run correctly
     */
    if (pFnReleaseThunkLock) pFnReleaseThunkLock(&count);
    if (pFnRestoreThunkLock) pFnRestoreThunkLock(count);
#endif
    /* Use QPC if a high-resolution timer was requested (<= 5ms) */
    if (TIME_qpcFreq.QuadPart != 0)
    {
        QueryPerformanceCounter(&perfCount);
        return (DWORD)(perfCount.QuadPart / TIME_qpcFreq.QuadPart);
    }
    /* Otherwise continue using GetTickCount */
    return GetTickCount();
}