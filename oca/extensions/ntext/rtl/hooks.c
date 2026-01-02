/*++

Copyright (c) 2024 Shorthorn Project

Module Name:

    hooks.c

Abstract:

    Implement Hooks of Rtl functions

Author:

    Skulltrail 07-November-2024

Revision History:

--*/

#include "main.h"
#include <ntstrsafe.h>


//
// Use bit 63 to indicate that the new style bit layout is followed.
//
#define NEW_STYLE_BIT_MASK              0x8000000000000000


//
// Condition extractor for the old style mask.
//
#define OLD_CONDITION(_m_,_t_)  (ULONG)((_m_&(0xff<<(1<<_t_)))>>(1<<_t_))

//
// Test to see  if the mask is an old style mask.
//
#define OLD_STYLE_CONDITION_MASK(_m_)  (((_m_) & NEW_STYLE_BIT_MASK)  == 0)

#define RTL_GET_CONDITION(_m_, _t_) \
        (OLD_STYLE_CONDITION_MASK(_m_) ? (OLD_CONDITION(_m_,_t_)) : \
                RtlpVerGetConditionMask((_m_), (_t_)))

#define LEXICAL_COMPARISON        1     /* Do string comparison. Used for minor numbers */
#define MAX_STRING_LENGTH         20    /* Maximum number of digits for sprintf */

ULONG
RtlpVerGetConditionMask(
        ULONGLONG       ConditionMask,
        ULONG   TypeMask
        );
		
#define MAX_PATH 260

static const WCHAR SECTION_NAME[] = L"\\BaseNamedObjects\\SharedAppCompat";

typedef struct _OCA_COMPATIBILITY_INFO{
	ULONG MajorVersion;
	ULONG MinorVersion;
	ULONG BuildNumber;
    UCHAR CSDVersion;	
	WCHAR emuPath[MAX_PATH];
	WCHAR msiPath[MAX_PATH];	
}OCA_COMPATIBILITY_INFO, *POCA_COMPATIBILITY_INFO;

void SanitizeFilenameForRegistry(const WCHAR* src, WCHAR* dst, size_t dstSize)
{
    size_t i;
    for (i = 0; src[i] != 0 && i < dstSize - 1; i++)
    {
        if (src[i] == L'\\')
            dst[i] = L'/'; // substitui barra invertida por barra normal
        else
            dst[i] = src[i];
    }
    dst[i] = 0;
}

ULONG
FindLikelyCreatorProcessPid(PSYSTEM_PROCESS_INFORMATION sysInfo, ULONG CurrentPid)
{
    ULONG offset = 0;
    PSYSTEM_PROCESS_INFORMATION p = NULL;
    PSYSTEM_PROCESS_INFORMATION best = NULL;
    LONGLONG closestDiff = 0x7FFFFFFFFFFFFFFFLL;
    LARGE_INTEGER childCreateTime;
    ULONG parentPid;

    /* 1) Achar registro do processo atual */
    while (1)
    {
        p = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)sysInfo + offset);

        if ((ULONG_PTR)p->UniqueProcessId == CurrentPid)
        {
            parentPid = (ULONG)p->InheritedFromUniqueProcessId;
            childCreateTime = p->CreateTime;
            break;
        }

        if (p->NextEntryOffset == 0)
            return 0;

        offset += p->NextEntryOffset;
    }

    /* 2) Se PPID for claramente válido, usamos ele */
    if (parentPid != 0 &&
        parentPid != CurrentPid)
    {
        // Listar pais “suspeitos” que NÃO devem ser considerados pais reais
        static const ULONG badParents[] = {
            0, 4,    /* Idle / System */
            8,       /* smss */
            528,     /* services */
            576,     /* lsass */
        };

        size_t i;
        BOOLEAN bad = FALSE;
        for (i = 0; i < sizeof(badParents) / sizeof(badParents[0]); i++)
        {
            if (parentPid == badParents[i])
            {
                bad = TRUE;
                break;
            }
        }

        if (!bad)
            return parentPid;
    }

    /* 3) Heurística de criador real:
          Achar processo cuja criação é imediatamente anterior ao filho */
    offset = 0;    

    while (1)
    {
        p = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)sysInfo + offset);

        if ((ULONG_PTR)p->UniqueProcessId != CurrentPid)
        {
            /* diferença de tempo */
            LONGLONG diff = childCreateTime.QuadPart - p->CreateTime.QuadPart;

            if (diff > 0 && diff < closestDiff)
            {
                closestDiff = diff;
                best = p;
            }
        }

        if (p->NextEntryOffset == 0)
            break;

        offset += p->NextEntryOffset;
    }

    if (best)
        return (ULONG)(ULONG_PTR)best->UniqueProcessId;

    return parentPid;
}


/* -- GetParentProcessOSVersionByPid -- */
/* Recebe ProcessId (PID) passado no parâmetro ProcessId (cast internamente). */
BOOLEAN
GetParentProcessOSVersionByPid(
    HANDLE ProcessId,      /* trata-se de PID, feito cast para CLIENT_ID */
    ULONG *MajorVersion,
    ULONG *MinorVersion,
    ULONG *BuildNumber
)
{
    NTSTATUS status;
    PROCESS_BASIC_INFORMATION pbi;
    ULONG retLen;
    HANDLE hProcess;
    OBJECT_ATTRIBUTES oa;
    CLIENT_ID cid;
    SIZE_T bytesRead;
    PEB pebRemote;

    RtlZeroMemory(&pbi, sizeof(pbi));
    retLen = 0;
    hProcess = NULL;
    RtlZeroMemory(&oa, sizeof(oa));
    cid.UniqueProcess = (HANDLE)(ULONG_PTR)ProcessId; /* caller supplies PID */
    cid.UniqueThread = (HANDLE)0;
    bytesRead = 0;
    RtlZeroMemory(&pebRemote, sizeof(pebRemote));

    InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);

    status = NtOpenProcess(&hProcess,
                           PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                           &oa,
                           &cid);
    if (!NT_SUCCESS(status))
    {
        DbgPrint("GetParentProcessOSVersionByPid: ZwOpenProcess falhou 0x%08X\n", status);
        return FALSE;
    }

    status = ZwQueryInformationProcess(hProcess,
                                       ProcessBasicInformation,
                                       &pbi,
                                       sizeof(pbi),
                                       &retLen);
    if (!NT_SUCCESS(status))
    {
        DbgPrint("GetParentProcessOSVersionByPid: ZwQueryInformationProcess falhou 0x%08X\n", status);
        ZwClose(hProcess);
        return FALSE;
    }

    if (pbi.PebBaseAddress == NULL) {
        DbgPrint("GetParentProcessOSVersionByPid: PEB base address NULL\n");
        ZwClose(hProcess);
        return FALSE;
    }

    status = ZwReadVirtualMemory(hProcess,
                                 pbi.PebBaseAddress,
                                 &pebRemote,
                                 sizeof(pebRemote),
                                 &bytesRead);

    if (!NT_SUCCESS(status) || bytesRead < sizeof(pebRemote))
    {
        DbgPrint("GetParentProcessOSVersionByPid: ZwReadVirtualMemory falhou 0x%08X bytesRead=%Iu\n",
                 status, bytesRead);
        ZwClose(hProcess);
        return FALSE;
    }

    /* Preenchendo os valores de saída */
    if (MajorVersion) *MajorVersion = (ULONG)pebRemote.OSMajorVersion;
    if (MinorVersion) *MinorVersion = (ULONG)pebRemote.OSMinorVersion;
    if (BuildNumber) *BuildNumber = (ULONG)pebRemote.OSBuildNumber;

    ZwClose(hProcess);
    return TRUE;
}

ULONG
GetNextPointValue (
    IN OUT WCHAR **p,
    IN OUT ULONG *len
    )
{
    ULONG Number;

    Number = 0;

    while (*len && (UNICODE_NULL != **p) && **p != L'.') {
        if ( L' ' != **p ) {
            Number = (Number * 10) + ( (ULONG)**p - L'0' );
        }

        (*p)++;
        (*len)--;
    }

    if ((*len) && (L'.' == **p)) {
        (*p)++;
        (*len)--;
    }

    return Number;
}

BOOLEAN GetMsiPathFromCommandLine(
    LPCWSTR CmdLine,
    LPWSTR OutPath,
    ULONG OutSize)
{
    const WCHAR *p = CmdLine;
    const WCHAR *start, *end;
    ULONG len;

    if (!CmdLine || !OutPath || OutSize == 0)
        return FALSE;

    OutPath[0] = 0;

    /* Procura pelo parâmetro /i ou /package */
    while (*p)
    {
        /* Ignora espaços */
        while (*p == L' ') p++;

        /* Verifica /i */
        if ((p[0] == L'/' || p[0] == L'-') &&
            (p[1] == L'i' || p[1] == L'I'))
        {
            p += 2;

            /* pula ":" caso exista */
            if (*p == L':') p++;

            /* pula espaços */
            while (*p == L' ') p++;

            goto extract;
        }

        /* Verifica /package */
        if ((p[0] == L'/' || p[0] == L'-') &&
            ( _wcsnicmp(&p[1], L"package", 7) == 0))
        {
            p += 8;

            if (*p == L':') p++;
            while (*p == L' ') p++;

            goto extract;
        }

        p++;
    }

    return FALSE;

extract:

    /* Caso esteja entre aspas */
    if (*p == L'"')
    {
        p++;
        start = p;

        while (*p && *p != L'"')
            p++;

        end = p;

        len = (ULONG)(end - start);

        if (len >= OutSize)
            return FALSE;

        wcsncpy(OutPath, start, len);
        OutPath[len] = 0;

        return TRUE;
    }

    /* Caso esteja sem aspas */
    start = p;
    while (*p && *p != L' ')
        p++;

    end = p;

    len = (ULONG)(end - start);

    if (len >= OutSize)
        return FALSE;

    wcsncpy(OutPath, start, len);
    OutPath[len] = 0;

    return TRUE;
}


BOOLEAN
StoreInSharedSection(POCA_COMPATIBILITY_INFO OcaCompatInfo)
{
    UNICODE_STRING sectionName;
    PVOID baseAddress;
    SIZE_T viewSize;
    OBJECT_ATTRIBUTES objAttr;
    HANDLE hSection = NULL;
    LARGE_INTEGER maxSize;
    NTSTATUS status;

    RtlInitUnicodeString(&sectionName, SECTION_NAME);
    InitializeObjectAttributes(&objAttr, &sectionName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    maxSize.QuadPart = sizeof(OCA_COMPATIBILITY_INFO);

    DbgPrint("[StoreInSharedSection] Criando seção… tamanho=%llu bytes\n", maxSize.QuadPart);

    status = NtCreateSection(
        &hSection,
        SECTION_ALL_ACCESS,
        &objAttr,
        &maxSize,
        PAGE_READWRITE,
        SEC_COMMIT,
        NULL
    );

    if (!NT_SUCCESS(status)) {
        DbgPrint("[StoreInSharedSection] Erro NtCreateSection: 0x%08X\n", status);
        return FALSE;
    }

    baseAddress = NULL;
    viewSize = (SIZE_T)maxSize.QuadPart;

    DbgPrint("[StoreInSharedSection] Mapeando seção no processo atual…\n");

    status = NtMapViewOfSection(
        hSection,
        (HANDLE)-1,  // Current process
        &baseAddress,
        0,
        0,
        NULL,
        &viewSize,
        ViewShare,
        0,
        PAGE_READWRITE
    );

    if (!NT_SUCCESS(status)) {
        DbgPrint("[StoreInSharedSection] Erro NtMapViewOfSection: 0x%08X\n", status);
        DbgPrint("[StoreInSharedSection] Desmapeando seção…\n");
        NtUnmapViewOfSection((HANDLE)-1, baseAddress);		
        NtClose(hSection);
        return FALSE;
    }

    DbgPrint("[StoreInSharedSection] Seção mapeada em %p, copia do caminho…\n", baseAddress);

    memcpy(baseAddress, OcaCompatInfo, sizeof(OCA_COMPATIBILITY_INFO));

    DbgPrint("[StoreInSharedSection] Finalizado com sucesso.\n");
    return TRUE;
}

BOOLEAN
LoadFromSharedSection(POCA_COMPATIBILITY_INFO OcaCompatInfo)
{
    UNICODE_STRING sectionName;
    PVOID baseAddress;
    SIZE_T viewSize;
    HANDLE hSection = NULL;
    NTSTATUS status;
    OBJECT_ATTRIBUTES objAttr;
	//OCA_COMPATIBILITY_INFO OcaCompatInfo;

    DbgPrint("[LoadFromSharedSection] Iniciando…\n");

    RtlInitUnicodeString(&sectionName, SECTION_NAME);
    InitializeObjectAttributes(&objAttr, &sectionName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    DbgPrint("[LoadFromSharedSection] Abrindo seção compartilhada…\n");
	
	__try{

		status = NtOpenSection(
			&hSection,
			SECTION_MAP_READ,
			&objAttr
		);

		if (!NT_SUCCESS(status)) {
			DbgPrint("[LoadFromSharedSection] Erro NtOpenSection: 0x%08X\n", status);
			return FALSE;
		}

		baseAddress = NULL;
		viewSize = 0;

		DbgPrint("[LoadFromSharedSection] Mapeando seção para leitura…\n");

		status = NtMapViewOfSection(
			hSection,
			(HANDLE)-1,
			&baseAddress,
			0,
			0,
			NULL,
			&viewSize,
			ViewShare,
			0,
			PAGE_READONLY
		);

		if (!NT_SUCCESS(status)) {
			DbgPrint("[LoadFromSharedSection] Erro NtMapViewOfSection: 0x%08X\n", status);
			NtClose(hSection);
			return FALSE;
		}

		DbgPrint("[LoadFromSharedSection] Seção mapeada em %p (tam=%llu). Copiando string…\n",
				 baseAddress, (unsigned long long)viewSize);

		memcpy(OcaCompatInfo, baseAddress, sizeof(OCA_COMPATIBILITY_INFO));
		
		DbgPrint("[LoadFromSharedSection] Desmapeando seção…\n");
		NtUnmapViewOfSection((HANDLE)-1, baseAddress);

		DbgPrint("[LoadFromSharedSection] Fechando seção…\n");
		NtClose(hSection);		

		return TRUE;
	}__except ( EXCEPTION_EXECUTE_HANDLER ) {
		return FALSE;
	}	

    DbgPrint("[LoadFromSharedSection] Finalizado com sucesso.\n");
    return FALSE;
}

BOOLEAN
HasSharedSection(POCA_COMPATIBILITY_INFO OcaCompatInfo)
{
    UNICODE_STRING sectionName;
    HANDLE hSection = NULL;
    NTSTATUS status;
    OBJECT_ATTRIBUTES objAttr;

    DbgPrint("[LoadFromSharedSection] Iniciando…\n");

    RtlInitUnicodeString(&sectionName, SECTION_NAME);
    InitializeObjectAttributes(&objAttr, &sectionName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    DbgPrint("[LoadFromSharedSection] Abrindo seção compartilhada…\n");
	
	status = NtOpenSection(
			&hSection,
			SECTION_MAP_READ,
			&objAttr
		);

	if (!NT_SUCCESS(status)) {
		return FALSE;
	}
	
    return TRUE;
}

/* -- ReadEmulatedVersion -- */
/* Preenche EmulatedVersion com uma UNICODE_STRING alocada (chamador deve liberar) */
BOOLEAN
ReadEmulatedVersion(
    PUNICODE_STRING EmulatedVersion,
    PWSTR FilePath
)
{
    PKEY_VALUE_PARTIAL_INFORMATION KeyInfo;
    UNICODE_STRING valueKeyName;
    UNICODE_STRING UnicodeKey;
    OBJECT_ATTRIBUTES Obj;
    HANDLE Handle;
    NTSTATUS status;
    WCHAR buffer[128];
    WCHAR SanitizedPath[MAX_PATH];
    WCHAR FullKeyPath[MAX_PATH];
    WCHAR Version[MAX_PATH];
    ULONG informationLength;
    ULONG_PTR currentPid;
    ULONG_PTR parentPid = 0;
    PSYSTEM_PROCESS_INFORMATION sysInfo = NULL;
    ULONG sysInfoSize = 0x10000;
    PPEB Peb;
    PWSTR heapBuf = NULL;
    SIZE_T neededBytes;

    RtlZeroMemory(EmulatedVersion, sizeof(*EmulatedVersion));
    KeyInfo = (PKEY_VALUE_PARTIAL_INFORMATION)buffer;
    RtlZeroMemory(buffer, sizeof(buffer));

    //DbgPrint("ReadEmulatedVersion: Iniciando para arquivo: %ws\n", FilePath);

    /* Sanitiza o caminho */
    SanitizeFilenameForRegistry(FilePath, SanitizedPath, MAX_PATH);
    //DbgPrint("ReadEmulatedVersion: Caminho sanitizado: %ws\n", SanitizedPath);

    /* Constroi caminho completo da chave */
    swprintf(FullKeyPath,
             L"\\REGISTRY\\MACHINE\\SOFTWARE\\OCA\\Settings\\%s",
             SanitizedPath);

    //DbgPrint("ReadEmulatedVersion: Tentando abrir chave: %ws\n", FullKeyPath);

    RtlInitUnicodeString(&UnicodeKey, FullKeyPath);
    InitializeObjectAttributes(&Obj, &UnicodeKey, OBJ_CASE_INSENSITIVE, NULL, NULL);

    /* Tentativa de abrir chave */
    status = NtOpenKey(&Handle, GENERIC_READ, &Obj);
    if (!NT_SUCCESS(status))
    {
        DbgPrint("ReadEmulatedVersion: Chave não encontrada. Status=0x%08X\n", status);

        /* Buscar por processo criador */
        currentPid = (ULONG_PTR)NtCurrentTeb()->ClientId.UniqueProcess;
        DbgPrint("ReadEmulatedVersion: PID atual = %u\n", (ULONG)currentPid);

        /* --- Loop para ler lista de processos --- */
        for (;;)
        {
            sysInfo = (PSYSTEM_PROCESS_INFORMATION)
                RtlAllocateHeap(RtlProcessHeap(), 0, sysInfoSize);

            if (!sysInfo)
            {
                DbgPrint("ReadEmulatedVersion: Falha RtlAllocateHeap(%lu)\n", sysInfoSize);
                return FALSE;
            }

            status = ZwQuerySystemInformation(
                SystemProcessInformation,
                sysInfo,
                sysInfoSize,
                &sysInfoSize
            );

            if (NT_SUCCESS(status))
                break;

            // DbgPrint("ReadEmulatedVersion: ZwQuerySystemInformation precisa realocar (status=0x%08X)\n",
                     // status);

            RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
            sysInfo = NULL;

            if (status != STATUS_INFO_LENGTH_MISMATCH &&
                status != STATUS_BUFFER_TOO_SMALL)
            {
                DbgPrint("ReadEmulatedVersion: Erro inesperado ZwQuerySystemInformation: 0x%08X\n",
                         status);
                return FALSE;
            }
        }

        /* --- Descobrir processo criador real --- */
        parentPid = FindLikelyCreatorProcessPid(sysInfo, (ULONG)currentPid);

        // if (parentPid)
            // DbgPrint("ReadEmulatedVersion: Processo criador detectado: PID=%u\n", (ULONG)parentPid);
        // else
            // DbgPrint("ReadEmulatedVersion: Processo criador NÃO encontrado.\n");

        /* Se achou criador real */
        if (parentPid)
        {
            ULONG Maj, Min, Bld;

            //DbgPrint("ReadEmulatedVersion: Lendo versão do Windows do pai via PEB remoto...\n");

            if (GetParentProcessOSVersionByPid((HANDLE)parentPid, &Maj, &Min, &Bld))
            {
                Peb = NtCurrentPeb();

                // DbgPrint("ReadEmulatedVersion: Pai=%u -> versão detectada %lu.%lu.%lu\n",
                         // (ULONG)parentPid, Maj, Min, Bld);

                // DbgPrint("ReadEmulatedVersion: Processo atual: %lu.%lu.%lu\n",
                         // Peb->OSMajorVersion, Peb->OSMinorVersion, Peb->OSBuildNumber);

                if (Peb->OSMajorVersion != Maj ||
                    Peb->OSMinorVersion != Min ||
                    Peb->OSBuildNumber != Bld)
                {
                    /* construir string final */
                    swprintf(Version, L"%lu.%lu.%lu", Maj, Min, Bld);

                    //DbgPrint("ReadEmulatedVersion: Versão será EMULADA como: %ws\n", Version);

                    neededBytes = (wcslen(Version) + 1) * sizeof(WCHAR);
                    heapBuf = (PWSTR)RtlAllocateHeap(RtlProcessHeap(), 0, neededBytes);

                    if (!heapBuf)
                    {
                        //DbgPrint("ReadEmulatedVersion: Falha ao alocar %Iu bytes\n", neededBytes);
                        RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
                        return FALSE;
                    }

                    RtlCopyMemory(heapBuf, Version, neededBytes);

                    EmulatedVersion->Buffer = heapBuf;
                    EmulatedVersion->Length = (USHORT)((wcslen(heapBuf)) * sizeof(WCHAR));
                    EmulatedVersion->MaximumLength = (USHORT)neededBytes;

                    DbgPrint("ReadEmulatedVersion: Sucesso — versão emulada definida.\n");

                    RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
                    return TRUE;
                }
                else
                {
                    DbgPrint("ReadEmulatedVersion: Versões iguais — emulação NÃO necessária.\n");
                    RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
                    return FALSE;
                }
            }

            //DbgPrint("ReadEmulatedVersion: Falha ao obter versão do processo criador.\n");
            RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
            return FALSE;
        }

        //DbgPrint("ReadEmulatedVersion: Criador não encontrado. Tentará registro normalmente.\n");
        RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);

        /* Continua para tentar registro */
    }

    /* Sucesso ao abrir chave */
    DbgPrint("ReadEmulatedVersion: Chave encontrada no registro!\n");

    /* Ler valor */
    RtlInitUnicodeString(&valueKeyName, L"CompatWindowsVersion");

    status = NtQueryValueKey(
        Handle,
        &valueKeyName,
        KeyValuePartialInformation,
        (PVOID)KeyInfo,
        sizeof(buffer),
        &informationLength
    );

    if (NT_SUCCESS(status) &&
        (KeyInfo->Type == REG_SZ || KeyInfo->Type == REG_MULTI_SZ))
    {
        PWSTR regStr = (PWSTR)KeyInfo->Data;
        SIZE_T lenChars = wcslen(regStr) + 1;
        SIZE_T lenBytes = lenChars * sizeof(WCHAR);

        //DbgPrint("ReadEmulatedVersion: Valor encontrado no registro: %ws\n", regStr);

        heapBuf = (PWSTR)RtlAllocateHeap(RtlProcessHeap(), 0, lenBytes);

        if (!heapBuf)
        {
            //DbgPrint("ReadEmulatedVersion: Falha ao alocar buffer de %Iu bytes\n", lenBytes);
            NtClose(Handle);
            return FALSE;
        }

        RtlCopyMemory(heapBuf, regStr, lenBytes);

        EmulatedVersion->Buffer = heapBuf;
        EmulatedVersion->Length = (USHORT)((lenChars - 1) * sizeof(WCHAR));
        EmulatedVersion->MaximumLength = (USHORT)lenBytes;

        //DbgPrint("ReadEmulatedVersion: Sucesso — valor carregado do registro.\n");

        NtClose(Handle);
        return TRUE;
    }

    DbgPrint("ReadEmulatedVersion: Valor NÃO encontrado na chave.\n");

    if (Handle)
        NtClose(Handle);

    return FALSE;
}

PSYSTEM_PROCESS_INFORMATION
FindProcessInfoByPid(
    PSYSTEM_PROCESS_INFORMATION List,
    ULONG Pid
)
{
    PSYSTEM_PROCESS_INFORMATION cur = List;

    for (;;)
    {
        if ((ULONG)(ULONG_PTR)cur->UniqueProcessId == Pid)
            return cur;

        if (cur->NextEntryOffset == 0)
            break;

        cur = (PSYSTEM_PROCESS_INFORMATION)
            ((PUCHAR)cur + cur->NextEntryOffset);
    }

    return NULL;
}

BOOLEAN
UnicodePathEquals(
    PWSTR A,
    PWSTR B
)
{
    if (!A || !B)
        return FALSE;

    while (*A && *B)
    {
        WCHAR ca = RtlUpcaseUnicodeChar(*A);
        WCHAR cb = RtlUpcaseUnicodeChar(*B);

        if (ca != cb)
            return FALSE;

        A++;
        B++;
    }

    return (*A == 0 && *B == 0);
}

BOOLEAN
GetProcessFullPathByPid(
    HANDLE ProcessId,
    PWSTR  Buffer,
    ULONG  BufferCch
)
{
    NTSTATUS status;
    HANDLE hProcess = NULL;
    PROCESS_BASIC_INFORMATION pbi;
    ULONG retLen;
    PPEB remotePeb;
    PRTL_USER_PROCESS_PARAMETERS procParams;
    UNICODE_STRING imagePath;

    OBJECT_ATTRIBUTES oa;
    CLIENT_ID cid;

    DbgPrint("[OCA][PATH] GetProcessFullPathByPid PID=%lu\n",
             (ULONG)(ULONG_PTR)ProcessId);

    InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);

    cid.UniqueProcess = ProcessId;
    cid.UniqueThread  = NULL;

    status = NtOpenProcess(
        &hProcess,
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        &oa,
        &cid
    );

    if (!NT_SUCCESS(status))
    {
        DbgPrint("[OCA][PATH] NtOpenProcess failed: 0x%08X\n", status);
        return FALSE;
    }

    status = NtQueryInformationProcess(
        hProcess,
        ProcessBasicInformation,
        &pbi,
        sizeof(pbi),
        &retLen
    );

    if (!NT_SUCCESS(status))
    {
        DbgPrint("[OCA][PATH] NtQueryInformationProcess failed: 0x%08X\n", status);
        NtClose(hProcess);
        return FALSE;
    }

    remotePeb = pbi.PebBaseAddress;

    status = NtReadVirtualMemory(
        hProcess,
        &remotePeb->ProcessParameters,
        &procParams,
        sizeof(procParams),
        NULL
    );

    if (!NT_SUCCESS(status) || !procParams)
    {
        DbgPrint("[OCA][PATH] Failed reading ProcessParameters ptr\n");
        NtClose(hProcess);
        return FALSE;
    }

    status = NtReadVirtualMemory(
        hProcess,
        &procParams->ImagePathName,
        &imagePath,
        sizeof(imagePath),
        NULL
    );

    if (!NT_SUCCESS(status) || !imagePath.Buffer)
    {
        DbgPrint("[OCA][PATH] Failed reading ImagePathName\n");
        NtClose(hProcess);
        return FALSE;
    }

    if ((imagePath.Length / sizeof(WCHAR)) >= BufferCch)
    {
        DbgPrint("[OCA][PATH] Buffer too small (%lu chars needed)\n",
                 imagePath.Length / sizeof(WCHAR));
        NtClose(hProcess);
        return FALSE;
    }

    status = NtReadVirtualMemory(
        hProcess,
        imagePath.Buffer,
        Buffer,
        imagePath.Length,
        NULL
    );

    if (!NT_SUCCESS(status))
    {
        DbgPrint("[OCA][PATH] Failed reading ImagePath buffer\n");
        NtClose(hProcess);
        return FALSE;
    }

    Buffer[imagePath.Length / sizeof(WCHAR)] = L'\0';

    DbgPrint("[OCA][PATH] FullPath = %ws\n", Buffer);

    NtClose(hProcess);
    return TRUE;
}

BOOLEAN
IsParentOrGrandparentMatchingCompatPath(
    POCA_COMPATIBILITY_INFO CompatInfo
)
{
    ULONG_PTR currentPid;
    ULONG parentPid = 0;
    ULONG grandParentPid = 0;

    WCHAR pathBuf[MAX_PATH];

    PSYSTEM_PROCESS_INFORMATION sysInfo = NULL;
    ULONG sysInfoSize = 0x10000;
    NTSTATUS status;

    PSYSTEM_PROCESS_INFORMATION curProc;
    PSYSTEM_PROCESS_INFORMATION parentProc;

    DbgPrint("[OCA][INHERIT] === START ===\n");
    DbgPrint("[OCA][INHERIT] Compat path: %ws\n", CompatInfo->emuPath);

    currentPid = (ULONG_PTR)NtCurrentTeb()->ClientId.UniqueProcess;

    /* Enumerar processos */
    for (;;)
    {
        sysInfo = RtlAllocateHeap(RtlProcessHeap(), 0, sysInfoSize);
        if (!sysInfo)
            return FALSE;

        status = ZwQuerySystemInformation(
            SystemProcessInformation,
            sysInfo,
            sysInfoSize,
            &sysInfoSize
        );

        if (NT_SUCCESS(status))
            break;

        RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
        sysInfo = NULL;

        if (status != STATUS_INFO_LENGTH_MISMATCH &&
            status != STATUS_BUFFER_TOO_SMALL)
            return FALSE;
    }

    curProc = FindProcessInfoByPid(sysInfo, (ULONG)currentPid);
    if (!curProc)
        goto cleanup;

    parentPid = (ULONG)(ULONG_PTR)
        curProc->InheritedFromUniqueProcessId;

    DbgPrint("[OCA][INHERIT] Parent PID=%lu\n", parentPid);

    /* --- PAI --- */
    if (parentPid &&
        GetProcessFullPathByPid((HANDLE)(ULONG_PTR)parentPid,
                                pathBuf, MAX_PATH))
    {
        if (UnicodePathEquals(pathBuf, CompatInfo->emuPath))
        {
            DbgPrint("[OCA][INHERIT] MATCH parent\n");
            goto match;
        }
    }

    parentProc = FindProcessInfoByPid(sysInfo, parentPid);
    if (!parentProc)
        goto cleanup;

    grandParentPid = (ULONG)(ULONG_PTR)
        parentProc->InheritedFromUniqueProcessId;

    DbgPrint("[OCA][INHERIT] Grandparent PID=%lu\n", grandParentPid);

    /* --- AVÔ --- */
    if (grandParentPid &&
        GetProcessFullPathByPid((HANDLE)(ULONG_PTR)grandParentPid,
                                pathBuf, MAX_PATH))
    {
        if (UnicodePathEquals(pathBuf, CompatInfo->emuPath))
        {
            DbgPrint("[OCA][INHERIT] MATCH grandparent\n");
            goto match;
        }
    }

cleanup:
    RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
    DbgPrint("[OCA][INHERIT] NO MATCH\n");
    return FALSE;

match:
    RtlFreeHeap(RtlProcessHeap(), 0, sysInfo);
    DbgPrint("[OCA][INHERIT] SUCCESS\n");
    return TRUE;
}

BOOLEAN
ReadGlobalEmulationVersion(
    PUNICODE_STRING EmulatedVersion
)
{
    OBJECT_ATTRIBUTES Obj;
    UNICODE_STRING KeyName;
    UNICODE_STRING ValueName;
    HANDLE KeyHandle = NULL;
    NTSTATUS status;

    WCHAR buffer[128];
    PKEY_VALUE_PARTIAL_INFORMATION KeyInfo;
    ULONG informationLength;

    PWSTR heapBuf;
    SIZE_T lenChars;
    SIZE_T lenBytes;

    RtlZeroMemory(EmulatedVersion, sizeof(*EmulatedVersion));
    KeyInfo = (PKEY_VALUE_PARTIAL_INFORMATION)buffer;
    RtlZeroMemory(buffer, sizeof(buffer));

    RtlInitUnicodeString(
        &KeyName,
        L"\\REGISTRY\\MACHINE\\SOFTWARE\\OCA\\Settings"
    );

    InitializeObjectAttributes(
        &Obj,
        &KeyName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );

    status = NtOpenKey(&KeyHandle, GENERIC_READ, &Obj);
    if (!NT_SUCCESS(status))
        return FALSE;

    RtlInitUnicodeString(&ValueName, L"GlobalVersion");

    status = NtQueryValueKey(
        KeyHandle,
        &ValueName,
        KeyValuePartialInformation,
        KeyInfo,
        sizeof(buffer),
        &informationLength
    );

    if (!NT_SUCCESS(status) || KeyInfo->Type != REG_SZ)
    {
        NtClose(KeyHandle);
        return FALSE;
    }

    lenChars = wcslen((PWSTR)KeyInfo->Data) + 1;
    lenBytes = lenChars * sizeof(WCHAR);

    heapBuf = (PWSTR)RtlAllocateHeap(
        RtlProcessHeap(),
        0,
        lenBytes
    );

    if (!heapBuf)
    {
        NtClose(KeyHandle);
        return FALSE;
    }

    RtlCopyMemory(heapBuf, KeyInfo->Data, lenBytes);

    EmulatedVersion->Buffer = heapBuf;
    EmulatedVersion->Length =
        (USHORT)((lenChars - 1) * sizeof(WCHAR));
    EmulatedVersion->MaximumLength =
        (USHORT)lenBytes;

    NtClose(KeyHandle);

    DbgPrint("[OCA] GlobalVersion aplicada: %ws\n", heapBuf);
    return TRUE;
}

BOOLEAN CheckIsUnsafeExe(LPWSTR ExePath){
	if(wcsstr(ExePath, L"svchost") != NULL || 
	   wcsstr(ExePath, L"explorer") != NULL || 
	   wcsstr(ExePath, L"smss") != NULL || 
	   wcsstr(ExePath, L"csrsrv") != NULL || 
	   wcsstr(ExePath, L"winlogon") != NULL || 
	   wcsstr(ExePath, L"wininit") != NULL || 
	   wcsstr(ExePath, L"searchindexer") != NULL || 
	   wcsstr(ExePath, L"wmiprvse") != NULL || 
	   wcsstr(ExePath, L"lsass") != NULL || 
	   wcsstr(ExePath, L"WindowsSearch") != NULL ||
	   wcsstr(ExePath, L"userinit") != NULL){
		return TRUE;
	}
	return FALSE;
}

BOOLEAN CheckIsMsiExec(LPWSTR ExePath){
	if(wcsstr(ExePath, L"msiexec") != NULL) {
		return TRUE;
	}
	return FALSE;
}

void ParseEmulationVersionAndApplyOnPeb(UNICODE_STRING EmulatedVersion, PPEB Peb){
	PWCHAR p;
	ULONG maj;
	ULONG min;
	ULONG bld;	
	ULONG len;	
	
	p = EmulatedVersion.Buffer;
	len = EmulatedVersion.Length / sizeof(WCHAR);

	maj = GetNextPointValue(&p, &len);
	min = GetNextPointValue(&p, &len);
	bld = GetNextPointValue(&p, &len);

	DbgPrint("[EMU] Parsed version: Major=%lu Minor=%lu Build=%lu\n",
			 maj, min, bld);

	Peb->OSMajorVersion = maj;
	Peb->OSMinorVersion = min;
	Peb->OSBuildNumber  = (USHORT)bld;	
}

void CreateCompatVersionAndStore(PPEB Peb, PWCHAR emuPath){
	OCA_COMPATIBILITY_INFO OcaCompatInfo = {0};
	
	OcaCompatInfo.MajorVersion = Peb->OSMajorVersion;
	OcaCompatInfo.MinorVersion = Peb->OSMinorVersion;
	OcaCompatInfo.BuildNumber = Peb->OSBuildNumber;
						
	wcscpy(OcaCompatInfo.emuPath, emuPath);				
						
	StoreInSharedSection(&OcaCompatInfo);		
}

HANDLE OpenProcessNative(ULONG_PTR pid)
{
    HANDLE hProcess = NULL;
    CLIENT_ID cid;
    OBJECT_ATTRIBUTES oa;

    cid.UniqueProcess = (HANDLE)(ULONG_PTR)pid;
    cid.UniqueThread  = NULL;

    InitializeObjectAttributes(
        &oa,
        NULL,
        0,
        NULL,
        NULL
    );

    NtOpenProcess(
        &hProcess,
        PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION,
        &oa,
        &cid
    );

    return hProcess;
}

NTSTATUS KillProcessNative(ULONG_PTR pid)
{
    HANDLE hProcess;
    NTSTATUS status;

    hProcess = OpenProcessNative(pid);
    if (!hProcess)
        return STATUS_INVALID_HANDLE;

    status = NtTerminateProcess(hProcess, STATUS_SUCCESS);

    NtClose(hProcess);
    return status;
}

NTSTATUS
NTAPI
RtlGetVersionCompatHook(
    OUT  PRTL_OSVERSIONINFOW lpVersionInformation
    )
{
	PPEB Peb;
	NT_PRODUCT_TYPE NtProductType;
	UNICODE_STRING EmulatedVersion;
	WCHAR emuPath[MAX_PATH];
	WCHAR msiPath[MAX_PATH];
	OCA_COMPATIBILITY_INFO OcaCompatInfo = {0};
	
	Peb = NtCurrentPeb();

	/* Copia inicialmente o caminho da imagem */
	wcscpy(emuPath, Peb->ProcessParameters->ImagePathName.Buffer);
	DbgPrint("[EMU] ImagePath original: %ws\n", emuPath);

	/* Se não é um executável bloqueado */
	if (!CheckIsUnsafeExe(emuPath))
	{
		//Verifica se a chave global de emulaço de verso esta preenchida
		if(ReadGlobalEmulationVersion(&EmulatedVersion)){
			ParseEmulationVersionAndApplyOnPeb(EmulatedVersion, Peb);	
			goto setVersionParameter;			
		}
		/* Verifica se é o msiexec.exe */
		if (CheckIsMsiExec(emuPath))
		{		
			if (GetMsiPathFromCommandLine(Peb->ProcessParameters->CommandLine.Buffer,
										  msiPath,
										  260))
			{
				wcscpy(emuPath, msiPath);				
				if (ReadEmulatedVersion(&EmulatedVersion, emuPath)){
					ParseEmulationVersionAndApplyOnPeb(EmulatedVersion, Peb);
					CreateCompatVersionAndStore(Peb, emuPath);
					goto setVersionParameter;
				}
				DbgPrint("[EMU] MSI path extracted: %ws\n", msiPath);
			}				
			else
			{				
				if(LoadFromSharedSection(&OcaCompatInfo)){
					Peb->OSMajorVersion = OcaCompatInfo.MajorVersion;
					Peb->OSMinorVersion = OcaCompatInfo.MinorVersion;
					Peb->OSBuildNumber  = (USHORT)OcaCompatInfo.BuildNumber;
					goto setVersionParameter;					
				}	
				goto setVersionParameter;
			}				
		}

		/* Tenta ler a versão emulada com base no caminho (exe ou .msi) */
		DbgPrint("[EMU] Using emulation lookup path: %ws\n", emuPath);

        if (ReadEmulatedVersion(&EmulatedVersion, emuPath))
		{		 
			ParseEmulationVersionAndApplyOnPeb(EmulatedVersion, Peb);
			CreateCompatVersionAndStore(Peb, emuPath);
			goto setVersionParameter;
		} 
		
		if(LoadFromSharedSection(&OcaCompatInfo)){
			Peb->OSMajorVersion = OcaCompatInfo.MajorVersion;
			Peb->OSMinorVersion = OcaCompatInfo.MinorVersion;
			Peb->OSBuildNumber  = (USHORT)OcaCompatInfo.BuildNumber;
			goto setVersionParameter;				
		}
	}
	else
	{
		DbgPrint("[EMU] Skipped: Executable marked as unsafe: %ws\n",
				  Peb->ProcessParameters->ImagePathName.Buffer);
	}
	
setVersionParameter:	
	lpVersionInformation->dwMajorVersion = Peb->OSMajorVersion;
	lpVersionInformation->dwMinorVersion = Peb->OSMinorVersion;
	lpVersionInformation->dwBuildNumber  = Peb->OSBuildNumber;

    lpVersionInformation->dwPlatformId   = Peb->OSPlatformId;
    if (Peb->CSDVersion.Buffer) {
        wcscpy( lpVersionInformation->szCSDVersion, Peb->CSDVersion.Buffer );
    } else {
        lpVersionInformation->szCSDVersion[0] = 0;
    }

    if (lpVersionInformation->dwOSVersionInfoSize == sizeof( OSVERSIONINFOEXW ))
    {
        ((POSVERSIONINFOEXW)lpVersionInformation)->wServicePackMajor = (Peb->OSCSDVersion >> 8) & 0xFF;
        ((POSVERSIONINFOEXW)lpVersionInformation)->wServicePackMinor = Peb->OSCSDVersion & 0xFF;
        ((POSVERSIONINFOEXW)lpVersionInformation)->wSuiteMask = (USHORT)(SharedUserData->SuiteMask&0xffff);
        ((POSVERSIONINFOEXW)lpVersionInformation)->wProductType = 0;
        if (RtlGetNtProductType( &NtProductType )) {
            ((POSVERSIONINFOEXW)lpVersionInformation)->wProductType = (UCHAR)NtProductType;
            if (NtProductType == VER_NT_WORKSTATION) {
               //
               // For workstation product never return VER_SUITE_TERMINAL
               //
                ((POSVERSIONINFOEXW)lpVersionInformation)->wSuiteMask = ((POSVERSIONINFOEXW)lpVersionInformation)->wSuiteMask & 0xffef;
            }

        }
    }

    return STATUS_SUCCESS;
}	

BOOLEAN
RtlpVerCompare(
    LONG Condition,
    LONG Value1,
    LONG Value2,
    BOOLEAN *Equal,
    int   Flags
    )
{
    char    String1[MAX_STRING_LENGTH];
    char    String2[MAX_STRING_LENGTH];
    LONG    Comparison;

    if (Flags & LEXICAL_COMPARISON) {
        sprintf(String1, "%d", Value1); 
        sprintf(String2, "%d", Value2);
        Comparison = strcmp(String2, String1);
        Value1 = 0;
        Value2 = Comparison;
    }
    *Equal = (Value1 == Value2);
    switch (Condition) {
        case VER_EQUAL:
            return (Value2 == Value1);

        case VER_GREATER:
            return (Value2 > Value1);

        case VER_LESS:
            return (Value2 < Value1);

        case VER_GREATER_EQUAL:
            return (Value2 >= Value1);

        case VER_LESS_EQUAL:
            return (Value2 <= Value1);

        default:
            break;
    }

    return FALSE;
}



NTSTATUS
NTAPI
RtlVerifyVersionInfoCompatHook(
    IN PRTL_OSVERSIONINFOEXW VersionInfo,
    IN ULONG TypeMask,
    IN ULONGLONG  ConditionMask
    )
{
    ULONG i;
    OSVERSIONINFOEXW CurrVersion;
    BOOLEAN SuiteFound = FALSE;
    BOOLEAN Equal;
        NTSTATUS Status;
    ULONG   Condition;


    if (TypeMask == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory( &CurrVersion, sizeof(OSVERSIONINFOEXW) );
    CurrVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    Status = RtlGetVersionCompatHook((PRTL_OSVERSIONINFOW)&CurrVersion);
    if (Status != STATUS_SUCCESS)
                    return Status;

    if ((TypeMask & VER_SUITENAME) && (VersionInfo->wSuiteMask != 0)) {
        for (i=0; i<16; i++) {
            if (VersionInfo->wSuiteMask&(1<<i)) {
                switch (RTL_GET_CONDITION(ConditionMask,VER_SUITENAME)) {
                    case VER_AND:
                        if (!(CurrVersion.wSuiteMask&(1<<i))) {
                            return STATUS_REVISION_MISMATCH;
                        }
                        break;

                    case VER_OR:
                        if (CurrVersion.wSuiteMask&(1<<i)) {
                            SuiteFound = TRUE;
                        }
                        break;

                    default:
                        return STATUS_INVALID_PARAMETER;
                }
            }
        }
        if ((RtlpVerGetConditionMask(ConditionMask,VER_SUITENAME) == VER_OR) && (SuiteFound == FALSE)) {
            return STATUS_REVISION_MISMATCH;
        }
    }

    Equal = TRUE;
    Condition = VER_EQUAL;
    if (TypeMask & VER_MAJORVERSION) {
        Condition = RTL_GET_CONDITION( ConditionMask, VER_MAJORVERSION);
        if (RtlpVerCompare(
                Condition,
                VersionInfo->dwMajorVersion,
                CurrVersion.dwMajorVersion,
                &Equal,
                0
                ) == FALSE)
        {
            if (!Equal) {
                return STATUS_REVISION_MISMATCH;
            }
        }
    }

    if (Equal) {
        ASSERT(Condition);
        if (TypeMask & VER_MINORVERSION) {
            if (Condition == VER_EQUAL) {
                Condition = RTL_GET_CONDITION(ConditionMask, VER_MINORVERSION); 
            }
            if (RtlpVerCompare(
                Condition,
                VersionInfo->dwMinorVersion,
                CurrVersion.dwMinorVersion,
                &Equal,
                LEXICAL_COMPARISON
                ) == FALSE)
            {
                if (!Equal) {
                    return STATUS_REVISION_MISMATCH;
                }
            }
        }

        if (Equal) {
            if (TypeMask & VER_SERVICEPACKMAJOR) {
                if (Condition == VER_EQUAL) {
                    Condition = RTL_GET_CONDITION(ConditionMask, VER_SERVICEPACKMAJOR); 
                }
                if (RtlpVerCompare(
                    Condition,
                    VersionInfo->wServicePackMajor,
                    CurrVersion.wServicePackMajor,
                    &Equal,
                    0
                    ) == FALSE)
                {
                    if (!Equal) {
                        return STATUS_REVISION_MISMATCH;
                    }
                }
            }
            if (Equal) {
                if (TypeMask & VER_SERVICEPACKMINOR) {
                    if (Condition == VER_EQUAL) {
                        Condition = RTL_GET_CONDITION(ConditionMask, VER_SERVICEPACKMINOR); 
                    }
                    if (RtlpVerCompare(
                        Condition,
                        (ULONG)VersionInfo->wServicePackMinor,
                        (ULONG)CurrVersion.wServicePackMinor,
                        &Equal,
                        LEXICAL_COMPARISON
                        ) == FALSE)
                    {
                        return STATUS_REVISION_MISMATCH;
                    }
                }
            }
        }
    }

    if ((TypeMask & VER_BUILDNUMBER) &&
        RtlpVerCompare(
            RTL_GET_CONDITION( ConditionMask, VER_BUILDNUMBER),
            VersionInfo->dwBuildNumber,
            CurrVersion.dwBuildNumber,
            &Equal,
            0
            ) == FALSE)
    {
        return STATUS_REVISION_MISMATCH;
    }

    if ((TypeMask & VER_PLATFORMID) &&
        RtlpVerCompare(
            RTL_GET_CONDITION( ConditionMask, VER_PLATFORMID),
            VersionInfo->dwPlatformId,
            CurrVersion.dwPlatformId,
            &Equal,
            0
            ) == FALSE)
    {
        return STATUS_REVISION_MISMATCH;
    }


    if ((TypeMask & VER_PRODUCT_TYPE) &&
        RtlpVerCompare(
            RTL_GET_CONDITION( ConditionMask, VER_PRODUCT_TYPE),
            VersionInfo->wProductType,
            CurrVersion.wProductType,
            &Equal,
            0
            ) == FALSE)
    {
        return STATUS_REVISION_MISMATCH;
    }

    return STATUS_SUCCESS;
}

ULONG
RtlpVerGetConditionMask(
        ULONGLONG       ConditionMask,
        ULONG   TypeMask
        )
{
        ULONG   NumBitsToShift;
        ULONG   Condition = 0;

        if (!TypeMask) {
                return 0;
        }

        for (NumBitsToShift = 0; TypeMask;  NumBitsToShift++) {
                TypeMask >>= 1;
    }

        Condition |=  (ConditionMask) >> ((NumBitsToShift - 1)
                                                                        * VER_NUM_BITS_PER_CONDITION_MASK);
        Condition &= VER_CONDITION_MASK;
        return Condition;
}