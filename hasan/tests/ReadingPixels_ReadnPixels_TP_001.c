#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/macro.h"

static const char* test_case1 = "ReadingPixels_ReadnPixels_TC_001";
static const char* test_case2 = "ReadingPixels_ReadnPixels_TC_002";
static const char* test_case3 = "ReadingPixels_ReadnPixels_TC_003";
static const char* test_case4 = "ReadingPixels_ReadnPixels_TC_004";

static const char* test_procedure = "ReadingPixels_ReadnPixels_TP_001";

#ifndef APIENTRYP
#ifdef APIENTRY
#define APIENTRYP APIENTRY *
#else
#define APIENTRYP *
#endif
#endif

#ifndef PFNGLREADNPIXELSPROC
typedef void (APIENTRYP PFNGLREADNPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
#endif
static PFNGLREADNPIXELSPROC pglReadnPixels = NULL;
#define glReadnPixels pglReadnPixels

static GLFWwindow* window = NULL;
static int width = 640, height = 480;
static int g_tests_failed = 0;
static const char* windowTitle = "glReadnPixels Robustness Tests";

void init(void);
void draw(void);
void clean(void);

// --- TEST 1: YETERSİZ KUTU BOYUTU (Buffer Overflow) ---
void ReadingPixels_ReadnPixels_TC_001(void) {
    while (glGetError() != GL_NO_ERROR);

    unsigned char kucukKutu[3];
    int kutuBoyutu = sizeof(kucukKutu); // Sadece 3 byte

    // 10x10 (300 byte) alan istiyoruz
    glReadnPixels(0, 0, 10, 10, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu, kucukKutu);

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case1, test_procedure, "Expected GL_INVALID_OPERATION (0x502), but got 0x%X", err);
        g_tests_failed = 1;
    }
}

// --- TEST 2: NEGATİF BOYUT VERME (Boundary) ---
void ReadingPixels_ReadnPixels_TC_002(void) {
    while (glGetError() != GL_NO_ERROR);

    unsigned char kutu[300];
    int kutuBoyutu = sizeof(kutu);

    // Genişlik ve yüksekliğe negatif değerler veriyoruz
    glReadnPixels(0, 0, -10, -10, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu, kutu);

    GLenum err = glGetError();
    if (err == GL_INVALID_VALUE) {
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case2, test_procedure, "Expected GL_INVALID_VALUE (0x501), but got 0x%X", err);
        g_tests_failed = 1;
    }
}

// --- TEST 3: GEÇERSİZ PARAMETRE (Fuzzing) ---
void ReadingPixels_ReadnPixels_TC_003(void) {
    while (glGetError() != GL_NO_ERROR);

    unsigned char kutu[3];
    int kutuBoyutu = sizeof(kutu);

    // GL_RGB yerine rastgele/tanımsız bir sayı (9999) gönderiyoruz
    glReadnPixels(0, 0, 1, 1, 9999, GL_UNSIGNED_BYTE, kutuBoyutu, kutu);

    GLenum err = glGetError();
    if (err == GL_INVALID_ENUM) {
        TEST_LOG_SUCCESS(test_case3, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case3, test_procedure, "Expected GL_INVALID_ENUM (0x500), but got 0x%X", err);
        g_tests_failed = 1;
    }
}

// --- TEST 4: EKRAN DIŞI KOORDİNAT (Out of Bounds) ---
void ReadingPixels_ReadnPixels_TC_004(void) {
    while (glGetError() != GL_NO_ERROR);

    unsigned char kutu[3];
    int kutuBoyutu = sizeof(kutu);

    // x ve y koordinatlarını ekranın çok dışına taşıyoruz (-50, -50)
    glReadnPixels(-50, -50, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu, kutu);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case4, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case4, test_procedure, "Unexpected error for out of bounds coordinates: 0x%X", err);
        g_tests_failed = 1;
    }
}

void init(void) {
    pglReadnPixels = (PFNGLREADNPIXELSPROC)glfwGetProcAddress("glReadnPixels");
    if (!pglReadnPixels) pglReadnPixels = (PFNGLREADNPIXELSPROC)glfwGetProcAddress("glReadnPixelsEXT");
    if (!pglReadnPixels) pglReadnPixels = (PFNGLREADNPIXELSPROC)glfwGetProcAddress("glReadnPixelsKHR");

    if (!pglReadnPixels) {
        printf("[HATA] glReadnPixels fonksiyon isaretcisi alinamadi!\n");
        g_tests_failed = 1;
        return;
    }

    ReadingPixels_ReadnPixels_TC_001();
    ReadingPixels_ReadnPixels_TC_002();
    ReadingPixels_ReadnPixels_TC_003();
    ReadingPixels_ReadnPixels_TC_004();
}

void draw(void) {
    if (g_tests_failed) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Siyah
    } else {
        glClearColor(0.0f, 0.2f, 0.6f, 1.0f); // Mavi
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void clean(void) {
}

int main(void) {
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    setenv("GALLIUM_DRIVER", "llvmpipe", 1);
    unsetenv("WAYLAND_DISPLAY");

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
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

    clean();
    glfwTerminate();
    return g_tests_failed ? -1 : 0;
}
