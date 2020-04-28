if(NOT CMAKE_SYSTEM_NAME STREQUAL Linux AND NOT CMAKE_SYSTEM_NAME STREQUAL Darwin)
    return()
endif()

add_executable(
    mbgl-render
    ${CMAKE_CURRENT_LIST_DIR}/render.cpp
)

target_link_libraries(
    mbgl-render
    PRIVATE Mapbox::Base::Extras::args mbgl-compiler-options mbgl-core
)

install(TARGETS mbgl-render RUNTIME DESTINATION bin)
