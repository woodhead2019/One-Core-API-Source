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
#include "initguid.h"

WINE_DEFAULT_DEBUG_CHANNEL(ole32);

// {AF86E2E0-B12D-4C6A-9C5A-D7AA65101E90}
DEFINE_GUID(IID_IInspectable, 0xAF86E2E0, 0xB12D, 0x4C6A, 0x9C, 0x5A, 0xD7, 0xAA, 0x65, 0x10, 0x1E, 0x90);

// {277151C3-9E3E-42F6-91A4-5DFDEB232451}
DEFINE_GUID(IID_ILauncherStatics, 0x277151C3, 0x9E3E, 0x42F6, 0x91, 0xA4, 0x5D, 0xFD, 0xEB, 0x23, 0x24, 0x51);

HRESULT STDMETHODCALLTYPE CLauncherStatics_QueryInterface(
	IN	ILauncherStatics	*This,
	IN	REFIID				RefIID,
	OUT	PPVOID				Interface)
{
	*Interface = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_ILauncherStatics)) {

		*Interface = This;
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CLauncherStatics_AddRef(
	IN	ILauncherStatics	*This)
{
	return 1;
}

ULONG STDMETHODCALLTYPE CLauncherStatics_Release(
	IN	ILauncherStatics	*This)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE CLauncherStatics_GetIids(
	IN	ILauncherStatics	*This,
	OUT	PULONG				NumberOfIids,
	OUT	IID					**IidArray)
{
	IID *Array;
	ULONG Count;

	Count = 1;

	Array = (IID *) CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array) {
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_ILauncherStatics;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CLauncherStatics_GetRuntimeClassName(
	IN	ILauncherStatics	*This,
	OUT	HSTRING				*ClassName)
{
	PCWSTR Name = L"Windows.System.Launcher";
	return WindowsCreateString(Name, (ULONG) wcslen(Name), ClassName);
}

HRESULT STDMETHODCALLTYPE CLauncherStatics_GetTrustLevel(
	IN	ILauncherStatics	*This,
	OUT	TrustLevel			*Level)
{
	*Level = BaseTrust;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CLauncherStatics_LaunchFileAsync(
	IN	ILauncherStatics	*This,
	IN	IUnknown			*StorageFile,
	OUT	IAsyncOperation		**AsyncOperation)
{
	//KexDebugCheckpoint(); // not properly implemented
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CLauncherStatics_LaunchFileWithOptionsAsync(
	IN	ILauncherStatics	*This,
	IN	IUnknown			*StorageFile,
	IN	IUnknown			*LauncherOptions,
	OUT	IAsyncOperation		**AsyncOperation)
{
	//KexDebugCheckpoint(); // not properly implemented
	return CLauncherStatics_LaunchFileAsync(This, StorageFile, AsyncOperation);
}

HRESULT STDMETHODCALLTYPE CLauncherStatics_LaunchUriAsync(
	IN	ILauncherStatics	*This,
	IN	IUriRuntimeClass	*Uri,
	OUT	IAsyncOperation		**AsyncOperation)
{
	HRESULT Result;
	HSTRING RawUri;
	HINSTANCE ShellExecuteResult;

	Result = Uri->lpVtbl->get_AbsoluteUri(Uri, &RawUri);
	if (FAILED(Result)) {
		return Result;
	}

	ShellExecuteResult = ShellExecuteW(
		NULL,
		L"open",
		WindowsGetStringRawBuffer(RawUri, NULL),
		NULL,
		NULL,
		SW_SHOW);

	WindowsDeleteString(RawUri);

	if (((ULONG) ShellExecuteResult) <= 32) {
		return E_FAIL;
	}

	// HACK: Some apps (e.g. Spotify) require a valid pointer to be passed out.
	// However it seems to do fine if we just pass a pointer to some random
	// interface such as ourselves.
	*AsyncOperation = (IAsyncOperation *) This;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CLauncherStatics_LaunchUriWithOptionsAsync(
	IN	ILauncherStatics	*This,
	IN	IUriRuntimeClass	*Uri,
	IN	IUnknown			*LauncherOptions,
	OUT	IAsyncOperation		**AsyncOperation)
{
	//KexDebugCheckpoint(); // not properly implemented
	return CLauncherStatics_LaunchUriAsync(This, Uri, AsyncOperation);
}

ILauncherStaticsVtbl CLauncherStaticsVtbl = {
	CLauncherStatics_QueryInterface,
	CLauncherStatics_AddRef,
	CLauncherStatics_Release,
	CLauncherStatics_GetIids,
	CLauncherStatics_GetRuntimeClassName,
	CLauncherStatics_GetTrustLevel,
	CLauncherStatics_LaunchFileAsync,
	CLauncherStatics_LaunchFileWithOptionsAsync,
	CLauncherStatics_LaunchUriAsync,
	CLauncherStatics_LaunchUriWithOptionsAsync
};

ILauncherStatics CLauncherStatics = {
	&CLauncherStaticsVtbl
};

HRESULT WINAPI DllGetActivationFactory( HSTRING classid, IActivationFactory **factory )
{
    *factory = (IActivationFactory*)&CLauncherStatics;
    return ERROR_SUCCESS;
}

HRESULT WINAPI DllCanUnloadNow()
{
	return ERROR_SUCCESS;
}