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
GL20SC - FramebufferObjects - GetFramebufferAttachmentParameteriv - ROBUSTNESS
*/


static const char* test_procedure = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_011";
static const char* test_case_12 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_012";
static const char* test_case_13 = "GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_013";

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetFramebufferAttachmentParameteriv((GLenum)0xFFFFFFFF, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM for invalid target, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if attachment is not
// GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, or GL_STENCIL_ATTACHMENT.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, (GLenum)0xFFFFFFFF, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM for invalid attachment, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if the attached object at the
// named attachment point is GL_RENDERBUFFER and pname is not
// GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE or GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_ENUM for invalid pname on renderbuffer attachment, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if the attached object at the named attachment point is GL_TEXTURE and
// pname is not GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
// GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL, or GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, tex = 0;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLenum)0xFFFFFFFF, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_ENUM for undefined pname on texture attachment, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if there is no attached object at
// the named attachment point and pname is not GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    // GL_DEPTH_ATTACHMENT noktasına hiçbir nesne bağlanmadı
    GLint params = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_INVALID_ENUM for non-OBJECT_TYPE pname on empty attachment point, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the default framebuffer object name 0 is bound.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Expected GL_INVALID_OPERATION with default framebuffer 0 bound, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

// Belirtilmeyen hatalar ------------------------------

// Dökümana göre bir hata oluştuğunda params içeriğinin değiştirilmemesi gerektiği
// açıkça belirtilmiştir (Notes bölümü). Bu test, geçersiz bir çağrı sonrasında
// params değişkeninin önceden atanmış sentinel değerini koruduğunu doğrular.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    const GLint sentinel = 0x7EADBEEF;
    GLint params = sentinel;

    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, (GLenum)0xFFFFFFFF, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);

    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Expected GL_INVALID_ENUM, got 0x%x.", err);
    }
    else if (params != sentinel) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "params was modified despite an error being generated (value=0x%x, expected sentinel=0x%x).", params, sentinel);
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// params parametresi olarak NULL pointer verildiğinde implementasyonun
// çökmediğini (segfault üretmediğini) doğrular.
/* It crashed on Windows */
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, NULL);

    GLenum err = glGetError();
    (void)err;

    // Buraya kadar gelinmişse implementasyon NULL pointer ile çökmemiştir.
    TEST_LOG_SUCCESS(test_case_8, test_procedure);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Yeni oluşturulmuş bir framebuffer'ın hiçbir attachment noktasına henüz bir nesne
// bağlanmamışken, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE sorgusunun spesifikasyona
// uygun şekilde GL_NONE döndürdüğünü doğrular (başlangıç durumu kontrolü).
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint objType = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Unexpected error 0x%x while querying object type of an unattached point.", err);
    }
    else if (objType != GL_NONE) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Expected GL_NONE for unattached attachment point, got 0x%x.", objType);
    }
    else {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Bağlı olan nesne bir renderbuffer olduğunda, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
// sorgusunun doğru renderbuffer ismini döndürdüğünü doğrular.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint objType = -1;
    GLint objName = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objName);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Unexpected error 0x%x while querying renderbuffer attachment.", err);
    }
    else if (objType != GL_RENDERBUFFER) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Expected GL_RENDERBUFFER as object type, got 0x%x.", objType);
    }
    else if ((GLuint)objName != rbo) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Expected object name %u, got %d.", rbo, objName);
    }
    else {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Framebuffer'a bir renderbuffer bağlıyken, texture'a özgü bir pname olan
// GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE sorgulanmaya çalışıldığında
// GL_INVALID_ENUM üretilip params değerinin bozulmadığını doğrular.
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    const GLint sentinel = 0x12345678;
    GLint params = sentinel;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE, &params);

    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Expected GL_INVALID_ENUM for texture-specific pname on renderbuffer attachment, got 0x%x.", err);
    }
    else if (params != sentinel) {
        TEST_LOG_FAIL(test_case_11, test_procedure, "params was modified despite an error being generated (value=0x%x, expected sentinel=0x%x).", params, sentinel);
    }
    else {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Bir attachment noktasından nesne kaldırıldığında (detach edildiğinde), bu noktanın
// tekrar GL_NONE durumuna döndüğünü doğrular (attach/detach durum tutarlılığı).
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    // Attachment noktasından nesneyi kaldır (detach).
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint objType = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_12, test_procedure, "Unexpected error 0x%x after detaching renderbuffer.", err);
    }
    else if (objType != GL_NONE) {
        TEST_LOG_FAIL(test_case_12, test_procedure, "Expected GL_NONE after detaching renderbuffer, got 0x%x.", objType);
    }
    else {
        TEST_LOG_SUCCESS(test_case_12, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Sınır/uç değer testi:
// attachment ve pname parametreleri GLenum (32 bit işaretsiz tamsayı) olduğundan,
// bu parametrelerin alabileceği kritik sınır/uç değerler tek bir test içinde
// art arda denenir:
// 0 -> tanımlı hiçbir enum ile eşleşmeyen, kesinlikle geçersiz değer
// 1 -> tanımlı hiçbir enum ile eşleşmeyen, en küçük pozitif değer
// INT32_MAX -> imzalı/imzasız temsil sınırındaki üst değer
// INT32_MAX + 1 -> imzalı taşma sınırı (0x80000000)
// UINT32_MAX - 1 -> imzasız üst sınırın bir altı
// UINT32_MAX -> imzasız temsilin mutlak üst sınırı (0xFFFFFFFF)
// Her adımda implementasyonun çökmediği ve tanımsız değerler için GL_INVALID_ENUM
// ürettiği doğrulanır; amaç sınır değerlerde kararlılığı sınamaktır.
/* It failed on Windows */
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    const GLenum boundaryValues[] = {
        0x00000000u,
        0x00000001u,
        (GLuint)INT32_MAX,
        (GLuint)((uint32_t)INT32_MAX + 1u),
        (GLuint)UINT32_MAX - 1u,
        (GLuint)UINT32_MAX
    };
    const size_t boundaryCount = sizeof(boundaryValues) / sizeof(boundaryValues[0]);

    int failedIndex = -1;

    // attachment parametresi için sınır değerleri denenir (pname sabit geçerli tutulur).
    for (size_t i = 0; i < boundaryCount; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        GLint params = -1;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, boundaryValues[i], GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);
        GLenum err = glGetError();

        if (err != GL_INVALID_ENUM) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_INVALID_ENUM for boundary attachment value 0x%x, got 0x%x.", boundaryValues[i], err);
            break;
        }
    }

    // pname parametresi için sınır değerleri denenir (attachment sabit geçerli tutulur).
    if (failedIndex == -1) {
        for (size_t i = 0; i < boundaryCount; ++i) {
            while (glGetError() != GL_NO_ERROR) {}

            GLint params = -1;
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, boundaryValues[i], &params);
            GLenum err = glGetError();

            if (err != GL_INVALID_ENUM) {
                failedIndex = (int)i;
                TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_INVALID_ENUM for boundary pname value 0x%x, got 0x%x.", boundaryValues[i], err);
                break;
            }
        }
    }

    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_13, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Initialization */
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_008();  /* It crashed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_012();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GFAP_ROBUSTNESS_TC_013();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_GFAP_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_GFAP_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}