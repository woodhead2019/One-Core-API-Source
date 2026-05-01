
add_definitions(
    -D__WINESRC__)

include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/psdk)

spec2def(d2d1_wrapper.dll d2d1_wrapper.spec ADD_IMPORTLIB)

list(APPEND SOURCE
	factory.c)

add_library(d2d1_wrapper SHARED
    ${SOURCE}
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/d2d1_wrapper_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/d2d1_wrapper.def)

set_module_type(d2d1_wrapper win32dll ENTRYPOINT DllMain)
target_link_libraries(d2d1_wrapper wine uuid)
add_importlibs(d2d1_wrapper msvcrt d2d1base gdi32 user32 kernel32 ntdll advapi32)
add_cd_file(TARGET d2d1_wrapper DESTINATION reactos/system32 FOR all)
