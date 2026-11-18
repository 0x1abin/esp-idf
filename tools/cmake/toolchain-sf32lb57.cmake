set(_CMAKE_TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_SYSTEM_PROCESSOR cortex-m33)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
include($ENV{IDF_PATH}/tools/cmake/toolchain.cmake)

get_property(_sf32_try_compile GLOBAL PROPERTY IN_TRY_COMPILE)
if(NOT _sf32_try_compile)
    set(_sf32_cpu "-mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
    # Keep the application on the Arm GNU libc ABI. The BSP compiles its own
    # short enums and wide characters and exposes a fixed-width C interface.
    idf_toolchain_add_flags(C_COMPILE_OPTIONS "${_sf32_cpu}"
                            CXX_COMPILE_OPTIONS "${_sf32_cpu}"
                            ASM_COMPILE_OPTIONS "${_sf32_cpu} -Wa,-mimplicit-it=thumb"
                            LINK_OPTIONS "${_sf32_cpu} --specs=nosys.specs -Wl,--no-wchar-size-warning,--no-enum-size-warning")
endif()
