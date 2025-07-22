/*++

Copyright (c) 2017 Shorthorn Project

Module Name:

    support.c

Abstract:

    This module implements various conversion routines
    that transform Win32 parameters into NT parameters.

Author:

    Skulltrail 18-March-2017

Revision History:

--*/
 
#define NDEBUG

#include "main.h"

BOOLEAN BaseRunningInServerProcess = FALSE;

PRTL_CONVERT_STRING Basep8BitStringToUnicodeString = RtlAnsiStringToUnicodeString;

VOID
WINAPI
BaseProcessStart(
	PPROCESS_START_ROUTINE lpStartAddress
)
{
    _SEH2_TRY
    {
        /* Set our Start Address */
        NtSetInformationThread(NtCurrentThread(),
                               ThreadQuerySetWin32StartAddress,
                               &lpStartAddress,
                               sizeof(PPROCESS_START_ROUTINE));

        /* Call the Start Routine */
        ExitThread(lpStartAddress());
    }
    _SEH2_EXCEPT(UnhandledExceptionFilter(_SEH2_GetExceptionInformation()))
    {
        /* Get the Exit code from the SEH Handler */
        if (!BaseRunningInServerProcess)
        {
            /* Kill the whole process, usually */
            ExitProcess(_SEH2_GetExceptionCode());
        }
        else
        {
            /* If running inside CSRSS, kill just this thread */
            ExitThread(_SEH2_GetExceptionCode());
        }
    }
    _SEH2_END;
}

VOID
WINAPI
BaseThreadStart(
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter
)
{
    /* Attempt to call the Thread Start Address */
    _SEH2_TRY
    {
        /* Legacy check which is still used today for Win32 threads */
        if (NtCurrentTeb()->NtTib.Version == (30 << 8)) // OS/2 V3.0 ("Cruiser")
        {
            /* This registers the termination port with CSRSS */
            if (!BaseRunningInServerProcess) CsrNewThread();
        }

        /* Get the exit code from the Thread Start */
        ExitThread(lpStartAddress(lpParameter));
    }
    _SEH2_EXCEPT(UnhandledExceptionFilter(_SEH2_GetExceptionInformation()))
    {
        /* Get the Exit code from the SEH Handler */
        if (!BaseRunningInServerProcess)
        {
            /* Kill the whole process, usually */
            ExitProcess(_SEH2_GetExceptionCode());
        }
        else
        {
            /* If running inside CSRSS, kill just this thread */
            ExitThread(_SEH2_GetExceptionCode());
        }
    }
    _SEH2_END;
}

/*
 * Converts an ANSI or OEM String to the TEB StaticUnicodeString
 */
PUNICODE_STRING
WINAPI
Basep8BitStringToStaticUnicodeString(
	IN LPCSTR String
)
{
    PUNICODE_STRING StaticString = &(NtCurrentTeb()->StaticUnicodeString);
    ANSI_STRING AnsiString;
    NTSTATUS Status;

    /* Initialize an ANSI String */
    Status = RtlInitAnsiStringEx(&AnsiString, String);
    if (!NT_SUCCESS(Status))
    {
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        /* Convert it */
        Status = Basep8BitStringToUnicodeString(StaticString, &AnsiString, FALSE);
    }

    if (NT_SUCCESS(Status)) return StaticString;

    if (Status == STATUS_BUFFER_OVERFLOW)
    {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
    }
    else
    {
        BaseSetLastNTError(Status);
    }

    return NULL;
}

POBJECT_ATTRIBUTES 
WINAPI 
BaseFormatObjectAttributes( 	
	OUT POBJECT_ATTRIBUTES  	ObjectAttributes,
	IN PSECURITY_ATTRIBUTES SecurityAttributes  	OPTIONAL,
	IN PUNICODE_STRING  	ObjectName 
) 		
{
    ULONG Attributes;
    HANDLE RootDirectory;
    PVOID SecurityDescriptor;
    DPRINTF("BaseFormatObjectAttributes. Security: %p, Name: %p\n",
            SecurityAttributes, ObjectName);

    /* Get the attributes if present */
    if (SecurityAttributes)
    {
        Attributes = SecurityAttributes->bInheritHandle ? OBJ_INHERIT : 0;
        SecurityDescriptor = SecurityAttributes->lpSecurityDescriptor;
    }
    else
    {
        if (!ObjectName) return NULL;
        Attributes = 0;
        SecurityDescriptor = NULL;
    }

    if (ObjectName)
    {
        Attributes |= OBJ_OPENIF;
        RootDirectory = BaseGetNamedObjectDirectory();
    }
    else
    {
        RootDirectory = NULL;
    }

    /* Create the Object Attributes */
    InitializeObjectAttributes(ObjectAttributes,
                               ObjectName,
                               Attributes,
                               RootDirectory,
                               SecurityDescriptor);
    DPRINTF("Attributes: %lx, RootDirectory: %p, SecurityDescriptor: %p\n",
            Attributes, RootDirectory, SecurityDescriptor);
    return ObjectAttributes;
}

/*
 * Converts an ANSI or OEM String to the TEB DynamicUnicodeString
 */
BOOLEAN 
WINAPI 
Basep8BitStringToDynamicUnicodeString(
	OUT PUNICODE_STRING UnicodeString, 
	IN LPCSTR String
) 		
{
    ANSI_STRING AnsiString;
    NTSTATUS Status;

    /* Initialize an ANSI String */
    Status = RtlInitAnsiStringEx(&AnsiString, String);
    if (!NT_SUCCESS(Status))
    {
        Status = STATUS_BUFFER_OVERFLOW;
    }
    else
    {
        /* Convert it */
        Status = Basep8BitStringToUnicodeString(UnicodeString, &AnsiString, TRUE);
    }

    if (NT_SUCCESS(Status)) return TRUE;

    if (Status == STATUS_BUFFER_OVERFLOW)
    {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
    }
    else
    {
        BaseSetLastNTError(Status);
    }

    return FALSE;
}

BOOL 
WINAPI 
BasepAnsiStringToDynamicUnicodeString(
	PUNICODE_STRING DestinationString, 
	PCSZ SourceStringParameter
)
{
  NTSTATUS status; 
  STRING SourceString; 

  if ( RtlInitAnsiStringEx(&SourceString, SourceStringParameter) < 0 )
    goto LABEL_10;
  status = RtlAnsiStringToUnicodeString(DestinationString, &SourceString, 1u);
  if ( NT_SUCCESS(status) )
    return TRUE;
  if ( status == STATUS_BUFFER_OVERFLOW )
LABEL_10:
    SetLastError(206);
  else
    BaseSetLastNTError(status);
  return FALSE;
}

/*
 * @implemented
 */
PLARGE_INTEGER 
WINAPI 
BaseFormatTimeOut(
	OUT PLARGE_INTEGER  	Timeout,
	IN DWORD  	Milliseconds 
) 		
{
    if ( (LONG) Milliseconds == -1 ) {
        return( NULL );
        }
    Timeout->QuadPart = UInt32x32To64( Milliseconds, 10000 );
    Timeout->QuadPart *= -1;
    return Timeout;
}