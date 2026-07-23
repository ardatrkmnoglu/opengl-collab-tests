#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "Rasterizaton_LineWidth_TP_001";
static const char* test_case_1 = "Rasterizaton_LineWidth_TC_001";
static const char* test_case_2 = "Rasterizaton_LineWidth_TC_002";
static const char* test_case_3 = "Rasterizaton_LineWidth_TC_003";
static const char* test_case_4 = "Rasterizaton_LineWidth_TC_004";
static const char* test_case_5 = "Rasterizaton_LineWidth_TC_005";
static const char* test_case_6 = "Rasterizaton_LineWidth_TC_006";
static const char* test_case_7 = "Rasterizaton_LineWidth_TC_007";
static const char* test_case_8 = "Rasterizaton_LineWidth_TC_008";

static void resetState_LineWidth(void)
{
    glLineWidth(1.0f);
    while (glGetError() != GL_NO_ERROR);
}

static int checkStatePreserved_GetError(const char* test_case, GLfloat expected)
{
    GLfloat actual;
    glGetFloatv(GL_LINE_WIDTH, &actual);

    if (fabsf(actual - expected) > 1e-6f)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Durum bozuldu: beklenen %.3f, gercek %.3f", expected, actual);
        return 0;
    }

    return 1;
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

void Rasterizaton_LineWidth_TC_001(void)
{
    GLfloat width;
    GLenum err;

    resetState_LineWidth();

    glLineWidth(2.0f);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Beklenmeyen hata: 0x%X", err);
        return;
    }

    glGetFloatv(GL_LINE_WIDTH, &width);
    if (fabsf(width - 2.0f) >= 1e-6f)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Beklenen genislik 2.000, gercek %.3f", width);
        return;
    }

    glLineWidth(0.0f);
    err = glGetError();
    if (err != GL_INVALID_VALUE)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "w=0.0 icin beklenen=0x%X gelen=0x%X", GL_INVALID_VALUE, err);
        return;
    }

    glLineWidth(-5.0f);
    err = glGetError();
    if (err != GL_INVALID_VALUE)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "w=-5.0 icin beklenen=0x%X gelen=0x%X", GL_INVALID_VALUE, err);
        return;
    }

    if (!checkStatePreserved_GetError(test_case_1, 2.0f))
        return;

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Hata kuyrugu temiz degil: 0x%X", err);
        return;
    }

    resetState_LineWidth();

    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2: Parametrik tarama
 *
 * -1000.0 ile +1000.0 arasindaki degerler sistematik olarak
 * test edilir. Pozitif olmayan degerler GL_INVALID_VALUE
 * uretmeli, pozitif degerler ise hata vermeden kabul
 * edilmelidir.
 * ============================================================ */

void Rasterizaton_LineWidth_TC_002(void)
{
    int i;
    int passCount = 0;
    int failCount = 0;

    resetState_LineWidth();

    for(i = -10000; i <= 10000; i++)
    {
        GLfloat w = (GLfloat)i * 0.1f;
        GLenum expected = (w <= 0.0f) ? GL_INVALID_VALUE : GL_NO_ERROR;
        GLenum err;

        glLineWidth(w);
        err = glGetError();

        if(err != expected)
        {
            TEST_LOG_FAIL(test_case_2, test_procedure,
                          "w=%.1f beklenen=0x%X gelen=0x%X", w, expected, err);
            failCount++;
        }
        else
        {
            passCount++;
        }
    }

    glLineWidth(1.0f);

    if (glGetError() != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Tarama sonrasi gecerli cagri hata uretti.");
        return;
    }

    if (!checkStatePreserved_GetError(test_case_2, 1.0f))
        return;

    TEST_LOG_INFO("Sonuc: %d PASS, %d FAIL", passCount, failCount);

    if (failCount != 0)
    {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Basarisiz cagri sayisi: %d", failCount);
        return;
    }

    TEST_LOG_SUCCESS(test_case_2, test_procedure);
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

void Rasterizaton_LineWidth_TC_003(void)
{
    GLenum err;

    resetState_LineWidth();

    glLineWidth(3.0f);
    err=glGetError();
    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Beklenmeyen hata: 0x%X",err);
        return;
    }

    glLineWidth(NAN);
    err=glGetError();
    TEST_LOG_INFO("NaN       -> 0x%X",err);
    if(!checkStatePreserved_GetError(test_case_3, 3.0f))
        return;

    glLineWidth(INFINITY);
    err=glGetError();
    TEST_LOG_INFO("+Infinity -> 0x%X",err);
    if(!checkStatePreserved_GetError(test_case_3, 3.0f))
        return;

    glLineWidth(-INFINITY);
    err=glGetError();
    TEST_LOG_INFO("-Infinity -> 0x%X",err);
    if(!checkStatePreserved_GetError(test_case_3, 3.0f))
        return;

    resetState_LineWidth();

    TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4: Hata kuyrugu butunlugu
 * ============================================================
 *
 * Ardisik gecersiz glLineWidth cagrilari altinda hata
 * kuyrugunun dogru sekilde olustugunu ve temizlendikten
 * sonra normal cagrilarin calismaya devam ettigini dogrular.
 * ============================================================ */

void Rasterizaton_LineWidth_TC_004(void)
{
    int i;
    GLenum err;
    int errorCount=0;

    resetState_LineWidth();

    for(i=0;i<100;i++)
        glLineWidth(-(GLfloat)(i+1));

    while((err=glGetError())!=GL_NO_ERROR)
    {
        if(err!=GL_INVALID_VALUE)
        {
            TEST_LOG_FAIL(test_case_4, test_procedure,
                          "Beklenen=0x%X gelen=0x%X",GL_INVALID_VALUE,err);
            return;
        }
        errorCount++;
    }

    TEST_LOG_INFO("Kuyruktan okunan hata sayisi: %d",errorCount);

    if(errorCount<=0)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Hata kuyrugunda hic hata bulunamadi.");
        return;
    }

    glLineWidth(4.0f);

    err=glGetError();
    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Beklenmeyen hata: 0x%X",err);
        return;
    }

    if(!checkStatePreserved_GetError(test_case_4, 4.0f))
        return;

    resetState_LineWidth();

    TEST_LOG_SUCCESS(test_case_4, test_procedure);
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

void Rasterizaton_LineWidth_TC_005(void)
{
    GLfloat range[2];
    GLfloat width;
    GLfloat request;
    GLenum err;

    resetState_LineWidth();

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE,range);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Aralik sorgusu hata uretti: 0x%X",err);
        return;
    }

    TEST_LOG_INFO("Desteklenen aralik: %.2f - %.2f",
                  range[0],range[1]);

    if(range[0]<=0.0f)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Alt sinir pozitif degil: %.2f",range[0]);
        return;
    }
    if(range[1]<range[0])
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Ust sinir alt sinirdan kucuk: %.2f < %.2f",range[1],range[0]);
        return;
    }
    if(range[0]>1.0f)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Alt sinir 1.0 degerini kapsamiyor: %.2f",range[0]);
        return;
    }
    if(range[1]<1.0f)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Ust sinir 1.0 degerini kapsamiyor: %.2f",range[1]);
        return;
    }

    request=range[1]+1000.0f;

    glLineWidth(request);

    err=glGetError();
    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Aralik ustu deger hata uretti: 0x%X",err);
        return;
    }

    glGetFloatv(GL_LINE_WIDTH,&width);

    TEST_LOG_INFO("Istek=%.2f  GL_LINE_WIDTH=%.2f",
                  request,width);

    glLineWidth(range[0]);

    err=glGetError();
    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Alt sinir degeri hata uretti: 0x%X",err);
        return;
    }

    if(!checkStatePreserved_GetError(test_case_5, range[0]))
        return;

    resetState_LineWidth();

    TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6: Rapid Toggle
 * ============================================================
 *
 * Farkli line width degerleri arasinda hizli gecisler yapilarak
 * durum makinesinin tutarliligi dogrulanir. Her gecisten sonra
 * GL_LINE_WIDTH sorgulanir ve beklenen degerle uyustugu kontrol edilir.
 * ============================================================ */

void Rasterizaton_LineWidth_TC_006(void)
{
    int i;
    const int tekrar=100000;

    resetState_LineWidth();

    for(i=0;i<tekrar;i++)
    {
        GLfloat width=(i%3==0)?1.0f:((i%3==1)?2.0f:5.0f);

        glLineWidth(width);

        if(glGetError()!=GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_6, test_procedure, "Iteration: %d width=%.3f",i,width);
            return;
        }

        if(!checkStatePreserved_GetError(test_case_6, width))
            return;
    }

    resetState_LineWidth();

    TEST_LOG_INFO("Sonuc: %d gecis tamamlandi",tekrar);

    TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7: Rapid Fire
 * ============================================================
 *
 * glLineWidth fonksiyonuna cok sayida ard arda cagri gonderilerek
 * surucunun yogun kullanim altinda kararliligini korudugu
 * dogrulanir.
 * ============================================================ */

void Rasterizaton_LineWidth_TC_007(void)
{
    int i;
    const int tekrar=1000000;

    resetState_LineWidth();

    for(i=0;i<tekrar;i++)
    {
        glLineWidth(1.0f);
        glLineWidth(2.0f);
        glLineWidth(5.0f);
    }

    if(glGetError()!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Yogun kullanim sonrasi hata olustu.");
        return;
    }

    if(!checkStatePreserved_GetError(test_case_7, 5.0f))
        return;

    resetState_LineWidth();

    TEST_LOG_INFO("Sonuc: %d cagri tamamlandi",tekrar*3);

    TEST_LOG_SUCCESS(test_case_7, test_procedure);
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

void Rasterizaton_LineWidth_TC_008(void)
{
    int i;
    GLfloat lastValid=1.0f;

    resetState_LineWidth();

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
            TEST_LOG_FAIL(test_case_8, test_procedure,
                          "width=%f expected=0x%X got=0x%X",
                          width,expected,err);
            return;
        }

        if(err==GL_NO_ERROR)
            lastValid=width;
        else
        {
            if(!checkStatePreserved_GetError(test_case_8, lastValid))
                return;
        }
    }

    resetState_LineWidth();

    TEST_LOG_SUCCESS(test_case_8, test_procedure);
}

/* ============================================================
 * Tum glLineWidth Robustness Testlerini Calistir
 * ============================================================ */

void Run_glLineWidth_Robustness(void)
{
    Rasterizaton_LineWidth_TC_001();
    Rasterizaton_LineWidth_TC_002();
    Rasterizaton_LineWidth_TC_003();
    Rasterizaton_LineWidth_TC_004();
    Rasterizaton_LineWidth_TC_005();
    Rasterizaton_LineWidth_TC_006();
    Rasterizaton_LineWidth_TC_007();
    Rasterizaton_LineWidth_TC_008();
}