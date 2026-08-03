//Gizem'de çalışması için
#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"


/*
GL20SC - FramebufferObjects - GenFramebuffers - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_FO_GF_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_011";
static const char* test_case_12 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_012";
static const char* test_case_13 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_013";
static const char* test_case_14 = "GS_GL20SC_FO_GF_ROBUSTNESS_TC_014";



// Belirtilen hata: GL_INVALID_VALUE is generated if n is negative.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}
    GLuint framebuffer = 0;
    glGenFramebuffers(-1, &framebuffer);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE){
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_VALUE, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}


// belirtilmeyen hatalar ------------------------------


// n negatif olduğunda GL_INVALID_VALUE üretilip çıkış dizisinin dokunulmadan kaldığını doğrular.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint sentinel = 0xDEADBEEF;
    GLuint buf = sentinel;
    glGenFramebuffers(-1, &buf);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_VALUE for negative n, got 0x%x.", err);
    }
    else if (buf != sentinel) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Output buffer modified despite GL_INVALID_VALUE (buf=0x%X)\n", buf);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
}

// n = INT_MIN gibi aşırı negatif bir değerle (olası integer overflow'a
// karşı) implementasyonun crash olmadan hayatta kalıp kalmadığını doğrular.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf = 0x12345678;
    glGenFramebuffers(INT_MIN, &buf);

    GLenum err = glGetError();

    if (err == GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "INT_MIN accepted without error - potential overflow risk.");
    }
    else {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
}

// n = 0 ve framebuffers = NULL kombinasyonunun crash olmadan/hatasız
// geçmesi beklenir (yazılacak eleman yok).
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    glGenFramebuffers(0, NULL);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "glGenFramebuffers(0, NULL) produced unexpected error 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
}

// n > 0 iken framebuffers = NULL verildiğinde (spesifikasyon tanımsız
// bırakıyor) implementasyonun segfault yerine tutarlı davranıp
// davranmadığını gözlemler.
/* It crashed on Windows */
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    glGenFramebuffers(4, NULL);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_5, test_procedure, "error = 0x%x.", err);
    }
}

// Dangling bir pointer'a yazma denemesinin implementasyonun bellek korumasına karşı davranışını test eder.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint *freedPtr = (GLuint *)malloc(sizeof(GLuint) * 4);
    free(freedPtr); // artık geçersiz
    glGenFramebuffers(4, freedPtr);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_6, test_procedure, "error = 0x%x.", err);
    }
}

// glGenFramebuffers'in yalnızca kendisine ayrılan alana yazıp yazmadığını
// anlamak için etrafını "canary" değerlerle sardığımız bir buffer üzerinde
// taşma testi yapar. Eğer fonksiyon cb.data dışına taşıp cb.before veya
// cb.after'ı bozarsa buffer overflow var demektir.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    typedef struct {
    GLuint before;
    GLuint data[4];
    GLuint after;
    } CanaryBuf;

    CanaryBuf cb;
    cb.before = 0xCAFEBABE;
    cb.after = 0xCAFEBABE;
    memset(cb.data, 0, sizeof(cb.data));
    glGenFramebuffers(4, cb.data);

    GLenum err = glGetError();
    if (cb.before != 0xCAFEBABE || cb.after != 0xCAFEBABE) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Buffer overflow detected! before=0x%X after=0x%X\n", cb.before, cb.after);
    }
    else if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Unexpected error: 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
}

// Çok büyük n değeriyle (INT_MAX) çağırarak bellek tahsis hatalarının
// crash yerine tutarlı şekilde ele alınıp alınmadığını gözlemler.
/* It crashed the entire system on Windows */
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    const GLsizei HUGE_N = INT_MAX;
    GLuint *framebuffers = (GLuint *)malloc((size_t)HUGE_N * sizeof(GLuint));
    if (!framebuffers) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Host allocation failed before even calling GL - skipping");
        return;
    }

    glGenFramebuffers(HUGE_N, framebuffers);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
    }

    free(framebuffers);
}

// Dizinin gerçek boyutundan büyük bir n ile çağırarak stack/heap
// taşması (yanlış kullanım senaryosu) altında kararlılığı test eder.
/* It crashed on Windows */
// Fail yazdıktan sonra sistem çöküyor.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint smallArray[2]; // kasıtlı olarak küçük
    glGenFramebuffers(2, smallArray);
    GLenum err1 = glGetError();

    // KASITLI HATALI KULLANIM: n, dizinin gercek boyutundan büyük.
    glGenFramebuffers(8, smallArray);
    GLenum err2 = glGetError();

    if (err1 == GL_NO_ERROR && err2 == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_9, test_procedure, "error1 = 0x%x, error2 = 0x%x", err1, err2);
    }
}

// Aynı n ile ardışık çok sayıda glGenFramebuffers
// döngüsüyle isim havuzunun (name pool) tükenmesi/wrap-around durumunu test eder.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int ITERATIONS = 100000;
    const GLsizei BATCH = 10;
    GLuint batch[BATCH];
    int failedAt = -1;

    for (int i = 0; i < ITERATIONS; i++) {
        glGenFramebuffers(BATCH, batch);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            failedAt = i;
            break;
        }
    }
    if (failedAt >= 0) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Name generation failed at iteration");
    }
    else {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
}

// Üretilen isimlerin, hiç bind edilmeden glIsFramebuffer sorgusunun tutarlı davranıp davranmadığını doğrular.
// (spec: "no framebuffer objects are associated until first bound").
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    GLboolean isFboBeforeBind = glIsFramebuffer(fbo);

    GLenum err = glGetError();

    if (isFboBeforeBind == GL_FALSE && err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Unexpected lifecycle behavior for never-bound name");
    }
}

// Unaligned bir çıkış pointer'ıyla çağrıldığında
// implementasyonun crash olmadan davranıp davranmadığını test eder.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    unsigned char rawBuffer[64];
    memset(rawBuffer, 0xAA, sizeof(rawBuffer));
    GLuint *unaligned = (GLuint *)(rawBuffer + 1); // kasıtlı hizalama bozukluğu

    glGenFramebuffers(4, unaligned);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_12, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_12, test_procedure, "error = 0x%x.", err);
    }
}

// Farklı n değerleriyle (geçerli/geçersiz karışık) ardışık çağrılarak
// implementasyonun genel kararlılığını test eder.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLsizei testValues[] = { 1, 0, -1, 2, 100000, 0, -100 };
    int numTests = sizeof(testValues) / sizeof(testValues[0]);

    for (int i = 0; i < numTests; i++) {
        GLsizei n = testValues[i];
        GLuint *arr = (n > 0) ? (GLuint *)malloc(sizeof(GLuint) * (size_t)n) : NULL;

        glGenFramebuffers(n, arr);

        GLenum err = glGetError();
        if ((n >= 0 && err != GL_NO_ERROR) || (n<0 && err != GL_INVALID_VALUE)) {
            TEST_LOG_FAIL(test_case_13, test_procedure, "error = 0x%x.", err);
            if (arr) {
                free(arr);
            }
            return;
        }
        if (arr) {
            free(arr);
        }
    }
    TEST_LOG_SUCCESS(test_case_13, test_procedure);
}

// glGenFramebuffers ve glGenRenderbuffers'in (ve varsa glGenBuffers'in)
// aynı isim uzayını paylaşıp paylaşmadığını (paylaşmaması gerekir; her
// obje türünün kendi ad uzayı olmalı) doğrular. Farklı obje türleri
// arasında isim çarpışması implementasyonun iç handle tablosunda ciddi
// bir tasarım hatasına işaret eder.
void GS_GL20SC_FO_GF_ROBUSTNESS_TC_014()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, rb;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rb);

    // fbo ismini renderbuffer olarak, rb ismini framebuffer olarak
    // yorumlamayı dene - glIsFramebuffer/glIsRenderbuffer ile çapraz kontrol.
    GLboolean fboIsRb = glIsRenderbuffer(fbo);
    GLboolean rbIsFbo = glIsFramebuffer(rb);

    // İsimler numerik olarak örtüşebilir (ikisi de küçük sayıdan
    // başlayabilir) ama tür olarak birbirine karışmamalıdır.
    if (fboIsRb == GL_FALSE && rbIsFbo == GL_FALSE) {
        TEST_LOG_SUCCESS(test_case_14, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_14, test_procedure, "Cross-type namespace confusion detected - potential type-safety bug");
    }
}


/* Initialization */
void GS_GL20SC_FO_GF_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_005(); /* It crashed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_008(); /* It crashed the entire system on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_009(); /* It crashed and failed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_012();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_013();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_GF_ROBUSTNESS_TC_014();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_GF_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_GF_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}