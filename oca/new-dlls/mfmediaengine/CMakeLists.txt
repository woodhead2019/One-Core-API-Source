
add_definitions(-D__WINESRC__)
remove_definitions(-D_WIN32_WINNT=0x502)
add_definitions(-D_WIN32_WINNT=0x600)
include_directories(${REACTOS_BINARY_DIR}/sdk/include/psdk)
include_directories(${REACTOS_BINARY_DIR}/sdk/include/reactos/wine)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/reactos/wine)
spec2def(mfmediaengine.dll mfmediaengine.spec)

set(baseaddress_mfmediaengine     0x67470000)

list(APPEND SOURCE
    main.c
    reader.c
    writer.c
    version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/mfmediaengine_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/mfmediaengine.def)
	
add_idl_headers(mfmediaengine_idlheader mediaengine_classes.idl)	

add_library(mfmediaengine SHARED ${SOURCE})
add_dependencies(mfmediaengine mf_idlheader)
set_module_type(mfmediaengine win32dll)
target_link_libraries(mfmediaengine wine uuid mfuuid)
add_importlibs(mfmediaengine mfplat ole32 msvcrt kernelex ntdll)
add_cd_file(TARGET mfmediaengine DESTINATION reactos/system32 FOR all)
