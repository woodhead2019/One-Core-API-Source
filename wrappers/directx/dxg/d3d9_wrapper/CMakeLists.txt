PROJECT(d3d9_dxg)

spec2def(d3d9_dxg.dll d3d9_dxg.spec ADD_IMPORTLIB)

set_cpp(WITH_RTTI WITH_RUNTIME WITH_EXCEPTIONS)

if(NOT MSVC)
    # HACK: this should be enabled globally!
    add_compile_flags_language("-std=c++11" "CXX")
endif()

include_directories(
    ${REACTOS_SOURCE_DIR}/sdk/include/reactos/dll
    ${REACTOS_SOURCE_DIR}/sdk/lib/atl 
    ${REACTOS_SOURCE_DIR}/sdk/include/c++/stlport
    includes)

list(APPEND SOURCE
    d3d9.h
    dllmain.cpp)

add_library(d3d9_dxg MODULE
    ${SOURCE}
    d3d9_dxg.rc
    ${CMAKE_CURRENT_BINARY_DIR}/d3d9_dxg.def)

set_module_type(d3d9_dxg win32dll UNICODE)
target_link_libraries(d3d9_dxg uuid wine)
add_importlibs(d3d9_dxg d3d9 user32 gdi32 msvcrt kernel32 ole32 oleaut32 uxtheme ntdll)
add_pch(d3d9_dxg d3d9.h SOURCE)
add_cd_file(TARGET d3d9_dxg DESTINATION reactos/system32 FOR all)
