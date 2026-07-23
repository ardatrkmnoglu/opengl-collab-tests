#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "C:/Users/Ozan/Desktop/Workspace/OpenGL_Proje/opengl-collab-tests/include/macro.h"

static const char* test_procedure = "ViewportandClipping_DepthRangef_TP_001";
static const char* test_case_1 = "ViewportandClipping_DepthRangef_TC_001";
static const char* test_case_2 = "ViewportandClipping_DepthRangef_TC_002";
static const char* test_case_3 = "ViewportandClipping_DepthRangef_TC_003";
static const char* test_case_4 = "ViewportandClipping_DepthRangef_TC_004";
static const char* test_case_5 = "ViewportandClipping_DepthRangef_TC_005";
static const char* test_case_6 = "ViewportandClipping_DepthRangef_TC_006";

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi/sonrasi OpenGL durumunu varsayilan
 * hale getirir ve hata kuyrugunu temizler.
 *
 * checkStatePreserved: Beklenen depth range durumunun
 * bozulmadigini dogrular.
 * ============================================================ */

static void resetState(void)
{
    glDepthRange(0.0,1.0);

    while(glGetError()!=GL_NO_ERROR);
}

static int checkStatePreserved(const char* test_case,
                               GLdouble expectedNear,GLdouble expectedFar)
{
    GLdouble depthRange[2];

    glGetDoublev(GL_DEPTH_RANGE,depthRange);

    if(fabs(depthRange[0]-expectedNear)>1e-6 ||
       fabs(depthRange[1]-expectedFar)>1e-6)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Depth range durumu bozuldu. Beklenen: (%lf,%lf) Gercek: (%lf,%lf)",
                      expectedNear,expectedFar,depthRange[0],depthRange[1]);
        return 0;
    }

    return 1;
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama
 * ============================================================ */

/*
 * glDepthRange'in temel sozlesmesini dogrular.
 * Tum double degerler kabul edilmeli ve GL_NO_ERROR
 * dondurmelidir. Spec'e gore near/far degerleri gerekirse
 * [0,1] araligina clamp edilir; hata olusmaz.
 */

void ViewportandClipping_DepthRangef_TC_001(void)
{
    GLdouble depthRange[2];
    GLenum err;

    resetState();

    glDepthRange(0.0,1.0);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure,
                      "Varsayilan cagri hata uretti: 0x%X",err);
        return;
    }

    glGetDoublev(GL_DEPTH_RANGE,depthRange);

    if(fabs(depthRange[0]-0.0)>1e-6 ||
       fabs(depthRange[1]-1.0)>1e-6)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure,
                      "Varsayilan state hatali: (%lf,%lf)",
                      depthRange[0],depthRange[1]);
        return;
    }

    glDepthRange(1.0,0.0);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure,
                      "Ters depth range hata uretti: 0x%X",err);
        return;
    }

    glGetDoublev(GL_DEPTH_RANGE,depthRange);

    if(fabs(depthRange[0]-1.0)>1e-6 ||
       fabs(depthRange[1]-0.0)>1e-6)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure,
                      "Ters depth range state hatali: (%lf,%lf)",
                      depthRange[0],depthRange[1]);
        return;
    }

    resetState();

    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2: Parametrik tarama
 * ============================================================ */

/*
 * near ve far parametreleri [-1.0,+2.0] araliginda
 * sistematik olarak taranir. Tum cagrilar hatasiz
 * kabul edilmeli ve implementasyon kararliligini
 * korumalidir.
 */

void ViewportandClipping_DepthRangef_TC_002(void)
{
    int nearStep;
    int farStep;
    int passCount=0;
    int failCount=0;

    resetState();

    for(nearStep=-10;nearStep<=20;nearStep++)
    {
        for(farStep=-10;farStep<=20;farStep++)
        {
            GLdouble nearVal=nearStep/10.0;
            GLdouble farVal=farStep/10.0;
            GLenum err;

            glDepthRange(nearVal,farVal);

            err=glGetError();

            if(err!=GL_NO_ERROR)
            {
                TEST_LOG_FAIL(test_case_2, test_procedure,
                              "near=%lf far=%lf error=0x%X",
                              nearVal,farVal,err);
                failCount++;
            }
            else
            {
                passCount++;
            }
        }
    }

    TEST_LOG_INFO("Sonuc: %d PASS, %d FAIL",passCount,failCount);

    if(failCount!=0)
    {
        TEST_LOG_FAIL(test_case_2, test_procedure,
                      "Basarisiz cagri sayisi: %d",failCount);
        return;
    }

    resetState();

    TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3: Ozel kayan nokta degerleri
 * ============================================================ */

/*
 * IEEE-754 ozel degerleri altinda davranisi degerlendirir.
 * Spec bu degerleri acikca tanimlamaz ancak implementasyon
 * cokmemeli, hata kuyrugunu bozmamali ve state'i gecersiz
 * hale getirmemelidir.
 */

void ViewportandClipping_DepthRangef_TC_003(void)
{
    GLenum err;
    GLdouble tests[][2]=
    {
        {0.0,1.0},
        {1.0,0.0},
        {-1.0,2.0},
        {-DBL_MAX,DBL_MAX},
        {DBL_MAX,-DBL_MAX},
        {DBL_MIN,DBL_MAX},
        {-DBL_MIN,DBL_MIN},
        {1000000.0,-1000000.0},
        {NAN,NAN},
        {INFINITY,INFINITY},
        {-INFINITY,-INFINITY}
    };

    int count=sizeof(tests)/sizeof(tests[0]);
    int i;

    resetState();

    for(i=0;i<count;i++)
    {
        glDepthRange(tests[i][0],tests[i][1]);

        err=glGetError();

        TEST_LOG_INFO("near=%lf far=%lf -> 0x%X",
                      tests[i][0],tests[i][1],err);

        if(err!=GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_3, test_procedure,
                          "Beklenmeyen hata olustu: 0x%X",err);
            return;
        }
    }

    resetState();

    TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4: State sorgusu ve clamp davranisi
 * ============================================================ */

/*
 * glDepthRange cagrisindan sonra GL_DEPTH_RANGE sorgulanir.
 * Spec'e gore near ve far degerleri [0,1] araligina clamp
 * edilerek saklanir. State sorgusu bu clamp edilmis
 * degerleri dondurmelidir.
 */

void ViewportandClipping_DepthRangef_TC_004(void)
{
    GLdouble depthRange[2];
    GLenum err;

    GLdouble values[][2]=
    {
        {0.0,1.0},
        {1.0,0.0},
        {0.25,0.75},
        {-1.0,2.0},
        {5.0,-5.0}
    };

    int count=sizeof(values)/sizeof(values[0]);
    int i;

    resetState();

    for(i=0;i<count;i++)
    {
        glDepthRange(values[i][0],values[i][1]);

        err=glGetError();

        if(err!=GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_4, test_procedure,
                          "Error=0x%X",err);
            return;
        }

        glGetDoublev(GL_DEPTH_RANGE,depthRange);

        if(depthRange[0]<0.0 || depthRange[0]>1.0)
        {
            TEST_LOG_FAIL(test_case_4, test_procedure,
                          "Near aralik disinda: %lf",depthRange[0]);
            return;
        }

        if(depthRange[1]<0.0 || depthRange[1]>1.0)
        {
            TEST_LOG_FAIL(test_case_4, test_procedure,
                          "Far aralik disinda: %lf",depthRange[1]);
            return;
        }
    }

    if(!checkStatePreserved(test_case_4,0.0,1.0))
        return;

    resetState();

    TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5: Error Queue ve State Preservation
 * ============================================================ */

/*
 * glDepthRange normal kosullarda hata uretmez.
 * Arka arkaya farkli degerlerle cagrildiginda hata kuyrugu
 * temiz kalmali ve state dogru sekilde guncellenmelidir.
 */

void ViewportandClipping_DepthRangef_TC_005(void)
{
    GLdouble depthRange[2];
    GLenum err;

    resetState();

    glDepthRange(0.20,0.80);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure,
                      "Error=0x%X",err);
        return;
    }

    glGetDoublev(GL_DEPTH_RANGE,depthRange);

    if(depthRange[0]<0.0 || depthRange[0]>1.0 ||
       depthRange[1]<0.0 || depthRange[1]>1.0)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure,
                      "Depth range state bozuldu: (%lf,%lf)",
                      depthRange[0],depthRange[1]);
        return;
    }

    glDepthRange(-1000.0,1000.0);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure,
                      "Error=0x%X",err);
        return;
    }

    glGetDoublev(GL_DEPTH_RANGE,depthRange);

    if(depthRange[0]<0.0 || depthRange[0]>1.0 ||
       depthRange[1]<0.0 || depthRange[1]>1.0)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure,
                      "Clamp sonrasi state gecersiz: (%lf,%lf)",
                      depthRange[0],depthRange[1]);
        return;
    }

    resetState();

    TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6: Random Stress Test
 * ============================================================ */

/*
 * glDepthRange 1.000.000 kez rastgele degerlerle cagrilir.
 * Beklenmeyen GL hatasi, context kaybi veya crash olmamalidir.
 */

void ViewportandClipping_DepthRangef_TC_006(void)
{
    unsigned int i;

    resetState();

    srand(12345);

    for(i=0;i<1000000;i++)
    {
        GLdouble nearValue=((GLdouble)rand()/RAND_MAX)*20.0-10.0;
        GLdouble farValue=((GLdouble)rand()/RAND_MAX)*20.0-10.0;

        GLenum err;

        glDepthRange(nearValue,farValue);

        err=glGetError();

        if(err!=GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_6, test_procedure,
                          "Iteration: %u Near: %lf Far: %lf Error: 0x%X",
                          i,nearValue,farValue,err);
            return;
        }
    }

    resetState();
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * Tum glDepthRange Robustness Testlerini Calistir
 * ============================================================ */

void Run_glDepthRange_Robustness(void)
{
    ViewportandClipping_DepthRangef_TC_001();
    ViewportandClipping_DepthRangef_TC_002();
    ViewportandClipping_DepthRangef_TC_003();
    ViewportandClipping_DepthRangef_TC_004();
    ViewportandClipping_DepthRangef_TC_005();
    ViewportandClipping_DepthRangef_TC_006();
}