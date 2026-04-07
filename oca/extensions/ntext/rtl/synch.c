/*++

Copyright (c) 2022  Shorthorn Project

Module Name:

    synch.c

Abstract:

    This module implements all NTAPI syncronization
    objects.

Author:

    Skulltrail 18-October-2022

Revision History:

--*/

#include <main.h>
#include <config.h>
#include <port.h>

#ifdef _WIN64
#define InterlockedBitTestAndSetPointer(ptr,val) InterlockedBitTestAndSet64((PLONGLONG)ptr,(LONGLONG)val)
#define InterlockedBitTestAndResetPointer(ptr,val) InterlockedBitTestAndReset64((PLONGLONG)ptr,(LONGLONG)val)
#define InterlockedAddPointer(ptr,val) InterlockedAdd64((PLONGLONG)ptr,(LONGLONG)val)
#define InterlockedAndPointer(ptr,val) InterlockedAnd64((PLONGLONG)ptr,(LONGLONG)val)
#define InterlockedOrPointer(ptr,val) InterlockedOr64((PLONGLONG)ptr,(LONGLONG)val)
#else
#define InterlockedBitTestAndSetPointer(ptr,val) InterlockedBitTestAndSet((PLONG)ptr,(LONG)val)
#define InterlockedBitTestAndResetPointer(ptr,val) InterlockedBitTestAndReset((PLONG)ptr,(LONG)val)
#define InterlockedAddPointer(ptr,val) InterlockedAdd((PLONG)ptr,(LONG)val)
#define InterlockedAndPointer(ptr,val) InterlockedAnd((PLONG)ptr,(LONG)val)
#define InterlockedOrPointer(ptr,val) InterlockedOr((PLONG)ptr,(LONG)val)
#endif

#define InterlockedExchangeAdd16 _InterlockedExchangeAdd16 

static DWORD ConditionVariableSpinCount = 256;
static DWORD SRWLockSpinCount = 256;


static LARGE_INTEGER* __fastcall NtFormatTimeOut(LARGE_INTEGER* Timeout, DWORD dwMilliseconds)
{
    if (dwMilliseconds == INFINITE)
        return NULL;
    Timeout->QuadPart = -10000ll * dwMilliseconds;
    return Timeout;
}

static NTSTATUS NTAPI SecondWaitWorkaroundNtWaitForKeyedEvent(
    IN HANDLE               KeyedEventHandle,
    IN YY_CV_WAIT_BLOCK*    Key,
    IN BOOLEAN              Alertable,
    IN PLARGE_INTEGER       Timeout OPTIONAL
)
{
	NTSTATUS Status;
   // 目前只有Windows XP收到报告说会卡死，所以我们这里判断一下
   if (Timeout == NULL)// && NtCurrentTeb()->ProcessEnvironmentBlock->OSMajorVersion < 6)
   {
       LARGE_INTEGER _nTimeOut;
       NtFormatTimeOut(&_nTimeOut, 0);
       for (; Key->uWakeupThreadId == 0;)
       {
           Status = NtWaitForKeyedEvent(KeyedEventHandle, (PVOID)Key, Alertable, &_nTimeOut);
           if (Status != STATUS_TIMEOUT)
              return STATUS_TIMEOUT;
           }

           // 等5毫秒应该足够唤醒线程调用Release了
           // 这里只是经验假设，可能不能彻底规避问题。但是正常情况下应该足以缓解死等问题。
           NtWaitForKeyedEvent(KeyedEventHandle, Key, Alertable, NtFormatTimeOut(&_nTimeOut, 5));
           return STATUS_SUCCESS;
    }
    return NtWaitForKeyedEvent(KeyedEventHandle, Key, Alertable, Timeout);
}

static NTSTATUS NTAPI SecondWaitWorkaroundNtReleaseKeyedEvent(
	IN HANDLE               KeyedEventHandle,
	IN YY_CV_WAIT_BLOCK*             Key,
	IN BOOLEAN              Alertable,
	IN PLARGE_INTEGER       Timeout OPTIONAL
)
{
   InterlockedExchange((volatile long*)&Key->uWakeupThreadId, HandleToUlong(NtCurrentTeb()->ClientId.UniqueThread));
   return NtReleaseKeyedEvent(KeyedEventHandle, Key, Alertable, Timeout);
}


VOID InitializeGlobalKeyedEventHandle()
{
    if (GlobalKeyedEventHandle == NULL)
    {
        HANDLE KeyedEventHandle = NULL;
        const wchar_t Name[] = L"\\KernelObjects\\CritSecOutOfMemoryEvent";
        UNICODE_STRING ObjectName = { sizeof(Name) - sizeof(wchar_t), sizeof(Name) - sizeof(wchar_t), (PWSTR)Name };
        OBJECT_ATTRIBUTES attr = { sizeof(attr), NULL, &ObjectName, 0, NULL, NULL };

        NTSTATUS status = NtOpenKeyedEvent(&KeyedEventHandle, MAXIMUM_ALLOWED, &attr);
        if (!NT_SUCCESS(status))
        {
            RtlRaiseStatus(STATUS_RESOURCE_NOT_OWNED);
            return;
        }

        if (InterlockedCompareExchangePointer((PVOID volatile *)&GlobalKeyedEventHandle, KeyedEventHandle, NULL) != NULL)
        {
            NtClose(KeyedEventHandle);
        }
    }
}

/******************************************************************
 *              RtlRunOnceBeginInitialize (NTDLL.@)
 */
DWORD 
NTAPI 
RtlRunOnceBeginInitialize( 
    RTL_RUN_ONCE *once, 
    ULONG flags, 
    void **context 
)
{
	InitializeGlobalKeyedEventHandle();
    if (flags & RTL_RUN_ONCE_CHECK_ONLY)
    {
        ULONG_PTR val = (ULONG_PTR)once->Ptr;
        if (flags & RTL_RUN_ONCE_ASYNC) return STATUS_INVALID_PARAMETER;
        if ((val & 3) != 2) return STATUS_UNSUCCESSFUL;
        if (context) *context = (void *)(val & ~3);
        return STATUS_SUCCESS;
    } 
    for (;;)
    {
        ULONG_PTR next, val = (ULONG_PTR)once->Ptr;
        switch (val & 3)
        {
        case 0:  /* first time */
            if (!interlocked_cmpxchg_ptr( &once->Ptr,
                                          (flags & RTL_RUN_ONCE_ASYNC) ? (void *)3 : (void *)1, 0 ))
                return STATUS_PENDING;
            break;
        case 1:  /* in progress, wait */			
            if (flags & RTL_RUN_ONCE_ASYNC) return STATUS_INVALID_PARAMETER;
            next = val & ~3;
            if (interlocked_cmpxchg_ptr( &once->Ptr, (void *)((ULONG_PTR)&next | 1),
                                         (void *)val ) == (void *)val)
                NtWaitForKeyedEvent( GlobalKeyedEventHandle, &next, FALSE, NULL );
            break;
        case 2:  /* done */
            if (context) *context = (void *)(val & ~3);
            return STATUS_SUCCESS;
        case 3:  /* in progress, async */
            if (!(flags & RTL_RUN_ONCE_ASYNC)) return STATUS_INVALID_PARAMETER;
            return STATUS_PENDING;
        }
    }
}

/******************************************************************
 *              RtlRunOnceComplete (NTDLL.@)
 */
DWORD
NTAPI
RtlRunOnceComplete( 
    PRTL_RUN_ONCE once, 
    ULONG flags, 
    PVOID context 
)
{
	InitializeGlobalKeyedEventHandle();
    if ((ULONG_PTR)context & 3) return STATUS_INVALID_PARAMETER;
    if (flags & RTL_RUN_ONCE_INIT_FAILED)
    {
        if (context) return STATUS_INVALID_PARAMETER;
        if (flags & RTL_RUN_ONCE_ASYNC) return STATUS_INVALID_PARAMETER;
    }
    else context = (void *)((ULONG_PTR)context | 2);
	
    for (;;)
    {
        ULONG_PTR val = (ULONG_PTR)once->Ptr;
        switch (val & 3)
        {
        case 1:  /* in progress */
            if (interlocked_cmpxchg_ptr( &once->Ptr, context, (void *)val ) != (void *)val) break;
            val &= ~3;
            while (val)
            {
                ULONG_PTR next = *(ULONG_PTR *)val;
                NtReleaseKeyedEvent( GlobalKeyedEventHandle, (void *)val, FALSE, NULL );
                val = next;
            }
            return STATUS_SUCCESS;
        case 3:  /* in progress, async */
            if (!(flags & RTL_RUN_ONCE_ASYNC)) return STATUS_INVALID_PARAMETER;
            if (interlocked_cmpxchg_ptr( &once->Ptr, context, (void *)val ) != (void *)val) break;
            return STATUS_SUCCESS;
        default:
            return STATUS_UNSUCCESSFUL;
        }
    }
}

/******************************************************************
  *              RtlRunOnceExecuteOnce (NTDLL.@)
  */
DWORD 
NTAPI 
RtlRunOnceExecuteOnce( 
    RTL_RUN_ONCE *once, 
    PRTL_RUN_ONCE_INIT_FN func,
    void *param, void **context 
)
{
     DWORD ret = RtlRunOnceBeginInitialize( once, 0, context );
 
     if (ret != STATUS_PENDING) return ret;
 
     if (!func( once, param, context ))
     {
         RtlRunOnceComplete( once, RTL_RUN_ONCE_INIT_FAILED, NULL );
         return STATUS_UNSUCCESSFUL;
     } 
     return RtlRunOnceComplete( once, 0, context ? *context : NULL );
}

/******************************************************************
 *              RtlRunOnceInitialize (NTDLL.@)
 */
void 
NTAPI 
RtlRunOnceInitialize( 
    RTL_RUN_ONCE *once 
)
{
    once->Ptr = NULL;
}

//New ConditionVariable API
void NTAPI RtlpInitConditionVariable(PEB* pPeb)
{
    if (pPeb->NumberOfProcessors == 1)
    {
        ConditionVariableSpinCount = 0;
        SRWLockSpinCount = 0;
    }
}

/***********************************************************************
 *           RtlInitializeConditionVariable   (NTDLL.@)
 */
void NTAPI RtlInitializeConditionVariable( RTL_CONDITION_VARIABLE *variable )
{
    variable->Ptr = NULL;
}

//将等待块插入 SRWLock 中
static BOOL __fastcall RtlpQueueWaitBlockToSRWLock(YY_CV_WAIT_BLOCK* pBlock, RTL_SRWLOCK *SRWLock, ULONG SRWLockMark)
{
    size_t shareCount;
    size_t Current;
    size_t New;
    USHORT backoff = 0;
				
    for (;;)
    {
        Current = *(volatile size_t*)SRWLock;
        if ((Current & 0x1) == 0)
            break;
        if (SRWLockMark == 0)
        {
            pBlock->flag |= 0x1;
        }
        else if ((Current & 0x2) == 0 && YY_SRWLOCK_GET_BLOCK(Current))
        {
            return FALSE;
        }
        pBlock->next = NULL;					
        if (Current & 0x2)
        {
            pBlock->notify = NULL;
            pBlock->shareCount = 0;
            pBlock->back = (YY_CV_WAIT_BLOCK*)YY_SRWLOCK_GET_BLOCK(Current);
            New = (size_t)(pBlock) | (Current & YY_CV_MASK);
        }
        else
        {
            shareCount = Current >> 4;
            pBlock->shareCount = shareCount;
            pBlock->notify = pBlock;
            New = shareCount <= 1 ? (size_t)(pBlock) | 0x3 : (size_t)(pBlock) | 0xB;
        }
        if ((size_t)InterlockedCompareExchangePointer((void *volatile *)SRWLock, (void*)New, (void*)Current) == Current)
            return TRUE;
        RtlBackoff(&backoff);
    }
    return FALSE;
}

static void __fastcall RtlpWakeConditionVariable(RTL_CONDITION_VARIABLE *ConditionVariable, size_t ConditionVariableStatus, size_t WakeCount)
{
    YY_CV_WAIT_BLOCK* notify = NULL;
    YY_CV_WAIT_BLOCK* pWake = NULL;
    YY_CV_WAIT_BLOCK* pWaitBlock;
    YY_CV_WAIT_BLOCK* pBlock;
    YY_CV_WAIT_BLOCK* tmp;
    YY_CV_WAIT_BLOCK* next;
    YY_CV_WAIT_BLOCK* back;
    YY_CV_WAIT_BLOCK** ppInsert = &pWake;
    size_t LastStatus;
    size_t MaxWakeCount;
    size_t Count = 0;
    USHORT backoff = 0;
    int steps = 0; 

    for (;;)
    {
        pWaitBlock = YY_CV_GET_BLOCK(ConditionVariableStatus);
        if ((ConditionVariableStatus & 0x7) == 0x7)
        {
            ConditionVariableStatus = (size_t)InterlockedExchangePointer((void *volatile *)ConditionVariable, 0);
            *ppInsert = YY_CV_GET_BLOCK(ConditionVariableStatus);
            break;
        }
        MaxWakeCount = WakeCount + (ConditionVariableStatus & 7);
        pBlock = pWaitBlock;

        steps = 0;                                      // reset before traversal
        for (; pBlock->notify == NULL;)
        {
            tmp = pBlock;
            pBlock = pBlock->back;
            pBlock->next = tmp;
            if (++steps % 32 == 0)
                RtlBackoff(&backoff);
        }

        if (MaxWakeCount <= Count)
        {
            LastStatus = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)pWaitBlock, (void*)ConditionVariableStatus);
            if (LastStatus == ConditionVariableStatus)
                break;
            ConditionVariableStatus = LastStatus;
            RtlBackoff(&backoff);
            continue;
        }
        else
        {
            notify = pBlock->notify;

            steps = 0;                                  // reset before traversal
            for (; MaxWakeCount > Count && notify->next;)
            {
                ++Count;
                *ppInsert = notify;
                notify->back = NULL;
                next = notify->next;
                pWaitBlock->notify = next;
                next->back = NULL;
                ppInsert = &notify->back;
                notify = next;

                if (++steps % 32 == 0)
                    RtlBackoff(&backoff);
            }

            if (MaxWakeCount <= Count)
            {
                LastStatus = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)pWaitBlock, (void*)ConditionVariableStatus);
                if (LastStatus == ConditionVariableStatus)
                    break;
                ConditionVariableStatus = LastStatus;
                RtlBackoff(&backoff);
                continue;
            }
            else
            {
                LastStatus = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, 0, (void*)ConditionVariableStatus);
                if (LastStatus == ConditionVariableStatus)
                {
                    *ppInsert = notify;
                    notify->back = 0;
                    break;
                }
                ConditionVariableStatus = LastStatus;
                RtlBackoff(&backoff);
                continue;
            }
        }
    }

    for (; pWake;)
    {
        back = pWake->back;
        if (!InterlockedBitTestAndReset((volatile LONG*)&pWake->flag, 1))
        {
            if (pWake->SRWLock == NULL || RtlpQueueWaitBlockToSRWLock(pWake, pWake->SRWLock, (pWake->flag >> 2) & 0x1) == FALSE)
            {
                SecondWaitWorkaroundNtReleaseKeyedEvent(GlobalKeyedEventHandle, pWake, 0, NULL);
            }
        }
        pWake = back;
    }
}

VOID
NTAPI
RtlWakeConditionVariable(
    _Inout_ RTL_CONDITION_VARIABLE *ConditionVariable
)
{
    size_t Current;
    size_t Last;
    USHORT backoff = 0;

    InitializeGlobalKeyedEventHandle();

    Current = *(volatile size_t*)ConditionVariable;
    for (; Current; Current = Last)
    {
        if (Current & 0x8)
        {
		if ((Current & 0x7) == 0x7)
                return;
            Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)(Current + 1), (void*)Current);
            if (Last == Current)
                return;
        }
        else
        {
            Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)(Current | 0x8), (void*)Current);
            if (Last == Current)
            {
                RtlpWakeConditionVariable(ConditionVariable, Current + 8, 1);
                return;
            }
        }
        RtlBackoff(&backoff);
    }
}
		
VOID
NTAPI
RtlWakeAllConditionVariable(
    _Inout_ RTL_CONDITION_VARIABLE *ConditionVariable
)
{
    size_t Current;
    size_t Last;
    YY_CV_WAIT_BLOCK* pBlock;
    YY_CV_WAIT_BLOCK* Tmp;
    USHORT backoff = 0;

    InitializeGlobalKeyedEventHandle();

    Current = *(volatile size_t*)ConditionVariable;
    for (; Current && (Current & 0x7) != 0x7; Current = Last)
    {
        if (Current & 0x8)
        {
            Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)(Current | 0x7), (void*)Current);
            if (Last == Current)
                return;
        }
        else
        {
            Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, 0, (void*)Current);
            if (Last == Current)
            {
                for (pBlock = YY_CV_GET_BLOCK(Current); pBlock;)
                {
                    Tmp = pBlock->back;
                    if (!InterlockedBitTestAndReset((volatile LONG*)&pBlock->flag, 1))
                    {
                        SecondWaitWorkaroundNtReleaseKeyedEvent(GlobalKeyedEventHandle, pBlock, FALSE, NULL);
                    }
                    pBlock = Tmp;
                }
                return;
            }
        }
        RtlBackoff(&backoff);
    }
}
		
static void __fastcall RtlpOptimizeConditionVariableWaitList(RTL_CONDITION_VARIABLE *ConditionVariable, size_t ConditionVariableStatus)
{
    YY_CV_WAIT_BLOCK *pWaitBlock;
    YY_CV_WAIT_BLOCK *pItem;
    YY_CV_WAIT_BLOCK *temp;
    size_t LastStatus;
    USHORT backoff = 0;
    int steps = 0;

    for (;;)
    {
        pWaitBlock = YY_CV_GET_BLOCK(ConditionVariableStatus);
        pItem = pWaitBlock;

        steps = 0;
        for (; pItem->notify == NULL;)
        {
            temp = pItem;
            pItem = pItem->back;
            pItem->next = temp;
            if (++steps % 32 == 0)
                RtlBackoff(&backoff);
        }

        pWaitBlock->notify = pItem->notify;

        LastStatus = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)pWaitBlock, (void*)ConditionVariableStatus);
        if (LastStatus == ConditionVariableStatus)
            return;

        if (LastStatus & 7)
        {
            RtlpWakeConditionVariable(ConditionVariable, LastStatus, 0);
            return;
        }
        ConditionVariableStatus = LastStatus;
        RtlBackoff(&backoff);
    }
}

static BOOL __fastcall RtlpWakeSingle(RTL_CONDITION_VARIABLE *ConditionVariable, YY_CV_WAIT_BLOCK* pBlock)
{
    size_t Current = (size_t)ConditionVariable;
    YY_CV_WAIT_BLOCK *pWaitBlock;
    YY_CV_WAIT_BLOCK *pSuccessor;
    size_t Last;
    size_t New;
    size_t back;
    YY_CV_WAIT_BLOCK* notify;
    BOOL bRet;
    USHORT backoff = 0;
    int steps = 0;

    for (; Current && (Current & 0x7) != 0x7;)
    {
        if (Current & 0x8)
        {
            Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)(Current | 0x7), (void*)Current);
            if (Last == Current)
                return FALSE;
            Current = Last;
            RtlBackoff(&backoff);
            continue;
        }
        else
        {
            New = Current | 0x8;
            Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)New, (void*)Current);
            if (Last == Current)
            {
                Current = New;
                notify = NULL;
                bRet = FALSE;
                pWaitBlock = YY_CV_GET_BLOCK(Current);
                pSuccessor = pWaitBlock;

                if (pWaitBlock)
                {
                    steps = 0;
                    for (; pWaitBlock;)
                    {
                        if (pWaitBlock == pBlock)
                        {
                            if (notify)
                            {
                                pWaitBlock = pWaitBlock->back;
                                bRet = TRUE;
                                notify->back = pWaitBlock;
                                if (!pWaitBlock) break;
                                pWaitBlock->next = notify;
                            }
                            else
                            {
                                back = (size_t)(pWaitBlock->back);
                                New = back == 0 ? back : back ^ ((New ^ back) & 0xF);
                                Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)New, (void*)Current);
                                if (Last == Current)
                                {
                                    Current = New;
                                    if (back == 0) return TRUE;
                                    bRet = TRUE;
                                }
                                else
                                {
                                    Current = Last;
                                    RtlBackoff(&backoff);
                                }
                                pSuccessor = pWaitBlock = YY_CV_GET_BLOCK(Current);
                                notify = NULL;
                            }
                        }
                        else
                        {
                            pWaitBlock->next = notify;
                            notify = pWaitBlock;
                            pWaitBlock = pWaitBlock->back;

                            if (++steps % 32 == 0)
                                RtlBackoff(&backoff);
                        }
                    }
                    if (pSuccessor)
                        pSuccessor->notify = notify;
                }

                RtlpWakeConditionVariable(ConditionVariable, Current, 0);
                return bRet;
            }
            Current = Last;
            RtlBackoff(&backoff);
        }
    }
    return FALSE;
}

NTSTATUS
NTAPI
RtlSleepConditionVariableCS(
    _Inout_ RTL_CONDITION_VARIABLE *ConditionVariable,
    _Inout_ PRTL_CRITICAL_SECTION   CriticalSection,
    _In_    const LARGE_INTEGER *   dwMilliseconds
)
{
    YY_CV_WAIT_BLOCK StackWaitBlock;
    size_t OldConditionVariable;
    size_t NewConditionVariable;
    size_t LastConditionVariable;
    size_t SpinCount;
    NTSTATUS Status = STATUS_SUCCESS;
    USHORT backoff = 0;

	InitializeGlobalKeyedEventHandle();

    StackWaitBlock.next = NULL;
    StackWaitBlock.flag = 2;
    StackWaitBlock.SRWLock = NULL;
    StackWaitBlock.uWakeupThreadId = 0;

    OldConditionVariable = *(size_t*)ConditionVariable;			
    for (;;)
    {
        NewConditionVariable = (size_t)(&StackWaitBlock) | (OldConditionVariable & YY_CV_MASK);
        StackWaitBlock.back = YY_CV_GET_BLOCK(OldConditionVariable);
        if (StackWaitBlock.back)
        {
            StackWaitBlock.notify = NULL;
            NewConditionVariable |= 0x8;
        }
        else
        {
            StackWaitBlock.notify = &StackWaitBlock;
        }
        LastConditionVariable = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)NewConditionVariable, (void*)OldConditionVariable);
        if (LastConditionVariable == OldConditionVariable)
            break;

        OldConditionVariable = LastConditionVariable;
        RtlBackoff(&backoff);
    }

    RtlLeaveCriticalSection(CriticalSection);
    if ((OldConditionVariable ^ NewConditionVariable) & 0x8)
    {
        RtlpOptimizeConditionVariableWaitList(ConditionVariable, NewConditionVariable);
    }
	
    InitializeGlobalKeyedEventHandle();
    for (SpinCount = ConditionVariableSpinCount; SpinCount; --SpinCount)
    {
        if (!(StackWaitBlock.flag & 2))
            break;
        YieldProcessor();
    }			
    if (InterlockedBitTestAndReset((volatile LONG*)&StackWaitBlock.flag, 1))
    {	
        Status = NtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, dwMilliseconds);
        if (Status == STATUS_TIMEOUT && RtlpWakeSingle(ConditionVariable, &StackWaitBlock) == FALSE)
        {
            SecondWaitWorkaroundNtWaitForKeyedEvent(GlobalKeyedEventHandle, &StackWaitBlock, 0, NULL);
            Status = STATUS_SUCCESS;
        }
    }
    RtlEnterCriticalSection(CriticalSection);
    return Status;
}

NTSTATUS
NTAPI
RtlSleepConditionVariableSRW(
    _Inout_ RTL_CONDITION_VARIABLE *ConditionVariable,
    _Inout_ RTL_SRWLOCK *SRWLock,
    _In_ PLARGE_INTEGER dwMilliseconds,
    _In_ ULONG Flags
)
{
    size_t SpinCount;
    YY_CV_WAIT_BLOCK StackWaitBlock;			
    size_t Current;
    size_t New;
    size_t Last;
    NTSTATUS Status = STATUS_SUCCESS;
    USHORT backoff = 0;
	
    do{
        if (Flags & ~RTL_CONDITION_VARIABLE_LOCKMODE_SHARED)
            break;		
        StackWaitBlock.next = NULL;
        StackWaitBlock.flag = 2;
        StackWaitBlock.SRWLock = NULL;
        StackWaitBlock.uWakeupThreadId = 0;
        if (Flags& RTL_CONDITION_VARIABLE_LOCKMODE_SHARED)
            StackWaitBlock.flag |= 0x4;
        Current = *(volatile size_t*)ConditionVariable;
        for (;;)
        {
            New = (size_t)(&StackWaitBlock) | (Current & YY_CV_MASK);
            if (StackWaitBlock.back = YY_CV_GET_BLOCK(Current))
            {
                StackWaitBlock.notify = NULL;
                New |= 0x8;
            }
            else
            {
                StackWaitBlock.notify = &StackWaitBlock;
            }
            Last = (size_t)InterlockedCompareExchangePointer((void *volatile *)ConditionVariable, (void*)New, (void*)Current);
            if (Last == Current)
                break;
            Current = Last;
            RtlBackoff(&backoff);
        }
        if (Flags& RTL_CONDITION_VARIABLE_LOCKMODE_SHARED)
            RtlReleaseSRWLockShared(SRWLock);
        else
            RtlReleaseSRWLockExclusive(SRWLock);
        if ((Current ^ New) & 0x8)
            RtlpOptimizeConditionVariableWaitList(ConditionVariable, New);
		
        InitializeGlobalKeyedEventHandle();
        for (SpinCount = ConditionVariableSpinCount; SpinCount; --SpinCount)
        {
            if (!(StackWaitBlock.flag & 2))
                break;
            YieldProcessor();
        }
        if (InterlockedBitTestAndReset((volatile LONG*)&StackWaitBlock.flag, 1))
        {
            Status = NtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, dwMilliseconds);
            if (Status == STATUS_TIMEOUT && RtlpWakeSingle(ConditionVariable, &StackWaitBlock) == FALSE)
            {
                SecondWaitWorkaroundNtWaitForKeyedEvent(GlobalKeyedEventHandle, (PVOID)&StackWaitBlock, 0, NULL);
                Status = STATUS_SUCCESS;
            }
        }
        if (Flags& RTL_CONDITION_VARIABLE_LOCKMODE_SHARED)
            RtlAcquireSRWLockShared(SRWLock);
        else
            RtlAcquireSRWLockExclusive(SRWLock);
    } while (FALSE);
    return Status;
}

//通过延时来暂时退避竞争
void
NTAPI
RtlBackoff(USHORT* const pBackoff)
{
    USHORT Backoff = *pBackoff;
    USHORT i;
	
    if (Backoff != 0)
    {
        if (Backoff + 1 < (1u << 13))
            Backoff *= 2;
    }
    else
    {
        if (IsUniprocessorMachine)
            return;
        Backoff = 64;
    }
    *pBackoff = Backoff;
    Backoff += (Backoff - 1) & __rdtsc();
    for (i = 0; i < Backoff; ++i)
        YieldProcessor();
}

typedef struct _KEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK *PKEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK;
typedef struct _KEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK {
    PVOID								Address;
    HANDLE								EventHandle;
    PKEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK Next;
    PKEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK Previous;
} KEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK;

typedef struct _KEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET {
    RTL_SRWLOCK							Lock;
    PKEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK	WaitBlocks;
} KEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET, *PKEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET;

#define KexRtlWoaHashEntries 128
static KEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET KexRtlWaitOnAddressHashTable[KexRtlWoaHashEntries] = {0};

#pragma warning(disable:4715)
static inline BOOLEAN KexRtlpEqualVolatileMemory(
    IN	volatile VOID	*Address1,
    IN	PVOID			Address2,
    IN	SIZE_T			Size)
{
    switch (Size) {
    case 1:		return (*(PUCHAR) Address1 == *(PUCHAR) Address2);
    case 2:		return (*(PUSHORT) Address1 == *(PUSHORT) Address2);
    case 4:		return (*(PULONG) Address1 == *(PULONG) Address2);
    case 8:		return (*(PULONGLONG) Address1 == *(PULONGLONG) Address2);
    default:	return FALSE;
    }
}
#pragma warning(default:4715)

static FORCEINLINE PKEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET KexRtlpGetWoaHashBucket(
    IN	volatile VOID	*Address)
{
    return &KexRtlWaitOnAddressHashTable[
        (((ULONG_PTR) Address) >> 4) & (KexRtlWoaHashEntries - 1)]; 
}

static inline VOID KexRtlpRemoveWoaWaitBlock(
    IN	PKEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET	HashBucket,
    IN	PKEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK		WaitBlock)
{
    if (WaitBlock->Previous == NULL)
        return;
    else if (WaitBlock->Next == WaitBlock)
        HashBucket->WaitBlocks = NULL;
    else
    {
        if (WaitBlock == HashBucket->WaitBlocks)
            HashBucket->WaitBlocks = WaitBlock->Next;
        WaitBlock->Previous->Next = WaitBlock->Next;
        WaitBlock->Next->Previous = WaitBlock->Previous;
    }
}

NTSTATUS NTAPI RtlWaitOnAddress(
    IN	volatile VOID	*Address,
    IN	PVOID			CompareAddress,
    IN	SIZE_T			AddressSize,
    IN	const LARGE_INTEGER*	Timeout OPTIONAL)
{
    NTSTATUS Status;
    PKEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET HashBucket;
    KEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK WaitBlock;
    if (Address == NULL || CompareAddress == NULL)
        return STATUS_INVALID_PARAMETER;
    if (AddressSize != 4 && AddressSize != 2 && AddressSize != 1 && AddressSize != 8)
        return STATUS_INVALID_PARAMETER;
    HashBucket = KexRtlpGetWoaHashBucket(Address);
    RtlAcquireSRWLockExclusive(&HashBucket->Lock);
    if (!KexRtlpEqualVolatileMemory(Address, CompareAddress, AddressSize)) {
        RtlReleaseSRWLockExclusive(&HashBucket->Lock);
        return STATUS_SUCCESS;
    }
    Status = NtCreateEvent(
        &WaitBlock.EventHandle,
        SYNCHRONIZE | EVENT_MODIFY_STATE,
        NULL,
        NotificationEvent,
        FALSE);
    if (!NT_SUCCESS(Status)) {
        RtlReleaseSRWLockExclusive(&HashBucket->Lock);
        return Status;
    }
    WaitBlock.Address = (PVOID) Address;
    if (HashBucket->WaitBlocks == NULL) {
        WaitBlock.Previous = &WaitBlock;
        WaitBlock.Next = &WaitBlock;
        HashBucket->WaitBlocks = &WaitBlock;
    } else {
        WaitBlock.Previous = HashBucket->WaitBlocks->Previous;
        WaitBlock.Next = HashBucket->WaitBlocks;
        HashBucket->WaitBlocks->Previous->Next = &WaitBlock;
        HashBucket->WaitBlocks->Previous = &WaitBlock;
    }
    RtlReleaseSRWLockExclusive(&HashBucket->Lock);
    Status = NtWaitForSingleObject(
        WaitBlock.EventHandle,
        FALSE,
        Timeout);
    if (Status == STATUS_TIMEOUT || !NT_SUCCESS(Status)) {
        RtlAcquireSRWLockExclusive(&HashBucket->Lock);
        KexRtlpRemoveWoaWaitBlock(HashBucket, &WaitBlock);
        RtlReleaseSRWLockExclusive(&HashBucket->Lock);
    }
    NtClose(WaitBlock.EventHandle);
    return Status;
}

static VOID RtlpWakeByAddress(
    IN	PVOID			Address,
    IN	BOOLEAN			WakeAll)
{
    PKEX_RTL_WAIT_ON_ADDRESS_HASH_BUCKET HashBucket;
    PKEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK WaitBlock;
    HashBucket = KexRtlpGetWoaHashBucket(Address);
    RtlAcquireSRWLockExclusive(&HashBucket->Lock);
    if (HashBucket->WaitBlocks == NULL) {
        RtlReleaseSRWLockExclusive(&HashBucket->Lock);
        return;
    }
    WaitBlock = HashBucket->WaitBlocks;
    while (TRUE) {
        PKEX_RTL_WAIT_ON_ADDRESS_WAIT_BLOCK NextWaitBlock = WaitBlock->Next;
        if (WaitBlock->Address == Address) {
            KexRtlpRemoveWoaWaitBlock(HashBucket, WaitBlock);
            WaitBlock->Previous = NULL;
            NtSetEvent(WaitBlock->EventHandle, NULL);
            if (!WakeAll)
                break;
        }
        if (HashBucket->WaitBlocks == NULL || NextWaitBlock == HashBucket->WaitBlocks)
            break;
        WaitBlock = NextWaitBlock;
    }
    RtlReleaseSRWLockExclusive(&HashBucket->Lock);
}

VOID NTAPI RtlWakeAddressSingle(IN PVOID Address)
{
    RtlpWakeByAddress(Address, FALSE);
}

VOID NTAPI RtlWakeAddressAll(IN PVOID Address)
{
    RtlpWakeByAddress(Address, TRUE);
}

/* Pointer-sized interlocked helpers */
#ifndef InterlockedAndPointer
 #if defined(_WIN64)
  #define InterlockedAndPointer(ptr, val) (PVOID)InterlockedAnd64((volatile LONG64*)(ptr), (LONG64)(val))
 #else
  #define InterlockedAndPointer(ptr, val) (PVOID)InterlockedAnd((volatile LONG*)(ptr), (LONG)(val))
 #endif
#endif

#ifndef InterlockedExchangeAddPointer
 #if defined(_WIN64)
  #define InterlockedExchangeAddPointer(ptr, val) (PVOID)InterlockedExchangeAdd64((volatile LONG64*)(ptr), (LONG64)(val))
 #else
  #define InterlockedExchangeAddPointer(ptr, val) (PVOID)InterlockedExchangeAdd((volatile LONG*)(ptr), (LONG)(val))
 #endif
#endif

#if defined(_WIN64)
 #define SRWM_ITEM  0xFFFFFFFFFFFFFFF0ULL
#else
 #define SRWM_ITEM  0xFFFFFFF0U
#endif
#define SRWM_FLAG  (~SRWM_ITEM)
#define SRW_COUNT_BIT   4
#define SRW_HOLD_BIT    0
#define NODE_SPIN_BIT   1
#define NODEF_EXCL      0x01
#define NODEF_SPIN      0x02

typedef struct _SRW_WAIT_NODE
{
    struct _SRW_WAIT_NODE* prev;
    struct _SRW_WAIT_NODE* head;
    struct _SRW_WAIT_NODE* next;
    ULONG shareSnapshot;
    ULONG flags;
    PRTL_SRWLOCK lock;
} SRW_WAIT_NODE;

typedef ULONG_PTR SRW_STATE;

__forceinline LONG RtlpInterlockedBit0SetPointer(volatile PVOID* Target)
{
#if defined(_M_X64)
    return _interlockedbittestandset64((volatile LONG64*)Target, 0);
#elif defined(_M_IX86)
    return _interlockedbittestandset((volatile LONG*)Target, 0);
#else
    PVOID OldValue = *Target;
    for (;;)
    {
        ULONG_PTR OldBits = (ULONG_PTR)OldValue;
        if (OldBits & SRWF_Hold) return 1;
		
        PVOID NewValue = (PVOID)(OldBits | SRWF_Hold);
        PVOID Prev = InterlockedCompareExchangePointer((PVOID*)Target, NewValue, OldValue);
		
        if (Prev == OldValue) return 0;
		
        OldValue = Prev;
    }
#endif
}

static VOID NTAPI RtlpBackoffExp(ULONG* pCount)
{
    ULONG n = *pCount;
    if (n == 0)
    {
        if (NtCurrentTeb()->ProcessEnvironmentBlock->NumberOfProcessors == 1) return;
        n = 64;
    }
    else if (n < 0x2000)
    {
        n <<= 1;
    }
    *pCount = n;
    while (n--) YieldProcessor();
}

__forceinline BOOLEAN RtlpWaitCouldDeadlock(void)
{
    return FALSE;
}

__forceinline BOOLEAN SrwNodeIsExclusive(const SRW_WAIT_NODE* n) { return (n->flags & NODEF_EXCL) != 0; }
__forceinline BOOLEAN SrwNodeIsSpinning(const SRW_WAIT_NODE* n)  { return (n->flags & NODEF_SPIN) != 0; }

static VOID NTAPI RtlpSrwWake(PRTL_SRWLOCK SRWLock, SRW_STATE OldStatus)
{
    SRW_STATE CurrStatus;
    SRW_WAIT_NODE* tail;
    SRW_WAIT_NODE* first;
    while (1)
    {
        if (OldStatus & SRWF_Hold)
        {
            do
            {
                CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus - SRWF_Link), (PVOID)OldStatus);
                if (CurrStatus == OldStatus) return;
                OldStatus = (SRW_STATE)CurrStatus;
            } while (OldStatus & SRWF_Hold);
        }
        tail = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
        first = tail->head;
        if (first == NULL)
        {
            SRW_WAIT_NODE* curr = tail;
            do
            {
                curr->prev->next = curr;
                curr = curr->prev;
                first = curr->head;
            } while (first == NULL);
            if (tail != curr) tail->head = first;
        }
        if ((first->next != NULL) && SrwNodeIsExclusive(first))
        {
            tail->head = first->next;
            first->next = NULL;
            InterlockedAndPointer(&SRWLock->Ptr, (PVOID)(~SRWF_Link));
            break;
        }
        else
        {
            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, NULL, (PVOID)OldStatus);
            if ((SRW_STATE)CurrStatus == OldStatus) break;
            tail->head = first;
            OldStatus = (SRW_STATE)CurrStatus;
        }
    }
    do
    {
        SRW_WAIT_NODE* next = first->next;
        if (InterlockedBitTestAndReset((PLONG)&first->flags, NODE_SPIN_BIT) == 0)
        {
            NtReleaseKeyedEvent(GlobalKeyedEventHandle, first, FALSE, NULL);
        }
        first = next;
    } while (first != NULL);
}

static VOID NTAPI RtlpSrwCompressQueue(PRTL_SRWLOCK SRWLock, SRW_STATE OldStatus)
{
    SRW_STATE CurrStatus;
    if (OldStatus & SRWF_Hold)
    {
        do
        {
            SRW_WAIT_NODE* tail = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
            if (tail != NULL)
            {
                SRW_WAIT_NODE* curr = tail;
                while (curr->head == NULL)
                {
                    curr->prev->next = curr;
                    curr = curr->prev;
                }
                tail->head = curr->head;
            }
            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus - SRWF_Link), (PVOID)OldStatus);
            if (CurrStatus == OldStatus) return;
            OldStatus = (SRW_STATE)CurrStatus;
        } while (OldStatus & SRWF_Hold);
    }
    RtlpSrwWake(SRWLock, OldStatus);
}

VOID NTAPI RtlInitializeSRWLock(PRTL_SRWLOCK SRWLock)
{
    SRWLock->Ptr = NULL;
}

VOID NTAPI RtlAcquireSRWLockExclusive(PRTL_SRWLOCK SRWLock)
{
    __ALIGNED(16) SRW_WAIT_NODE node;
    BOOLEAN IsOptimize;
    SRW_STATE NewStatus;
    ULONG backoff;
    SRW_STATE CurrStatus;
    SRW_STATE OldStatus;
    int i;

    backoff = 0;

    InitializeGlobalKeyedEventHandle();

    if (RtlpInterlockedBit0SetPointer(&SRWLock->Ptr) == 0)
        return;

    OldStatus = (SRW_STATE)SRWLock->Ptr;

    while (1)
    {
        if (OldStatus & SRWF_Hold)
        {
            if (RtlpWaitCouldDeadlock()) NtTerminateProcess((HANDLE)-1, STATUS_THREAD_IS_TERMINATING);

            node.flags = NODEF_EXCL | NODEF_SPIN;
            node.next = NULL;
            IsOptimize = FALSE;

            if (OldStatus & SRWF_Wait)
            {
                node.head = NULL;
                node.shareSnapshot = 0;
                node.prev = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
                NewStatus = (SRW_STATE)&node | (OldStatus & SRWF_Many) | (SRWF_Link | SRWF_Wait | SRWF_Hold);
                if (!(OldStatus & SRWF_Link)) IsOptimize = TRUE;
            }
            else
            {
                node.head = &node;
                node.shareSnapshot = (ULONG)(OldStatus >> SRW_COUNT_BIT);
                if (node.shareSnapshot > 1)
                    NewStatus = (SRW_STATE)&node | (SRWF_Many | SRWF_Wait | SRWF_Hold);
                else
                    NewStatus = (SRW_STATE)&node | (SRWF_Wait | SRWF_Hold);
            }

            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            if (CurrStatus == OldStatus)
            {
                if (IsOptimize) RtlpSrwCompressQueue(SRWLock, NewStatus);

                for (i = SRWLockSpinCount; i > 0; --i)
                {
                    if (!SrwNodeIsSpinning(&node)) break;
                    YieldProcessor();
                }

                if (InterlockedBitTestAndReset((PLONG)&node.flags, NODE_SPIN_BIT))
                    NtWaitForKeyedEvent(GlobalKeyedEventHandle, &node, FALSE, NULL);

                OldStatus = CurrStatus;
            }
            else
            {
                RtlpBackoffExp(&backoff);
                OldStatus = (SRW_STATE)SRWLock->Ptr;
            }
        }
        else
        {
            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus + SRWF_Hold), (PVOID)OldStatus);
            if (CurrStatus == OldStatus) return;
            RtlpBackoffExp(&backoff);
            OldStatus = (SRW_STATE)SRWLock->Ptr;
        }
    }
}

VOID NTAPI RtlAcquireSRWLockShared(PRTL_SRWLOCK SRWLock)
{
    __ALIGNED(16) SRW_WAIT_NODE node;
    BOOLEAN IsOptimize;
    ULONG backoff;
    int i;
    SRW_STATE NewStatus;
    SRW_STATE CurrStatus;
    SRW_STATE OldStatus;

    backoff = 0;

    InitializeGlobalKeyedEventHandle();

    OldStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)((1 << SRW_COUNT_BIT) | SRWF_Hold), NULL);
    if (OldStatus == 0) return;

    while (1)
    {
        if ((OldStatus & SRWF_Hold) && ((OldStatus & SRWF_Wait) || ((OldStatus & SRWM_ITEM) == (SRW_STATE)NULL)))
        {
            if (RtlpWaitCouldDeadlock()) NtTerminateProcess((HANDLE)-1, STATUS_THREAD_IS_TERMINATING);

            node.flags = NODEF_SPIN;
            node.shareSnapshot = 0;
            IsOptimize = FALSE;
            node.next = NULL;

            if (OldStatus & SRWF_Wait)
            {
                node.prev = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
                NewStatus = (SRW_STATE)&node | (OldStatus & (SRWF_Many | SRWF_Hold)) | (SRWF_Link | SRWF_Wait);
                node.head = NULL;
                if (!(OldStatus & SRWF_Link)) IsOptimize = TRUE;
            }
            else
            {
                node.head = &node;
                NewStatus = (SRW_STATE)&node | (SRWF_Wait | SRWF_Hold);
            }

            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            if (CurrStatus == OldStatus)
            {
                if (IsOptimize) RtlpSrwCompressQueue(SRWLock, NewStatus);

                for (i = SRWLockSpinCount; i > 0; --i)
                {
                    if (!SrwNodeIsSpinning(&node)) break;
                    YieldProcessor();
                }

                if (InterlockedBitTestAndReset((PLONG)&node.flags, NODE_SPIN_BIT))
                    NtWaitForKeyedEvent(GlobalKeyedEventHandle, &node, FALSE, NULL);

                OldStatus = CurrStatus;
            }
            else
            {
                RtlpBackoffExp(&backoff);
                OldStatus = (SRW_STATE)SRWLock->Ptr;
            }
        }
        else
        {
            if (OldStatus & SRWF_Wait)
                NewStatus = OldStatus + SRWF_Hold;
            else
                NewStatus = (OldStatus + (1 << SRW_COUNT_BIT)) | SRWF_Hold;

            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            if (CurrStatus == OldStatus) return;

            RtlpBackoffExp(&backoff);
            OldStatus = (SRW_STATE)SRWLock->Ptr;
        }
    }
}

VOID NTAPI RtlReleaseSRWLockExclusive(PRTL_SRWLOCK SRWLock)
{
    SRW_STATE CurrStatus;
    SRW_STATE OldStatus;

    InitializeGlobalKeyedEventHandle();

    OldStatus = (SRW_STATE)InterlockedExchangeAddPointer(&SRWLock->Ptr, (PVOID)(-(LONG_PTR)SRWF_Hold));
    if ((OldStatus & SRWF_Wait) && !(OldStatus & SRWF_Link))
    {
        OldStatus -= SRWF_Hold;
        CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus + SRWF_Link), (PVOID)OldStatus);
        if (CurrStatus == OldStatus) RtlpSrwWake(SRWLock, OldStatus + SRWF_Link);
    }
}

VOID NTAPI RtlReleaseSRWLockShared(PRTL_SRWLOCK SRWLock)
{
    SRW_STATE CurrStatus;
    SRW_STATE NewStatus;
    ULONG count;
    SRW_STATE OldStatus;

    InitializeGlobalKeyedEventHandle();

    OldStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, NULL, (PVOID)(((1 << SRW_COUNT_BIT) | SRWF_Hold)));
    if (OldStatus == ((1 << SRW_COUNT_BIT) | SRWF_Hold)) return;

    if (!(OldStatus & SRWF_Wait))
    {
        do
        {
            if ((OldStatus & SRWM_ITEM) <= (1 << SRW_COUNT_BIT))
                NewStatus = 0;
            else
                NewStatus = OldStatus - (1 << SRW_COUNT_BIT);

            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            if (CurrStatus == OldStatus) return;
            OldStatus = CurrStatus;
        } while (!(OldStatus & SRWF_Wait));
    }

    if (OldStatus & SRWF_Many)
    {
        SRW_WAIT_NODE* curr = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
        while (curr->head == NULL) curr = curr->prev;
        curr = curr->head;
        count = InterlockedDecrement((PLONG)&curr->shareSnapshot);
        if (count > 0) return;
    }

    while (1)
    {
        NewStatus = OldStatus & (~(SRWF_Many | SRWF_Hold));
        if (OldStatus & SRWF_Link)
        {
            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            if (CurrStatus == OldStatus) return;
        }
        else
        {
            NewStatus |= SRWF_Link;
            CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            if (CurrStatus == OldStatus)
            {
                RtlpSrwWake(SRWLock, NewStatus);
                return;
            }
        }
        OldStatus = CurrStatus;
    }
}

BOOLEAN NTAPI RtlTryAcquireSRWLockExclusive(PRTL_SRWLOCK SRWLock)
{
    InitializeGlobalKeyedEventHandle();
    return !(RtlpInterlockedBit0SetPointer(&SRWLock->Ptr) == TRUE);
}

BOOLEAN NTAPI RtlTryAcquireSRWLockShared(PRTL_SRWLOCK SRWLock)
{
    ULONG backoff;
    SRW_STATE NewStatus;
    SRW_STATE CurrStatus;
    SRW_STATE OldStatus;

    backoff = 0;

    InitializeGlobalKeyedEventHandle();

    OldStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)((1 << SRW_COUNT_BIT) | SRWF_Hold), NULL);
    if (OldStatus == 0) return TRUE;

    while (1)
    {
        if ((OldStatus & SRWF_Hold) && ((OldStatus & SRWF_Wait) || (OldStatus & SRWM_ITEM) == (SRW_STATE)NULL))
            return FALSE;

        if (OldStatus & SRWF_Wait)
            NewStatus = OldStatus + SRWF_Hold;
        else
            NewStatus = OldStatus + (1 << SRW_COUNT_BIT);

        CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
        if (CurrStatus == OldStatus) return TRUE;

        RtlpBackoffExp(&backoff);
        OldStatus = (SRW_STATE)SRWLock->Ptr;
    }
}

// static __forceinline LONG RtlpMarkSrwHeld(volatile PVOID* Target)
// {
// #if defined(_M_X64)
    // return _interlockedbittestandset64((volatile LONG64*)Target, 0);
// #elif defined(_M_IX86)
    // return _interlockedbittestandset((volatile LONG*)Target, 0);
// #else
    // PVOID OldValue = *Target;
    // for (;;)
    // {
        // ULONG_PTR OldBits = (ULONG_PTR)OldValue;
        // if (OldBits & SRWF_Hold) return 1; /* bit was already set */
		
        // PVOID NewValue = (PVOID)(OldBits | SRWF_Hold);
        // PVOID Prev = InterlockedCompareExchangePointer((PVOID*)Target, NewValue, OldValue);
		
        // if (Prev == OldValue) return 0;     /* successfully set from 0 to 1 */
		
        // OldValue = Prev;
    // }
// #endif
// }

// static VOID NTAPI RtlpBackoffExp(ULONG* pCount)
// {
    // ULONG n = *pCount;
    // if (n == 0)
    // {
        // if (NtCurrentTeb()->ProcessEnvironmentBlock->NumberOfProcessors == 1) return;
        // n = 64;
    // }
    // else if (n < 0x2000)
    // {
        // n <<= 1;
    // }
    // *pCount = n;
    // while (n--) YieldProcessor();
// }

// /* Helpers to test node attributes; avoid raw bit-twiddling at callsites. */
// static __forceinline BOOLEAN SrwNodeIsExclusive(const SRW_WAIT_NODE* n) { return (n->flags & NODEF_EXCL) != 0; }
// static __forceinline BOOLEAN SrwNodeIsSpinning(const SRW_WAIT_NODE* n)  { return (n->flags & NODEF_SPIN) != 0; }

// static VOID NTAPI RtlpSrwWake(PRTL_SRWLOCK SRWLock, SRW_STATE OldStatus)
// {
    // SRW_STATE CurrStatus;
    // SRW_WAIT_NODE* tail;
    // SRW_WAIT_NODE* first;

    // while (1)
    // {
        // while (OldStatus & SRWF_Hold)
        // {
            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus & ~(SRW_STATE)SRWF_Link), (PVOID)OldStatus);
            // if (CurrStatus == OldStatus) return;
            // OldStatus = (SRW_STATE)CurrStatus;
        // }
		
        // tail = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
        // first = tail->head;
        // if (first == NULL)
        // {
            // SRW_WAIT_NODE* curr = tail;
            // do
            // {
                // curr->prev->next = curr;
                // curr = curr->prev;
                // first = curr->head;
            // } while (first == NULL);
            // if (tail != curr) tail->head = first;
        // }

        // if ((first->next != NULL) && SrwNodeIsExclusive(first))
        // {
            // tail->head = first->next;
            // first->next = NULL;
            // InterlockedAndPointer(&SRWLock->Ptr, (PVOID)(~((SRW_STATE)SRWF_Link)));
            // break;
        // }
        // else
        // {
            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, NULL, (PVOID)OldStatus);
            // if (CurrStatus == OldStatus) break;
            // tail->head = first;
            // OldStatus = (SRW_STATE)CurrStatus;
        // }
    // }
	
    // do
    // {
        // SRW_WAIT_NODE* next = first->next;
        // if (InterlockedBitTestAndReset((PLONG)&first->flags, NODE_SPIN_BIT) == 0)
        // {
            // NtReleaseKeyedEvent(GlobalKeyedEventHandle, first, FALSE, NULL);
        // }
        // first = next;
    // } while (first != NULL);
// }

// static VOID NTAPI RtlpSrwCompressQueue(PRTL_SRWLOCK SRWLock, SRW_STATE OldStatus)
// {
    // SRW_STATE CurrStatus;
    // while (OldStatus & SRWF_Hold)
    // {
        // SRW_WAIT_NODE* tail = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
        // if (tail != NULL)
        // {
            // SRW_WAIT_NODE* curr = tail;
            // while (curr->head == NULL)
            // {
                // curr->prev->next = curr;
                // curr = curr->prev;
            // }
            // tail->head = curr->head;
        // }
        // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus & ~(SRW_STATE)SRWF_Link), (PVOID)OldStatus);
        // if (CurrStatus == OldStatus) return;
        // OldStatus = (SRW_STATE)CurrStatus;
    // }
    // RtlpSrwWake(SRWLock, OldStatus);
// }

// VOID NTAPI RtlInitializeSRWLock(PRTL_SRWLOCK SRWLock)
// {
    // SRWLock->Ptr = NULL;
// }

// VOID NTAPI RtlAcquireSRWLockExclusive(PRTL_SRWLOCK SRWLock)
// {
    // __ALIGNED(16) SRW_WAIT_NODE node;
    // BOOLEAN IsOptimize;
    // SRW_STATE NewStatus;
    // ULONG backoff = 0;
    // SRW_STATE CurrStatus;
    // SRW_STATE OldStatus;
    // int i;

    // if (RtlpMarkSrwHeld(&SRWLock->Ptr) == 0)
        // return;

    // OldStatus = (SRW_STATE)SRWLock->Ptr;

    // while (1)
    // {
        // if (OldStatus & SRWF_Hold)
        // {
            // if (RtlpWaitCouldDeadlock()) NtTerminateProcess((HANDLE)-1, STATUS_THREAD_IS_TERMINATING);

            // node.flags = NODEF_EXCL | NODEF_SPIN;
            // node.next = NULL;
            // IsOptimize = FALSE;

            // if (OldStatus & SRWF_Wait)
            // {
                // node.head = NULL;
                // node.shareSnapshot = 0;
                // node.prev = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
                // NewStatus = (SRW_STATE)&node | (OldStatus & SRWF_Many) | (SRWF_Link | SRWF_Wait | SRWF_Hold);
                // if (!(OldStatus & SRWF_Link)) IsOptimize = TRUE;
            // }
            // else
            // {
                // node.head = &node;
                // node.shareSnapshot = (ULONG)(OldStatus >> SRW_COUNT_BIT);
                // if (node.shareSnapshot > 1)
                    // NewStatus = (SRW_STATE)&node | (SRWF_Many | SRWF_Wait | SRWF_Hold);
                // else
                    // NewStatus = (SRW_STATE)&node | (SRWF_Wait | SRWF_Hold);
            // }

            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            // if (CurrStatus == OldStatus)
            // {
                // if (IsOptimize) RtlpSrwCompressQueue(SRWLock, NewStatus);
                // for (i = SRWLockSpinCount; i > 0; --i)
                // {
                    // if (!SrwNodeIsSpinning(&node)) break;
                    // YieldProcessor();
                // }
                // if (InterlockedBitTestAndReset((PLONG)&node.flags, NODE_SPIN_BIT))
                    // NtWaitForKeyedEvent(GlobalKeyedEventHandle, &node, FALSE, NULL);
                // OldStatus = CurrStatus;
            // }
            // else
            // {
                // RtlpBackoffExp(&backoff);
                // OldStatus = (SRW_STATE)SRWLock->Ptr;
            // }
        // }
        // else
        // {
            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus | SRWF_Hold), (PVOID)OldStatus);
            // if (CurrStatus == OldStatus) return;
            // RtlpBackoffExp(&backoff);
            // OldStatus = (SRW_STATE)SRWLock->Ptr;
        // }
    // }
// }

// VOID NTAPI RtlAcquireSRWLockShared(PRTL_SRWLOCK SRWLock)
// {
    // __ALIGNED(16) SRW_WAIT_NODE node;
    // BOOLEAN IsOptimize;
    // ULONG backoff = 0;
    // int i;

    // SRW_STATE NewStatus;
    // SRW_STATE CurrStatus;
    // SRW_STATE OldStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)((1 << SRW_COUNT_BIT) | SRWF_Hold), NULL);
    // if (OldStatus == 0) return;

    // while (1)
    // {
    // if ((OldStatus & SRWF_Hold) && ((OldStatus & SRWF_Wait) || ((OldStatus & SRWM_ITEM) == (SRW_STATE)NULL)))
        // {
            // if (RtlpWaitCouldDeadlock()) NtTerminateProcess((HANDLE)-1, STATUS_THREAD_IS_TERMINATING);

            // node.flags = NODEF_SPIN;
            // node.shareSnapshot = 0;
            // IsOptimize = FALSE;
            // node.next = NULL;

            // if (OldStatus & SRWF_Wait)
            // {
                // node.prev = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
                // NewStatus = (SRW_STATE)&node | (OldStatus & (SRWF_Many | SRWF_Hold)) | (SRWF_Link | SRWF_Wait);
                // node.head = NULL;
                // if (!(OldStatus & SRWF_Link)) IsOptimize = TRUE;
            // }
            // else
            // {
                // node.head = &node;
                // NewStatus = (SRW_STATE)&node | (SRWF_Wait | SRWF_Hold);
            // }

            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            // if (CurrStatus == OldStatus)
            // {
                // if (IsOptimize) RtlpSrwCompressQueue(SRWLock, NewStatus);
                // for (i = SRWLockSpinCount; i > 0; --i)
                // {
                    // if (!SrwNodeIsSpinning(&node)) break;
                    // YieldProcessor();
                // }
                // if (InterlockedBitTestAndReset((PLONG)&node.flags, NODE_SPIN_BIT))
                    // NtWaitForKeyedEvent(GlobalKeyedEventHandle, &node, FALSE, NULL);
                // OldStatus = CurrStatus;
            // }
            // else
            // {
                // RtlpBackoffExp(&backoff);
                // OldStatus = (SRW_STATE)SRWLock->Ptr;
            // }
        // }
        // else
        // {
            // if (OldStatus & SRWF_Wait)
                // NewStatus = OldStatus | SRWF_Hold;
            // else
                // NewStatus = (OldStatus + (1 << SRW_COUNT_BIT)) | SRWF_Hold;
            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            // if (CurrStatus == OldStatus) return;
            // RtlpBackoffExp(&backoff);
            // OldStatus = (SRW_STATE)SRWLock->Ptr;
        // }
    // }
// }

// VOID NTAPI RtlReleaseSRWLockExclusive(PRTL_SRWLOCK SRWLock)
// {
    // SRW_STATE CurrStatus;
    // SRW_STATE OldStatus = (SRW_STATE)InterlockedExchangeAddPointer(&SRWLock->Ptr, (PVOID)(-(LONG_PTR)SRWF_Hold));
    // if ((OldStatus & SRWF_Wait) && !(OldStatus & SRWF_Link))
    // {
        // OldStatus &= ~(SRW_STATE)SRWF_Hold;
        // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)(OldStatus | SRWF_Link), (PVOID)OldStatus);
        // if (CurrStatus == OldStatus) RtlpSrwWake(SRWLock, OldStatus | SRWF_Link);
    // }
// }

// VOID NTAPI RtlReleaseSRWLockShared(PRTL_SRWLOCK SRWLock)
// {
    // SRW_STATE CurrStatus, NewStatus;
    // ULONG count;
    // SRW_STATE OldStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, NULL, (PVOID)(((1 << SRW_COUNT_BIT) | SRWF_Hold)));
    // if (OldStatus == ((1 << SRW_COUNT_BIT) | SRWF_Hold)) return;

    // while (!(OldStatus & SRWF_Wait))
    // {
        // if ((OldStatus & (SRWM_ITEM)) <= (1 << SRW_COUNT_BIT)) NewStatus = 0;
        // else NewStatus = OldStatus - (1 << SRW_COUNT_BIT);
        // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
        // if (CurrStatus == OldStatus) return;
        // OldStatus = CurrStatus;
    // }

    // if (OldStatus & SRWF_Many)
    // {
        // SRW_WAIT_NODE* curr = (SRW_WAIT_NODE*)(OldStatus & SRWM_ITEM);
        // while (curr->head == NULL) curr = curr->prev;
        // curr = curr->head;
        // count = InterlockedDecrement((PLONG)&curr->shareSnapshot);
        // if (count > 0) return;
    // }

    // while (1)
    // {
        // NewStatus = OldStatus & (~(SRW_STATE)(SRWF_Many | SRWF_Hold));
        // if (OldStatus & SRWF_Link)
        // {
            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            // if (CurrStatus == OldStatus) return;
        // }
        // else
        // {
            // NewStatus |= SRWF_Link;
            // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
            // if (CurrStatus == OldStatus)
            // {
                // RtlpSrwWake(SRWLock, NewStatus);
                // return;
            // }
        // }
        // OldStatus = CurrStatus;
    // }
// }

// BOOLEAN NTAPI RtlTryAcquireSRWLockExclusive(PRTL_SRWLOCK SRWLock)
// {
    // return RtlpMarkSrwHeld(&SRWLock->Ptr) == 0;
// }

// BOOLEAN NTAPI RtlTryAcquireSRWLockShared(PRTL_SRWLOCK SRWLock)
// {
    // ULONG backoff = 0;
    // SRW_STATE NewStatus;
    // SRW_STATE CurrStatus;
    // SRW_STATE OldStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)((1 << SRW_COUNT_BIT) | SRWF_Hold), NULL);
    // if (OldStatus == 0) return TRUE;
    // while (1)
    // {
        // if ((OldStatus & SRWF_Hold) && ((OldStatus & SRWF_Wait) || (OldStatus & SRWM_ITEM) == (SRW_STATE)NULL))
            // return FALSE;
        // if (OldStatus & SRWF_Wait) NewStatus = OldStatus | SRWF_Hold;
        // else NewStatus = OldStatus + (1 << SRW_COUNT_BIT);
        // CurrStatus = (SRW_STATE)InterlockedCompareExchangePointer(&SRWLock->Ptr, (PVOID)NewStatus, (PVOID)OldStatus);
        // if (CurrStatus == OldStatus) return TRUE;
        // RtlpBackoffExp(&backoff);
        // OldStatus = (SRW_STATE)SRWLock->Ptr;
    // }
// }