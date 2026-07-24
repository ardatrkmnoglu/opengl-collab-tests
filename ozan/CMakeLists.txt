cmake_minimum_required(VERSION 3.29)
project(RasterizationRobustness C)

set(CMAKE_C_STANDARD 11)

file(GLOB TEST_SOURCES "tests/Rasterization/*.c")

add_executable(robustness_test main.c ${TEST_SOURCES}
        tests/ViewportandClipping/GS_GL20SC_VC_DR_ROBUSTNESS_TP_001.c
        tests/ViewportandClipping/GS_GL20SC_VC_V_ROBUSTNESS_TP_001.c
        tests/PixelRectangles/GS_GL20SC_PR_PS_ROBUSTNESS_TP_001.c
        tests/ErrorsandStatusReset/GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001.c
        tests/SpecialFunctions/GS_GL20SC_SF_FL_ROBUSTNESS_TP_001.c
        tests/SpecialFunctions/GS_GL20SC_SF_FI_ROBUSTNESS_TP_001.c
        tests/SpecialFunctions/glHint.c)

target_link_libraries(robustness_test PRIVATE opengl32 m)
    