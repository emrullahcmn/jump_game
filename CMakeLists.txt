cmake_minimum_required(VERSION 4.1)
project(MY_GAME_3 C)

option(USE_SYSTEM_RAYLIB "Use prebuilt raylib instead of FetchContent" OFF)
add_executable(main main.c)
set_target_properties(main PROPERTIES OUTPUT_NAME "jump_game")
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
)

if(USE_SYSTEM_RAYLIB)
    message(STATUS "Using system-installed raylib")

    # Include directories (where raylib.h is)
    target_include_directories(main PRIVATE "C:/mingw/include")

    # Link directories (where libraylib.a is)
    target_link_directories(main PRIVATE "C:/mingw/lib")

    # Link libraries (Raylib + Windows dependencies)
    target_link_libraries(main PRIVATE raylib opengl32 gdi32 winmm user32 kernel32)
else()
    message(STATUS "Using FetchContent to download raylib")
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
endif()



# Link with raylib (cross-platform)
target_link_libraries(main PRIVATE raylib)

# avoid console window in Windows
if(WIN32 AND MINGW)
    target_link_options(main PRIVATE "-mwindows")
endif()

# On macOS, request frameworks (raylib does this automatically)
if(APPLE)
    target_link_libraries(main PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()
