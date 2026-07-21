#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>

/* ============================================================
 * Test altyapısı :
 *
 * resetState:
 *   Her testten önce viewport varsayılan değerlere döndürülür ve
 *   OpenGL hata kuyruğu tamamen temizlenir.
 * checkStatePreserved:
 *   Geçersiz bir glViewport çağrısından sonra viewport durumunun
 *   değişmediğini doğrular. Beklenmeyen bir değişiklik olması
 *   durumunda assert ile test sonlandırılır.
 * ============================================================ */

static void resetState(void)
{
    glViewport(0, 0, 640, 480);
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    if (viewport[0] != x || viewport[1] != y || viewport[2] != width || viewport[3] != height)
    {
        printf("  [FAIL] Viewport durumu bozuldu!\n");
        printf("         Beklenen : (%d, %d, %d, %d)\n", x, y, width, height);
        printf("         Gercek   : (%d, %d, %d, %d)\n", viewport[0], viewport[1], viewport[2], viewport[3]);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Temel Robustness Doğrulaması
 * ============================================================ */

/*
 * glViewport fonksiyonunun temel sözleşmesini doğrular.
 *
 * - Geçerli viewport parametreleri hata üretmemelidir.
 * - Negatif width veya height değerleri
 *   GL_INVALID_VALUE ile reddedilmelidir.
 * - Başarısız çağrılar mevcut viewport durumunu
 *   değiştirmemelidir.
 *
 * Böylece hatalı bir çağrının sonraki çizim işlemlerini
 * etkilemediği doğrulanmış olur.
 */

void test_viewport_basicRobustness(void)
{
    GLint viewport[4];
    GLenum err;

    printf("TEST: Basic Robustness\n");
    resetState();

    /* Geçerli viewport */
    glViewport(10, 20, 640, 480);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glGetIntegerv(GL_VIEWPORT, viewport);

    assert(viewport[0] == 10);
    assert(viewport[1] == 20);
    assert(viewport[2] == 640);
    assert(viewport[3] == 480);

    /* Negatif width */
    glViewport(10, 20, -1, 480);
    err = glGetError();
    assert(err == GL_INVALID_VALUE);

    checkStatePreserved(10, 20, 640, 480);

    /* Negatif height */
    glViewport(10, 20, 640, -1);
    err = glGetError();
    assert(err == GL_INVALID_VALUE);

    checkStatePreserved(10, 20, 640, 480);
    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Parametrik Tarama — Negatif Boyutlar
 * ============================================================ */

/*
 * Width ve height parametreleri geniş bir aralıkta
 * sistematik olarak taranır.
 *
 * Beklenen davranış:
 *
 * width  < 0  --> GL_INVALID_VALUE
 * height < 0  --> GL_INVALID_VALUE
 *
 * width >= 0 ve height >= 0
 * ise GL_NO_ERROR dönmelidir.
 *
 * Bu test tek bir sınır değer yerine binlerce farklı
 * kombinasyonu deneyerek implementasyona özgü hataları
 * ortaya çıkarmayı amaçlar.
 */

void test_viewport_negativeDimensions(void)
{
    int w;
    int h;
    int passCount = 0;
    int failCount = 0;

    printf("TEST: Negative Dimension Sweep\n");
    resetState();

    for (w = -100; w <= 100; w++)
    {
        for (h = -100; h <= 100; h++)
        {
            GLenum expected;
            GLenum err;
            expected = (w < 0 || h < 0) ? GL_INVALID_VALUE : GL_NO_ERROR;

            glViewport(0, 0, w, h);
            err = glGetError();
            if (err != expected)
            {
                printf("  [FAIL] width=%d height=%d " "beklenen=0x%X gelen=0x%X\n", w, h, expected, err);
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
 * TEST 3: Sınır Koordinatları (Boundary Coordinates)
 * ============================================================ */
/*
 * Viewport başlangıç koordinatları için sınır değer analizi yapılır.
 *
 * Amaç;
 *
 *  - Çok büyük pozitif koordinatlar
 *  - Çok büyük negatif koordinatlar
 *  - Sıfır koordinatları
 *
 * kullanıldığında sürücünün çökmeden çalıştığını doğrulamaktır.
 *
 * OpenGL ES/OpenGL spesifikasyonuna göre x ve y değerleri
 * negatif olabilir. Bu nedenle hata beklenmez.
 */

void test_viewport_boundaryCoordinates(void)
{
    GLint viewport[4];
    GLenum err;

    printf("TEST: Boundary Coordinates\n");
    resetState();
    GLint coordinates[][2] =
    {
        {0,0},
        {-1,-1},
        {INT_MAX,INT_MAX},
        {INT_MIN,INT_MIN},
        {INT_MAX,INT_MIN},
        {INT_MIN,INT_MAX},
        {-1000000,-1000000},
        {1000000,1000000}
    };

    int count = sizeof(coordinates)/sizeof(coordinates[0]);
    for(int i=0;i<count;i++)
    {
        glViewport(coordinates[i][0], coordinates[i][1], 640, 480);
        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("  [FAIL] x=%d y=%d Error=0x%X\n", coordinates[i][0], coordinates[i][1], err);
            assert(0);
        }

        glGetIntegerv(GL_VIEWPORT, viewport);
        assert(viewport[0] == coordinates[i][0]);
        assert(viewport[1] == coordinates[i][1]);
    }

    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 4: Maksimum Viewport Boyutları
 * ============================================================ */

/*
 * Implementasyonun desteklediği maksimum viewport
 * boyutları sorgulanır.
 *
 * Daha sonra;
 *
 *  1) Tam maksimum değer kullanılır.
 *  2) Maksimum değerin iki katı kullanılır.
 *  3) INT_MAX kullanılır.
 *
 * Amaç implementasyonun sınır değerlerde
 * crash oluşturmadan çalıştığını doğrulamaktır.
 */

void test_viewport_limits(void)
{
    GLint maxViewport[2];
    GLenum err;
    printf("TEST: Maximum Viewport Limits\n");
    resetState();

    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewport);
    printf("  Desteklenen maksimum viewport : %d x %d\n", maxViewport[0], maxViewport[1]);

    /*------------------------------------------------------*/
    /* Maksimum desteklenen viewport                        */
    /*------------------------------------------------------*/

    glViewport(0, 0, maxViewport[0], maxViewport[1]);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    /*------------------------------------------------------*/
    /* Maksimum değerin iki katı                            */
    /*------------------------------------------------------*/

    glViewport(0, 0, maxViewport[0] * 2, maxViewport[1] * 2);

    err = glGetError();
    if(err != GL_NO_ERROR)
    {
        printf("  Uyari : Buyuk viewport Error=0x%X\n", err);
    }

    /*------------------------------------------------------*/
    /* INT_MAX                                              */
    /*------------------------------------------------------*/

    glViewport(0, 0, INT_MAX, INT_MAX);

    err = glGetError();
    if(err != GL_NO_ERROR)
    {
        printf("  Uyari : INT_MAX Error=0x%X\n", err);
    }

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Hata Kuyruğu ve Durumun Korunması
 * ============================================================ */

/*
 * Geçersiz glViewport() çağrılarından sonra:
 *
 *  - Doğru OpenGL hatasının üretildiği,
 *  - Viewport durumunun değişmediği,
 *  - Sonraki geçerli çağrıların normal şekilde çalıştığı
 *
 * doğrulanır.
 */

void test_viewport_errorQueue(void)
{
    GLenum err;

    printf("TEST: Error Queue and State Preservation\n");
    resetState();

    /* Bilinen geçerli viewport ayarla */
    glViewport(50, 50, 400, 300);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    /* Geçersiz çağrı */
    glViewport(50, 50, -1, 300);

    err = glGetError();
    assert(err == GL_INVALID_VALUE);

    /* Durum değişmemeli */
    checkStatePreserved(50, 50, 400, 300);

    /* Sonraki geçerli çağrı normal çalışmalı */
    glViewport(0, 0, 640, 480);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 6: Rastgele Dayanıklılık (Stress/Fuzz Testi)
 * ============================================================ */
/*
 * glViewport() fonksiyonu rastgele oluşturulan
 * milyonlarca parametre ile çağrılır.
 *
 * Amaç;
 *
 *  - Driver crash
 *  - Assertion failure
 *  - Context kaybı
 *  - Bellek bozulması
 *  - Beklenmeyen OpenGL hataları
 *
 * gibi robustness problemlerini ortaya çıkarmaktır.
 */

void test_viewport_stress(void)
{
    unsigned int i;
    printf("TEST: Random Stress Test\n");

    resetState();
    srand(12345);

    for(i = 0; i < 1000000; i++)
    {
        GLint x = (rand() % 2000000000) - 1000000000;

        GLint y = (rand() % 2000000000) - 1000000000;

        GLsizei width = (rand() % 2000000000) - 1000000000;

        GLsizei height = (rand() % 2000000000) - 1000000000;

        GLenum expected;

        if(width < 0 || height < 0)
            expected = GL_INVALID_VALUE;
        else
            expected = GL_NO_ERROR;

        glViewport(x, y, width, height);
        GLenum err = glGetError();

        if(err != expected)
        {
            printf("\n[FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Viewport  : (%d,%d,%d,%d)\n", x, y, width, height);
            printf("Expected  : 0x%X\n", expected);
            printf("Received  : 0x%X\n", err);
            assert(0);
        }
    }

    resetState();
    printf("  [PASS] 1,000,000 rastgele viewport testi başarıyla tamamlandı.\n\n");
}


/* ============================================================
 * Tüm glViewport Robustness Testlerini Çalıştır
 * ============================================================ */

void Run_glViewport_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("      glViewport Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_viewport_basicRobustness();
    test_viewport_negativeDimensions();
    test_viewport_boundaryCoordinates();
    test_viewport_limits();
    test_viewport_errorQueue();
    test_viewport_stress();

    printf("=============================================\n");
    printf(" Tüm glViewport Robustness Testleri Başarılı\n");
    printf("=============================================\n\n");
}