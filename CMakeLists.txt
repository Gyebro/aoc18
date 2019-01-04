cmake_minimum_required(VERSION 3.12)
project(aoc18)

set(CMAKE_CXX_STANDARD 14)

add_executable(aoc18
        sources/main.cpp
        sources/common.h
        sources/common.cpp sources/days.cpp sources/days.h sources/config.h)

# Copy inputs to app dir
add_custom_command(TARGET aoc18 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/inputs"
        $<TARGET_FILE_DIR:aoc18>)