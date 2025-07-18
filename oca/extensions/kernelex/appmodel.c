/*++

Copyright (c) 2017 Shorthorn Project

Module Name:

    appmodel.c

Abstract:

    This module implements Windows Store APIs

Author:

    Skulltrail 19-July-2017

Revision History:

--*/

#include <main.h>

WINE_DEFAULT_DEBUG_CHANNEL(appmodel);

/***********************************************************************
 *           GetCurrentPackageId       (KERNEL32.@)
 */
LONG WINAPI GetCurrentPackageId(UINT32 *len, BYTE *buffer)
{
    FIXME("(%p %p): stub\n", len, buffer);
    return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *           GetCurrentPackageFamilyName       (KERNEL32.@)
 */
LONG WINAPI GetCurrentPackageFamilyName(UINT32 *length, PWSTR name)
{
    FIXME("(%p %p): stub\n", length, name);
    return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *           GetCurrentPackageFullName       (KERNEL32.@)
 */
LONG WINAPI GetCurrentPackageFullName(UINT32 *length, PWSTR name)
{
    FIXME("(%p %p): stub\n", length, name);
    return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *           GetPackageFullName       (KERNEL32.@)
 */
LONG WINAPI GetPackageFullName(HANDLE process, UINT32 *length, PWSTR name)
{
    FIXME("(%p %p %p): stub\n", process, length, name);
    return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *          AppPolicyGetProcessTerminationMethod (kernelex.@)
 */
LONG WINAPI AppPolicyGetProcessTerminationMethod(HANDLE token, AppPolicyProcessTerminationMethod *policy)
{
    FIXME("%p, %p\n", token, policy);

    if(policy)
        *policy = AppPolicyProcessTerminationMethod_ExitProcess;

    return ERROR_SUCCESS;
}

/***********************************************************************
 *          AppPolicyGetThreadInitializationType (kernelex.@)
 */
LONG WINAPI AppPolicyGetThreadInitializationType(HANDLE token, AppPolicyThreadInitializationType *policy)
{
    FIXME("%p, %p\n", token, policy);

    if(policy)
        *policy = AppPolicyThreadInitializationType_None;

    return ERROR_SUCCESS;
}

/***********************************************************************
 *          AppPolicyGetShowDeveloperDiagnostic (kernelex.@)
 */
LONG WINAPI AppPolicyGetShowDeveloperDiagnostic(HANDLE token, AppPolicyShowDeveloperDiagnostic *policy)
{
    FIXME("%p, %p\n", token, policy);

    if(policy)
        *policy = AppPolicyShowDeveloperDiagnostic_ShowUI;

    return ERROR_SUCCESS;
}

/***********************************************************************
 *          AppPolicyGetWindowingModel (kernelex.@)
 */
LONG WINAPI AppPolicyGetWindowingModel(HANDLE token, AppPolicyWindowingModel *policy)
{
    FIXME("%p, %p\n", token, policy);

    if(policy)
        *policy = AppPolicyWindowingModel_ClassicDesktop;

    return ERROR_SUCCESS;
}

/***********************************************************************
 *      LoadPackagedLibrary    (kernelex.@)
 */
HMODULE WINAPI /* DECLSPEC_HOTPATCH */ LoadPackagedLibrary( LPCWSTR name, DWORD reserved )
{
    FIXME( "semi-stub, name %s, reserved %#x.\n", debugstr_w(name), reserved );
    SetLastError( APPMODEL_ERROR_NO_PACKAGE );
    return LoadLibraryW(name);
}

/***********************************************************************
 *          AppPolicyGetMediaFoundationCodecLoading (kernelex.@)
 */

LONG WINAPI AppPolicyGetMediaFoundationCodecLoading(HANDLE token, AppPolicyMediaFoundationCodecLoading *policy)
{
    FIXME("%p, %p\n", token, policy);

    if(policy)
        *policy = AppPolicyMediaFoundationCodecLoading_All;

    return ERROR_SUCCESS;
}

LONG 
WINAPI 
FindPackagesByPackageFamily(
  PCWSTR packageFamilyName,
  UINT32 packageFilters,
  UINT32 *count,
  PWSTR  *packageFullNames,
  UINT32 *bufferLength,
  WCHAR  *buffer,
  UINT32 *packageProperties
)
{
	return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *         GetPackageFamilyName   (kernelex.@)
 */
LONG WINAPI /* DECLSPEC_HOTPATCH */ GetPackageFamilyName( HANDLE process, UINT32 *length, WCHAR *name )
{
    FIXME( "(%p %p %p): stub\n", process, length, name );
    return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *         GetPackagePathByFullName   (kernelex.@)
 */
LONG WINAPI GetPackagePathByFullName(const WCHAR *name, UINT32 *len, WCHAR *path)
{
    if (!len || !name)
        return ERROR_INVALID_PARAMETER;

    FIXME( "(%s %p %p): stub\n", debugstr_w(name), len, path );

    return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *         GetPackagesByPackageFamily   (kernelex.@)
 */
LONG WINAPI DECLSPEC_HOTPATCH GetPackagesByPackageFamily(const WCHAR *family_name, UINT32 *count,
                                                         WCHAR *full_names, UINT32 *buffer_len, WCHAR *buffer)
{
    FIXME( "(%s %p %p %p %p): stub\n", debugstr_w(family_name), count, full_names, buffer_len, buffer );

    if (!count || !buffer_len)
        return ERROR_INVALID_PARAMETER;

    *count = 0;
    *buffer_len = 0;
    return ERROR_SUCCESS;
}

/***********************************************************************
 *         GetPackagePathByFullName   (kernelex.@)
 */
LONG WINAPI GetApplicationUserModelId(HANDLE hProcess, UINT32 *applicationUserModelIdLength, PWSTR  applicationUserModelId)
{
	FIXME( "(%p %p %p): stub\n", hProcess, applicationUserModelIdLength, applicationUserModelId );
    return APPMODEL_ERROR_NO_PACKAGE;
}

LONG WINAPI GetCurrentApplicationUserModelId(UINT32 *applicationUserModelIdLength, PWSTR  applicationUserModelId)
{
	FIXME( "(%p %p): stub\n", applicationUserModelIdLength, applicationUserModelId );
    return APPMODEL_ERROR_NO_PACKAGE;	
}

/***********************************************************************
 *         GetCurrentPackagePath   (kernelex.@)
 */
LONG WINAPI /* DECLSPEC_HOTPATCH */ GetCurrentPackagePath( UINT32 *length, WCHAR *path )
{
    FIXME( "(%p %p): stub\n", length, path );
    return APPMODEL_ERROR_NO_PACKAGE;
}

/***********************************************************************
 *         PackageIdFromFullName   (kernelex.@)
 */
LONG WINAPI PackageIdFromFullName(const WCHAR *full_name, UINT32 flags, UINT32 *buffer_length, BYTE *buffer)
{
	return ERROR_NOT_FOUND;
}

LONG WINAPI GetPackageApplicationIds(
  PACKAGE_INFO_REFERENCE packageInfoReference,
  UINT32                 *bufferLength,
  BYTE                   *buffer,
  UINT32                 *count
)
{
	return APPMODEL_ERROR_NO_PACKAGE;
}

LONG WINAPI OpenPackageInfoByFullName(
  PCWSTR                 packageFullName,
  const UINT32           reserved,
  PACKAGE_INFO_REFERENCE *packageInfoReference
)
{
	return ERROR_NOT_FOUND;
}

/***********************************************************************
 *         GetCurrentPackageInfo   (kernelbase.@)
 */
LONG WINAPI GetCurrentPackageInfo( const UINT32 flags, UINT32 *buffer_size, BYTE *buffer, UINT32 *count )
{
    FIXME( "(%#x %p %p %p): stub\n", flags, buffer_size, buffer, count );
    return APPMODEL_ERROR_NO_PACKAGE;
}

NTSTATUS NTAPI ApiSetQueryApiSetPresence(
	IN	PUNICODE_STRING	Namespace,
	OUT	PBOOLEAN		Present)
{
	*Present = TRUE;
	return STATUS_SUCCESS;
}