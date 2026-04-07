
if(MSVC)
    # error C4101: unreferenced local variable in Release-Configuration due to debug logging
    replace_compile_flags("/we4101" " ")
endif()

remove_definitions(-D_WIN32_WINNT=0x502)
add_definitions(-D_WIN32_WINNT=0x602)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/host)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/win32k)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/subsys/csr)
include_directories(${REACTOS_SOURCE_DIR}/win32ss/user/winsrv/usersrv)
include_directories(${REACTOS_SOURCE_DIR}/win32ss/include)

list(APPEND SOURCE
	clipboard.c
	cursor.c
	desktop.c
	display.c
	dpi.c
	dwm.c
	exticon.c
	hooks.c
	icon.c
	input.c
	magnification
    main.c
	message.c
	misc.c
	notification.c
	process.c
	shutdown.c
	sysparams.c
	timer.c
	touch.c
	window.c
    main.h)

add_library(user_shared ${SOURCE})
#disable pch to prevent macro redefinition rtl.h WIN32_NO_STATUS
#add_pch(rtl rtl.h SOURCE)
add_dependencies(user_shared psdk)
