#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// void glGetBufferParameteriv(GLenum target, GLenum value, GLint * data);
// halihazırda bağlı (bound) olan bir buffer nesnesinin bazı özelliklerini öğrenmek için kullanılır.
// target: Hangi tür buffer’a bakacağını söyler
// value: Hangi parametreyi istediğini söyler
// data: Sonucun yazılacağı adres


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void rTest_glGetBufferParameteriv_invalid_enum_target()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_invalid_enum_target()\n");

    GLint data = -1;
    glGetBufferParameteriv((GLenum)0xFFFFFFFF, GL_BUFFER_SIZE, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
        assert(err == GL_INVALID_ENUM);
    }
    printf("[PASS] rTest_glGetBufferParameteriv_invalid_enum_target()\n");
}

// Belirtilen hata: GL_INVALID_ENUM is generated if value is not GL_BUFFER_SIZE or GL_BUFFER_USAGE.
void rTest_glGetBufferParameteriv_invalid_enum_value()
{
    while (glGetError() != GL_NO_ERROR) {}

    printf("[START] rTest_glGetBufferParameteriv_invalid_enum_value()\n");

    GLuint buffer;
    GLint data = -1;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, (GLenum)0xFFFFFFFF, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM){
        printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
        assert(err == GL_INVALID_ENUM);
    }
    printf("[PASS] rTest_glGetBufferParameteriv_invalid_enum_value()\n");

    glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
void rTest_glGetBufferParameteriv_invalid_operation_zero_buffer_bound()
{
    while (glGetError() != GL_NO_ERROR) {}

    printf("[START] rTest_glGetBufferParameteriv_invalid_operation_zero_buffer_bound()\n");

    GLint data = -1;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        printf("[FAIL] Expected GL_INVALID_OPERATION, but got 0x%X\n", err);
        assert(err == GL_INVALID_OPERATION);
    }
    printf("[PASS] rTest_glGetBufferParameteriv_invalid_operation_zero_buffer_bound()\n");
}


// belirtilmeyen hatalar ------------------------------

// target parametresi GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER disinda bir enum oldugunda GL_INVALID_ENUM uretilip data'nin degismedigini doğrular.
void rTest_glGetBufferParameteriv_invalid_target()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_invalid_target()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

    GLint sentinel = 0x7EADBEEF;
    GLint data = sentinel;
    glGetBufferParameteriv(GL_TEXTURE_2D, GL_BUFFER_SIZE, &data);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {printf("[FAIL] Expected GL_INVALID_ENUM for invalid target, got 0x%X\n", err);}
    else if (data != sentinel) {printf("[FAIL] data was modified despite GL_INVALID_ENUM (data=%d)\n", data);}
    else {printf("[PASS] Invalid target correctly rejected, data untouched.\n");}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// value parametresi GL_BUFFER_SIZE/GL_BUFFER_USAGE disinda bir enum oldugunda GL_INVALID_ENUM uretilip data'nin degismedigini doğrular.
void rTest_glGetBufferParameteriv_invalid_value()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_invalid_value()\n");

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
        printf("[INFO] value=0x%X => err=0x%X, data-modified=%s\n",
        bogusValues[i], err, (data != sentinel) ? "yes" : "no");
        if (err != GL_INVALID_ENUM || data != sentinel) allCorrect = 0;
    }
    if (allCorrect) printf("[PASS] All invalid value enums correctly rejected without touching data.\n");
    else printf("[FAIL] Some invalid value enums not properly rejected.\n");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// Reserved isim 0, target'a bind edilmisken (ya da hic bind edilmemisken) GL_INVALID_OPERATION uretilip uretilmedigini doğrular.
void rTest_glGetBufferParameteriv_reserved_name_zero_bound()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_reserved_name_zero_bound()\n");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint sentinel = 0x7EADBEEF;
    GLint data = sentinel;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);
    GLenum err = glGetError();

    if (err != GL_INVALID_OPERATION) {printf("[FAIL] Expected GL_INVALID_OPERATION with reserved name 0 bound, got 0x%X\n", err);}
    else if (data != sentinel) {printf("[FAIL] data was modified despite GL_INVALID_OPERATION (data=%d)\n", data);}
    else {printf("[PASS] Correctly rejected query on reserved buffer name 0.\n");}
}

// GL_ELEMENT_ARRAY_BUFFER hedefi icin de ayni reserved-0 davranisini dogrular (iki farkli target icin de spec ayni sekilde uygulanmali).
void rTest_glGetBufferParameteriv_element_array_zero_bound()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_element_array_zero_bound()\n");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    GLint data = -1;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &data);
    GLenum err = glGetError();

    if (err != GL_INVALID_OPERATION) {printf("[FAIL] Expected GL_INVALID_OPERATION, got 0x%X\n", err);}
    else {printf("[PASS] Correctly rejected GL_ELEMENT_ARRAY_BUFFER query with 0 bound.\n");}
}

// data parametresi NULL iken cagirildiginda (n>0 anlaminda tek bir GLint yazilacak durum)
// implementasyonun segfault yerine tanimli/tutarli davranip davranmadigini gozlemler.
void rTest_glGetBufferParameteriv_null_data_pointer()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_null_data_pointer()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
    while (glGetError() != GL_NO_ERROR) {}

    // Spesifikasyonda tanimsizdir; amac implementasyonun crash olmadan hayatta kalip kalmadigini gozlemlemektir.
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, NULL);

    GLenum err = glGetError();
    printf("[INFO] NULL data pointer => err=0x%X\n", err);
    printf("[PASS] Implementation did not crash on NULL output pointer.\n");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// data, gecersiz/erisilemez (dangling) bir bellek adresi oldugunda implementasyonun bellek koruma ihlaline karsi davranisini test eder.
void rTest_glGetBufferParameteriv_dangling_data_pointer()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_dangling_data_pointer()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_DRAW);
    while (glGetError() != GL_NO_ERROR) {}

    GLint *freedPtr = (GLint *)malloc(sizeof(GLint));
    free(freedPtr); // artik gecersiz

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, freedPtr);

    GLenum err = glGetError();
    printf("[INFO] Dangling data pointer call completed. err=0x%X\n", err);
    printf("[PASS] Implementation did not crash on dangling pointer (best-effort check).\n");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// GL_BUFFER_SIZE sorgusunun, glBufferData ile ayrilan gercek boyutla tutarli olup olmadigini;
// sifir boyutlu bir bufferda dogru sekilde 0 dondurup dondurmedigini doğrular.
void rTest_glGetBufferParameteriv_size_consistency()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_size_consistency()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    // Baslangicta (storage olusmadan once) boyut 0 olmali.
    GLint initialSize = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &initialSize);

    GLenum errInitial = glGetError();
    printf("[INFO] Initial (pre-BufferData) size=%d, err=0x%X\n", initialSize, errInitial);

    const GLsizeiptr allocSize = 256;
    glBufferData(GL_ARRAY_BUFFER, allocSize, NULL, GL_STATIC_DRAW);

    GLint size = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {printf("[FAIL] Unexpected error querying size: 0x%X\n", err);}
    else if (size != (GLint)allocSize) {printf("[FAIL] Size mismatch: expected %ld, got %d\n", (long)allocSize, size);}
    else if (initialSize != 0) {printf("[FAIL] Initial size before glBufferData was not 0 (got %d)\n", initialSize);}
    else {printf("[PASS] Buffer size query consistent with allocation, initial value 0.\n");}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// GL_BUFFER_USAGE'in initial degerinin spesifikasyona uygun sekilde GL_STATIC_DRAW oldugunu,
// ve glBufferData sonrasi degisen usage degerlerinin dogru yansitilip yansitilmadigini doğrular.
void rTest_glGetBufferParameteriv_usage_initial_and_updates()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_usage_initial_and_updates()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);

    GLint initialUsage = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &initialUsage);
    printf("[INFO] Initial usage (pre-BufferData) = 0x%X (expected GL_STATIC_DRAW=0x%X)\n",
    initialUsage, GL_STATIC_DRAW);

    GLenum usages[] = { GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW };
    int n = sizeof(usages) / sizeof(usages[0]);
    int allCorrect = (initialUsage == GL_STATIC_DRAW);

    for (int i = 0; i < n; i++) {
        glBufferData(GL_ARRAY_BUFFER, 16, NULL, usages[i]);
        GLint got = -1;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &got);
        printf("[INFO] Set usage=0x%X => queried=0x%X\n", usages[i], got);
        if ((GLenum)got != usages[i]) allCorrect = 0;
    }

    if (allCorrect) printf("[PASS] Usage values consistent across updates.\n");
    else printf("[FAIL] Usage value mismatch detected.\n");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// target ve value her ikisi de gecersiz oldugunda hangi hatanin (spesifikasyon oncelik belirtmiyor)
// uretildigini gozlemler - implementasyon tutarli bir siraya sahip olmali (crash olmamali).
void rTest_glGetBufferParameteriv_both_invalid()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_both_invalid()\n");

    GLint data = 0x1234;
    glGetBufferParameteriv(0xFFFFFFFF, 0xDEADBEEF, &data);
    GLenum err = glGetError();

    printf("[INFO] Both target and value invalid => err=0x%X (data=%d)\n", err, data);
    if (err == GL_INVALID_ENUM) {printf("[PASS] Correctly reported GL_INVALID_ENUM for combined invalid params.\n");}
    else {printf("[FAIL] Unexpected error for combined invalid target/value.\n");}
}

// Silinmis bir buffer, hala ayni target'a bind edilmis GIBI davranildigi (yani silme sonrasi binding'in 0'a dondugu spesifikasyon davranisini)
// dogrulamak icin: sil, tekrar sorgula, GL_INVALID_OPERATION beklenir (cunku binding otomatik olarak 0'a doner).
void rTest_glGetBufferParameteriv_after_delete_binding_reverts()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_after_delete_binding_reverts()\n");

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_DRAW);
    glDeleteBuffers(1, &buf); // spec: bind edilmis buffer silinirse binding 0'a doner

    GLenum errDelete = glGetError();

    GLint data = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);

    GLenum errQuery = glGetError();
    printf("[INFO] errDelete=0x%X, post-delete query errQuery=0x%X, data=%d\n", errDelete, errQuery, data);

    if (errQuery == GL_INVALID_OPERATION) {printf("[PASS] Correctly reverted to reserved-0 behavior after deleting bound buffer.\n");}
    else {printf("[FAIL] Expected GL_INVALID_OPERATION after delete (binding should revert to 0).\n");}
}

// Ayni buffer nesnesi hem GL_ARRAY_BUFFER hem GL_ELEMENT_ARRAY_BUFFER hedeflerine ayni anda bind edildiginde (OpenGL'de gecerli bir durum,
// buffer'lar target-agnostic'tir) her iki target uzerinden sorgunun tutarli sonuc verip vermedigini doğrular.
void rTest_glGetBufferParameteriv_same_buffer_multiple_targets()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_same_buffer_multiple_targets()\n");

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

    printf("[INFO] sizeViaArray=%d, sizeViaElement=%d\n", sizeViaArray, sizeViaElement);

    if (sizeViaArray == sizeViaElement && sizeViaArray == 100) {
        printf("[PASS] Buffer object state consistent across different bind targets.\n");
    }
    else {printf("[FAIL] Inconsistent buffer state across targets - possible aliasing bug.\n");}

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// data cikis parametresi unaligned bir adres oldugunda implementasyonun crash olmadan davranip davranmadigini test eder.
void rTest_glGetBufferParameteriv_unaligned_data_pointer()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_unaligned_data_pointer()\n");

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
    printf("[INFO] Unaligned data pointer => err=0x%X\n", err);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buf);
}

// Cok sayida ardisik gecersiz cagriyla error state'inin (glGetError kuyrugu) tasip tasmadigini / birikip birikmedigini
// kontrol eder (implementasyonun error flag mekanizmasinin kararliligi).
void rTest_glGetBufferParameteriv_error_state_stress() {
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGetBufferParameteriv_error_state_stress()\n");

    GLint data;
    const int ITER = 10000;
    for (int i = 0; i < ITER; i++) {
        glGetBufferParameteriv(GL_TEXTURE_2D, GL_BUFFER_SIZE, &data); // her seferinde INVALID_ENUM}

        // OpenGL spesifikasyonu: art arda hatalar sadece "en eski" hatayi tutar, yeni hata bayragi eklenmez.
        GLenum err1 = glGetError();
        GLenum err2 = glGetError(); // ikinci cagri GL_NO_ERROR donmeli
        printf("[INFO] After %d invalid calls: err1=0x%X, err2=0x%X\n", ITER, err1, err2);

        if (err1 == GL_INVALID_ENUM && err2 == GL_NO_ERROR) {
            printf("[PASS] Error flag behaves as sticky single-flag per spec.\n");
        }
        else {printf("[FAIL] Unexpected error queue behavior under stress.\n");}
    }
}