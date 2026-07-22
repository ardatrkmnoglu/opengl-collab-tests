#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


// void glBindFramebuffer(GLenum target, GLuint framebuffer);
// OpenGL’de hangi framebuffer’a çizim yapılacağını ve hangi framebuffer’dan okunacağını seçmek için kullanılır
// target: Modern çekirdekte genelde hep GL_FRAMEBUFFER kullanılır.
// framebuffer = 0: Varsayılan framebuffer
// framebuffer = 0 olmayan bir id dersen: Senin glGenFramebuffers ile oluşturduğun, “off-screen” (ekran dışında) bir framebuffer’a geçersin.


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.
void rTest_glBindFramebuffer_invalid_enum()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_invalid_enum()\n");

    glBindFramebuffer((GLenum)0xFFFFFFFF, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM){
    printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
    assert(err == GL_INVALID_ENUM);
    }
    printf("[PASS] rTest_glBindFramebuffer_invalid_enum()\n");
}

// belirtilmeyen hatalar ------------------------------

// target parametresi GL_FRAMEBUFFER disinda bir enum oldugunda GL_INVALID_ENUM uretilip onceki binding'in bozulmadigini doğrular.
void rTest_glBindFramebuffer_invalid_target()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_invalid_target()\n");

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    glBindFramebuffer(GL_RENDERBUFFER, fbo); // gecersiz target

    GLenum err = glGetError();

    GLint currentBinding = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBinding);

    if (err != GL_INVALID_ENUM) {printf("[FAIL] Expected GL_INVALID_ENUM for invalid target, got 0x%X\n", err);}
    else if ((GLuint)currentBinding != fbo) {printf("[FAIL] Previous binding was altered by invalid call (binding=%d, expected=%u)\n",currentBinding, fbo);}
    else {printf("[PASS] Invalid target rejected, previous binding preserved.\n");}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
}

// glGenFramebuffers ile hiç uretilmemis, rastgele/keyfi bir framebuffer ismiyle bind cagirildiginda davranisi test eder
void rTest_glBindFramebuffer_arbitrary_unused_name()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_arbitrary_unused_name()\n");

    GLuint arbitraryName = 0xABCD1234;
    glBindFramebuffer(GL_FRAMEBUFFER, arbitraryName);

    GLenum err = glGetError();

    GLboolean isFbo = glIsFramebuffer(arbitraryName);

    printf("[INFO] Arbitrary unused name bind => err=0x%X, glIsFramebuffer=%s\n", err, isFbo ? "GL_TRUE" : "GL_FALSE");
    printf("[PASS] Implementation did not crash with arbitrary/ungenerated name.\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Eger implicit olusturulduysa temizlemeyi dene (guvenli, gecersizse etkisi yok)
    glDeleteFramebuffers(1, &arbitraryName);
}

// Yeni oluşturulup ilk kez bind edilen bir framebuffer'ın başlangıçta herhangi bir attachment içermediğini doğrular
// GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT ve GL_STENCIL_ATTACHMENT noktalarının OBJECT_TYPE'ı,
// bu noktalara bir renderbuffer veya texture bağlanana kadar GL_NONE olmalıdır
void rTest_glBindFramebuffer_initial_attachment_state()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_initial_attachment_state()\n");

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); // ilk bind

    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
    int allNone = 1;

    for (int i = 0; i < 3; i++) {
        GLint objType = -1;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachments[i], GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        printf("[INFO] attachment=0x%X objType=0x%X (expect GL_NONE=0x%X)\n", attachments[i], objType, GL_NONE);
        if (objType != GL_NONE) allNone = 0;
    }

    if (allNone) printf("[PASS] All attachment points initialized to GL_NONE on first bind.\n");
    else printf("[FAIL] One or more attachment points not initialized to GL_NONE.\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
}

// framebuffer = 0 iken (varsayilan) target uzerinde query/modify islemlerinin GL_INVALID_OPERATION ile reddedildigini doğrular
// (spec: "While framebuffer object name zero is bound... GL_INVALID_OPERATION").
void rTest_glBindFramebuffer_zero_bound_query_rejected()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_zero_bound_query_rejected()\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    // 0 bagliyken FBO-spesifik bir modify/query denemesi (glFramebufferRenderbuffer uzerinden dolayli
    // olarak dogrulaniyor, cunku glBindFramebuffer'in kendisi baglama disinda dogrudan bir query API'si sunmuyor).
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);

    GLenum err = glGetError();
    printf("[INFO] Attempt to modify attachment with FBO 0 bound => err=0x%X\n", err);
    if (err == GL_INVALID_OPERATION) {printf("[PASS] Correctly rejected modification while default FBO bound.\n");}
    else {printf("[FAIL] Expected GL_INVALID_OPERATION per spec note.\n");}
}

// Eğer GL_FRAMEBUFFER hedefine bağlı olan bir FBO (framebuffer nesnesi) glDeleteFramebuffers ile silinirse,
// OpenGL spesifikasyonuna göre GL_FRAMEBUFFER_BINDING durumunun otomatik olarak
// varsayılan framebuffer’a (0) dönmesi gerekir.
// Yani: "silinen FBO'ya işaret eden dangling bir binding kalmamalı".
void rTest_glBindFramebuffer_binding_reverts_after_delete()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_binding_reverts_after_delete()\n");

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glDeleteFramebuffers(1, &fbo);

    GLenum errDelete = glGetError();

    GLint currentBinding = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBinding);

    printf("[INFO] errDelete=0x%X, binding after delete=%d (expect 0)\n", errDelete, currentBinding);
    if (currentBinding == 0) {printf("[PASS] Binding correctly reverted to 0 after deleting bound FBO.\n");}
    else {printf("[FAIL] Binding did not revert to 0 - dangling binding risk.\n");}
}

// Ayni framebuffer'in ust uste ayni isimle tekrar tekrar bind edilmesinin (no-op olmasi beklenen durum)
// implementasyonu bozup bozmadigini (orn. state'i sifirlayip sifirlamadigini) test eder.
void rTest_glBindFramebuffer_repeated_rebind_same_name()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_repeated_rebind_same_name()\n");

    GLuint fbo, rb;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb);

    for (int i = 0; i < 1000; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo); // ayni isim, tekrar tekrar
    }

    GLenum err = glGetError();

    GLint objType = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);

    printf("[INFO] After 1000x rebind: err=0x%X, attachment objType=0x%X (expect GL_RENDERBUFFER)\n", err, objType);

    if (err == GL_NO_ERROR && objType == GL_RENDERBUFFER) {printf("[PASS] Repeated rebinding did not corrupt or reset FBO state.\n");}
    else {printf("[FAIL] State corrupted or error occurred during repeated rebind.\n");}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rb);
}

// glGenFramebuffers ile hiç üretilmemiş, aşırı/uç framebuffer isimleri
// (örneğin UINT_MAX’e çok yakın değerler) ile glBindFramebuffer çağrıldığında,
// sürücünün/implementasyonun iç kaynak tablosunda (örneğin isimleri dizi indeksi
// gibi kullanan basit implementasyonlarda) out-of-bounds erişim, bellek hatası
// ya da crash olup olmadığını kontrol eder.
void rTest_glBindFramebuffer_extreme_name_value()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_extreme_name_value()\n");

    GLuint extremeNames[] = { 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000 };
    int n = sizeof(extremeNames) / sizeof(extremeNames[0]);

    for (int i = 0; i < n; i++) {
        while (glGetError() != GL_NO_ERROR) {}
        glBindFramebuffer(GL_FRAMEBUFFER, extremeNames[i]);
        GLenum err = glGetError();
        printf("[INFO] name=0x%X => err=0x%X\n", extremeNames[i], err);
    }
    printf("[PASS] Extreme framebuffer name values did not crash the implementation.\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// OpenGL nesne isimlerinin *bağlamdan bağımsız düz birer GLuint* olması
// gerçeğini kullanarak, “tip karışıklığı” durumunda sürücünün dayanıklılığını sınar.
// Senaryo:
// - glGenRenderbuffers ile bir renderbuffer ismi (rb) üretiyoruz.
// - Bu renderbuffer ismini, sanki bir framebuffer adıymış gibi kullanıyoruz.
void rTest_glBindFramebuffer_type_confusion_with_other_object()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_type_confusion_with_other_object()\n");

    GLuint rb;
    glGenRenderbuffers(1, &rb);
    glBindFramebuffer(GL_FRAMEBUFFER, rb); // renderbuffer ismini framebuffer olarak kullan

    GLenum err = glGetError();
    printf("[INFO] Using renderbuffer name as framebuffer name => err=0x%X\n", err);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteRenderbuffers(1, &rb);
}

// glBindFramebuffer ile “sık ve tekrarlı bind/unbind (0’a dön)”
// işlemlerinin sürücüde zamanla bozulma, kaynak sızıntısı veya kararsızlık
// (örneğin hataya düşme, iç durumun bozulması) üretip üretmediğini ölçer.
void rTest_glBindFramebuffer_bind_unbind_stress()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_bind_unbind_stress()\n");

    const int ITER = 5000;
    GLuint fbos[8];
    glGenFramebuffers(8, fbos);

    for (int i = 0; i < ITER; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[i % 8]); //bind
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
    }

    GLenum err = glGetError();
    printf("[INFO] %d bind/unbind cycles completed. err=0x%X\n", ITER, err);

    glDeleteFramebuffers(8, fbos);
}

// Bir framebuffer bağlıyken (nonzero), farklı bir framebuffer'a gecis yapildiginda önceki
// binding’in gerçekten bırakılıp bırakılmadığını doğrular (GL_FRAMEBUFFER_BINDING query'si ile)
// (spec: "previous binding is automatically broken")
void rTest_glBindFramebuffer_previous_binding_broken_on_switch()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_previous_binding_broken_on_switch()\n");

    GLuint fbo1, fbo2;
    glGenFramebuffers(1, &fbo1);
    glGenFramebuffers(1, &fbo2);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
    GLint b1 = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &b1); //binding == fbo1 olmalı

    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    GLint b2 = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &b2); //binding == fbo2 olmalı (önceki fbo bırakılmalı)

    printf("[INFO] After bind fbo1: binding=%d (expect %u), after bind fbo2: binding=%d (expect %u)\n", b1, fbo1, b2, fbo2);
    if ((GLuint)b1 == fbo1 && (GLuint)b2 == fbo2 && b1 != b2) {printf("[PASS] Binding correctly switches, previous binding broken.\n");}
    else {printf("[FAIL] Binding state inconsistent across switch.\n");}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo1);
    glDeleteFramebuffers(1, &fbo2);
}

// Halihazirda bind edilmis olan framebuffer'in tekrar ayni isimle bind
// edilmesinin (self-rebind) attachment durumunu sifirlayip sifirlamadigini test eder
// - spec bu konuda sessiz, "ilk bind" ile "sonraki bind"ler arasinda fark olmamali.
void rTest_glBindFramebuffer_self_rebind_preserves_state()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindFramebuffer_self_rebind_preserves_state()\n");

    GLuint fbo, rb;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 8, 8);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    // Baska bir seye gecmeden dogrudan ayni fbo'yu tekrar bind et.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLint objType = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);

    printf("[INFO] Attachment objType after self-rebind=0x%X (expect GL_RENDERBUFFER)\n", objType);
    if (objType == GL_RENDERBUFFER) {printf("[PASS] Self-rebind preserved existing attachment state correctly.\n");}
    else {printf("[FAIL] Self-rebind unexpectedly reset attachment state.\n");}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rb);
}
