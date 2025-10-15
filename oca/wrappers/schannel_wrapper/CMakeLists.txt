
include_directories(
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/libs/mbedtls)


add_definitions(-D__WINESRC__ -D_WINE)
spec2def(schannel_wrapper.dll schannel_wrapper.spec)

list(APPEND SOURCE
    lsamode.c
    schannel_mbedtls.c
    schannel_main.c
    schannel_wine.c
    secur32_wine.c
    usermode.c
    stubs.c
    precomp.h
    ${CMAKE_CURRENT_BINARY_DIR}/schannel_wrapper_stubs.c)

add_library(schannel_wrapper MODULE
    ${SOURCE}
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/schannel_wrapper.def)

set_module_type(schannel_wrapper win32dll)
target_link_libraries(schannel_wrapper wine)
add_importlibs(schannel_wrapper mbedtls crypt32 secur32 advapi32 msvcrt kernel32 ntdll)
add_pch(schannel_wrapper precomp.h SOURCE)
add_cd_file(TARGET schannel_wrapper DESTINATION reactos/system32 FOR all)
