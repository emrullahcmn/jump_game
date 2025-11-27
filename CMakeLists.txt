cmake_minimum_required(VERSION 4.1)
project(MY_GAME_3 C)

add_executable(main main.c)
set_target_properties(main PROPERTIES OUTPUT_NAME "jump_game")
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
)

message(STATUS "Using FetchContent to download raylib")
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)
target_link_libraries(main PRIVATE raylib)

if(MINGW)
    target_link_libraries(main PRIVATE winmm)
endif()

if(APPLE)
    target_link_libraries(main PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()