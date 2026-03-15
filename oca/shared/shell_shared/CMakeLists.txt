if(MSVC)
    # error C4311: 'type cast': pointer truncation from 'HANDLE' to 'INT'
    replace_compile_flags("/we4311" " ")
endif()

remove_definitions(-D_WIN32_WINNT=0x502)
add_definitions(-D_WIN32_WINNT=0x600)

include_directories(${REACTOS_SOURCE_DIR}/sdk/include)
include_directories(${REACTOS_SOURCE_DIR}/sdk/lib/atl)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/psdk)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/sdk/include/psdk)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/oca/dependencies/propsys)
include_directories(${REACTOS_SOURCE_DIR}/oca/dependencies/shellbase)

list(APPEND SOURCE
	assoc.c
	classes.c
	debughlp.c
	ebrowser.c
	hooks.c
	iconcache.c
    main.c
	misc.c
	path.c
	pidl.c
	shellitem.c
	shelllink.c
	shellole.c
	shellpath.c
	shlfsbind.c
	shlfileop.c
	shlmenu.c
	#shlview.c
	shlview_cmenu.c
	unimplemented.c
	utils.c
	precomp.h
	${CMAKE_CURRENT_BINARY_DIR}/shell32_classes_i.c
	${CMAKE_CURRENT_BINARY_DIR}/shell32_classes_p.h)

generate_idl_iids(shell32_classes.idl)
add_idl_headers(shellsharedidl shell32_classes.idl)


add_rpcproxy_files(
    shell32_classes.idl)
	
add_library(shell_shared ${SOURCE})

add_dependencies(shell_shared psdk shellsharedidl)

#add_dependencies(ole_shared psdk olesharedidl)