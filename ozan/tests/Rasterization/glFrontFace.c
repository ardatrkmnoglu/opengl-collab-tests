#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine (GL_CCW) getirir ve birikmis
 * hatalari temizler; boylece testler birbirine bagimli olmaz.
 * checkStatePreserved: Reddedilen cagrilarin mevcut durumu
 * bozup bozmadigini dogrular; beklenen degerle gercek deger
 * uyusmazsa program assert ile durur. Bu iki fonksiyon olmadan
 * hata ayiklama zorlasir, cunku hangi testin hangi durumu
 * biraktigi takip edilemez.
 * ============================================================ */

static void resetState(void) {
    glFrontFace(GL_CCW);
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLint expected) {
    GLint actual;
    glGetIntegerv(GL_FRONT_FACE, &actual);
    if (actual != expected) {
        printf("  [FAIL] Durum bozuldu: beklenen 0x%X, gercek 0x%X\n",
               expected, actual);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Hata kuyrugu butunlugu
 *
 * Ard arda gecersiz enum gonderimi altinda hata kuyrugunun
 * dogru sekilde doldugunu, bosaldigini ve temizlendikten sonra
 * normal islemlerin devam ettigini dogrular.
 * ============================================================ */

void test_frontFace_errorQueue(void) {
    int i;
    GLenum err;
    int hataSayisi = 0;

    printf("TEST: Error Queue Management\n");
    resetState();

    for (i = 0; i < 100; i++) {
        glFrontFace((GLenum)(0x0BAD + i));
    }

    while ((err = glGetError()) != GL_NO_ERROR) {
        assert(err == GL_INVALID_ENUM);
        hataSayisi++;
    }

    printf("  Kuyruktan okunan hata sayisi: %d\n", hataSayisi);
    assert(hataSayisi > 0);

    glFrontFace(GL_CW);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(GL_CW);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Coklu cagri ve durum gecisleri
 *
 * GL_CW ve GL_CCW arasinda hizli gecisler yaparak durum
 * makinesinin tutarliligini dogrular. Her gecisten sonra
 * durum sorgulanir ve beklenen degerle eslestigi kontrol edilir.
 * ============================================================ */

void test_frontFace_rapidToggle(void) {
    int i;
    const int tekrar = 10000;

    printf("TEST: Rapid Toggle (CW <-> CCW)\n");
    resetState();

    for (i = 0; i < tekrar; i++) {
        GLenum hedef = (i % 2 == 0) ? GL_CW : GL_CCW;
        GLint face;

        glFrontFace(hedef);
        assert(glGetError() == GL_NO_ERROR);

        glGetIntegerv(GL_FRONT_FACE, &face);
        assert(face == hedef);
    }

    glFrontFace(GL_CCW);
    checkStatePreserved(GL_CCW);
    assert(glGetError() == GL_NO_ERROR);

    printf("  Sonuc: %d gecis tamamlandi\n", tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Gecersiz cagrilar arasinda gecerli cagrilar
 *
 * Gecersiz enum'larin arasina gecerli degerler serpistirerek
 * surucunun hata durumundan kurtulup kurtulamadigini dogrular.
 * Bazi implementasyonlar hata sonrasi "takili" kalabilir.
 * ============================================================ */

void test_frontFace_mixedValidity(void) {
    int i;
    GLenum pattern[] = {
        GL_CW, (GLenum)0x1234, GL_CCW, (GLenum)0x5678,
        GL_CW, (GLenum)0x9ABC, GL_CCW, (GLenum)0xDEF0
    };
    int n = sizeof(pattern) / sizeof(pattern[0]);

    printf("TEST: Mixed Validity Pattern\n");
    resetState();

    for (i = 0; i < n; i++) {
        GLenum deger = pattern[i];
        GLenum beklenen = (deger == GL_CW || deger == GL_CCW)
                          ? GL_NO_ERROR : GL_INVALID_ENUM;
        GLenum err;

        glFrontFace(deger);
        err = glGetError();
        assert(err == beklenen);
    }

    glFrontFace(GL_CCW);
    checkStatePreserved(GL_CCW);
    assert(glGetError() == GL_NO_ERROR);

    printf("  Sonuc: %d karisik cagri tamamlandi\n", n);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Display list etkilesimi (Eski OpenGL)
 *
 * glFrontFace cagrilarinin display listeye dogru sekilde
 * kaydedilip kaydedilmedigini dogrular. Gecersiz degerler
 * liste derlenirken hata uretmeli, listeye yazilmamalidir.
 * ============================================================ */

void test_frontFace_displayList(void) {
    GLuint list;
    GLint face;

    printf("TEST: Display List Interaction\n");
    resetState();

    list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    glFrontFace(GL_CW);
    assert(glGetError() == GL_NO_ERROR);

    glFrontFace((GLenum)0x0BAD);
    /* Hata aninda uretilebilir veya listeye kaydedilebilir;
     * davranis implementasyona baglidir. */

    glEndList();

    glFrontFace(GL_CCW);
    glCallList(list);

    glGetIntegerv(GL_FRONT_FACE, &face);
    printf("  List sonrasi durum: 0x%X\n", face);

    glDeleteLists(list, 1);
    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 5: Push/PopAttrib etkilesimi
 *
 * glPushAttrib ve glPopAttrib ile GL_TRANSFORM_BIT kullanarak
 * FrontFace durumunun yigina kaydedilip geri yuklendigini
 * dogrular. Durum yonetiminin bu mekanizmayla uyumlu calismasi
 * gerekir.
 * ============================================================ */

void test_frontFace_attribStack(void) {
    GLint face;

    printf("TEST: Attribute Stack (Push/PopAttrib)\n");
    resetState();

    glFrontFace(GL_CW);
    assert(glGetError() == GL_NO_ERROR);

    glPushAttrib(GL_TRANSFORM_BIT);

    glFrontFace(GL_CCW);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(GL_CCW);

    glPopAttrib();

    glGetIntegerv(GL_FRONT_FACE, &face);
    printf("  Pop sonrasi durum: 0x%X (beklenen GL_CW=0x%X)\n",
           face, GL_CW);

    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 6: Culling kombinasyonlari
 *
 * glFrontFace ile glCullFace'in farkli kombinasyonlarinda
 * tutarli davranis gosterdigini dogrular. Her kombinasyon
 * sonrasi durum sorgulanir.
 * ============================================================ */

void test_frontFace_cullCombinations(void) {
    GLenum frontModes[] = {GL_CCW, GL_CW};
    GLenum cullModes[] = {GL_BACK, GL_FRONT, GL_FRONT_AND_BACK};
    int i, j;

    printf("TEST: Cull Face Combinations\n");
    resetState();

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            GLint face, cull;

            glFrontFace(frontModes[i]);
            glCullFace(cullModes[j]);
            assert(glGetError() == GL_NO_ERROR);

            glGetIntegerv(GL_FRONT_FACE, &face);
            glGetIntegerv(GL_CULL_FACE_MODE, &cull);

            assert(face == frontModes[i]);
            assert(cull == cullModes[j]);
        }
    }

    glDisable(GL_CULL_FACE);
    resetState();
    printf("  Sonuc: 6 kombinasyon tamamlandi\n");
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7: Cok buyuk enum degerleri
 *
 * 32-bit enum araliginin ust kisimlarini test eder.
 * OpenGL spec 16-bit enum uzayini kullanir ancak 32-bit
 * degerler gonderildiginde implementasyonun davranisi
 * belirsizdir. Cokme veya durum bozulmasi kritik hatadir.
 * ============================================================ */

void test_frontFace_largeEnum(void) {
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

        glFrontFace(largeValues[i]);
        err = glGetError();

        printf("  Enum=0x%08X -> 0x%X\n", largeValues[i], err);
    }

    checkStatePreserved(GL_CCW);
    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 8: Thread benzeri hizli ardisik cagri
 *
 * Tek bir thread icerisinde glFrontFace'e ait cagrilari
 * mimimum gecikmeyle ard arda gondererek durum makinesinin
 * race condition benzeri senaryolarda tutarli kalip
 * kalmadigini gozlemler.
 * ============================================================ */

void test_frontFace_rapidFire(void) {
    int i;
    const int tekrar = 50000;

    printf("TEST: Rapid Fire (50K calls)\n");
    resetState();

    for (i = 0; i < tekrar; i++) {
        glFrontFace(GL_CW);
        glFrontFace(GL_CCW);
    }

    glGetError();
    glFrontFace(GL_CW);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(GL_CW);

    resetState();
    printf("  Sonuc: %d cift cagri tamamlandi\n", tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * Test calistirma cercevesi
 * ============================================================ */

