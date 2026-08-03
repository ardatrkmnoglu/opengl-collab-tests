//Gizem'de çalışması için
#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"


/*
GL20SC - FramebufferObjects - FramebufferTexture2D - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_011";
static const char* test_case_12 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_012";
static const char* test_case_13 = "GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_013";

// Yardımcı fonksiyon: her testte temiz bir framebuffer + texture çiftinin
// hazırlanması için kullanılan yardımcı kurulum rutini.
static void setup_fbo_and_texture(GLuint* fbo, GLuint* tex)
{
    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    while (glGetError() != GL_NO_ERROR) {}
}

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D((GLenum)0xFFFFFFFF, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM for invalid target, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if textarget is not an accepted texture target and texture is not 0.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLenum)0xFFFFFFFF, tex, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM for invalid textarget, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if attachment is not an accepted attachment point.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)0xFFFFFFFF, GL_TEXTURE_2D, tex, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_ENUM for invalid attachment, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if level is not 0 and texture is not 0.
/* It failed on Windows */
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 3);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_VALUE for non-zero level with non-zero texture, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the default framebuffer object name 0 is bound.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_INVALID_OPERATION with default framebuffer 0 bound, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if texture is neither 0 nor the name of an existing texture object.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint nonExistentTexture = 0xABCD1234;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nonExistentTexture, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Expected GL_INVALID_OPERATION for non-existent texture name, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if texture is the name of an existing
// two-dimensional texture object but textarget is not GL_TEXTURE_2D or if texture
// is the name of an existing cube map texture object but textarget is GL_TEXTURE_2D.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, tex, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Expected GL_INVALID_OPERATION for 2D texture with mismatched textarget, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: texture mevcut bir cube map texture olduğu halde textarget
// GL_TEXTURE_2D olduğunda GL_INVALID_OPERATION üretilmelidir.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, cubeTex;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &cubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    for (int face = 0; face < 6; ++face) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cubeTex, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Expected GL_INVALID_OPERATION for cube map texture with GL_TEXTURE_2D textarget, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilmeyen hatalar ------------------------------

// Geçersiz bir çağrı sonrasında (invalid attachment enum) mevcut geçerli attachment
// durumunun bozulmadığını doğrular.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)0xFFFFFFFF, GL_TEXTURE_2D, tex, 0); // gecersiz attachment

    GLenum err = glGetError();

    GLint objType = -1, objName = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objName);

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Expected GL_INVALID_ENUM, got 0x%x.", err);
    }
    else if (objType != GL_TEXTURE || (GLuint)objName != tex) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Previous valid attachment was altered by a failed call (objType=0x%x, objName=%d).", objType, objName);
    }
    else {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// texture parametresi 0 verildiğinde, mevcut bir attachment'ın başarıyla ayrıldığını
// (detach) ve ilgili attachment parametrelerinin varsayılan değerlere döndüğünü doğrular.
/* It failed on Windows */
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);

    GLenum err = glGetError();

    GLint objType = -1, objName = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objName);

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Unexpected error 0x%x while detaching texture with texture=0.", err);
    }
    else if (objType != GL_NONE || objName != 0) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Attachment was not properly detached (objType=0x%x, objName=%d).", objType, objName);
    }
    else {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// texture=0 iken level 0 olmayan bir değerle çağrıldığında GL_INVALID_VALUE
// üretilmemesi gerektiğini doğrular; çünkü belirtilen hata sadece "texture 0 değilken"
// level kısıtlamasını kapsar.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 5);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Expected GL_NO_ERROR for texture=0 regardless of level value, got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Bağlı framebuffer'a eklenmiş bir texture, texture bağlıyken (bound) ve aktif
// olarak sample edilebilecek şekilde tekrar aynı framebuffer'a eklendiğinde
// (feedback loop senaryosu) implementasyonun çökmediğini doğrular. Döküman bu
// durumda sonucun tanımsız olabileceğini belirtir, ancak çökme kabul edilemez.
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glBindTexture(GL_TEXTURE_2D, tex); // ayni texture hem attachment hem de bound texture

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    GLenum err = glGetError();
    (void)err;

    // Buraya kadar gelinmişse implementasyon feedback loop senaryosunda çökmemiştir.
    TEST_LOG_SUCCESS(test_case_12, test_procedure);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Sınır/uç değer testi:
// level ve texture parametreleri için kritik sınır/uç değerler tek bir test içinde
// art arda denenir:
// level = INT_MIN -> texture!=0 iken kesinlikle geçersiz (0 değil)
// level = -1 -> texture!=0 iken kesinlikle geçersiz (0 değil)
// level = 0 -> texture!=0 iken tek gecerli deger
// level = 1 -> texture!=0 iken kesinlikle geçersiz (0 değil)
// level = INT_MAX -> texture!=0 iken kesinlikle geçersiz (0 değil)
// texture = UINT_MAX (0xFFFFFFFF) -> mevcut olmayan isim, GL_INVALID_OPERATION beklenir
// texture = 0 ile level = INT_MAX -> texture=0 oldugundan level kisitlamasi devre disi,
// GL_NO_ERROR beklenir
// Her adımda implementasyonun çökmediği ve beklenen hata sınıflandırmasının doğru
// yapıldığı kontrol edilir; amaç sınır değerlerde kararlılığı sınamaktır.
/* It failed on Windows */
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, tex;
    setup_fbo_and_texture(&fbo, &tex);

    int failedIndex = -1;

    // texture != 0 iken level sinir degerleri (0 disindaki her deger GL_INVALID_VALUE vermeli)
    const GLint levelBoundaryValues[] = { INT_MIN, -1, 0, 1, INT_MAX };
    const size_t levelCount = sizeof(levelBoundaryValues) / sizeof(levelBoundaryValues[0]);

    for (size_t i = 0; i < levelCount && failedIndex == -1; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, levelBoundaryValues[i]);
        GLenum err = glGetError();

        if (levelBoundaryValues[i] == 0) {
            if (err != GL_NO_ERROR) {
                failedIndex = (int)i;
                TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_NO_ERROR for level=0 with non-zero texture, got 0x%x.", err);
            }
        }
        else {
            if (err != GL_INVALID_VALUE) {
                failedIndex = (int)i;
                TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_INVALID_VALUE for level=%d with non-zero texture, got 0x%x.", levelBoundaryValues[i], err);
            }
        }
    }

    // texture sinir degeri: mevcut olmayan buyuk bir isim
    if (failedIndex == -1) {
        while (glGetError() != GL_NO_ERROR) {}

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)0xFFFFFFFF, 0);
        GLenum err = glGetError();

        if (err != GL_INVALID_OPERATION) {
            failedIndex = (int)levelCount;
            TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_INVALID_OPERATION for non-existent texture=UINT_MAX, got 0x%x.", err);
        }
    }

    // texture=0 iken level sinirinin devre disi kalmasi
    if (failedIndex == -1) {
        while (glGetError() != GL_NO_ERROR) {}

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, INT_MAX);
        GLenum err = glGetError();

        if (err != GL_NO_ERROR) {
            failedIndex = (int)levelCount + 1;
            TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_NO_ERROR for texture=0 regardless of level=INT_MAX, got 0x%x.", err);
        }
    }

    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_13, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Initialization */
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_004();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_010();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_012();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FT2D_ROBUSTNESS_TC_013();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_FT2D_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_FT2D_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}