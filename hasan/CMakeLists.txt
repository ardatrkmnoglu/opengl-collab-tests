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

# Ana executable: main.c + tum test dosyalari
add_executable(hasan_tests main.c
    tests/ReadingPixels_ReadnPixels_TP_001.c
    tests/Per-FragmentOperations_Scissor_TP_001.c
    tests/Texturing_BindTexture_TP_001.c
    tests/Texturing_TexStorage2D_TP_001.c
)
target_link_libraries(hasan_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${GLEW_LIBRARIES} ${M_LIB})


add_executable(Texturing_TexSubImage2D_TP_001 tests/Texturing_TexSubImage2D_TP_001.c)
target_link_libraries(Texturing_TexSubImage2D_TP_001 PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(Texturing_GenTextures_TP_001 tests/Texturing_GenTextures_TP_001.c)
target_link_libraries(Texturing_GenTextures_TP_001 PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(Texturing_GenerateMipmap_TP_001 tests/Texturing_GenerateMipmap_TP_001.c)
target_link_libraries(Texturing_GenerateMipmap_TP_001 PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(Texturing_TexSubImage2D_TP_002 tests/Texturing_TexSubImage2D_TP_002.c)
target_link_libraries(Texturing_TexSubImage2D_TP_002 PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})
