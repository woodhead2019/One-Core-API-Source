/*++

Copyright (c) 2021 Shorthorn Project

Module Name:

    atom.c

Abstract:

    This module contains the Win32 Atom Management APIs

Author:

    Skulltrail 25-March-2021

Revision History:

--*/

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(dllatom); 

ATOM WINAPI GlobalAddAtomExA(
  LPCSTR lpString,
  DWORD  Flags //Ignored for now
)
{
    if (Flags) DbgPrint("GlobalAddAtom:: Undocumented flags %i\n", Flags);
    return GlobalAddAtomA(lpString);
}

ATOM WINAPI GlobalAddAtomExW(
  LPCWSTR lpString,
  DWORD  Flags //Ignored for now
)
{
    if (Flags) DbgPrint("GlobalAddAtom:: Undocumented flags %i\n", Flags);
    return GlobalAddAtomW(lpString);
}