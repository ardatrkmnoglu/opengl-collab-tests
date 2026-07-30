#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * CMake'den -DTEST_INIT, -DTEST_DRAW, -DTEST_CLOSE
 * tanimlari ile derlenir. Boylece her test dosyasi
 * kendi basina bagimsiz bir executable olarak calisir.
 *
 * Ornek:
 *   -DTEST_INIT=GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_init
 *   -DTEST_DRAW=GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_draw
 *   -DTEST_CLOSE=GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_close
 */

extern void TEST_INIT(void);
extern void TEST_DRAW(void);
extern void TEST_CLOSE(void);

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

    TEST_INIT();

    while (!glfwWindowShouldClose(window)) {
        TEST_DRAW();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    TEST_CLOSE();
    glfwTerminate();
    return 0;
}
