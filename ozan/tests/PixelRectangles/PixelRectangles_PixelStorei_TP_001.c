#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "PixelRectangles_PixelStorei_TP_001";
static const char* test_case_1 = "PixelRectangles_PixelStorei_TC_001";
static const char* test_case_2 = "PixelRectangles_PixelStorei_TC_002";
static const char* test_case_3 = "PixelRectangles_PixelStorei_TC_003";
static const char* test_case_4 = "PixelRectangles_PixelStorei_TC_004";
static const char* test_case_5 = "PixelRectangles_PixelStorei_TC_005";
static const char* test_case_6 = "PixelRectangles_PixelStorei_TC_006";

/* ============================================================
 * TEST 1: Temel Robustness Doğrulaması
 * ============================================================ */

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

void PixelRectangles_PixelStorei_TC_001(void)
{
    GLenum err;
    int i;
    GLint validValues[]={
        1,
        2,
        4,
        8
    };

    resetState_PixelStorei();

    for(i=0;i<4;i++)
    {
        glPixelStorei(GL_PACK_ALIGNMENT,validValues[i]);
        err=glGetError();

        if(err!=GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_1, test_procedure,
                          "PACK_ALIGNMENT=%d Error=0x%X",validValues[i],err);
            return;
        }

        if(!checkIntState(test_case_1,GL_PACK_ALIGNMENT,validValues[i]))
            return;

        glPixelStorei(GL_UNPACK_ALIGNMENT,validValues[i]);
        err=glGetError();

        if(err!=GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case_1, test_procedure,
                          "UNPACK_ALIGNMENT=%d Error=0x%X",validValues[i],err);
            return;
        }

        if(!checkIntState(test_case_1,GL_UNPACK_ALIGNMENT,validValues[i]))
            return;
    }

    resetState_PixelStorei();

    TEST_LOG_SUCCESS(test_case_1, test_procedure);
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

void PixelRectangles_PixelStorei_TC_002(void)
{
    GLenum err;
    int i;
    int count;

    GLint invalidValues[]={
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

    count=sizeof(invalidValues)/sizeof(invalidValues[0]);

    resetState_PixelStorei();

    glPixelStorei(GL_PACK_ALIGNMENT,4);

    for(i=0;i<count;i++)
    {
        glPixelStorei(GL_PACK_ALIGNMENT,invalidValues[i]);
        err=glGetError();

        if(err!=GL_INVALID_VALUE)
        {
            TEST_LOG_FAIL(test_case_2, test_procedure,
                          "Alignment=%d Beklenen=GL_INVALID_VALUE Gelen=0x%X",
                          invalidValues[i],err);
            return;
        }

        if(!checkIntState(test_case_2,GL_PACK_ALIGNMENT,4))
            return;
    }

    resetState_PixelStorei();

    TEST_LOG_SUCCESS(test_case_2, test_procedure);
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

void PixelRectangles_PixelStorei_TC_003(void)
{
    GLenum err;
    GLenum pname;

    resetState_PixelStorei();

    /* Bilinen geçerli durum */
    glPixelStorei(GL_PACK_ALIGNMENT,4);

    if(!checkIntState(test_case_3,GL_PACK_ALIGNMENT,4))
        return;

    /* Çok sayıda geçersiz enum değeri dene */
    for(pname=0;pname<10000;pname++)
    {
        /* Geçerli pname'ları atla */
        if(pname==GL_PACK_ALIGNMENT)
            continue;
        if(pname==GL_UNPACK_ALIGNMENT)
            continue;

        glPixelStorei(pname,4);
        err=glGetError();

        if(err==GL_INVALID_ENUM)
        {
            /* Beklenen durum */
            if(!checkIntState(test_case_3,GL_PACK_ALIGNMENT,4))
                return;
        }
        else if(err==GL_NO_ERROR)
        {
            /*
             * Bazı implementasyonlar extension enumlarını
             * kabul edebilir. Bu durumda state'in yine de
             * bozulmadığını doğrula.
             */
            if(!checkIntState(test_case_3,GL_PACK_ALIGNMENT,4))
                return;
        }
        else
        {
            TEST_LOG_FAIL(test_case_3, test_procedure,
                          "pname=0x%X Beklenmeyen Error=0x%X",pname,err);
            return;
        }
    }

    resetState_PixelStorei();

    TEST_LOG_SUCCESS(test_case_3, test_procedure);
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

void PixelRectangles_PixelStorei_TC_004(void)
{
    GLenum err;
    GLint value;

    resetState_PixelStorei();

    /*--------------------------------------------*/
    /* PACK_ALIGNMENT                             */
    /*--------------------------------------------*/

    glPixelStorei(GL_PACK_ALIGNMENT,8);
    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "PACK_ALIGNMENT=8 Error=0x%X",err);
        return;
    }

    glGetIntegerv(GL_PACK_ALIGNMENT,&value);

    if(value!=8)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "PACK_ALIGNMENT beklenen=8 gercek=%d",value);
        return;
    }

    glPixelStorei(GL_PACK_ALIGNMENT,3);

    err=glGetError();

    if(err!=GL_INVALID_VALUE)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Beklenen=GL_INVALID_VALUE Gelen=0x%X",err);
        return;
    }

    glGetIntegerv(GL_PACK_ALIGNMENT,&value);

    if(value!=8)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "PACK_ALIGNMENT durumu bozuldu. Gercek=%d",value);
        return;
    }

    /*--------------------------------------------*/
    /* UNPACK_ALIGNMENT                           */
    /*--------------------------------------------*/

    glPixelStorei(GL_UNPACK_ALIGNMENT,2);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "UNPACK_ALIGNMENT=2 Error=0x%X",err);
        return;
    }

    glGetIntegerv(GL_UNPACK_ALIGNMENT,&value);

    if(value!=2)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "UNPACK_ALIGNMENT beklenen=2 gercek=%d",value);
        return;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT,-5);

    err=glGetError();

    if(err!=GL_INVALID_VALUE)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Beklenen=GL_INVALID_VALUE Gelen=0x%X",err);
        return;
    }

    glGetIntegerv(GL_UNPACK_ALIGNMENT,&value);

    if(value!=2)
    {
        TEST_LOG_FAIL(test_case_4, test_procedure, "UNPACK_ALIGNMENT durumu bozuldu. Gercek=%d",value);
        return;
    }

    resetState_PixelStorei();

    TEST_LOG_SUCCESS(test_case_4, test_procedure);
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

void PixelRectangles_PixelStorei_TC_005(void)
{
    GLenum err;

    resetState_PixelStorei();

    /* Geçerli çağrı */
    glPixelStorei(GL_PACK_ALIGNMENT,4);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Beklenen=GL_NO_ERROR Gelen=0x%X",err);
        return;
    }

    /* Geçersiz alignment */
    glPixelStorei(GL_PACK_ALIGNMENT,3);

    err=glGetError();

    if(err!=GL_INVALID_VALUE)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Beklenen=GL_INVALID_VALUE Gelen=0x%X",err);
        return;
    }

    /* Geçersiz pname */
    glPixelStorei(GL_TEXTURE_2D,4);

    err=glGetError();

    if(err!=GL_INVALID_ENUM)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Beklenen=GL_INVALID_ENUM Gelen=0x%X",err);
        return;
    }

    /* Tekrar geçerli çağrı */
    glPixelStorei(GL_UNPACK_ALIGNMENT,8);

    err=glGetError();

    if(err!=GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Beklenen=GL_NO_ERROR Gelen=0x%X",err);
        return;
    }

    if(!checkIntState(test_case_5,GL_UNPACK_ALIGNMENT,8))
        return;

    resetState_PixelStorei();

    TEST_LOG_SUCCESS(test_case_5, test_procedure);
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

void PixelRectangles_PixelStorei_TC_006(void)
{
    unsigned int i;

    GLenum validPnames[]={
        GL_PACK_ALIGNMENT,
        GL_UNPACK_ALIGNMENT
    };

    resetState_PixelStorei();

    srand(12345);

    for(i=0;i<1000000;i++)
    {
        GLenum pname;
        GLint value;
        GLenum err;

        if(rand()%2)
            pname=validPnames[rand()%2];
        else
            pname=(GLenum)rand();

        value=(rand()%200)-100;

        glPixelStorei(pname,value);

        err=glGetError();

        if(err!=GL_NO_ERROR &&
           err!=GL_INVALID_ENUM &&
           err!=GL_INVALID_VALUE)
        {
            TEST_LOG_FAIL(test_case_6, test_procedure,
                          "Iteration : %u pname : 0x%X value : %d Error : 0x%X",
                          i,pname,value,err);
            return;
        }
    }

    resetState_PixelStorei();

    TEST_LOG_INFO("1,000,000 rastgele test basariyla tamamlandi.");
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * Tüm glPixelStorei Robustness Testlerini Çalıştır
 * ============================================================
 */

void Run_glPixelStorei_Robustness(void)
{
    PixelRectangles_PixelStorei_TC_001();
    PixelRectangles_PixelStorei_TC_002();
    PixelRectangles_PixelStorei_TC_003();
    PixelRectangles_PixelStorei_TC_004();
    PixelRectangles_PixelStorei_TC_005();
    PixelRectangles_PixelStorei_TC_006();
}