//Gizem'de çalışması için
#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"

/*
GL20SC - FramebufferObjects - BindRenderbuffer - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_FO_BR_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_BR_ROBUSTNESS_TC_010";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_RENDERBUFFER.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindRenderbuffer((GLenum)0xFFFFFFFF, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%x.", err);
        return;
    }
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

// belirtilmeyen hatalar ------------------------------

// target parametresi GL_RENDERBUFFER dışında bir enum olduğunda GL_INVALID_ENUM üretilip
// önceki binding'in bozulmadığını doğrular.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glBindRenderbuffer(GL_FRAMEBUFFER, rbo); // gecersiz target

    GLenum err = glGetError();

    GLint currentBinding = -1;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &currentBinding);

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM for invalid target, got 0x%x.", err);
    }
    else if ((GLuint)currentBinding != rbo) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Previous binding was altered by invalid call (binding=%d, expected=%u)\n", currentBinding, rbo);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// glGenRenderbuffers ile hiç üretilmemiş, rastgele/keyfi bir renderbuffer ismiyle bind
// çağrıldığında implementasyonun çökmemiş olduğunu ve tanımsız davranış sergilemediğini test eder.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint arbitraryName = 0xABCD1234;
    glBindRenderbuffer(GL_RENDERBUFFER, arbitraryName);

    GLenum err = glGetError();
    GLboolean isRbo = glIsRenderbuffer(arbitraryName);
    (void)isRbo;

    if (err == GL_NO_ERROR || err == GL_INVALID_OPERATION || err == GL_INVALID_VALUE) {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Implementation crashed or returned unexpected error (0x%x) with arbitrary/ungenerated name", err);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// renderbuffer == 0 bağlıyken, target GL_RENDERBUFFER üzerinde state sorgulayan/değiştiren bir çağrı
// (örneğin glRenderbufferStorage) GL_INVALID_OPERATION hatası üretmeli.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_004()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_OPERATION when modifying target with renderbuffer 0 bound, got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

// Yeni oluşturulup ilk kez bind edilen bir renderbuffer'in başlangıç durumunun
// spesifikasyona uygun olduğunu (format GL_RGBA4, boyut 0x0) doğrular.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); // first bind

    GLint width = -1, height = -1, internalFormat = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &internalFormat);

    if (width != 0 || height != 0) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected zero-sized renderbuffer on first bind, got %dx%d", width, height);
    }
    else if (internalFormat != GL_RGBA4) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected initial internal format GL_RGBA4, got 0x%x", internalFormat);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Farklı bir renderbuffer bind edildiğinde önceki bağlamanın otomatik olarak
// kırılıp GL_RENDERBUFFER_BINDING'in yeni isme güncellendiğini doğrular.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo1, rbo2;
    glGenRenderbuffers(1, &rbo1);
    glGenRenderbuffers(1, &rbo2);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo1);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo2);

    GLint currentBinding = -1;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &currentBinding);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Unexpected error 0x%x during rebind sequence.", err);
    }
    else if ((GLuint)currentBinding != rbo2) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Expected active binding %u after rebind, got %d", rbo2, currentBinding);
    }
    else {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// renderbuffer parametresi olarak büyük/geçersiz (garbage) bir değer verildiğinde
// implementasyonun çökmenden güvenli şekilde davranmasını test eder.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint garbageNames[] = { 0xFFFFFFFF, 0x7FFFFFFF, 0x00000000, 0x12345678, 0xDEADBEEF };
    int crashed = 0;

    for (size_t i = 0; i < sizeof(garbageNames) / sizeof(garbageNames[0]); ++i) {
        glBindRenderbuffer(GL_RENDERBUFFER, garbageNames[i]);
        glGetError(); // hata ne olursa olsun sadece çökme/çökmeme önemli, temizle
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glGetError();
    }

    if (crashed) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Implementation crashed while binding garbage renderbuffer names.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
}

// Aynı renderbuffer isminin ardışık, gereksiz yere tekrar tekrar bind edilmesinin
// (aynı objeye rebind) herhangi bir hataya veya durum bozulmasına yol açmadığını doğrular.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);

    for (int i = 0; i < 1000; ++i) {
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    }

    GLenum err = glGetError();
    GLint currentBinding = -1;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &currentBinding);

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Unexpected error 0x%x after repeated rebind of same renderbuffer.", err);
    }
    else if ((GLuint)currentBinding != rbo) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Binding state corrupted after repeated rebind (binding=%d, expected=%u)", currentBinding, rbo);
    }
    else {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// glGenRenderbuffers ile sadece "reserve" edilmiş (henüz hiç bind edilmemiş) bir isim
// glIsRenderbuffer ile sorgulandığında GL_FALSE dönmesi beklenir; bind sonrası GL_TRUE olmalı.
// Bu, "name generated but not yet bound" durumunun doğru ayırt edildiğini test eder.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);

    GLboolean isRboBeforeBind = glIsRenderbuffer(rbo);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    GLboolean isRboAfterBind = glIsRenderbuffer(rbo);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Unexpected error 0x%x during generate/bind sequence.", err);
    }
    else if (isRboBeforeBind != GL_FALSE) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "glIsRenderbuffer returned TRUE before first bind (name only reserved).");
    }
    else if (isRboAfterBind != GL_TRUE) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "glIsRenderbuffer returned FALSE after bind, object should now exist.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// SINIR/UÇ DEĞER TESTİ:
// renderbuffer parametresi bir GLuint (32 bit işaretsiz tamsayı) olduğundan, bu parametrenin
// alabileceği tüm kritik sınır/uç değerler tek bir test içinde art arda denenir:
// 0 -> reserved değer, "unbind" anlamına gelir, geçerli olmalı
// 1 -> en küçük geçerli isim adayı (üretilmemiş olsa da implementasyon çökmemeli)
// INT32_MAX -> imzalı/imzasız temsil sınırındaki üst değer
// INT32_MAX + 1 -> imzalı taşma sınırı (0x80000000)
// UINT32_MAX - 1-> imzasız üst sınırın bir altı
// UINT32_MAX -> imzasız temsilin mutlak üst sınırı (0xFFFFFFFF)
// Her adımda implementasyonun çökmediği ve ardından güvenli şekilde 0'a (unbind) dönülebildiği
// doğrulanır. Amaç spesifik bir hata kodu beklemek değil, sınır değerlerde kararlılığı sınamaktır.
void GS_GL20SC_FO_BR_ROBUSTNESS_TC_010()
{
    const GLuint boundaryValues[] = {
    0x00000000u, // 0 - reserved / unbind
    0x00000001u, // 1 - en küçük olası (üretilmemiş) isim
    0x7FFFFFFFu, // INT32_MAX
    0x80000000u, // INT32_MAX + 1 (imzalı taşma sınırı)
    0xFFFFFFFEu, // UINT32_MAX - 1
    0xFFFFFFFFu // UINT32_MAX (mutlak üst sınır)
    };

    const size_t boundaryCount = sizeof(boundaryValues) / sizeof(boundaryValues[0]);

    int failedIndex = -1;

    for (size_t i = 0; i < boundaryCount; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        glBindRenderbuffer(GL_RENDERBUFFER, boundaryValues[i]);
        GLenum err = glGetError();

        // 0 değeri için GL_NO_ERROR kesinlikle beklenir; diğer sınır değerlerde
        // implementasyonun çökmemesi ve öngörülebilir bir hata döndürmesi yeterlidir.
        if (boundaryValues[i] == 0 && err != GL_NO_ERROR) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_10, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%x.", err);
            return;
        }
        if (err != GL_NO_ERROR && err != GL_INVALID_OPERATION && err != GL_INVALID_VALUE) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_10, test_procedure, "Unexpected/undefined error code 0x%x for boundary value 0x%x", err, boundaryValues[i]);
            return;
        }

        // Her denemeden sonra güvenli bir referans noktasına (0) dönülerek
        // bir sonraki sınır değerin önceki durumdan etkilenmemesi sağlanır.
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glGetError();
    }

    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_10, test_procedure, "failedIndex != -1");
    }
}


/* Initialization */
void GS_GL20SC_FO_BR_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BR_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_BR_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_BR_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}