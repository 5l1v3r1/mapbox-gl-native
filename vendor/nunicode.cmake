if(TARGET mbgl-vendor-nunicode)
    return()
endif()

add_library(
    mbgl-vendor-nunicode STATIC
    ${CMAKE_CURRENT_LIST_DIR}/nunicode/src/libnu/ducet.c
    ${CMAKE_CURRENT_LIST_DIR}/nunicode/src/libnu/strcoll.c
    ${CMAKE_CURRENT_LIST_DIR}/nunicode/src/libnu/strings.c
    ${CMAKE_CURRENT_LIST_DIR}/nunicode/src/libnu/tolower.c
    ${CMAKE_CURRENT_LIST_DIR}/nunicode/src/libnu/tounaccent.c
    ${CMAKE_CURRENT_LIST_DIR}/nunicode/src/libnu/toupper.c
    ${CMAKE_CURRENT_LIST_DIR}/nunicode/src/libnu/utf8.c
)

target_compile_definitions(
    mbgl-vendor-nunicode
    PRIVATE NU_BUILD_STATIC
)

target_compile_options(mbgl-vendor-nunicode PRIVATE -Wno-error)

target_include_directories(
    mbgl-vendor-nunicode SYSTEM
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/nunicode/include
)

set_target_properties(
    mbgl-vendor-nunicode
    PROPERTIES
        INTERFACE_MAPBOX_NAME "libnu"
        INTERFACE_MAPBOX_URL "https://bitbucket.org/alekseyt/nunicode"
        INTERFACE_MAPBOX_AUTHOR "Aleksey Tulinov"
        INTERFACE_MAPBOX_LICENSE ${CMAKE_CURRENT_LIST_DIR}/nunicode/LICENSE
)
