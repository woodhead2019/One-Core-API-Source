	
add_definitions(-D__WINESRC__)
remove_definitions(-D_WIN32_WINNT=0x502 -DWINVER=0x502)
add_definitions(-D_WIN32_WINNT=0x600 -DWINVER=0x600)
include_directories(BEFORE ${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/psdk)
include_directories(${REACTOS_SOURCE_DIR}/sdk/lib/3rdparty/freetype/include)
spec2def(dwrite_wrapper.dll dwrite_wrapper.spec)

set(baseaddress_dwrite_wrapper     0x74D70000)

list(APPEND SOURCE
	main.c
	dwrite_private.h
	scripts.h
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/dwrite_wrapper_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/dwrite_wrapper.def)

add_library(dwrite_wrapper SHARED ${SOURCE})
set_module_type(dwrite_wrapper win32dll)
target_link_libraries(dwrite_wrapper wine uuid dwrite_uuids d2d1_uuids dxguid d3d11_uuids d3d10_uuids d3d10_1_uuids dxgi_uuids)
add_importlibs(dwrite_wrapper advapi32 msvcrt kernel32 ntdll user32 gdi32)
add_cd_file(TARGET dwrite_wrapper DESTINATION reactos/system32 FOR all)
