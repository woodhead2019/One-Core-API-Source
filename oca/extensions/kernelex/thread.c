/*++

Copyright (c) 2017  Shorthorn Project

Module Name:

    thread.c

Abstract:

    This module implements Win32 Thread Object APIs

Author:

    Skulltrail 16-March-2017

Revision History:

--*/
 
#define NDEBUG

#include "main.h"


#define THREAD_SET_LIMITED_INFORMATION   0x0400

#ifndef ROUND_DOWN
#define ROUND_DOWN(n, align) (((ULONG)n) & ~((align) - 1l))
#endif

#ifndef ROUND_UP
#define ROUND_UP(n, align) ROUND_DOWN(((ULONG)n) + (align) - 1, (align))
#endif

WINE_DEFAULT_DEBUG_CHANNEL(kernel32);
WINE_DECLARE_DEBUG_CHANNEL(winedbg);

static DWORD (WINAPI *pConsoleIMERoutine)(LPVOID);
static DWORD (WINAPI *pCtrlRoutine)(LPVOID);
void WINAPI RtlFreeUserStack( void *stack );

extern BOOLEAN BaseRunningInServerProcess;

/*******************************************************************
 *         format_exception_msg
 */
static int format_exception_msg( const EXCEPTION_POINTERS *ptr, char *buffer, int size )
{
    const EXCEPTION_RECORD *rec = ptr->ExceptionRecord;
    int len,len2;

    switch(rec->ExceptionCode)
    {
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        len = snprintf( buffer, size, "Unhandled division by zero" );
        break;
    case EXCEPTION_INT_OVERFLOW:
        len = snprintf( buffer, size, "Unhandled overflow" );
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        len = snprintf( buffer, size, "Unhandled array bounds" );
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        len = snprintf( buffer, size, "Unhandled illegal instruction" );
        break;
    case EXCEPTION_STACK_OVERFLOW:
        len = snprintf( buffer, size, "Unhandled stack overflow" );
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        len = snprintf( buffer, size, "Unhandled privileged instruction" );
        break;
    case EXCEPTION_ACCESS_VIOLATION:
        if (rec->NumberParameters == 2)
            len = snprintf( buffer, size, "Unhandled page fault on %s access to 0x%08lx",
                            rec->ExceptionInformation[0] == EXCEPTION_WRITE_FAULT ? "write" :
                            rec->ExceptionInformation[0] == EXCEPTION_EXECUTE_FAULT ? "execute" : "read",
                            rec->ExceptionInformation[1]);
        else
            len = snprintf( buffer, size, "Unhandled page fault");
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        len = snprintf( buffer, size, "Unhandled alignment" );
        break;
    case CONTROL_C_EXIT:
        len = snprintf( buffer, size, "Unhandled ^C");
        break;
    case STATUS_POSSIBLE_DEADLOCK:
        len = snprintf( buffer, size, "Critical section %08lx wait failed",
                 rec->ExceptionInformation[0]);
        break;
    case EXCEPTION_WINE_STUB:
        if ((ULONG_PTR)rec->ExceptionInformation[1] >> 16)
            len = snprintf( buffer, size, "Unimplemented function %s.%s called",
                            (char *)rec->ExceptionInformation[0], (char *)rec->ExceptionInformation[1] );
        else
            len = snprintf( buffer, size, "Unimplemented function %s.%ld called",
                            (char *)rec->ExceptionInformation[0], rec->ExceptionInformation[1] );
        break;
    case EXCEPTION_WINE_ASSERTION:
        len = snprintf( buffer, size, "Assertion failed" );
        break;
    default:
        len = snprintf( buffer, size, "Unhandled exception 0x%08x in thread %x", rec->ExceptionCode, GetCurrentThreadId());
        break;
    }
    if ((len<0) || (len>=size))
        return -1;
#ifdef __i386__
    if (ptr->ContextRecord->SegCs)
        len2 = snprintf(buffer+len, size-len, " at address 0x%04x:0x%08x",
                        LOWORD(ptr->ContextRecord->SegCs),
                        (DWORD)ptr->ExceptionRecord->ExceptionAddress);
    else
#endif
        len2 = snprintf(buffer+len, size-len, " at address %p",
                        ptr->ExceptionRecord->ExceptionAddress);
    if ((len2<0) || (len>=size-len))
        return -1;
    return len+len2;
}

/******************************************************************
 *		start_debugger
 *
 * Does the effective debugger startup according to 'format'
 */
static BOOL	start_debugger(PEXCEPTION_POINTERS epointers, HANDLE hEvent)
{
    OBJECT_ATTRIBUTES attr;
    UNICODE_STRING nameW;
    char *cmdline, *env, *p;
    HANDLE		hDbgConf;
    DWORD		bAuto = TRUE;
    PROCESS_INFORMATION	info;
    STARTUPINFOA	startup;
    char*		format = NULL;
    BOOL		ret = FALSE;
    char 		buffer[256];

    static const WCHAR AeDebugW[] = {'\\','R','e','g','i','s','t','r','y','\\',
                                     'M','a','c','h','i','n','e','\\',
                                     'S','o','f','t','w','a','r','e','\\',
                                     'M','i','c','r','o','s','o','f','t','\\',
                                     'W','i','n','d','o','w','s',' ','N','T','\\',
                                     'C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\',
                                     'A','e','D','e','b','u','g',0};
    static const WCHAR DebuggerW[] = {'D','e','b','u','g','g','e','r',0};
    static const WCHAR AutoW[] = {'A','u','t','o',0};

    format_exception_msg( epointers, buffer, sizeof(buffer) );
    MESSAGE("wine: %s (thread %04x), starting debugger...\n", buffer, GetCurrentThreadId());

    attr.Length = sizeof(attr);
    attr.RootDirectory = 0;
    attr.ObjectName = &nameW;
    attr.Attributes = 0;
    attr.SecurityDescriptor = NULL;
    attr.SecurityQualityOfService = NULL;
    RtlInitUnicodeString( &nameW, AeDebugW );

    if (!NtOpenKey( &hDbgConf, KEY_READ, &attr ))
    {
        KEY_VALUE_PARTIAL_INFORMATION *info;
        DWORD format_size = 0;

        RtlInitUnicodeString( &nameW, DebuggerW );
        if (NtQueryValueKey( hDbgConf, &nameW, KeyValuePartialInformation,
                             NULL, 0, &format_size ) == STATUS_BUFFER_TOO_SMALL)
        {
            char *data = HeapAlloc(GetProcessHeap(), 0, format_size);
            NtQueryValueKey( hDbgConf, &nameW, KeyValuePartialInformation,
                             data, format_size, &format_size );
            info = (KEY_VALUE_PARTIAL_INFORMATION *)data;
            RtlUnicodeToMultiByteSize( &format_size, (WCHAR *)info->Data, info->DataLength );
            format = HeapAlloc( GetProcessHeap(), 0, format_size+1 );
            RtlUnicodeToMultiByteN( format, format_size, NULL,
                                    (WCHAR *)info->Data, info->DataLength );
            format[format_size] = 0;

            if (info->Type == REG_EXPAND_SZ)
            {
                char* tmp;

                /* Expand environment variable references */
                format_size=ExpandEnvironmentStringsA(format,NULL,0);
                tmp=HeapAlloc(GetProcessHeap(), 0, format_size);
                ExpandEnvironmentStringsA(format,tmp,format_size);
                HeapFree(GetProcessHeap(), 0, format);
                format=tmp;
            }
            HeapFree( GetProcessHeap(), 0, data );
        }

        RtlInitUnicodeString( &nameW, AutoW );
        if (!NtQueryValueKey( hDbgConf, &nameW, KeyValuePartialInformation,
                              buffer, sizeof(buffer)-sizeof(WCHAR), &format_size ))
       {
           info = (KEY_VALUE_PARTIAL_INFORMATION *)buffer;
           if (info->Type == REG_DWORD) memcpy( &bAuto, info->Data, sizeof(DWORD) );
           else if (info->Type == REG_SZ)
           {
               WCHAR *str = (WCHAR *)info->Data;
               str[info->DataLength/sizeof(WCHAR)] = 0;
               bAuto = atoiW( str );
           }
       }

       NtClose(hDbgConf);
    }

    if (format)
    {
        size_t format_size = strlen(format) + 2*20;
        cmdline = HeapAlloc(GetProcessHeap(), 0, format_size);
        snprintf(cmdline, format_size, format, (long)GetCurrentProcessId(), (long)HandleToLong(hEvent));
        HeapFree(GetProcessHeap(), 0, format);
    }
    else
    {
        cmdline = HeapAlloc(GetProcessHeap(), 0, 80);
        snprintf(cmdline, 80, "winedbg --auto %ld %ld", /* as in tools/wine.inf */
                 (long)GetCurrentProcessId(), (long)HandleToLong(hEvent));
    }

    if (!bAuto)
    {
	HMODULE			mod = GetModuleHandleA( "user32.dll" );
	MessageBoxA_funcptr	pMessageBoxA = NULL;

	if (mod) pMessageBoxA = (MessageBoxA_funcptr)GetProcAddress( mod, "MessageBoxA" );
	if (pMessageBoxA)
	{
            static const char msg[] = ".\nDo you wish to debug it?";

            format_exception_msg( epointers, buffer, sizeof(buffer)-sizeof(msg) );
            strcat( buffer, msg );
	    if (pMessageBoxA( 0, buffer, "Exception raised", MB_YESNO | MB_ICONHAND ) == IDNO)
	    {
		TRACE("Killing process\n");
		goto EXIT;
	    }
	}
    }

    /* make WINEDEBUG empty in the environment */
    env = GetEnvironmentStringsA();
    if (!TRACE_ON(winedbg))
    {
        for (p = env; *p; p += strlen(p) + 1)
        {
            if (!memcmp( p, "WINEDEBUG=", sizeof("WINEDEBUG=")-1 ))
            {
                char *next = p + strlen(p);
                char *end = next + 1;
                while (*end) end += strlen(end) + 1;
                memmove( p + sizeof("WINEDEBUG=") - 1, next, end + 1 - next );
                break;
            }
        }
    }

    TRACE("Starting debugger %s\n", debugstr_a(cmdline));
    memset(&startup, 0, sizeof(startup));
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_SHOWNORMAL;
    ret = CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, 0, env, NULL, &startup, &info);
    FreeEnvironmentStringsA( env );

    if (ret)
    {
        /* wait for debugger to come up... */
        HANDLE handles[2];
        CloseHandle(info.hThread);
        handles[0]=hEvent;
        handles[1]=info.hProcess;
        WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        CloseHandle(info.hProcess);
    }
    else ERR("Couldn't start debugger (%s) (%d)\n"
             "Read the Wine Developers Guide on how to set up winedbg or another debugger\n",
             debugstr_a(cmdline), GetLastError());
EXIT:
    HeapFree(GetProcessHeap(), 0, cmdline);
    return ret;
}

/******************************************************************
 *		start_debugger_atomic
 *
 * starts the debugger in an atomic way:
 *	- either the debugger is not started and it is started
 *	- or the debugger has already been started by another thread
 *	- or the debugger couldn't be started
 *
 * returns TRUE for the two first conditions, FALSE for the last
 */
static BOOL start_debugger_atomic(PEXCEPTION_POINTERS epointers)
{
    static HANDLE	hRunOnce /* = 0 */;

    if (hRunOnce == 0)
    {
	OBJECT_ATTRIBUTES	attr;
	HANDLE			hEvent;

	attr.Length                   = sizeof(attr);
	attr.RootDirectory            = 0;
	attr.Attributes               = OBJ_INHERIT;
	attr.ObjectName               = NULL;
	attr.SecurityDescriptor       = NULL;
	attr.SecurityQualityOfService = NULL;

	/* ask for manual reset, so that once the debugger is started,
	 * every thread will know it */
	NtCreateEvent( &hEvent, EVENT_ALL_ACCESS, &attr, NotificationEvent, FALSE );
        if (InterlockedCompareExchangePointer( &hRunOnce, hEvent, 0 ) == 0)
	{
	    /* ok, our event has been set... we're the winning thread */
	    BOOL	ret = start_debugger( epointers, hRunOnce );
	    DWORD	tmp;

	    if (!ret)
	    {
		/* so that the other threads won't be stuck */
		NtSetEvent( hRunOnce, &tmp );
	    }
	    return ret;
	}

	/* someone beat us here... */
	CloseHandle( hEvent );
    }

    /* and wait for the winner to have actually created the debugger */
    WaitForSingleObject( hRunOnce, INFINITE );
    /* in fact, here, we only know that someone has tried to start the debugger,
     * we'll know by reposting the exception if it has actually attached
     * to the current process */
    return TRUE;
}

BOOL isXPOrLower(){
	OSVERSIONINFOW osVersion = {0};
	
	GetVersionExW(&osVersion);
	
	if(osVersion.dwMajorVersion<=5){
		if(osVersion.dwMinorVersion<=1){
			return TRUE;
		}
	}
	
	return FALSE;
}

VOID
WINAPI
BaseRundownFls(_In_ PVOID FlsData)
{
    ULONG n, FlsHighIndex;
    PRTL_FLS_DATA pFlsData;
    PFLS_CALLBACK_FUNCTION lpCallback;

    pFlsData = FlsData;

    RtlAcquirePebLock();
    FlsHighIndex = NtCurrentPeb()->FlsHighIndex;
    RemoveEntryList(&pFlsData->ListEntry);
    RtlReleasePebLock();

    for (n = 1; n <= FlsHighIndex; ++n)
    {
        lpCallback = NtCurrentPeb()->FlsCallback[n];
        if (lpCallback && pFlsData->Data[n])
        {
            lpCallback(pFlsData->Data[n]);
        }
    }

    RtlFreeHeap(RtlGetProcessHeap(), 0, FlsData);
}

// /***********************************************************************
 // *           FlsAlloc   (KERNEL32.@) - For .Net 4.5.1 + Framework Support (native functions not work)
 // */
// DWORD WINAPI FlsAlloc( PFLS_CALLBACK_FUNCTION lpCallback )
// {
	// if(isXPOrLower()){
        // return TlsAlloc();				
	// }else{
		// DWORD index;		
		// if (!set_ntstatus( RtlFlsAlloc( lpCallback, &index ))) return FLS_OUT_OF_INDEXES;
		// return index;		
	// }	
// }

// /***********************************************************************
 // *           FlsFree   (KERNEL32.@) - Framework Support (native functions not work)
 // */
// BOOL WINAPI FlsFree( DWORD index )
// {	
	// if(isXPOrLower()){
	    // return TlsFree(index); 	
	// }else{
		// return set_ntstatus( RtlFlsFree( index ));		
	// }	
// }

// /***********************************************************************
 // *           FlsGetValue   (KERNEL32.@) - Framework Support (native functions not work)
 // */
// PVOID WINAPI FlsGetValue( DWORD index )
// {
	// if(isXPOrLower()){
	    // return TlsGetValue(index); 	
	// }else{
		// void *data;
		// if (!set_ntstatus( RtlFlsGetValue( index, &data ))) return NULL;
		// SetLastError( ERROR_SUCCESS );
		// return data;	
	// }		
// }

// /***********************************************************************
 // *           FlsSetValue   (KERNEL32.@) - Framework Support (native functions not work)
 // */
// BOOL WINAPI FlsSetValue( DWORD index, PVOID lpFlsData )
// {
	// if(isXPOrLower()){
	    // return TlsSetValue(index, lpFlsData);	
	// }else{
		// return set_ntstatus( RtlFlsSetValue( index, lpFlsData ));		
	// }	
// }

/*
 * @implemented
 */
DWORD
WINAPI
FlsAlloc(PFLS_CALLBACK_FUNCTION lpCallback)
{
    DWORD dwFlsIndex;
    PPEB Peb = NtCurrentPeb();
    PRTL_FLS_DATA pFlsData;

    RtlAcquirePebLock();

    pFlsData = NtCurrentTeb()->FlsData;

    if (!Peb->FlsCallback &&
        !(Peb->FlsCallback = RtlAllocateHeap(RtlGetProcessHeap(), HEAP_ZERO_MEMORY,
                                             FLS_MAXIMUM_AVAILABLE * sizeof(PVOID))))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        dwFlsIndex = FLS_OUT_OF_INDEXES;
    }
    else
    {
        dwFlsIndex = RtlFindClearBitsAndSet(Peb->FlsBitmap, 1, 1);
        if (dwFlsIndex != FLS_OUT_OF_INDEXES)
        {
            if (!pFlsData &&
                !(pFlsData = RtlAllocateHeap(RtlGetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RTL_FLS_DATA))))
            {
                RtlClearBits(Peb->FlsBitmap, dwFlsIndex, 1);
                dwFlsIndex = FLS_OUT_OF_INDEXES;
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            }
            else
            {
                if (!NtCurrentTeb()->FlsData)
                {
                    NtCurrentTeb()->FlsData = pFlsData;
                    InsertTailList(&Peb->FlsListHead, &pFlsData->ListEntry);
                }

                pFlsData->Data[dwFlsIndex] = NULL; /* clear the value */
                Peb->FlsCallback[dwFlsIndex] = lpCallback;

                if (dwFlsIndex > Peb->FlsHighIndex)
                    Peb->FlsHighIndex = dwFlsIndex;
            }
        }
        else
        {
            SetLastError(ERROR_NO_MORE_ITEMS);
        }
    }
    RtlReleasePebLock();
    return dwFlsIndex;
}


/*
 * @implemented
 */
BOOL
WINAPI
FlsFree(DWORD dwFlsIndex)
{
    BOOL ret;
    PPEB Peb = NtCurrentPeb();

    if (dwFlsIndex >= FLS_MAXIMUM_AVAILABLE)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    RtlAcquirePebLock();

    _SEH2_TRY
    {
        ret = RtlAreBitsSet(Peb->FlsBitmap, dwFlsIndex, 1);
        if (ret)
        {
            PLIST_ENTRY Entry;
            PFLS_CALLBACK_FUNCTION lpCallback;

            RtlClearBits(Peb->FlsBitmap, dwFlsIndex, 1);
            lpCallback = Peb->FlsCallback[dwFlsIndex];

            for (Entry = Peb->FlsListHead.Flink; Entry != &Peb->FlsListHead; Entry = Entry->Flink)
            {
                PRTL_FLS_DATA pFlsData;

                pFlsData = CONTAINING_RECORD(Entry, RTL_FLS_DATA, ListEntry);
                if (pFlsData->Data[dwFlsIndex])
                {
                    if (lpCallback)
                    {
                        lpCallback(pFlsData->Data[dwFlsIndex]);
                    }
                    pFlsData->Data[dwFlsIndex] = NULL;
                }
            }
            Peb->FlsCallback[dwFlsIndex] = NULL;
        }
        else
        {
            SetLastError(ERROR_INVALID_PARAMETER);
        }
    }
    _SEH2_FINALLY
    {
        RtlReleasePebLock();
    }
    _SEH2_END;

    return ret;
}

/*
 * @implemented
 */
PVOID
WINAPI
FlsGetValue(DWORD dwFlsIndex)
{
    PRTL_FLS_DATA pFlsData;

    pFlsData = NtCurrentTeb()->FlsData;
    if (!dwFlsIndex || dwFlsIndex >= FLS_MAXIMUM_AVAILABLE || !pFlsData)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    SetLastError(ERROR_SUCCESS);
    return pFlsData->Data[dwFlsIndex];
}


/*
 * @implemented
 */
BOOL
WINAPI
FlsSetValue(DWORD dwFlsIndex,
            PVOID lpFlsData)
{
    PRTL_FLS_DATA pFlsData;

    if (!dwFlsIndex || dwFlsIndex >= FLS_MAXIMUM_AVAILABLE)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    pFlsData = NtCurrentTeb()->FlsData;

    if (!NtCurrentTeb()->FlsData &&
        !(NtCurrentTeb()->FlsData = RtlAllocateHeap(RtlGetProcessHeap(), HEAP_ZERO_MEMORY,
                                                    sizeof(RTL_FLS_DATA))))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    if (!pFlsData)
    {
        pFlsData = NtCurrentTeb()->FlsData;
        RtlAcquirePebLock();
        InsertTailList(&NtCurrentPeb()->FlsListHead, &pFlsData->ListEntry);
        RtlReleasePebLock();
    }
    pFlsData->Data[dwFlsIndex] = lpFlsData;
    return TRUE;
}

/***********************************************************************
 *           ConvertThreadToFiberEx   (KERNEL32.@) - For XP support
 *
 * @implemented
 */
LPVOID
WINAPI
ConvertThreadToFiberEx(
	_In_opt_ LPVOID lpParameter,
    _In_ DWORD dwFlags
)
{
    PTEB Teb;
    PFIBER Fiber;

    /* Check for invalid flags */
    if (dwFlags & ~FIBER_FLAG_FLOAT_SWITCH)
    {
        /* Fail */
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    /* Are we already a fiber? */
    Teb = NtCurrentTeb();
    if (Teb->HasFiberData)
    {
        /* Fail */
        SetLastError(ERROR_ALREADY_FIBER);
        return NULL;
    }

    /* Allocate the fiber */
    Fiber = RtlAllocateHeap(RtlGetProcessHeap(),
                            0,
                            sizeof(FIBER));
    if (!Fiber)
    {
        /* Fail */
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    /* Copy some contextual data from the thread to the fiber */
    Fiber->FiberData = lpParameter;
    Fiber->ExceptionList = Teb->NtTib.ExceptionList;
    Fiber->StackBase = Teb->NtTib.StackBase;
    Fiber->StackLimit = Teb->NtTib.StackLimit;
    Fiber->DeallocationStack = Teb->DeallocationStack;
    Fiber->FlsData = Teb->FlsData;
    Fiber->GuaranteedStackBytes = Teb->GuaranteedStackBytes;
    Fiber->ActivationContextStackPointer = Teb->ActivationContextStackPointer;

    /* Save FPU State if requested, otherwise just the basic registers */
    Fiber->FiberContext.ContextFlags = (dwFlags & FIBER_FLAG_FLOAT_SWITCH) ?
                                       (CONTEXT_FULL | CONTEXT_FLOATING_POINT) :
                                       CONTEXT_FULL;

    /* Associate the fiber to the current thread */
    Teb->NtTib.FiberData = Fiber;
    Teb->HasFiberData = TRUE;

    /* Return opaque fiber data */
    return (LPVOID)Fiber;
}

/**********************************************************************
 *           SetThreadStackGuarantee   (KERNEL32.@)
 */
BOOL 
WINAPI 
SetThreadStackGuarantee(
	IN OUT PULONG StackSizeInBytes
)
{
	ULONG prev_size = NtCurrentTeb()->GuaranteedStackBytes;
	ULONG new_size = (*StackSizeInBytes + 4095) & ~4095;

	/* at least 2 pages on 64-bit */
	if (sizeof(void *) > sizeof(int) && new_size) new_size = max( new_size, 8192 );

	*StackSizeInBytes = prev_size;
	if (new_size >= (char *)NtCurrentTeb()->NtTib.StackBase - (char *)NtCurrentTeb()->DeallocationStack)
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}
	if (new_size > prev_size) NtCurrentTeb()->GuaranteedStackBytes = (new_size + 4095) & ~4095;
	return TRUE;
}

DWORD
WINAPI
ConsoleIMERoutine(
    IN LPVOID lpThreadParameter
    )
{
	HMODULE hkernel32 = GetModuleHandleA("kernel32.dll");
	pConsoleIMERoutine = (void *)GetProcAddress(hkernel32, "ConsoleIMERoutine");
	if(pConsoleIMERoutine){
		return pConsoleIMERoutine(lpThreadParameter);
	}else{		
		return 0;
	}   		
}

DWORD
WINAPI
CtrlRoutine(
    IN LPVOID lpThreadParameter
)
{
	HMODULE hkernel32 = GetModuleHandleA("kernel32.dll");
	pCtrlRoutine = (void *)GetProcAddress(hkernel32, "CtrlRoutine");
	if(pCtrlRoutine){
		return pCtrlRoutine(lpThreadParameter);
	}else{		
		return 0;
	}  	
}

/*
 * @implemented
 */
DWORD
WINAPI
GetThreadId(IN HANDLE Thread)
{
    THREAD_BASIC_INFORMATION ThreadBasic;
    NTSTATUS Status;

    Status = NtQueryInformationThread(Thread,
                                      ThreadBasicInformation,
                                      &ThreadBasic,
                                      sizeof(THREAD_BASIC_INFORMATION),
                                      NULL);
    if (!NT_SUCCESS(Status))
    {
        BaseSetLastNTError(Status);
        return 0;
    }

    return HandleToUlong(ThreadBasic.ClientId.UniqueThread);
}

DWORD
APIENTRY
GetProcessIdOfThread(
    HANDLE Thread
    )
{
    NTSTATUS Status;
    THREAD_BASIC_INFORMATION ThreadBasic;

    Status = NtQueryInformationThread (Thread,
                                       ThreadBasicInformation,
                                       &ThreadBasic,
                                       sizeof (ThreadBasic),
                                       NULL);

    if (!NT_SUCCESS (Status)) {
        BaseSetLastNTError (Status);
        return 0;
    }

    return HandleToUlong (ThreadBasic.ClientId.UniqueProcess);
}

BOOL 
WINAPI 
GetThreadInformation(
	HANDLE ProcessHandle, 
	THREADINFOCLASS ThreadInformationClass,
	PVOID ThreadInformation, 
	DWORD ThreadInformationSize
)
{
   // BOOL result = FALSE; // esi@2
   // NTSTATUS status; // eax@3

   // if ( ThreadInformationClass )
   // {
    // BaseSetLastNTError(STATUS_INVALID_PARAMETER);
	// return FALSE;
   // }
   // else
   // {
    // status = NtQueryInformationProcess(
               // ProcessHandle,
               // ProcessDebugPort|0x20,
               // ThreadInformation,
               // ThreadInformationSize,
               // 0);
    // if ( NT_SUCCESS(status) )
      // result = TRUE;
    // else
      // BaseSetLastNTError(status);
   // }
   // return result;
   SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
   return FALSE;
}

/***********************************************************************
 *              CreateThreadpool (KERNEL32.@)
 */
PTP_POOL 
WINAPI 
CreateThreadpool( 
	PVOID reserved 
)
{
    TP_POOL *pool;
    NTSTATUS status;

    //TRACE( "%p\n", reserved );

    status = TpAllocPool( &pool, reserved );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return NULL;
    }

    return pool;
}

/***********************************************************************
 *              CreateThreadpoolWork (KERNEL32.@)
 */
PTP_WORK 
WINAPI 
CreateThreadpoolWork( 
	PTP_WORK_CALLBACK callback, 
	PVOID userdata,
	TP_CALLBACK_ENVIRON *environment 
)
{
    TP_WORK *work;
    NTSTATUS status;

    status = TpAllocWork( &work, callback, userdata, environment );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return NULL;
    }

    return work;
}

/***********************************************************************
 *              CreateThreadpoolTimer (KERNEL32.@)
 */
PTP_TIMER 
WINAPI 
CreateThreadpoolTimer( 
	PTP_TIMER_CALLBACK callback, 
	PVOID userdata,
	TP_CALLBACK_ENVIRON *environment
)
{
    TP_TIMER *timer;
    NTSTATUS status;

    status = TpAllocTimer( &timer, callback, userdata, environment );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return NULL;
    }

    return timer;
}

/***********************************************************************
 *              CreateThreadpoolWait (KERNEL32.@)
 */
PTP_WAIT 
WINAPI 
CreateThreadpoolWait( 
	PTP_WAIT_CALLBACK callback, 
	PVOID userdata,
	TP_CALLBACK_ENVIRON *environment 
)
{
    TP_WAIT *wait;
    NTSTATUS status;

    status = TpAllocWait( &wait, callback, userdata, environment );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return NULL;
    }

    return wait;
}

/***********************************************************************
 *              GetThreadGroupAffinity (KERNEL32.@)
 */
BOOL WINAPI GetThreadGroupAffinity(HANDLE hThread, PGROUP_AFFINITY GroupAffinity)
{
    HANDLE hProcess;
    DWORD_PTR ProcessAffinityMask;
    DWORD_PTR SystemAffinityMask;    
    DWORD Pid = GetProcessIdOfThread(hThread);
    BOOL Res;
    if (!GroupAffinity) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    
    if(!Pid)
        return FALSE;
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, Pid);
    if(!hProcess)
        return FALSE;
    Res = GetProcessAffinityMask(hProcess, &ProcessAffinityMask, &SystemAffinityMask);
    CloseHandle(hProcess);
    if (Res) {
        GroupAffinity->Mask = ProcessAffinityMask;
        GroupAffinity->Group = 0;
    }
    return Res;
}

/***********************************************************************
 *              SetThreadGroupAffinity (KERNEL32.@)
 */
BOOL WINAPI SetThreadGroupAffinity(HANDLE hThread, const GROUP_AFFINITY *GroupAffinity, PGROUP_AFFINITY PreviousGroupAffinity)
{
    DWORD Pid;
    HANDLE hProcess;
    DWORD_PTR ProcessAffinityMask;
    DWORD_PTR SystemAffinityMask;
    BOOL Res;
    
    if (!GroupAffinity) {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    } else if (!PreviousGroupAffinity) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    Pid = GetProcessIdOfThread(hThread);
    if (!Pid)
        return FALSE;
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, Pid);
    if (!hProcess)
        return FALSE;
    Res = GetProcessAffinityMask(hProcess, &ProcessAffinityMask, &SystemAffinityMask);
    CloseHandle(hProcess);
    if (!Res)
        return FALSE;
    PreviousGroupAffinity->Mask = ProcessAffinityMask;
    PreviousGroupAffinity->Group = 0;
    return SetThreadAffinityMask(hThread, GroupAffinity->Mask);
}

/*************************************************************************
 *              rtlmode_to_win32mode
 */
static 
DWORD 
rtlmode_to_win32mode( 
	DWORD rtlmode 
)
{
    DWORD win32mode = 0;

    if (rtlmode & 0x10)
        win32mode |= SEM_FAILCRITICALERRORS;
    if (rtlmode & 0x20)
        win32mode |= SEM_NOGPFAULTERRORBOX;
    if (rtlmode & 0x40)
        win32mode |= SEM_NOOPENFILEERRORBOX;

    return win32mode;
}

/***********************************************************************
 *              SetThreadErrorMode (KERNEL32.@)
 *
 * Set the thread local error mode.
 *
 * PARAMS
 *  mode    [I] The new error mode, a bitwise or of SEM_FAILCRITICALERRORS,
 *              SEM_NOGPFAULTERRORBOX and SEM_NOOPENFILEERRORBOX.
 *  oldmode [O] Destination of the old error mode (may be NULL)
 *
 * RETURNS
 *  Success: TRUE
 *  Failure: FALSE, check GetLastError
 */
BOOL 
WINAPI 
SetThreadErrorMode( 
	DWORD mode, 
	LPDWORD oldmode 
)
{
    NTSTATUS status;
    DWORD tmp = 0;

    if (mode & ~(SEM_FAILCRITICALERRORS |
                 SEM_NOGPFAULTERRORBOX |
                 SEM_NOOPENFILEERRORBOX))
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if (mode & SEM_FAILCRITICALERRORS)
        tmp |= 0x10;
    if (mode & SEM_NOGPFAULTERRORBOX)
        tmp |= 0x20;
    if (mode & SEM_NOOPENFILEERRORBOX)
        tmp |= 0x40;

    status = RtlSetThreadErrorMode( tmp, oldmode );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return FALSE;
    }

    if (oldmode)
        *oldmode = rtlmode_to_win32mode(*oldmode);

    return TRUE;
}

/***********************************************************************
 *              GetThreadErrorMode (KERNEL32.@)
 *
 * Get the thread local error mode.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The current thread local error mode.
 */
DWORD 
WINAPI 
GetThreadErrorMode( void )
{
    return rtlmode_to_win32mode( RtlGetThreadErrorMode() );
}

VOID 
WINAPI 
RaiseFailFastException(
	EXCEPTION_RECORD *record, 
	CONTEXT *context, 
	DWORD flags
)
{
    EXCEPTION_RECORD rec;
    CONTEXT ctx;

    if (!context)
    {
        ctx.ContextFlags = CONTEXT_FULL;
        NtGetContextThread(GetCurrentThread(), &ctx);
        context = &ctx;
    }

    if (!record)
    {
        rec.ExceptionCode    = STATUS_FAIL_FAST_EXCEPTION;
        rec.ExceptionFlags   = 0;
        rec.ExceptionRecord  = NULL;
        rec.ExceptionAddress = RaiseFailFastException;
        rec.NumberParameters = 0;
        record = &rec;
    }

    if (!NtCurrentTeb()->ProcessEnvironmentBlock->BeingDebugged)
    {
        EXCEPTION_POINTERS epointers;

        epointers.ExceptionRecord = record;
        epointers.ContextRecord = context;
        start_debugger_atomic(&epointers);
    }
}
 
BOOL 
WINAPI 
QueryThreadCycleTime(
  _In_  HANDLE   ThreadHandle,
  _Out_ PULONG64 CycleTime
)
{
	LARGE_INTEGER ltime;
	UINT32 cycles; 
	QueryPerformanceCounter(&ltime);

	cycles = (UINT32) ((ltime.QuadPart >> 8) & 0xFFFFFFF);	
	
	*CycleTime = cycles;
	return TRUE;
}

HANDLE 
WINAPI
CreateRemoteThreadEx(
  _In_      HANDLE                       hProcess,
  _In_opt_  LPSECURITY_ATTRIBUTES        lpThreadAttributes,
  _In_      SIZE_T                       dwStackSize,
  _In_      LPTHREAD_START_ROUTINE       lpStartAddress,
  _In_opt_  LPVOID                       lpParameter,
  _In_      DWORD                        dwCreationFlags,
  _In_opt_  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
  _Out_opt_ LPDWORD                      lpThreadId
)
{
	HANDLE handle = NULL;
	
	handle = CreateRemoteThread(hProcess,
								lpThreadAttributes,
								dwStackSize,
								lpStartAddress,
								lpParameter,
								dwCreationFlags,
								lpThreadId);
	//TODO create Thread with ThreadAttibutesList
	return handle;	
}

/***********************************************************************
 *           IsThreadAFiber   (KERNEL32.@)
 */
BOOL WINAPI IsThreadAFiber(void)
{
	//return NtCurrentTeb()->NtTib.FiberData != NULL;
    return NtCurrentTeb()->HasFiberData;
}

/***********************************************************************
 *              SetThreadpoolTimerEx (KERNEL32.@)
 */
BOOL WINAPI SetThreadpoolTimerEx( TP_TIMER *timer, FILETIME *due_time,
                                DWORD period, DWORD window_length )
{
    LARGE_INTEGER timeout;

    //TRACE( "%p, %p, %u, %u\n", timer, due_time, period, window_length );

    if (due_time)
    {
        timeout.u.LowPart = due_time->dwLowDateTime;
        timeout.u.HighPart = due_time->dwHighDateTime;
    }

    return TpSetTimerEx( timer, due_time ? &timeout : NULL, period, window_length );
}

/***********************************************************************
 *              SetThreadpoolTimer (KERNEL32.@)
 */
VOID WINAPI SetThreadpoolTimer( TP_TIMER *timer, FILETIME *due_time,
                                DWORD period, DWORD window_length )
{
    SetThreadpoolTimerEx(timer, due_time, period, window_length);
}

/***********************************************************************
 *              SetThreadpoolWaitEx (KERNEL32.@)
 */
BOOL WINAPI SetThreadpoolWaitEx( TP_WAIT *wait, HANDLE handle, FILETIME *due_time, PVOID reserved )
{
    LARGE_INTEGER timeout;

    //TRACE( "%p, %p, %p\n", wait, handle, due_time );

    if (!handle)
    {
        due_time = NULL;
    }
    else if (due_time)
    {
        timeout.u.LowPart = due_time->dwLowDateTime;
        timeout.u.HighPart = due_time->dwHighDateTime;
    }

    return TpSetWaitEx( wait, handle, due_time ? &timeout : NULL, reserved );
}


/***********************************************************************
 *              SetThreadpoolWait (KERNEL32.@)
 */
VOID WINAPI SetThreadpoolWait( TP_WAIT *wait, HANDLE handle, FILETIME *due_time )
{
    SetThreadpoolWaitEx(wait, handle, due_time, NULL);
}

/***********************************************************************
 *              TrySubmitThreadpoolCallback (KERNEL32.@)
 */
BOOL WINAPI TrySubmitThreadpoolCallback( PTP_SIMPLE_CALLBACK callback, PVOID userdata,
                                         TP_CALLBACK_ENVIRON *environment )
{
    NTSTATUS status;

    //TRACE( "%p, %p, %p\n", callback, userdata, environment );

    status = TpSimpleTryPost( callback, userdata, environment );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return FALSE;
    }

    return TRUE;
}

/***********************************************************************
 *              CallbackMayRunLong (KERNEL32.@)
 */
BOOL WINAPI CallbackMayRunLong( TP_CALLBACK_INSTANCE *instance )
{
    NTSTATUS status;

    //TRACE( "%p\n", instance );

    status = TpCallbackMayRunLong( instance );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return FALSE;
    }

    return TRUE;
}

/***********************************************************************
 *              CreateThreadpoolCleanupGroup (KERNEL32.@)
 */
PTP_CLEANUP_GROUP WINAPI CreateThreadpoolCleanupGroup( void )
{
    TP_CLEANUP_GROUP *group;
    NTSTATUS status;

    //TRACE( "\n" );

    status = TpAllocCleanupGroup( &group );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return NULL;
    }

    return group;
}

/**********************************************************************
*           SetThreadIdealProcessorEx   (KERNEL32.@)
*/
// Fixed bug where it crashes when "previous" is set to NULL 
BOOL 
WINAPI 
SetThreadIdealProcessorEx(
    HANDLE hThread, 
    PPROCESSOR_NUMBER processor, 
    PPROCESSOR_NUMBER previous
)
{
    DWORD result;

    if (!processor || processor->Group > 0 || processor->Number > MAXIMUM_PROCESSORS)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    
    result = SetThreadIdealProcessor(hThread, processor->Number);
    if (result == -1)
        return FALSE;
    
    if (previous)
    {
        previous->Group = 0;
        previous->Number = result;
        previous->Reserved = 0;
    }
    
    return TRUE;
}

BOOL
WINAPI
SetThreadPriorityInternal(
    HANDLE hThread,
    int nPriority
)
{
	BOOL ret;
	
	ret = SetThreadPriority(hThread,
							nPriority);
	
	
	DbgPrint("SetThreadPriorityInternal :: SetThreadPriority returned: %d\n", ret);
	
	return ret;
}

LONG
WINAPI
BasepCheckForReadOnlyResource(
    PVOID Va
    )
{
    SIZE_T RegionSize;
    ULONG OldProtect;
    NTSTATUS Status;
    MEMORY_BASIC_INFORMATION MemInfo;
    PIMAGE_RESOURCE_DIRECTORY ResourceDirectory;
    ULONG ResourceSize;
    char *rbase, *va;
    LONG ReturnValue;

    //
    // Locate the base address that continas this va
    //

    Status = NtQueryVirtualMemory(
                NtCurrentProcess(),
                Va,
                MemoryBasicInformation,
                (PVOID)&MemInfo,
                sizeof(MemInfo),
                NULL
                );
    if ( !NT_SUCCESS(Status) ) {
        return EXCEPTION_CONTINUE_SEARCH;
        }

    //
    // if the va is readonly and in an image then continue
    //

    if ( !((MemInfo.Protect == PAGE_READONLY) && (MemInfo.Type == MEM_IMAGE)) ){
        return EXCEPTION_CONTINUE_SEARCH;
        }

    ReturnValue = EXCEPTION_CONTINUE_SEARCH;

     _SEH2_TRY {
        ResourceDirectory = (PIMAGE_RESOURCE_DIRECTORY)
            RtlImageDirectoryEntryToData(MemInfo.AllocationBase,
                                         TRUE,
                                         IMAGE_DIRECTORY_ENTRY_RESOURCE,
                                         &ResourceSize
                                         );

        rbase = (char *)ResourceDirectory;
        va = (char *)Va;

        if ( rbase && va >= rbase && va < rbase+ResourceSize ) {
            RegionSize = 1;
            Status = NtProtectVirtualMemory(
                        NtCurrentProcess(),
                        &va,
                        &RegionSize,
                        PAGE_READWRITE,
                        &OldProtect
                        );
            if ( NT_SUCCESS(Status) ) {
                ReturnValue = EXCEPTION_CONTINUE_EXECUTION;
                }
            }
        }
    _SEH2_EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
        ;
        }
		
	_SEH2_END;	

    return ReturnValue;
}

/***********************************************************************
 *              CreateThreadpoolIo (KERNEL32.@)
 */
PTP_IO 
WINAPI 
CreateThreadpoolIo(
    HANDLE                file,
    PTP_WIN32_IO_CALLBACK pfnio,
    PVOID                 pv,
    PTP_CALLBACK_ENVIRON  pcbe
)
{
    TP_IO *io;
    NTSTATUS status;

    //TRACE( "%p, %p, %p\n", pfnio, pv, pcbe );

    status = TpAllocIoCompletion( &io, file, (PTP_IO_CALLBACK)pfnio, pv, pcbe );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return NULL;
    }

    return io;
}

BOOL  
WINAPI
GetThreadIdealProcessorEx(
	HANDLE hThread, 
	PPROCESSOR_NUMBER lpIdealProcessor
)
{
	lpIdealProcessor->Number = SetThreadIdealProcessor(hThread, MAXIMUM_PROCESSORS);
	lpIdealProcessor->Group = 0;

	if(lpIdealProcessor->Number == -1)
		return FALSE;
	else
		return TRUE;
}

#define THREADDESC_SUCCESS 0x10000000
/***********************************************************************
 * SetThreadDescription [KERNEL32.@]  Sets name of thread.
 *
 * RETURNS
 *    Success: TRUE
 *    Failure: FALSE
 */
HRESULT WINAPI SetThreadDescription( HANDLE handle, const WCHAR *descW )
{
    //TRACE("(%p,%s)\n", handle, wine_dbgstr_w( descW ));
    if (handle != GetCurrentThread())
    {
        FIXME("Can't set other thread description\n");
        return THREADDESC_SUCCESS;
    }

    return THREADDESC_SUCCESS;
}
/***********************************************************************
 * GetThreadDescription [KERNEL32.@]  Retrieves name of thread.
 *
 * RETURNS
 *    Success: TRUE
 *    Failure: FALSE
 */
HRESULT WINAPI GetThreadDescription( HANDLE handle, WCHAR **descW )
{
    *descW = LocalAlloc( 0, 16 * sizeof(WCHAR) );
    if(!*descW)
        return E_OUTOFMEMORY;
    if (handle != GetCurrentThread())
    {
        DbgPrint("Can't get other thread description\n");
        (*descW)[0] = 0;
        return THREADDESC_SUCCESS;
    }
    (*descW)[0] = 0;
	
    return THREADDESC_SUCCESS;
}

/***********************************************************************
 *		GetCurrentThreadStackLimits (KERNEL32.@)
 */
void WINAPI GetCurrentThreadStackLimits(ULONG_PTR *low, ULONG_PTR *high)
{
    *low = (ULONG_PTR)NtCurrentTeb()->DeallocationStack;
    *high = (ULONG_PTR)NtCurrentTeb()->NtTib.StackBase;
}

/***********************************************************************
 *           SetThreadpoolStackInformation   (kernelex.@)
 */
BOOL WINAPI DECLSPEC_HOTPATCH SetThreadpoolStackInformation( PTP_POOL pool, PTP_POOL_STACK_INFORMATION stack_info )
{
	NTSTATUS Status;
	
	Status = TpSetPoolStackInformation( pool, stack_info );
	
    return NT_SUCCESS(Status);
}

/***********************************************************************
 *           QueryThreadpoolStackInformation   (kernelex.@)
 */
BOOL WINAPI DECLSPEC_HOTPATCH QueryThreadpoolStackInformation( PTP_POOL pool, PTP_POOL_STACK_INFORMATION stack_info )
{
	NTSTATUS Status;
	
	Status = TpQueryPoolStackInformation( pool, stack_info );
	
    return NT_SUCCESS(Status);
}

/**********************************************************************
 *            SetThreadInformation   (kernelex.@)
 */
BOOL 
WINAPI 
DECLSPEC_HOTPATCH 
SetThreadInformation( 
	HANDLE thread, 
	THREAD_INFORMATION_CLASS info_class,
    VOID *info, 
	DWORD size
)
{
    switch (info_class)
    {
        case ThreadMemoryPriority:
            return set_ntstatus( NtSetInformationThread( thread, ThreadPagePriority, info, size ));
        // case ThreadPowerThrottling:
            // return set_ntstatus( NtSetInformationThread( thread, ThreadPowerThrottlingState, info, size ));
        case ThreadAbsoluteCpuPriority:
            NtSetInformationThread( thread, ThreadActualBasePriority, info, size );
            return TRUE;
        // The other two info_classes are only introduced on Windows 10, so we do not need to do anything.
        default:
			FIXME("Unsupported class %u.\n", info_class);		
            return TRUE;        
    }
}

/*
 * @implemented
 */
HANDLE
WINAPI
OpenThread(IN DWORD dwDesiredAccess,
           IN BOOL bInheritHandle,
           IN DWORD dwThreadId)
{
    NTSTATUS Status;
    HANDLE ThreadHandle;
    OBJECT_ATTRIBUTES ObjectAttributes;
    CLIENT_ID ClientId;
	
	if(dwDesiredAccess & THREAD_QUERY_LIMITED_INFORMATION)
	{
		dwDesiredAccess |= THREAD_QUERY_INFORMATION;
	}

	if(dwDesiredAccess & THREAD_SET_LIMITED_INFORMATION)
	{
		dwDesiredAccess |= THREAD_SET_INFORMATION;
	}	

    ClientId.UniqueProcess = 0;
    ClientId.UniqueThread = ULongToHandle(dwThreadId);

    InitializeObjectAttributes(&ObjectAttributes,
                               NULL,
                               (bInheritHandle ? OBJ_INHERIT : 0),
                               NULL,
                               NULL);

    Status = NtOpenThread(&ThreadHandle,
                          dwDesiredAccess,
                          &ObjectAttributes,
                          &ClientId);
    if (!NT_SUCCESS(Status))
    {
        BaseSetLastNTError(Status);
        return NULL;
    }

    return ThreadHandle;
}

HANDLE WINAPI GetCurrentTransaction()
{
	HANDLE result; // eax

	result = NtCurrentTeb()->CurrentTransactionHandle;
	if ( !result )
		return (HANDLE)-1;
	return result;
}

BOOLEAN WINAPI SetCurrentTransaction(HANDLE Transaction)
{
	NtCurrentTeb()->CurrentTransactionHandle = Transaction;
	return 1;
}

/***********************************************************************
 *           SetThreadSelectedCpuSets   (kernelbase.@)
 */
BOOL WINAPI SetThreadSelectedCpuSets(HANDLE thread, const ULONG *cpu_set_ids, ULONG count)
{
    FIXME( "thread %p, cpu_set_ids %p, count %lu stub.\n", thread, cpu_set_ids, count );

    return TRUE;
}

// For performance reasons... modern jemalloc will use these APIs if avaliable.
// A faster implementation of TlsGetValue.
LPVOID WINAPI TlsGetValue2( DWORD index )
{
    if (index < TLS_MINIMUM_AVAILABLE)
        return NtCurrentTeb()->TlsSlots[index];
       
    index -= TLS_MINIMUM_AVAILABLE;
    if (index >= 8 * sizeof(NtCurrentTeb()->ProcessEnvironmentBlock->TlsExpansionBitmapBits))
        return NULL;
    
    if (NtCurrentTeb()->TlsExpansionSlots) 
        return NtCurrentTeb()->TlsExpansionSlots[index];
    
    return NULL;
}

LPVOID WINAPI FlsGetValue2(DWORD dwFlsIndex) {
    DWORD LastError = RtlGetLastWin32Error();
    LPVOID Result = FlsGetValue(dwFlsIndex);
    RtlSetLastWin32Error(LastError);
    return Result;
}