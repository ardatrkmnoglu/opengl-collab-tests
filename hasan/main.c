#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>
#include "tests/tests.h"

void init(void);
void draw(void);
void cleanup(void);

int main(void) {
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    setenv("GALLIUM_DRIVER", "llvmpipe", 1);
    unsetenv("WAYLAND_DISPLAY");

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hasan Robustness Tests", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    init();

    while (!glfwWindowShouldClose(window)) {
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
    glfwTerminate();
    return 0;
}

void init(void) {
    /* --------------- ReadingPixels - ReadnPixels --------------- */
    ReadingPixels_ReadnPixels_init();
    ReadingPixels_ReadnPixels_TC_001();
    ReadingPixels_ReadnPixels_TC_002();
    ReadingPixels_ReadnPixels_TC_003();
    ReadingPixels_ReadnPixels_TC_004();
}

void draw(void) {
}

void cleanup(void) {
}
