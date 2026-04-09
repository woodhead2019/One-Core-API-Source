/*++

Copyright (c) 2018 Shorthorn Project.

Module Name:

    iphlpapi.c

Abstract:

    This file contains functions related with iphpalpi for support
    new applications designed for Vista or above.

Revision History:

    18-04-2018

--*/

#include "main.h" 

WINE_DEFAULT_DEBUG_CHANNEL(iphlpapi);

#define TCP_TABLE2 ~0u /* Internal tcp table for GetTcp(6)Table2() */

#define CHARS_IN_GUID 39

#define NdisMediumTunnel       15
#define NdisMediumNative802_11 16

#define IFENT_SOFTWARE_LOOPBACK 24 /* This is an SNMP constant from rfc1213 */

const NPI_MODULEID NPI_MS_IPV4_MODULEID = {0x00};
const NPI_MODULEID NPI_MS_IPV6_MODULEID = {0x01};
const NPI_MODULEID NPI_MS_TCP_MODULEID = {0x03};
const NPI_MODULEID NPI_MS_NDIS_MODULEID = {0x11};
  

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpv)
{
    TRACE("fdwReason %u\n", fdwReason);

    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hInstDLL);
            break;
    }

    return TRUE;
}

static BOOL map_address_6to4( const SOCKADDR_IN6 *addr6, SOCKADDR_IN *addr4 )
{
    ULONG i;

    if (addr6->sin6_family != WS_AF_INET6) return FALSE;

    for (i = 0; i < 5; i++)
        if (addr6->sin6_addr.u.Word[i]) return FALSE;

    if (addr6->sin6_addr.u.Word[5] != 0xffff) return FALSE;

    addr4->sin_family = WS_AF_INET;
    addr4->sin_port   = addr6->sin6_port;
    addr4->sin_addr.S_un.S_addr = addr6->sin6_addr.u.Word[6] << 16 | addr6->sin6_addr.u.Word[7];
    memset( &addr4->sin_zero, 0, sizeof(addr4->sin_zero) );

    return TRUE;
}

static BOOL find_src_address( MIB_IPADDRTABLE *table, const SOCKADDR_IN *dst, SOCKADDR_IN6 *src )
{
    MIB_IPFORWARDROW row;
    DWORD i, j;

    if (GetBestRoute( dst->sin_addr.S_un.S_addr, 0, &row )) return FALSE;

    for (i = 0; i < table->dwNumEntries; i++)
    {
        /* take the first address */
        if (table->table[i].dwIndex == row.dwForwardIfIndex)
        {
            src->sin6_family   = WS_AF_INET6;
            src->sin6_port     = 0;
            src->sin6_flowinfo = 0;
            for (j = 0; j < 5; j++) src->sin6_addr.u.Word[j] = 0;
            src->sin6_addr.u.Word[5] = 0xffff;
            src->sin6_addr.u.Word[6] = table->table[i].dwAddr & 0xffff;
            src->sin6_addr.u.Word[7] = table->table[i].dwAddr >> 16;
            return TRUE;
        }
    }

    return FALSE;
}

ULONG WINAPI ResolveNeighbor(
  _In_     SOCKADDR *NetworkAddress,
  _Out_    PVOID PhysicalAddress,
  _Inout_  PULONG PhysicalAddressLength
)
{
	return ERROR_NOT_SUPPORTED;
}

DWORD WINAPI ParseNetworkString(
  _In_       const WCHAR *NetworkString,
  _In_       DWORD Types,
  _Out_opt_  PNET_ADDRESS_INFO AddressInfo,
  _Out_opt_  USHORT *PortNumber,
  _Out_opt_  BYTE *PrefixLength
)
{
	return ERROR_INVALID_PARAMETER;
}

/******************************************************************
 *    ConvertInterfaceGuidToLuid (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceGuidToLuid(const GUID *guid, NET_LUID *luid)
{
    DWORD ret;
    MIB_IFROW row;

    TRACE("(%s %p)\n", debugstr_guid(guid), luid);

    if (!guid || !luid) return ERROR_INVALID_PARAMETER;

    row.dwIndex = guid->Data1;
    if ((ret = GetIfEntry( &row ))) return ret;

    luid->Info.Reserved     = 0;
    luid->Info.NetLuidIndex = guid->Data1;
    luid->Info.IfType       = row.dwType;
    return NO_ERROR;
}

/******************************************************************
 *    ConvertInterfaceLuidToIndex (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceLuidToIndex(const NET_LUID *luid, NET_IFINDEX *index)
{
    DWORD ret;
    MIB_IFROW row;

    TRACE("(%p %p)\n", luid, index);

    if (!luid || !index) return ERROR_INVALID_PARAMETER;

    row.dwIndex = luid->Info.NetLuidIndex;
    if ((ret = GetIfEntry( &row ))) return ret;

    *index = luid->Info.NetLuidIndex;
    return NO_ERROR;
}

/******************************************************************
 *    CancelMibChangeNotify2 (IPHLPAPI.@)
 */
DWORD 
WINAPI 
CancelMibChangeNotify2(HANDLE _hNotificationHandle)
{
    FIXME("(handle %p): stub\n", _hNotificationHandle);	
        // NotifyIpInterfaceChange返回的句柄始终等于 2
    if (_hNotificationHandle != (HANDLE)2)
    {
            return ERROR_INVALID_PARAMETER;
    }	

    return NO_ERROR;
}

VOID WINAPI FreeMibTable(
  _In_  PVOID Memory
)
{
	TRACE("(%p)\n", Memory);
	HeapFree(GetProcessHeap(), 0, Memory);
}

DWORD WINAPI GetTeredoPort(
  _Out_  USHORT *Port
)
{
	UNIMPLEMENTED;
	return ERROR_NOT_SUPPORTED;
}

DWORD 
WINAPI
NotifyStableUnicastIpAddressTable(
  _In_     ADDRESS_FAMILY  Family,
  _Inout_  PMIB_UNICASTIPADDRESS_TABLE *Table,
  _In_     PSTABLE_UNICAST_IPADDRESS_TABLE_CALLBACK CallerCallback,
  _In_     PVOID CallerContext,
  _Inout_  HANDLE *NotificationHandle
)
{
	UNIMPLEMENTED;
	return ERROR_NOT_SUPPORTED;
}

DWORD 
WINAPI 
ResolveIpNetEntry2(
  _Inout_   PMIB_IPNET_ROW2 Row,
  _In_opt_  const SOCKADDR_INET *SourceAddress
)
{
	UNIMPLEMENTED;
	return ERROR_NOT_SUPPORTED;
}

DWORD 
WINAPI 
GetIpNetEntry2(
  _Inout_  PMIB_IPNET_ROW2 Row
)
{
	UNIMPLEMENTED;
	return ERROR_NOT_SUPPORTED;
}

/******************************************************************
 *    ConvertInterfaceLuidToGuid (IPHLPAPI.@)
 */
DWORD 
WINAPI 
ConvertInterfaceLuidToGuid(
	const NET_LUID *luid, 
	GUID *guid
)
{
    DWORD ret;
    MIB_IFROW row;

    TRACE("(%p %p)\n", luid, guid);

    if (!luid || !guid) return ERROR_INVALID_PARAMETER;

    row.dwIndex = luid->Info.NetLuidIndex;
    if ((ret = GetIfEntry( &row ))) return ret;

    guid->Data1 = luid->Info.NetLuidIndex;
    return NO_ERROR;
}

#define NTDDI_VERSION 0x06000000 
void ConvertIfTypeToNdisTypes(DWORD ifType, NDIS_MEDIUM *mediaType, NDIS_PHYSICAL_MEDIUM *physicalMediumType, NET_IF_CONNECTION_TYPE *connectType) {
	NDIS_MEDIUM mType = -1;
	NDIS_PHYSICAL_MEDIUM pmType = NdisPhysicalMediumUnspecified;
	NET_IF_MEDIA_CONNECT_STATE netconnectType = NET_IF_CONNECTION_DEDICATED;
	
	switch (ifType) {
		case IF_TYPE_OTHER:
			break;
		case IF_TYPE_ETHERNET_CSMACD:
			mType = NdisMedium802_3;
			pmType = NdisPhysicalMedium802_3;
			break;
		case IF_TYPE_ISO88025_TOKENRING:
			mType = NdisMedium802_5;
			pmType = NdisPhysicalMedium802_5;
			break;
		case IF_TYPE_FDDI:
			mType = NdisMediumFddi;
			break;
		case IF_TYPE_ATM:
		case IF_TYPE_ATM_DXI:
			mType = NdisMediumAtm;
			pmType = NdisPhysicalMedium802_3;
			break;
		case IF_TYPE_IEEE80211:
			mType = NdisMediumNative802_11;
			pmType = NdisPhysicalMediumNative802_11;
			netconnectType = NET_IF_CONNECTION_PASSIVE;
			break;
		case IF_TYPE_TUNNEL:
			mType = NdisMediumTunnel;
			break;
		case IF_TYPE_IEEE1394:
			mType = NdisMedium1394;
			pmType = NdisPhysicalMedium1394;
			netconnectType = NET_IF_CONNECTION_PASSIVE;
			break;
		case IF_TYPE_ARCNET:
			mType = NdisMediumArcnetRaw;
		case IF_TYPE_ARCNET_PLUS:
			mType = NdisMediumArcnet878_2;
			break;
		default:
			TRACE("unsupported ifType type %i detected, report so your network adapter is supported properly for this wrapper\n", ifType);
			break;
	}
	
	if (mediaType) *mediaType = mType;
	if (physicalMediumType) *physicalMediumType = pmType;
	if (connectType) *connectType = netconnectType;
}

DWORD ConvertIfRowToIfRow2(MIB_IFROW *row, MIB_IF_ROW2 *row2, BOOL fastConversion) {
	PIP_ADAPTER_ADDRESSES_XP adapterAddrs = NULL;
	PIP_ADAPTER_ADDRESSES_XP adapterAddrCur;
	
	DWORD adapterAddrSize;
	DWORD err = NO_ERROR;
	BOOL accurateConversionSuccess = FALSE;
	
	memset( row2, 0, sizeof(MIB_IF_ROW2) );
	
	// convert stats and the "easy parts"
	row2->InOctets        = row->dwInOctets;
    row2->InUcastPkts     = row->dwInUcastPkts;
    row2->InNUcastPkts    = row->dwInNUcastPkts;
    row2->InDiscards      = row->dwInDiscards;
    row2->InErrors        = row->dwInErrors;
    row2->InUnknownProtos = row->dwInUnknownProtos;
    row2->OutOctets       = row->dwOutOctets;
    row2->OutUcastPkts    = row->dwOutUcastPkts;
    row2->OutNUcastPkts   = row->dwOutNUcastPkts;
    row2->OutDiscards     = row->dwOutDiscards;
    row2->OutErrors       = row->dwOutErrors;
	
	row2->InterfaceIndex = row->dwIndex;
	row2->Type = row->dwType;
	row2->Mtu = row->dwMtu;
	row2->TransmitLinkSpeed = row->dwSpeed;
	row2->ReceiveLinkSpeed = row->dwSpeed;
	row2->AccessType = (row2->Type == MIB_IF_TYPE_LOOPBACK) ? NET_IF_ACCESS_LOOPBACK : NET_IF_ACCESS_BROADCAST;
    row2->InterfaceAndOperStatusFlags.ConnectorPresent = row2->Type != MIB_IF_TYPE_LOOPBACK;
    row2->InterfaceAndOperStatusFlags.HardwareInterface = row2->Type != MIB_IF_TYPE_LOOPBACK;
	row2->AdminStatus = row->dwAdminStatus == TRUE ? NET_IF_ADMIN_STATUS_UP : NET_IF_ADMIN_STATUS_DOWN; // will be filled in later
	
	// interface LUID will map to dwIndex
	row2->InterfaceLuid.Info.Reserved = 0;
	row2->InterfaceLuid.Info.NetLuidIndex = row->dwIndex;
	row2->InterfaceLuid.Info.IfType = row->dwType;
	
	// interface GUID will be equalivent to {index, 0, 0, 0, 0, ...}
	row2->InterfaceGuid.Data1 = row->dwIndex;
	
	// for PhysicalMediumType it is a 'best-guess' based on IfType
	ConvertIfTypeToNdisTypes(row->dwType, &row2->MediaType, &row2->PhysicalMediumType, &row2->ConnectionType);
	
	// IF_MAX_PHYS_ADDRESS_LENGTH is 32, MAXLEN_PHYSADDR is 8, it is safe to do this... but why IPV6??
	row2->PhysicalAddressLength = row->dwPhysAddrLen;
    memmove( &row2->PhysicalAddress, &row->bPhysAddr, row->dwPhysAddrLen );
    memmove( &row2->PermanentPhysicalAddress, &row->bPhysAddr, row->dwPhysAddrLen );
	
	// convert name and description to alias and description
	memmove(&row2->Alias, &row->wszName, sizeof(row->wszName));
	MultiByteToWideChar(CP_ACP, 0, (const char *)&(row->bDescr), row->dwDescrLen, row2->Description, sizeof(row2->Description) / sizeof(WCHAR));
	
	if (!fastConversion) {
		// alias is actually FriendlyName, so we have to do this
		// 0xF = skip all the addresses that we don't need, speeds things up
		if ((err = GetAdaptersAddresses(AF_UNSPEC, 0xF, 0, NULL, &adapterAddrSize)) != ERROR_BUFFER_OVERFLOW) goto finished;
		if (!(adapterAddrs = malloc(adapterAddrSize))) goto finished;
		if ((err = GetAdaptersAddresses(AF_UNSPEC, 0xF, 0, adapterAddrs, &adapterAddrSize))) goto finished;
		
		adapterAddrCur = adapterAddrs;
		while (adapterAddrCur != NULL) {
			if (adapterAddrCur->IfIndex == row->dwIndex) {
				row2->OperStatus = adapterAddrCur->OperStatus;
				row2->DirectionType = (adapterAddrCur->Flags & IP_ADAPTER_RECEIVE_ONLY) ? NET_IF_DIRECTION_RECEIVEONLY : NET_IF_DIRECTION_SENDRECEIVE;
				wcscpy(row2->Alias, adapterAddrCur->FriendlyName);
				accurateConversionSuccess = TRUE;
				goto finished;
			}
			adapterAddrCur = adapterAddrCur->Next;
		}
	finished:
		free(adapterAddrs);
	}

	
	if (!accurateConversionSuccess) {
		// Convert dwOperStatus to IfOperStatus values because we can't get the actual
		// dwOperStatus values through GetAdaptersAddresses
		switch (row->dwOperStatus) {
			case IF_OPER_STATUS_NON_OPERATIONAL:
			case IF_OPER_STATUS_UNREACHABLE:
			case IF_OPER_STATUS_DISCONNECTED:
				row2->OperStatus = IfOperStatusDown;
				break;
			case IF_OPER_STATUS_CONNECTING:
				row2->OperStatus = IfOperStatusTesting;
				break;
			default:
				row2->OperStatus = IfOperStatusUp;
				break;
		}
		row2->DirectionType = NET_IF_DIRECTION_SENDRECEIVE;
	}
	
	return err;
}

DWORD WINAPI GetIfEntry2(MIB_IF_ROW2 *row2)
{
    DWORD ret;
    MIB_IFROW row;
	
	TRACE("GetIfEntry2 called: %p\n", row2);
	
	if (!row2)
		return ERROR_INVALID_PARAMETER;
	
	row.dwIndex = row2->InterfaceLuid.Info.NetLuidIndex ? row2->InterfaceLuid.Info.NetLuidIndex : row2->InterfaceIndex;
	
	if ((ret = GetIfEntry(&row))) return ret;
	
	ret = ConvertIfRowToIfRow2(&row, row2, FALSE);
	if (ret)
		TRACE("GetIfEntry2 accurate conversion failed with error %i\n", ret);
	
    return NO_ERROR;
}

DWORD WINAPI GetIfEntry2Ex(MIB_IF_ENTRY_LEVEL level, PMIB_IF_ROW2 row2) {
	DWORD ret;
	
	TRACE("GetIfEntry2Ex called: %p\n", row2);

	if ((ret = GetIfEntry2(row2)) == NO_ERROR && level == MibIfEntryNormalWithoutStatistics) {
		row2->InOctets        = 0;
		row2->InUcastPkts     = 0;
		row2->InNUcastPkts    = 0;
		row2->InDiscards      = 0;
		row2->InErrors        = 0;
		row2->InUnknownProtos = 0;
		row2->OutOctets       = 0;
		row2->OutUcastPkts    = 0;
		row2->OutNUcastPkts   = 0;
		row2->OutDiscards     = 0;
		row2->OutErrors       = 0;
	}
	return ret;
}

/***********************************************************************
 *		IcmpSendEcho2Ex (IPHLPAPI.@)
 */
DWORD WINAPI IcmpSendEcho2Ex(
    HANDLE                   IcmpHandle,
    HANDLE                   Event,
    PIO_APC_ROUTINE          ApcRoutine,
    PVOID                    ApcContext,
    IPAddr                   SourceAddress,
    IPAddr                   DestinationAddress,
    LPVOID                   RequestData,
    WORD                     RequestSize,
    PIP_OPTION_INFORMATION   RequestOptions,
    LPVOID                   ReplyBuffer,
    DWORD                    ReplySize,
    DWORD                    Timeout
    )
{
    TRACE("(%p, %p, %p, %p, %08x, %08x, %p, %d, %p, %p, %d, %d): stub\n", IcmpHandle,
            Event, ApcRoutine, ApcContext, SourceAddress, DestinationAddress, RequestData,
            RequestSize, RequestOptions, ReplyBuffer, ReplySize, Timeout);

    if (Event)
    {
        FIXME("unsupported for events\n");
        return 0;
    }
    if (ApcRoutine)
    {
        FIXME("unsupported for APCs\n");
        return 0;
    }
    if (SourceAddress)
    {
        FIXME("unsupported for source addresses\n");
        return 0;
    }

    return IcmpSendEcho(IcmpHandle, DestinationAddress, RequestData,
            RequestSize, RequestOptions, ReplyBuffer, ReplySize, Timeout);
}

DWORD get_interface_indices( BOOL skip_loopback, InterfaceIndexTable **table )
{
    if (table) *table = NULL;
    return 0;
}

/******************************************************************
 *    GetIfTable2Ex (IPHLPAPI.@)
 */
DWORD WINAPI GetIfTable2Ex(MIB_IF_TABLE_LEVEL Level, PMIB_IF_TABLE2 *Table) {
	DWORD res;
	DWORD size;
	DWORD newSize;
	PMIB_IFTABLE ifTable = NULL;
	PMIB_IF_TABLE2 ifTable2;
	PIP_ADAPTER_ADDRESSES_XP adapterAddrs = NULL;
	PIP_ADAPTER_ADDRESSES_XP adapterAddrCur;
	
	int i;
	
	if ((res = GetIfTable(NULL, &size, FALSE)) != ERROR_INSUFFICIENT_BUFFER)
		return res;
	
	if ((ifTable = malloc(size)) == NULL) return ERROR_NOT_ENOUGH_MEMORY;
	
	if ((res = GetIfTable(ifTable, &size, FALSE)) != ERROR_SUCCESS) goto cleanup;
	
	// Get the amount of interface entries we want, converting the size of MIB_IF_TABLE to MIB_IFTABLE2.
	newSize = sizeof(MIB_IF_TABLE2) + (sizeof(MIB_IF_ROW2) * (ifTable->dwNumEntries - 1));
	
	if ((ifTable2 = HeapAlloc(GetProcessHeap(), 0, newSize)) == NULL) {
		res = ERROR_NOT_ENOUGH_MEMORY;
		goto cleanup;
	}
	
	ifTable2->NumEntries = ifTable->dwNumEntries;
	
	for (i = 0; i < ifTable->dwNumEntries; i++) {
		ConvertIfRowToIfRow2(&ifTable->table[i], &ifTable2->Table[i], TRUE);
	}
	
	// we have to set alias on all GetIfTable2Ex entries at the same time, because slow conversion is too slow when calling it 100+ times.
	if ((res = GetAdaptersAddresses(AF_UNSPEC, 0xF, 0, NULL, &newSize)) != ERROR_BUFFER_OVERFLOW) goto cleanup_adapters;
	if (!(adapterAddrs = malloc(newSize))) goto cleanup_adapters;
	if ((res = GetAdaptersAddresses(AF_UNSPEC, 0xF, 0, adapterAddrs, &newSize))) goto cleanup_adapters;
		
	adapterAddrCur = adapterAddrs;
	while (adapterAddrCur != NULL) {
		for (i = 0; i < ifTable->dwNumEntries; i++) {
			PMIB_IF_ROW2 ifRow = &ifTable2->Table[i];
			if (ifRow->InterfaceIndex == adapterAddrCur->IfIndex) {
				ifRow->OperStatus = adapterAddrCur->OperStatus;
				ifRow->DirectionType = (adapterAddrCur->Flags & IP_ADAPTER_RECEIVE_ONLY) ? NET_IF_DIRECTION_RECEIVEONLY : NET_IF_DIRECTION_SENDRECEIVE;
				wcscpy(ifRow->Alias, adapterAddrCur->FriendlyName);
				break;
			}
		}
		adapterAddrCur = adapterAddrCur->Next;
	}
	
	free(adapterAddrs);
	goto cleanup;
cleanup_adapters:
	free(ifTable2);
	free(adapterAddrs);
cleanup:		
	free(ifTable);
	return res;
}

/******************************************************************
 *    GetIfTable2 (IPHLPAPI.@)
 */
DWORD WINAPI GetIfTable2( MIB_IF_TABLE2 **table )
{
    TRACE( "table %p\n", table );
    return GetIfTable2Ex(MibIfTableNormal, table);
}

NETIOAPI_API GetIpInterfaceEntry(
  _Inout_ PMIB_IPINTERFACE_ROW Row
)
{
	return ERROR_NOT_FOUND;
}

/******************************************************************
 *    NotifyUnicastIpAddressChange (IPHLPAPI.@)
 */
DWORD WINAPI NotifyUnicastIpAddressChange(ADDRESS_FAMILY family, PUNICAST_IPADDRESS_CHANGE_CALLBACK callback,
                                          PVOID context, BOOLEAN init_notify, PHANDLE handle)
{
    FIXME("(family %d, callback %p, context %p, init_notify %d, handle %p): semi-stub\n",
          family, callback, context, init_notify, handle);
    if (family != AF_INET && family != AF_INET6 && family != AF_UNSPEC) return ERROR_INVALID_PARAMETER;
    if (handle) *handle = NULL;
    if (init_notify)
        callback(context, NULL, MibInitialNotification);

    return ERROR_NOT_SUPPORTED;
}

/******************************************************************
 *    GetIpForwardTable2 (IPHLPAPI.@)
 */
DWORD WINAPI
GetIpForwardTable2(
    ADDRESS_FAMILY Family,
    PMIB_IPFORWARD_TABLE2 *Table
)
{
    DWORD ret, size = 0;
    PMIB_IPFORWARDTABLE v1_table = NULL;
    PMIB_IPFORWARD_TABLE2 out_table;
    ULONG i, count;

    if (!Table)
        return ERROR_INVALID_PARAMETER;

    if (Family != AF_UNSPEC && Family != AF_INET)
        return ERROR_NOT_SUPPORTED;

    *Table = NULL;

    /* Query size */
    ret = GetIpForwardTable(NULL, &size, FALSE);
    if (ret != ERROR_INSUFFICIENT_BUFFER)
        return ret;

    v1_table = HeapAlloc(GetProcessHeap(), 0, size);
    if (!v1_table)
        return ERROR_OUTOFMEMORY;

    ret = GetIpForwardTable(v1_table, &size, FALSE);
    if (ret != NO_ERROR)
    {
        HeapFree(GetProcessHeap(), 0, v1_table);
        return ret;
    }

    count = v1_table->dwNumEntries;

    /* Alocar tabela com espaço variável */
    out_table = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(MIB_IPFORWARD_TABLE2) +
        (count - 1) * sizeof(MIB_IPFORWARD_ROW2));

    if (!out_table)
    {
        HeapFree(GetProcessHeap(), 0, v1_table);
        return ERROR_OUTOFMEMORY;
    }

    out_table->NumEntries = count;

    for (i = 0; i < count; i++)
    {
        const MIB_IPFORWARDROW *r1 = &v1_table->table[i];
        MIB_IPFORWARD_ROW2 *r2 = &out_table->Table[i];

        /* Interface */
        r2->InterfaceIndex = r1->dwForwardIfIndex;
        r2->InterfaceLuid.Value = 0; /* não temos equivalente direto */

        /* DestinationPrefix */
        r2->DestinationPrefix.Prefix.si_family = AF_INET;
        r2->DestinationPrefix.Prefix.Ipv4.sin_addr.s_addr = r1->dwForwardDest;

        /* máscara -> prefix length */
        {
            DWORD mask = ntohl(r1->dwForwardMask);
            ULONG prefix = 0;

            while (mask & 0x80000000)
            {
                prefix++;
                mask <<= 1;
            }

            r2->DestinationPrefix.PrefixLength = (UCHAR)prefix;
        }

        /* NextHop */
        r2->NextHop.si_family = AF_INET;
        r2->NextHop.Ipv4.sin_addr.s_addr = r1->dwForwardNextHop;

        /* Métrica */
        r2->Metric = r1->dwForwardMetric1;

        /* Protocolo */
        r2->Protocol = (NL_ROUTE_PROTOCOL)r1->dwForwardProto;

        /* Idade */
        r2->Age = r1->dwForwardAge;

        /* Defaults razoáveis */
        r2->ValidLifetime = 0xFFFFFFFF;
        r2->PreferredLifetime = 0xFFFFFFFF;

        r2->Loopback = FALSE;
        r2->AutoconfigureAddress = FALSE;
        r2->Publish = FALSE;
        r2->Immortal = FALSE;

        r2->Origin = NlroManual;
    }

    HeapFree(GetProcessHeap(), 0, v1_table);

    *Table = out_table;
    return NO_ERROR;
}

/******************************************************************
 *    NotifyRouteChange2 (IPHLPAPI.@)
 */
DWORD WINAPI
NotifyRouteChange2(
          ADDRESS_FAMILY             AddressFamily,
          PIPFORWARD_CHANGE_CALLBACK Callback,
          PVOID                      CallerContext,
          BOOLEAN                    InitialNotification,
          HANDLE                     *NotificationHandle
)
{
    PMIB_IPFORWARD_TABLE2 table = NULL;
    HANDLE handle_value = NULL;

    TRACE("NotifyRouteChange2(Family %d, Callback %p, Ctx %p, Initial %d, Handle %p)\n",
          AddressFamily, Callback, CallerContext, InitialNotification, NotificationHandle);

    if (!NotificationHandle || !Callback)
        return ERROR_INVALID_PARAMETER;

    if (AddressFamily != AF_UNSPEC && AddressFamily != AF_INET)
        return ERROR_NOT_SUPPORTED;

    /* Allocate a dummy handle to represent the registration. */
    handle_value = HeapAlloc(GetProcessHeap(), 0, sizeof(DWORD));
    if (!handle_value)
        return ERROR_OUTOFMEMORY;

    *NotificationHandle = handle_value;

    if (InitialNotification)
    {
        if (GetIpForwardTable2(AddressFamily, &table) == NO_ERROR)
        {
            Callback(*NotificationHandle, AddressFamily, CallerContext, table);
            /* Free immediately since we don't yet provide FreeMibTable export here. */
            HeapFree(GetProcessHeap(), 0, table);
        }
    }

    return NO_ERROR;
}

/******************************************************************
 *    GetBestRoute2 (IPHLPAPI.@)
 *
 * PARAMS
 *  InterfaceLuid    [In]     Interface LUID (can be NULL)
 *  InterfaceIndex   [In]     Interface index (can be 0)
 *  SourceAddress    [In]     Source address (can be NULL)
 *  DestinationAddress [In]   Destination address
 *  AddressSortOptions [In]   Address sorting options
 *  BestRoute        [Out]    Best route information
 *  BestSourceAddress [Out]   Best source address (can be NULL)
 *
 * RETURNS
 *  DWORD
 */
DWORD WINAPI
GetBestRoute2(
    IN PVOID InterfaceLuid,
    IN ULONG InterfaceIndex,
    IN CONST PVOID SourceAddress,
    IN CONST PVOID DestinationAddress,
    IN ULONG AddressSortOptions,
    OUT PMIB_IPFORWARD_ROW2 BestRoute,
    OUT PVOID BestSourceAddress)
{
    DWORD ret;
    MIB_IPFORWARDROW oldRoute;
    DWORD destAddr, srcAddr = 0;
    PSOCKADDR_IN destSockAddr;
    PSOCKADDR_IN srcSockAddr;

    if (!DestinationAddress || !BestRoute)
        return ERROR_INVALID_PARAMETER;

    destSockAddr = (PSOCKADDR_IN)DestinationAddress;
    srcSockAddr  = (PSOCKADDR_IN)SourceAddress;

    if (destSockAddr->sin_family != AF_INET)
        return ERROR_NOT_SUPPORTED;

    destAddr = destSockAddr->sin_addr.s_addr;

    if (srcSockAddr && srcSockAddr->sin_family == AF_INET)
        srcAddr = srcSockAddr->sin_addr.s_addr;

    ret = GetBestRoute(destAddr, srcAddr, &oldRoute);
    if (ret != ERROR_SUCCESS)
        return ret;

    ZeroMemory(BestRoute, sizeof(MIB_IPFORWARD_ROW2));

    /* Interface */
    BestRoute->InterfaceIndex = oldRoute.dwForwardIfIndex;
    BestRoute->InterfaceLuid.Value = 0; /* não temos equivalente direto */

    /* DestinationPrefix */
    BestRoute->DestinationPrefix.Prefix.si_family = AF_INET;
    BestRoute->DestinationPrefix.Prefix.Ipv4.sin_addr.s_addr =
        oldRoute.dwForwardDest;

    /* máscara -> prefix length */
    {
        DWORD mask = ntohl(oldRoute.dwForwardMask);
        ULONG prefix = 0;

        while (mask & 0x80000000)
        {
            prefix++;
            mask <<= 1;
        }

        BestRoute->DestinationPrefix.PrefixLength = (UCHAR)prefix;
    }

    /* NextHop */
    BestRoute->NextHop.si_family = AF_INET;
    BestRoute->NextHop.Ipv4.sin_addr.s_addr =
        oldRoute.dwForwardNextHop;

    /* Métrica */
    BestRoute->Metric = oldRoute.dwForwardMetric1;

    /* Protocolo */
    BestRoute->Protocol = (NL_ROUTE_PROTOCOL)oldRoute.dwForwardProto;

    /* Idade */
    BestRoute->Age = oldRoute.dwForwardAge;

    /* Valores padrão */
    BestRoute->ValidLifetime     = 0xFFFFFFFF;
    BestRoute->PreferredLifetime = 0xFFFFFFFF;

    BestRoute->Loopback = (oldRoute.dwForwardType == MIB_IPROUTE_TYPE_DIRECT);
    BestRoute->AutoconfigureAddress = FALSE;
    BestRoute->Publish = FALSE;
    BestRoute->Immortal = FALSE;

    BestRoute->Origin = NlroManual;

    /* Melhor endereço de origem */
    if (BestSourceAddress)
    {
        PMIB_IPADDRTABLE addrTable = NULL;
        DWORD size = 0;
        ULONG i;

        PSOCKADDR_IN bestSrc = (PSOCKADDR_IN)BestSourceAddress;

        ZeroMemory(bestSrc, sizeof(SOCKADDR_IN));
        bestSrc->sin_family = AF_INET;

        ret = GetIpAddrTable(NULL, &size, FALSE);
        if (ret == ERROR_INSUFFICIENT_BUFFER)
        {
            addrTable = HeapAlloc(GetProcessHeap(), 0, size);
            if (addrTable)
            {
                ret = GetIpAddrTable(addrTable, &size, FALSE);
                if (ret == ERROR_SUCCESS)
                {
                    for (i = 0; i < addrTable->dwNumEntries; i++)
                    {
                        if (addrTable->table[i].dwIndex ==
                            oldRoute.dwForwardIfIndex)
                        {
                            bestSrc->sin_addr.s_addr =
                                addrTable->table[i].dwAddr;
                            break;
                        }
                    }
                }

                HeapFree(GetProcessHeap(), 0, addrTable);
            }
        }
    }

    return ERROR_SUCCESS;
}

/******************************************************************
 *    NotifyIpInterfaceChange (IPHLPAPI.@)
 */
DWORD WINAPI NotifyIpInterfaceChange(ADDRESS_FAMILY family, PIPINTERFACE_CHANGE_CALLBACK callback,
                                     PVOID context, BOOLEAN init_notify, PHANDLE handle)
{
    FIXME("(family %d, callback %p, context %p, init_notify %d, handle %p): stub\n",
          family, callback, context, init_notify, handle);
    if (handle) *handle = NULL;
    return NO_ERROR;
}

/******************************************************************
 *    CreateSortedAddressPairs (IPHLPAPI.@)
 */
DWORD WINAPI CreateSortedAddressPairs( const PSOCKADDR_IN6 src_list, DWORD src_count,
                                       const PSOCKADDR_IN6 dst_list, DWORD dst_count,
                                       DWORD options, PSOCKADDR_IN6_PAIR *pair_list,
                                       DWORD *pair_count )
{
    DWORD i, size, ret;
    SOCKADDR_IN6_PAIR *pairs;
    SOCKADDR_IN6 *ptr;
    SOCKADDR_IN addr4;
    MIB_IPADDRTABLE table;

    FIXME( "(src_list %p src_count %u dst_list %p dst_count %u options %x pair_list %p pair_count %p): stub\n",
           src_list, src_count, dst_list, dst_count, options, pair_list, pair_count );

    if (src_list || src_count || !dst_list || !pair_list || !pair_count || dst_count > 500)
        return ERROR_INVALID_PARAMETER;

    for (i = 0; i < dst_count; i++)
    {
        if (!map_address_6to4( &dst_list[i], &addr4 ))
        {
            FIXME("only mapped IPv4 addresses are supported\n");
            return ERROR_NOT_SUPPORTED;
        }
    }

    size = dst_count * sizeof(*pairs);
    size += dst_count * sizeof(SOCKADDR_IN6) * 2; /* source address + destination address */
    if (!(pairs = HeapAlloc( GetProcessHeap(), 0, size ))) return ERROR_NOT_ENOUGH_MEMORY;
    ptr = (SOCKADDR_IN6 *)&pairs[dst_count];

    if ((ret = GetIpAddrTable( &table, GetProcessHeap(), 0 )))
    {
        HeapFree( GetProcessHeap(), 0, pairs );
        return ret;
    }

    for (i = 0; i < dst_count; i++)
    {
        pairs[i].SourceAddress = ptr++;
        if (!map_address_6to4( &dst_list[i], &addr4 ) ||
            !find_src_address( &table, &addr4, pairs[i].SourceAddress ))
        {
            //char buf[46];
            memset( pairs[i].SourceAddress, 0, sizeof(*pairs[i].SourceAddress) );
            pairs[i].SourceAddress->sin6_family = WS_AF_INET6;
        }

        pairs[i].DestinationAddress = ptr++;
        memcpy( pairs[i].DestinationAddress, &dst_list[i], sizeof(*pairs[i].DestinationAddress) );
    }
    *pair_list = pairs;
    *pair_count = dst_count;

    HeapFree( GetProcessHeap(), 0, &table );
    return NO_ERROR;
}

/******************************************************************
 *    ConvertLengthToIpv4Mask (IPHLPAPI.@)
 */
DWORD WINAPI ConvertLengthToIpv4Mask(ULONG mask_len, ULONG *mask)
{
    if (mask_len > 32)
    {
        *mask = INADDR_NONE;
        return ERROR_INVALID_PARAMETER;
    }

    if (mask_len == 0)
        *mask = 0;
    else
        *mask = htonl(~0u << (32 - mask_len));

    return NO_ERROR;
}

BOOL isInterface( TDIEntityID *if_maybe ) {
    return
        if_maybe->tei_entity == IF_ENTITY;
}

BOOL isIpEntity( HANDLE tcpFile, TDIEntityID *ent ) {
    return (ent->tei_entity == CL_NL_ENTITY ||
            ent->tei_entity == CO_NL_ENTITY);
}

NTSTATUS tdiGetMibForIfEntity
( HANDLE tcpFile, TDIEntityID *ent, IFEntrySafelySized *entry ) {
    TCP_REQUEST_QUERY_INFORMATION_EX req = TCP_REQUEST_QUERY_INFORMATION_INIT;
    NTSTATUS status = STATUS_SUCCESS;
    DWORD returnSize;

    WARN("TdiGetMibForIfEntity(tcpFile %x,entityId %x)\n",
           (int)tcpFile, (int)ent->tei_instance);

    req.ID.toi_class                = INFO_CLASS_PROTOCOL;
    req.ID.toi_type                 = INFO_TYPE_PROVIDER;
    req.ID.toi_id                   = IF_MIB_STATS_ID;
    req.ID.toi_entity               = *ent;

    status = DeviceIoControl( tcpFile,
                              IOCTL_TCP_QUERY_INFORMATION_EX,
                              &req,
                              sizeof(req),
                              entry,
                              sizeof(*entry),
                              &returnSize,
                              NULL );

    if(!status)
    {
            WARN("IOCTL Failed\n");
            return STATUS_UNSUCCESSFUL;
    }

    TRACE("TdiGetMibForIfEntity() => {\n"
           "  if_index ....................... %x\n"
           "  if_type ........................ %x\n"
           "  if_mtu ......................... %d\n"
           "  if_speed ....................... %x\n"
           "  if_physaddrlen ................. %d\n",
           entry->ent.if_index,
           entry->ent.if_type,
           entry->ent.if_mtu,
           entry->ent.if_speed,
           entry->ent.if_physaddrlen);
    TRACE("  if_physaddr .................... %02x:%02x:%02x:%02x:%02x:%02x\n"
           "  if_descr ....................... %s\n",
           entry->ent.if_physaddr[0] & 0xff,
           entry->ent.if_physaddr[1] & 0xff,
           entry->ent.if_physaddr[2] & 0xff,
           entry->ent.if_physaddr[3] & 0xff,
           entry->ent.if_physaddr[4] & 0xff,
           entry->ent.if_physaddr[5] & 0xff,
           entry->ent.if_descr);
    TRACE("} status %08x\n",status);

    return STATUS_SUCCESS;
}

NTSTATUS getNthIpEntity( HANDLE tcpFile, DWORD index, TDIEntityID *ent ) {
    DWORD numEntities = 0;
    DWORD numRoutes = 0;
    TDIEntityID *entitySet = 0;
    NTSTATUS status = tdiGetEntityIDSet( tcpFile, &entitySet, &numEntities );
    int i;

    if( !NT_SUCCESS(status) )
        return status;

    for( i = 0; i < numEntities; i++ ) {
        if( isIpEntity( tcpFile, &entitySet[i] ) ) {
            TRACE("Entity %d is an IP Entity\n", i);
            if( numRoutes == index ) break;
            else numRoutes++;
        }
    }

    if( numRoutes == index && i < numEntities ) {
        TRACE("Index %d is entity #%d - %04x:%08x\n", index, i,
               entitySet[i].tei_entity, entitySet[i].tei_instance );
        memcpy( ent, &entitySet[i], sizeof(*ent) );
        tdiFreeThingSet( entitySet );
        return STATUS_SUCCESS;
    } else {
        tdiFreeThingSet( entitySet );
        return STATUS_UNSUCCESSFUL;
    }
}

NTSTATUS tdiGetIpAddrsForIpEntity
( HANDLE tcpFile, TDIEntityID *ent, IPAddrEntry **addrs, PDWORD numAddrs ) {
    NTSTATUS status;

    TRACE("TdiGetIpAddrsForIpEntity(tcpFile %x,entityId %x)\n",
           (DWORD)tcpFile, ent->tei_instance);

    status = tdiGetSetOfThings( tcpFile,
                                INFO_CLASS_PROTOCOL,
                                INFO_TYPE_PROVIDER,
                                IP_MIB_ADDRTABLE_ENTRY_ID,
                                CL_NL_ENTITY,
				ent->tei_instance,
                                0,
                                sizeof(IPAddrEntry),
                                (PVOID *)addrs,
                                numAddrs );

    return status;
}

static NTSTATUS getInterfaceInfoSet( HANDLE tcpFile,
                                     IFInfo **infoSet,
                                     PDWORD numInterfaces ) {
    DWORD numEntities;
    TDIEntityID *entIDSet = NULL;
    NTSTATUS status = tdiGetEntityIDSet( tcpFile, &entIDSet, &numEntities );
    IFInfo *infoSetInt = 0;
    int curInterf = 0, i;

    if (!NT_SUCCESS(status)) {
        ERR("getInterfaceInfoSet: tdiGetEntityIDSet() failed: 0x%lx\n", status);
        return status;
    }

    infoSetInt = HeapAlloc( GetProcessHeap(), 0,
                            sizeof(IFInfo) * numEntities );

    if( infoSetInt ) {
        for( i = 0; i < numEntities; i++ ) {
            if( isInterface( &entIDSet[i] ) ) {
                infoSetInt[curInterf].entity_id = entIDSet[i];
                status = tdiGetMibForIfEntity
                    ( tcpFile,
                      &entIDSet[i],
                      &infoSetInt[curInterf].if_info );
                TRACE("tdiGetMibForIfEntity: %08x\n", status);
                if( NT_SUCCESS(status) ) {
                    DWORD numAddrs;
                    IPAddrEntry *addrs;
                    TDIEntityID ip_ent;
                    int j;

                    status = getNthIpEntity( tcpFile, curInterf, &ip_ent );
                    if( NT_SUCCESS(status) )
                        status = tdiGetIpAddrsForIpEntity
                            ( tcpFile, &ip_ent, &addrs, &numAddrs );
                    for( j = 0; NT_SUCCESS(status) && j < numAddrs; j++ ) {
                        TRACE("ADDR %d: index %d (target %d)\n", j, addrs[j].iae_index, infoSetInt[curInterf].if_info.ent.if_index);
                        if( addrs[j].iae_index ==
                            infoSetInt[curInterf].if_info.ent.if_index ) {
                            memcpy( &infoSetInt[curInterf].ip_addr,
                                    &addrs[j],
                                    sizeof( addrs[j] ) );
                            curInterf++;
                            break;
                        }
                    }
                    if ( NT_SUCCESS(status) )
                        tdiFreeThingSet(addrs);
                }
            }
        }

        tdiFreeThingSet(entIDSet);

        if (NT_SUCCESS(status)) {
            *infoSet = infoSetInt;
            *numInterfaces = curInterf;
        } else {
            HeapFree(GetProcessHeap(), 0, infoSetInt);
        }

        return status;
    } else {
        tdiFreeThingSet(entIDSet);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
}

NTSTATUS getInterfaceInfoByName( HANDLE tcpFile, char *name, IFInfo *info ) {
    IFInfo *ifInfo;
    DWORD numInterfaces;
    int i;
    NTSTATUS status = getInterfaceInfoSet( tcpFile, &ifInfo, &numInterfaces );

    if( NT_SUCCESS(status) )
    {
        for( i = 0; i < numInterfaces; i++ ) {
            if( !strcmp((PCHAR)ifInfo[i].if_info.ent.if_descr, name) ) {
                memcpy( info, &ifInfo[i], sizeof(*info) );
                break;
            }
        }

        HeapFree(GetProcessHeap(), 0,ifInfo);

        return i < numInterfaces ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
    }

    return status;
}

DWORD getInterfaceIndexByName(const char *name, PDWORD index)
{
    IFInfo ifInfo;
    HANDLE tcpFile;
    NTSTATUS status = openTcpFile( &tcpFile, FILE_READ_DATA );

    if( NT_SUCCESS(status) ) {
        status = getInterfaceInfoByName( tcpFile, (char *)name, &ifInfo );

        if( NT_SUCCESS(status) ) {
            *index = ifInfo.if_info.ent.if_index;
        }

        closeTcpFile( tcpFile );
    }

    return status;
}

NTSTATUS getInterfaceInfoByIndex( HANDLE tcpFile, DWORD index, IFInfo *info ) {
    IFInfo *ifInfo;
    DWORD numInterfaces;
    NTSTATUS status = getInterfaceInfoSet( tcpFile, &ifInfo, &numInterfaces );
    int i;

    if( NT_SUCCESS(status) )
    {
        for( i = 0; i < numInterfaces; i++ ) {
            if( ifInfo[i].if_info.ent.if_index == index ) {
                memcpy( info, &ifInfo[i], sizeof(*info) );
                break;
            }
        }

        HeapFree(GetProcessHeap(), 0, ifInfo);

        return i < numInterfaces ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
    }

    return status;
}

NETIO_STATUS 
WINAPI 
ConvertInterfaceIndexToLuid(
  _In_  NET_IFINDEX InterfaceIndex,
  _Out_ PNET_LUID   InterfaceLuid
)
{
    MIB_IFROW row;
    NETIO_STATUS Status;
    if (InterfaceLuid == NULL)
        return ERROR_INVALID_PARAMETER;
  
    memset(InterfaceLuid, 0, sizeof(*InterfaceLuid));
    memset(&row, 0, sizeof(MIB_IFROW));

    row.dwIndex = InterfaceIndex;
    Status = GetIfEntry(&row);
    if (Status != NO_ERROR) {
        ERR("ConvertInterfaceIndexToLuid: GetIfEntry(%i) failed with result %i", Status, InterfaceIndex);
        return Status;
    }
    InterfaceLuid->Info.Reserved     = 0;
    InterfaceLuid->Info.NetLuidIndex = InterfaceIndex;
    InterfaceLuid->Info.IfType       = row.dwType;
    return NO_ERROR;
}

/******************************************************************
 *    ConvertInterfaceLuidToNameA (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceLuidToNameA(const NET_LUID *luid, char *name, SIZE_T len)
{
    DWORD err;
    WCHAR nameW[IF_MAX_STRING_SIZE + 1];

    TRACE( "(%p %p %u)\n", luid, name, (DWORD)len );

    if (!luid) return ERROR_INVALID_PARAMETER;
    if (!name || !len) return ERROR_NOT_ENOUGH_MEMORY;

    err = ConvertInterfaceLuidToNameW( luid, nameW, ARRAY_SIZE(nameW) );
    if (err) return err;

    if (!WideCharToMultiByte( CP_UNIXCP, 0, nameW, -1, name, len, NULL, NULL ))
        err = GetLastError();
    return err;
}

/******************************************************************
 *    ConvertInterfaceLuidToNameW (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceLuidToNameW(const NET_LUID *luid, WCHAR *name, SIZE_T len)
{
    DWORD ret;
    MIB_IFROW row;

    TRACE("(%p %p %u)\n", luid, name, (DWORD)len);

    if (!luid || !name) return ERROR_INVALID_PARAMETER;

    row.dwIndex = luid->Info.NetLuidIndex;
    if ((ret = GetIfEntry( &row ))) return ret;

    if (len < strlenW( row.wszName ) + 1) return ERROR_NOT_ENOUGH_MEMORY;
    strcpyW( name, row.wszName );
    return NO_ERROR;
}

/******************************************************************
 *    ConvertInterfaceNameToLuidA (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceNameToLuidA(const char *name, NET_LUID *luid)
{
    WCHAR nameW[IF_MAX_STRING_SIZE];

    TRACE( "(%s %p)\n", debugstr_a(name), luid );

    if (!name) return ERROR_INVALID_NAME;
    if (!MultiByteToWideChar( CP_ACP, 0, name, -1, nameW, ARRAY_SIZE(nameW) ))
        return GetLastError();

    return ConvertInterfaceNameToLuidW( nameW, luid );
}

/******************************************************************
 *    ConvertInterfaceNameToLuidW (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceNameToLuidW(const WCHAR *name, NET_LUID *luid)
{
    DWORD ret;
    IF_INDEX index;
    MIB_IFROW row;
    char nameA[IF_MAX_STRING_SIZE + 1];

    TRACE("(%s %p)\n", debugstr_w(name), luid);

    if (!luid) return ERROR_INVALID_PARAMETER;
    memset( luid, 0, sizeof(*luid) );

    if (!WideCharToMultiByte( CP_ACP, 0, name, -1, nameA, sizeof(nameA), NULL, NULL ))
        return ERROR_INVALID_NAME;

    if ((ret = getInterfaceIndexByName( nameA, &index ))) return ret;

    row.dwIndex = index;
    if ((ret = GetIfEntry( &row ))) return ret;

    luid->Info.Reserved     = 0;
    luid->Info.NetLuidIndex = index;
    luid->Info.IfType       = row.dwType;
    return NO_ERROR;
}

/******************************************************************
 *    if_indextoname (IPHLPAPI.@)
 */
/******************************************************************
 *    if_indextoname (IPHLPAPI.@)
 */
char *WINAPI IPHLP_if_indextoname( NET_IFINDEX index, char *name )
{
    NET_LUID luid;
    DWORD err;

    TRACE( "(%lu, %p)\n", index, name );

    err = ConvertInterfaceIndexToLuid( index, &luid );
    if (err) return NULL;

    err = ConvertInterfaceLuidToNameA( &luid, name, IF_MAX_STRING_SIZE );
    if (err) return NULL;
    return name;
}

/* The comments say MAX_ADAPTER_NAME is required, but really only IF_NAMESIZE
 * bytes are necessary.
 */
char *getInterfaceNameByIndex(IF_INDEX index, char *name)
{
  return IPHLP_if_indextoname(index, name);
}

/******************************************************************
 *    if_nametoindex (IPHLPAPI.@)
 */
IF_INDEX WINAPI IPHLP_if_nametoindex(const char *name)
{
    IF_INDEX index;
    NET_LUID luid;
    DWORD err;

    TRACE( "(%s)\n", name );

    err = ConvertInterfaceNameToLuidA( name, &luid );
    if (err) return 0;

    err = ConvertInterfaceLuidToIndex( &luid, &index );
    if (err) index = 0;
    return index;
}

void getInterfacePhysicalFromInfo( IFInfo *info,
                                   PDWORD len, PBYTE addr, PDWORD type ) {
    *len = info->if_info.ent.if_physaddrlen;
    memcpy( addr, info->if_info.ent.if_physaddr, *len );
    *type = info->if_info.ent.if_type;
}

DWORD getInterfacePhysicalByName(const char *name, PDWORD len, PBYTE addr,
                                 PDWORD type)
{
    HANDLE tcpFile;
    IFInfo info;
    NTSTATUS status = openTcpFile( &tcpFile, FILE_READ_DATA );

    if( NT_SUCCESS(status) ) {
        status = getInterfaceInfoByName( tcpFile, (char *)name, &info );
        if( NT_SUCCESS(status) )
            getInterfacePhysicalFromInfo( &info, len, addr, type );
        closeTcpFile( tcpFile );
    }

    return status;
}

DWORD getAddrByIndexOrName( char *name, DWORD index, IPHLPAddrType addrType ) {
    IFInfo ifInfo;
    HANDLE tcpFile;
    NTSTATUS status = STATUS_SUCCESS;
    DWORD addrOut = INADDR_ANY;

    status = openTcpFile( &tcpFile, FILE_READ_DATA );

    if( NT_SUCCESS(status) ) {
        status = getIPAddrEntryForIf( tcpFile, name, index, &ifInfo );
        if( NT_SUCCESS(status) ) {
            switch( addrType ) {
            case IPAAddr:  addrOut = ifInfo.ip_addr.iae_addr; break;
            case IPABcast: addrOut = ifInfo.ip_addr.iae_bcastaddr; break;
            case IPAMask:  addrOut = ifInfo.ip_addr.iae_mask; break;
            case IFMtu:    addrOut = ifInfo.if_info.ent.if_mtu; break;
            case IFStatus: addrOut = ifInfo.if_info.ent.if_operstatus; break;
            }
        }
        closeTcpFile( tcpFile );
    }

    return addrOut;
}

DWORD getInterfaceMtuByName(const char *name, PDWORD mtu) {
    *mtu = getAddrByIndexOrName( (char *)name, 0, IFMtu );
    return STATUS_SUCCESS;
}

DWORD getInterfaceEntryByName(const char *name, PMIB_IFROW entry)
{
  BYTE addr[MAX_INTERFACE_PHYSADDR];
  DWORD ret, len = sizeof(addr), type;

  if (!name)
    return ERROR_INVALID_PARAMETER;
  if (!entry)
    return ERROR_INVALID_PARAMETER;

  if (getInterfacePhysicalByName(name, &len, addr, &type) == NO_ERROR) {
    WCHAR *assigner;
    const char *walker;

    memset(entry, 0, sizeof(MIB_IFROW));
    for (assigner = entry->wszName, walker = name; *walker; 
     walker++, assigner++)
      *assigner = *walker;
    *assigner = 0;
    getInterfaceIndexByName(name, &entry->dwIndex);
    entry->dwPhysAddrLen = len;
    memcpy(entry->bPhysAddr, addr, len);
    memset(entry->bPhysAddr + len, 0, sizeof(entry->bPhysAddr) - len);
    entry->dwType = type;
    /* FIXME: how to calculate real speed? */
    getInterfaceMtuByName(name, &entry->dwMtu);
    /* lie, there's no "administratively down" here */
    entry->dwAdminStatus = MIB_IF_ADMIN_STATUS_UP;
    getInterfaceStatusByName(name, (PDWORD)&entry->dwOperStatus);
    /* punt on dwLastChange? */
    entry->dwDescrLen = min(strlen(name), MAX_INTERFACE_DESCRIPTION - 1);
    memcpy(entry->bDescr, name, entry->dwDescrLen);
    entry->bDescr[entry->dwDescrLen] = '\0';
    entry->dwDescrLen++;
    ret = NO_ERROR;
  }
  else
    ret = ERROR_INVALID_DATA;
  return ret;
}

DWORD getInterfaceStatsByName(const char *name, PMIB_IFROW entry)
{
  if (!name)
    return ERROR_INVALID_PARAMETER;
  if (!entry)
    return ERROR_INVALID_PARAMETER;

  return NO_ERROR;
}

DWORD getInterfaceStatsByIndex(DWORD index, PMIB_IFROW entry)
{
    return ERROR_INVALID_PARAMETER;
}

NTSTATUS getIPAddrEntryForIf(HANDLE tcpFile,
                             char *name,
                             DWORD index,
                             IFInfo *ifInfo) {
    NTSTATUS status =
        name ?
        getInterfaceInfoByName( tcpFile, name, ifInfo ) :
        getInterfaceInfoByIndex( tcpFile, index, ifInfo );

    if (!NT_SUCCESS(status)) {
        ERR("getIPAddrEntryForIf returning %lx\n", status);
    }

    return status;
}

DWORD getInterfaceStatusByName(const char *name, PDWORD status) {
    *status = getAddrByIndexOrName( (char *)name, 0, IFStatus );
    return STATUS_SUCCESS;
}

BOOL isLoopback( HANDLE tcpFile, TDIEntityID *loop_maybe ) {
    IFEntrySafelySized entryInfo;
    NTSTATUS status;

    status = tdiGetMibForIfEntity( tcpFile,
                                   loop_maybe,
                                   &entryInfo );

    return NT_SUCCESS(status) &&
           (entryInfo.ent.if_type == IFENT_SOFTWARE_LOOPBACK);
}


InterfaceIndexTable *getInterfaceIndexTableInt( BOOL nonLoopbackOnly ) {
  DWORD numInterfaces, curInterface = 0;
  int i;
  IFInfo *ifInfo;
  InterfaceIndexTable *ret = 0;
  HANDLE tcpFile;
  NTSTATUS status = openTcpFile( &tcpFile, FILE_READ_DATA );

  if( NT_SUCCESS(status) ) {
      status = getInterfaceInfoSet( tcpFile, &ifInfo, &numInterfaces );

      TRACE("InterfaceInfoSet: %08x, %04x:%08x\n",
             status,
             ifInfo->entity_id.tei_entity,
             ifInfo->entity_id.tei_instance);

      if( NT_SUCCESS(status) ) {
          ret = (InterfaceIndexTable *)
              calloc(1,
                     sizeof(InterfaceIndexTable) +
                     (numInterfaces - 1) * sizeof(DWORD));

          if (ret) {
              ret->numAllocated = numInterfaces;
              TRACE("NumInterfaces = %d\n", numInterfaces);

              for( i = 0; i < numInterfaces; i++ ) {
                  TRACE("Examining interface %d\n", i);
                  if( !nonLoopbackOnly ||
                      !isLoopback( tcpFile, &ifInfo[i].entity_id ) ) {
                      TRACE("Interface %d matches (%d)\n", i, curInterface);
                      ret->indexes[curInterface++] =
                          ifInfo[i].if_info.ent.if_index;
                  }
              }

              ret->numIndexes = curInterface;
          }

          tdiFreeThingSet( ifInfo );
      }
      closeTcpFile( tcpFile );
  }

  return ret;
}

InterfaceIndexTable *getInterfaceIndexTable(void) {
    return getInterfaceIndexTableInt( FALSE );
}

/******************************************************************
 *    GetIpInterfaceTable (IPHLPAPI.@)
 */
DWORD WINAPI GetIpInterfaceTable(
    ADDRESS_FAMILY Family,
    PMIB_IPINTERFACE_TABLE *Table
)
{
    InterfaceIndexTable *if_table;
    PMIB_IPINTERFACE_TABLE out_table;
    DWORD i, count;

    if (!Table)
        return ERROR_INVALID_PARAMETER;

    if (Family != AF_UNSPEC && Family != AF_INET)
        return ERROR_NOT_SUPPORTED;

    *Table = NULL;

    if_table = getInterfaceIndexTable();
    if (!if_table)
        return ERROR_OUTOFMEMORY;

    count = if_table->numIndexes;

    /* Estrutura variável */
    out_table = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(MIB_IPINTERFACE_TABLE) +
        (count - 1) * sizeof(MIB_IPINTERFACE_ROW));

    if (!out_table)
    {
        free(if_table);
        return ERROR_OUTOFMEMORY;
    }

    out_table->NumEntries = 0;

    for (i = 0; i < count; i++)
    {
        MIB_IFROW ifrow;
        MIB_IPINTERFACE_ROW *row;
        DWORD idx = if_table->indexes[i];
        DWORD ret;

        ZeroMemory(&ifrow, sizeof(ifrow));
        ifrow.dwIndex = idx;

        ret = GetIfEntry(&ifrow);
        if (ret != NO_ERROR)
            continue;

        row = &out_table->Table[out_table->NumEntries];
        ZeroMemory(row, sizeof(*row));

        /* Interface */
        row->InterfaceIndex = idx;
        row->InterfaceLuid.Value = 0; /* fallback */

        row->Family = AF_INET;

        /* Métricas */
        row->Metric = 0;
        row->UseAutomaticMetric = TRUE;

        /* MTU */
        row->NlMtu = ifrow.dwMtu;

        /* Flags padrão */
        row->Connected = (ifrow.dwOperStatus == IF_OPER_STATUS_OPERATIONAL);
        row->SupportsWakeUpPatterns = FALSE;
        row->SupportsNeighborDiscovery = FALSE;
        row->SupportsRouterDiscovery = FALSE;

        row->ReachableTime = 0;
        //row->TransmitOffload = 0;
        //row->ReceiveOffload = 0;

        row->DisableDefaultRoutes = FALSE;

        /* Valores padrão razoáveis */
        row->AdvertisingEnabled = FALSE;
        //row->ForwardingEnabled = (ifrow.dwType == IF_TYPE_ROUTER_WORKSTATION_DIALOUT);

        row->WeakHostSend = FALSE;
        row->WeakHostReceive = FALSE;

        row->UseNeighborUnreachabilityDetection = FALSE;

        row->ManagedAddressConfigurationSupported = FALSE;
        row->OtherStatefulConfigurationSupported = FALSE;

        row->AdvertiseDefaultRoute = FALSE;

        out_table->NumEntries++;
    }

    free(if_table);

    *Table = out_table;
    return NO_ERROR;
}

/******************************************************************
 *    GetIpNetTable2 (IPHLPAPI.@)
 */
DWORD WINAPI GetIpNetTable2(ADDRESS_FAMILY family, PMIB_IPNET_TABLE2 *table)
{
    static int once;

    if (!once++) FIXME("(%u %p): stub\n", family, table);
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************
 *    GetTcp6Table (IPHLPAPI.@)
 */
ULONG WINAPI GetTcp6Table(PMIB_TCP6TABLE table, PULONG size, BOOL order)
{
    TRACE("(table %p, size %p, order %d)\n", table, size, order);
    return GetExtendedTcpTable(table, size, order, WS_AF_INET6, TCP_TABLE_BASIC_ALL, 0);
}

/******************************************************************
 *    GetTcp6Table2 (IPHLPAPI.@)
 */
DWORD WINAPI GetTcp6Table2(MIB_TCP6TABLE2 **Table)
{
    DWORD size = 0;
    DWORD ret;
    PMIB_TCP6TABLE_OWNER_PID oldTable;
    DWORD i;

    ret = GetExtendedTcpTable(NULL,
                              &size,
                              FALSE,
                              AF_INET6,
                              TCP_TABLE_OWNER_PID_ALL,
                              0);

    if (ret != ERROR_INSUFFICIENT_BUFFER)
        return ret;

    oldTable = (PMIB_TCP6TABLE_OWNER_PID)
        HeapAlloc(GetProcessHeap(), 0, size);

    if (!oldTable)
        return ERROR_NOT_ENOUGH_MEMORY;

    ret = GetExtendedTcpTable(oldTable,
                              &size,
                              FALSE,
                              AF_INET6,
                              TCP_TABLE_OWNER_PID_ALL,
                              0);

    if (ret != NO_ERROR)
    {
        HeapFree(GetProcessHeap(), 0, oldTable);
        return ret;
    }

    {
        DWORD count = oldTable->dwNumEntries;
        SIZE_T newSize =
            sizeof(MIB_TCP6TABLE2) +
            (count - 1) * sizeof(MIB_TCP6ROW2);

        MIB_TCP6TABLE2 *newTable =
            (MIB_TCP6TABLE2*)HeapAlloc(GetProcessHeap(),
                                       HEAP_ZERO_MEMORY,
                                       newSize);

        if (!newTable)
        {
            HeapFree(GetProcessHeap(), 0, oldTable);
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        newTable->dwNumEntries = count;

        for (i = 0; i < count; i++)
        {
            PMIB_TCP6ROW_OWNER_PID src = &oldTable->table[i];
            MIB_TCP6ROW2 *dst = &newTable->table[i];

            memcpy(&dst->LocalAddr, src->ucLocalAddr, 16);
            dst->dwLocalScopeId = src->dwLocalScopeId;
            dst->dwLocalPort = src->dwLocalPort;

            memcpy(&dst->RemoteAddr, src->ucRemoteAddr, 16);
            dst->dwRemoteScopeId = src->dwRemoteScopeId;
            dst->dwRemotePort = src->dwRemotePort;

            dst->State = src->dwState;
            dst->dwOwningPid = src->dwOwningPid;
        }

        *Table = newTable;
    }

    HeapFree(GetProcessHeap(), 0, oldTable);

    return NO_ERROR;
}

/******************************************************************
 *    GetTcpTable2 (IPHLPAPI.@)
 */
DWORD WINAPI GetTcpTable2(MIB_TCPTABLE2 **Table)
{
    DWORD size = 0;
    DWORD ret;
    PMIB_TCPTABLE_OWNER_PID oldTable;
    DWORD i;

    ret = GetExtendedTcpTable(NULL,
                              &size,
                              FALSE,
                              AF_INET,
                              TCP_TABLE_OWNER_PID_ALL,
                              0);

    if (ret != ERROR_INSUFFICIENT_BUFFER)
        return ret;

    oldTable = (PMIB_TCPTABLE_OWNER_PID)
        HeapAlloc(GetProcessHeap(), 0, size);

    if (!oldTable)
        return ERROR_NOT_ENOUGH_MEMORY;

    ret = GetExtendedTcpTable(oldTable,
                              &size,
                              FALSE,
                              AF_INET,
                              TCP_TABLE_OWNER_PID_ALL,
                              0);

    if (ret != NO_ERROR)
    {
        HeapFree(GetProcessHeap(), 0, oldTable);
        return ret;
    }

    {
        DWORD count = oldTable->dwNumEntries;
        SIZE_T newSize =
            sizeof(MIB_TCPTABLE2) +
            (count - 1) * sizeof(MIB_TCPROW2);

        MIB_TCPTABLE2 *newTable =
            (MIB_TCPTABLE2*)HeapAlloc(GetProcessHeap(),
                                      HEAP_ZERO_MEMORY,
                                      newSize);

        if (!newTable)
        {
            HeapFree(GetProcessHeap(), 0, oldTable);
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        newTable->dwNumEntries = count;

        for (i = 0; i < count; i++)
        {
            PMIB_TCPROW_OWNER_PID src = &oldTable->table[i];
            MIB_TCPROW2 *dst = &newTable->table[i];

            dst->dwState = src->dwState;
            dst->dwLocalAddr = src->dwLocalAddr;
            dst->dwLocalPort = src->dwLocalPort;
            dst->dwRemoteAddr = src->dwRemoteAddr;
            dst->dwRemotePort = src->dwRemotePort;
            dst->dwOwningPid = src->dwOwningPid;

            /* campo inexistente no XP */
            dst->dwOffloadState = 0;
        }

        *Table = newTable;
    }

    HeapFree(GetProcessHeap(), 0, oldTable);

    return NO_ERROR;
}

/******************************************************************
 *    GetUdp6Table (IPHLPAPI.@)
 */
DWORD WINAPI GetUdp6Table(PMIB_UDP6TABLE pUdpTable, PDWORD pdwSize, BOOL bOrder)
{
    return GetExtendedUdpTable(pUdpTable, pdwSize, bOrder, WS_AF_INET6, UDP_TABLE_BASIC, 0);
}

DWORD WINAPI GetUnicastIpAddressEntry(MIB_UNICASTIPADDRESS_ROW *row)
{
    IP_ADAPTER_ADDRESSES_LH *aa, *ptr;
    ULONG size = 0;
    DWORD ret;

    TRACE("%p\n", row);

    if (!row)
        return ERROR_INVALID_PARAMETER;

    ret = GetAdaptersAddresses(row->Address.si_family, 0, NULL, NULL, &size);
    if (ret != ERROR_BUFFER_OVERFLOW)
        return ret;
    if (!(ptr = HeapAlloc(GetProcessHeap(), 0, size)))
        return ERROR_OUTOFMEMORY;
    if ((ret = GetAdaptersAddresses(row->Address.si_family, 0, NULL, (PIP_ADAPTER_ADDRESSES)ptr, &size)))
    {
        HeapFree(GetProcessHeap(), 0, ptr);
        return ret;
    }

    ret = ERROR_FILE_NOT_FOUND;
    for (aa = ptr; aa; aa = aa->Next)
    {
        IP_ADAPTER_UNICAST_ADDRESS *ua;

        if (aa->IfIndex != row->InterfaceIndex &&
            memcmp(&aa->Luid, &row->InterfaceLuid, sizeof(row->InterfaceLuid)))
            continue;
        ret = ERROR_NOT_FOUND;

        ua = aa->FirstUnicastAddress;
        while (ua)
        {
            SOCKADDR_INET *uaaddr = (SOCKADDR_INET *)ua->Address.lpSockaddr;

            if ((row->Address.si_family == WS_AF_INET6 &&
                 !memcmp(&row->Address.Ipv6.sin6_addr, &uaaddr->Ipv6.sin6_addr, sizeof(uaaddr->Ipv6.sin6_addr))) ||
                (row->Address.si_family == WS_AF_INET &&
                 row->Address.Ipv4.sin_addr.S_un.S_addr == uaaddr->Ipv4.sin_addr.S_un.S_addr))
            {
                memcpy(&row->InterfaceLuid, &aa->Luid, sizeof(aa->Luid));
                row->InterfaceIndex     = aa->IfIndex;
                row->PrefixOrigin       = ua->PrefixOrigin;
                row->SuffixOrigin       = ua->SuffixOrigin;
                row->ValidLifetime      = ua->ValidLifetime;
                row->PreferredLifetime  = ua->PreferredLifetime;
                row->OnLinkPrefixLength = ua->OnLinkPrefixLength;
                row->SkipAsSource       = 0;
                row->DadState           = ua->DadState;
                if (row->Address.si_family == WS_AF_INET6)
                    row->ScopeId.Value  = row->Address.Ipv6.sin6_scope_id;
                else
                    row->ScopeId.Value  = 0;
                NtQuerySystemTime(&row->CreationTimeStamp);
                HeapFree(GetProcessHeap(), 0, ptr);
                return NO_ERROR;
            }
            ua = ua->Next;
        }
    }
    HeapFree(GetProcessHeap(), 0, ptr);

    return ret;
}

DWORD WINAPI GetUnicastIpAddressTable(ADDRESS_FAMILY family, MIB_UNICASTIPADDRESS_TABLE **table)
{
    IP_ADAPTER_ADDRESSES_LH *aa, *ptr;
    MIB_UNICASTIPADDRESS_TABLE *data;
    DWORD ret, count = 0;
    ULONG size, flags;

    TRACE("%u, %p\n", family, table);

    if (!table || (family != WS_AF_INET && family != WS_AF_INET6 && family != WS_AF_UNSPEC))
        return ERROR_INVALID_PARAMETER;

    flags = GAA_FLAG_SKIP_ANYCAST |
            GAA_FLAG_SKIP_MULTICAST |
            GAA_FLAG_SKIP_DNS_SERVER |
            GAA_FLAG_SKIP_FRIENDLY_NAME;

    ret = GetAdaptersAddresses(family, flags, NULL, NULL, &size);
    if (ret != ERROR_BUFFER_OVERFLOW)
        return ret;
    if (!(ptr = HeapAlloc(GetProcessHeap(), 0, size)))
        return ERROR_OUTOFMEMORY;
    if ((ret = GetAdaptersAddresses(family, flags, NULL, (IP_ADAPTER_ADDRESSES*)ptr, &size)))
    {
        HeapFree(GetProcessHeap(), 0, ptr);
        return ret;
    }

    for (aa = ptr; aa; aa = aa->Next)
    {
        IP_ADAPTER_UNICAST_ADDRESS *ua = aa->FirstUnicastAddress;
        while (ua)
        {
            count++;
            ua = ua->Next;
        }
    }

    if (!(data = HeapAlloc(GetProcessHeap(), 0, sizeof(*data) + (count - 1) * sizeof(data->Table[0]))))
    {
        HeapFree(GetProcessHeap(), 0, ptr);
        return ERROR_OUTOFMEMORY;
    }

    data->NumEntries = 0;
    for (aa = ptr; aa; aa = aa->Next)
    {
        IP_ADAPTER_UNICAST_ADDRESS *ua = aa->FirstUnicastAddress;
        while (ua)
        {
            MIB_UNICASTIPADDRESS_ROW *row = &data->Table[data->NumEntries];
            memcpy(&row->Address, ua->Address.lpSockaddr, ua->Address.iSockaddrLength);
            memcpy(&row->InterfaceLuid, &aa->Luid, sizeof(aa->Luid));
            row->InterfaceIndex     = aa->IfIndex;
            row->PrefixOrigin       = ua->PrefixOrigin;
            row->SuffixOrigin       = ua->SuffixOrigin;
            row->ValidLifetime      = ua->ValidLifetime;
            row->PreferredLifetime  = ua->PreferredLifetime;
            row->OnLinkPrefixLength = ua->OnLinkPrefixLength;
            row->SkipAsSource       = 0;
            row->DadState           = ua->DadState;
            if (row->Address.si_family == WS_AF_INET6)
                row->ScopeId.Value  = row->Address.Ipv6.sin6_scope_id;
            else
                row->ScopeId.Value  = 0;
            NtQuerySystemTime(&row->CreationTimeStamp);

            data->NumEntries++;
            ua = ua->Next;
        }
    }

    HeapFree(GetProcessHeap(), 0, ptr);

    *table = data;
    return ret;
}

/******************************************************************
 *    SetPerTcpConnectionEStats (IPHLPAPI.@)
 */
DWORD WINAPI SetPerTcpConnectionEStats(PMIB_TCPROW row, TCP_ESTATS_TYPE state, PBYTE rw,
                                       ULONG version, ULONG size, ULONG offset)
{
  FIXME("(row %p, state %d, rw %p, version %u, size %u, offset %u): stub\n",
        row, state, rw, version, size, offset);
  return ERROR_NOT_SUPPORTED;
}

DWORD WINAPI ConvertStringToGuidW( const WCHAR *str, GUID *guid )
{
    UNICODE_STRING ustr;

    RtlInitUnicodeString( &ustr, str );
    return RtlNtStatusToDosError( RtlGUIDFromString( &ustr, guid ) );
}

DWORD WINAPI ConvertGuidToStringA( const GUID *guid, char *str, DWORD len )
{
    if (len < CHARS_IN_GUID) return ERROR_INSUFFICIENT_BUFFER;
    sprintf( str, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
             guid->Data1, guid->Data2, guid->Data3, guid->Data4[0], guid->Data4[1], guid->Data4[2],
             guid->Data4[3], guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7] );
    return ERROR_SUCCESS;
}

DWORD WINAPI ConvertGuidToStringW( const GUID *guid, WCHAR *str, DWORD len )
{
    static const WCHAR fmt[] = { '{','%','0','8','X','-','%','0','4','X','-','%','0','4','X','-',
                                 '%','0','2','X','%','0','2','X','-','%','0','2','X','%','0','2','X',
                                 '%','0','2','X','%','0','2','X','%','0','2','X','%','0','2','X','}',0 };

    if (len < CHARS_IN_GUID) return ERROR_INSUFFICIENT_BUFFER;
    sprintfW( str, fmt,
              guid->Data1, guid->Data2, guid->Data3, guid->Data4[0], guid->Data4[1], guid->Data4[2],
              guid->Data4[3], guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7] );
    return ERROR_SUCCESS;
}

/******************************************************************
 *    ConvertInterfaceAliasToLuid (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceAliasToLuid(
    LPCWSTR Alias,
    NET_LUID *Luid)
{
    ULONG size;
    DWORD ret;
    PIP_ADAPTER_ADDRESSES aa;
    PIP_ADAPTER_ADDRESSES cur;

    size = 0;

    ret = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size);
    if (ret != ERROR_BUFFER_OVERFLOW)
        return ret;

    aa = (PIP_ADAPTER_ADDRESSES)HeapAlloc(GetProcessHeap(), 0, size);
    if (!aa)
        return ERROR_NOT_ENOUGH_MEMORY;

    ret = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, aa, &size);
    if (ret != NO_ERROR)
    {
        HeapFree(GetProcessHeap(), 0, aa);
        return ret;
    }

    cur = aa;

    while (cur)
    {
        if (cur->FriendlyName && lstrcmpiW(cur->FriendlyName, Alias) == 0)
        {
            ULONG index = cur->IfIndex;
            ULONG type  = cur->IfType;

            Luid->Value = ((ULONGLONG)type << 24) | index;

            HeapFree(GetProcessHeap(), 0, aa);
            return NO_ERROR;
        }

        cur = cur->Next;
    }

    HeapFree(GetProcessHeap(), 0, aa);

    return ERROR_NOT_FOUND;
}

/******************************************************************
 *    ConvertInterfaceLuidToAlias (IPHLPAPI.@)
 */
DWORD WINAPI ConvertInterfaceLuidToAlias(
    const NET_LUID *InterfaceLuid,
    LPWSTR InterfaceAlias,
    SIZE_T Length)
{
    DWORD ifIndex;
    ULONG size = 0;
    DWORD ret;
    PIP_ADAPTER_ADDRESSES aa;
    PIP_ADAPTER_ADDRESSES cur;

    if (!InterfaceLuid || !InterfaceAlias)
        return ERROR_INVALID_PARAMETER;

    ifIndex = (DWORD)(InterfaceLuid->Value & 0xFFFFFFFF);

    ret = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size);

    if (ret != ERROR_BUFFER_OVERFLOW)
        return ret;

    aa = (PIP_ADAPTER_ADDRESSES)
        HeapAlloc(GetProcessHeap(), 0, size);

    if (!aa)
        return ERROR_NOT_ENOUGH_MEMORY;

    ret = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, aa, &size);

    if (ret != NO_ERROR)
    {
        HeapFree(GetProcessHeap(), 0, aa);
        return ret;
    }

    cur = aa;

    while (cur)
    {
        if (cur->IfIndex == ifIndex)
        {
            if (!cur->FriendlyName)
            {
                HeapFree(GetProcessHeap(), 0, aa);
                return ERROR_NOT_FOUND;
            }

            if (lstrlenW(cur->FriendlyName) + 1 > Length)
            {
                HeapFree(GetProcessHeap(), 0, aa);
                return ERROR_INSUFFICIENT_BUFFER;
            }

            lstrcpyW(InterfaceAlias, cur->FriendlyName);

            HeapFree(GetProcessHeap(), 0, aa);
            return NO_ERROR;
        }

        cur = cur->Next;
    }

    HeapFree(GetProcessHeap(), 0, aa);

    return ERROR_NOT_FOUND;
}

DWORD WINAPI GetAnycastIpAddressTable(ADDRESS_FAMILY family, MIB_ANYCASTIPADDRESS_TABLE **table)
{
    FIXME( "(%u, %p) stub\n", family, table );
    if (!table || (family != AF_INET && family != AF_INET6 && family != AF_UNSPEC))
        return ERROR_INVALID_PARAMETER;

    *table = heap_alloc_zero(sizeof(MIB_ANYCASTIPADDRESS_TABLE));
    if (!*table) return ERROR_NOT_ENOUGH_MEMORY;
    (*table)->NumEntries = 0;
    return NO_ERROR;
}

/***********************************************************************
 *    GetCurrentThreadCompartmentId (IPHLPAPI.@)
 */
NET_IF_COMPARTMENT_ID WINAPI GetCurrentThreadCompartmentId( void )
{
    FIXME( "stub\n" );
    return NET_IF_COMPARTMENT_ID_PRIMARY;
}

/***********************************************************************
 *    SetCurrentThreadCompartmentId (IPHLPAPI.@)
 */
DWORD WINAPI SetCurrentThreadCompartmentId( NET_IF_COMPARTMENT_ID id )
{
    FIXME( "(%x): stub\n", id );
    return ERROR_SUCCESS;
}