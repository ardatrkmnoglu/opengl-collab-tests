#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>

/* ============================================================
 * Test altyapısı
 *
 * resetState:
 *   OpenGL hata kuyruğunu temizler.
 *
 * checkStatePreserved:
 *   glGetError() çağrılarının OpenGL state'ini
 *   değiştirmediğini doğrular.
 * ============================================================
 */

static void resetState(void)
{
    while (glGetError() != GL_NO_ERROR);
}

static void checkStatePreserved(GLfloat expectedWidth)
{
    GLfloat width = 0.0f;

    glGetFloatv(GL_LINE_WIDTH, &width);

    if(width != expectedWidth)
    {
        printf("  [FAIL] OpenGL state bozuldu!\n");
        printf("         Beklenen : %f\n", expectedWidth);
        printf("         Gercek   : %f\n", width);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Basic Robustness
 * ============================================================
 */

/*
 * glGetError() hata kuyruğu boş olduğu zaman
 * GL_NO_ERROR döndürmelidir.
 *
 * Ayrıca arka arkaya yapılan çağrılar
 * herhangi bir hata üretmemelidir.
 */

void test_getError_basicRobustness(void)
{
    GLenum err;

    printf("TEST: Basic Robustness\n");

    resetState();

    err = glGetError();
    assert(err == GL_NO_ERROR);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Single Error Retrieval
 * ============================================================
 */

/*
 * Bilinçli olarak tek bir OpenGL hatası oluşturulur.
 *
 * İlk glGetError() doğru hata kodunu
 * döndürmelidir.
 *
 * İkinci glGetError() ise
 * GL_NO_ERROR döndürmelidir.
 */

void test_getError_singleErrorRetrieval(void)
{
    GLenum err;

    printf("TEST: Single Error Retrieval\n");

    resetState();

    glEnable((GLenum)0xFFFFFFFF);

    err = glGetError();

    if(err != GL_INVALID_ENUM)
    {
        printf("  [FAIL]\n");
        printf("Beklenen : GL_INVALID_ENUM\n");
        printf("Gercek   : 0x%X\n", err);
        assert(0);
    }

    err = glGetError();

    if(err != GL_NO_ERROR)
    {
        printf("  [FAIL]\n");
        printf("Error queue temizlenmedi.\n");
        assert(0);
    }

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Consecutive Empty Queue Reads
 * ============================================================
 */

/*
 * Error queue boşaltıldıktan sonra
 * glGetError() fonksiyonu çok sayıda
 * ardışık çağrıda sürekli GL_NO_ERROR
 * döndürmelidir.
 */

void test_getError_emptyQueueReads(void)
{
    unsigned int i;
    GLenum err;

    printf("TEST: Consecutive Empty Queue Reads\n");

    resetState();

    for(i = 0; i < 10000; i++)
    {
        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("  [FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Error     : 0x%X\n", err);
            assert(0);
        }
    }

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Multiple Error Queue
 * ============================================================
 */

/*
 * Birden fazla OpenGL hatası oluşturulur.
 *
 * glGetError() tekrar tekrar çağrılarak
 * hata kuyruğunun doğru şekilde boşaltıldığı
 * doğrulanır.
 *
 * Son çağrı mutlaka GL_NO_ERROR döndürmelidir.
 */

void test_getError_multipleErrorQueue(void)
{
    GLenum err;
    int errorCount = 0;

    printf("TEST: Multiple Error Queue\n");

    resetState();

    /* Bilinçli olarak geçersiz enumlar gönder */
    glEnable((GLenum)0xFFFFFFFF);
    glDisable((GLenum)0xFFFFFFFE);
    glEnable((GLenum)0xFFFFFFFD);

    while((err = glGetError()) != GL_NO_ERROR)
    {
        if(err != GL_INVALID_ENUM)
        {
            printf("  [FAIL]\n");
            printf("Beklenmeyen hata kodu : 0x%X\n", err);
            assert(0);
        }

        errorCount++;
    }

    if(errorCount == 0)
    {
        printf("  [FAIL]\n");
        printf("Hiç hata okunamadı.\n");
        assert(0);
    }

    err = glGetError();

    if(err != GL_NO_ERROR)
    {
        printf("  [FAIL]\n");
        printf("Error queue tamamen temizlenmedi.\n");
        assert(0);
    }

    printf("  Toplam okunan hata : %d\n", errorCount);
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: State Preservation
 * ============================================================
 */

/*
 * glGetError() yalnızca hata bayrağını okur.
 *
 * Herhangi bir OpenGL state'ini değiştirmemelidir.
 *
 * Bu testte line width değiştirilir,
 * glGetError() çok sayıda çağrılır
 * ve state'in korunup korunmadığı doğrulanır.
 */

void test_getError_statePreservation(void)
{
    GLenum err;
    unsigned int i;
    GLfloat width;

    printf("TEST: State Preservation\n");

    resetState();

    glLineWidth(3.0f);

    err = glGetError();

    if(err != GL_NO_ERROR)
    {
        printf("  [FAIL]\n");
        printf("glLineWidth hata üretti.\n");
        assert(0);
    }

    glGetFloatv(GL_LINE_WIDTH, &width);

    if(width != 3.0f)
    {
        printf("  [FAIL]\n");
        printf("Line width ayarlanamadı.\n");
        assert(0);
    }

    for(i = 0; i < 10000; i++)
    {
        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("  [FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Error     : 0x%X\n", err);
            assert(0);
        }
    }

    checkStatePreserved(3.0f);

    glGetFloatv(GL_LINE_WIDTH, &width);

    if(width != 3.0f)
    {
        printf("  [FAIL]\n");
        printf("State glGetError() çağrıları sırasında değişti.\n");
        assert(0);
    }

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 6: Random Stress Test
 * ============================================================
 */

/*
 * glGetError() fonksiyonu 1.000.000 kez çağrılır.
 *
 * Amaç:
 *  - Driver crash
 *  - Memory corruption
 *  - Context kaybı
 *  - Beklenmeyen OpenGL hataları
 * gibi robustness problemlerini ortaya çıkarmaktır.
 *
 * Hata kuyruğu boş olduğundan her çağrı
 * GL_NO_ERROR döndürmelidir.
 */

void test_getError_stress(void)
{
    unsigned int i;
    GLenum err;

    printf("TEST: Random Stress Test\n");

    resetState();

    for(i = 0; i < 1000000; i++)
    {
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
    printf("  [PASS] 1,000,000 glGetError() çağrısı başarıyla tamamlandı.\n\n");
}


/* ============================================================
 * TEST 7: Error Queue Recovery
 * ============================================================
 */

/*
 * Bu testte glGetError() fonksiyonunun
 * hata kuyruğunu tamamen temizlediği doğrulanır.
 *
 * Bir hata oluşturulur, okunur ve ardından
 * tekrar GL_NO_ERROR döndürdüğü kontrol edilir.
 */

void test_getError_errorQueueRecovery(void)
{
    GLenum err;

    printf("TEST: Error Queue Recovery\n");
    resetState();

    glEnable((GLenum)0xFFFFFFFF);
    err = glGetError();

    if(err != GL_INVALID_ENUM)
    {
        printf("  [FAIL]\n");
        printf("Beklenen : GL_INVALID_ENUM\n");
        printf("Gercek   : 0x%X\n", err);
        assert(0);
    }

    for(int i = 0; i < 100; i++)
    {
        err = glGetError();

        if(err != GL_NO_ERROR)
        {
            printf("  [FAIL]\n");
            printf("Error queue temizlenmedi.\n");
            printf("Iteration : %d\n", i);
            printf("Error     : 0x%X\n", err);
            assert(0);
        }
    }

    printf("  [PASS]\n\n");
}


/* ============================================================
 * Tüm glGetError Robustness Testlerini Çalıştır
 * ============================================================
 */

void Run_glGetError_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("     glGetError Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_getError_basicRobustness();
    test_getError_singleErrorRetrieval();
    test_getError_emptyQueueReads();
    test_getError_multipleErrorQueue();
    test_getError_statePreservation();
    test_getError_stress();
    test_getError_errorQueueRecovery();

    printf("=============================================\n");
    printf(" Tüm glGetError Robustness Testleri Başarılı\n");
    printf("=============================================\n\n");
}