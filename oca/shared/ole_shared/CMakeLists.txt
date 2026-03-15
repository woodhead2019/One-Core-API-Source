if(MSVC)
    # error C4101: unreferenced local variable in Release-Configuration due to debug logging
    replace_compile_flags("/we4101" " ")
	
endif()


include_directories(${REACTOS_SOURCE_DIR}/sdk/include/host)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/win32k)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/subsys/csr)
include_directories(${REACTOS_SOURCE_DIR}/win32ss/user/winsrv/usersrv)
include_directories(${REACTOS_SOURCE_DIR}/win32ss/include)

generate_idl_iids(dcom.idl)
add_idl_headers(olesharedidl dcom.idl)

add_rpcproxy_files(
    dcom.idl)

list(APPEND SOURCE
	apartment.c
	main.c
	main.h
	mta.c
	roapi.c)

add_library(ole_shared ${SOURCE})
#disable pch to prevent macro redefinition rtl.h WIN32_NO_STATUS
#add_pch(rtl rtl.h SOURCE)
add_dependencies(ole_shared psdk olesharedidl)
