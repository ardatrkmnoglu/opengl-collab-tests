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
GL20SC - BufferObjects - BufferSubData - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_011";
static const char* test_case_12 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_012";
static const char* test_case_13 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_013";
static const char* test_case_14 = "GS_GL20SC_BO_BSD_ROBUSTNESS_TC_014";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    int data = 123;
    glBufferSubData(0xFFFFFFFF, 0, sizeof(data), &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if offset is negative.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_002()
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
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_VALUE, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
}

// Belirtilen hata: GL_INVALID_VALUE is generated if size is negative.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_003()
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
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_VALUE, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
}

// Belirtilen hata: GL_INVALID_VALUE is generated if offset and size together define a region beyond the allocated data store.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_004()
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
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_VALUE, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR){}

    int data = 123;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_INVALID_OPERATION, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
}


// belirtilmeyen hatalar ------------------------

// Offset ve size değerlerinin toplamında oluşabilecek integer overflow durumunda
// implementasyonun sınır kontrollerini güvenli şekilde yapıp yapmadığını gözlemler.
// GLsizeptr 32 bit ise LLONG_MAX yerine INT_MAX kullanmak gerekebilir
// It failed on Windows
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_006()
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
    if (err == GL_INVALID_VALUE) {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_6, test_procedure, "error = 0x%x.", err);
    }
}

// Buffer sınırının tam bitiş noktası ve bir byte ötesi kullanılarak
// implementasyonun sınır kontrollerindeki kararlılığı gözlemlenir.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

    char data[16] = {0};
    glBufferSubData(GL_ARRAY_BUFFER, 240, 16, data); // offset + size == 256, tam sınırda; spec'e göre GEÇERLİ olmalı

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "expected GL_NO_ERROR but got 0x%x.", err);
        return;
    }

    // Bir fazlasını dene: sınırı 1 byte aşan durum
    glBufferSubData(GL_ARRAY_BUFFER, 241, 16, data); // 241+16=257 > 256
    err = glGetError();
    if (err == GL_INVALID_VALUE) {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_7, test_procedure, "error = 0x%x.", err);
    }
}

// Negatif offset değerinin büyük bir size ile "telafi edildiği" durumda implementasyonun offset
// doğrulamasını bağımsız olarak yapıp yapmadığını ve sınır kontrollerindeki kararlılığını gözlemler
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_008()
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
    if (err == GL_INVALID_VALUE) {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
    }
}

// Sıfır byte güncelleme isteğinde implementasyonun gereksiz bellek erişimi
// yapmadan çağrıyı güvenli şekilde tamamlayıp tamamlamadığını gözlemler.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 0, NULL);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_9, test_procedure, "error = 0x%x.", err);
    }
}

// Hedefe herhangi bir buffer bağlı değilken glBufferSubData çağrısının implementasyon
// tarafından güvenli şekilde ele alınıp alınmadığını gözlemler.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    char data[16] = {0};
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_10, test_procedure, "expected: GL_INVALID_OPERATION, error = 0x%x.", err);
    }
}

// Data store'u henüz oluşturulmamış (0 byte) bir buffer nesnesine yazma isteğinin
// implementasyon tarafından güvenli şekilde ele alınıp alınmadığını gözlemler
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    // glBufferData hiç çağrılmadı -- spec: "immediately after first bound, zero-sized memory buffer" durumu geçerli

    char data[16] = {0};
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

    GLenum err = glGetError();
    if (err == GL_INVALID_VALUE) {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_11, test_procedure, "expected: GL_INVALID_VALUE, error = 0x%x.", err);
    }
}

// Kaynak veri tamponunun belirtilen size değerinden küçük olduğu hatalı API kullanımına
// karşı implementasyonun dayanıklılığını gözlemler.
// It crashed on Windows
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_012()
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
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_12, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_12, test_procedure, "error = 0x%x.", err);
    }
}

// Serbest bırakılmış bir istemci bellek işaretçisi kullanılarak implementasyonun
// geçersiz veri kaynağı karşısındaki davranışı gözlemlenir.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_013(void)
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
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_13, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_13, test_procedure, "error = 0x%x.", err);
    }
}

// Aynı buffer bölgesine çakışan ve hizasız güncellemeleri art arda gerçekleştirerek
// implementasyonun yoğun bellek kopyalama yükü altındaki kararlılığını gözlemler.
void GS_GL20SC_BO_BSD_ROBUSTNESS_TC_014()
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
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_14, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_14, test_procedure, "error = 0x%x.", err);
    }
    // Spec alignment gereksinimini not olarak belirtiyor ama ihlali için hata tanımlamıyor;
    // burada amaç implementasyonun iç kopyalama rutininin (örn. SIMD/vektörize memcpy) hizasız erişimde çökmesi
}


/* Initialization */
void GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_006(); // It failed on Windows
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_012(); // It crashed on Windows
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_013();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_BSD_ROBUSTNESS_TC_014();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}