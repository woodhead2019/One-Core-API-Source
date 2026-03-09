/*++

Copyright (c) 2023 Shorthorn Project

Module Name:

    main.c

Abstract:

    This module implements COM Main functions APIs

Author:

    Skulltrail 12-October-2023

Revision History:

--*/

#define WIN32_NO_STATUS

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(hooks);

HRESULT 
WINAPI 
CoWaitForMultipleHandlesHook(
    DWORD dwFlags,
    DWORD dwTimeout,
    ULONG cHandles,
    LPHANDLE pHandles,
    LPDWORD lpdwIndex)
{
    DWORD filteredFlags;

    /* Remove APENAS as flags não suportadas */
    filteredFlags = dwFlags &
        ~(COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES);

    return CoWaitForMultipleHandles(
        filteredFlags,
        dwTimeout,
        cHandles,
        pHandles,
        lpdwIndex);
}

struct init_spy
{
    struct list entry;
    IInitializeSpy *spy;
    unsigned int id;
};

/* this is what is stored in TEB->ReservedForOle */
struct tlsdata
{
    struct apartment *apt;
    IErrorInfo       *errorinfo;
    DWORD             thread_seqid;  /* returned with CoGetCurrentProcess */
    DWORD             flags;         /* tlsdata_flags (+0Ch on x86) */
    void             *unknown0;
    DWORD             inits;         /* number of times CoInitializeEx called */
    DWORD             ole_inits;     /* number of times OleInitialize called */
    GUID              causality_id;  /* unique identifier for each COM call */
    LONG              pending_call_count_client; /* number of client calls pending */
    LONG              pending_call_count_server; /* number of server calls pending */
    DWORD             unknown;
    IObjContext      *context_token; /* (+38h on x86) */
    IUnknown         *call_state;    /* current call context (+3Ch on x86) */
    DWORD             unknown2[46];
    IUnknown         *cancel_object; /* cancel object set by CoSetCancelObject (+F8h on x86) */
    IUnknown         *state;         /* see CoSetState */
    struct list       spies;         /* Spies installed with CoRegisterInitializeSpy */
    DWORD             spies_lock;
    DWORD             cancelcount;
    CO_MTA_USAGE_COOKIE implicit_mta_cookie; /* mta referenced by roapi from sta thread */
};

/***********************************************************************
 *           InternalTlsAllocData    (combase.@)
 */
HRESULT WINAPI InternalTlsAllocData(struct tlsdata **data)
{
    if (!(*data = calloc(1, sizeof(**data))))
        return E_OUTOFMEMORY;

    list_init(&(*data)->spies);
    NtCurrentTeb()->ReservedForOle = *data;

    return S_OK;
}

static inline HRESULT com_get_tlsdata(struct tlsdata **data)
{
    *data = NtCurrentTeb()->ReservedForOle;
    return *data ? S_OK : InternalTlsAllocData(data);
}

static struct init_spy *get_spy_entry(struct tlsdata *tlsdata, unsigned int id)
{
    struct init_spy *spy;

    LIST_FOR_EACH_ENTRY(spy, &tlsdata->spies, struct init_spy, entry)
    {
        if (id == spy->id && spy->spy)
            return spy;
    }

    return NULL;
}

typedef HRESULT (WINAPI *PFN_CoRegisterInitializeSpy)(
    IInitializeSpy *spy,
    ULARGE_INTEGER *cookie
);

typedef HRESULT (WINAPI *PFN_CoRevokeInitializeSpy)(
    ULARGE_INTEGER cookie
);

/******************************************************************************
 *          CoRegisterInitializeSpy    (combase.@)
 */
HRESULT WINAPI CoRegisterInitializeSpyHook(IInitializeSpy *spy, ULARGE_INTEGER *cookie)
{
    struct tlsdata *tlsdata;
    struct init_spy *entry;
    unsigned int id;
    HRESULT hr;
    static PFN_CoRegisterInitializeSpy pCoRegisterInitializeSpy = NULL;
    static HMODULE hOleBase = NULL;

    if (!pCoRegisterInitializeSpy)
    {
        if (!hOleBase)
        {
            hOleBase = GetModuleHandleW(L"olebase.dll");
            if (!hOleBase)
                hOleBase = LoadLibraryW(L"olebase.dll");
        }

        if (hOleBase)
        {
            pCoRegisterInitializeSpy =
                (PFN_CoRegisterInitializeSpy)GetProcAddress(
                    hOleBase,
                    "CoRegisterInitializeSpy"
                );
        }
    }

    if (pCoRegisterInitializeSpy)
        return pCoRegisterInitializeSpy(spy, cookie);

    if (!spy || !cookie)
        return E_INVALIDARG;

    if (FAILED(hr = com_get_tlsdata(&tlsdata)))
        return hr;

    hr = IInitializeSpy_QueryInterface(spy, &IID_IInitializeSpy, (void **)&spy);
    if (FAILED(hr))
        return hr;

    entry = malloc(sizeof(*entry));
    if (!entry)
    {
        IInitializeSpy_Release(spy);
        return E_OUTOFMEMORY;
    }

    entry->spy = spy;

    id = 0;
    while (get_spy_entry(tlsdata, id) != NULL)
    {
        id++;
    }

    entry->id = id;
    list_add_head(&tlsdata->spies, &entry->entry);

    cookie->u.HighPart = GetCurrentThreadId();
    cookie->u.LowPart = entry->id;

    return S_OK;
}

/******************************************************************************
 *          CoRevokeInitializeSpy    (combase.@)
 */
HRESULT WINAPI CoRevokeInitializeSpyHook(ULARGE_INTEGER cookie)
{
    struct tlsdata *tlsdata;
    struct init_spy *spy;
    HRESULT hr;
    static PFN_CoRevokeInitializeSpy pCoRevokeInitializeSpy = NULL;
    static HMODULE hOleBase = NULL;

    if (!pCoRevokeInitializeSpy)
    {
        if (!hOleBase)
        {
            hOleBase = GetModuleHandleW(L"olebase.dll");
            if (!hOleBase)
                hOleBase = LoadLibraryW(L"olebase.dll");
        }

        if (hOleBase)
        {
            pCoRevokeInitializeSpy =
                (PFN_CoRevokeInitializeSpy)GetProcAddress(
                    hOleBase,
                    "CoRevokeInitializeSpy"
                );
        }
    }

    if (pCoRevokeInitializeSpy)
        return pCoRevokeInitializeSpy(cookie);    

    if (cookie.u.HighPart != GetCurrentThreadId())
        return E_INVALIDARG;

    if (FAILED(hr = com_get_tlsdata(&tlsdata)))
        return hr;

    if (!(spy = get_spy_entry(tlsdata, cookie.u.LowPart))) return E_INVALIDARG;

    IInitializeSpy_Release(spy->spy);
    spy->spy = NULL;
    if (!tlsdata->spies_lock)
    {
        list_remove(&spy->entry);
        free(spy);
    }
    return S_OK;
}