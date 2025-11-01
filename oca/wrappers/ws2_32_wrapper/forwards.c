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

// SetAddrInfoExA
// SetAddrInfoExW
// WahCloseApcHelper
// WahCloseHandleHelper
// WahCloseNotificationHandleHelper
// WahCloseSocketHandle
// WahCloseThread
// WahCompleteRequest
// WahCreateHandleContextTable
// WahCreateNotificationHandle
// WahCreateSocketHandle
// WahDestroyHandleContextTable
// WahDisableNonIFSHandleSupport
// WahEnableNonIFSHandleSupport
// WahEnumerateHandleContexts
// WahInsertHandleContext
// WahNotifyAllProcesses
// WahOpenApcHelper
// WahOpenCurrentThread
// WahOpenHandleHelper
// WahOpenNotificationHandleHelper
// WahQueueUserApc
// WahReferenceContextByHandle
// WahRemoveHandleContext
// WahWaitForNotification
// WahWriteLSPEvent
// WSAAdvertiseProvider
// WSAConnectByList
// WSAEnumNameSpaceProvidersExA
// WSAEnumNameSpaceProvidersExW
// WSAAdvertiseProvider
// WSAConnectByList
// WSAEnumNameSpaceProvidersExA
// WSAEnumNameSpaceProvidersExW
// WSAProviderCompleteAsyncCall
// WSAUnadvertiseProvider
// WSCGetApplicationCategory
// WSCInstallNameSpaceEx
// WSCSetApplicationCategory

#pragma comment(linker, "/EXPORT:SetAddrInfoExA=ws2_base.SetAddrInfoExA")
#pragma comment(linker, "/EXPORT:SetAddrInfoExW=ws2_base.SetAddrInfoExW")
#pragma comment(linker, "/EXPORT:WahCloseApcHelper=ws2_base.WahCloseApcHelper")
#pragma comment(linker, "/EXPORT:WahCloseHandleHelper=ws2_base.WahCloseHandleHelper")
#pragma comment(linker, "/EXPORT:WahCloseNotificationHandleHelper=ws2_base.WahCloseNotificationHandleHelper")
#pragma comment(linker, "/EXPORT:WahCloseSocketHandle=ws2_base.WahCloseSocketHandle")
#pragma comment(linker, "/EXPORT:WahCloseThread=ws2_base.WahCloseThread")
#pragma comment(linker, "/EXPORT:WahCompleteRequest=ws2_base.WahCompleteRequest")
#pragma comment(linker, "/EXPORT:WahCreateHandleContextTable=ws2_base.WahCreateHandleContextTable")
#pragma comment(linker, "/EXPORT:WahCreateNotificationHandle=ws2_base.WahCreateNotificationHandle")
#pragma comment(linker, "/EXPORT:WahCreateSocketHandle=ws2_base.WahCreateSocketHandle")
#pragma comment(linker, "/EXPORT:WahDestroyHandleContextTable=ws2_base.WahDestroyHandleContextTable")
#pragma comment(linker, "/EXPORT:WahDisableNonIFSHandleSupport=ws2_base.WahDisableNonIFSHandleSupport")
#pragma comment(linker, "/EXPORT:WahEnableNonIFSHandleSupport=ws2_base.WahEnableNonIFSHandleSupport")
#pragma comment(linker, "/EXPORT:WahEnumerateHandleContexts=ws2_base.WahEnumerateHandleContexts")
#pragma comment(linker, "/EXPORT:WahInsertHandleContext=ws2_base.WahInsertHandleContext")
#pragma comment(linker, "/EXPORT:WahNotifyAllProcesses=ws2_base.WahNotifyAllProcesses")
#pragma comment(linker, "/EXPORT:WahOpenApcHelper=ws2_base.WahOpenApcHelper")
#pragma comment(linker, "/EXPORT:WahOpenCurrentThread=ws2_base.WahOpenCurrentThread")
#pragma comment(linker, "/EXPORT:WahOpenHandleHelper=ws2_base.WahOpenHandleHelper")
#pragma comment(linker, "/EXPORT:WahOpenNotificationHandleHelper=ws2_base.WahOpenNotificationHandleHelper")
#pragma comment(linker, "/EXPORT:WahQueueUserApc=ws2_base.WahQueueUserApc")
#pragma comment(linker, "/EXPORT:WahReferenceContextByHandle=ws2_base.WahReferenceContextByHandle")
#pragma comment(linker, "/EXPORT:WahRemoveHandleContext=ws2_base.WahRemoveHandleContext")
#pragma comment(linker, "/EXPORT:WahWaitForNotification=ws2_base.WahWaitForNotification")
#pragma comment(linker, "/EXPORT:WahWriteLSPEvent=ws2_base.WahWriteLSPEvent")
#pragma comment(linker, "/EXPORT:WSAAdvertiseProvider=ws2_base.WSAAdvertiseProvider")
#pragma comment(linker, "/EXPORT:WSAConnectByList=ws2_base.WSAConnectByList")
#pragma comment(linker, "/EXPORT:WSAEnumNameSpaceProvidersExA=ws2_base.WSAEnumNameSpaceProvidersExA")
#pragma comment(linker, "/EXPORT:WSAEnumNameSpaceProvidersExW=ws2_base.WSAEnumNameSpaceProvidersExW")
#pragma comment(linker, "/EXPORT:WSAProviderCompleteAsyncCall=ws2_base.WSAProviderCompleteAsyncCall")
#pragma comment(linker, "/EXPORT:WSAUnadvertiseProvider=ws2_base.WSAUnadvertiseProvider")
#pragma comment(linker, "/EXPORT:WSCGetApplicationCategory=ws2_base.WSCGetApplicationCategory")
#pragma comment(linker, "/EXPORT:WSCInstallNameSpaceEx=ws2_base.WSCInstallNameSpaceEx")
#pragma comment(linker, "/EXPORT:WSCSetApplicationCategory=ws2_base.WSCSetApplicationCategory")