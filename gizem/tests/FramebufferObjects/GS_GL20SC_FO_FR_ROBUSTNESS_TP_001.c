//Gizem'de çalışması için
#include <limits.h>
#include <glad/gles2.h>
#include <stdio.h>
#include <stdint.h>
#include "../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"


/*
GL20SC - FramebufferObjects - FramebufferRenderbuffer - ROBUSTNESS
*/


static const char* test_procedure = "GS_GL20SC_FO_FR_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_FR_ROBUSTNESS_TC_010";

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferRenderbuffer((GLenum)0xFFFFFFFF, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM for invalid target, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if renderbuffertarget
// is not GL_RENDERBUFFER and renderbuffer is not 0
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLenum)0xFFFFFFFF, rbo);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM for invalid renderbuffertarget, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if attachment is
// not an accepted attachment point.
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, (GLenum)0xFFFFFFFF, GL_RENDERBUFFER, rbo);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_ENUM for invalid attachment point, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the default
// framebuffer object name 0 is bound.
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_OPERATION with default framebuffer 0 bound, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if renderbuffer
// is neither 0 nor the name of an existing renderbuffer object.
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint nonExistentRbo = 0xABCD1234;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, nonExistentRbo);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_INVALID_OPERATION for non-existent renderbuffer name, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Belirtilmeyen hatalar ------------------------------

// renderbuffer parametresi 0 verildiğinde, önceden bağlı olan bir görüntünün
// detach edildiğini ve attachment object type'ının GL_NONE olduğunu doğrular.
/* It failed on Windows */
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_006()
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

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);

    GLenum err = glGetError();

    GLint objType = -1, objName = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &objName);

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Unexpected error 0x%x while detaching renderbuffer with renderbuffer=0.", err);
    }
    else if (objType != GL_NONE || objName != 0) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Attachment was not properly detached (objType=0x%x, objName=%d).", objType, objName);
    }
    else {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// renderbuffertarget geçersiz bir enum olduğunda, renderbuffer 0 olarak verilirse
// (spesifikasyona göre bu durumda kontrol atlanır) GL_INVALID_ENUM üretilmemesi gerektiğini
// doğrular; çünkü ilgili hata sadece renderbuffer 0'dan farklıyken tanımlanmıştır.
/* It failed on Windows */
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (GLenum)0xFFFFFFFF, 0);

    GLenum err = glGetError();
    if (err == GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "GL_INVALID_ENUM incorrectly generated for invalid renderbuffertarget while renderbuffer=0 (spec exempts this case).");
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Geçerli bir renderbuffer'ın aynı framebuffer içinde birden fazla attachment
// noktasına (örneğin hem GL_COLOR_ATTACHMENT0 hem GL_DEPTH_ATTACHMENT) eş zamanlı
// olarak bağlanabildiğini ve implementasyonun bunu çökmeden kabul ettiğini doğrular.
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    while (glGetError() != GL_NO_ERROR) {}

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    GLenum err = glGetError();

    GLint colorType = -1, depthType = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &colorType);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &depthType);

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Unexpected error 0x%x while attaching same renderbuffer to multiple points.", err);
    }
    else if (colorType != GL_RENDERBUFFER || depthType != GL_RENDERBUFFER) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Renderbuffer was not correctly attached to multiple attachment points.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Aynı attachment noktasına art arda çok sayıda farklı renderbuffer bağlanıp
// çözülmesinin (attach/detach döngüsü) implementasyonu çökertmediğini ve
// bellek/durum bozulmasına yol açmadığını doğrulayan stres testidir.
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    int failed = 0;

    for (int i = 0; i < 500; ++i) {
        GLuint rbo = 0;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 4, 4);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

        if (glGetError() != GL_NO_ERROR) {
            failed = 1;
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
    }

    if (failed) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Errors occurred during repeated attach/detach cycle stress test.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Sınır/uç değer testi:
// renderbuffer parametresi bir GLuint (32 bit işaretsiz tamsayı) olduğundan, bu parametrenin
// alabileceği kritik sınır/uç değerler tek bir test içinde art arda denenir:
// 0 -> geçerli, "detach" anlamına gelir (spesifikasyona göre kabul edilmeli)
// 1 -> en küçük olası (muhtemelen üretilmemiş) isim adayı
// INT32_MAX -> imzalı/imzasız temsil sınırındaki üst değer
// INT32_MAX + 1 -> imzalı taşma sınırı (0x80000000)
// UINT32_MAX - 1 -> imzasız üst sınırın bir altı
// UINT32_MAX -> imzasız temsilin mutlak üst sınırı (0xFFFFFFFF)
// 0 dışındaki tüm değerler var olan bir renderbuffer nesnesini temsil etmediğinden
// GL_INVALID_OPERATION üretmesi beklenir; amaç implementasyonun bu sınır değerlerde
// çökmeden, öngörülebilir ve tutarlı bir hata sınıflandırması yapmasını sınamaktır.
/* It failed on Windows */
void GS_GL20SC_FO_FR_ROBUSTNESS_TC_010()
{
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    const GLuint boundaryValues[] = {
        0x00000000u,               // 0 - gecerli, detach anlamina gelir
        0x00000001u,               // 1 - en kucuk olasi (uretilmemis) isim

        (GLuint)INT32_MAX,         // INT32_MAX
        (GLuint)((uint32_t)INT32_MAX + 1u), // INT32_MAX + 1

        (GLuint)UINT32_MAX - 1u,  // UINT32_MAX - 1
        (GLuint)UINT32_MAX        // UINT32_MAX
    };
    const size_t boundaryCount = sizeof(boundaryValues) / sizeof(boundaryValues[0]);

    int failedIndex = -1;

    for (size_t i = 0; i < boundaryCount; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, boundaryValues[i]);
        GLenum err = glGetError();

        if (boundaryValues[i] == 0 && err != GL_NO_ERROR) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_10, test_procedure, "Expected GL_NO_ERROR for renderbuffer=0 (detach), got 0x%x.", err);
            break;
        }
        else if (boundaryValues[i] != 0 && err != GL_INVALID_OPERATION) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_10, test_procedure, "Expected GL_INVALID_OPERATION for non-existent renderbuffer name 0x%x, got 0x%x.", boundaryValues[i], err);
            break;
        }
    }

    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/* Initialization */
void GS_GL20SC_FO_FR_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_006();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_007();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_FR_ROBUSTNESS_TC_010();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_FR_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_FR_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}