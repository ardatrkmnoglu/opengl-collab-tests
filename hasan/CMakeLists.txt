cmake_minimum_required(VERSION 3.20)
project(hasan_robustness_tests C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenGL, GLFW3, GLESv2, EGL for Linux/Ubuntu environment
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW glew)

find_library(GLES2_LIB NAMES GLESv2 REQUIRED)
find_library(EGL_LIB NAMES EGL REQUIRED)
find_library(M_LIB m)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Ortak link kutuphaneleri
set(COMMON_LIBS ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${GLEW_LIBRARIES} ${M_LIB})

# =============================================================
# Yardimci makro: her test dosyasi icin ayri executable olusturur
# test_runner.c + test dosyasi derlenir,
# -DTEST_INIT / -DTEST_DRAW / -DTEST_CLOSE makrolariyla
# dogru fonksiyonlar cagirilir.
# =============================================================
macro(add_robustness_test TARGET_NAME TEST_SOURCE TEST_PREFIX)
    add_executable(${TARGET_NAME}
        main.c
        ${TEST_SOURCE}
    )
    target_compile_definitions(${TARGET_NAME} PRIVATE
        TEST_INIT=${TEST_PREFIX}_init
        TEST_DRAW=${TEST_PREFIX}_draw
        TEST_CLOSE=${TEST_PREFIX}_close
    )
    target_link_libraries(${TARGET_NAME} PRIVATE ${COMMON_LIBS})
endmacro()

# --- PerFragmentOperations - Scissor ---
add_robustness_test(PFO_Scissor
    tests/PerFragmentOperations/GS_GL20SC_PFO_S_ROBUSTNESS_TP_001.c
    GS_GL20SC_PFO_S_ROBUSTNESS_TP_001
)

# --- PerFragmentOperations - BlendEquation ---
add_robustness_test(PFO_BlendEquation
    tests/PerFragmentOperations/GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001.c
    GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001
)

# --- PerFragmentOperations - BlendFunc ---
add_robustness_test(PFO_BlendFunc
    tests/PerFragmentOperations/GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001.c
    GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001
)

# --- PerFragmentOperations - BlendFuncSeparate ---
add_robustness_test(PFO_BlendFuncSeparate
    tests/PerFragmentOperations/GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001.c
    GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001
)

# --- PerFragmentOperations - BlendEquationSeparate ---
add_robustness_test(PFO_BlendEquationSeparate
    tests/PerFragmentOperations/GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001.c
    GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001
)

# --- ReadingPixels - ReadnPixels ---
add_robustness_test(RP_ReadnPixels
    tests/ReadingPixels/GS_GL20SC_RP_RNP_ROBUSTNESS_TP_001.c
    GS_GL20SC_RP_RNP_ROBUSTNESS_TP_001
)

# --- Texturing - BindTexture ---
add_robustness_test(TEXT_BindTexture
    tests/Texturing/GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001.c
    GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001
)

# --- Texturing - TexStorage2D ---
add_robustness_test(TEXT_TexStorage2D
    tests/Texturing/GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001.c
    GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001
)

# --- Texturing - TexSubImage2D ---
add_robustness_test(TEXT_TexSubImage2D
    tests/Texturing/GS_GL20SC_TEXT_TSI_ROBUSTNESS_TP_001.c
    GS_GL20SC_TEXT_TSI_ROBUSTNESS_TP_001
)

# --- Texturing - GenTextures ---
add_robustness_test(TEXT_GenTextures
    tests/Texturing/GS_GL20SC_TEXT_GT_ROBUSTNESS_TP_001.c
    GS_GL20SC_TEXT_GT_ROBUSTNESS_TP_001
)

# --- Texturing - GenerateMipmap ---
add_robustness_test(TEXT_GenerateMipmap
    tests/Texturing/GS_GL20SC_TEXT_GM_ROBUSTNESS_TP_001.c
    GS_GL20SC_TEXT_GM_ROBUSTNESS_TP_001
)
