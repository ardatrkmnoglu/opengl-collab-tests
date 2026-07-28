//Gizem'de çalışması için
#include <glad/gles2.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"


/*
GL20SC - FramebufferObjects - GenRenderbuffers - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_FO_GR_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_011";
static const char* test_case_12 = "GS_GL20SC_FO_GR_ROBUSTNESS_TC_012";


// Belirtilen hata: GL_INVALID_VALUE is generated if n is negative.
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 12345;
    glGenRenderbuffers(-1, &rbo);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_VALUE, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}


// Belirtilmeyen hatalar ------------------------------


// n negatif olduğunda, hem hatanın üretildiğini hem de çıktı dizisinin (renderbuffers)
// hiçbir şekilde değiştirilmediğini doğrular (yan etki kontrolü).
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint sentinel = 0xDEADBEEF;
    GLuint rbo = sentinel;

    glGenRenderbuffers(-5, &rbo);

    GLenum err = glGetError();

    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_VALUE for negative n, got 0x%x.", err);
    }
    else if (rbo != sentinel) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Output array was modified despite invalid n (value=0x%x, expected sentinel=0x%x).", rbo, sentinel);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
}

// renderbuffers parametresi olarak NULL pointer verildiğinde implementasyonun
// çökmediğini (segfault üretmediğini) doğrular.
/* It crashed on Windows */
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    glGenRenderbuffers(1, NULL);

    GLenum err = glGetError();
    (void)err;

    // Buraya kadar gelinmişse implementasyon NULL pointer ile çökmemiştir.
    TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

// n = 0 verildiğinde herhangi bir hata üretilmemesi ve çıktı dizisinin
// değiştirilmemesi gerektiğini doğrular.
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint sentinel = 0x11223344;
    GLuint rbo = sentinel;

    glGenRenderbuffers(0, &rbo);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_NO_ERROR for n=0, got 0x%x.", err);
    }
    else if (rbo != sentinel) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Output array was modified despite n=0 (value=0x%x, expected sentinel=0x%x).", rbo, sentinel);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
}

// Üretilen isimlerin sıfır olmadığını ve benzersiz olduğunu doğrular.
// Spesifikasyon, üretilen isimlerin çağrı öncesinde kullanımda olmadığını garanti eder.
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int count = 16;
    GLuint names[16] = { 0 };

    glGenRenderbuffers(count, names);

    GLenum err = glGetError();
    int valid = 1;

    for (int i = 0; i < count; ++i) {
        if (names[i] == 0) {
            valid = 0;
            break;
        }
        for (int j = i + 1; j < count; ++j) {
            if (names[i] == names[j]) {
                valid = 0;
                break;
            }
        }
        if (!valid) break; // valid == 0 ise break
    }

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Unexpected error 0x%x during valid glGenRenderbuffers call.", err);
    }
    else if (!valid) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Generated names contain zero or duplicate values.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
}

// Çok büyük bir n değeriyle çağrıldığında implementasyonun çökmediğini,
// bellek yetersizliği durumunda dahi güvenli davrandığını doğrular (stres testi).
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int hugeCount = 1000000;
    GLuint* names = (GLuint*)malloc(sizeof(GLuint) * hugeCount);

    if (names == NULL) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Test setup failed: could not allocate host memory for stress test.");
        return;
    }

    glGenRenderbuffers(hugeCount, names);

    GLenum err = glGetError();
    (void)err;

    // Buraya kadar gelinmişse implementasyon çok büyük n değeriyle çökmemiştir.
    TEST_LOG_SUCCESS(test_case_6, test_procedure);

    free(names);
}

// Sınır/uç değer testi:
// n parametresi GLsizei (imzalı 32 bit tamsayı) olduğundan, bu parametrenin alabileceği
// kritik sınır/uç değerler tek bir test içinde art arda denenir:
// INT_MIN -> mutlak alt sınır, kesinlikle geçersiz
// -1 -> en yaygın geçersiz negatif değer
// 0 -> geçerli, işlemsiz (no-op) sınır durumu
// 1 -> en küçük geçerli pozitif değer
// INT_MAX -> mutlak üst sınır (gerçekte tahsis edilemeyecek kadar büyük, ancak
// implementasyonun çökmeden bir hata ile bu durumu ele alması beklenir)
// Her adımda implementasyonun çökmediği ve negatif değerler için GL_INVALID_VALUE
// ürettiği doğrulanır; amaç sınır değerlerde kararlılığı ve doğru hata sınıflandırmasını sınamaktır.
/* It crashed on Windows */
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_007()
{
    const GLsizei boundaryValues[] = {INT_MIN, -1, 0, 1, INT_MAX};
    const size_t boundaryCount = sizeof(boundaryValues) / sizeof(boundaryValues[0]);

    int failedIndex = -1;
    GLuint dummy = 0;
    
    for (size_t i = 0; i < boundaryCount; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        // INT_MAX gibi tahsis edilemeyecek büyüklükte değerler için gerçek bir dizi
        // ayırmak yerine, implementasyonun n kontrolünü tahsisten önce yapıp yapmadığını
        // sınamak amacıyla küçük bir buffer kasıtlı olarak kullanılmaktadır.
        dummy = 0;
        glGenRenderbuffers(boundaryValues[i], &dummy);

        GLenum err = glGetError();

        if (boundaryValues[i] < 0 && err != GL_INVALID_VALUE) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_7, test_procedure, "Expected GL_INVALID_VALUE for n=%d, got 0x%x.", boundaryValues[i], err);
            break;
        }
    }
    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
}

// renderbuffers parametresi olarak hizasız (unaligned) bir bellek adresi verildiğinde
// implementasyonun çökmediğini doğrular. Bazı platformlarda hizasız erişim veri
// yapılarının okunmasında/yazılmasında sorun yaratabilir.
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    // 1 bayt kaydırılarak GLuint için hizasız bir adres elde edilir.
    unsigned char rawBuffer[sizeof(GLuint) * 4 + 1] = { 0 };
    GLuint* unalignedPtr = (GLuint*)(rawBuffer + 1);

    glGenRenderbuffers(1, unalignedPtr);

    GLenum err = glGetError();
    (void)err;

    // Buraya kadar gelinmişse implementasyon hizasız pointer ile çökmemiştir.
    TEST_LOG_SUCCESS(test_case_8, test_procedure);

    GLuint alignedCopy;
    memcpy(&alignedCopy, unalignedPtr, sizeof(GLuint));
}

// Spesifikasyona göre, üretilen bir renderbuffer ismi ilk kez glBindRenderbuffer ile
// bağlanana kadar hiçbir renderbuffer nesnesiyle ilişkilendirilmemiştir. Bu nedenle
// glIsRenderbuffer, henüz bağlanmamış üretilmiş bir isim için GL_FALSE döndürmelidir.
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);

    GLboolean isRbo = glIsRenderbuffer(rbo);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Unexpected error 0x%x while querying unbound generated name.", err);
    }
    else if (isRbo != GL_FALSE) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Expected GL_FALSE for a generated but never-bound renderbuffer name, got GL_TRUE.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
}

// glGenRenderbuffers çağrısının, o an bağlı olan renderbuffer binding durumunu
// hiçbir şekilde değiştirmediğini doğrular. İsim üretmek yan etki olarak
// mevcut context durumunu bozmamalıdır.
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint boundRbo = 0;
    glGenRenderbuffers(1, &boundRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, boundRbo);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint freshNames[8] = { 0 };
    glGenRenderbuffers(8, freshNames);

    GLenum err = glGetError();

    GLint currentBinding = -1;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &currentBinding);

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Unexpected error 0x%x during name generation while a renderbuffer was bound.", err);
    }
    else if ((GLuint)currentBinding != boundRbo) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Current binding was altered by glGenRenderbuffers (binding=%d, expected=%u).", currentBinding, boundRbo);
    }
    else {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Ardışık birden fazla glGenRenderbuffers çağrısının, sadece tek bir çağrı içinde değil,
// çağrılar arasında da benzersiz isimler ürettiğini doğrular (global isim alanı kontrolü).
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int batchCount = 4;
    const int batchSize = 32;
    GLuint allNames[4 * 32] = { 0 };

    for (int i = 0; i < batchCount; ++i) {
        glGenRenderbuffers(batchSize, &allNames[i * batchSize]);
    }

    GLenum err = glGetError();
    int duplicateFound = 0;

    for (int i = 0; i < batchCount * batchSize && !duplicateFound; ++i) {
        for (int j = i + 1; j < batchCount * batchSize; ++j) {
            if (allNames[i] == allNames[j]) {
                duplicateFound = 1;
                break;
            }
        }
    }

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Unexpected error 0x%x during multiple sequential gen calls.", err);
    }
    else if (duplicateFound) {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Duplicate renderbuffer name detected across separate glGenRenderbuffers calls.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
}

// renderbuffers dizisinin sınırları etrafına koruma (guard) değerleri yerleştirilerek,
// implementasyonun istenenden fazla eleman yazıp yazmadığı (buffer overflow) tespit edilir.
void GS_GL20SC_FO_GR_ROBUSTNESS_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    const GLuint guardPattern = 0xCCCCCCCC;
    const int n = 4;

    struct {
        GLuint guardBefore;
        GLuint names[4];
        GLuint guardAfter;
    } buffer;

    buffer.guardBefore = guardPattern;
    buffer.guardAfter = guardPattern;
    for (int i = 0; i < n; ++i) {
        buffer.names[i] = 0;
    }

    glGenRenderbuffers(n, buffer.names);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_12, test_procedure, "Unexpected error 0x%x during guarded buffer test.", err);
    }
    else if (buffer.guardBefore != guardPattern) {
        TEST_LOG_FAIL(test_case_12, test_procedure, "Buffer underflow detected: memory before the array was overwritten.");
    }
    else if (buffer.guardAfter != guardPattern) {
        TEST_LOG_FAIL(test_case_12, test_procedure, "Buffer overflow detected: memory after the array was overwritten.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_12, test_procedure);
    }
}

/* Initialization */
void GS_GL20SC_FO_GR_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_003(); /* It crashed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_007(); /* It crashed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GR_ROBUSTNESS_TC_012();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_GR_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_GR_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}