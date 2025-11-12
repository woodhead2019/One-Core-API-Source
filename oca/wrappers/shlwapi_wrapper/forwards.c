/*++

Copyright (c) 2024 Shorthorn Project

Module Name:

    forwards.c

Abstract:

    This module implements Fowards of Native Windows Sockets 2 APIs

Author:

    Skulltrail 23-October-2024

Revision History:

--*/
 
#include "main.h"

#pragma comment(linker, "/EXPORT:SetAddrInfoExA=ws2_base.SetAddrInfoExA")
#pragma comment(linker, "/EXPORT:__OrdinalFunction236=shell32.#236,@236,NONAME")