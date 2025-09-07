#ifndef ACPPAGE_PRECOMP_H
#define ACPPAGE_PRECOMP_H

// Definições de arquitetura e versão do Windows
#define WIN32_LEAN_AND_MEAN
#define NTDDI_VERSION NTDDI_WIN7
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

// Garantir que as definições do Windows estão corretas
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#ifndef WINVER
#define WINVER 0x0601
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN7
#endif

#define COBJMACROS
#define WIN32_NO_STATUS
#define _INC_WINDOWS
#define COM_NO_WINDOWS_H
#define NTOS_MODE_USER

#include <windows.h>
#include <windowsx.h>
#include <winbase.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlsimpcoll.h>
#include <atlstr.h>
#include <atlwin.h>
#include <rosdlgs.h>

#include <apphelp.h>
#include <sfc.h>
#include <uxtheme.h>

ULONG DbgPrint(PCH Format,...);

extern const GUID CLSID_CLayerUIPropPage;
extern HMODULE g_hModule;
extern LONG g_ModuleRefCnt;

EXTERN_C BOOL WINAPI GetExeFromLnk(PCWSTR pszLnk, PWSTR pszExe, size_t cchSize);
BOOL IsBuiltinLayer(PCWSTR Name);

#include "resource.h"
#include "CLayerStringList.hpp"
#include "CLayerUIPropPage.hpp"

BOOL ShowEditCompatModes(HWND hWnd, CLayerUIPropPage* page);

#endif /* ACPPAGE_PRECOMP_H */