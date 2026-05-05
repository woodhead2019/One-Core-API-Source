
include_directories(
    include
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/idl
    ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/adns/src
    ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/adns/adns_win32)

add_rpc_files(client ${REACTOS_SOURCE_DIR}/sdk/include/reactos/idl/dnsrslvr.idl)

add_definitions(-DADNS_JGAA_WIN32 -D_CRT_NO_POSIX_ERROR_CODES)
spec2def(dnsapibase.dll dnsapibase.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    adns.c
    context.c
    memory.c
    names.c
    query.c
    record.c
    rpc.c
    stubs.c
    precomp.h
    ${CMAKE_CURRENT_BINARY_DIR}/dnsrslvr_c.c)

add_library(dnsapibase MODULE
    ${SOURCE}
    dnsapibase.rc
    ${CMAKE_CURRENT_BINARY_DIR}/dnsapibase_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/dnsapibase.def)

set_module_type(dnsapibase win32dll)
target_link_libraries(dnsapibase adns ${PSEH_LIB})
add_importlibs(dnsapibase advapi32 rpcrt4 user32 ws2_32 iphlpapi msvcrt kernel32 ntdll)
add_pch(dnsapibase precomp.h SOURCE)
add_cd_file(TARGET dnsapibase DESTINATION reactos/system32 FOR all)
