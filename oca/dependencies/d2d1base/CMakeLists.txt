
add_definitions(
    -D__WINESRC__)

include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/psdk)

spec2def(d2d1base.dll d2d1base.spec ADD_IMPORTLIB)

list(APPEND SOURCE)

add_library(d2d1base SHARED
    ${SOURCE}
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/d2d1base_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/d2d1base.def)

set_module_type(d2d1base win32dll)
target_link_libraries(d2d1base wine uuid)
add_importlibs(d2d1base msvcrt gdi32 user32 kernel32 ntdll advapi32)
add_cd_file(TARGET d2d1base DESTINATION reactos/system32 FOR all)
