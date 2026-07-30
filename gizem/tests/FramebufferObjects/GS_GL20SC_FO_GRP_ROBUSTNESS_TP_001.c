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
GL20SC - FramebufferObjects - GetRenderbufferParameteriv - ROBUSTNESS
*/


static const char* test_procedure = "GS_GL20SC_FO_GRP_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_FO_GRP_ROBUSTNESS_TC_011";

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_RENDERBUFFER.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetRenderbufferParameteriv((GLenum)0xFFFFFFFF, GL_RENDERBUFFER_WIDTH, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM for invalid target, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if pname is not GL_RENDERBUFFER_WIDTH,
// GL_RENDERBUFFER_HEIGHT, GL_RENDERBUFFER_INTERNAL_FORMAT, GL_RENDERBUFFER_RED_SIZE,
// GL_RENDERBUFFER_GREEN_SIZE, GL_RENDERBUFFER_BLUE_SIZE, GL_RENDERBUFFER_ALPHA_SIZE,
// GL_RENDERBUFFER_DEPTH_SIZE, or GL_RENDERBUFFER_STENCIL_SIZE.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, (GLenum)0xFFFFFFFF, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM for invalid pname, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved renderbuffer object name 0 is bound.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint params = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &params);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_OPERATION with reserved name 0 bound, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

// Belirtilmeyen hatalar ------------------------------

// Dokümanda açıkça belirtilen davranış: "If an error is generated, no change is made to
// the contents of params." Geçersiz bir pname ile çağrı yapıldığında params değerinin
// hiçbir şekilde değiştirilmediğini doğrular.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLint sentinel = 0x7A7A7A7A;
    GLint params = sentinel;

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, (GLenum)0xFFFFFFFF, &params);

    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_ENUM for invalid pname, got 0x%x.", err);
    }
    else if (params != sentinel) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "params was modified despite error being generated (value=0x%x, expected sentinel=0x%x).", params, sentinel);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Aynı davranış, renderbuffer 0 bağlıyken (GL_INVALID_OPERATION durumunda) da doğrulanır:
// hata üretildiğinde params değerine dokunulmamalıdır.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint sentinel = 0x5B5B5B5B;
    GLint params = sentinel;

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &params);

    GLenum err = glGetError();

    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_INVALID_OPERATION with reserved name 0 bound, got 0x%x.", err);
    }
    else if (params != sentinel) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "params was modified despite error being generated (value=0x%x, expected sentinel=0x%x).", params, sentinel);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
}

// Yeni oluşturulmuş, henüz glRenderbufferStorage çağrılmamış bir renderbuffer için
// dokümanda belirtilen tüm başlangıç değerlerinin (WIDTH=0, HEIGHT=0,
// INTERNAL_FORMAT=GL_RGBA4, RED/GREEN/BLUE/ALPHA/DEPTH/STENCIL_SIZE=0) doğru olduğunu doğrular.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    struct {
        GLenum pname;
        GLint expected;
    } checks[] = {
    { GL_RENDERBUFFER_WIDTH, 0 },
    { GL_RENDERBUFFER_HEIGHT, 0 },
    { GL_RENDERBUFFER_INTERNAL_FORMAT, GL_RGBA4 },
    { GL_RENDERBUFFER_RED_SIZE, 0 },
    { GL_RENDERBUFFER_GREEN_SIZE, 0 },
    { GL_RENDERBUFFER_BLUE_SIZE, 0 },
    { GL_RENDERBUFFER_ALPHA_SIZE, 0 },
    { GL_RENDERBUFFER_DEPTH_SIZE, 0 },
    { GL_RENDERBUFFER_STENCIL_SIZE, 0 }
    };
    const size_t checkCount = sizeof(checks) / sizeof(checks[0]);

    int failedIndex = -1;

    for (size_t i = 0; i < checkCount; ++i) {
        GLint params = -1;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, checks[i].pname, &params);
        GLenum err = glGetError();

        if (err != GL_NO_ERROR) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_6, test_procedure, "Unexpected error 0x%x while querying pname 0x%x on initial renderbuffer.", err, checks[i].pname);
            break;
        }
        else if (params != checks[i].expected) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_6, test_procedure, "Expected initial value %d for pname 0x%x, got %d.", checks[i].expected, checks[i].pname, params);
            break;
        }
    }

    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// glRenderbufferStorage çağrısından sonra GL_RENDERBUFFER_WIDTH ve GL_RENDERBUFFER_HEIGHT
// değerlerinin talep edilen boyutlarla eşleştiğini doğrular.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, 37, 91);
    while (glGetError() != GL_NO_ERROR) {}

    GLint width = -1, height = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Unexpected error 0x%x while querying width/height after storage allocation.", err);
    }
    else if (width != 37 || height != 91) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Expected width=37 height=91, got width=%d height=%d.", width, height);
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Dokümanda belirtilen not: bileşen çözünürlükleri, glRenderbufferStorage'a verilen
// internalformat'tan farklı olabilir; ancak GL_STENCIL_INDEX8 formatı için
// GL_RENDERBUFFER_STENCIL_SIZE değerinin 0'dan büyük, renk bileşenlerinin ise
// 0 olması beklenir. Bu, format'a göre doğru bileşenin raporlandığını doğrular.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, 16, 16);
    while (glGetError() != GL_NO_ERROR) {}

    GLint stencilSize = -1, redSize = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &stencilSize);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, &redSize);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Unexpected error 0x%x while querying stencil/red size for GL_STENCIL_INDEX8.", err);
    }
    else if (stencilSize <= 0) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Expected non-zero GL_RENDERBUFFER_STENCIL_SIZE for GL_STENCIL_INDEX8 format, got %d.", stencilSize);
    }
    else if (redSize != 0) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Expected zero GL_RENDERBUFFER_RED_SIZE for a stencil-only format, got %d.", redSize);
    }
    else {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// params parametresi olarak NULL pointer verildiğinde implementasyonun
// çökmediğini (segfault üretmediğini) doğrular.
/* It crashed on Windows */
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, NULL);

    GLenum err = glGetError();
    (void)err;

    // Buraya kadar gelinmişse implementasyon NULL pointer ile çökmemiştir.
    TEST_LOG_SUCCESS(test_case_9, test_procedure);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// params dizisinin sınırları etrafına guard (canary) değerleri yerleştirilerek,
// implementasyonun tek bir GLint dışına yazıp yazmadığı (buffer overflow) tespit edilir.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    const GLint guardPattern = 0xCCCCCCCC;

    struct {
        GLint guardBefore;
        GLint params;
        GLint guardAfter;
    } buffer;

    buffer.guardBefore = guardPattern;
    buffer.params = -1;
    buffer.guardAfter = guardPattern;

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &buffer.params);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Unexpected error 0x%x during guarded buffer test.", err);
    }
    else if (buffer.guardBefore != guardPattern) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Buffer underflow detected: memory before params was overwritten.");
    }
    else if (buffer.guardAfter != guardPattern) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Buffer overflow detected: memory after params was overwritten.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Sınır/uç değer testi:
// pname parametresi bir GLenum olduğundan, geçerli/geçersiz enum aralığının sınır
// değerleri tek bir test içinde art arda denenir:
// 0 -> tanımsız/geçersiz enum, GL_INVALID_ENUM beklenir
// GL_RENDERBUFFER_WIDTH -> geçerli aralığın bilinen bir üyesi
// GL_RENDERBUFFER_STENCIL_SIZE -> geçerli aralığın bilinen diğer bir üyesi
// GL_RENDERBUFFER_STENCIL_SIZE + 1 -> geçerli enumlardan hemen sonraki değer, geçersiz olmalı
// 0x7FFFFFFF -> GLenum için pratik üst sınıra yakın büyük değer
// 0xFFFFFFFF -> GLenum'un mutlak üst sınırı (imzasız 32 bit temsil)
// Her adımda implementasyonun çökmediği ve geçersiz değerler için GL_INVALID_ENUM
// ürettiği doğrulanır; amaç sınır değerlerde kararlılığı ve doğru hata sınıflandırmasını sınamaktır.
void GS_GL20SC_FO_GRP_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    struct {
        GLenum pname;
        int isValid;
    } boundaryValues[] = {
        { (GLenum)0x00000000, 0 },
        { GL_RENDERBUFFER_WIDTH, 1 },
        { GL_RENDERBUFFER_STENCIL_SIZE, 1 },
        { (GLenum)(GL_RENDERBUFFER_STENCIL_SIZE + 1), 0 },
        { (GLenum)0x7FFFFFFF, 0 },
        { (GLenum)0xFFFFFFFF, 0 }
    };
    const size_t boundaryCount = sizeof(boundaryValues) / sizeof(boundaryValues[0]);

    int failedIndex = -1;

    for (size_t i = 0; i < boundaryCount; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        GLint params = -1;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, boundaryValues[i].pname, &params);
        GLenum err = glGetError();

        if (!boundaryValues[i].isValid && err != GL_INVALID_ENUM) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_11, test_procedure, "Expected GL_INVALID_ENUM for pname 0x%x, got 0x%x.", boundaryValues[i].pname, err);
            break;
        }
        else if (boundaryValues[i].isValid && err != GL_NO_ERROR) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_11, test_procedure, "Unexpected error 0x%x for valid pname 0x%x.", err, boundaryValues[i].pname);
            break;
        }
    }

    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/* Initialization */
void GS_GL20SC_FO_GRP_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_009();  /* It crashed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GRP_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_GRP_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_GRP_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}