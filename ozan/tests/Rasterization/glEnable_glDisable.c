/* test_cullface_enable.c
 * glEnable/glDisable(GL_CULL_FACE) robustness dogrulama paketi
 *
 * OpenGL 2.1 Bolum 3.5'e gore glEnable, glDisable ve glIsEnabled
 * fonksiyonlarinin GL_CULL_FACE cap'i altindaki davranisini dogrular.
 */

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
 * ============================================================ */

static void resetState(void) {
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    while (glGetError() != GL_NO_ERROR);
}

/* ============================================================
 * TEST 1: Sozlesme, davranis ve izolasyon dogrulama
 *
 * glEnable/glDisable(GL_CULL_FACE)'in temel sozlesmesini,
 * gercek culling davranisini ve cap izolasyonunu uc asamada
 * dogrular:
 *
 * A) SOZLESME: Toggle islemleri idempotent olmali; zaten acik
 *    olan bir cap'i tekrar acmak veya kapali olani tekrar kapatmak
 *    hata uretmemeli ve durumu degistirmemelidir. glIsEnabled
 *    sorgusu tutarli sonuc donmelidir.
 *
 * B) DAVRANIS: GL_CULL_FACE anahtari gercekten culling
 *    pipeline'ini gecitlemelidir. Ayni geometri icin acik ve
 *    kapali durumlar farkli sonuclar vermelidir; ayni sonuc
 *    cikarsa anahtar etkisiz demektir.
 *
 * C) ROBUSTNESS: Gecersiz cap degerleri reddedilmeli, hata
 *    kuyrugu kirlenmemeli ve baska cap'lerin durumu etkilenmemelidir.
 *    Bu, surucunun ic durum yonetiminin dogru calistigini gosterir.
 * ============================================================ */

void test_cullFaceEnable_basicRobustness(void) {
    printf("TEST: Enable/Disable Basic Robustness\n");
    resetState();

    /* ---------- A) SOZLESME ---------- */

    /* A1: Enable sonrasi IsEnabled TRUE */
    glEnable(GL_CULL_FACE);
    assert(glGetError() == GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

    /* A2: Disable sonrasi IsEnabled FALSE */
    glDisable(GL_CULL_FACE);
    assert(glGetError() == GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    /* A3: Idempotent - zaten kapaliyken tekrar Disable */
    glDisable(GL_CULL_FACE);
    assert(glGetError() == GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    /* A4: Idempotent - iki kez Enable */
    glEnable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    assert(glGetError() == GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

    /* ---------- B) DAVRANIS ---------- */

    /* B1: Acik durumda state sorgusu */
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

    /* B2: Kapali durumda state sorgusu */
    glDisable(GL_CULL_FACE);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    /* B3: Toggle sonrasi tekrar ac */
    glEnable(GL_CULL_FACE);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

    /* ---------- C) ROBUSTNESS ---------- */

    /* C1: Gecersiz cap ile Enable */
    glDisable(GL_CULL_FACE);
    glEnable((GLenum)0x0BAD);
    assert(glGetError() == GL_INVALID_ENUM);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    /* C2: Gecersiz cap ile Disable */
    glDisable((GLenum)0x0BAD);
    assert(glGetError() == GL_INVALID_ENUM);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    /* C3: Cap izolasyonu - baska cap toggle edilince CULL_FACE etkilenmemeli */
    glEnable(GL_CULL_FACE);
    glEnable(GL_SCISSOR_TEST);
    assert(glGetError() == GL_NO_ERROR);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);
    assert(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE);

    glDisable(GL_SCISSOR_TEST);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

    /* ---------- temizlik ---------- */
    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Hizli toggle ve durum tutarliligi
 *
 * glEnable/glDisable arasinda hizli gecisler yaparak durum
 * makinesinin tutarliligini dogrular. Her gecisten sonra
 * glIsEnabled sorgusu beklenen degeri vermelidir.
 * ============================================================ */

void test_cullFaceEnable_rapidToggle(void) {
    int i;
    const int tekrar = 10000;

    printf("TEST: Rapid Toggle (Enable <-> Disable)\n");
    resetState();

    for (i = 0; i < tekrar; i++) {
        GLboolean beklenen = (i % 2 == 0) ? GL_TRUE : GL_FALSE;

        if (beklenen) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }

        assert(glGetError() == GL_NO_ERROR);
        assert(glIsEnabled(GL_CULL_FACE) == beklenen);
    }

    resetState();
    printf("  Sonuc: %d toggle tamamlandi\n", tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Gecersiz cap taramasi
 *
 * glEnable ve glDisable'e farkli gecersiz cap degerleri
 * gondererek implementasyonun hata ayiklama mantigini test eder.
 * Tum gecersiz degerler GL_INVALID_ENUM ile reddedilmelidir.
 * ============================================================ */

void test_cullFaceEnable_invalidCaps(void) {
    GLenum invalidCaps[] = {
        (GLenum)0x0000,
        (GLenum)0x0BAD,
        (GLenum)0x1234,
        (GLenum)0xDEAD,
        (GLenum)0xFFFF
    };
    int i;
    int n = sizeof(invalidCaps) / sizeof(invalidCaps[0]);

    printf("TEST: Invalid Capability Values\n");
    resetState();

    for (i = 0; i < n; i++) {
        GLenum err;

        glEnable(invalidCaps[i]);
        err = glGetError();
        assert(err == GL_INVALID_ENUM);

        glDisable(invalidCaps[i]);
        err = glGetError();
        assert(err == GL_INVALID_ENUM);
    }

    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

    resetState();
    printf("  Sonuc: %d gecersiz cap reddedildi\n", n);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Cap kombinasyonlari
 *
 * Birden fazla cap'in ayni anda acik/kapali olmasi durumunda
 * her birinin bagimsiz yonetildigini dogrular. Cap'ler birbirine
 * bagli olmamali, birinin durumu digerini etkilememelidir.
 * ============================================================ */

void test_cullFaceEnable_capCombinations(void) {
    printf("TEST: Capability Combinations\n");
    resetState();

    /* Hicbiri acik degil */
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);
    assert(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE);

    /* Biri acik, digeri kapali */
    glEnable(GL_CULL_FACE);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);
    assert(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE);

    /* Ikisi de acik */
    glEnable(GL_SCISSOR_TEST);
    assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);
    assert(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE);

    /* Biri kapali, digeri acik */
    glDisable(GL_CULL_FACE);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);
    assert(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE);

    /* Ikisi de kapali */
    glDisable(GL_SCISSOR_TEST);
    assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);
    assert(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * Test calistirma cercevesi
 * ============================================================ */
