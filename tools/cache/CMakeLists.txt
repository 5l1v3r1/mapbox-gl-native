if(NOT CMAKE_SYSTEM_NAME STREQUAL Linux AND NOT CMAKE_SYSTEM_NAME STREQUAL Darwin)
    return()
endif()

add_executable(
    mbgl-cache
    ${CMAKE_CURRENT_LIST_DIR}/cache.cpp
)

target_link_libraries(
    mbgl-cache
    PRIVATE
        Mapbox::Base
        Mapbox::Base::Extras::args
        mbgl-compiler-options
        mbgl-core
)

install(TARGETS mbgl-cache RUNTIME DESTINATION bin)
