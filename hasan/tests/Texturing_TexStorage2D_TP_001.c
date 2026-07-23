#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/macro.h"

static const char* test_case1 = "Texturing_TexStorage2D_TC_001";
static const char* test_case2 = "Texturing_TexStorage2D_TC_002";
static const char* test_case3 = "Texturing_TexStorage2D_TC_003";
static const char* test_case4 = "Texturing_TexStorage2D_TC_004";
static const char* test_case5 = "Texturing_TexStorage2D_TC_005";

static const char* test_procedure = "Texturing_TexStorage2D_TP_001";

#ifndef GL_RGBA8_OES
#define GL_RGBA8_OES 0x8058
#endif

// PFNGLTEXSTORAGE2DEXTPROC GLES2/gl2ext.h baslik dosyasinda zaten tanimlidir
static PFNGLTEXSTORAGE2DEXTPROC pglTexStorage2D = NULL;

void Texturing_TexStorage2D_init(void) {
    pglTexStorage2D = (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress("glTexStorage2DEXT");
    if (!pglTexStorage2D) pglTexStorage2D = (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress("glTexStorage2D");

    if (!pglTexStorage2D) {
        printf("[HATA] glTexStorage2DEXT bulunamadi!\n");
    }
}

// Test 1: Negatif Boyut Testi (Invalid Value)
void Texturing_TexStorage2D_TC_001(void) {
    while (glGetError() != GL_NO_ERROR);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    while (glGetError() != GL_NO_ERROR);

    // Genişlik olarak kasten negatif bir değer (-256) veriyoruz
    if (pglTexStorage2D) {
        pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, -256, 256);
    }

    GLenum err = glGetError();
    if (err == GL_INVALID_VALUE) {
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case1, test_procedure, "Beklenen GL_INVALID_VALUE (0x501), alinan: 0x%X", err);
    }

    glDeleteTextures(1, &textureID);
}

// Test 2: Geçersiz İç Format Testi (Invalid Enum)
void Texturing_TexStorage2D_TC_002(void) {
    while (glGetError() != GL_NO_ERROR);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    while (glGetError() != GL_NO_ERROR);

    // internalformat parametresine geçersiz/uydurma bir enum (0x9999) veriyoruz
    if (pglTexStorage2D) {
        pglTexStorage2D(GL_TEXTURE_2D, 1, 0x9999, 256, 256);
    }

    GLenum err = glGetError();
    if (err == GL_INVALID_ENUM) {
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case2, test_procedure, "Beklenen GL_INVALID_ENUM (0x500), alinan: 0x%X", err);
    }

    glDeleteTextures(1, &textureID);
}

// Test 3: Değiştirilemezlik (Immutability) Testi (Invalid Operation)
void Texturing_TexStorage2D_TC_003(void) {
    while (glGetError() != GL_NO_ERROR);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    while (glGetError() != GL_NO_ERROR);

    if (pglTexStorage2D) {
        // İlk tahsis: Başarılı olması beklenir
        pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, 256, 256);
        while (glGetError() != GL_NO_ERROR);

        // İkinci tahsis: Aynı doku üzerinde tekrar ayırma işlemi deniyoruz
        pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, 512, 512);
    }

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case3, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case3, test_procedure, "Beklenen GL_INVALID_OPERATION (0x502), alinan: 0x%X", err);
    }

    glDeleteTextures(1, &textureID);
}

// Test 4 : level ile boyut ilişkisini test etme
void Texturing_TexStorage2D_TC_004(void) {
    while (glGetError() != GL_NO_ERROR);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    while (glGetError() != GL_NO_ERROR);

    if (pglTexStorage2D) {
        pglTexStorage2D(GL_TEXTURE_2D, 2, GL_RGBA8_OES, 500, 500);
    }

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case4, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case4, test_procedure, "Beklenen GL_INVALID_OPERATION (0x502), alinan: 0x%X", err);
    }

    glDeleteTextures(1, &textureID);
}

// Test 5 : max level'i aşmaya çalışmak
void Texturing_TexStorage2D_TC_005(void) {
    while (glGetError() != GL_NO_ERROR);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    while (glGetError() != GL_NO_ERROR);

    GLsizei w = 64;
    GLsizei h = 64;
    GLint levels = 8;

    if (pglTexStorage2D) {
        pglTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8_OES, w, h);
    }

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case5, test_procedure);
    } else {
        TEST_LOG_FAIL(test_case5, test_procedure, "Beklenen GL_INVALID_OPERATION (0x502), alinan: 0x%X", err);
    }

    glDeleteTextures(1, &textureID);
}
