
project(d3d9ex_wrapper)

include_directories(${REACTOS_SOURCE_DIR}/sdk/include/c++)
include_directories(${REACTOS_SOURCE_DIR}/sdk/include/c++/stlport)

set_cpp(WITH_RUNTIME)
if(NOT MSVC)
    # HACK: this should be enabled globally!
    add_compile_flags_language("-std=c++11" "CXX")
endif()

add_definitions(
    -D_ATL_NO_EXCEPTIONS)

include_directories(
    ${REACTOS_SOURCE_DIR})
spec2def(d3d9ex_wrapper.dll d3d9ex_wrapper.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    d3d9ex_wrapper.cpp)
	

add_library(d3d9ex_wrapper MODULE
    ${SOURCE}
    d3d9ex_wrapper.spec
	d3d9ex_wrapper.rc
    ${CMAKE_CURRENT_BINARY_DIR}/d3d9ex_wrapper.def)

set_module_type(d3d9ex_wrapper win32dll UNICODE)
target_link_libraries(d3d9ex_wrapper dxguid uuid wine)
add_importlibs(d3d9ex_wrapper kernel32 ntdll msvcrt user32)
add_dependencies(d3d9ex_wrapper wineheaders d3d_idl_headers)
add_cd_file(TARGET d3d9ex_wrapper DESTINATION reactos/system32 FOR all)
