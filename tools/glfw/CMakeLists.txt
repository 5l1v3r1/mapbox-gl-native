if(NOT CMAKE_SYSTEM_NAME STREQUAL Linux AND NOT CMAKE_SYSTEM_NAME STREQUAL Darwin)
    return()
endif()

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW glfw3 REQUIRED)

add_executable(
    mbgl-glfw
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/glfw_view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/glfw_renderer_frontend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/settings_json.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test_writer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp
)

set_property(
    SOURCE ${CMAKE_CURRENT_LIST_DIR}/glfw_view.cpp
    PROPERTY COMPILE_DEFINITIONS MAPBOX_PUCK_ASSETS_PATH=\"${CMAKE_CURRENT_LIST_DIR}/assets/\"
)

if(MBGL_WITH_OPENGL)
    target_sources(
        mbgl-glfw
        PRIVATE ${CMAKE_CURRENT_LIST_DIR}/glfw_gl_backend.cpp
    )
    target_compile_definitions(
        mbgl-glfw
        PRIVATE MBGL_RENDER_BACKEND_OPENGL=1
    )
endif()

target_include_directories(
    mbgl-glfw
    PRIVATE
        ${GLFW_INCLUDE_DIRS}
        # For /platform/default/src/mbgl/map/map_snapshotter.hpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)

# Use target_link_directories when we move away from CMake 3.10.
target_link_libraries(
    mbgl-glfw
    PRIVATE $<$<BOOL:${GLFW_LIBRARY_DIRS}>:-L${GLFW_LIBRARY_DIRS}>
)

target_link_libraries(
    mbgl-glfw
    PRIVATE
        ${GLFW_LIBRARIES}
        Mapbox::Base::Extras::args
        Mapbox::Base::Extras::filesystem
        Mapbox::Base::Extras::rapidjson
        Mapbox::Base::cheap-ruler-cpp
        Mapbox::Map
        mbgl-compiler-options
)

if(MBGL_WITH_OPENGL)
    target_link_libraries(
        mbgl-glfw
        PRIVATE OpenGL::GL
    )
endif()

set_property(TARGET mbgl-glfw PROPERTY FOLDER Executables)

install(TARGETS mbgl-glfw RUNTIME DESTINATION bin)

if(NOT MBGL_PUBLIC_BUILD)
    include(${PROJECT_SOURCE_DIR}/internal/tools/glfw/internal.cmake)
endif()
