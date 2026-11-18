# SPDX-License-Identifier: Apache-2.0
function(sf32lb57_setup_image elf)
    idf_build_get_property(bsp SF32LB57_BSP_BUILD)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    set(binary "${build_dir}/${CMAKE_PROJECT_NAME}.bin")
    set(manifest "${build_dir}/sftool_param.json")
    set(image_tool "${idf_path}/tools/sf32lb57/image.py")

    add_custom_command(OUTPUT "${binary}" "${manifest}"
        COMMAND "${CMAKE_OBJCOPY}" -O binary "$<TARGET_FILE:${elf}>" "${binary}"
        COMMAND "${python}" "${image_tool}" package "${bsp}" "${binary}" "${manifest}"
        DEPENDS ${elf} "${bsp}/sftool_param.json" "${image_tool}"
        VERBATIM)
    add_custom_target(sf32lb57_image ALL DEPENDS "${binary}" "${manifest}")
    add_custom_target(app DEPENDS sf32lb57_image)
    foreach(action flash app-flash)
        add_custom_target(${action}
            COMMAND "${python}" "${image_tool}" ${action} "${manifest}"
            DEPENDS sf32lb57_image
            USES_TERMINAL VERBATIM)
    endforeach()
endfunction()
