#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine getirir ve birikmis hatalari
 * temizler; boylece testler birbirine bagimli olmaz.
 * checkStatePreserved: PolygonOffset durumunun beklenen
 * factor ve units degerlerini korudugunu dogrular.
 * ============================================================ */

static void resetState(void) {
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glPolygonOffset(0.0f, 0.0f);
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLfloat expectedFactor,
                                GLfloat expectedUnits) {
    GLfloat factor, units;

    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &factor);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &units);

    if (fabsf(factor - expectedFactor) > 1e-6f ||
        fabsf(units - expectedUnits) > 1e-6f) {

        printf("  [FAIL] Durum bozuldu\n");
        printf("         factor: beklenen %.3f gercek %.3f\n",
               expectedFactor, factor);
        printf("         units : beklenen %.3f gercek %.3f\n",
               expectedUnits, units);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama
 *
 * glPolygonOffset'in temel sozlesmesini dogrular.
 * Tum float degerler kabul edilmeli ve hicbir durumda
 * GL hata kodu uretilmemelidir. Ayrica son yazilan
 * factor ve units degerleri durum sorgusunda geri
 * alinabilmelidir.
 * ============================================================ */

void test_polygonOffset_basicRobustness(void) {
    printf("TEST: Basic Robustness\n");
    resetState();

    glPolygonOffset(0.0f, 0.0f);
    assert(glGetError() == GL_NO_ERROR);

    glPolygonOffset(2.0f, 3.0f);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(2.0f, 3.0f);

    glPolygonOffset(-1000.0f, -500.0f);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(-1000.0f, -500.0f);

    glPolygonOffset(1000.0f, 500.0f);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(1000.0f, 500.0f);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Parametrik stres taramasi
 *
 * Genis bir float araliginda factor ve units
 * parametreleri sistematik olarak taranir.
 * Tum cagrilar GL_NO_ERROR donmelidir.
 * ============================================================ */

void test_polygonOffset_stressSweep(void) {
    int i;
    int passCount = 0;
    int failCount = 0;

    printf("TEST: Stress Sweep\n");
    resetState();

    for (i = -10000; i <= 10000; i++) {

        GLfloat value = (GLfloat)i * 0.1f;
        GLenum err;

        glPolygonOffset(value, value);
        err = glGetError();

        if (err != GL_NO_ERROR) {
            printf("  [FAIL] %.1f -> 0x%X\n",
                   value, err);
            failCount++;
        } else {
            passCount++;
        }
    }

    glPolygonOffset(0.0f, 0.0f);
    checkStatePreserved(0.0f, 0.0f);

    printf("  Sonuc: %d PASS, %d FAIL\n",
           passCount, failCount);

    assert(failCount == 0);

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Hata kuyrugu butunlugu
 *
 * Ard arda cok sayida glPolygonOffset cagrisi sonrasi hata
 * kuyrugunun dogru calistigini dogrular. Spec'e gore bu
 * fonksiyon hata uretmez; kuyruk temiz kalmalidir.
 * ============================================================ */

void test_polygonOffset_errorQueue(void) {
    int i;

    printf("TEST: Error Queue Management\n");
    resetState();

    for (i = 0; i < 1000; i++) {
        glPolygonOffset((GLfloat)i, (GLfloat)(-i));
    }

    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(999.0f, -999.0f);

    printf("  1000 cagri sonrasi kuyruk temiz\n");
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Durum korunumu
 *
 * Baska OpenGL fonksiyonlari hata uretse bile
 * glPolygonOffset durumunun degismedigini dogrular.
 * ============================================================ */

void test_polygonOffset_statePreservation(void) {
    printf("TEST: State Preservation\n");
    resetState();

    glPolygonOffset(5.0f, 7.0f);
    assert(glGetError() == GL_NO_ERROR);

    glFrontFace((GLenum)0x0BAD);
    assert(glGetError() == GL_INVALID_ENUM);

    checkStatePreserved(5.0f, 7.0f);
    assert(glGetError() == GL_NO_ERROR);

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: IEEE-754 ozel float degerleri
 *
 * NaN ve ±Infinity degerlerinin glPolygonOffset
 * tarafindan nasil ele alindigini gozlemler.
 * OpenGL spec bu degerler icin kesin davranis
 * tanimlamaz. Test bilgilendiricidir.
 * ============================================================ */

void test_polygonOffset_specialFloats(void) {
    GLenum err;
    GLfloat factor, units;

    printf("TEST: Special Float Values\n");
    resetState();

    glPolygonOffset(NAN, NAN);
    err = glGetError();
    printf("  NaN             -> 0x%X\n", err);

    glPolygonOffset(INFINITY, INFINITY);
    err = glGetError();
    printf("  +INFINITY       -> 0x%X\n", err);

    glPolygonOffset(-INFINITY, -INFINITY);
    err = glGetError();
    printf("  -INFINITY       -> 0x%X\n", err);

    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &factor);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &units);

    printf("  factor=%.3f units=%.3f\n", factor, units);

    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 6: Hizli durum gecisleri
 *
 * Farkli factor ve units degerleri arasinda hizli gecisler
 * yaparak durum makinesinin tutarliligini dogrular.
 * Her gecisten sonra durum sorgulanir.
 * ============================================================ */

void test_polygonOffset_rapidToggle(void) {
    int i;
    const int tekrar = 10000;

    printf("TEST: Rapid Toggle\n");
    resetState();

    for (i = 0; i < tekrar; i++) {

        GLfloat factor = (i % 2 == 0) ? 1.0f : -1.0f;
        GLfloat units  = (i % 2 == 0) ? 2.0f : -2.0f;

        glPolygonOffset(factor, units);
        assert(glGetError() == GL_NO_ERROR);

        checkStatePreserved(factor, units);
    }

    printf("  Sonuc: %d gecis tamamlandi\n", tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7: Enable/Disable etkilesimi
 *
 * glPolygonOffset durumunun
 * GL_POLYGON_OFFSET_FILL acik veya kapali olsa bile
 * korunup korunmadigini dogrular.
 * ============================================================ */

void test_polygonOffset_enableDisable(void) {
    printf("TEST: Enable/Disable Interaction\n");
    resetState();

    glPolygonOffset(4.0f, 8.0f);
    assert(glGetError() == GL_NO_ERROR);

    glEnable(GL_POLYGON_OFFSET_FILL);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(4.0f, 8.0f);

    glDisable(GL_POLYGON_OFFSET_FILL);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(4.0f, 8.0f);

    glEnable(GL_POLYGON_OFFSET_FILL);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(4.0f, 8.0f);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * Tum testleri calistir
 * ============================================================ */

int main(void) {

    test_polygonOffset_basicRobustness();
    test_polygonOffset_stressSweep();
    test_polygonOffset_errorQueue();
    test_polygonOffset_statePreservation();
    test_polygonOffset_specialFloats();
    test_polygonOffset_rapidToggle();
    test_polygonOffset_enableDisable();

    printf("=========================================\n");
    printf("Tum glPolygonOffset robustness testleri basarili.\n");
    printf("=========================================\n");

    return 0;
}
