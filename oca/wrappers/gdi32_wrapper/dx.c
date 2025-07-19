/*++

Copyright (c) 2025 Shorthorn Project

Module Name:

    d3dkmt.c

Abstract:

    Implement Directx related functions

Author:

    Skulltrail 11-May-2025

Revision History:

--*/

#include <main.h>

HBITMAP WINAPI CreateBitmapFromDxSurface(HDC hdc, UINT32 cx, UINT32 cy, ULONG dxgiFormat, HANDLE phDxSurface)
{
	pCreateBitmapFromDxSurface pCreateBitmapDx;
	
    pCreateBitmapDx = (pCreateBitmapFromDxSurface) GetProcAddress(
                            GetModuleHandle(TEXT("gdibase")),
                            "CreateBitmapFromDxSurface");	

	if(pCreateBitmapDx){
		return pCreateBitmapDx(hdc, cx, cy, dxgiFormat, phDxSurface);
	}else{
	    return CreateCompatibleBitmap(hdc, cx, cy);
	}
}