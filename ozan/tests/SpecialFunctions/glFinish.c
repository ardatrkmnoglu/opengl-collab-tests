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
 *   glFinish() çağrısından sonra OpenGL state'inin
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

    if(viewport[0] != x || viewport[1] != y || viewport[2] != width || viewport[3] != height)
    {
        printf("  [FAIL] Viewport state bozuldu!\n");
        printf("         Beklenen : (%d, %d, %d, %d)\n", x, y, width, height);
        printf("         Gercek   : (%d, %d, %d, %d)\n", viewport[0], viewport[1], viewport[2], viewport[3]);
        assert(0);
    }
}


/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 */

/*
 * glFinish() daha önce gönderilmiş bütün
 * OpenGL komutlarının tamamlanmasını beklemelidir.
 *
 * Bu testte basit OpenGL komutları çalıştırılır.
 * Ardından glFinish() çağrılır.
 *
 * Beklenen:
 *
 *  - GL_NO_ERROR
 *  - Driver crash olmamalıdır.
 *  - Context kaybı yaşanmamalıdır.
 */

void test_finish_basicRobustness(void)
{
    GLenum err;

    printf("TEST: Basic Robustness\n");

    resetState();

    glClearColor(1.0f,0.0f,0.0f,1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glFinish();

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
 * glFinish() yalnızca GPU komutlarının
 * tamamlanmasını bekler.
 *
 * OpenGL state'ini değiştirmemelidir.
 *
 * Viewport ayarlanır.
 * glFinish() çağrılır.
 *
 * Viewport'un aynı kaldığı doğrulanır.
 */

void test_finish_statePreservation(void)
{
    GLenum err;

    printf("TEST: State Preservation\n");

    resetState();

    glViewport(10,20,320,240);

    err = glGetError();

    assert(err == GL_NO_ERROR);

    glFinish();

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
 * glFinish() hata kuyruğunu değiştirmemelidir.
 *
 * Geçerli OpenGL komutlarından sonra çağrıldığında
 * GL_NO_ERROR üretmeli ve hata kuyruğunu
 * bozmamalıdır.
 */

void test_finish_errorQueuePreservation(void)
{
    GLenum err;

    printf("TEST: Error Queue Preservation\n");

    resetState();

    glClear(GL_COLOR_BUFFER_BIT);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    glFinish();

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
 * glFinish() arka arkaya çok sayıda çağrıldığında
 * kararlı çalışmalıdır.
 *
 * Her çağrı başarılı olmalı ve
 * herhangi bir OpenGL hatası üretmemelidir.
 */

void test_finish_repeatedInvocation(void)
{
    unsigned int i;
    GLenum err;

    printf("TEST: Repeated Invocation\n");

    resetState();

    for(i = 0; i < 10000; i++)
    {
        glFinish();

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
 * TEST 5 : Synchronization Robustness
 * ============================================================
 */

/*
 * glFinish() daha önce gönderilmiş OpenGL
 * komutlarının tamamlanmasını beklemelidir.
 *
 * Bu testte her iterasyonda framebuffer
 * farklı bir renkle temizlenir ve
 * glFinish() çağrılır.
 *
 * Amaç;
 * - Driver'ın senkronizasyon sırasında
 *   hata üretmediğini,
 * - Komutların güvenli şekilde
 *   tamamlandığını doğrulamaktır.
 */

void test_finish_synchronizationRobustness(void)
{
    unsigned int i;
    GLenum err;

    printf("TEST: Synchronization Robustness\n");

    resetState();

    for(i = 0; i < 5000; i++)
    {
        GLfloat color = (GLfloat)(i % 255) / 255.0f;

        glClearColor(color,0.0f,1.0f - color,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glFinish();

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
 * glFinish() 1.000.000 kez çağrılır.
 *
 * Amaç:
 *  - Driver crash
 *  - Memory corruption
 *  - Context kaybı
 *  - Beklenmeyen OpenGL hataları
 * gibi robustness problemlerini ortaya çıkarmaktır.
 *
 * Her çağrı GL_NO_ERROR üretmelidir.
 */

void test_finish_stress(void)
{
    unsigned int i;
    GLenum err;

    printf("TEST: Stress Test\n");

    resetState();

    for(i = 0; i < 1000000; i++)
    {
        glFinish();

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
    printf("  [PASS] 1,000,000 glFinish() çağrısı başarıyla tamamlandı.\n\n");
}


/* ============================================================
 * TEST 7 : Consecutive Render Synchronization
 * ============================================================
 */
/*
 * glFinish() ardışık render işlemlerinden sonra
 * güvenilir şekilde çalışmalıdır.
 *
 * Framebuffer farklı renklerle temizlenir,
 * her temizleme işleminden sonra glFinish()
 * çağrılır.
 *
 * Her iterasyonda GL_NO_ERROR beklenmektedir.
 */

void test_finish_consecutiveRenderSynchronization(void)
{
    unsigned int i;
    GLenum err;

    printf("TEST: Consecutive Render Synchronization\n");

    resetState();

    for(i = 0; i < 1000; i++)
    {
        GLfloat color = (GLfloat)(i % 100) / 100.0f;
        glClearColor(color,1.0f - color,color * 0.5f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glFinish();

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
 * Tüm glFinish Robustness Testlerini Çalıştır
 * ============================================================
 */

void Run_glFinish_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("       glFinish Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_finish_basicRobustness();
    test_finish_statePreservation();
    test_finish_errorQueuePreservation();
    test_finish_repeatedInvocation();
    test_finish_synchronizationRobustness();
    test_finish_stress();
    test_finish_consecutiveRenderSynchronization();

    printf("=============================================\n");
    printf(" Tüm glFinish Robustness Testleri Başarılı\n");
    printf("=============================================\n\n");
}