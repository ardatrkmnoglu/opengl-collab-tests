#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi/sonrasi OpenGL durumunu ve hata
 * kuyrugunu temizler. checkStatePreserved: Reddedilen cagrilarin
 * mevcut GL_LINE_WIDTH'i degistirmedigini dogrular; basarisizlik
 * durumunda assert ile program durur.
 * ============================================================ */
static void resetState(void) {
    glLineWidth(1.0f);
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLfloat expected) {
    GLfloat actual;
    glGetFloatv(GL_LINE_WIDTH, &actual);
    if (actual != expected) {
        printf("  [FAIL] Durum bozuldu: beklenen %f, gercek %f\n",
               expected, actual);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama — gecerli ve gecersiz girdiler
 * ============================================================ */

/*
 * glLineWidth'in temel sozlesmesini dogrular: pozitif degerler hatasiz
 * kabul edilmeli, pozitif olmayan degerler GL_INVALID_VALUE ile
 * reddedilmelidir. Reddedilen cagrilar idempotent olmalidir; yani
 * mevcut GL_LINE_WIDTH durumunu degistirmemelidir. Bu ozellik,
 * basarisiz bir cagrinin sonraki cizim komutlarina sirayet etmesini
 * ve belirlenemez goruntu olusmasini engeller.
 */

void test_lineWidth_basicRobustness(void) {
    GLfloat width;
    GLenum err;

    printf("TEST: Basic Robustness\n");
    resetState();

    glLineWidth(2.0f);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetFloatv(GL_LINE_WIDTH, &width);
    assert(width == 2.0f);

    glLineWidth(0.0f);
    err = glGetError();
    assert(err == GL_INVALID_VALUE);

    glLineWidth(-5.0f);
    err = glGetError();
    assert(err == GL_INVALID_VALUE);

    checkStatePreserved(2.0f);
    assert(glGetError() == GL_NO_ERROR);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Parametrik tarama — sinir deger kesfi
 * ============================================================ */

/*
 * glLineWidth'i surekli bir parametrik aralikta test ederek, noktasal
 * kontrollerin yakalayamayacagi implementasyon-ozgulu sinir
 * anomalilerini ortaya cikarir. Tarama [-1000.0, +1000.0] araliginda
 * 0.1 cozunurluguyle gerceklestirilir (20.001 ornek):
 *
 *   w <= 0  → GL_INVALID_VALUE  (spec ile zorunlu reddetme)
 *   w > 0   → GL_NO_ERROR       (kabul, kirpmali)
 *
 * Dongu tam sayi indeksleme ve basit carpma kullanir; birikimli
 * yuvarlama hatasi olusmaz.
 */

void test_lineWidth_stressSweep(void) {
    int i;
    int passCount = 0;
    int failCount = 0;

    printf("TEST: Stress Sweep (-1000.0 .. +1000.0)\n");
    resetState();

    for (i = -10000; i <= 10000; i++) {
        float w = (float)i * 0.1f;
        GLenum expected = (w <= 0.0f) ? GL_INVALID_VALUE : GL_NO_ERROR;
        GLenum err;

        glLineWidth(w);
        err = glGetError();

        if (err != expected) {
            printf("  [FAIL] w=%.1f, beklenen 0x%X, gelen 0x%X\n",
                   w, expected, err);
            failCount++;
        } else {
            passCount++;
        }
    }

    glLineWidth(1.0f);
    checkStatePreserved(1.0f);
    assert(glGetError() == GL_NO_ERROR);

    printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
    assert(failCount == 0);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: IEEE-754 ozel degerleri
 * ============================================================ */
/*
 * IEEE-754 ozel kayan nokta degerleri altinda davranisi degerlendirir:
 *   NaN         — karsilastirma islemleri tanimsizdir; NaN <= 0
 *                yanlis doner, dolayisiyla basit aralik kontrolleri
 *                NaN'i kabul edebilir
 *   +Sonsuz     — her sonlu implementasyon limitini asar
 *   -Sonsuz     — isaret biti setlidir; negatif olarak reddedilmeli
 *
 * OpenGL spec NaN davranisini acikca tanimlamaz, ancak robust bir
 * implementasyon cokmemeli, dahili istisna uretmemeli ve surucu
 * durumunu bozmamalidir. Bu test bilgilendiricidir; kesin hata
 * kodlari implementasyona baglidir.
 * KISICA:
 * NaN ve ±Infinity degerlerinin glLineWidth tarafindan nasil
 * ele alindigini gozlemler. Bu degerler icin OpenGL spec kesin
 * bir davranis belirtmez; farkli suruculer farkli hata kodlari
 * dondurebilir veya sessizce kirpabilir. Testin amaci kesin
 * bir dogrulama yapmak degil, implementasyonun cokmedigini
 * ve durumu bozmadigini kontrol etmektir. Cikti insani tarafindan
 * degerlendirilir: INVALID_VALUE veya NO_ERROR makul karsilanir,
 * ancak crash, segfault veya GL_LINE_WIDTH degisikligi kritik
 * bir robustness acigi olarak rapor edilmelidir.
 * segfault veya GL_LINE_WIDTH degisikligi,
 * surucunun ozel float degerleri isleyememesinden kaynaklanan kritik bir
 * robustness acigidir; bu durumda uygulama aniden sonlanir veya sonraki
 * cizim komutlarinda goruntu bozulmasina yol acar.
 */

void test_lineWidth_specialFloats(void) {
    GLenum err;
    GLfloat width;

    printf("TEST: Special Float Values (NaN, Inf)\n");
    resetState();

    glLineWidth(NAN);
    err = glGetError();
    printf("  NaN       -> 0x%X (beklenen INVALID_VALUE=0x%X)\n",
           err, GL_INVALID_VALUE);

    glLineWidth(INFINITY);
    err = glGetError();
    printf("  +INFINITY -> 0x%X\n", err);

    glLineWidth(-INFINITY);
    err = glGetError();
    printf("  -INFINITY -> 0x%X (beklenen INVALID_VALUE=0x%X)\n",
           err, GL_INVALID_VALUE);

    glGetFloatv(GL_LINE_WIDTH, &width);
    printf("  Son durum: width=%f (beklenen 1.0)\n", width);

    resetState();
    printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 4: Hata kuyrugu butunlugu
 * ============================================================ */

/*
 * Surekli hata enjeksiyonu altinda hata kuyrugu davranisini dogrular.
 * OpenGL en az bir hata kaydini garanti eder; kuyruk derinligi
 * implementasyona baglidir. Test sunlari kontrol eder:
 *
 *   - Hatalar glGetError ile kaydedilir ve okunabilir
 *   - Kuyruk GL_NO_ERROR'a kadar tamamen bosaltilabilir
 *   - Bosaltma sonrasi islemler eski hata kirliliginden etkilenmez
 *
 * Kuyrugun duzgun bosaltilamamasi, bozuk hata durumu izleme
 * isaretidir; bu durum uretim ortaminda sonraki hatalarin
 * maskelenmesine yol acar.
 */

void test_lineWidth_errorQueue(void) {
    int i;
    GLenum err;
    int errorCount = 0;

    printf("TEST: Error Queue Management\n");
    resetState();

    for (i = 0; i < 50; i++) {
        glLineWidth(-1.0f * (i + 1));
    }

    while ((err = glGetError()) != GL_NO_ERROR) {
        assert(err == GL_INVALID_VALUE);
        errorCount++;
    }

    printf("  Kuyruktan okunan hata sayisi: %d\n", errorCount);
    assert(errorCount > 0);

    glLineWidth(4.0f);
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(4.0f);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Implementasyon limitleri ve aralik sorgusu
 * ============================================================ */
/*
 * ES 2.0'da line width araligi GL_ALIASED_LINE_WIDTH_RANGE ile
 * sorgulanir (GL_LINE_WIDTH_RANGE masaustu-GL tokenidir, ES'te
 * GL_INVALID_ENUM uretir). Bu test:
 *   - Aralik sorgusunun hatasiz calistigini ve spec garantisini
 *     (aralik 1.0'i icermeli, min > 0) dogrular
 *   - Aralik ustu pozitif bir degerin HATA URETMEDIGINI dogrular
 *     (kirpma raster asamasinda sessizce olur, error degildir)
 *   - GL_LINE_WIDTH sorgusunun kirpilmis degeri degil, SPECIFIED
 *     degeri dondurdugunu; yani surucunun state'i erken
 *     clamp'lemedigini garanti eder
 */

#ifndef GL_ALIASED_LINE_WIDTH_RANGE
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E  /* ES2 header yoksa guvence */
#endif

void test_lineWidth_limits(void) {
    GLfloat range[2];
    GLfloat width;
    GLfloat request;
    GLenum err;

    printf("TEST: Implementation Limits (Aliased Line Width Range)\n");
    resetState();

    /* ---------- 1) Aralik sorgusu robustlugu ---------- */
    range[0] = range[1] = -1.0f;             /* sentinel: dolduruldu mu? */
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, range);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    printf("  ALIASED_LINE_WIDTH_RANGE: [%.2f, %.2f]\n",
           range[0], range[1]);

    /* Spec garantisi: aralik 1.0'i icermeli, min pozitif olmali */
    assert(range[0] > 0.0f);
    assert(range[1] >= range[0]);
    assert(range[0] <= 1.0f);
    assert(range[1] >= 1.0f);

    /* ---------- 2) Aralik ustu deger -> hata YOK ---------- */
    request = range[1] * 10.0f;
    glLineWidth(request);
    err = glGetError();
    assert(err == GL_NO_ERROR);              /* w > 0 her zaman kabul edilir */

    /* ---------- 3) State SPECIFIED degeri korur (erken clamp yok) ---------- */
    glGetFloatv(GL_LINE_WIDTH, &width);
    assert(glGetError() == GL_NO_ERROR);
    if (fabsf(width - request) > request * 1e-5f) {
        printf("  [FAIL] state erken clamp'lenmis: istek %.1f, sorgu %.1f\n",
               request, width);
        assert(0);
    }
    printf("  Istek %.1f -> GL_LINE_WIDTH %.1f (specified korunuyor)\n",
           request, width);

    /* ---------- 4) Alt sinir da kabul edilmeli ---------- */
    glLineWidth(range[0]);
    assert(glGetError() == GL_NO_ERROR);
    glGetFloatv(GL_LINE_WIDTH, &width);
    assert(fabsf(width - range[0]) <= range[0] * 1e-5f);

    resetState();
    printf("  [PASS]\n\n");
}

