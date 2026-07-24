#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../../include/macro.h"

/*
GL20SC - BufferObjects - GenBuffers - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_BO_GB_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_BO_GB_ROBUSTNESS_TC_008";



// Belirtilen hata: GL_INVALID_VALUE is generated if n is negative
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffer = 0;
    glGenBuffers(-1, &buffer);
    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_VALUE, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
    }
}


// belirtilmeyen hatalar ------------------------


// n = 0 ile çağrı
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf = 0xCDCDCDCD; // sentinel değer
    glGenBuffers(0, &buf);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR && buf == 0xCDCDCDCD) {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_2, test_procedure, "error = 0x%x, buffer=0x%08X.", err, buf);
    }
}

// buffers = NULL, n > 0 (negative robustness)
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    glGenBuffers(5, NULL);

    GLenum err = glGetError();
    TEST_LOG_INFO("GS_GL20SC_BO_GB_ROBUSTNESS_TC_003 -- tanımsız davranış. Gözlem testidir. hata = 0x%X\n", err);
}

// Aşırı büyük n
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    const GLsizei largeCount = 100000;
    GLuint *buffers = (GLuint *)malloc(sizeof(GLuint) * largeCount);
    if (buffers == NULL)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Memory allocation failed");
        return;
    }
    glGenBuffers(largeCount, buffers);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_4, test_procedure, "error = 0x%x.", err);
    }
    free(buffers);
}

// Aynı array'i art arda, isim tekilliğini bozmaya çalışarak
// çağırma (fonksiyon 1000 kez art arda çağrıldığında hata veriyor mu)
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffers[10];
    for (int i = 0; i < 1000; ++i)
    {
        glGenBuffers(10, buffers);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_5, test_procedure, "error = 0x%x.", err);
            return;
        }
    }
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

// Çok sayıda buffer adı üreterek döndürülen isimlerin benzersiz
// olduğunu ve reserved 0 isminin üretilmediğini doğrular.
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    const GLsizei COUNT = 1000;
    GLuint buffers[COUNT];
    glGenBuffers(COUNT, buffers);

    // 0 ismi üretilmemeli
    for (int i = 0; i < COUNT; i++) {
        if (buffers[i] == 0) {
            TEST_LOG_FAIL(test_case_6, test_procedure, "glGenBuffers returned reserved name");
            return;
        } }

    // Aynı isim iki kez üretilmemeli
    for (int i = 0; i < COUNT; i++) {
        for (int j = i + 1; j < COUNT; j++) {
            if (buffers[i] == buffers[j]) {
                TEST_LOG_FAIL(test_case_6, test_procedure, "Duplicate buffer name %u found.\n", buffers[i]);
                return;
            } } }

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_6, test_procedure, "error = 0x%x.", err);
    }
}

// Bind edilmemiş buffer isimleri üzerinde glIsBuffer
// çağrısının spesifikasyona uygun davranıp davranmadığını doğrular.
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    GLboolean isBuffer = glIsBuffer(buf);

    if (isBuffer == GL_FALSE) {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_7, test_procedure, "glIsBuffer=%s\n", isBuffer ? "GL_TRUE" : "GL_FALSE");
    }
}

// Büyük 'n' değeri ve kasıtlı olarak yetersiz output buffer kullanılarak implementasyonun
// geçersiz istemci belleği karşısındaki davranışı test edilir (negative robustness)
void GS_GL20SC_BO_GB_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLsizei huge_n = INT_MAX; // n * sizeof(GLuint) iç hesapta overflow edebilir

    GLuint buffers[1]; // kasıtlı olarak yetersiz boyutlu tampon

    // NOT: gerçek n kadar büyük array vermiyoruz -- implementasyonun n'i gerçekten kullanıp kullanmadığını, yoksa iç limitle mi kısıtladığını
    // görmek için. Bu, çağıranın hatası olsa da spec bir üst sınır tanımlamıyor, bu yüzden implementasyonun nasıl davrandığını izliyoruz.
    glGenBuffers(huge_n, buffers);

    GLenum err = glGetError();
    TEST_LOG_INFO("GS_GL20SC_BO_GB_ROBUSTNESS_TC_008 - tamamen tanımsız davranış. Gözlem testidir. error=0x%x\n", err);
}


/* Initialization */
void GS_GL20SC_BO_GB_ROBUSTNESS_TP_001_init(void) {
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_001();
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_002();
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_003();
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_004();
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_005();
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_006();
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_007();
    CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GB_ROBUSTNESS_TC_008();
    CHECK_ERROR(test_procedure);
}

void GS_GL20SC_BO_GB_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_GB_ROBUSTNESS_TP_001_close(void) {
    CHECK_ERROR(test_procedure);
}
