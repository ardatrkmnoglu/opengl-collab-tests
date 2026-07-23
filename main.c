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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Ozan Robustness Tests", NULL, NULL);
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
    /* --------------- ViewportandClipping - Viewport --------------- */
    ViewportandClipping_Viewport_TC_001();
    ViewportandClipping_Viewport_TC_002();
    ViewportandClipping_Viewport_TC_003();
    ViewportandClipping_Viewport_TC_004();
    ViewportandClipping_Viewport_TC_005();
    ViewportandClipping_Viewport_TC_006();

    /* --------------- ViewportandClipping - DepthRangef --------------- */
    ViewportandClipping_DepthRangef_TC_001();
    ViewportandClipping_DepthRangef_TC_002();
    ViewportandClipping_DepthRangef_TC_003();
    ViewportandClipping_DepthRangef_TC_004();
    ViewportandClipping_DepthRangef_TC_005();
    ViewportandClipping_DepthRangef_TC_006();

    /* --------------- Rasterization - CullFace --------------- */
    Rasterizaton_CullFace_TC_001();
    Rasterizaton_CullFace_TC_002();
    Rasterizaton_CullFace_TC_003();
    Rasterizaton_CullFace_TC_004();
    Rasterizaton_CullFace_TC_005();
    Rasterizaton_CullFace_TC_006();
    Rasterizaton_CullFace_TC_007();
    Rasterizaton_CullFace_TC_008();
    Rasterizaton_CullFace_TC_009();

    /* --------------- Rasterization - FrontFace --------------- */
    Rasterizaton_FrontFace_TC_001();
    Rasterizaton_FrontFace_TC_002();
    Rasterizaton_FrontFace_TC_003();
    Rasterizaton_FrontFace_TC_004();
    Rasterizaton_FrontFace_TC_005();
    Rasterizaton_FrontFace_TC_006();
    Rasterizaton_FrontFace_TC_007();
    Rasterizaton_FrontFace_TC_008();

    /* --------------- Rasterization - LineWidth --------------- */
    Rasterizaton_LineWidth_TC_001();
    Rasterizaton_LineWidth_TC_002();
    Rasterizaton_LineWidth_TC_003();
    Rasterizaton_LineWidth_TC_004();
    Rasterizaton_LineWidth_TC_005();
    Rasterizaton_LineWidth_TC_006();
    Rasterizaton_LineWidth_TC_007();
    Rasterizaton_LineWidth_TC_008();

    /* --------------- Rasterization - PolygonOffset --------------- */
    Rasterizaton_PolygonOffset_TC_001();
    Rasterizaton_PolygonOffset_TC_002();
    Rasterizaton_PolygonOffset_TC_003();
    Rasterizaton_PolygonOffset_TC_004();
    Rasterizaton_PolygonOffset_TC_005();
    Rasterizaton_PolygonOffset_TC_006();
    Rasterizaton_PolygonOffset_TC_007();

    /* --------------- PixelRectangles - PixelStorei --------------- */
    PixelRectangles_PixelStorei_TC_001();
    PixelRectangles_PixelStorei_TC_002();
    PixelRectangles_PixelStorei_TC_003();
    PixelRectangles_PixelStorei_TC_004();
    PixelRectangles_PixelStorei_TC_005();
    PixelRectangles_PixelStorei_TC_006();

    /* --------------- SpecialFunctions - Flush --------------- */
    SpecialFunctions_Flush_TC_001();
    SpecialFunctions_Flush_TC_002();
    SpecialFunctions_Flush_TC_003();
    SpecialFunctions_Flush_TC_004();
    SpecialFunctions_Flush_TC_005();
    SpecialFunctions_Flush_TC_006();
    SpecialFunctions_Flush_TC_007();

    /* --------------- SpecialFunctions - Finish --------------- */
    SpecialFunctions_Finish_TC_001();
    SpecialFunctions_Finish_TC_002();
    SpecialFunctions_Finish_TC_003();
    SpecialFunctions_Finish_TC_004();
    SpecialFunctions_Finish_TC_005();
    SpecialFunctions_Finish_TC_006();
    SpecialFunctions_Finish_TC_007();

    /* --------------- ErrorsandStatusReset - GetError --------------- */
    ErrorsandStatusReset_GetError_TC_001();
    ErrorsandStatusReset_GetError_TC_002();
    ErrorsandStatusReset_GetError_TC_003();
    ErrorsandStatusReset_GetError_TC_004();
    ErrorsandStatusReset_GetError_TC_005();
    ErrorsandStatusReset_GetError_TC_006();
    ErrorsandStatusReset_GetError_TC_007();
}

void draw(void) {
}

void cleanup(void) {
}