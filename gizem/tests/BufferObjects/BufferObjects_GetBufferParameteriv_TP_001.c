#include <glad/gles2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/macro.h"

// void glGetBufferParameteriv(GLenum target, GLenum value, GLint * data);
// halihazırda bağlı (bound) olan bir buffer nesnesinin bazı özelliklerini öğrenmek için kullanılır.
// target: Hangi tür buffer’a bakacağını söyler
// value: Hangi parametreyi istediğini söyler
// data: Sonucun yazılacağı adres


static const char* test_procedure = "BufferObjects_GetBufferParameteriv_TP_001";
static const char* test_case_1 = "BufferObjects_GetBufferParameteriv_TC_001";
static const char* test_case_2 = "BufferObjects_GetBufferParameteriv_TC_002";
static const char* test_case_3 = "BufferObjects_GetBufferParameteriv_TC_003";
static const char* test_case_4 = "BufferObjects_GetBufferParameteriv_TC_004";
static const char* test_case_5 = "BufferObjects_GetBufferParameteriv_TC_005";
static const char* test_case_6 = "BufferObjects_GetBufferParameteriv_TC_006";
static const char* test_case_7 = "BufferObjects_GetBufferParameteriv_TC_007";
static const char* test_case_8 = "BufferObjects_GetBufferParameteriv_TC_008";
static const char* test_case_9 = "BufferObjects_GetBufferParameteriv_TC_009";
static const char* test_case_10 = "BufferObjects_GetBufferParameteriv_TC_010";
static const char* test_case_11 = "BufferObjects_GetBufferParameteriv_TC_011";
static const char* test_case_12 = "BufferObjects_GetBufferParameteriv_TC_012";
static const char* test_case_13 = "BufferObjects_GetBufferParameteriv_TC_013";
static const char* test_case_14 = "BufferObjects_GetBufferParameteriv_TC_014";
static const char* test_case_15 = "BufferObjects_GetBufferParameteriv_TC_015";
static const char* test_case_16 = "BufferObjects_GetBufferParameteriv_TC_016";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void BufferObjects_GetBufferParameteriv_TC_001()
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
void BufferObjects_GetBufferParameteriv_TC_002()
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
    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
void BufferObjects_GetBufferParameteriv_TC_003()
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

// target parametresi GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER disinda bir enum oldugunda GL_INVALID_ENUM uretilip data'nin degismedigini doğrular.
void BufferObjects_GetBufferParameteriv_TC_004()
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
    glDeleteBuffers(1, &buf);
}

// value parametresi GL_BUFFER_SIZE/GL_BUFFER_USAGE disinda bir enum oldugunda GL_INVALID_ENUM uretilip data'nin degismedigini doğrular.
void BufferObjects_GetBufferParameteriv_TC_005()
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
    glDeleteBuffers(1, &buf);
}

// Reserved isim 0, target'a bind edilmisken (ya da hic bind edilmemisken) GL_INVALID_OPERATION uretilip uretilmedigini doğrular.
void BufferObjects_GetBufferParameteriv_TC_006()
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

// GL_ELEMENT_ARRAY_BUFFER hedefi icin de ayni reserved-0 davranisini dogrular (iki farkli target icin de spec ayni sekilde uygulanmali).
void BufferObjects_GetBufferParameteriv_TC_007()
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

// data parametresi NULL iken cagirildiginda (n>0 anlaminda tek bir GLint yazilacak durum)
// implementasyonun segfault yerine tanimli/tutarli davranip davranmadigini gozlemler.
void BufferObjects_GetBufferParameteriv_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
    while (glGetError() != GL_NO_ERROR) {}

    // Spesifikasyonda tanimsizdir; amac implementasyonun crash olmadan hayatta kalip kalmadigini gozlemlemektir.
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, NULL);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// data, gecersiz/erisilemez (dangling) bir bellek adresi oldugunda implementasyonun bellek koruma ihlaline karsi davranisini test eder.
// test, başta geçmiş görünüyor ancak kısa süre sonra çöküyor. Sorunu bulamadım.
void BufferObjects_GetBufferParameteriv_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_DRAW);
    while (glGetError() != GL_NO_ERROR) {}

    GLint *freedPtr = (GLint *)malloc(sizeof(GLint));
    free(freedPtr); // artik gecersiz

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, freedPtr);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_9, test_procedure, "error = 0x%x.", err);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// GL_BUFFER_SIZE sorgusunun, glBufferData ile ayrilan gercek boyutla tutarli olup olmadigini;
// sifir boyutlu bir bufferda dogru sekilde 0 dondurup dondurmedigini doğrular.
void BufferObjects_GetBufferParameteriv_TC_010()
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
    glDeleteBuffers(1, &buf);
}

// GL_BUFFER_USAGE'in initial degerinin spesifikasyona uygun sekilde GL_STATIC_DRAW oldugunu,
// ve glBufferData sonrasi degisen usage degerlerinin dogru yansitilip yansitilmadigini doğrular.
void BufferObjects_GetBufferParameteriv_TC_011()
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
    glDeleteBuffers(1, &buf);
}

// target ve value her ikisi de gecersiz oldugunda hangi hatanin (spesifikasyon oncelik belirtmiyor)
// uretildigini gozlemler - implementasyon tutarli bir siraya sahip olmali (crash olmamali).
void BufferObjects_GetBufferParameteriv_TC_012()
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

// Silinmis bir buffer, hala ayni target'a bind edilmis GIBI davranildigi (yani silme sonrasi binding'in 0'a dondugu spesifikasyon davranisini)
// dogrulamak icin: sil, tekrar sorgula, GL_INVALID_OPERATION beklenir (cunku binding otomatik olarak 0'a doner).
void BufferObjects_GetBufferParameteriv_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_DRAW);
    glDeleteBuffers(1, &buf); // spec: bind edilmis buffer silinirse binding 0'a doner

    GLenum errDelete = glGetError();

    GLint data = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);

    GLenum errQuery = glGetError();

    if (errQuery == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case_13, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_INVALID_OPERATION after delete (binding should revert to 0)");
    }
}

// Ayni buffer nesnesi hem GL_ARRAY_BUFFER hem GL_ELEMENT_ARRAY_BUFFER hedeflerine ayni anda bind edildiginde (OpenGL'de gecerli bir durum,
// buffer'lar target-agnostic'tir) her iki target uzerinden sorgunun tutarli sonuc verip vermedigini doğrular.
void BufferObjects_GetBufferParameteriv_TC_014()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 100, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf); // ayni obje, farkli target
    GLint sizeViaArray = -1, sizeViaElement = -1;
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeViaArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeViaElement);

    if (sizeViaArray == sizeViaElement && sizeViaArray == 100) {
        TEST_LOG_SUCCESS(test_case_14, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_14, test_procedure, "Inconsistent buffer state across targets - possible aliasing bug");
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// data cikis parametresi unaligned bir adres oldugunda implementasyonun crash olmadan davranip davranmadigini test eder.
void BufferObjects_GetBufferParameteriv_TC_015()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 16, NULL, GL_STATIC_DRAW);

    while (glGetError() != GL_NO_ERROR) {}

    unsigned char raw[64];
    memset(raw, 0xAA, sizeof(raw));
    GLint *unaligned = (GLint *)(raw + 1); // kasitli hizalama bozuklugu

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, unaligned);

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_15, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_15, test_procedure, "error = 0x%x.", err);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// Cok sayida ardisik gecersiz cagriyla error state'inin (glGetError kuyrugu) tasip tasmadigini / birikip birikmedigini
// kontrol eder (implementasyonun error flag mekanizmasinin kararliligi).
void BufferObjects_GetBufferParameteriv_TC_016() {
    while (glGetError() != GL_NO_ERROR) {}

    GLint data;
    const int ITER = 10000;
    for (int i = 0; i < ITER; i++) {
        glGetBufferParameteriv(GL_TEXTURE_2D, GL_BUFFER_SIZE, &data); // her seferinde INVALID_ENUM}

        // OpenGL spesifikasyonu: art arda hatalar sadece "en eski" hatayi tutar, yeni hata bayragi eklenmez.
        GLenum err1 = glGetError();
        GLenum err2 = glGetError(); // ikinci cagri GL_NO_ERROR donmeli

        if (err1 != GL_INVALID_ENUM || err2 != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_16, test_procedure, "Unexpected error queue behavior under stress");
            return;
        }
    }
    TEST_LOG_SUCCESS(test_case_16, test_procedure);
}