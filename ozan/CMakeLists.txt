cmake_minimum_required(VERSION 3.29)
project(RasterizationRobustness C)

set(CMAKE_C_STANDARD 11)

file(GLOB TEST_SOURCES "tests/Rasterization/*.c")

add_executable(robustness_test main.c ${TEST_SOURCES}
        tests/ViewportandClipping/ViewportandClipping_DepthRangef_TP_001.c
        tests/ViewportandClipping/ViewportandClipping_Viewport_TP_001.c
        tests/PixelRectangles/PixelRectangles_PixelStorei_TP_001.c
        tests/ErrorsandStatusReset/ErrorsandStatusReset_GetError_TP_001.c
        tests/SpecialFunctions/SpecialFunctions_Flush_TP_001.c
        tests/SpecialFunctions/SpecialFunctions_Finish_TP_001.c
        tests/SpecialFunctions/glHint.c)

target_link_libraries(robustness_test PRIVATE opengl32 m)
