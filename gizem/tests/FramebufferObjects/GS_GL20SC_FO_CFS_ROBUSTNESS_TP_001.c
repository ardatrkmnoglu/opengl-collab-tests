//Gizem'de çalışması için
#include <limits.h>
#include <glad/gles2.h>
#include <stdio.h>
#include "../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"


/*
GL20SC - FramebufferObjects - CheckFramebufferStatus - ROBUSTNESS
*/


static const char* test_procedure = "GS_GL20SC_FO_CFS_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_CFS_ROBUSTNESS_TC_009";

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLenum status = glCheckFramebufferStatus((GLenum)0xFFFFFFFF);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

// Belirtilmeyen hatalar ------------------------------

// Varsayılan pencere sistemi framebuffer'ı (GL_FRAMEBUFFER_BINDING == 0) bağlıyken
// dökümanda açıkça belirtildiği gibi her zaman GL_FRAMEBUFFER_COMPLETE dönmesi gerektiğini doğrular.
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Unexpected error 0x%x while querying default framebuffer status.", err);
    }
    else if (status != GL_FRAMEBUFFER_COMPLETE) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_FRAMEBUFFER_COMPLETE for default framebuffer, got 0x%x.", status);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
}

// Hiçbir attachment eklenmemiş yeni bir framebuffer için GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
// dönmesi gerektiğini doğrular.
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Unexpected error 0x%x while querying empty framebuffer status.", err);
    }
    else if (status != GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT for empty framebuffer, got 0x%x.", status);
    }
    else {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Renk için depth-only bir format (non-color-renderable) bağlandığında
// GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT dönmesi gerektiğini doğrular.
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, rbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 16, 16);

    // depth-renderable formati kasitli olarak renk baglanti noktasina bagliyoruz.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Unexpected error 0x%x while querying mismatched attachment status.", err);
    }
    else if (status != GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT && status != GL_FRAMEBUFFER_UNSUPPORTED) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT (or GL_FRAMEBUFFER_UNSUPPORTED) for non-color-renderable color attachment, got 0x%x.", status);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Farklı boyutlarda iki renderbuffer (renk ve derinlik) bağlandığında
// GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS dönmesi gerektiğini doğrular.
/* It failed on Windows */
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0, colorRbo = 0, depthRbo = 0;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &colorRbo);
    glGenRenderbuffers(1, &depthRbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glBindRenderbuffer(GL_RENDERBUFFER, colorRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRbo);

    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 32, 32); // farkli boyut
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);

    while (glGetError() != GL_NO_ERROR) {}

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Unexpected error 0x%x while querying mismatched dimensions status.", err);
    }
    else if (status != GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS for mismatched attachment sizes, got 0x%x.", status);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Uygun boyut ve formatta bir renk eklentisi (color attachment) bağlandığında
// GL_FRAMEBUFFER_COMPLETE dönmesi gerektiğini doğrular (pozitif/geçerli durum kontrolü).
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_006()
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

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Unexpected error 0x%x while querying valid framebuffer status.", err);
    }
    else if (status != GL_FRAMEBUFFER_COMPLETE) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Expected GL_FRAMEBUFFER_COMPLETE for valid color attachment, got 0x%x.", status);
    }
    else {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Framebuffer eksiksizken (complete) rendering komutlarının hatasız çalıştığını,
// eksik/tamamlanmamış (incomplete) haldeyken ise glClear gibi bir rendering komutunun
// GL_INVALID_FRAMEBUFFER_OPERATION ürettiğini doğrular.
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    // Hicbir attachment eklenmedigi icin framebuffer eksik/tamamlanmamis olmali.
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    while (glGetError() != GL_NO_ERROR) {}

    glClear(GL_COLOR_BUFFER_BIT);

    GLenum err = glGetError();

    if (status == GL_FRAMEBUFFER_COMPLETE) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Test setup invalid: framebuffer unexpectedly reported as complete before rendering attempt.");
    }
    else if (err != GL_INVALID_FRAMEBUFFER_OPERATION) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Expected GL_INVALID_FRAMEBUFFER_OPERATION when rendering to an incomplete framebuffer, got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Geçersiz target ile yapılan çağrının, o an bağlı framebuffer'ın binding durumunu
// bozmadığını doğrular (yan etki kontrolü).
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    glCheckFramebufferStatus((GLenum)0xFFFFFFFF); // gecersiz target

    GLenum err = glGetError();

    GLint currentBinding = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBinding);

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Expected GL_INVALID_ENUM for invalid target, got 0x%x.", err);
    }
    else if ((GLuint)currentBinding != fbo) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Previous binding was altered by invalid call (binding=%d, expected=%u).", currentBinding, fbo);
    }
    else {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Aynı framebuffer üzerinde art arda çok sayıda (binlerce) durum sorgusu yapılmasının
// implementasyonu çökertmediğini ve tutarlı sonuç döndürdüğünü doğrular (stres testi).
void GS_GL20SC_FO_CFS_ROBUSTNESS_TC_009()
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

    GLenum lastStatus = 0;
    int inconsistent = 0;

    for (int i = 0; i < 5000; ++i) {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (i > 0 && status != lastStatus) {
            inconsistent = 1;
            break;
        }
        lastStatus = status;
    }

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Unexpected error 0x%x during repeated status query stress test.", err);
    }
    else if (inconsistent) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Repeated queries on an unmodified framebuffer returned inconsistent status values.");
    }
    else if (lastStatus != GL_FRAMEBUFFER_COMPLETE) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Expected GL_FRAMEBUFFER_COMPLETE across repeated queries, got 0x%x.", lastStatus);
    }
    else {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/* Initialization */
void GS_GL20SC_FO_CFS_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_005();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_CFS_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_CFS_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_CFS_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}