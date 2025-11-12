/*++

Copyright (c) 2025  Shorthorn Project

Module Name:

    shfileop.c

Abstract:

    This module implements Localized APIs for Shell

Author:

    Skulltrail 31-October-2025

Revision History:

--*/

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(shellord);

/*************************************************************************
 *              SHRemoveLocalizedName (SHELL32.@)
 */
HRESULT WINAPI SHRemoveLocalizedName(const WCHAR *path)
{
    FIXME("%s stub\n", debugstr_w(path));
    return S_OK;
}
