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
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Executables for each test file in tests/
add_executable(getTexParameter_tests tests/getTexParameter_tests.c)
target_link_libraries(getTexParameter_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(readNPixels_tests tests/readNPixels_tests.c)
target_link_libraries(readNPixels_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${GLEW_LIBRARIES} ${M_LIB})

add_executable(texStorage2D_tests tests/texStorage2D_tests.c)
target_link_libraries(texStorage2D_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(texSubImage2D_tests tests/TexSubImage2D.c)
target_link_libraries(texSubImage2D_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(glGenTextures_tests tests/glGenTextures_tests.c)
target_link_libraries(glGenTextures_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(glBindTexture_tests tests/glBindTexture_tests.c)
target_link_libraries(glBindTexture_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(glGenerateMipmap_tests tests/glGenerateMipmap_tests.c)
target_link_libraries(glGenerateMipmap_tests PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})

add_executable(texSubImage2D_visual_test tests/texSubImage2D_visual_test.c)
target_link_libraries(texSubImage2D_visual_test PRIVATE ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLES2_LIB} ${EGL_LIB} ${M_LIB})
