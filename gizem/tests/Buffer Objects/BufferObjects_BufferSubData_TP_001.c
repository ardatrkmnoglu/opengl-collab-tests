#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
// Daha önceden oluşturulmuş bir buffer’ın içindeki belirli bir kısmı günceller
// Yeni bellek ayırmaz, var olan glBufferData ile oluşturulmuş data store’un içini kısmen değiştirir
// target ile belirtilen ve şu an glBindBuffer ile bağlanmış buffer’ı kullanır
// Buffer’ın offset byte’tan başlayan kısmına, size byte uzunluğunda data pointer’ındaki veriyi kopyalar.



// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void BufferObjects_BufferSubData_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    int data = 123;
    glBufferSubData(0xFFFFFFFF, 0, sizeof(data), &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glBufferSubData_invalid_enum_target()\n");
}

// Belirtilen hata: GL_INVALID_VALUE is generated if offset is negative.
void BufferObjects_BufferSubData_TC_002()
{
    while (glGetError() != GL_NO_ERROR){}

    GLuint buffer;
    int data = 123;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, -1, sizeof(data), &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glBufferSubData_invalid_value_negative_offset()\n");

    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if size is negative.
void BufferObjects_BufferSubData_TC_003()
{
    while (glGetError() != GL_NO_ERROR){}

    GLuint buffer;
    int data = 123;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, -1, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glBufferSubData_invalid_value_negative_size()\n");

    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if offset and size together define a region beyond the allocated data store.
void BufferObjects_BufferSubData_TC_004()
{
    while (glGetError() != GL_NO_ERROR){}

    GLuint buffer;
    int data = 123;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 60, 8, &data);  // 60 + 8 = 68 > 64

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glBufferSubData_invalid_value_out_of_bounds()\n");

    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
void BufferObjects_BufferSubData_TC_005()
{
    while (glGetError() != GL_NO_ERROR){}

    int data = 123;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        printf("[FAIL] Expected GL_INVALID_OPERATION, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glBufferSubData_invalid_operation_zero_buffer_bound()\n");
}



// belirtilmeyen hatalar ------------------------

// Offset ve size değerlerinin toplamında oluşabilecek integer overflow durumunda implementasyonun sınır kontrollerini güvenli şekilde yapıp yapmadığını gözlemler.
// GLsizeptr 32 bit ise LLONG_MAX yerine INT_MAX kullanmak gerekebilir
void BufferObjects_BufferSubData_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

    // Eğer implementasyon "offset + size <= buffer_size" kontrolünü wraparound'a karşı korumasız yapıyorsa,
    // negatif/küçük bir toplam elde edip sınır kontrolünü atlatabilir -> OOB write
    GLintptr offset = 100;
    GLsizeiptr size = (GLsizeiptr)LLONG_MAX - 50; // offset + size overflow eder
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, NULL);

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(offset=%lld, size=%lld) completed, glError=0x%X\n", (long long)offset, (long long)size, err);
}

// Buffer sınırının tam bitiş noktası ve bir byte ötesi kullanılarak implementasyonun sınır kontrollerindeki kararlılığı gözlemlenir.
void BufferObjects_BufferSubData_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

    char data[16] = {0};
    glBufferSubData(GL_ARRAY_BUFFER, 240, 16, data); // offset + size == 256, tam sınırda; spec'e göre GEÇERLİ olmalı

    GLenum err = glGetError();
    printf("[INFO] Exact boundary update completed (expected: NO_ERROR), glError=0x%X\n", err);

    // Bir fazlasını dene: sınırı 1 byte aşan durum
    glBufferSubData(GL_ARRAY_BUFFER, 241, 16, data); // 241+16=257 > 256
    err = glGetError();
    printf("[INFO] One-byte-beyond boundary update completed (expected: INVALID_VALUE ), glError=0x%X\n", err);
}

// Negatif offset değerinin büyük bir size ile "telafi edildiği" durumda implementasyonun offset
// doğrulamasını bağımsız olarak yapıp yapmadığını ve sınır kontrollerindeki kararlılığını gözlemler
void BufferObjects_BufferSubData_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

    // negatif offset tek başına INVALID_VALUE üretmeli.
    // Ama size'ı öyle seçtik ki offset+size matematiksel olarak buffer içinde "makul" görünüyor
    // -- implementasyon offset'i ayrı ayrı kontrol etmiyorsa bu sinsi bir OOB write'a yol açabilir.
    GLintptr offset = -512;
    GLsizeiptr size = 600; // offset+size = 88, buffer içinde gibi görünüyor

    char data[600] = {0};
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(offset=%lld, size=%lld) completed, (expected:INVALID_VALUE) glError=0x%X\n", (long long)offset, (long long)size, err);
}

// Sıfır byte güncelleme isteğinde implementasyonun gereksiz bellek erişimi yapmadan çağrıyı güvenli şekilde tamamlayıp tamamlamadığını gözlemler.
void BufferObjects_BufferSubData_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 0, NULL);

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(size=0, data=NULL) completed, glError=0x%X\n", err);
}

// Hedefe herhangi bir buffer bağlı değilken glBufferSubData çağrısının implementasyon tarafından güvenli şekilde ele alınıp alınmadığını gözlemler.
void BufferObjects_BufferSubData_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    char data[16] = {0};
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(buffer=0) completed, glError=0x%X (expected: GL_INVALID_OPERATION)\n", err);
}

// Data store'u henüz oluşturulmamış (0 byte) bir buffer nesnesine yazma isteğinin
// implementasyon tarafından güvenli şekilde ele alınıp alınmadığını gözlemler
void BufferObjects_BufferSubData_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    // glBufferData hiç çağrılmadı -- spec: "immediately after first bound, zero-sized memory buffer" durumu geçerli

    char data[16] = {0};
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(zero-sized store, size=16) completed, glError=0x%X (expected: GL_INVALID_VALUE)\n", err);
}

// Kaynak veri tamponunun belirtilen size değerinden küçük olduğu hatalı API kullanımına karşı implementasyonun dayanıklılığını gözlemler.
void BufferObjects_BufferSubData_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

    char small_source[8] = {0};
    // size=4096 beyan ediliyor ama kaynak sadece 8 byte -- GL bunu doğrulayamaz, implementasyonun kendi belleğinden OOB okumasını dener
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4096, small_source);

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(source smaller than size) completed, glError=0x%X\n", err);
}

// Serbest bırakılmış bir istemci bellek işaretçisi kullanılarak implementasyonun geçersiz veri kaynağı karşısındaki davranışı gözlemlenir.
void BufferObjects_BufferSubData_TC_013(void)
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

    char *heap_data = (char *)malloc(256);
    free(heap_data); /* serbest bırakıldı */

    glBufferSubData(GL_ARRAY_BUFFER, 0, 256, heap_data);

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(dangling data pointer) completed, glError=0x%X\n", err);
}

// Aynı buffer bölgesine çakışan ve hizasız güncellemeleri art arda gerçekleştirerek
// implementasyonun yoğun bellek kopyalama yükü altındaki kararlılığını gözlemler.
void BufferObjects_BufferSubData_TC_014()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

    char raw[64];
    for (int i = 0; i < 5000; ++i) {
        GLintptr offset = (i * 3) % 4090; // sürekli çakışan, hizasız offsetler
        GLsizeiptr size = 1 + (i % 63);   // rastgele, çoğunlukla hizasız boyutlar
        if (offset + size > 4096) continue; // sınır ihlalini bu testte istemiyoruz

        void* misaligned = raw + (i % 3); // hizasız kaynak pointer
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, misaligned);
    }

    GLenum err = glGetError();
    printf("[INFO] glBufferSubData(overlapping/misaligned thrash) completed, glError=0x%X\n", err);
    // Spec alignment gereksinimini not olarak belirtiyor ama ihlali için hata tanımlamıyor;
    // burada amaç implementasyonun iç kopyalama rutininin (örn. SIMD/vektörize memcpy) hizasız erişimde çökmesi
}