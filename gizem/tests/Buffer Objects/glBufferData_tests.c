#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// void glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
// Target parametresiyle bind ettiğin buffer object için ekran kartı (GPU) üzerinde yeni bir data store oluşturur
// Eski veriyi tamamen siler
// İstersen verdiğin data pointer’ındaki veriyi bu yeni belleğe kopyalayarak başlatır
// usage parametresiyle de bu veriyi nasıl kullanacağını sürücüye ipucu olarak bildirirsin (performans optimizasyonu için)


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void rTest_glBufferData_invalid_enum_target()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_invalid_enum_target()\n");

    glBufferData(0xFFFFFFFF, 16, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
        assert(err == GL_INVALID_ENUM);
    }
    printf("[PASS] rTest_glBufferData_invalid_enum_target()\n");
}

// Belirtilen hata: GL_INVALID_ENUM is generated if usage is not GL_STREAM_DRAW, GL_STATIC_DRAW, or GL_DYNAMIC_DRAW.
void rTest_glBufferData_invalid_enum_usage()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_invalid_enum_usage()\n");

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 16, NULL, 0xFFFFFFFF);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
        assert(err == GL_INVALID_ENUM);
    }
    printf("[PASS] rTest_glBufferData_invalid_enum_usage()\n");

    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if size is negative.
void rTest_glBufferData_invalid_value_negative_size()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_invalid_value_negative_size()\n");

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, -1, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
        assert(err == GL_INVALID_VALUE);
    }
    printf("[PASS] rTest_glBufferData_invalid_value_negative_size()\n");

    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
void rTest_glBufferData_invalid_operation_zero_buffer_bound()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_invalid_operation_zero_buffer_bound()\n");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferData(GL_ARRAY_BUFFER, 16, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        printf("[FAIL] Expected GL_INVALID_OPERATION, but got 0x%X\n", err);
        assert(err == GL_INVALID_OPERATION);
    }
    printf("[PASS] rTest_glBufferData_invalid_operation_zero_buffer_bound()\n");
}

// Belirtilen hata: GL_OUT_OF_MEMORY is generated if the GL is unable to create a data store with the specified size.
void rTest_glBufferData_out_of_memory()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_out_of_memory()\n");

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)INTPTR_MAX, NULL, GL_STATIC_DRAW );

    GLenum err = glGetError();
    if (err == GL_OUT_OF_MEMORY) { printf("[PASS] GL_OUT_OF_MEMORY was generated.\n");}
    else if (err == GL_NO_ERROR) { printf("[INFO] GL_OUT_OF_MEMORY was not generated. This behavior is implementation-dependent.\n");}
    else {
        printf("[FAIL] Expected GL_OUT_OF_MEMORY or GL_NO_ERROR, but got 0x%X\n", err);
        assert(0); }

    glDeleteBuffers(1, &buffer);
}


// Belirtilmeyen hatalar --------------------------------------------------

// Kaynak veri boyutunun belirtilen 'size' değerinden küçük olduğu hatalı API kullanımına karşı implementasyonun dayanıklılığını gözlemler.
void rTest_glBufferData_source_buffer_too_small()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_source_buffer_too_small()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char smallSource[16] = {0}; // Sadece 16 byte'lık kaynak veri.

    // Kasıtlı yanlış kullanım: OpenGL'den 4096 byte okuması isteniyor ancak kaynak yalnızca 16 byte.
    glBufferData(GL_ARRAY_BUFFER, 4096, smallSource, GL_STATIC_DRAW);

    GLenum err = glGetError();
    printf("[INFO] Misuse robustness (source buffer too small): glError=0x%X \n", err);

    glDeleteBuffers(1, &buf);
}

// size = 0 ama data != NULL
// Sıfır boyutlu data store oluşturulurken geçerli bir data pointer'ı verilmesinin implementasyon tarafından güvenli şekilde ele alınıp alınmadığını doğrular.
void rTest_glBufferData_zero_size_nonnull_data()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_zero_size_nonnull_data()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char dummy = 0xAB;
    glBufferData(GL_ARRAY_BUFFER, 0, &dummy, GL_STATIC_DRAW);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) printf("[PASS] size=0, data!=NULL accepted.\n");
    else printf("[INFO] size=0, data!=NULL returned glError=0x%X\n", err);

    glDeleteBuffers(1, &buf);
}

// Sınır ve aşırı boyut size değerleri karşısında implementasyonun kararlılığını gözlemler.
void rTest_glBufferData_size_overflow_boundary()
{
    printf("[START] rTest_glBufferData_size_overflow_boundary()\n");

    GLsizeiptr candidates[] = { -1, INT_MIN, (GLsizeiptr)INT_MAX + 1, LLONG_MAX };

    for (int i = 0; i < 4; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        GLuint buf;
        glGenBuffers(1, &buf);
        glBindBuffer(GL_ARRAY_BUFFER, buf);
        glBufferData(GL_ARRAY_BUFFER, candidates[i], NULL, GL_STATIC_DRAW);

        GLenum err = glGetError();
        printf("[INFO] Boundary size test: size=%lld, glError=0x%X\n", (long long)candidates[i], err);

        glDeleteBuffers(1, &buf);
    }
}

// Geçersiz ve kirlenmiş usage enum değerleri karşısında implementasyonun kararlılığını gözlemler.
void rTest_glBufferData_dirty_usage_enum()
{
    printf("[START] rTest_glBufferData_dirty_usage_enum()\n");
    GLenum candidates[] = {GL_STATIC_DRAW, 0xFFFF0000u, 0xFFFFFFFFu, 0x12345678u, 0xDEADBEEFu};

    for (int i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++){
        while (glGetError() != GL_NO_ERROR) {}

        GLenum usage = candidates[i];
        GLuint buf;
        glGenBuffers(1, &buf);
        glBindBuffer(GL_ARRAY_BUFFER, buf);
        glBufferData(GL_ARRAY_BUFFER, 64, NULL, usage);

        GLenum err = glGetError();
        printf("[INFO] glBufferData(usage=0x%08X) completed, glError=0x%X\n", usage, err);

        glDeleteBuffers(1, &buf);
    }
}

// Hedefe herhangi bir buffer bağlı değilken glBufferData çağrısının implementasyon tarafından güvenli şekilde ele alınıp alınmadığını gözlemler.
void rTest_glBufferData_target_zero_bound()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_target_zero_bound()\n");

    glBindBuffer(GL_ARRAY_BUFFER, 0); // hiçbir buffer bound değil
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    printf("[INFO] glBufferData(buffer=0) completed, glError=0x%X\n", err);
}

// Aynı buffer üzerinde farklı boyutlarda data store'ları art arda oluşturarak implementasyonun reallocation işlemlerindeki kararlılığını gözlemler.
void rTest_glBufferData_repeated_resize_thrash()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_repeated_resize_thrash()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    const GLsizeiptr sizes[] = { 0, 1, 16, 64, 256, 4096, 65536, 1048576};
    for (int i = 0; i < 10000; ++i)
    {
        GLsizeiptr size = sizes[i % (sizeof(sizes) / sizeof(sizes[0]))];
        glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR){
            printf("[INFO] Iteration=%d, size=%lld, glError=0x%X\n", i, (long long)size, err);
            break;
        }
    }
    printf("[INFO] Repeated resize stress test completed.\n");

    glDeleteBuffers(1, &buf);
}

// Hizasız bir kaynak data pointer'ı kullanılarak implementasyonun hatalı istemci girdisi karşısındaki kararlılığı gözlemlenir.
void rTest_glBufferData_misaligned_data_pointer()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_misaligned_data_pointer()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char raw[128] = {0};
    void *misaligned = raw + 1;   // Kasıtlı olarak hizasız pointer
    glBufferData(GL_ARRAY_BUFFER, 64, misaligned, GL_STATIC_DRAW);

    GLenum err = glGetError();
    printf("[INFO] glBufferData(misaligned data pointer) completed, glError=0x%X\n", err);

    glDeleteBuffers(1, &buf);
}

// Serbest bırakılmış (dangling) bir kaynak pointer kullanılarak implementasyonun hatalı istemci girdisi karşısındaki kararlılığı gözlemlenir.
void rTest_glBufferData_dangling_data_pointer()
{
    printf("[START] rTest_glBufferData_dangling_data_pointer()\n");
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    char *heapData = (char *)malloc(256);
    if (heapData == NULL) {
        printf("[ERROR] Memory allocation failed.\n");
        glDeleteBuffers(1, &buf);
        return;
    }

    memset(heapData, 0xAB, 256);
    free(heapData); // Pointer artık dangling
    glBufferData(GL_ARRAY_BUFFER, 256, heapData, GL_STATIC_DRAW);

    GLenum err = glGetError();
    printf("[INFO] glBufferData(dangling data pointer) completed, glError=0x%X\n", err);

    glDeleteBuffers(1, &buf);
}

// Büyük bir data store tahsis denemesi sonrasında buffer nesnesinin durumunun korunup korunmadığını gözlemler.
void rTest_glBufferData_state_after_out_of_memory()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBufferData_state_after_out_of_memory()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    // Başlangıçta küçük bir veri deposu oluştur.
    glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

    // Gerçekçi olmayan büyük bir tahsis denemesi.
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)1 << 40, NULL, GL_STATIC_DRAW);

    GLenum err = glGetError();
    printf("[INFO] glBufferData(huge size) completed, glError=0x%X\n", err);

    GLint sizeAfter = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeAfter);
    printf("[INFO] Buffer size after allocation attempt: %d bytes\n", sizeAfter);

    glDeleteBuffers(1, &buf);
}
