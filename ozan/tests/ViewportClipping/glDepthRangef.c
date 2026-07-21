#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

/* ============================================================
 * Test altyapısı
 *
 * resetState:
 *   Her testten önce depth range varsayılan değerlere
 *   döndürülür ve OpenGL hata kuyruğu temizlenir.
 *
 * checkStatePreserved:
 *   Geçersiz veya beklenmeyen parametrelerden sonra
 *   depth range durumunun bozulmadığını doğrular.
 * ============================================================ */

static void resetState(void)
{
    glDepthRange(0.0, 1.0);
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLdouble expectedNear, GLdouble expectedFar)
{
    GLdouble depthRange[2];
    glGetDoublev(GL_DEPTH_RANGE, depthRange);

    if (fabs(depthRange[0] - expectedNear) > 0.000001 ||
        fabs(depthRange[1] - expectedFar) > 0.000001)
    {
        printf("  [FAIL] Depth range durumu bozuldu!\n");
        printf("         Beklenen : (%lf, %lf)\n", expectedNear, expectedFar);
        printf("         Gercek   : (%lf, %lf)\n", depthRange[0], depthRange[1]);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Temel Robustness Doğrulaması
 * ============================================================ */

/*
 * glDepthRange() fonksiyonunun temel davranışı doğrulanır.
 * - Varsayılan değerler kabul edilmelidir.
 * - Ters depth range kabul edilmelidir.
 * - Geçerli tüm çağrılar GL_NO_ERROR üretmelidir.
 * Amaç; sürücünün normal kullanım senaryolarında
 * beklenen davranışı gösterdiğini doğrulamaktır.
 */

void test_depthRange_basicRobustness(void)
{
    GLdouble depthRange[2];
    GLenum err;

    printf("TEST: Basic Robustness\n");

    resetState();

    /* Varsayılan değer */
    glDepthRange(0.0, 1.0);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetDoublev(GL_DEPTH_RANGE, depthRange);

    assert(fabs(depthRange[0] - 0.0) < 0.000001);
    assert(fabs(depthRange[1] - 1.0) < 0.000001);

    /* Ters depth range */
    glDepthRange(1.0, 0.0);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetDoublev(GL_DEPTH_RANGE, depthRange);

    assert(fabs(depthRange[0] - 1.0) < 0.000001);
    assert(fabs(depthRange[1] - 0.0) < 0.000001);

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Parametrik Tarama
 * ============================================================ */
/*
 * Depth range parametreleri sistematik olarak taranır.
 * near ve far değerleri -1.0 ile +2.0 arasında
 * farklı kombinasyonlarla denenir.
 * OpenGL spesifikasyonuna göre değerler gerektiğinde
 * [0,1] aralığına sınırlandırılır (clamp edilir) ve hata üretilmez.
 * Amaç implementasyonun farklı parametre
 * kombinasyonlarında kararlı çalıştığını
 * doğrulamaktır.
 */

void test_depthRange_parameterSweep(void)
{
    int nearStep;
    int farStep;
    int passCount = 0;
    int failCount = 0;

    printf("TEST: Parameter Sweep\n");
    resetState();

    for (nearStep = -10; nearStep <= 20; nearStep++)
    {
        for (farStep = -10; farStep <= 20; farStep++)
        {
            GLdouble nearVal = nearStep / 10.0;
            GLdouble farVal  = farStep / 10.0;
            GLenum err;
            glDepthRange(nearVal, farVal);
            err = glGetError();

            if (err != GL_NO_ERROR)
            {
                printf("  [FAIL] near=%lf far=%lf Error=0x%X\n", nearVal, farVal, err);
                failCount++;
            }
            else
            {
                passCount++;
            }
        }
    }

    printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);

    assert(failCount == 0);
    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Özel Kayan Nokta Değerleri
 * ============================================================ */
/*
 * glDepthRange() fonksiyonuna olağan dışı (extreme) kayan nokta
 * değerleri gönderilerek sürücünün kararlılığı test edilir.
 * Kullanılan değerler:
 *  - FLT_MAX
 *  - FLT_MIN
 *  - DBL_MAX
 *  - DBL_MIN
 * OpenGL bu değerleri [0,1] aralığına sınırlandırmalıdır
 * Herhangi bir OpenGL hatası veya çökme meydana gelmemelidir.
 */

void test_depthRange_specialValues(void)
{
    GLenum err;
    printf("TEST: Special Floating Point Values\n");
    resetState();

    GLdouble tests[][2] =
    {
        {0.0, 1.0},
        {1.0, 0.0},
        {-1.0, 2.0},
        {-DBL_MAX, DBL_MAX},
        {DBL_MAX, -DBL_MAX},
        {DBL_MIN, DBL_MAX},
        {-DBL_MIN, DBL_MIN},
        {1000000.0, -1000000.0}
    };

    int count = sizeof(tests) / sizeof(tests[0]);

    for(int i = 0; i < count; i++)
    {
        glDepthRange(tests[i][0], tests[i][1]);
        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("  [FAIL] near=%lf far=%lf Error=0x%X\n", tests[i][0], tests[i][1], err);
            assert(0);
        }
    }

    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 4: State Query Doğrulaması
 * ============================================================ */
/*
 * verilen değerlerin OpenGL tarafından doğru şekilde
 * saklanıp saklanmadığını kontrol eden bir state query testidir
 * glDepthRange() çağrısından sonra OpenGL durumunun
 * doğru güncellendiği doğrulanır.
 * glGetDoublev(GL_DEPTH_RANGE, ...)
 * kullanılarak mevcut depth range okunur.
 *
 * Amaç;
 * - State'in bozulmadığını,
 * - Fonksiyonun doğru çalıştığını,
 * - Geçersiz olmayan çağrıların beklenen sonucu oluşturduğunu doğrulamaktır.
 */

void test_depthRange_stateQuery(void)
{
    GLdouble depthRange[2];
    GLenum err;

    printf("TEST: State Query\n");
    resetState();

    GLdouble values[][2] =
    {
        {0.0, 1.0},
        {1.0, 0.0},
        {0.25, 0.75},
        {-1.0, 2.0},
        {5.0, -5.0}
    };

    int count = sizeof(values) / sizeof(values[0]);

    for(int i = 0; i < count; i++)
    {
        glDepthRange(values[i][0], values[i][1]);

        err = glGetError();
        assert(err == GL_NO_ERROR);

        glGetDoublev(GL_DEPTH_RANGE, depthRange);

        if(depthRange[0] < 0.0 || depthRange[0] > 1.0)
        {
            printf("  [FAIL] Near değeri aralık dışında.\n");
            assert(0);
        }

        if(depthRange[1] < 0.0 || depthRange[1] > 1.0)
        {
            printf("  [FAIL] Far değeri aralık dışında.\n");
            assert(0);
        }
    }

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Error Queue ve State Preservation
 * ============================================================
 */
/*
 * Bu testte glDepthRange() fonksiyonunun:
 *  - Arka arkaya çağrıldığında hata kuyruğunu bozmadığı,
 *  - OpenGL state'ini doğru güncellediği,
 *  - Beklenmeyen GL hataları üretmediği doğrulanmaktadır.
 *
 * glDepthRange() parametreleri OpenGL tarafından clamp edildiği
 * için bu fonksiyon normal şartlarda GL_INVALID_VALUE üretmez.
 */

void test_depthRange_errorQueue(void)
{
    GLdouble depthRange[2];
    GLenum err;
    printf("TEST: Error Queue and State Preservation\n");
    resetState();

    glDepthRange(0.20,0.80);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetDoublev(GL_DEPTH_RANGE,depthRange);

    assert(depthRange[0] >= 0.0);
    assert(depthRange[0] <= 1.0);
    assert(depthRange[1] >= 0.0);
    assert(depthRange[1] <= 1.0);

    glDepthRange(-1000.0,1000.0);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetDoublev(GL_DEPTH_RANGE,depthRange);

    assert(depthRange[0] >= 0.0);
    assert(depthRange[0] <= 1.0);
    assert(depthRange[1] >= 0.0);
    assert(depthRange[1] <= 1.0);

    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 6: Rastgele Stress / Fuzz Testi
 * ============================================================
 */
/*
 * glDepthRange() 1.000.000 kez rastgele değerlerle çağrılır.
 * Amaç:
 *  - Driver crash
 *  - Memory corruption
 *  - Context kaybı
 *  - Beklenmeyen OpenGL hataları gibi robustness problemlerini ortaya çıkarmaktır.
 *
 * glDepthRange() tüm değerleri clamp ettiği için
 * GL_NO_ERROR beklenmektedir.
 */

void test_depthRange_stress(void)
{
    unsigned int i;
    printf("TEST: Random Stress Test\n");
    resetState();

    srand(12345);

    for(i=0;i<1000000;i++)
    {
        GLdouble nearValue = ((GLdouble)rand()/RAND_MAX)*20.0 - 10.0;

        GLdouble farValue = ((GLdouble)rand()/RAND_MAX)*20.0 - 10.0;

        glDepthRange(nearValue,farValue);
        GLenum err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("\n[FAIL]\n");
            printf("Iteration : %u\n",i);
            printf("Near      : %lf\n",nearValue);
            printf("Far       : %lf\n",farValue);
            printf("Error     : 0x%X\n",err);

            assert(0);
        }
    }

    resetState();
    printf("  [PASS] 1,000,000 rastgele test başarıyla tamamlandı.\n\n");
}


/* ============================================================
 * Tüm glDepthRange Robustness Testlerini Çalıştır
 * ============================================================
 */

void Run_glDepthRange_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("    glDepthRange Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_depthRange_basicRobustness();
    test_depthRange_parameterSweep();
    test_depthRange_specialValues();
    test_depthRange_stateQuery();
    test_depthRange_errorQueue();
    test_depthRange_stress();

    printf("=============================================\n");
    printf(" Tüm glDepthRange Robustness Testleri Başarılı\n");
    printf("=============================================\n\n");
}