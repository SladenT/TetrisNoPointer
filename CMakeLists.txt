# Project Defaults
cmake_minimum_required(VERSION 3.28)
set(C_STANDARD 99)
project(TetrisNoPointer)


# Source Specification
set(SRC "${CMAKE_SOURCE_DIR}/src")
set(CMAKEDIR "${CMAKE_SOURCE_DIR}/cmake")

add_executable(${PROJECT_NAME} "${SRC}/main.c")


# CMake Includes
include("${CMAKEDIR}/source_handler.cmake")

target_link_libraries(${PROJECT_NAME}
                        opengl32
                        gdi32
                        ${CMAKE_SOURCE_DIR}/libs/libraylib.a
                        winmm)

set(includelist 
    "${SRC}/")

# Source Globbing
target_include_directories(
    ${PROJECT_NAME}
        PUBLIC
            "${includelist}"
            "${CMAKE_SOURCE_DIR}/include/")

glob_sources(${PROJECT_NAME} "${includelist}" "*.c")