
add_definitions(-D__WINESRC__)
remove_definitions(-D_WIN32_WINNT=0x502)
add_definitions(-D_WIN32_WINNT=0x602)	


spec2def(userex.dll userex.spec)

if(ARCH STREQUAL "amd64")
	set(baseaddress_userex     0x67C30000)
elseif(ARCH STREQUAL "i386")
	set(baseaddress_userex     0x67a20000)
endif()

list(APPEND SOURCE
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/userex_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/userex.def)

add_library(userex SHARED ${SOURCE})
set_module_type(userex win32dll ENTRYPOINT 0)

target_link_libraries(userex wine user_shared)

add_importlibs(userex user32 kernel32 gdi32 ntdll advapi32 winsrv)

add_cd_file(TARGET userex DESTINATION reactos/system32 FOR all)
