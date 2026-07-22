#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>

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

static void resetState(void)
{
    while (glGetError() != GL_NO_ERROR);
}

static void checkViewportPreserved(GLint x,GLint y,GLsizei width,GLsizei height)
{
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);

    if(viewport[0] != x ||
       viewport[1] != y ||
       viewport[2] != width ||
       viewport[3] != height)
    {
        printf("  [FAIL] Viewport state bozuldu!\n");
        printf("         Beklenen : (%d, %d, %d, %d)\n",x, y, width, height);
        printf("         Gercek   : (%d, %d, %d, %d)\n",viewport[0],viewport[1],viewport[2],viewport[3]);
        assert(0);
    }
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

    printf("TEST: Basic Robustness\n");

    resetState();

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glFlush();

    err = glGetError();

    if(err != GL_NO_ERROR)
    {
        printf("  [FAIL]\n");
        printf("Error : 0x%X\n", err);
        assert(0);
    }

    resetState();

    printf("  [PASS]\n\n");
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

    printf("TEST: State Preservation\n");

    resetState();

    glViewport(10,20,320,240);
    err = glGetError();
    assert(err == GL_NO_ERROR);

    glFlush();

    err = glGetError();
    assert(err == GL_NO_ERROR);

    checkViewportPreserved(10,20,320,240);

    resetState();
    printf("  [PASS]\n\n");
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

    printf("TEST: Error Queue Preservation\n");

    resetState();

    glClear(GL_COLOR_BUFFER_BIT);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    glFlush();

    err = glGetError();

    if(err != GL_NO_ERROR)
    {
        printf("  [FAIL]\n");
        printf("Beklenmeyen hata kodu : 0x%X\n", err);
        assert(0);
    }

    err = glGetError();
    assert(err == GL_NO_ERROR);

    resetState();

    printf("  [PASS]\n\n");
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

    printf("TEST: Repeated Invocation\n");

    resetState();

    for(i = 0; i < 10000; i++)
    {
        glFlush();

        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("  [FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Error     : 0x%X\n", err);
            assert(0);
        }
    }

    resetState();

    printf("  [PASS]\n\n");
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

    printf("TEST: Command Submission Robustness\n");

    resetState();

    for(i = 0; i < 5000; i++)
    {
        GLfloat color = (GLfloat)(i % 255) / 255.0f;

        glClearColor(color,0.0f,1.0f - color,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glFlush();
        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("  [FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Error     : 0x%X\n", err);
            assert(0);
        }
    }

    resetState();

    printf("  [PASS]\n\n");
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

    printf("TEST: Stress Test\n");

    resetState();

    for(i = 0; i < 1000000; i++)
    {
        glFlush();

        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("\n[FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Error     : 0x%X\n", err);
            assert(0);
        }
    }

    resetState();
    printf("  [PASS] 1,000,000 glFlush() çağrısı başarıyla tamamlandı.\n\n");
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

    printf("TEST: Consecutive Command Submission\n");
    resetState();

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
            printf("  [FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Error     : 0x%X\n", err);
            assert(0);
        }
    }

    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * Tüm glFlush Robustness Testlerini Çalıştır
 * ============================================================
 */

void Run_glFlush_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("       glFlush Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_flush_basicRobustness();
    test_flush_statePreservation();
    test_flush_errorQueuePreservation();
    test_flush_repeatedInvocation();
    test_flush_commandSubmissionRobustness();
    test_flush_stress();
    test_flush_consecutiveCommandSubmission();

    printf("=============================================\n");
    printf(" Tüm glFlush Robustness Testleri Başarılı\n");
    printf("=============================================\n\n");
}