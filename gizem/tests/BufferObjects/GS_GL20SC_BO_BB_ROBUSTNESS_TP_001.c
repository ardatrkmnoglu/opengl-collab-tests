#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../../include/macro.h"


/*
GL20SC - BufferObjects - BindBuffer - ROBUSTNESS
*/


static const char* test_procedure = "GS_GL20SC_BO_BB_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_BO_BB_ROBUSTNESS_TC_010";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not one of the allowable values.
 void GS_GL20SC_BO_BB_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(0xFFFFFFFF, 1);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%X\n", err);
    }
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}


// belirtilmeyen hatalar ------------------------


// glGenBuffers ile oluşturulmamış bir ismin bind edilmesi
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint name = 424242;
    glBindBuffer(GL_ARRAY_BUFFER, name);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    else {
         TEST_LOG_FAIL(test_case_2, test_procedure, "error = 0x%x.", err);
    }
}

// Aynı buffer adı ile tekrar glBindBuffer çağırınca GL error oluşup oluşmadığını test eder.
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_003()
 {
     while (glGetError() != GL_NO_ERROR) {}

     GLuint buf = 0;

     glGenBuffers(1, &buf);
     glBindBuffer(GL_ARRAY_BUFFER, buf);

     // Aynı isim tekrar bağlanıyor
     glBindBuffer(GL_ARRAY_BUFFER, buf);

     GLenum err = glGetError();
     if (err == GL_NO_ERROR) {
         TEST_LOG_SUCCESS(test_case_3, test_procedure);
     }
     else {
         TEST_LOG_FAIL(test_case_3, test_procedure, "error = 0x%x.", err);
     }
 }

// Büyük/alışılmadık buffer isimlerinin bind edilmesi
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_004()
{
    GLuint candidates[] = {
        0xFFFFFFFFu,   // UINT_MAX
        0x80000000u,   // sign-bit sınırı
        0x7FFFFFFFu,   // INT_MAX
        0xDEADBEEFu,
        0xCDCDCDCDu    // tipik uninitialized heap pattern
        };

    for (int i = 0; i < 5; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        glBindBuffer(GL_ARRAY_BUFFER, candidates[i]);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_4, test_procedure, "Error = 0x%x.", err);
            return;
        }
    }
     TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

// Geçersiz target enum değerlerine karşı implementasyonun hata kontrolünün testi
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLenum polluted = GL_ARRAY_BUFFER | 0xFFFF0000u;
    glBindBuffer(polluted, 1);

    // Spec'e göre bu "allowable değil" -> INVALID_ENUM beklenir
    GLenum err = glGetError();
     if (err == GL_INVALID_ENUM) {
         TEST_LOG_SUCCESS(test_case_5, test_procedure);
     }
     else {
         TEST_LOG_FAIL(test_case_5, test_procedure, "expected GL_INVALID_ENUM but got = 0x%x.", err);
     }
}

// Aynı buffer nesnesinin farklı target'lara hızlı ve tekrarlı şekilde
// bağlanması sırasında implementasyonun kararlılığını test eder.
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);

    for (int i = 0; i < 5000; ++i)
    {
        GLenum target = (i % 2 == 0) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
        glBindBuffer(target, buf);
        glBufferData(target, (i % 7) * 37, NULL, GL_STATIC_DRAW);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_6, test_procedure, "error = 0x%x.", err);
            return;
        }
    }
     TEST_LOG_SUCCESS(test_case_6, test_procedure);
}


// Buffer'ı tekrar tekrar 0'a bağlayıp bağlama durumunu sorgulayarak
// implementasyonun state yönetimi kararlılığını test eder.
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    for (int i = 0; i < 1000; ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        GLint binding = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding);

        if (binding != 0) {
            TEST_LOG_FAIL(test_case_7, test_procedure, "GL_ARRAY_BUFFER_BINDING=%d (expected 0)", binding);
            return;
        }

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_7, test_procedure, "error = 0x%x.", err);
            return; }
    }
     TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

// Çok sayıda buffer ismi üzerinde rastgele bind işlemleri yaparak implementasyonun
// isim yönetimi ve durum değişikliklerine karşı dayanıklılığını test eder.
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int N = 20000;
    GLuint *names = (GLuint *)malloc(sizeof(GLuint) * N);

    if (names == NULL) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Memory allocation failed");
        return;
    }

    glGenBuffers(N, names);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "glGenBuffers failed: error = 0x%x.", err);
        free(names);
        return;
    }

    uint32_t seed = 0x1234567u;

    for (int i = 0; i < N; ++i)
    {
        seed ^= seed << 13;
        seed ^= seed >> 17;
        seed ^= seed << 5;
        GLuint name = names[seed % N];
        seed ^= seed << 13;
        seed ^= seed >> 17;
        seed ^= seed << 5;
        GLenum target = (seed & 1) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

        glBindBuffer(target, name);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
            free(names);
            return;
        }
    }
    free(names);

     TEST_LOG_SUCCESS(test_case_8, test_procedure);
}

// Aynı target üzerinde farklı buffer'lar arasında sürekli geçiş yaparak
// implementasyonun state yönetimi kararlılığını test eder.
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffers[2];
    glGenBuffers(2, buffers);

    for (int i = 0; i < 10000; ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_9, test_procedure, "error = 0x%x.", err);
            return;
        }
    }
     TEST_LOG_SUCCESS(test_case_9, test_procedure);
}

// Buffer nesnelerinin oluşturma, bağlama ve silme yaşam döngüsünü tekrarlı
// olarak çalıştırarak implementasyonun dayanıklılığını test eder.
void GS_GL20SC_BO_BB_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    for (int i = 0; i < 5000; ++i)
    {
        GLuint buf;
        glGenBuffers(1, &buf);
        glBindBuffer(GL_ARRAY_BUFFER, buf);
        glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_10, test_procedure, "Iteration=%d, glError=0x%X",i, err);
            return;
        }
    }
     TEST_LOG_SUCCESS(test_case_10, test_procedure);
}

/* Initialization */
void GS_GL20SC_BO_BB_ROBUSTNESS_TP_001_init(void) {
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_001();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_002();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_003();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_004();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_005();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_006();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_007();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_008();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_009();
     CHECK_ERROR(test_procedure);
     GS_GL20SC_BO_BB_ROBUSTNESS_TC_010();
     CHECK_ERROR(test_procedure);
 }

void GS_GL20SC_BO_BB_ROBUSTNESS_TP_001_draw(void) {

 }

/* Cleanup */
void GS_GL20SC_BO_BB_ROBUSTNESS_TP_001_close(void) {
     CHECK_ERROR(test_procedure);
 }
