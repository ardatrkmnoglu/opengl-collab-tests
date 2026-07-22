#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen baslangic durumuna getirir ve hata kuyrugunu temizler.
 * checkStatePreserved: Gecersiz cagrilarin GL_CULL_FACE
 * durumunu degistirmedigini dogrular.
 * ============================================================ */

static void resetState(void)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    while(glGetError()!=GL_NO_ERROR);
}

static void checkStatePreserved(GLboolean expected)
{
    GLboolean actual=glIsEnabled(GL_CULL_FACE);
    if(actual!=expected)
    {
        printf("  [FAIL] Durum bozuldu: beklenen=%d gercek=%d\n",
               expected,
               actual);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Basic Robustness
 *
 * glEnable/glDisable(GL_CULL_FACE) sozlesmesini dogrular.
 * Idempotent davranis, invalid enum ve durum korunumu test edilir.
 * ============================================================ */

void test_enableCullFace_basicRobustness(void)
{
    printf("TEST: Basic Robustness\n");

    resetState();

    glEnable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_TRUE);

    glEnable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_TRUE);

    glDisable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_FALSE);

    glDisable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_FALSE);

    glEnable((GLenum)0x0BAD);
    assert(glGetError()==GL_INVALID_ENUM);
    checkStatePreserved(GL_FALSE);

    glDisable((GLenum)0x0BAD);
    assert(glGetError()==GL_INVALID_ENUM);
    checkStatePreserved(GL_FALSE);

    resetState();

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Stress Sweep
 *
 * Tum enum uzayi taranir.
 * Sadece GL_CULL_FACE kabul edilmelidir.
 * ============================================================ */

void test_enableCullFace_stressSweep(void)
{
    unsigned int i;
    int passCount=0;
    int failCount=0;

    printf("TEST: Stress Sweep\n");

    resetState();

    for(i=0;i<=0xFFFF;i++)
    {
        GLenum cap=(GLenum)i;
        GLenum expected;

        if(cap==GL_CULL_FACE)
            expected=GL_NO_ERROR;
        else
            expected=GL_INVALID_ENUM;

        glEnable(cap);

        if(glGetError()==expected)
            passCount++;
        else
            failCount++;
        resetState();
    }

    printf("  Sonuc: %d PASS %d FAIL\n",
           passCount,
           failCount);

    assert(failCount==0);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Hata kuyrugu butunlugu
 *
 * Ard arda gecersiz capability degerleri gonderildiginde hata
 * kuyrugunun dogru sekilde olustugunu, bosaltildigini ve
 * temizlendikten sonra gecerli cagrilarin normal calistigini
 * dogrular.
 * ============================================================ */

void test_cullFaceEnable_errorQueue(void) {
    int i;
    GLenum err;
    int errorCount = 0;

    printf("TEST: Error Queue Management\n");
    resetState();

    for(i=0;i<100;i++)
        glEnable((GLenum)(0x0BAD+i));

    while((err=glGetError())!=GL_NO_ERROR){
        assert(err==GL_INVALID_ENUM);
        errorCount++;
    }

    printf("  Kuyruktan okunan hata sayisi: %d\n",errorCount);
    assert(errorCount>0);

    glEnable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE)==GL_TRUE);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Gecersiz capability degerleri
 *
 * glEnable ve glDisable'e farkli gecersiz capability degerleri
 * gondererek implementasyonun tumunu GL_INVALID_ENUM ile
 * reddettigini dogrular. Reddedilen cagrilar mevcut durumu
 * degistirmemelidir.
 * ============================================================ */

void test_cullFaceEnable_invalidCaps(void) {
    GLenum invalidCaps[]={
        (GLenum)0x0000,
        (GLenum)0x0BAD,
        (GLenum)0x1234,
        (GLenum)0xDEAD,
        (GLenum)0xFFFF
    };
    int i;
    int n=sizeof(invalidCaps)/sizeof(invalidCaps[0]);

    printf("TEST: Invalid Capability Values\n");
    resetState();

    for(i=0;i<n;i++){
        glEnable(invalidCaps[i]);
        assert(glGetError()==GL_INVALID_ENUM);
        assert(glIsEnabled(GL_CULL_FACE)==GL_FALSE);

        glDisable(invalidCaps[i]);
        assert(glGetError()==GL_INVALID_ENUM);
        assert(glIsEnabled(GL_CULL_FACE)==GL_FALSE);
    }

    resetState();
    printf("  Sonuc: %d gecersiz capability reddedildi\n",n);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Hizli toggle ve durum tutarliligi
 *
 * glEnable/glDisable arasinda hizli gecisler yaparak durum
 * makinesinin tutarliligini dogrular. Her gecisten sonra
 * glIsEnabled beklenen sonucu vermelidir.
 * ============================================================ */

void test_cullFaceEnable_rapidToggle(void) {
    int i;
    const int tekrar=10000;

    printf("TEST: Rapid Toggle (Enable <-> Disable)\n");
    resetState();

    for(i=0;i<tekrar;i++){
        GLboolean expected=(i%2==0)?GL_TRUE:GL_FALSE;

        if(expected)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        assert(glGetError()==GL_NO_ERROR);
        assert(glIsEnabled(GL_CULL_FACE)==expected);
    }

    resetState();
    printf("  Sonuc: %d toggle tamamlandi\n",tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 6: Capability izolasyonu
 *
 * GL_CULL_FACE ile baska capability'lerin birbirinden bagimsiz
 * yonetildigini dogrular. Bir capability'nin enable/disable
 * edilmesi diger capability'nin durumunu degistirmemelidir.
 * ============================================================ */

void test_cullFaceEnable_capIsolation(void) {
    printf("TEST: Capability Isolation\n");
    resetState();

    assert(glIsEnabled(GL_CULL_FACE)==GL_FALSE);
    assert(glIsEnabled(GL_SCISSOR_TEST)==GL_FALSE);

    glEnable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE)==GL_TRUE);
    assert(glIsEnabled(GL_SCISSOR_TEST)==GL_FALSE);

    glEnable(GL_SCISSOR_TEST);
    assert(glGetError()==GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE)==GL_TRUE);
    assert(glIsEnabled(GL_SCISSOR_TEST)==GL_TRUE);

    glDisable(GL_SCISSOR_TEST);
    assert(glGetError()==GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE)==GL_TRUE);
    assert(glIsEnabled(GL_SCISSOR_TEST)==GL_FALSE);

    glDisable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE)==GL_FALSE);
    assert(glIsEnabled(GL_SCISSOR_TEST)==GL_FALSE);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7: Buyuk capability enum degerleri
 *
 * 32-bit enum uzayinin ust kisimlarini test eder.
 * Implementasyon gecersiz capability degerlerini
 * GL_INVALID_ENUM ile reddetmeli ve mevcut durumu
 * bozmamalidir.
 * ============================================================ */

void test_cullFaceEnable_largeEnum(void) {
    GLenum values[]={
        (GLenum)0x10000,
        (GLenum)0x7FFFFFFF,
        (GLenum)0x80000000,
        (GLenum)0xFFFFFFFF
    };
    int i;
    int n=sizeof(values)/sizeof(values[0]);

    printf("TEST: Large Capability Values\n");
    resetState();

    for(i=0;i<n;i++){
        glEnable(values[i]);
        printf("  Enable(0x%08X) -> 0x%X\n",
               values[i],glGetError());

        glDisable(values[i]);
        printf("  Disable(0x%08X) -> 0x%X\n",
               values[i],glGetError());
    }

    assert(glIsEnabled(GL_CULL_FACE)==GL_FALSE);

    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 8: Rapid Fire
 *
 * glEnable/glDisable fonksiyonlarini minimum gecikmeyle
 * art arda cagirarak durum makinesinin uzun sureli yuk
 * altinda tutarli kaldigini dogrular.
 * ============================================================ */

void test_cullFaceEnable_rapidFire(void) {
    int i;
    const int tekrar=50000;

    printf("TEST: Rapid Fire (50K calls)\n");
    resetState();

    for(i=0;i<tekrar;i++){
        glEnable(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
    }

    assert(glGetError()==GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE)==GL_FALSE);

    glEnable(GL_CULL_FACE);
    assert(glGetError()==GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE)==GL_TRUE);

    resetState();
    printf("  Sonuc: %d cift cagri tamamlandi\n",tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * Tum glEnable/glDisable(GL_CULL_FACE) Robustness Testlerini Calistir
 * ============================================================ */

void Run_glEnableCullFace_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf(" glEnable/glDisable(GL_CULL_FACE) Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_enableCullFace_basicRobustness();
    test_enableCullFace_stressSweep();
    test_cullFaceEnable_errorQueue();
    test_cullFaceEnable_invalidCaps();
    test_cullFaceEnable_rapidToggle();
    test_cullFaceEnable_capIsolation();
    test_cullFaceEnable_largeEnum();
    test_cullFaceEnable_rapidFire();

    printf("=============================================\n");
    printf(" Tum glEnable/glDisable(GL_CULL_FACE)\n");
    printf(" Robustness Testleri Basarili\n");
    printf("=============================================\n\n");
}
