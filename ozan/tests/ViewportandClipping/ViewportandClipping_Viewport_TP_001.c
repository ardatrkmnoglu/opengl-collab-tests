#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "ViewportandClipping_Viewport_TP_001";
static const char* test_case_1 = "ViewportandClipping_Viewport_TC_001";
static const char* test_case_2 = "ViewportandClipping_Viewport_TC_002";
static const char* test_case_3 = "ViewportandClipping_Viewport_TC_003";
static const char* test_case_4 = "ViewportandClipping_Viewport_TC_004";
static const char* test_case_5 = "ViewportandClipping_Viewport_TC_005";
static const char* test_case_6 = "ViewportandClipping_Viewport_TC_006";

/* ============================================================
 * TEST 1: Temel Robustness Dogrulamasi
 * ============================================================ */

/*
 * Gecerli viewport degerleri hata uretmemeli,
 * negatif width veya height GL_INVALID_VALUE
 * ile reddedilmeli ve state korunmalidir.
 */

void ViewportandClipping_Viewport_TC_001(void)
{
    GLint viewport[4];
    GLenum err;
    resetState_Viewport();

    glViewport(10,20,640,480);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Gecerli viewport reddedildi.");
        return;
    }

    glGetIntegerv(GL_VIEWPORT,viewport);

    if(viewport[0]!=10 ||
       viewport[1]!=20 ||
       viewport[2]!=640 ||
       viewport[3]!=480)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Viewport state hatali.");
        return;
    }

    glViewport(10,20,-1,480);

    err=glGetError();

    if(err!=GL_INVALID_VALUE)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Negatif width kabul edildi.");
        return;
    }

    if(!checkViewport(test_case_1, test_procedure, 10,20,640,480))
        return;

    glViewport(10,20,640,-1);

    err=glGetError();

    if(err!=GL_INVALID_VALUE)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Negatif height kabul edildi.");
        return;
    }

    if(!checkViewport(test_case_1, test_procedure, 10,20,640,480))
        return;
    resetState_Viewport();

    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2: Parametrik Tarama
 * ============================================================ */

/*
 * Width ve height parametreleri sistematik olarak
 * taranir.
 *
 * width<0 veya height<0 ise
 * GL_INVALID_VALUE beklenir.
 */

void ViewportandClipping_Viewport_TC_002(void)
{
    int w,h;
    int passCount=0;
    int failCount=0;

    resetState_Viewport();

    for(w=-100;w<=100;w++)
    {
        for(h=-100;h<=100;h++)
        {
            GLenum expected;
            GLenum err;

            expected=(w<0 || h<0) ?
                     GL_INVALID_VALUE :
                     GL_NO_ERROR;

            glViewport(0,0,w,h);

            err=glGetError();

            if(err!=expected)
            {
                TEST_LOG_FAIL(test_case_2, test_procedure, "width=%d height=%d expected=0x%X Error=0x%X.", w,h,expected,err);
                failCount++;
            }
            else
            {
                passCount++;
            }
        }
    }

    if(failCount!=0)
    {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Basarisiz cagri sayisi: %d", failCount);
        return;
    }

    resetState_Viewport();

    TEST_LOG_SUCCESS(test_case_2, test_procedure);


}

/* ============================================================
 * TEST 3: Sinir koordinatlari
 *
 * Viewport baslangic koordinatlari icin sinir deger analizi
 * yapilir. Negatif, sifir ve cok buyuk koordinatlar altinda
 * implementasyonun hata uretmeden ve durumu dogru guncelleyerek
 * calistigi dogrulanir.
 *
 * Not: x+width ve y+height hesaplamalarinda tasma olusmamasi
 * icin koordinat sinirlari INT_MAX/2 ve INT_MIN/2 ile
 * sinirlandirilmistir.
 * ============================================================ */

void ViewportandClipping_Viewport_TC_003(void)
{
    GLint viewport[4];
    GLenum err;
    GLint coordinates[][2] = {
        {0, 0},
        {-1, -1},
        {INT_MAX / 2, INT_MAX / 2},
        {INT_MIN / 2, INT_MIN / 2},
        {INT_MAX / 2, INT_MIN / 2},
        {INT_MIN / 2, INT_MAX / 2},
        {-1000000, -1000000},
        {1000000, 1000000}
    };
    int i;
    int count = sizeof(coordinates) / sizeof(coordinates[0]);

    resetState_Viewport();

    for (i = 0; i < count; i++) {

        glViewport(coordinates[i][0], coordinates[i][1], 640, 480);
        err = glGetError();

        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case_3, test_procedure, "x=%d y=%d -> 0x%X", coordinates[i][0], coordinates[i][1], err);
            return;
        }

        glGetIntegerv(GL_VIEWPORT, viewport);

        if (viewport[0] != coordinates[i][0] || viewport[1] != coordinates[i][1]) {

            TEST_LOG_FAIL(test_case_3, test_procedure,
                          "Viewport koordinatlari dogru saklanmadi. Beklenen: (%d,%d) Gercek: (%d,%d)",
                          coordinates[i][0], coordinates[i][1], viewport[0], viewport[1]);
            return;

            }
    }

    resetState_Viewport();

    TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4: Viewport Limitleri
 *
 * Implementasyonun destekledigi maksimum viewport boyutlari
 * sorgulanir. Maksimum degerler, bunlarin ustundeki degerler
 * ve INT_MAX ile davranis gozlemlenir.
 * ============================================================ */

void ViewportandClipping_Viewport_TC_004(void)
{
    GLint maxViewport[2];
    GLenum err;

    resetState_Viewport();

    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewport);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "GL_MAX_VIEWPORT_DIMS okunamadi (0x%X)", err);
        return;
    }

    glViewport(0, 0, maxViewport[0], maxViewport[1]);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Maksimum viewport reddedildi");
        return;
    }

    glViewport(0, 0, maxViewport[0] * 2, maxViewport[1] * 2);

    err = glGetError();

    if (err != GL_NO_ERROR)
        TEST_LOG_INFO("Buyuk viewport -> 0x%X", err);

    glViewport(0, 0, INT_MAX, INT_MAX);

    err = glGetError();

    if (err != GL_NO_ERROR)
        TEST_LOG_INFO("INT_MAX viewport -> 0x%X", err);

    resetState_Viewport();

    TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5: Hata kuyrugu butunlugu
 *
 * Gecersiz viewport boyutlari altinda hata kuyrugunun dogru
 * calistigini ve basarisiz cagrilarin viewport durumunu
 * degistirmedigini dogrular.
 * ============================================================ */

void ViewportandClipping_Viewport_TC_005(void) {
    GLenum err;

    resetState_Viewport();

    glViewport(50, 50, 400, 300);
    if (glGetError() != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Gecerli viewport ayarlanamadi");
        return;
    }

    glViewport(50, 50, -1, 300);
    err = glGetError();

    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Beklenen=0x%X Gelen=0x%X",
                      GL_INVALID_VALUE, err);
        return;
    }

    if (!checkViewport(test_case_5, test_procedure, 50, 50, 400, 300))
        return;

    glViewport(0, 0, 640, 480);
    if (glGetError() != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Gecerli viewport sonrasi hata olustu");
        return;
    }

    resetState_Viewport();

    TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6: Rastgele stres testi
 *
 * Rastgele viewport parametreleri ile surucunun uzun sureli
 * kararliligi test edilir. Negatif width/height
 * GL_INVALID_VALUE, diger durumlar GL_NO_ERROR donmelidir.
 *
 * Not: width ve height degerleri hem pozitif hem negatif
 * olabilecek sekilde uretilir; boylece gecerli ve gecersiz
 * cagrilar birlikte taranir.
 * ============================================================ */

void ViewportandClipping_Viewport_TC_006(void) {
    unsigned int i;
    unsigned int validCount = 0;
    unsigned int invalidCount = 0;

    resetState_Viewport();

    srand(12345);

    for (i = 0; i < 1000000; i++) {
        GLint x = randInt32();
        GLint y = randInt32();
        GLsizei width  = (GLsizei)(randInt32() % 4096);
        GLsizei height = (GLsizei)(randInt32() % 4096);

        GLenum expected;
        GLenum err;

        expected = (width < 0 || height < 0)
                 ? GL_INVALID_VALUE
                 : GL_NO_ERROR;

        if (expected == GL_NO_ERROR)
            validCount++;
        else
            invalidCount++;

        glViewport(x, y, width, height);
        err = glGetError();

        if (err != expected) {
            TEST_LOG_FAIL(test_case_6, test_procedure,
                          "Iteration=%u Viewport=(%d,%d,%d,%d) Beklenen=0x%X Gelen=0x%X",
                          i, x, y, width, height, expected, err);
            return;
        }
    }

    resetState_Viewport();

    TEST_LOG_INFO("1000000 rastgele cagri tamamlandi (gecerli: %u, gecersiz: %u)",
                  validCount, invalidCount);
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * Tum glViewport robustness testlerini calistir
 * ============================================================ */

void Run_glViewport_Robustness(void) {
    ViewportandClipping_Viewport_TC_001();
    ViewportandClipping_Viewport_TC_002();
    ViewportandClipping_Viewport_TC_003();
    ViewportandClipping_Viewport_TC_004();
    ViewportandClipping_Viewport_TC_005();
    ViewportandClipping_Viewport_TC_006();
}