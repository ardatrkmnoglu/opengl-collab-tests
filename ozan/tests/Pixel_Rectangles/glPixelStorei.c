#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>

/* ============================================================
 * Test altyapısı
 *
 * resetState:
 *   Her testten önce GL_PACK_ALIGNMENT ve
 *   GL_UNPACK_ALIGNMENT varsayılan değerleri olan
 *   4'e ayarlanır ve OpenGL hata kuyruğu temizlenir.
 *
 * checkStatePreserved:
 *   Geçersiz bir glPixelStorei() çağrısından sonra
 *   OpenGL state'inin değişmediğini doğrular.
 * ============================================================ */

static void resetState(void)
{
    glPixelStorei(GL_PACK_ALIGNMENT,4);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    while(glGetError()!=GL_NO_ERROR);
}

static void checkStatePreserved(GLenum pname, GLint expectedValue)
{
    GLint value;
    glGetIntegerv(pname,&value);
    if(value!=expectedValue)
    {
        printf("  [FAIL] PixelStore durumu bozuldu!\n");
        printf("         Beklenen : %d\n",expectedValue);
        printf("         Gercek   : %d\n",value);
        assert(0);
    }
}

/* ============================================================
 * TEST 1: Temel Robustness Doğrulaması
 * ============================================================
 */

/*
 * glPixelStorei() fonksiyonunun desteklediği tüm
 * geçerli hizalama (alignment) değerleri test edilir.
 *
 * OpenGL spesifikasyonuna göre yalnızca
 *
 *      1
 *      2
 *      4
 *      8
 *
 * değerleri geçerlidir.
 *
 * Hem GL_PACK_ALIGNMENT hem de
 * GL_UNPACK_ALIGNMENT için bu değerler
 * denenerek herhangi bir OpenGL hatası
 * oluşmadığı doğrulanır.
 */

void test_pixelStore_basicRobustness(void)
{
    GLenum err;
    GLint validValues[] =
    {
        1,
        2,
        4,
        8
    };

    printf("TEST: Basic Robustness\n");
    resetState();
    for(int i=0;i<4;i++)
    {
        glPixelStorei(GL_PACK_ALIGNMENT, validValues[i]);
        err=glGetError();
        if(err!=GL_NO_ERROR)
        {
            printf("  [FAIL] PACK_ALIGNMENT=%d Error=0x%X\n", validValues[i], err);
            assert(0);
        }

        checkStatePreserved(GL_PACK_ALIGNMENT, validValues[i]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, validValues[i]);
        err=glGetError();
        if(err!=GL_NO_ERROR)
        {
            printf("  [FAIL] UNPACK_ALIGNMENT=%d Error=0x%X\n", validValues[i], err);
            assert(0);
        }

        checkStatePreserved(GL_UNPACK_ALIGNMENT, validValues[i]);
    }
    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 2: Geçersiz Alignment Değerleri
 * ============================================================
 */

/*
 * OpenGL spesifikasyonuna göre alignment
 * yalnızca
 *
 *      1
 *      2
 *      4
 *      8
 *
 * olabilir.
 *
 * Bunun dışındaki tüm değerler
 * GL_INVALID_VALUE üretmelidir.
 *
 * Ayrıca geçersiz çağrıdan sonra mevcut
 * PixelStore durumunun değişmediği de
 * doğrulanır.
 */

void test_pixelStore_invalidAlignment(void)
{
    GLenum err;

    GLint invalidValues[] =
    {
        0,
        3,
        5,
        6,
        7,
        9,
        -1,
        -2,
        10,
        100,
        INT_MAX,
        INT_MIN
    };

    printf("TEST: Invalid Alignment Values\n");
    resetState();
    glPixelStorei(GL_PACK_ALIGNMENT,4);

    for(int i=0;
        i<sizeof(invalidValues)/sizeof(invalidValues[0]);
        i++)
    {
        glPixelStorei(GL_PACK_ALIGNMENT, invalidValues[i]);
        err=glGetError();

        if(err!=GL_INVALID_VALUE)
        {
            printf("  [FAIL] Alignment=%d Beklenen=GL_INVALID_VALUE Gelen=0x%X\n", invalidValues[i], err);
            assert(0);
        }

        checkStatePreserved(GL_PACK_ALIGNMENT, 4);
    }
    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Geçersiz pname Değerleri
 * ============================================================
 */

/*
 * glPixelStorei() fonksiyonu yalnızca
 *
 *      GL_PACK_ALIGNMENT
 *      GL_UNPACK_ALIGNMENT
 *
 * parametrelerini kabul eder.
 *
 * Bunun dışındaki GLenum değerlerinin
 * GL_INVALID_ENUM üretmesi beklenir.
 *
 * Ayrıca geçersiz çağrı sonrasında mevcut
 * PixelStore durumunun değişmediği de
 * doğrulanmaktadır.
 */

void test_pixelStore_invalidPname(void)
{
    GLenum err;
    GLenum pname;
    printf("TEST: Invalid pname Values\n");
    resetState();

    /* Bilinen geçerli durum */
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    checkStatePreserved(GL_PACK_ALIGNMENT, 4);

    /* Çok sayıda geçersiz enum değeri dene */
    for (pname = 0; pname < 10000; pname++)
    {
        /* Geçerli pname'ları atla */
        if (pname == GL_PACK_ALIGNMENT)
            continue;
        if (pname == GL_UNPACK_ALIGNMENT)
            continue;

        glPixelStorei(pname, 4);
        err = glGetError();

        if (err == GL_INVALID_ENUM)
        {
            /* Beklenen durum */
            checkStatePreserved(GL_PACK_ALIGNMENT, 4);
        }
        else if (err == GL_NO_ERROR)
        {
            /*
             * Bazı implementasyonlar extension enumlarını
             * kabul edebilir. Bu durumda state'in yine de
             * bozulmadığını doğrula.
             */
            checkStatePreserved(GL_PACK_ALIGNMENT, 4);
        }
        else
        {
            printf("  [FAIL] pname=0x%X Beklenmeyen Error=0x%X\n", pname, err);
            assert(0);
        }
    }
    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 4: State Preservation
 * ============================================================
 */

/*
 * Geçerli ve geçersiz glPixelStorei()
 * çağrılarından sonra OpenGL state'inin
 * doğru korunup korunmadığı doğrulanır.
 *
 * Test boyunca hem PACK_ALIGNMENT hem de
 * UNPACK_ALIGNMENT değerleri kontrol edilir.
 *
 * Geçersiz çağrıların mevcut durumu
 * değiştirmemesi beklenmektedir.
 */

void test_pixelStore_statePreservation(void)
{
    GLenum err;
    GLint value;
    printf("TEST: State Preservation\n");
    resetState();

    /*--------------------------------------------*/
    /* PACK_ALIGNMENT                             */
    /*--------------------------------------------*/

    glPixelStorei(GL_PACK_ALIGNMENT,8);
    err=glGetError();

    assert(err==GL_NO_ERROR);

    glGetIntegerv(GL_PACK_ALIGNMENT, &value);

    assert(value==8);

    glPixelStorei(GL_PACK_ALIGNMENT,3);

    err=glGetError();

    assert(err==GL_INVALID_VALUE);

    glGetIntegerv(GL_PACK_ALIGNMENT, &value);

    assert(value==8);

    /*--------------------------------------------*/
    /* UNPACK_ALIGNMENT                           */
    /*--------------------------------------------*/

    glPixelStorei(GL_UNPACK_ALIGNMENT,2);

    err=glGetError();

    assert(err==GL_NO_ERROR);

    glGetIntegerv(GL_UNPACK_ALIGNMENT, &value);

    assert(value==2);

    glPixelStorei(GL_UNPACK_ALIGNMENT,-5);

    err=glGetError();

    assert(err==GL_INVALID_VALUE);

    glGetIntegerv(GL_UNPACK_ALIGNMENT, &value);

    assert(value==2);

    resetState();

    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Error Queue ve State Preservation
 * ============================================================
 */

/*
 * Bu testte glPixelStorei() fonksiyonunun hata kuyruğunu
 * doğru yönettiği doğrulanmaktadır.
 *
 * Amaç;
 *
 *  - GL_INVALID_VALUE
 *  - GL_INVALID_ENUM
 *  - GL_NO_ERROR
 *
 * durumlarının arka arkaya oluştuğu senaryolarda
 * hata kuyruğunun doğru çalıştığını doğrulamaktır.
 */

void test_pixelStore_errorQueue(void)
{
    GLenum err;
    printf("TEST: Error Queue and State Preservation\n");
    resetState();

    /* Geçerli çağrı */
    glPixelStorei(GL_PACK_ALIGNMENT,4);

    err = glGetError();
    assert(err == GL_NO_ERROR);

    /* Geçersiz alignment */

    glPixelStorei(GL_PACK_ALIGNMENT,3);

    err = glGetError();

    assert(err == GL_INVALID_VALUE);

    /* Geçersiz pname */

    glPixelStorei(GL_TEXTURE_2D,4);

    err = glGetError();

    assert(err == GL_INVALID_ENUM);

    /* Tekrar geçerli çağrı */

    glPixelStorei(GL_UNPACK_ALIGNMENT,8);

    err = glGetError();

    assert(err == GL_NO_ERROR);

    checkStatePreserved(GL_UNPACK_ALIGNMENT, 8);

    resetState();

    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 6: Rastgele Stress / Fuzz Testi
 * ============================================================
 */

/*
 * glPixelStorei() fonksiyonu rastgele pname ve
 * alignment değerleri ile 1.000.000 kez çağrılır.
 *
 * Amaç;
 *
 *  - Driver crash
 *  - Context kaybı
 *  - Bellek bozulması
 *  - Beklenmeyen OpenGL hataları
 *
 * gibi robustness problemlerini ortaya çıkarmaktır.
 *
 * Beklenen hatalar:
 *
 *      GL_NO_ERROR
 *      GL_INVALID_ENUM
 *      GL_INVALID_VALUE
 *
 * Bunların dışındaki herhangi bir hata FAIL kabul edilir.
 */

void test_pixelStore_stress(void)
{
    unsigned int i;
    printf("TEST: Random Stress Test\n");

    resetState();

    srand(12345);

    GLenum validPnames[] =
    {
        GL_PACK_ALIGNMENT,
        GL_UNPACK_ALIGNMENT
    };

    for(i=0;i<1000000;i++)
    {
        GLenum pname;

        if(rand()%2)
            pname = validPnames[rand()%2];
        else
            pname = (GLenum)rand();
        GLint value =
            (rand()%200)-100;
        glPixelStorei(pname,value);

        GLenum err = glGetError();

        if(err != GL_NO_ERROR &&
           err != GL_INVALID_ENUM &&
           err != GL_INVALID_VALUE)
        {
            printf("\n[FAIL]\n");
            printf("Iteration : %u\n",i);
            printf("pname     : 0x%X\n",pname);
            printf("value     : %d\n",value);
            printf("Error     : 0x%X\n",err);

            assert(0);
        }
    }

    resetState();
    printf("  [PASS] 1,000,000 rastgele test başarıyla tamamlandı.\n\n");
}


/* ============================================================
 * Tüm glPixelStorei Robustness Testlerini Çalıştır
 * ============================================================
 */

void Run_glPixelStorei_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("   glPixelStorei Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_pixelStore_basicRobustness();
    test_pixelStore_invalidAlignment();
    test_pixelStore_invalidPname();
    test_pixelStore_statePreservation();
    test_pixelStore_errorQueue();
    test_pixelStore_stress();

    printf("=============================================\n");
    printf(" Tüm glPixelStorei Robustness Testleri Başarılı\n");
    printf("=============================================\n\n");
}