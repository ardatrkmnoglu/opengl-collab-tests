#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/macro.h"

// void glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
// Target parametresiyle bind ettiğin buffer object için ekran kartı (GPU) üzerinde yeni bir data store oluşturur
// Eski veriyi tamamen siler
// İstersen verdiğin data pointer’ındaki veriyi bu yeni belleğe kopyalayarak başlatır
// usage parametresiyle de bu veriyi nasıl kullanacağını sürücüye ipucu olarak bildirirsin (performans optimizasyonu için)

static const char* test_procedure = "BufferObjects_BufferData_TP_001";
static const char* test_case_1 = "BufferObjects_BufferData_TC_001";
static const char* test_case_2 = "BufferObjects_BufferData_TC_002";
static const char* test_case_3 = "BufferObjects_BufferData_TC_003";
static const char* test_case_4 = "BufferObjects_BufferData_TC_004";
static const char* test_case_5 = "BufferObjects_BufferData_TC_005";
static const char* test_case_6 = "BufferObjects_BufferData_TC_006";
static const char* test_case_7 = "BufferObjects_BufferData_TC_007";
static const char* test_case_8 = "BufferObjects_BufferData_TC_008";
static const char* test_case_9 = "BufferObjects_BufferData_TC_009";
static const char* test_case_10 = "BufferObjects_BufferData_TC_010";
static const char* test_case_11 = "BufferObjects_BufferData_TC_011";
static const char* test_case_12 = "BufferObjects_BufferData_TC_012";
static const char* test_case_13 = "BufferObjects_BufferData_TC_013";
static const char* test_case_14 = "BufferObjects_BufferData_TC_014";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void BufferObjects_BufferData_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBufferData(0xFFFFFFFF, 16, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%X", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
    }
}

// Belirtilen hata: GL_INVALID_ENUM is generated if usage is not GL_STREAM_DRAW, GL_STATIC_DRAW, or GL_DYNAMIC_DRAW.
void BufferObjects_BufferData_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 16, NULL, 0xFFFFFFFF);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_2, test_procedure);

    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if size is negative.
void BufferObjects_BufferData_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, -1, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_VALUE, but got 0x%x.", err);
        glDeleteBuffers(1, &buffer);
        return;
    }
    TEST_LOG_SUCCESS(test_case_3, test_procedure);
    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
void BufferObjects_BufferData_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferData(GL_ARRAY_BUFFER, 16, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_OPERATION, but got = 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
}

// Belirtilen hata: GL_OUT_OF_MEMORY is generated if the GL is unable to create a data store with the specified size.
void BufferObjects_BufferData_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)INTPTR_MAX, NULL, GL_STATIC_DRAW );

    GLenum err = glGetError();
    if (err == GL_OUT_OF_MEMORY) {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    else if (err == GL_NO_ERROR) {
        TEST_LOG_INFO("GL_OUT_OF_MEMORY was not generated. This behavior is implementation-dependent.");
    }
    else {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_OUT_OF_MEMORY or GL_NO_ERROR, but got 0x%x.", err);
    }

    glDeleteBuffers(1, &buffer);
}


// Belirtilmeyen hatalar --------------------------------------------------

// Kaynak veri boyutunun belirtilen 'size' değerinden küçük olduğu hatalı API
// kullanımına karşı implementasyonun dayanıklılığını gözlemler.
void BufferObjects_BufferData_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char smallSource[16] = {0}; // Sadece 16 byte'lık kaynak veri.

    // Kasıtlı yanlış kullanım: OpenGL'den 4096 byte okuması isteniyor ancak kaynak yalnızca 16 byte.
    glBufferData(GL_ARRAY_BUFFER, 4096, smallSource, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_6, test_procedure, "error = 0x%x.", err);
    }
    glDeleteBuffers(1, &buf);
}

// size = 0 ama data != NULL
// Sıfır boyutlu data store oluşturulurken geçerli bir data pointer'ı verilmesinin
// implementasyon tarafından güvenli şekilde ele alınıp alınmadığını doğrular.
void BufferObjects_BufferData_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char dummy = 0xAB;
    glBufferData(GL_ARRAY_BUFFER, 0, &dummy, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_7, test_procedure, "error = 0x%x.", err);
    }

    glDeleteBuffers(1, &buf);
}

// Sınır ve aşırı boyut size değerleri karşısında implementasyonun kararlılığını gözlemler.
void BufferObjects_BufferData_TC_008()
{
    GLsizeiptr candidates[] = { -1, INT_MIN, (GLsizeiptr)INT_MAX + 1, LLONG_MAX };

    for (int i = 0; i < 4; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        GLuint buf;
        glGenBuffers(1, &buf);
        glBindBuffer(GL_ARRAY_BUFFER, buf);
        glBufferData(GL_ARRAY_BUFFER, candidates[i], NULL, GL_STATIC_DRAW);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
            return;
        }
        glDeleteBuffers(1, &buf);
    }
    TEST_LOG_SUCCESS(test_case_8, test_procedure);
}

// Geçersiz ve kirlenmiş usage enum değerleri karşısında implementasyonun kararlılığını gözlemler.
void BufferObjects_BufferData_TC_009()
{
    GLenum candidates[] = {GL_STATIC_DRAW, 0xFFFF0000u, 0xFFFFFFFFu, 0x12345678u, 0xDEADBEEFu};

    for (int i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++){
        while (glGetError() != GL_NO_ERROR) {}

        GLenum usage = candidates[i];
        GLuint buf;
        glGenBuffers(1, &buf);
        glBindBuffer(GL_ARRAY_BUFFER, buf);
        glBufferData(GL_ARRAY_BUFFER, 64, NULL, usage);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_9, test_procedure, "error = 0x%x.", err);
            glDeleteBuffers(1, &buf);
            return;
        }
        glDeleteBuffers(1, &buf);
    }
    TEST_LOG_SUCCESS(test_case_9, test_procedure);
}

// Hedefe herhangi bir buffer bağlı değilken glBufferData çağrısının implementasyon
// tarafından güvenli şekilde ele alınıp alınmadığını gözlemler.
void BufferObjects_BufferData_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(GL_ARRAY_BUFFER, 0); // hiçbir buffer bound değil
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_10, test_procedure, "error = 0x%x.", err);
    }
}

// Aynı buffer üzerinde farklı boyutlarda data store'ları art arda oluşturarak
// implementasyonun reallocation işlemlerindeki kararlılığını gözlemler.
void BufferObjects_BufferData_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    const GLsizeiptr sizes[] = { 0, 1, 16, 64, 256, 4096, 65536, 1048576};
    for (int i = 0; i < 10000; ++i)
    {
        GLsizeiptr size = sizes[i % (sizeof(sizes) / sizeof(sizes[0]))];
        glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_11, test_procedure, "error = 0x%x.", err);
            glDeleteBuffers(1, &buf);
            return;
        }
    }
    TEST_LOG_SUCCESS(test_case_11, test_procedure);
    glDeleteBuffers(1, &buf);
}

// Hizasız bir kaynak data pointer'ı kullanılarak implementasyonun hatalı
// istemci girdisi karşısındaki kararlılığı gözlemlenir.
void BufferObjects_BufferData_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char raw[128] = {0};
    void *misaligned = raw + 1;   // Kasıtlı olarak hizasız pointer
    glBufferData(GL_ARRAY_BUFFER, 64, misaligned, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_12, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_12, test_procedure, "error = 0x%x.", err);
    }
    glDeleteBuffers(1, &buf);
}

// Serbest bırakılmış (dangling) bir kaynak pointer kullanılarak implementasyonun
// hatalı istemci girdisi karşısındaki kararlılığı gözlemlenir.
void BufferObjects_BufferData_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char *heapData = (char *)malloc(256);
    if (heapData == NULL) {
        TEST_LOG_FAIL(test_case_13, test_procedure, "Memory allocation failed");
        glDeleteBuffers(1, &buf);
        return;
    }

    memset(heapData, 0xAB, 256);
    free(heapData); // Pointer artık dangling
    glBufferData(GL_ARRAY_BUFFER, 256, heapData, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_13, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_13, test_procedure, "error = 0x%x.", err);
    }
    glDeleteBuffers(1, &buf);
}

// Büyük bir data store tahsis denemesi sonrasında buffer nesnesinin
// durumunun korunup korunmadığını gözlemler.
void BufferObjects_BufferData_TC_014()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    // Başlangıçta küçük bir veri deposu oluştur.
    glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

    // Gerçekçi olmayan büyük bir tahsis denemesi.
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)1 << 40, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();

    GLint sizeAfter = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeAfter);

    if ((err == GL_OUT_OF_MEMORY || err == GL_NO_ERROR) && sizeAfter == 1024) {
        TEST_LOG_SUCCESS(test_case_14, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_14, test_procedure, "error = 0x%x.", err);
    }

    glDeleteBuffers(1, &buf);
}
