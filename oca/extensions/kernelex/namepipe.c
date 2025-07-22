/*++

Copyright (c) 2017  Shorthorn Project

Module Name:

    namepipe.c

Abstract:

    This module contains the Win32 Named Pipe API

Author:

    Skulltrail 21-March-2017

Revision History:

--*/

#include "main.h"

WINE_DEFAULT_DEBUG_CHANNEL(kernel32);

BOOL 
WINAPI 
GetNamedPipeAttribute(
	HANDLE Pipe, 
	PIPE_ATTRIBUTE_TYPE AttributeType, 
	PSTR AttributeName, 
	PVOID AttributeValue, 
	PSIZE_T AttributeValueLength
)
{
  ULONG fscltCode; // edx@3
  NTSTATUS status; // eax@4
  BOOL result; // eax@5
  struct _IO_STATUS_BLOCK IoStatusBlock; // [sp+0h] [bp-8h]@4

  if ( AttributeType )
  {
    if ( AttributeType == 1 )
    {
      fscltCode = 0x110030;
    }
    else
    {
      if ( AttributeType != 2 )
      {
        SetLastError(0x57u);
        return FALSE;
      }
      fscltCode = 0x110038;
    }
  }
  else
  {
    fscltCode = 0x110028;
  }
  status = NtFsControlFile(
             Pipe,
             0,
             0,
             0,
             &IoStatusBlock,
             fscltCode,
             AttributeName,
             strlen(AttributeName) + 1,
             AttributeValue,
             *AttributeValueLength);
  if ( NT_SUCCESS(status))
  {
    *AttributeValueLength = IoStatusBlock.Information;
    result = TRUE;
  }
  else
  {
    BaseSetLastNTError(status);
    result = FALSE;
  }
  return result;
}

/***********************************************************************
 *           GetNamedPipeClientProcessId  (KERNEL32.@)
 */
BOOL 
WINAPI 
GetNamedPipeClientProcessId( HANDLE pipe, ULONG *id )
{
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;

    TRACE( "%p %p\n", pipe, id );

    status = NtFsControlFile( pipe, NULL, NULL, NULL, &iosb, FSCTL_PIPE_GET_CONNECTION_ATTRIBUTE,
                              (void *)"ClientProcessId", sizeof("ClientProcessId"), id, sizeof(*id) );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return FALSE;
    }
    return TRUE;
}

/***********************************************************************
 *           GetNamedPipeServerProcessId  (KERNEL32.@)
 */
BOOL WINAPI GetNamedPipeServerProcessId( HANDLE pipe, ULONG *id )
{
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;

    TRACE( "%p, %p\n", pipe, id );

    status = NtFsControlFile( pipe, NULL, NULL, NULL, &iosb, FSCTL_PIPE_GET_CONNECTION_ATTRIBUTE,
                              (void *)"ServerProcessId", sizeof("ServerProcessId"), id, sizeof(*id) );
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return FALSE;
    }
    return TRUE;
}

/***********************************************************************
 *           GetNamedPipeClientSessionId  (KERNEL32.@)
 */
BOOL WINAPI GetNamedPipeClientSessionId( HANDLE pipe, ULONG *id )
{
    FIXME( "%p, %p\n", pipe, id );

    if (!id) return FALSE;
    *id = NtCurrentTeb()->ProcessEnvironmentBlock->SessionId;
    return TRUE;
}

/***********************************************************************
 *           GetNamedPipeServerSessionId  (KERNEL32.@)
 */
BOOL WINAPI GetNamedPipeServerSessionId( HANDLE pipe, ULONG *id )
{
    FIXME( "%p, %p\n", pipe, id );

    if (!id) return FALSE;
    *id = NtCurrentTeb()->ProcessEnvironmentBlock->SessionId;
    return TRUE;
}

BOOL 
WINAPI 
GetNamedPipeClientComputerNameW(
	HANDLE Pipe, 
	LPWSTR ClientComputerName, 
	ULONG ClientComputerNameLength
)
{
  BOOL resp; // esi@1

  resp = GetNamedPipeAttribute(
           Pipe,
           PipeConnectionAttribute,
           "ClientComputerName",
           ClientComputerName,
           (PSIZE_T)ClientComputerNameLength);
  if ( !resp && GetLastError() == 1168 )
    SetLastError(0xE5u);
  return resp;
}

BOOL 
WINAPI 
GetNamedPipeClientComputerNameA(
	HANDLE Pipe, 
	LPSTR ClientComputerName, 
	ULONG ClientComputerNameLength
)
{
	PUNICODE_STRING string;
	string = Basep8BitStringToStaticUnicodeString(ClientComputerName);
	return GetNamedPipeClientComputerNameW(Pipe, string->Buffer, ClientComputerNameLength);
}

BOOL 
WINAPI 
SetNamedPipeAttribute(
	HANDLE Pipe, 
	PIPE_ATTRIBUTE_TYPE AttributeType, 
	PSTR AttributeName, 
	PVOID AttributeValue, 
	SIZE_T AttributeValueLength
)
{
  BOOL result;
  SIZE_T size;
  size_t localSize;
  PVOID alloc;
  PVOID other;
  ULONG number;
  NTSTATUS status; 
  struct _IO_STATUS_BLOCK IoStatusBlock; 
  ULONG AttributeTypea;
  ULONG AttributeValueLengtha;

  if ( AttributeType )
  {
    if ( AttributeType == 1 )
    {
      AttributeTypea = 0x110034;
    }
    else
    {
      if ( AttributeType != 2 )
      {
        SetLastError(0x57u);
        return FALSE;
      }
      AttributeTypea = 0x11003C;
    }
  }
  else
  {
    AttributeTypea = 0x11002C;
  }
  size = AttributeValueLength;
  localSize = strlen(AttributeName) + 1;
  AttributeValueLengtha = localSize + AttributeValueLength;

  alloc = RtlAllocateHeap(GetProcessHeap(), 0, AttributeValueLengtha);
  other = alloc;
  if ( alloc )
  {
    memcpy(alloc, AttributeName, localSize);
    number = 0;
    if ( size )
    {
      memcpy((char *)other + localSize, AttributeValue, size);
      number = 0;
    }
    status = NtFsControlFile(
               Pipe,
               (HANDLE)number,
               (PIO_APC_ROUTINE)number,
               (PVOID)number,
               &IoStatusBlock,
               AttributeTypea,
               other,
               AttributeValueLengtha,
               (PVOID)number,
               number);
    if ( status >= 0 )
    {
      result = TRUE;
    }
    else
    {
      BaseSetLastNTError(status);
      result = FALSE;
    }
	RtlFreeHeap(GetProcessHeap(), 0, other);
  }
  else
  {
    SetLastError(8u);
    result = FALSE;
  }
  return result;
}