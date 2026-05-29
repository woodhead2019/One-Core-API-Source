
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
add_definitions(-D__WINESRC__)
spec2def(sxs_wrapper.dll sxs_wrapper.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    cache.c
    name.c
    sxs.c
    precomp.h
    ${CMAKE_CURRENT_BINARY_DIR}/sxs_wrapper_stubs.c)

add_library(sxs_wrapper MODULE
    ${SOURCE}
    guid.c
    ${CMAKE_CURRENT_BINARY_DIR}/sxs_wrapper.def)

set_module_type(sxs_wrapper win32dll)
target_link_libraries(sxs_wrapper wine)
add_importlibs(sxs_wrapper oleaut32 ole32 msvcrt kernel32 ntdll)
add_pch(sxs_wrapper precomp.h SOURCE)
add_cd_file(TARGET sxs_wrapper DESTINATION reactos/system32 FOR all)
