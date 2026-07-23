#include <GL/gl.h>
#include <stdio.h>
#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "SpecialFunctions_Flush_TP_001";
static const char* test_case_1 = "SpecialFunctions_Flush_TC_001";
static const char* test_case_2 = "SpecialFunctions_Flush_TC_002";
static const char* test_case_3 = "SpecialFunctions_Flush_TC_003";
static const char* test_case_4 = "SpecialFunctions_Flush_TC_004";
static const char* test_case_5 = "SpecialFunctions_Flush_TC_005";
static const char* test_case_6 = "SpecialFunctions_Flush_TC_006";
static const char* test_case_7 = "SpecialFunctions_Flush_TC_007";

/* ============================================================
 * Test altyapısı
 *
 * resetState:
 *   Her testten önce OpenGL hata kuyruğu temizlenir.
 *
 * checkViewportPreserved:
 *   glFlush() çağrısından sonra OpenGL state'inin
 *   değişmediğini doğrular.
 * ============================================================
 */

static int checkViewportPreserved(const char* test_case,
                                  GLint x,GLint y,GLsizei width,GLsizei height)
{
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);

    if(viewport[0] != x || viewport[1] != y || viewport[2] != width ||viewport[3] != height)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Viewport state bozuldu. Beklenen: (%d,%d,%d,%d) Gercek: (%d,%d,%d,%d)",
                      x, y, width, height,
                      viewport[0], viewport[1], viewport[2], viewport[3]);
        return 0;
    }

    return 1;
}


/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 */

/*
 * glFlush() daha önce gönderilmiş OpenGL
 * komutlarını GPU'ya göndermelidir.
 *
 * Fonksiyon hata üretmemeli ve
 * geçerli komutlardan sonra
 * GL_NO_ERROR döndürmelidir.
 */

void test_flush_basicRobustness(void)
{
    GLenum err;

    while(glGetError()!=GL_NO_ERROR) {};

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glFlush();

    err = glGetError();

    if(err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Error : 0x%X", err);
        return;
    }

    TEST_LOG_SUCCESS(test_case_1, test_procedure);
}


/* ============================================================
 * TEST 2 : State Preservation
 * ============================================================
 */

/*
 * glFlush() yalnızca komutların yürütülmesini
 * başlatmalıdır.
 *
 * OpenGL state'ini değiştirmemelidir.
 *
 * Viewport ayarlanır.
 * glFlush() çağrılır.
 *
 * Viewport'un aynı kaldığı doğrulanır.
 */

void test_flush_statePreservation(void)
{
    GLenum err;

    while(glGetError()!=GL_NO_ERROR) {};

    glViewport(10,20,320,240);
    err = glGetError();
    if(err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Error : 0x%X", err);
        return;
    }

    glFlush();

    err = glGetError();
    if(err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Error : 0x%X", err);
        return;
    }

    if(!checkViewportPreserved(test_case_2,10,20,320,240))
        return;

    TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : Error Queue Preservation
 * ============================================================
 */

/*
 * glFlush() hata kuyruğunu değiştirmemelidir.
 *
 * Geçerli OpenGL komutlarından sonra çağrıldığında
 * GL_NO_ERROR üretmeli ve hata kuyruğunu
 * bozmamalıdır.
 */

void test_flush_errorQueuePreservation(void)
{
    GLenum err;

    while(glGetError()!=GL_NO_ERROR) {};

    glClear(GL_COLOR_BUFFER_BIT);

    err = glGetError();
    if(err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Error : 0x%X", err);
        return;
    }

    glFlush();

    err = glGetError();

    if(err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Beklenmeyen hata kodu : 0x%X", err);
        return;
    }

    err = glGetError();
    if(err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Error : 0x%X", err);
        return;
    }

    TEST_LOG_SUCCESS(test_case_3, test_procedure);
}


/* ============================================================
 * TEST 4 : Repeated Invocation
 * ============================================================
 */

/*
 * glFlush() arka arkaya çok sayıda çağrıldığında
 * kararlı çalışmalıdır.
 *
 * Her çağrı başarılı olmalı ve
 * herhangi bir OpenGL hatası üretmemelidir.
 */

void test_flush_repeatedInvocation(void)
{
    unsigned int i;
    GLenum err;

    while(glGetError()!=GL_NO_ERROR) {};

    for(i = 0; i < 10000; i++)
    {
        glFlush();

        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_4, test_procedure,
                          "Iteration : %u Error : 0x%X", i, err);
            return;
        }
    }

    TEST_LOG_SUCCESS(test_case_4, test_procedure);
}


/* ============================================================
 * TEST 5 : Command Submission Robustness
 * ============================================================
 */

/*
 * glFlush() gönderilen OpenGL komutlarını
 * GPU'ya iletmelidir.
 *
 * Bu testte framebuffer farklı renklerle
 * temizlenir ve her temizleme işleminden sonra
 * glFlush() çağrılır.
 *
 * Amaç;
 * - Komut gönderiminin güvenli şekilde
 *   gerçekleştirildiğini,
 * - Beklenmeyen OpenGL hatalarının
 *   oluşmadığını doğrulamaktır.
 */

void test_flush_commandSubmissionRobustness(void)
{
    unsigned int i;
    GLenum err;

    while(glGetError()!=GL_NO_ERROR) {};

    for(i = 0; i < 5000; i++)
    {
        GLfloat color = (GLfloat)(i % 255) / 255.0f;

        glClearColor(color,0.0f,1.0f - color,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glFlush();
        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_5, test_procedure,
                          "Iteration : %u Error : 0x%X", i, err);
            return;
        }
    }

    TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Stress Test
 * ============================================================
 */

/*
 * glFlush() 1.000.000 kez çağrılır.
 *
 * Amaç:
 *  - Driver crash
 *  - Memory corruption
 *  - Context kaybı
 *  - Beklenmeyen OpenGL hataları
 *
 * gibi robustness problemlerini ortaya çıkarmaktır.
 *
 * Her çağrı GL_NO_ERROR üretmelidir.
 */

void test_flush_stress(void)
{
    unsigned int i;
    GLenum err;

    while(glGetError()!=GL_NO_ERROR) {};

    for(i = 0; i < 1000000; i++)
    {
        glFlush();

        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_6, test_procedure,
                          "Iteration : %u Error : 0x%X", i, err);
            return;
        }
    }

    TEST_LOG_SUCCESS(test_case_6, test_procedure);
}


/* ============================================================
 * TEST 7 : Consecutive Command Submission
 * ============================================================
 */

/*
 * glFlush() ardışık komut gönderimlerinde
 * kararlı çalışmalıdır.
 *
 * Her iterasyonda yeni bir OpenGL komutu
 * gönderilir ve ardından glFlush() çağrılır.
 *
 * Amaç;
 * komut kuyruğunun sürekli kullanımında
 * sürücünün kararlı kaldığını doğrulamaktır.
 */

void test_flush_consecutiveCommandSubmission(void)
{
    unsigned int i;
    GLenum err;

    while(glGetError()!=GL_NO_ERROR) {};

    for(i = 0; i < 1000; i++)
    {
        GLfloat color = (GLfloat)(i % 100) / 100.0f;

        glClearColor(color,
                     1.0f - color,
                     0.5f,
                     1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        glFlush();

        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_7, test_procedure,
                          "Iteration : %u Error : 0x%X", i, err);
            return;
        }
    }

    while(glGetError()!=GL_NO_ERROR) {};

    TEST_LOG_SUCCESS(test_case_7, test_procedure);
}


/* ============================================================
 * Tüm glFlush Robustness Testlerini Çalıştır
 * ============================================================
 */

void Run_glFlush_Robustness(void)
{
    test_flush_basicRobustness();
    test_flush_statePreservation();
    test_flush_errorQueuePreservation();
    test_flush_repeatedInvocation();
    test_flush_commandSubmissionRobustness();
    test_flush_stress();
    test_flush_consecutiveCommandSubmission();
}