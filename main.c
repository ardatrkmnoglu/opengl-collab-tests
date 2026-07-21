#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "include/rtests.h"

/** COLORS!
 * RED:    "\x1b[31m"
 * GREEN:  "\x1b[32m"
 * YELLOW: "\x1b[33m"
 * BLUE:   "\x1b[34m"
 *
 * RESET:  "\x1b[0m"
 */

/**************************************/
/********** Helper Functions **********/
/**************************************/

// create window context
int createContext(GLFWwindow** window) {
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	*window = glfwCreateWindow(640, 480, "test", NULL, NULL);
	if (!*window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(*window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		return -1;
	glGetError();
	return 0;
}

// destroy window context
void destroyContext(GLFWwindow** window) {
	glfwDestroyWindow(*window);
	glfwTerminate();
}

int main() {
	GLFWwindow* w;
	createContext(&w);

	printf("[INFO] Initializing: Robustness tests for OpenGL SC 2.0\n");
	printf("--------------------------------------------------\n");

	init();
	draw();
	cleanup();

	printf("--------------------------------------------------\n");
	printf("[SUCCESS] All tests complete.\n");

	destroyContext(&w);
	return 0;
}
