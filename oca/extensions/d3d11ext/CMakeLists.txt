
add_definitions(-D__WINESRC__)
include_directories(${REACTOS_SOURCE_DIR}/include/reactos/wine)
spec2def(d3d11ext.dll d3d11ext.spec)

set(baseaddress_d3d11ext     0x613e0000)

list(APPEND SOURCE
    factory.c
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/d3d11ext_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/d3d11ext.def)

add_library(d3d11ext SHARED ${SOURCE})
set_module_type(d3d11ext win32dll)
target_link_libraries(d3d11ext wine)
add_importlibs(d3d11ext d3d11 msvcrt kernel32 ntdll dxgi)
add_cd_file(TARGET d3d11ext DESTINATION reactos/system32 FOR all)
