/*++

Copyright (c) 2025 Shorthorn Project

Module Name:

    ldrrsrc.c

Abstract:

    This module implements RTL Loader APIs

Author:

    Skulltrail 04-July-2025

Revision History:

--*/

#include <main.h>

USHORT
NTAPI
NameToOrdinal(
    _In_ PCSTR ExportName,
    _In_ PVOID ImageBase,
    _In_ ULONG NumberOfNames,
    _In_ PULONG NameTable,
    _In_ PUSHORT OrdinalTable)
{
    LONG Low, Mid, High, Ret;

    /* Fail if no names */
    if (!NumberOfNames)
        return -1;

    /* Do a binary search */
    Low = Mid = 0;
    High = NumberOfNames - 1;
    while (High >= Low)
    {
        /* Get new middle value */
        Mid = (Low + High) >> 1;

        /* Compare name */
        Ret = strcmp(ExportName, (PCHAR)RVA(ImageBase, NameTable[Mid]));
        if (Ret < 0)
        {
            /* Update high */
            High = Mid - 1;
        }
        else if (Ret > 0)
        {
            /* Update low */
            Low = Mid + 1;
        }
        else
        {
            /* We got it */
            break;
        }
    }

    /* Check if we couldn't find it */
    if (High < Low)
        return -1;

    /* Otherwise, this is the ordinal */
    return OrdinalTable[Mid];
}

/**
 * @brief
 * ReactOS-only helper routine for RtlFindExportedRoutineByName(),
 * that provides a finer granularity regarding the nature of the
 * export, and the failure reasons.
 *
 * @param[in]   ImageBase
 * The base address of the loaded image.
 *
 * @param[in]   ExportName
 * The name of the export, given as an ANSI NULL-terminated string.
 *
 * @param[out]  Function
 * The address of the named exported routine, or NULL if not found.
 * If the export is a forwarder (see @p IsForwarder below), this
 * address points to the forwarder name.
 *
 * @param[out]  IsForwarder
 * An optional pointer to a BOOLEAN variable, that is set to TRUE
 * if the found export is a forwarder, and FALSE otherwise.
 *
 * @param[in]   NotFoundStatus
 * The status code to return in case the export could not be found
 * (examples: STATUS_ENTRYPOINT_NOT_FOUND, STATUS_PROCEDURE_NOT_FOUND).
 *
 * @return
 * A status code as follows:
 * - STATUS_SUCCESS if the named exported routine is found;
 * - The custom @p NotFoundStatus if the export could not be found;
 * - STATUS_INVALID_PARAMETER if the image is invalid or does not
 *   contain an Export Directory.
 *
 * @note
 * See RtlFindExportedRoutineByName() for more remarks.
 * Used by psmgr.c PspLookupSystemDllEntryPoint() as well.
 **/
NTSTATUS
NTAPI
RtlpFindExportedRoutineByName(
    _In_ PVOID ImageBase,
    _In_ PCSTR ExportName,
    _Out_ PVOID* Function,
    _Out_opt_ PBOOLEAN IsForwarder,
    _In_ NTSTATUS NotFoundStatus)
{
    PIMAGE_EXPORT_DIRECTORY ExportDirectory;
    PULONG NameTable;
    PUSHORT OrdinalTable;
    ULONG ExportSize;
    USHORT Ordinal;
    PULONG ExportTable;
    ULONG_PTR FunctionAddress;

    /* Get the export directory */
    ExportDirectory = RtlImageDirectoryEntryToData(ImageBase,
                                                   TRUE,
                                                   IMAGE_DIRECTORY_ENTRY_EXPORT,
                                                   &ExportSize);
    if (!ExportDirectory)
        return STATUS_INVALID_PARAMETER;

    /* Setup name tables */
    NameTable = (PULONG)RVA(ImageBase, ExportDirectory->AddressOfNames);
    OrdinalTable = (PUSHORT)RVA(ImageBase, ExportDirectory->AddressOfNameOrdinals);

    /* Get the ordinal */
    Ordinal = NameToOrdinal(ExportName,
                            ImageBase,
                            ExportDirectory->NumberOfNames,
                            NameTable,
                            OrdinalTable);

    /* Check if we couldn't find it */
    if (Ordinal == -1)
        return NotFoundStatus;

    /* Validate the ordinal */
    if (Ordinal >= ExportDirectory->NumberOfFunctions)
        return NotFoundStatus;

    /* Resolve the function's address */
    ExportTable = (PULONG)RVA(ImageBase, ExportDirectory->AddressOfFunctions);
    FunctionAddress = (ULONG_PTR)RVA(ImageBase, ExportTable[Ordinal]);

    /* Check if the function is actually a forwarder */
    if (IsForwarder)
    {
        *IsForwarder = FALSE;
        if ((FunctionAddress > (ULONG_PTR)ExportDirectory) &&
            (FunctionAddress < (ULONG_PTR)ExportDirectory + ExportSize))
        {
            /* It is, and points to the forwarder name */
            *IsForwarder = TRUE;
        }
    }

    /* We've found it */
    *Function = (PVOID)FunctionAddress;
    return STATUS_SUCCESS;
}

/**
 * @brief
 * Finds the address of a given named exported routine in a loaded image.
 * Note that this function does not support forwarders.
 *
 * @param[in]   ImageBase
 * The base address of the loaded image.
 *
 * @param[in]   ExportName
 * The name of the export, given as an ANSI NULL-terminated string.
 *
 * @return
 * The address of the named exported routine, or NULL if not found.
 * If the export is a forwarder, this function returns NULL as well.
 *
 * @note
 * This routine was originally named MiLocateExportName(), with a separate
 * duplicated MiFindExportedRoutineByName() one (taking a PANSI_STRING)
 * on Windows <= 2003. Both routines have been then merged and renamed
 * to MiFindExportedRoutineByName() on Windows 8 (taking a PCSTR instead),
 * and finally renamed and exported as RtlFindExportedRoutineByName() on
 * Windows 10.
 *
 * @see https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/mm/sysload/mmgetsystemroutineaddress.htm
 **/
PVOID
NTAPI
RtlFindExportedRoutineByName(
    _In_ PVOID ImageBase,
    _In_ PCSTR ExportName)
{
    NTSTATUS Status;
    BOOLEAN IsForwarder = FALSE;
    PVOID Function;

    /* Call the internal API */
    Status = RtlpFindExportedRoutineByName(ImageBase,
                                           ExportName,
                                           &Function,
                                           &IsForwarder,
                                           STATUS_ENTRYPOINT_NOT_FOUND);
    if (!NT_SUCCESS(Status))
        return NULL;

    /* If the export is actually a forwarder, log the error and fail */
    if (IsForwarder)
    {
        DbgPrint("RtlFindExportedRoutineByName does not support forwarders!\n");
        return NULL;
    }

    /* We've found the export */
    return Function;
}