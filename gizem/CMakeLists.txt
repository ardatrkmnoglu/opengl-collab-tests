cmake_minimum_required(VERSION 3.20)
project(untitled C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# GLFW kurulumu
include(FetchContent)
FetchContent_Declare(
        glfw
        URL https://github.com/glfw/glfw/archive/refs/tags/3.4.zip
)
FetchContent_MakeAvailable(glfw)

# GLAD için klasör yolunu tanıt
include_directories(external/glad/include)
file(GLOB TEST_SOURCES tests/*.c)
add_executable(untitled main.c ${TEST_SOURCES} external/glad/src/glad.c)

# Kütüphaneleri bağla
target_link_libraries(untitled PRIVATE glfw)
