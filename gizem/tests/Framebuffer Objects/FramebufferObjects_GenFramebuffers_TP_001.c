#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// void glGenFramebuffers(GLsizei n, GLuint * framebuffers);
// OpenGL’den senin için yeni framebuffer ID numaraları üretmesini ister.
// n: Kaç tane framebuffer ID’si istiyorsun.
// framebuffers: Bu ID’lerin yazılacağı GLuint dizisinin adresi


// Belirtilen hata: GL_INVALID_VALUE is generated if n is negative.
void FramebufferObjects_GenFramebuffers_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}
    GLuint framebuffer = 0;
    glGenFramebuffers(-1, &framebuffer);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE){
        printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
    }
    printf("[PASS] rTest_glGenFramebuffers_invalid_value()\n");
}


// belirtilmeyen hatalar ------------------------------


// n negatif oldugunda GL_INVALID_VALUE uretilip cikis dizisinin dokunulmadan kaldigini doğrular.
void FramebufferObjects_GenFramebuffers_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint sentinel = 0xDEADBEEF;
    GLuint buf = sentinel;
    glGenFramebuffers(-1, &buf);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {printf("[FAIL] Expected GL_INVALID_VALUE for negative n, got 0x%X\n", err);}
    else if (buf != sentinel) {printf("[FAIL] Output buffer modified despite GL_INVALID_VALUE (buf=0x%X)\n", buf);}
    else {printf("[PASS] Negative n correctly rejected, output untouched.\n");}
}

// n = INT_MIN gibi asiri negatif bir degerle (olasi integer overflow'a
// karsi) implementasyonun crash olmadan hayatta kalip kalmadigini doğrular.
void FramebufferObjects_GenFramebuffers_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint buf = 0x12345678;
    glGenFramebuffers(INT_MIN, &buf);

    GLenum err = glGetError();
    printf("[INFO] n=INT_MIN => err=0x%X, buf unchanged=%s\n",
    err, (buf == 0x12345678) ? "yes" : "no");
    if (err == GL_NO_ERROR) {printf("[FAIL] INT_MIN accepted without error - potential overflow risk.\n");}
    else {printf("[PASS] Implementation survived INT_MIN without crash.\n");}
}

// n = 0 ve framebuffers = NULL kombinasyonunun crash olmadan/hatasiz
// gecmesi beklenir (yazilacak eleman yok).
void FramebufferObjects_GenFramebuffers_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    glGenFramebuffers(0, NULL);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {printf("[FAIL] glGenFramebuffers(0, NULL) produced unexpected error 0x%X\n", err);}
    else {printf("[PASS] glGenFramebuffers(0, NULL) did not crash and produced no error.\n");}
}

// n > 0 iken framebuffers = NULL verildiginde (spesifikasyon tanimsiz
// birakiyor) implementasyonun segfault yerine tutarli davranip
// davranmadigini gozlemler.
void FramebufferObjects_GenFramebuffers_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    glGenFramebuffers(4, NULL);

    GLenum err = glGetError();
    printf("[INFO] glGenFramebuffers(4, NULL) survived. err=0x%X\n", err);
    printf("[PASS] Implementation did not crash on NULL output pointer (n>0).\n");
}

// Dangling bir pointer'a yazma denemesinin implementasyonun bellek korumasina karsi davranisini test eder.
void FramebufferObjects_GenFramebuffers_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint *freedPtr = (GLuint *)malloc(sizeof(GLuint) * 4);
    free(freedPtr); // artik gecersiz
    glGenFramebuffers(4, freedPtr);

    GLenum err = glGetError();
    printf("[INFO] Call with dangling pointer completed. err=0x%X\n", err);
    printf("[PASS] Implementation did not crash on dangling pointer (best-effort check).\n");
}

// glGenFramebuffers'in yalnizca kendisine ayrilan alana yazip yazmadigini
// anlamak icin etrafini "canary" degerlerle sardigimiz bir buffer uzerinde
// tasma testi yapar. Eger fonksiyon cb.data disina tasip cb.before veya
// cb.after'i bozarsa buffer overflow var demektir.
void FramebufferObjects_GenFramebuffers_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    typedef struct {
    GLuint before;
    GLuint data[4];
    GLuint after;
    } CanaryBuf;

    CanaryBuf cb;
    cb.before = 0xCAFEBABE;
    cb.after = 0xCAFEBABE;
    memset(cb.data, 0, sizeof(cb.data));
    glGenFramebuffers(4, cb.data);

    GLenum err = glGetError();
    if (cb.before != 0xCAFEBABE || cb.after != 0xCAFEBABE) {printf("[FAIL] Buffer overflow detected! before=0x%X after=0x%X\n", cb.before, cb.after);}
    else if (err != GL_NO_ERROR) {printf("[FAIL] Unexpected error: 0x%X\n", err);}
    else {printf("[PASS] No overflow into adjacent memory detected, canaries intact.\n");}

    glDeleteFramebuffers(4, cb.data);
}

// Cok buyuk n degeriyle (INT_MAX) cagirarak bellek tahsis hatalarinin
// crash yerine tutarli sekilde ele alinip alinmadigini gozlemler.
void FramebufferObjects_GenFramebuffers_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    const GLsizei HUGE_N = INT_MAX;
    GLuint *framebuffers = (GLuint *)malloc((size_t)HUGE_N * sizeof(GLuint));
    if (!framebuffers) {
    printf("[INFO] Host allocation failed before even calling GL - skipping.\n");
    return;
    }

    glGenFramebuffers(HUGE_N, framebuffers);

    GLenum err = glGetError();
    printf("[INFO] glGenFramebuffers(INT_MAX, ...) => err=0x%X\n", err);
    printf("[PASS] Implementation handled huge n without crashing process.\n");

    free(framebuffers);
}

// Dizinin gercek boyutundan buyuk bir n ile cagirarak stack/heap
// tasmasi (yanlis kullanim senaryosu) altinda kararliligi test eder.
void FramebufferObjects_GenFramebuffers_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint smallArray[2]; // kasitli olarak kucuk
    glGenFramebuffers(2, smallArray);
    GLenum err1 = glGetError();

    // KASITLI HATALI KULLANIM: n, dizinin gercek boyutundan buyuk.
    glGenFramebuffers(8, smallArray);
    GLenum err2 = glGetError();

    printf("[INFO] Correct-size call err=0x%X, oversized call err=0x%X\n", err1, err2);
    printf("[PASS] Test completed without process crash (best-effort detection).\n");

    glDeleteFramebuffers(2, smallArray);
}

// Ayni n ile ardisik cok sayida glGenFramebuffers/glDeleteFramebuffers
// dongusuyle isim havuzunun (name pool) tukenmesi/wrap-around durumunu test eder
void FramebufferObjects_GenFramebuffers_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int ITERATIONS = 100000;
    const GLsizei BATCH = 10;
    GLuint batch[BATCH];
    int failedAt = -1;

    for (int i = 0; i < ITERATIONS; i++) {
        glGenFramebuffers(BATCH, batch);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            failedAt = i;
            break;
        }
        glDeleteFramebuffers(BATCH, batch);
    }
    if (failedAt >= 0) printf("[INFO] Name generation failed at iteration %d\n", failedAt);
    else printf("[INFO] Completed %d iterations without failure.\n", ITERATIONS);

    printf("[PASS] No crash observed during repeated generate/delete cycles.\n");
}

// Uretilen isimlerin, hic bind edilmeden silinip silinemedigini ve
// glIsFramebuffer sorgusunun tutarli davranip davranmadigini doğrular
// (spec: "no framebuffer objects are associated until first bound").
void FramebufferObjects_GenFramebuffers_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    GLboolean isFboBeforeBind = glIsFramebuffer(fbo);
    glDeleteFramebuffers(1, &fbo);

    GLenum err = glGetError();
    printf("[INFO] glIsFramebuffer before any bind=%s (expect GL_FALSE per spec), delete err=0x%X\n",
    isFboBeforeBind ? "GL_TRUE" : "GL_FALSE", err);

    if (isFboBeforeBind == GL_FALSE && err == GL_NO_ERROR) {printf("[PASS] Unbound name correctly reported as non-framebuffer, deleted cleanly.\n");}
    else {printf("[FAIL] Unexpected lifecycle behavior for never-bound name.\n");}
}

// Ayni ismin silme sonrasi tekrar uretilip uretilmedigini (isim geri
// donusum/reuse politikasini) crash olmadan gozlemler.
void FramebufferObjects_GenFramebuffers_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint first;
    glGenFramebuffers(1, &first);
    glDeleteFramebuffers(1, &first);

    GLuint second;
    glGenFramebuffers(1, &second);

    printf("[INFO] first=%u, second=%u (reuse=%s)\n", first, second, (first == second) ? "yes" : "no");
    printf("[PASS] No crash across generate-delete-generate cycle.\n");

    glDeleteFramebuffers(1, &second);
}

// Unaligned bir cikis pointer'iyla cagrildiginda
// implementasyonun crash olmadan davranip davranmadigini test eder.
void FramebufferObjects_GenFramebuffers_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    unsigned char rawBuffer[64];
    memset(rawBuffer, 0xAA, sizeof(rawBuffer));
    GLuint *unaligned = (GLuint *)(rawBuffer + 1); // kasitli hizalama bozuklugu

    glGenFramebuffers(4, unaligned);

    GLenum err = glGetError();
    printf("[INFO] Unaligned pointer call => err=0x%X\n", err);
    printf("[PASS] Implementation did not crash with unaligned pointer.\n");
}

// Farkli n degerleriyle (gecerli/gecersiz karisik) art arda cagrilarak
// implementasyonun genel kararliligini test eder.
void FramebufferObjects_GenFramebuffers_TC_014()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLsizei testValues[] = { 1, 0, -1, 2, 100000, 0, -100 };
    int numTests = sizeof(testValues) / sizeof(testValues[0]);

    for (int i = 0; i < numTests; i++) {
        GLsizei n = testValues[i];
        GLuint *arr = (n > 0) ? (GLuint *)malloc(sizeof(GLuint) * (size_t)n) : NULL;

        glGenFramebuffers(n, arr);
        GLenum err = glGetError();
        printf("[INFO] n=%d => err=0x%X\n", n, err);
        if (arr) {
            glDeleteFramebuffers(n, arr);
            free(arr);
        }
    }
    printf("[PASS] Varying n sequence completed without crash.\n");
}

// glGenFramebuffers ve glGenRenderbuffers'in (ve varsa glGenBuffers'in)
// aynı isim uzayini paylasip paylasmadigini (paylasmamasi gerekir - her
// obje turunun kendi ad uzayi olmali) doğrular. Farkli obje turleri
// arasinda isim carpismasi implementasyonun ic handle tablosunda ciddi
// bir tasarim hatasina isaret eder.
void FramebufferObjects_GenFramebuffers_TC_015()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, rb;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rb);

    // fbo ismini renderbuffer olarak, rb ismini framebuffer olarak
    // yorumlamayi dene - glIsFramebuffer/glIsRenderbuffer ile capraz kontrol.
    GLboolean fboIsRb = glIsRenderbuffer(fbo);
    GLboolean rbIsFbo = glIsFramebuffer(rb);

    printf("[INFO] fbo(%u) as renderbuffer=%s, rb(%u) as framebuffer=%s\n",
    fbo, fboIsRb ? "GL_TRUE" : "GL_FALSE",
    rb, rbIsFbo ? "GL_TRUE" : "GL_FALSE");

    // Isimler numerik olarak ortusebilir (her ikisi de kucuk sayidan
    // baslayabilir) ama tur olarak birbirine karismamalidir.
    if (fboIsRb == GL_FALSE && rbIsFbo == GL_FALSE) {printf("[PASS] Namespace correctly isolated between object types.\n");}
    else {printf("[FAIL] Cross-type namespace confusion detected - potential type-safety bug.\n");}

    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rb);
}

