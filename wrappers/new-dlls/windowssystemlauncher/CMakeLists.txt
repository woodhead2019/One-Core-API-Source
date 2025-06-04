
add_definitions(
    -D__WINESRC__
    -D_olebase_
    -DCOM_NO_WINDOWS_H
    -DENTRY_PREFIX=olebase_
    -DPROXY_CLSID=CLSID_PSFactoryBuffer
    -DWINE_REGISTER_DLL)
	
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/wine)	
include_directories(${CMAKE_CURRENT_BINARY_DIR})	

spec2def(windowssystemlauncher.dll windowssystemlauncher.spec ADD_IMPORTLIB)

set(baseaddress_windowssystemlauncher     0x61000000)

generate_idl_iids(activation.idl inspectable.idl hstring.idl restrictederrorinfo.idl)
add_idl_headers(windowssystemlauncheridl activation.idl inspectable.idl hstring.idl restrictederrorinfo.idl)
add_rpcproxy_files(
    activation.idl
	inspectable.idl
	hstring.idl
	restrictederrorinfo.idl)

list(APPEND SOURCE
    main.c
	version.rc
	# ${CMAKE_CURRENT_BINARY_DIR}/dcom_i.c
    ${CMAKE_CURRENT_BINARY_DIR}/windowssystemlauncher_stubs.c
	#${CMAKE_CURRENT_BINARY_DIR}/proxy.dlldata.c
    ${CMAKE_CURRENT_BINARY_DIR}/windowssystemlauncher.def)

add_library(windowssystemlauncher SHARED  ${SOURCE})
set_module_type(windowssystemlauncher win32dll ENTRYPOINT 0)
target_link_libraries(windowssystemlauncher wine uuid)
add_importlibs(windowssystemlauncher ole32 rpcrt4 msvcrt advapi32 kernel32 combase shell32 ntdll)
add_dependencies(windowssystemlauncher windowssystemlauncheridl)
add_cd_file(TARGET windowssystemlauncher DESTINATION reactos/system32 FOR all)
