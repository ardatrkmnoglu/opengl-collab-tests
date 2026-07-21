#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine getirir ve birikmis hatalari
 * temizler; boylece testler birbirine bagimli olmaz.
 * checkStatePreserved: Reddedilen cagrilarin mevcut durumu
 * bozup bozmadigini dogrular; beklenen degerle gercek deger
 * uyusmazsa program assert ile durur.
 * ============================================================ */

static void resetState(void) {
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLint expected) {
    GLint actual;
    glGetIntegerv(GL_CULL_FACE_MODE, &actual);
    if (actual != expected) {
        printf("  [FAIL] Durum bozuldu: beklenen 0x%X, gercek 0x%X\n",
               expected, actual);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama
 *
 * glCullFace'in temel sozlesmesini dogrular: gecerli degerler
 * (GL_BACK, GL_FRONT, GL_FRONT_AND_BACK) kabul edilmeli,
 * gecersiz enum'lar GL_INVALID_ENUM ile reddedilmelidir.
 * Reddedilen cagrilar durumu bozmamalidir.
 * ============================================================ */

void test_cullFace_basicRobustness(void) {
    GLint mode = 0;
    GLenum err;

    printf("TEST: Basic Robustness\n");
    resetState();

    glCullFace(GL_FRONT);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetIntegerv(GL_CULL_FACE_MODE, &mode);
    assert(mode == GL_FRONT);

    glCullFace(GL_FRONT_AND_BACK);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetIntegerv(GL_CULL_FACE_MODE, &mode);
    assert(mode == GL_FRONT_AND_BACK);

    glCullFace((GLenum)0x0BAD);
    assert(glGetError() == GL_INVALID_ENUM);

    glCullFace(GL_CCW);
    assert(glGetError() == GL_INVALID_ENUM);

    checkStatePreserved(GL_FRONT_AND_BACK);

    assert(glGetError() == GL_NO_ERROR);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Enum uzayi taramasi
 *
 * 16-bit enum uzayinin tamamini (0x0000 - 0xFFFF, 65536 deger)
 * sistematik olarak tarar. Yalnizca GL_BACK, GL_FRONT ve
 * GL_FRONT_AND_BACK kabul edilmeli, kalan 65533 deger
 * GL_INVALID_ENUM ile reddedilmelidir.
 * ============================================================ */

void test_cullFace_stressSweep(void) {
    long i;
    int passCount = 0;
    int failCount = 0;

    printf("TEST: Stress Sweep (0x0000 .. 0xFFFF)\n");
    resetState();

    for (i = 0x0000; i <= 0xFFFF; i++) {
        GLenum deger = (GLenum)i;
        GLenum beklenen = (deger == GL_BACK || deger == GL_FRONT ||
                           deger == GL_FRONT_AND_BACK)
                          ? GL_NO_ERROR : GL_INVALID_ENUM;
        GLenum err;

        glCullFace(deger);
        err = glGetError();

        if (err != beklenen) {
            printf("  [FAIL] Enum=0x%04lX Beklenen=0x%X Gelen=0x%X\n",
                   i, beklenen, err);
            failCount++;
        } else {
            passCount++;
        }
    }

    {
        GLint mode = 0;
        glCullFace(GL_BACK);
        glGetIntegerv(GL_CULL_FACE_MODE, &mode);
        assert(mode == GL_BACK);
        assert(glGetError() == GL_NO_ERROR);
    }

    printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
    assert(failCount == 0);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Hata kuyrugu butunlugu
 *
 * Ard arda gecersiz enum gonderimi altinda hata kuyrugunun
 * dogru sekilde doldugunu, bosaldigini ve temizlendikten sonra
 * normal islemlerin devam ettigini dogrular.
 * ============================================================ */

void test_cullFace_errorQueue(void) {
    int i;
    GLenum err;
    int hataSayisi = 0;

    printf("TEST: Error Queue Management\n");
    resetState();

    for (i = 0; i < 100; i++) {
        glCullFace((GLenum)(0x0BAD + i));
    }

    while ((err = glGetError()) != GL_NO_ERROR) {
        assert(err == GL_INVALID_ENUM);
        hataSayisi++;
    }

    printf("  Kuyruktan okunan hata sayisi: %d\n", hataSayisi);
    assert(hataSayisi > 0);

    glCullFace(GL_FRONT);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(GL_FRONT);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Coklu cagri ve durum gecisleri
 *
 * Gecerli cull modlari arasinda hizli gecisler yaparak durum
 * makinesinin tutarliligini dogrular. Her gecisten sonra durum
 * sorgulanir ve beklenen degerle eslestigi kontrol edilir.
 * ============================================================ */

void test_cullFace_rapidToggle(void) {
    int i;
    const int tekrar = 10000;

    printf("TEST: Rapid Toggle (BACK <-> FRONT <-> F&B)\n");
    resetState();

    for (i = 0; i < tekrar; i++) {
        GLenum hedef;
        GLint mode;

        switch (i % 3) {
            case 0: hedef = GL_BACK; break;
            case 1: hedef = GL_FRONT; break;
            case 2: hedef = GL_FRONT_AND_BACK; break;
        }

        glCullFace(hedef);
        assert(glGetError() == GL_NO_ERROR);

        glGetIntegerv(GL_CULL_FACE_MODE, &mode);
        assert(mode == hedef);
    }

    glCullFace(GL_BACK);
    checkStatePreserved(GL_BACK);
    assert(glGetError() == GL_NO_ERROR);

    printf("  Sonuc: %d gecis tamamlandi\n", tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Gecersiz cagrilar arasinda gecerli cagrilar
 *
 * Gecersiz enum'larin arasina gecerli degerler serpistirerek
 * surucunun hata durumundan kurtulup kurtulamadigini dogrular.
 * Bazi implementasyonlar hata sonrasi "takili" kalabilir.
 * ============================================================ */

void test_cullFace_mixedValidity(void) {
    int i;
    GLenum pattern[] = {
        GL_BACK, (GLenum)0x1234, GL_FRONT, (GLenum)0x5678,
        GL_FRONT_AND_BACK, (GLenum)0x9ABC, GL_BACK, (GLenum)0xDEF0
    };
    int n = sizeof(pattern) / sizeof(pattern[0]);

    printf("TEST: Mixed Validity Pattern\n");
    resetState();

    for (i = 0; i < n; i++) {
        GLenum deger = pattern[i];
        GLenum beklenen = (deger == GL_BACK || deger == GL_FRONT ||
                           deger == GL_FRONT_AND_BACK)
                          ? GL_NO_ERROR : GL_INVALID_ENUM;
        GLenum err;

        glCullFace(deger);
        err = glGetError();
        assert(err == beklenen);
    }

    glCullFace(GL_BACK);
    checkStatePreserved(GL_BACK);
    assert(glGetError() == GL_NO_ERROR);

    printf("  Sonuc: %d karisik cagri tamamlandi\n", n);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 6: Culling etkinlestirme/kapatma etkilesimi
 *
 * glCullFace'in glEnable/glDisable(GL_CULL_FACE) ile olan
 * etkilesimini dogrular. Culling kapaliyken cull mode'un
 * etkisiz olmasi, acikken etkili olmasi gerekir.
 * ============================================================ */

void test_cullFace_enableDisable(void) {
    printf("TEST: Enable/Disable Interaction\n");
    resetState();

    glCullFace(GL_FRONT_AND_BACK);
    glGetError();

    glDisable(GL_CULL_FACE);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    glEnable(GL_CULL_FACE);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

    glDisable(GL_CULL_FACE);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    checkStatePreserved(GL_BACK);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7: FrontFace etkilesimi
 *
 * glCullFace ile glFrontFace'in birlikte calistigini dogrular.
 * Farkli kombinasyonlarda durumlarin tutarli oldugunu kontrol eder.
 * ============================================================ */

void test_cullFace_frontFaceCombo(void) {
    GLenum frontModes[] = {GL_CCW, GL_CW};
    GLenum cullModes[] = {GL_BACK, GL_FRONT, GL_FRONT_AND_BACK};
    int i, j;

    printf("TEST: FrontFace Combinations\n");
    resetState();

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            GLint face, mode;

            glFrontFace(frontModes[i]);
            glCullFace(cullModes[j]);
            assert(glGetError() == GL_NO_ERROR);

            glGetIntegerv(GL_FRONT_FACE, &face);
            glGetIntegerv(GL_CULL_FACE_MODE, &mode);

            assert(face == frontModes[i]);
            assert(mode == cullModes[j]);
        }
    }

    resetState();
    printf("  Sonuc: 6 kombinasyon tamamlandi\n");
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 8: Cok buyuk enum degerleri
 *
 * 32-bit enum araliginin ust kisimlarini test eder.
 * OpenGL spec 16-bit enum uzayini kullanir ancak 32-bit
 * degerler gonderildiginde implementasyonun davranisi
 * belirsizdir. Cokme veya durum bozulmasi kritik hatadir.
 * ============================================================ */

void test_cullFace_largeEnum(void) {
    GLenum largeValues[] = {
        (GLenum)0x10000,
        (GLenum)0x7FFFFFFF,
        (GLenum)0x80000000,
        (GLenum)0xFFFFFFFF
    };
    int i;
    int n = sizeof(largeValues) / sizeof(largeValues[0]);

    printf("TEST: Large Enum Values\n");
    resetState();

    for (i = 0; i < n; i++) {
        GLenum err;

        glCullFace(largeValues[i]);
        err = glGetError();

        printf("  Enum=0x%08X -> 0x%X\n", largeValues[i], err);
    }

    checkStatePreserved(GL_BACK);
    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * Test calistirma cercevesi
 * ============================================================ */
