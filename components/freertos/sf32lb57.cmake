include(FetchContent)
set(SF32LB57_FREERTOS_PATH "$ENV{SF32LB57_FREERTOS_PATH}" CACHE PATH "Optional FreeRTOS-Kernel V10.5.1 checkout")
if(SF32LB57_FREERTOS_PATH)
    set(arm_kernel "${SF32LB57_FREERTOS_PATH}")
else()
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    FetchContent_Declare(sf32lb57_arm_port
        URL https://codeload.github.com/FreeRTOS/FreeRTOS-Kernel/tar.gz/def7d2df2b0506d3d249334974f51e427c17a41c
        URL_HASH SHA256=0196ed0e0ed3e2fdba0cd1ae6d581147395b0e502b5c3a5dfc8ee84722b4a623
        SOURCE_SUBDIR portable/GCC/ARM_CM33_NTZ/non_secure)
    FetchContent_MakeAvailable(sf32lb57_arm_port)
    set(arm_kernel "${sf32lb57_arm_port_SOURCE_DIR}")
endif()

set(arm_port "${arm_kernel}/portable/GCC/ARM_CM33_NTZ/non_secure")
if(NOT EXISTS "${arm_port}/port.c")
    message(FATAL_ERROR "SF32LB57 requires the FreeRTOS V10.5.1 GCC Cortex-M33 port.")
endif()

target_sources(${COMPONENT_LIB} PRIVATE "${arm_port}/port.c" "${arm_port}/portasm.c" "port_sf32lb57.c")
target_include_directories(${COMPONENT_LIB} PUBLIC "${arm_port}")
set_source_files_properties("${arm_port}/port.c" PROPERTIES
    COMPILE_DEFINITIONS "SysTick_Handler=sf32lb57_freertos_systick")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u app_main")
