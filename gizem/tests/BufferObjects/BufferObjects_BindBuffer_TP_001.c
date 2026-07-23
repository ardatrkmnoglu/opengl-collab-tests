#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../../include/macro.h"

// void glBindBuffer(GLenum target, GLuint buffer);
// Bir buffer nesnesini belirli bir target'a bağlar
// Bağlandıktan sonra o hedef üzerinde yapılan işlemler artık bu buffer üzerinde gerçekleştirilir


static const char* test_procedure = "BufferObjects_BindBuffer_TP_001";
static const char* test_case_1 = "BufferObjects_BindBuffer_TC_001";
static const char* test_case_2 = "BufferObjects_BindBuffer_TC_002";
static const char* test_case_3 = "BufferObjects_BindBuffer_TC_003";
static const char* test_case_4 = "BufferObjects_BindBuffer_TC_004";
static const char* test_case_5 = "BufferObjects_BindBuffer_TC_005";
static const char* test_case_6 = "BufferObjects_BindBuffer_TC_006";
static const char* test_case_7 = "BufferObjects_BindBuffer_TC_007";
static const char* test_case_8 = "BufferObjects_BindBuffer_TC_008";
static const char* test_case_9 = "BufferObjects_BindBuffer_TC_009";
static const char* test_case_10 = "BufferObjects_BindBuffer_TC_010";
static const char* test_case_11 = "BufferObjects_BindBuffer_TC_011";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not one of the allowable values.
 void BufferObjects_BindBuffer_TC_001()
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
void BufferObjects_BindBuffer_TC_002()
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

// Silinen bir buffer isminin tekrar bind edilmesiyle yeni bir buffer
// nesnesi oluşturulup oluşturulmadığını test eder.
void BufferObjects_BindBuffer_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glDeleteBuffers(1, &buf);
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
void BufferObjects_BindBuffer_TC_004()
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
void BufferObjects_BindBuffer_TC_005()
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
void BufferObjects_BindBuffer_TC_006()
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
            glDeleteBuffers(1, &buf);
            return;
        }
    }
     TEST_LOG_SUCCESS(test_case_6, test_procedure);
    glDeleteBuffers(1, &buf);
}

// Aynı buffer nesnesi iki target'a bağlıyken silme işlemi sonrası
// implementasyonun kararlılığını ve hata davranışını test eder
void BufferObjects_BindBuffer_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf); // aynı obje şimdi iki target'ta aktif
    glDeleteBuffers(1, &buf);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 64, NULL);
    GLenum arrayErr = glGetError();

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 64, NULL);
    GLenum elementErr = glGetError();

     if (arrayErr == GL_INVALID_OPERATION && elementErr == GL_INVALID_OPERATION){
         TEST_LOG_SUCCESS(test_case_7, test_procedure);
     }
     else {
         TEST_LOG_FAIL(test_case_7, test_procedure,
             "(ARRAY_BUFFER=0x%X, ELEMENT_ARRAY_BUFFER=0x%X)\n", arrayErr, elementErr );
     }
}

// Buffer'ı tekrar tekrar 0'a bağlayıp bağlama durumunu sorgulayarak
// implementasyonun state yönetimi kararlılığını test eder.
void BufferObjects_BindBuffer_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    for (int i = 0; i < 1000; ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        GLint binding = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding);

        if (binding != 0) {
            TEST_LOG_FAIL(test_case_8, test_procedure, "GL_ARRAY_BUFFER_BINDING=%d (expected 0)", binding);
            return;
        }

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
            return; }
    }
     TEST_LOG_SUCCESS(test_case_8, test_procedure);
}

// Çok sayıda buffer ismi üzerinde rastgele bind işlemleri yaparak implementasyonun
// isim yönetimi ve durum değişikliklerine karşı dayanıklılığını test eder.
void BufferObjects_BindBuffer_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int N = 20000;
    GLuint *names = (GLuint *)malloc(sizeof(GLuint) * N);

    if (names == NULL) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Memory allocation failed");
        return;
    }

    glGenBuffers(N, names);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "glGenBuffers failed: error = 0x%x.", err);
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
            TEST_LOG_FAIL(test_case_9, test_procedure, "error = 0x%x.", err);
            glDeleteBuffers(N, names);
            free(names);
            return;
        }
    }

    glDeleteBuffers(N, names);
    free(names);

     TEST_LOG_SUCCESS(test_case_9, test_procedure);
}

// Aynı target üzerinde farklı buffer'lar arasında sürekli geçiş yaparak
// implementasyonun state yönetimi kararlılığını test eder.
void BufferObjects_BindBuffer_TC_010()
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
            TEST_LOG_FAIL(test_case_10, test_procedure, "error = 0x%x.", err);
            glDeleteBuffers(2, buffers);
            return;
        }
    }

    glDeleteBuffers(2, buffers);
     TEST_LOG_SUCCESS(test_case_10, test_procedure);
}

// Buffer nesnelerinin oluşturma, bağlama ve silme yaşam döngüsünü tekrarlı
// olarak çalıştırarak implementasyonun dayanıklılığını test eder.
void BufferObjects_BindBuffer_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    for (int i = 0; i < 5000; ++i)
    {
        GLuint buf;
        glGenBuffers(1, &buf);
        glBindBuffer(GL_ARRAY_BUFFER, buf);
        glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
        glDeleteBuffers(1, &buf);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_11, test_procedure, "Iteration=%d, glError=0x%X",i, err);
            return;
        }
    }
     TEST_LOG_SUCCESS(test_case_11, test_procedure);
}

