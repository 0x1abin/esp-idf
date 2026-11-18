idf_build_get_property(target IDF_TARGET)
if(NOT target STREQUAL "sf32lb57")
    idf_component_register()
    return()
endif()

idf_component_register(SRCS "startup.c" INCLUDE_DIRS "include")

set(SF32LB57_SDK_PATH "$ENV{SIFLI_SDK}" CACHE PATH "SiFli SDK v2.5.2 checkout")
if(NOT EXISTS "${SF32LB57_SDK_PATH}/drivers/cmsis/sf32lb57x")
    message(FATAL_ERROR "Set SIFLI_SDK or SF32LB57_SDK_PATH to a SiFli SDK v2.5.2 checkout.")
endif()
file(STRINGS "${SF32LB57_SDK_PATH}/version.txt" sifli_version LIMIT_COUNT 1)
if(NOT sifli_version STREQUAL "v2.5.2")
    message(FATAL_ERROR "SF32LB57 uses the SiFli v2.5.2 BSP. Found ${sifli_version}.")
endif()
find_program(sf32_scons scons REQUIRED)
get_filename_component(sf32_arm_bin "${CMAKE_C_COMPILER}" DIRECTORY)

set(bsp_project "${CMAKE_CURRENT_BINARY_DIR}/bsp")
set(bsp_build "${bsp_project}/build_sf32lb57-lcd_n16r8_dpi_hcpu")
set(bsp_library "${bsp_build}/libsf32lb57_bsp.a")
set(bsp_linker "${CMAKE_CURRENT_BINARY_DIR}/hcpu.lds")
file(GLOB bsp_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bsp/*")
foreach(source IN LISTS bsp_sources)
    get_filename_component(name "${source}" NAME)
    configure_file("${source}" "${bsp_project}/${name}" COPYONLY)
endforeach()
configure_file(include/sf32lb57_board.h "${bsp_project}/sf32lb57_board.h" COPYONLY)

# Give SCons its own build tree. Its incremental scan tracks vendor source changes.
add_custom_target(sf32lb57_bsp
    COMMAND "${CMAKE_COMMAND}" -E chdir "${bsp_project}"
        "${CMAKE_COMMAND}" -E env
        "SIFLI_SDK=${SF32LB57_SDK_PATH}"
        "PYTHONPATH=${SF32LB57_SDK_PATH}/tools/build:${SF32LB57_SDK_PATH}/tools"
        "RTT_CC=gcc" "RTT_EXEC_PATH=${sf32_arm_bin}"
        "${sf32_scons}" --board=sf32lb57-lcd_n16r8_dpi -j4
    COMMAND "${CMAKE_COMMAND}" "-DINPUT=${bsp_build}/link_copy.lds" "-DOUTPUT=${bsp_linker}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/configure_linker.cmake"
    BYPRODUCTS "${bsp_library}" "${bsp_linker}" "${bsp_build}/sftool_param.json"
    VERBATIM)

add_dependencies(${COMPONENT_LIB} sf32lb57_bsp)
target_link_libraries(${COMPONENT_LIB} INTERFACE
    "-Wl,--whole-archive,${bsp_library},--no-whole-archive")
target_link_options(${COMPONENT_LIB} INTERFACE
    "-T${bsp_linker}" "-Wl,--gc-sections,--undefined=Reset_Handler"
    "-Wl,--defsym=pxCurrentTCB=pxCurrentTCBs")
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_DEPENDS "${bsp_library}" "${bsp_linker}")
idf_build_set_property(SF32LB57_BSP_BUILD "${bsp_build}")
