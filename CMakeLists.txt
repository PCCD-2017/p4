cmake_minimum_required (VERSION 2.6)
project (p4)
add_executable(ej1 ej1.c)
set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/out
)