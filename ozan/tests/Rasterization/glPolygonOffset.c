
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
 * ============================================================ */

static void resetState(void) {
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    glPolygonOffset(0.0f, 0.0f);
    while (glGetError() != GL_NO_ERROR);
}

/* ============================================================
 * TEST 1: Sozlesme ve durum yonetimi
 *
 * glPolygonOffset'in temel sozlesmesini dogrular: tum float
 * degerler kabul edilmeli, hicbiri hata uretmemelidir. Spec'te
 * yasak deger yoktur; implementasyon degerleri sessizce
 * kendi araligina kirpmalidir. Ayrica durum sorgusunun
 * tutarli sonuc verdigi kontrol edilir.
 * ============================================================ */

void test_polygonOffset_basicRobustness(void) {
    GLfloat f = 0.0f, u = 0.0f;

    printf("TEST: Basic Robustness\n");
    resetState();

    /* Tum float degerler kabul edilmeli, hata uretilmemeli */
    glPolygonOffset(0.0f, 0.0f);
    assert(glGetError() == GL_NO_ERROR);

    glPolygonOffset(-1000.0f, -1000.0f);
    assert(glGetError() == GL_NO_ERROR);

    glPolygonOffset(1e30f, 1e30f);
    assert(glGetError() == GL_NO_ERROR);

    glPolygonOffset(-1e30f, -1e30f);
    assert(glGetError() == GL_NO_ERROR);

    glPolygonOffset(NAN, NAN);
    assert(glGetError() == GL_NO_ERROR);

    glPolygonOffset(INFINITY, INFINITY);
    assert(glGetError() == GL_NO_ERROR);

    /* Durum sorgusu son yazilan degerleri vermeli */
    glPolygonOffset(2.0f, 3.0f);
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
    assert(f == 2.0f && u == 3.0f);
    assert(glGetError() == GL_NO_ERROR);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Stres taramasi
 *
 * Genis bir float araliginda sistematik tarama yaparak
 * implementasyonun tutarliligini dogrular. Negatif, sifir,
 * pozitif ve cok buyuk degerler test edilir.
 * ============================================================ */

void test_polygonOffset_stressSweep(void) {
    int i;
    int passCount = 0;
    int failCount = 0;

    printf("TEST: Stress Sweep\n");
    resetState();

    for (i = -10000; i <= 10000; i++) {
        float val = (float)i * 0.1f;
        GLenum err;

        glPolygonOffset(val, val);
        err = glGetError();

        if (err != GL_NO_ERROR) {
            printf("  [FAIL] val=%.1f -> 0x%X\n", val, err);
            failCount++;
        } else {
            passCount++;
        }
    }

    printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
    assert(failCount == 0);

    resetState();
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
    GLenum err;

    printf("TEST: Error Queue Integrity\n");
    resetState();

    for (i = 0; i < 1000; i++) {
        glPolygonOffset((float)i, (float)-i);
    }

    err = glGetError();
    assert(err == GL_NO_ERROR);

    printf("  1000 cagri sonrasi kuyruk temiz\n");

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Durum korunumu ve gecersiz cagri etkilesimi
 *
 * glPolygonOffset cagrilarinin diger OpenGL cagrilarinin
 * hata durumundan etkilenmedigini dogrular. Baska fonksiyonlar
 * hata uretse bile PolygonOffset durumu bozulmamali.
 * ============================================================ */

void test_polygonOffset_statePreservation(void) {
    GLfloat f = 0.0f, u = 0.0f;

    printf("TEST: State Preservation\n");
    resetState();

    glPolygonOffset(5.0f, 7.0f);

    /* Baska bir fonksiyondan hata uret */
    glFrontFace((GLenum)0x0BAD);
    assert(glGetError() == GL_INVALID_ENUM);

    /* PolygonOffset durumu bozulmamis olmali */
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
    assert(f == 5.0f && u == 7.0f);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Ozel float degerleri
 *
 * IEEE-754 ozel degerlerinin (NaN, ±Infinity) davranisini
 * inceler. OpenGL spec bu degerleri acikca tanimlamaz;
 * cokme veya durum bozulmasi kritik hatadir.
 * ============================================================ */

void test_polygonOffset_specialFloats(void) {
    GLenum err;
    GLfloat f, u;

    printf("TEST: Special Float Values\n");
    resetState();

    glPolygonOffset(NAN, NAN);
    err = glGetError();
    printf("  NaN       -> 0x%X\n", err);

    glPolygonOffset(INFINITY, INFINITY);
    err = glGetError();
    printf("  +INFINITY -> 0x%X\n", err);

    glPolygonOffset(-INFINITY, -INFINITY);
    err = glGetError();
    printf("  -INFINITY -> 0x%X\n", err);

    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
    printf("  Son durum: factor=%f, units=%f\n", f, u);

    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}
