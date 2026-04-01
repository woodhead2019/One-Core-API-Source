
include_directories(
    include
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/idl
    ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/adns/src
    ${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/adns/adns_win32)

add_definitions(-DADNS_JGAA_WIN32 -D_CRT_NO_POSIX_ERROR_CODES)
spec2def(dnsapi_wrapper.dll dnsapi_wrapper.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    main.c
    precomp.h)

add_library(dnsapi_wrapper MODULE
    ${SOURCE}
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/dnsapi_wrapper_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/dnsapi_wrapper.def)

set_module_type(dnsapi_wrapper win32dll)
target_link_libraries(dnsapi_wrapper adns ${PSEH_LIB})
add_importlibs(dnsapi_wrapper dnsapibase)
add_pch(dnsapi_wrapper precomp.h SOURCE)
add_cd_file(TARGET dnsapi_wrapper DESTINATION reactos/system32 FOR all)
