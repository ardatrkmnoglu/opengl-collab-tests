#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


// void glBindBuffer(GLenum target, GLuint buffer);
// Bir buffer nesnesini belirli bir target'a bağlar
// Bağlandıktan sonra o hedef üzerinde yapılan işlemler artık bu buffer üzerinde gerçekleştirilir


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not one of the allowable values.
 void BufferObjects_BindBuffer_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(0xFFFFFFFF, 1);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glBindBuffer_invalid_enum()\n");
}


// belirtilmeyen hatalar ------------------------


// glGenBuffers ile oluşturulmamış bir ismin bind edilmesi
void BufferObjects_BindBuffer_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint name = 424242;
    glBindBuffer(GL_ARRAY_BUFFER, name);
    GLenum err = glGetError();
    printf("[INFO] glBindBuffer(new name=%u): error=0x%X\n", name, err);
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
    printf("[INFO] Bind deleted name: error=0x%X\n", err);
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
        printf("[INFO] Boundary buffer 0x%08X: glError=0x%X\n", candidates[i], err);    }
}

// Geçersiz target enum değerlerine karşı implementasyonun hata kontrolünün testi
void BufferObjects_BindBuffer_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLenum polluted = GL_ARRAY_BUFFER | 0xFFFF0000u;
    glBindBuffer(polluted, 1);
    GLenum err = glGetError();
    // Spec'e göre bu "allowable değil" -> INVALID_ENUM beklenir
    printf("[INFO] Polluted target=0x%08X : glError=0x%X (expected GL_INVALID_ENUM)\n", polluted, err);
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
            printf("[FAIL] Iteration=%d, target=%s, glError=0x%X\n",
                   i,
                   target == GL_ARRAY_BUFFER ? "GL_ARRAY_BUFFER" : "GL_ELEMENT_ARRAY_BUFFER",
                   err);
            glDeleteBuffers(1, &buf);
            return;
        }
    }
    printf("[PASS] Rapid cross-target rebind stress completed without OpenGL errors.\n");
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

    printf("[INFO] After deleting double-bound buffer: ARRAY_BUFFER error=0x%X, ELEMENT_ARRAY_BUFFER error=0x%X\n", arrayErr, elementErr);
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
            printf("[FAIL] Iteration=%d, GL_ARRAY_BUFFER_BINDING=%d (expected 0)\n", i, binding);
            return; }

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);
            return; }
    }
    printf("[PASS] Zero binding/query thrash completed successfully.\n");
}

// Çok sayıda buffer ismi üzerinde rastgele bind işlemleri yaparak implementasyonun
// isim yönetimi ve durum değişikliklerine karşı dayanıklılığını test eder.
void BufferObjects_BindBuffer_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int N = 20000;
    GLuint *names = (GLuint *)malloc(sizeof(GLuint) * N);

    if (names == NULL) {
        printf("[FAIL] Memory allocation failed.\n");
        return;
    }

    glGenBuffers(N, names);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("[FAIL] glGenBuffers failed: glError=0x%X\n", err);
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
            printf("[FAIL] Iteration=%d, buffer=%u, target=%s, glError=0x%X\n",
                   i, name,
                   target == GL_ARRAY_BUFFER ? "GL_ARRAY_BUFFER" : "GL_ELEMENT_ARRAY_BUFFER",
                   err);

            glDeleteBuffers(N, names);
            free(names);
            return;
        }
    }

    glDeleteBuffers(N, names);
    free(names);

    printf("[PASS] Massive buffer namespace fuzz completed without OpenGL errors.\n");
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
            printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);

            glDeleteBuffers(2, buffers);
            return;
        }
    }

    glDeleteBuffers(2, buffers);

    printf("[PASS] Binding churn stress completed without OpenGL errors.\n");
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
            printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);
            return;
        }
    }
    printf("[PASS] Buffer lifecycle stress completed without OpenGL errors.\n");
}

