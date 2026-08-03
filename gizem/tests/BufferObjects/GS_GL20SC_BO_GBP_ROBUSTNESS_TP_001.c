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
GL20SC - BufferObjects - GetBufferParameteriv - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_011";
static const char* test_case_12 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_012";
static const char* test_case_13 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_013";
static const char* test_case_14 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_014";
static const char* test_case_15 = "GS_GL20SC_BO_GBP_ROBUSTNESS_TC_015";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint data = -1;
    glGetBufferParameteriv((GLenum)0xFFFFFFFF, GL_BUFFER_SIZE, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%X\n", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if value is not GL_BUFFER_SIZE or GL_BUFFER_USAGE.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buffer;
    GLint data = -1;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, (GLenum)0xFFFFFFFF, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM){
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%x.", err);
    }
    TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint data = -1;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_OPERATION, but got 0x%x.", err);
    }
    TEST_LOG_SUCCESS(test_case_3, test_procedure);
}


// belirtilmeyen hatalar ------------------------------

// target parametresi GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER dışında bir enum olduğunda
// GL_INVALID_ENUM üretilip data'nın değişmediği doğrulanır.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

    GLint sentinel = 0x7EADBEEF;
    GLint data = sentinel;
    glGetBufferParameteriv(GL_TEXTURE_2D, GL_BUFFER_SIZE, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_ENUM for invalid target, got 0x%x.", err);
    }
    else if (data != sentinel) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "data was modified despite GL_INVALID_ENUM (data=%d)\n", data);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// value parametresi GL_BUFFER_SIZE/GL_BUFFER_USAGE dışında bir enum
// olduğunda GL_INVALID_ENUM üretilip data'nın değişmediği doğrulanır.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

    GLenum bogusValues[] = { GL_FRAMEBUFFER, 0, 0xFFFFFFFF, GL_ARRAY_BUFFER, 0xDEADBEEF };
    int n = sizeof(bogusValues) / sizeof(bogusValues[0]);
    int allCorrect = 1;

    for (int i = 0; i < n; i++) {
        while (glGetError() != GL_NO_ERROR) {}
        GLint sentinel = 0x7EADBEEF;
        GLint data = sentinel;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, bogusValues[i], &data);
        GLenum err = glGetError();
        if (err != GL_INVALID_ENUM || data != sentinel) allCorrect = 0;
    }
    if (allCorrect) {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Some invalid value enums not properly rejected");
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Reserved isim 0, target'a bind edilmişken (ya da hiç bind edilmemişken)
// GL_INVALID_OPERATION üretilip üretilmediğini doğrular.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint sentinel = 0x7EADBEEF;
    GLint data = sentinel;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);
    GLenum err = glGetError();

    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Expected GL_INVALID_OPERATION with reserved name 0 bound, got 0x%X.", err);
    }
    else if (data != sentinel) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "data was modified despite GL_INVALID_OPERATION (data=%d)", data);
    }
    else {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
}

// GL_ELEMENT_ARRAY_BUFFER hedefi için de aynı reserved-0 davranışını doğrular
// (iki farklı target için de spec aynı şekilde uygulanmalı).
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint data = -1;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &data);
    GLenum err = glGetError();

    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Expected GL_INVALID_OPERATION, error = 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
}

// data parametresi NULL iken çağrıldığında (n > 0 anlamında tek bir GLint yazılacak durum)
// implementasyonun segfault yerine tanımlı/tutarlı davranıp davranmadığını gözlemler.
// It crashed on Windows
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
    while (glGetError() != GL_NO_ERROR) {}

    // Spesifikasyonda tanımsızdır; amaç implementasyonun crash olmadan hayatta kalıp kalmadığını gözlemlemektir.
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, NULL);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// data, geçersiz/erişilemez (dangling) bir bellek adresi olduğunda
// implementasyonun bellek koruma ihlaline karşı davranışını test eder.
// test, başta geçmiş görünüyor ancak kısa süre sonra çöküyor. Sorunu bulamadım.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_DRAW);
    while (glGetError() != GL_NO_ERROR) {}

    GLint *freedPtr = (GLint *)malloc(sizeof(GLint));
    free(freedPtr); // artık geçersiz

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, freedPtr);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_9, test_procedure, "error = 0x%x.", err);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// GL_BUFFER_SIZE sorgusunun, glBufferData ile ayrılan gerçek boyutla tutarlı olup olmadığını;
// sıfır boyutlu bir bufferda doğru şekilde 0 dönüp dönmediğini doğrular.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    // Baslangicta (storage olusmadan once) boyut 0 olmali.
    GLint initialSize = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &initialSize);

    const GLsizeiptr allocSize = 256;
    glBufferData(GL_ARRAY_BUFFER, allocSize, NULL, GL_STATIC_DRAW);

    GLint size = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Unexpected error querying size: 0x%x.", err);
    }
    else if (size != (GLint)allocSize) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Size mismatch: expected %ld, got %d", (long)allocSize, size);
    }
    else if (initialSize != 0) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Initial size before glBufferData was not 0 (got %d)", initialSize);
    }
    else {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// GL_BUFFER_USAGE'in initial değerinin spesifikasyona uygun şekilde GL_STATIC_DRAW olduğunu,
// ve glBufferData sonrası değişen usage değerlerinin doğru yansıtılıp yansıtılmadığını doğrular.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    GLint initialUsage = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &initialUsage);

    GLenum usages[] = { GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW };
    int n = sizeof(usages) / sizeof(usages[0]);
    int allCorrect = (initialUsage == GL_STATIC_DRAW);

    for (int i = 0; i < n; i++) {
        glBufferData(GL_ARRAY_BUFFER, 16, NULL, usages[i]);
        GLint got = -1;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &got);
        if ((GLenum)got != usages[i]) allCorrect = 0;
    }

    if (allCorrect) TEST_LOG_SUCCESS(test_case_11, test_procedure);
    else TEST_LOG_FAIL(test_case_11, test_procedure, "Usage value mismatch detected");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// target ve value her ikisi de geçersiz olduğunda hangi hatanın (spesifikasyon öncelik belirtmiyor)
// üretildiğini gözlemler - implementasyon tutarlı bir sıraya sahip olmalı (crash olmamalı).
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint data = 0x1234;
    glGetBufferParameteriv(0xFFFFFFFF, 0xDEADBEEF, &data);
    GLenum err = glGetError();

    if (err == GL_INVALID_ENUM) {
        TEST_LOG_SUCCESS(test_case_12, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_12, test_procedure, "Unexpected error for combined invalid target/value.");
    }
}

// Aynı buffer nesnesi hem GL_ARRAY_BUFFER hem de GL_ELEMENT_ARRAY_BUFFER hedeflerine aynı
// anda bind edildiğinde (OpenGL'de geçerli bir durum, buffer'lar target'dan bağımsızdır)
// her iki target üzerinden sorgunun tutarlı sonuc verip vermediğini doğrular.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 100, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf); // aynı obje, farklı target
    GLint sizeViaArray = -1, sizeViaElement = -1;
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeViaArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeViaElement);

    if (sizeViaArray == sizeViaElement && sizeViaArray == 100) {
        TEST_LOG_SUCCESS(test_case_13, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_13, test_procedure, "Inconsistent buffer state across targets - possible aliasing bug");
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// data çıkış parametresi unaligned bir adres olduğunda implementasyonun crash olmadan davranıp davranmadığını test eder.
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_014()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 16, NULL, GL_STATIC_DRAW);

    while (glGetError() != GL_NO_ERROR) {}

    unsigned char raw[64];
    memset(raw, 0xAA, sizeof(raw));
    GLint *unaligned = (GLint *)(raw + 1); // kasıtlı hizalama bozukluğu

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, unaligned);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_14, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_14, test_procedure, "error = 0x%x.", err);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Çok sayıda ardışık geçersiz çağrıyla error state'inin (glGetError kuyruğu) taşırmadığını / birikip birikmediğini
// kontrol eder (implementasyonun error flag mekanizmasının kararlılığı).
void GS_GL20SC_BO_GBP_ROBUSTNESS_TC_015() {
    while (glGetError() != GL_NO_ERROR) {}

    GLint data;
    const int ITER = 10000;
    for (int i = 0; i < ITER; i++) {
        glGetBufferParameteriv(GL_TEXTURE_2D, GL_BUFFER_SIZE, &data); // her seferinde INVALID_ENUM

        // OpenGL spesifikasyonu: art arda hatalar sadece "en eski" hatayı tutar, yeni hata bayrağı eklenmez.
        GLenum err1 = glGetError();
        GLenum err2 = glGetError(); // ikinci cağrı GL_NO_ERROR dönmeli

        if (err1 != GL_INVALID_ENUM || err2 != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_15, test_procedure, "Unexpected error queue behavior under stress");
            return;
        }
    }
    TEST_LOG_SUCCESS(test_case_15, test_procedure);
}


/* Initialization */
void GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_008(); // It crashed on Windows
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_012();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_013();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_014();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_BO_GBP_ROBUSTNESS_TC_015();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}