#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef GL_RGBA8_OES
#define GL_RGBA8_OES 0x8058
#endif

// PFNGLTEXSTORAGE2DEXTPROC GLES2/gl2ext.h baslik dosyasinda zaten tanimlidir
static PFNGLTEXSTORAGE2DEXTPROC pglTexStorage2D = NULL;

static GLFWwindow* window = NULL;
static int width = 640, height = 480;
static int g_tests_failed = 0;
static const char* windowTitle = "glTexStorage2D Robustness Tests";

void init(void);
void draw(void);
void clean(void);

static void clearGLErrors(void) {
    while (glGetError() != GL_NO_ERROR);
}

// Test 1: Negatif Boyut Testi (Invalid Value)
static void test_NegativeDimensions(void) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    clearGLErrors();

    // Genişlik olarak kasten negatif bir değer (-256) veriyoruz
    pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, -256, 256);

    GLenum err = glGetError();
    printf("[TEST 1] Negatif Boyut Testi: ");
    if (err == GL_INVALID_VALUE) {
        printf("BASARILI (Beklenen GL_INVALID_VALUE hatasi alindi)\n");
    } else {
        printf("BASARISIZ (Beklenmeyen sonuc: 0x%X)\n", err);
        g_tests_failed = 1;
    }

    glDeleteTextures(1, &textureID);
}

// Test 2: Geçersiz İç Format Testi (Invalid Enum)
static void test_InvalidInternalFormat(void) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    clearGLErrors();

    // internalformat parametresine geçersiz/uydurma bir enum (0x9999) veriyoruz
    pglTexStorage2D(GL_TEXTURE_2D, 1, 0x9999, 256, 256);

    GLenum err = glGetError();
    printf("[TEST 2] Gecersiz Format Testi: ");
    if (err == GL_INVALID_ENUM) {
        printf("BASARILI (Beklenen GL_INVALID_ENUM hatasi alindi)\n");
    } else {
        printf("BASARISIZ (Beklenmeyen sonuc: 0x%X)\n", err);
        g_tests_failed = 1;
    }

    glDeleteTextures(1, &textureID);
}

// Test 3: Değiştirilemezlik (Immutability) Testi (Invalid Operation)
static void test_Immutability(void) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    clearGLErrors();

    // İlk tahsis: Başarılı olması beklenir
    pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, 256, 256);
    clearGLErrors();

    // İkinci tahsis: Aynı doku üzerinde tekrar ayırma işlemi deniyoruz
    pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, 512, 512);

    GLenum err = glGetError();
    printf("[TEST 3] Degistirilemezlik (Immutability) Testi: ");

    if (err == GL_INVALID_OPERATION) {
        printf("BASARILI (Sistem ikinci ayirma islemini GL_INVALID_OPERATION ile reddetti)\n");
    } else {
        printf("BASARISIZ (Sistem kurali ihlal etti veya yanlis hata dondu: 0x%X)\n", err);
        g_tests_failed = 1;
    }

    glDeleteTextures(1, &textureID);
}

// Test 4 : level ile boyut ilişkisini test etme
static void test_level_width_heigth(void) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    clearGLErrors();

    pglTexStorage2D(GL_TEXTURE_2D, 2, GL_RGBA8_OES, 500, 500);

    GLenum err = glGetError();
    printf("[TEST 4] Level > 1 heigth-width testi: ");

    if (err == GL_INVALID_OPERATION) {
        printf("BASARILI (Sistem spesifikasyona uydu ve GL_INVALID_OPERATION döndürdü)\n");
    } else if (err != GL_NO_ERROR) {
        printf("BASARISIZ (Yanlis hata kodu döndü: 0x%X, beklenen: 0x%X)\n", err, GL_INVALID_OPERATION);
        g_tests_failed = 1;
    } else {
        printf("BASARISIZ (Sistem kurali ihlal etti, hata firlatmasi gerekirken firlatmadi!)\n");
        g_tests_failed = 1;
    }

    glDeleteTextures(1, &textureID);
}

// Test 5 : max level'i aşmaya çalışmak
static void test_max_levels_exceeded(void) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    clearGLErrors();

    GLsizei w = 64;
    GLsizei h = 64;
    GLint levels = 8;

    pglTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8_OES, w, h);

    GLenum err = glGetError();
    printf("[TEST 5] Maksimum Mipmap Seviyesi Asimi Testi: ");

    if (err == GL_INVALID_OPERATION) {
        printf("BASARILI (Sistem formüle uydu ve GL_INVALID_OPERATION döndürdü)\n");
    } else if (err != GL_NO_ERROR) {
        printf("BASARISIZ (Yanlis hata kodu döndü: 0x%X, beklenen: 0x%X)\n", err, GL_INVALID_OPERATION);
        g_tests_failed = 1;
    } else {
        printf("BASARISIZ (Sistem kurali ihlal etti, kapasite disi seviyeye ragmen hata firlatmadi!)\n");
        g_tests_failed = 1;
    }

    glDeleteTextures(1, &textureID);
}

void init(void) {
    pglTexStorage2D = (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress("glTexStorage2DEXT");
    if (!pglTexStorage2D) pglTexStorage2D = (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress("glTexStorage2D");

    if (!pglTexStorage2D) {
        printf("[HATA] glTexStorage2DEXT bulunamadi!\n");
        g_tests_failed = 1;
        return;
    }

    printf("=== TexStorage2D Robustness Testleri Basliyor ===\n\n");

    test_NegativeDimensions();
    test_InvalidInternalFormat();
    test_Immutability();
    test_level_width_heigth();
    test_max_levels_exceeded();

    printf("\n=== Testler Tamamlandi ===\n");
}

void draw(void) {
    if (g_tests_failed) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Siyah
    } else {
        glClearColor(0.0f, 0.4f, 0.2f, 1.0f); // Yeşil
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
