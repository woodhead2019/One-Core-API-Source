
add_definitions(
    -D__WINESRC__
    -D_olebase_
    -DCOM_NO_WINDOWS_H
    -DENTRY_PREFIX=olebase_
    -DPROXY_CLSID=CLSID_PSFactoryBuffer
    -DWINE_REGISTER_DLL)
	
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/wine)

set(baseaddress_ole32_wrapper 0x6B610000)

spec2def(ole32_wrapper.dll ole32_wrapper.spec)

# generate_idl_iids(dcom.idl)
# add_idl_headers(dcom.idl)

# add_rpcproxy_files(
    # dcom.idl)

list(APPEND SOURCE
    hooks.c
	version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/ole32_wrapper_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/ole32_wrapper.def)

add_library(ole32_wrapper SHARED  ${SOURCE})
set_module_type(ole32_wrapper win32dll ENTRYPOINT 0)
target_link_libraries(ole32_wrapper wine ole_shared uuid)
add_importlibs(ole32_wrapper olebase rpcrt4 msvcrt kernel32 user32 ntdll)
add_cd_file(TARGET ole32_wrapper DESTINATION reactos/system32 FOR all)
