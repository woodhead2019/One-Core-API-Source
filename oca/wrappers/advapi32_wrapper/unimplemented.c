/*++

Copyright (c) 2026  Shorthorn Project

Module Name:

    unimplemented.c

Abstract:

    Unimplemented API Interfaces

Author:

    Skulltrail 27-January-2026

Revision History:

--*/

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(unimplemented);

// Required for Word 2013. We pretty much cheated and skipped osppsvc to get this far.
BOOL WINAPI GetThreadWaitChain(HWCT handle, DWORD_PTR ctx, DWORD flags, DWORD thread_id, DWORD *node_count,
    WAITCHAIN_NODE_INFO *node_info_arr, BOOL *is_cycle)
{
	if(pGetThreadWaitChain){
		pGetThreadWaitChain(handle, ctx, flags, thread_id, node_count, node_info_arr, is_cycle);
	}	
	
    FIXME( "handle %p, ctx %Ix, flags %ld, thread_id %ld, node_count %p, node_info_arr %p, is_cycle %p stub!\n",
           handle, ctx, flags, thread_id, node_count, node_info_arr, is_cycle );
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

HWCT WINAPI OpenThreadWaitChainSession(DWORD flags, PWAITCHAINCALLBACK callback)
{
    if (pOpenThreadWaitChainSession)
        return pOpenThreadWaitChainSession(flags, callback);
	
    FIXME("flags %ld, callback %p stub!\n", flags, callback);
    SetLastError(ERROR_NOT_SUPPORTED);
    return NULL;
}

/***********************************************************************
 *      RegisterWaitChainCOMCallback (ole32.@)
 */
void WINAPI RegisterWaitChainCOMCallback(PCOGETCALLSTATE call_state_cb,
                                         PCOGETACTIVATIONSTATE activation_state_cb)
{
    FIXME("%p, %p\n", call_state_cb, activation_state_cb);
}

void WINAPI CloseThreadWaitChainSession(HWCT handle){
	if(pCloseThreadWaitChainSession){
		pCloseThreadWaitChainSession(handle);
	}		
    FIXME("%p\n", handle);	
}

ULONG
WINAPI
WmiSendDataBlock(ULONG *InputBuffer, int a2, int a3, int a4, int a5)
{
	return ERROR_SUCCESS;
}


/******************************************************************************
 * QueryWindows31FilesMigration [ADVAPI32.@]
 *
 * PARAMS
 *   x1 []
 */
DWORD WINAPI
QueryWindows31FilesMigration( DWORD x1 )
{
	//Need a getprocaddress
	FIXME("(%d):stub\n",x1);
	return 0;
}

/******************************************************************************
 * SynchronizeWindows31FilesAndWindowsNTRegistry [ADVAPI32.@]
 *
 * PARAMS
 *   x1 []
 *   x2 []
 *   x3 []
 *   x4 []
 */
BOOL WINAPI
SynchronizeWindows31FilesAndWindowsNTRegistry( DWORD x1, DWORD x2, DWORD x3,
                                               DWORD x4 )
{
	//Need a getprocaddress
	FIXME("(0x%08x,0x%08x,0x%08x,0x%08x):stub\n",x1,x2,x3,x4);
	return TRUE;
}