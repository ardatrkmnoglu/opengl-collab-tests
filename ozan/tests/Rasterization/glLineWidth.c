#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

static void resetState(void)
{
    glLineWidth(1.0f);
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLfloat expected)
{
    GLfloat actual;
    glGetFloatv(GL_LINE_WIDTH, &actual);

    if (fabsf(actual - expected) > 1e-6f)
    {
        printf("  [FAIL] Durum bozuldu: beklenen %.3f, gercek %.3f\n", expected, actual);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama
 *
 * glLineWidth fonksiyonunun temel sozlesmesini dogrular.
 * Pozitif line width degerleri kabul edilmeli ve hata
 * olusturmamalidir. Sifir veya negatif degerler ise
 * GL_INVALID_VALUE hatasi ile reddedilmelidir.
 * Ayrica gecersiz cagrilar mevcut GL_LINE_WIDTH durumunu
 * degistirmemelidir.
 * ============================================================ */

void test_lineWidth_basicRobustness(void)
{
    GLfloat width;
    GLenum err;

    printf("TEST: Basic Robustness\n");
    resetState();

    glLineWidth(2.0f);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetFloatv(GL_LINE_WIDTH, &width);
    assert(fabsf(width - 2.0f) < 1e-6f);

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
 * TEST 2: Parametrik tarama
 *
 * -1000.0 ile +1000.0 arasindaki degerler sistematik olarak
 * test edilir. Pozitif olmayan degerler GL_INVALID_VALUE
 * uretmeli, pozitif degerler ise hata vermeden kabul
 * edilmelidir.
 * ============================================================ */

void test_lineWidth_stressSweep(void)
{
    int i;
    int passCount = 0;
    int failCount = 0;

    printf("TEST: Stress Sweep (-1000.0 .. +1000.0)\n");

    resetState();

    for(i = -10000; i <= 10000; i++)
    {
        GLfloat w = (GLfloat)i * 0.1f;
        GLenum expected = (w <= 0.0f) ? GL_INVALID_VALUE : GL_NO_ERROR;
        GLenum err;

        glLineWidth(w);
        err = glGetError();

        if(err != expected)
        {
            printf("  [FAIL] w=%.1f beklenen=0x%X gelen=0x%X\n", w, expected, err);
            failCount++;
        }
        else
        {
            passCount++;
        }
    }

    glLineWidth(1.0f);

    assert(glGetError() == GL_NO_ERROR);

    checkStatePreserved(1.0f);

    printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);

    assert(failCount == 0);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: IEEE-754 ozel degerleri
 * ============================================================
 *
 * NaN ve ±Infinity degerlerinin glLineWidth tarafindan nasil
 * ele alindigini gozlemler. OpenGL ES 2.0 bu degerler icin
 * kesin bir hata kodu tanimlamaz. Testin amaci implementasyonun
 * cokmedigini ve mevcut GL_LINE_WIDTH durumunu bozmadigini
 * dogrulamaktir.
 * ============================================================ */

void test_lineWidth_specialFloats(void)
{
    GLenum err;

    printf("TEST: Special Float Values\n");

    resetState();

    glLineWidth(3.0f);
    assert(glGetError()==GL_NO_ERROR);

    glLineWidth(NAN);
    err=glGetError();
    printf("  NaN       -> 0x%X\n",err);
    checkStatePreserved(3.0f);

    glLineWidth(INFINITY);
    err=glGetError();
    printf("  +Infinity -> 0x%X\n",err);
    checkStatePreserved(3.0f);

    glLineWidth(-INFINITY);
    err=glGetError();
    printf("  -Infinity -> 0x%X\n",err);
    checkStatePreserved(3.0f);

    resetState();

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Hata kuyrugu butunlugu
 * ============================================================
 *
 * Ardisik gecersiz glLineWidth cagrilari altinda hata
 * kuyrugunun dogru sekilde olustugunu ve temizlendikten
 * sonra normal cagrilarin calismaya devam ettigini dogrular.
 * ============================================================ */

void test_lineWidth_errorQueue(void)
{
    int i;
    GLenum err;
    int errorCount=0;

    printf("TEST: Error Queue Management\n");

    resetState();

    for(i=0;i<100;i++)
        glLineWidth(-(GLfloat)(i+1));

    while((err=glGetError())!=GL_NO_ERROR)
    {
        assert(err==GL_INVALID_VALUE);
        errorCount++;
    }

    printf("  Kuyruktan okunan hata sayisi: %d\n",errorCount);

    assert(errorCount>0);

    glLineWidth(4.0f);

    assert(glGetError()==GL_NO_ERROR);

    checkStatePreserved(4.0f);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Implementasyon limitleri
 * ============================================================
 *
 * GL_ALIASED_LINE_WIDTH_RANGE sorgulanir ve implementasyonun
 * destekledigi line width araligi dogrulanir. Aralik ustunde
 * bir deger gonderildiginde hata olusmamali, implementasyon
 * gerekirse bu degeri rasterization asamasinda kirpmalidir.
 * ============================================================ */

#ifndef GL_ALIASED_LINE_WIDTH_RANGE
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#endif

void test_lineWidth_limits(void)
{
    GLfloat range[2];
    GLfloat width;
    GLfloat request;
    GLenum err;

    printf("TEST: Implementation Limits\n");

    resetState();

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE,range);

    err=glGetError();

    assert(err==GL_NO_ERROR);

    printf("  Desteklenen aralik: %.2f - %.2f\n",
           range[0],range[1]);

    assert(range[0]>0.0f);
    assert(range[1]>=range[0]);
    assert(range[0]<=1.0f);
    assert(range[1]>=1.0f);

    request=range[1]+1000.0f;

    glLineWidth(request);

    assert(glGetError()==GL_NO_ERROR);

    glGetFloatv(GL_LINE_WIDTH,&width);

    printf("  Istek=%.2f  GL_LINE_WIDTH=%.2f\n",
           request,width);

    glLineWidth(range[0]);

    assert(glGetError()==GL_NO_ERROR);

    checkStatePreserved(range[0]);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 6: Rapid Toggle
 * ============================================================
 *
 * Farkli line width degerleri arasinda hizli gecisler yapilarak
 * durum makinesinin tutarliligi dogrulanir. Her gecisten sonra
 * GL_LINE_WIDTH sorgulanir ve beklenen degerle uyustugu kontrol edilir.
 * ============================================================ */

void test_lineWidth_rapidToggle(void)
{
    int i;
    const int tekrar=100000;

    printf("TEST: Rapid Toggle\n");

    resetState();

    for(i=0;i<tekrar;i++)
    {
        GLfloat width=(i%3==0)?1.0f:((i%3==1)?2.0f:5.0f);

        glLineWidth(width);

        assert(glGetError()==GL_NO_ERROR);

        checkStatePreserved(width);
    }

    resetState();

    printf("  Sonuc: %d gecis tamamlandi\n",tekrar);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7: Rapid Fire
 * ============================================================
 *
 * glLineWidth fonksiyonuna cok sayida ard arda cagri gonderilerek
 * surucunun yogun kullanim altinda kararliligini korudugu
 * dogrulanir.
 * ============================================================ */

void test_lineWidth_rapidFire(void)
{
    int i;
    const int tekrar=1000000;

    printf("TEST: Rapid Fire\n");

    resetState();

    for(i=0;i<tekrar;i++)
    {
        glLineWidth(1.0f);
        glLineWidth(2.0f);
        glLineWidth(5.0f);
    }

    assert(glGetError()==GL_NO_ERROR);

    checkStatePreserved(5.0f);

    resetState();

    printf("  Sonuc: %d cift cagri tamamlandi\n",tekrar*3);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 8: Random Fuzz
 * ============================================================
 *
 * Rastgele line width degerleri gonderilerek surucunun beklenmeyen
 * girdiler altinda kararliligi test edilir. Pozitif degerlerde
 * GL_NO_ERROR, sifir ve negatif degerlerde GL_INVALID_VALUE
 * beklenir.
 * ============================================================ */

void test_lineWidth_randomFuzz(void)
{
    int i;
    GLfloat lastValid=1.0f;

    printf("TEST: Random Fuzz\n");

    resetState();

    srand(12345);

    for(i=0;i<1000000;i++)
    {
        GLfloat width=((GLfloat)(rand()%40000)-20000.0f)/10.0f;
        GLenum expected=(width<=0.0f)?GL_INVALID_VALUE:GL_NO_ERROR;
        GLenum err;

        glLineWidth(width);

        err=glGetError();

        if(err!=expected)
        {
            printf("  [FAIL] width=%f expected=0x%X got=0x%X\n",
                   width,expected,err);
            assert(0);
        }

        if(err==GL_NO_ERROR)
            lastValid=width;
        else
            checkStatePreserved(lastValid);
    }

    resetState();

    printf("  [PASS]\n\n");
}

/* ============================================================
 * Tum glLineWidth Robustness Testlerini Calistir
 * ============================================================ */

void Run_glLineWidth_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("      glLineWidth Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_lineWidth_basicRobustness();
    test_lineWidth_stressSweep();
    test_lineWidth_specialFloats();
    test_lineWidth_errorQueue();
    test_lineWidth_limits();
    test_lineWidth_rapidToggle();
    test_lineWidth_rapidFire();
    test_lineWidth_randomFuzz();

    printf("=============================================\n");
    printf("      Tum glLineWidth Testleri Basarili\n");
    printf("=============================================\n\n");
}