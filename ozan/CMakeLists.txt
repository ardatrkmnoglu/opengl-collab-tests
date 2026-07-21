cmake_minimum_required(VERSION 3.29)
project(RasterizationRobustness C)

set(CMAKE_C_STANDARD 11)

file(GLOB TEST_SOURCES "tests/Rasterization/*.c")

add_executable(robustness_test main.c ${TEST_SOURCES})

target_link_libraries(robustness_test PRIVATE opengl32 m)
