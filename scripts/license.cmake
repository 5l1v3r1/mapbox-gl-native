# Generate the license file for the target ${param} at config time and write it to ${output_dir} on the `license` target.

function(mbgl_generate_license param output_dir)
    # Fake targets or system dependencies.
    list(APPEND BLACKLIST "mbgl-compiler-options")
    list(APPEND BLACKLIST "OpenGL::GL")
    list(APPEND BLACKLIST "OpenGL::GLX")
    list(APPEND BLACKLIST "PNG::PNG")

    get_target_property(LIBRARIES ${param} LINK_LIBRARIES)
    list(INSERT LIBRARIES 0 ${param})

    # cmake-format: off
    foreach(LIBRARY IN LISTS LIBRARIES)
    # cmake-format: on
        if(${LIBRARY} IN_LIST BLACKLIST)
            continue()
        endif()

        if(TARGET ${LIBRARY})
            get_target_property(NAME ${LIBRARY} INTERFACE_MAPBOX_NAME)
            get_target_property(URL ${LIBRARY} INTERFACE_MAPBOX_URL)
            get_target_property(AUTHOR ${LIBRARY} INTERFACE_MAPBOX_AUTHOR)
            get_target_property(LICENSE ${LIBRARY} INTERFACE_MAPBOX_LICENSE)

            if(NOT LICENSE OR NOT EXISTS ${LICENSE})
                message(FATAL_ERROR "License not found for target: ${LIBRARY}")
            endif()

            file(READ ${LICENSE} LICENSE_DATA)

            string(APPEND LICENSE_LIST "### [${NAME}](${URL}) by ${AUTHOR}\n\n")
            string(APPEND LICENSE_LIST "```\n${LICENSE_DATA}\n```\n\n")
            string(APPEND LICENSE_LIST "---\n\n")
        endif()
    endforeach()

    set(LICENSE_FILE LICENSE-${CMAKE_SYSTEM_NAME}.md)
    file(WRITE ${CMAKE_BINARY_DIR}/${LICENSE_FILE} ${LICENSE_LIST})

    add_custom_target(
        license
        COMMAND
            ${CMAKE_COMMAND}
            -E
            copy
            ${CMAKE_BINARY_DIR}/${LICENSE_FILE}
            ${output_dir}/${LICENSE_FILE}
    )
endfunction()
