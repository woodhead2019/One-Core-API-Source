
project(appcompat)

set_cpp(WITH_RUNTIME)
if(NOT MSVC)
    # HACK: this should be enabled globally!
    add_compile_flags_language("-std=c++11" "CXX")
endif()

add_definitions(
    -D_ATL_NO_EXCEPTIONS
	-DISOLATION_AWARE_ENABLED)

include_directories(
    ${REACTOS_SOURCE_DIR}/sdk/lib/atl
    ${REACTOS_SOURCE_DIR}/dll/appcompat/apphelp
    ${REACTOS_SOURCE_DIR})
spec2def(slayeroca.dll slayeroca.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    SlayerOCA.cpp
    CEditCompatModes.cpp
    CLayerUIPropPage.cpp
    precomp.h)

add_library(slayeroca MODULE
    ${SOURCE}
    slayeroca.spec
    SlayerOCA.rc
    CLayerUIPropPage.hpp
    CLayerStringList.hpp
    resource.h
    ${CMAKE_CURRENT_BINARY_DIR}/slayeroca.def)

set_module_type(slayeroca win32dll UNICODE)
target_link_libraries(slayeroca wine uuid)
add_delay_importlibs(slayeroca apphelp ole32 oleaut32 shlwapi comctl32)
add_importlibs(slayeroca shell32 user32 advapi32 msvcrt kernel32 uxtheme)
add_pch(slayeroca precomp.h SOURCE)
add_cd_file(TARGET slayeroca DESTINATION reactos/system32 FOR all)
