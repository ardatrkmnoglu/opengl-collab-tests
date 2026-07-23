#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// glGenBuffers: yeni buffer nesneleri için ID üretir
// n: Kaç tane buffer ismi üretileceği.
// buffers: Üretilen buffer ID’lerinin yazılacağı dizi.


// Belirtilen hata: GL_INVALID_VALUE is generated if n is negative
void BufferObjects_GenBuffers_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffer = 0;
    glGenBuffers(-1, &buffer);
    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glGenBuffers_invalid_value()\n");
}

// n = 0 ile çağrı
void BufferObjects_GenBuffers_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf = 0xCDCDCDCD; // sentinel değer
    glGenBuffers(0, &buf);
    GLenum err = glGetError();
    printf("[INFO] glGenBuffers(n=0): error=0x%X, buffer=0x%08X\n", err, buf);
}

// buffers = NULL, n > 0 (negative robustness)
void BufferObjects_GenBuffers_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    glGenBuffers(5, NULL);
    GLenum err = glGetError();
    printf("[INFO] glGenBuffers(buffers=nullptr, n=5): error=0x%X\n", err);
}

// Aşırı büyük n
void BufferObjects_GenBuffers_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    const GLsizei largeCount = 100000;
    GLuint *buffers = (GLuint *)malloc(sizeof(GLuint) * largeCount);
    if (buffers == NULL)
    {
        printf("[ERROR] Memory allocation failed.\n");
        return;
    }
    glGenBuffers(largeCount, buffers);
    GLenum err = glGetError();
    printf("[INFO] glGenBuffers(n=%d): error=0x%X\n", largeCount, err);
    free(buffers);
}

// Aynı array'i art arda, isim tekilliğini bozmaya çalışarak çağırma (fonksiyon 1000 kez art arda çağrıldığında hata veriyor mu)
void BufferObjects_GenBuffers_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffers[10];
    for (int i = 0; i < 1000; ++i)
    {
        glGenBuffers(10, buffers);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("[INFO] glGenBuffers failed at iteration %d: error=0x%X\n", i, err);
            return; }
    }
    printf("[PASS] Repeated glGenBuffers(10) x1000 completed successfully.\n");
}

// Çok sayıda buffer adı üreterek döndürülen isimlerin benzersiz olduğunu ve reserved 0 isminin üretilmediğini doğrular.
void BufferObjects_GenBuffers_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    const GLsizei COUNT = 1000;
    GLuint buffers[COUNT];
    glGenBuffers(COUNT, buffers);

    // 0 ismi üretilmemeli
    for (int i = 0; i < COUNT; i++) {
        if (buffers[i] == 0) {
            printf("[FAIL] glGenBuffers returned reserved name 0.\n");
            return;
        } }

    // Aynı isim iki kez üretilmemeli
    for (int i = 0; i < COUNT; i++) {
        for (int j = i + 1; j < COUNT; j++) {
            if (buffers[i] == buffers[j]) {
                printf("[FAIL] Duplicate buffer name %u found.\n", buffers[i]);
                return;
            } } }

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) printf("[PASS] All generated buffer names are unique and non-zero.\n");
    else printf("[FAIL] glGetError() = 0x%X\n", err);

    glDeleteBuffers(COUNT, buffers);
}

// Bind edilmemiş buffer isimleri üzerinde glIsBuffer ve glDeleteBuffers çağrılarının spesifikasyona uygun davranıp davranmadığını doğrular.
void BufferObjects_GenBuffers_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    GLboolean isBuffer = glIsBuffer(buf);

    glDeleteBuffers(1, &buf);

    GLenum err = glGetError();
    printf("[INFO] Unbound buffer name: glIsBuffer=%s, glDeleteBuffers error=0x%X\n", isBuffer ? "GL_TRUE" : "GL_FALSE", err);
}

// Aynı buffer isminin birden fazla kez silinmesi durumunda implementasyonun kararlılığını test eder.
void BufferObjects_GenBuffers_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glDeleteBuffers(1, &buf);
    GLenum firstErr = glGetError();
    glDeleteBuffers(1, &buf);
    GLenum secondErr = glGetError();
    printf("[INFO] Double delete: firstErr=0x%X, secondErr=0x%X\n", firstErr, secondErr);
}

// Büyük 'n' değeri ve kasıtlı olarak yetersiz output buffer kullanılarak implementasyonun geçersiz istemci belleği karşısındaki davranışı test edilir (negative robustness)
void BufferObjects_GenBuffers_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLsizei huge_n = INT_MAX; // n * sizeof(GLuint) iç hesapta overflow edebilir

    GLuint buffers[1]; // kasıtlı olarak yetersiz boyutlu tampon

    // NOT: gerçek n kadar büyük array vermiyoruz -- implementasyonun n'i gerçekten kullanıp kullanmadığını, yoksa iç limitle mi kısıtladığını
    // görmek için. Bu, çağıranın hatası olsa da spec bir üst sınır tanımlamıyor, bu yüzden implementasyonun nasıl davrandığını izliyoruz.
    glGenBuffers(huge_n, buffers);

    GLenum err = glGetError();
    printf("n=INT_MAX -> glError=0x%x\n", err);
}



