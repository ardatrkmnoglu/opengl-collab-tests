#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>

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

static void hatalariTemizle(void) {
    while (glGetError() != GL_NO_ERROR);
}

// --- TEST 1: YETERSİZ KUTU BOYUTU (Buffer Overflow) ---
static void ReadingPixels_ReadnPixels_TC_001(void) {
    static const char* test_case = "ReadingPixels_ReadnPixels_TC_001";
    printf("[TEST][%s][%s] Yetersiz Kutu Boyutu Testi Basliyor...\n", test_procedure, test_case);
    hatalariTemizle();

    unsigned char kucukKutu[3];
    int kutuBoyutu = sizeof(kucukKutu); // Sadece 3 byte

    // 10x10 (300 byte) alan istiyoruz
    glReadnPixels(0, 0, 10, 10, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu, kucukKutu);

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        printf(" -> BASARILI (PASSED): GL_INVALID_OPERATION hatasi yakalandi.\n\n");
    } else {
        printf(" -> BASARISIZ (FAILED): Beklenen GL_INVALID_OPERATION alinmadi (Kod: 0x%X)!\n\n", err);
        g_tests_failed = 1;
    }
}

// --- TEST 2: NEGATİF BOYUT VERME (Boundary) ---
static void ReadingPixels_ReadnPixels_TC_002(void) {
    static const char* test_case = "ReadingPixels_ReadnPixels_TC_002";
    printf("[TEST][%s][%s] Negatif Boyut Testi Basliyor...\n", test_procedure, test_case);
    hatalariTemizle();

    unsigned char kutu[300];
    int kutuBoyutu = sizeof(kutu);

    // Genişlik ve yüksekliğe negatif değerler veriyoruz
    glReadnPixels(0, 0, -10, -10, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu, kutu);

    GLenum err = glGetError();
    if (err == GL_INVALID_VALUE) {
        printf(" -> BASARILI (PASSED): GL_INVALID_VALUE hatasi yakalandi.\n\n");
    } else {
        printf(" -> BASARISIZ (FAILED): Beklenen GL_INVALID_VALUE alinmadi (Kod: 0x%X)!\n\n", err);
        g_tests_failed = 1;
    }
}

// --- TEST 3: GEÇERSİZ PARAMETRE (Fuzzing) ---
static void ReadingPixels_ReadnPixels_TC_003(void) {
    static const char* test_case = "ReadingPixels_ReadnPixels_TC_003";
    printf("[TEST][%s][%s] Gecersiz Parametre (Enum) Testi Basliyor...\n", test_procedure, test_case);
    hatalariTemizle();

    unsigned char kutu[3];
    int kutuBoyutu = sizeof(kutu);

    // GL_RGB yerine rastgele/tanımsız bir sayı (9999) gönderiyoruz
    glReadnPixels(0, 0, 1, 1, 9999, GL_UNSIGNED_BYTE, kutuBoyutu, kutu);

    GLenum err = glGetError();
    if (err == GL_INVALID_ENUM) {
        printf(" -> BASARILI (PASSED): GL_INVALID_ENUM hatasi yakalandi.\n\n");
    } else {
        printf(" -> BASARISIZ (FAILED): Beklenen GL_INVALID_ENUM alinmadi (Kod: 0x%X)!\n\n", err);
        g_tests_failed = 1;
    }
}

// --- TEST 4: EKRAN DIŞI KOORDİNAT (Out of Bounds) ---
static void ReadingPixels_ReadnPixels_TC_004(void) {
    static const char* test_case = "ReadingPixels_ReadnPixels_TC_004";
    printf("[TEST][%s][%s] Ekran Disi Koordinat Testi Basliyor...\n", test_procedure, test_case);
    hatalariTemizle();

    unsigned char kutu[3];
    int kutuBoyutu = sizeof(kutu);

    // x ve y koordinatlarını ekranın çok dışına taşııyoruz (-50, -50)
    glReadnPixels(-50, -50, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu, kutu);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        printf(" -> BASARILI (PASSED): Program cokmedi, hata firlatilmadan islem gormezden gelindi.\n\n");
    } else {
        printf(" -> DIKKAT: Beklenmeyen bir hata kodu dondu: 0x%X\n\n", err);
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

    printf("=========================================\n");
    printf("  %s SAĞLAMLIK TESTLERİ\n", test_procedure);
    printf("=========================================\n\n");

    ReadingPixels_ReadnPixels_TC_001();
    ReadingPixels_ReadnPixels_TC_002();
    ReadingPixels_ReadnPixels_TC_003();
    ReadingPixels_ReadnPixels_TC_004();

    printf("=========================================\n");
    if (g_tests_failed) {
        printf("        BAZI TESTLER BASARISIZ OLDU!\n");
    } else {
        printf("        TÜM TESTLER TAMAMLANDI\n");
    }
    printf("=========================================\n");
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
    // Clean up resources if necessary
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
