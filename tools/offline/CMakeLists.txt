if(NOT CMAKE_SYSTEM_NAME STREQUAL Linux AND NOT CMAKE_SYSTEM_NAME STREQUAL Darwin)
    return()
endif()

add_executable(
    mbgl-offline
    ${CMAKE_CURRENT_LIST_DIR}/offline.cpp
)

target_link_libraries(
    mbgl-offline
    PRIVATE Mapbox::Base::Extras::args mbgl-compiler-options mbgl-core
)

install(TARGETS mbgl-offline RUNTIME DESTINATION bin)
