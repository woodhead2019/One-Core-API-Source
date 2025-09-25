
add_definitions(-D__WINESRC__)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/host)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/win32k)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/subsys/csr)
include_directories(${REACTOS_SOURCE_DIR}/win32ss/user/winsrv/usersrv)
include_directories(${REACTOS_SOURCE_DIR}/win32ss/include)

list(APPEND SOURCE
	dib.c
	display.c
	dpi.c
	dwm.c
	hooks.c
	icon.c
	input.c
	magnification
    main.c
	misc.c
	paint.c
	process.c
	shutdown.c
	syscall.c
	sysparams.c
	timer.c
	touch.c
	window.c
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/userex_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/userex.def)

add_library(userex SHARED ${SOURCE})

target_link_libraries(userex wine)
