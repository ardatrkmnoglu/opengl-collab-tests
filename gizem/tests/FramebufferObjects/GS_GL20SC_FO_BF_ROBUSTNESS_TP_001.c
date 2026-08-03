//Gizem'de çalışması için
#include <glad/gles2.h>
#include <stdio.h>
#include "../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"


/*
GL20SC - FramebufferObjects - BindFramebuffer - ROBUSTNESS
*/


static const char* test_procedure = "GS_GL20SC_FO_BF_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_FO_BF_ROBUSTNESS_TC_011";


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindFramebuffer((GLenum)0xFFFFFFFF, 0);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM){
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

// belirtilmeyen hatalar ------------------------------

// target parametresi GL_FRAMEBUFFER dışında bir enum olduğunda GL_INVALID_ENUM
// üretilip önceki binding'in bozulmadığını doğrular.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    while (glGetError() != GL_NO_ERROR) {}

    glBindFramebuffer(GL_RENDERBUFFER, fbo); // geçersiz target

    GLenum err = glGetError();

    GLint currentBinding = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBinding);

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM for invalid target, got 0x%x.", err);
    }
    else if ((GLuint)currentBinding != fbo) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Previous binding was altered by invalid call (binding=%d, expected=%u)\n",currentBinding, fbo);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// glGenFramebuffers ile hiç üretilmemiş, rastgele/keyfi bir framebuffer ismiyle bind çağırıldığında davranışı test eder.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint arbitraryName = 0xABCD1234;
    glBindFramebuffer(GL_FRAMEBUFFER, arbitraryName);

    GLenum err = glGetError();

    GLboolean isFbo = glIsFramebuffer(arbitraryName);

    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Implementation did not crash with arbitrary/ungenerated name");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Eğer implicit oluşturulduysa temizlemeyi dene (güvenli, geçersizse etkisi yok).
}

// Yeni oluşturulup ilk kez bind edilen bir framebuffer'ın başlangıçta herhangi bir attachment içermediğini doğrular
// GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT ve GL_STENCIL_ATTACHMENT noktalarının OBJECT_TYPE'ı,
// bu noktalara bir renderbuffer veya texture bağlanana kadar GL_NONE olmalıdır
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); // ilk bind

    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
    int allNone = 1;

    for (int i = 0; i < 3; i++) {
        GLint objType = -1;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachments[i], GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE) allNone = 0;
    }

    if (allNone) TEST_LOG_SUCCESS(test_case_4, test_procedure);
    else TEST_LOG_FAIL(test_case_4, test_procedure, "One or more attachment points not initialized to GL_NONE");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// framebuffer = 0 iken (varsayılan) target üzerinde query/modify işlemlerinin GL_INVALID_OPERATION ile reddedildiğini doğrular.
// (spec: "While framebuffer object name zero is bound... GL_INVALID_OPERATION").
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    // 0 bağlıyken FBO-spesifik bir modify/query denemesi (glFramebufferRenderbuffer üzerinden dolaylı
    // olarak doğrulanıyor, çünkü glBindFramebuffer'in kendisi bağlama dışında doğrudan bir query API'si sunmuyor).
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);

    GLenum err = glGetError();
    if (err == GL_INVALID_OPERATION) {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_INVALID_OPERATION per spec note.");
    }
}

// Aynı framebuffer'in üst üste aynı isimle tekrar tekrar bind edilmesinin (no-op olması beklenen durum)
// implementasyonu bozup bozmadığını (örn. state'i sıfırlayıp sıfırlamadığını) test eder.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, rb;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb);

    for (int i = 0; i < 1000; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo); // aynı isim, tekrar tekrar
    }

    GLenum err = glGetError();

    GLint objType = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);

    if (err == GL_NO_ERROR && objType == GL_RENDERBUFFER) {
        TEST_LOG_SUCCESS(test_case_6, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_6, test_procedure, "State corrupted or error occurred during repeated rebind.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// glGenFramebuffers ile hiç üretilmemiş, aşırı/uç framebuffer isimleri
// (örneğin UINT_MAX’e çok yakın değerler) ile glBindFramebuffer çağrıldığında,
// sürücünün/implementasyonun iç kaynak tablosunda (örneğin isimleri dizi indeksi
// gibi kullanan basit implementasyonlarda) out-of-bounds erişim, bellek hatası
// ya da crash olup olmadığını kontrol eder.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint extremeNames[] = { 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000 };
    int n = sizeof(extremeNames) / sizeof(extremeNames[0]);

    for (int i = 0; i < n; i++) {
        while (glGetError() != GL_NO_ERROR) {}
        glBindFramebuffer(GL_FRAMEBUFFER, extremeNames[i]);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_7, test_procedure, "error = 0x%x.", err);
            return;
        }
    }
    TEST_LOG_SUCCESS(test_case_7, test_procedure);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// OpenGL nesne isimlerinin *bağlamdan bağımsız düz birer GLuint* olması
// gerçeğini kullanarak, “tip karışıklığı” durumunda sürücünün dayanıklılığını sınar.
// Senaryo:
// - glGenRenderbuffers ile bir renderbuffer ismi (rb) üretiyoruz.
// - Bu renderbuffer ismini, sanki bir framebuffer adıymış gibi kullanıyoruz.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rb;
    glGenRenderbuffers(1, &rb);
    glBindFramebuffer(GL_FRAMEBUFFER, rb); // renderbuffer ismini framebuffer olarak kullan

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_8, test_procedure, "error = 0x%x.", err);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// glBindFramebuffer ile “sık ve tekrarlı bind/unbind (0’a dön)”
// işlemlerinin sürücüde zamanla bozulma, kaynak sızıntısı veya kararsızlık
// (örneğin hataya düşme, iç durumun bozulması) üretip üretmediğini ölçer.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    const int ITER = 5000;
    GLuint fbos[8];
    glGenFramebuffers(8, fbos);

    for (int i = 0; i < ITER; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[i % 8]); //bind
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
    }

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_9, test_procedure, "error = 0x%x.", err);
    }
}

// Bir framebuffer bağlıyken (nonzero), farklı bir framebuffer'a geçiş yapıldığında önceki
// bağlamanın gerçekten bırakılıp bırakılmadığını doğrular (GL_FRAMEBUFFER_BINDING sorgusu ile).
// (spec: "previous binding is automatically broken")
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo1, fbo2;
    glGenFramebuffers(1, &fbo1);
    glGenFramebuffers(1, &fbo2);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
    GLint b1 = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &b1); //binding == fbo1 olmalı

    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    GLint b2 = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &b2); //binding == fbo2 olmalı (önceki fbo bırakılmalı)

    if ((GLuint)b1 == fbo1 && (GLuint)b2 == fbo2 && b1 != b2) {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Binding state inconsistent across switch");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Halihazırda bind edilmiş olan framebuffer'in tekrar aynı isimle bind
// edilmesinin (self-rebind) attachment durumunu sıfırlayıp sıfırlamadığını test eder
// - spec bu konuda sessiz; "ilk bind" ile "sonraki bind"ler arasında fark olmamalı.
void GS_GL20SC_FO_BF_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint fbo, rb;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 8, 8);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    // Başka bir şeye geçmeden doğrudan aynı FBO'yu tekrar bind et.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLint objType = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);

    if (objType == GL_RENDERBUFFER) {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
    else {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Self-rebind unexpectedly reset attachment state.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



/* Initialization */
void GS_GL20SC_FO_BF_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_007();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_BF_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_BF_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_BF_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}