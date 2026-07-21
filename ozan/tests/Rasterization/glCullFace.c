#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* ============================================================
 * Test altyapısı
 *
 * resetState:
 * Her test öncesinde OpenGL durumu varsayılan haline getirilir.
 * Culling kapatılır, FrontFace GL_CCW yapılır ve CullFace
 * modu GL_BACK olarak ayarlanır. Ardından hata kuyruğu temizlenir.
 *
 * checkStatePreserved:
 * Geçersiz çağrılar sonrasında GL_CULL_FACE_MODE değerinin
 * değişmediğini doğrular.
 * ============================================================ */

static void resetState(void)
{
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    while(glGetError()!=GL_NO_ERROR);
}

static void checkStatePreserved(GLint expected)
{
    GLint actual;
    glGetIntegerv(GL_CULL_FACE_MODE,
                  &actual);

    if(actual!=expected)
    {
        printf("  [FAIL] State bozuldu!\n");
        printf("         Beklenen : 0x%X\n",expected);
        printf("         Gercek   : 0x%X\n",actual);

        assert(0);
    }
}


/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 *
 * Amaç
 * ----
 * glCullFace() fonksiyonunun kabul ettiği üç geçerli
 * enum değeri doğrulanır.
 *
 * Ardından geçersiz enum değerleri gönderilerek
 * GL_INVALID_ENUM üretildiği ve mevcut state'in
 * değişmediği kontrol edilir.
 * ============================================================ */

void test_cullFace_basicRobustness(void)
{
    GLenum err;
    printf("TEST : Basic Robustness\n");
    resetState();
    glCullFace(GL_BACK);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_BACK);

    glCullFace(GL_FRONT);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_FRONT);

    glCullFace(GL_FRONT_AND_BACK);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_FRONT_AND_BACK);

    glCullFace((GLenum)0x0BAD);
    err=glGetError();

    assert(err==GL_INVALID_ENUM);
    checkStatePreserved(GL_FRONT_AND_BACK);
    glCullFace(GL_CCW);
    err=glGetError();

    assert(err==GL_INVALID_ENUM);
    checkStatePreserved(GL_FRONT_AND_BACK);
    resetState();

    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 2 : Stress Sweep
 * ============================================================
 *
 * Amaç
 * ----
 * 16-bit GLenum uzayındaki tüm değerler sistematik
 * olarak denenir.
 *
 * Yalnızca
 *
 *      GL_BACK
 *      GL_FRONT
 *      GL_FRONT_AND_BACK
 *
 * değerlerinin kabul edilmesi beklenmektedir.
 *
 * Geçersiz değerlerde state'in korunup korunmadığı
 * da doğrulanmaktadır.
 * ============================================================ */

void test_cullFace_stressSweep(void)
{
    GLenum mode;
    int passCount=0;
    int failCount=0;
    printf("TEST : Stress Sweep\n");
    resetState();

    for(mode=0; mode<65536; mode++)
    {
        GLenum expected=
            (mode==GL_BACK ||
             mode==GL_FRONT ||
             mode==GL_FRONT_AND_BACK)
            ?
            GL_NO_ERROR
            :
            GL_INVALID_ENUM;

        GLenum err;
        glCullFace(mode);
        err=glGetError();
        if(err!=expected)
        {
            printf("  [FAIL] Enum=0x%X Beklenen=0x%X Gelen=0x%X\n",
                   mode,
                   expected,
                   err);

            failCount++;
        }
        else
        {
            passCount++;
        }

        if(err==GL_INVALID_ENUM)
        {
            checkStatePreserved(GL_BACK);
        }
    }

    printf("  PASS : %d\n",passCount);
    printf("  FAIL : %d\n",failCount);

    assert(failCount==0);
    resetState();

    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 3 : Error Queue Management
 * ============================================================
 *
 * Amaç
 * ----
 * Arka arkaya çok sayıda geçersiz enum gönderildiğinde
 * hata kuyruğunun bozulmadığı doğrulanır.
 *
 * Daha sonra geçerli bir çağrı yapılarak sürücünün
 * normal çalışmaya döndüğü kontrol edilir.
 * ============================================================ */

void test_cullFace_errorQueue(void)
{
    GLenum err;
    int i;
    int errorCount=0;
    printf("TEST : Error Queue Management\n");
    resetState();
    for(i=0;i<100;i++)
    {
        glCullFace((GLenum)(0x5000+i));
    }

    while((err=glGetError())!=GL_NO_ERROR)
    {
        assert(err==GL_INVALID_ENUM);

        errorCount++;
    }
    assert(errorCount>0);
    glCullFace(GL_FRONT);

    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_FRONT);
    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4 : Rapid Toggle
 * ============================================================
 *
 * Amaç
 * ----
 * GL_BACK, GL_FRONT ve GL_FRONT_AND_BACK arasında
 * yüz binlerce kez geçiş yapılarak OpenGL durum
 * makinesinin kararlılığı doğrulanır.
 *
 * Her çağrıdan sonra GL_CULL_FACE_MODE sorgulanır.
 * ============================================================ */

void test_cullFace_rapidToggle(void)
{
    const int repeat = 100000;
    int i;
    printf("TEST : Rapid Toggle\n");

    resetState();
    for(i=0;i<repeat;i++)
    {
        GLenum expected;
        GLint current;

        switch(i%3)
        {
            case 0:
                expected=GL_BACK;
                break;

            case 1:
                expected=GL_FRONT;
                break;

            default:
                expected=GL_FRONT_AND_BACK;
                break;
        }

        glCullFace(expected);
        assert(glGetError()==GL_NO_ERROR);
        glGetIntegerv(GL_CULL_FACE_MODE,
                      &current);
        assert(current==expected);
    }

    resetState();

    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 5 : State Preservation
 * ============================================================
 *
 * Amaç
 * ----
 * Geçersiz glCullFace() çağrılarının mevcut
 * GL_CULL_FACE_MODE değerini değiştirmediği
 * doğrulanmaktadır.
 *
 * Önce geçerli bir durum oluşturulur.
 * Daha sonra farklı geçersiz enum değerleri
 * gönderilir.
 * ============================================================ */

void test_cullFace_statePreservation(void)
{
    GLenum err;
    GLenum invalidEnums[] =
    {
        0,
        1,
        2,
        1234,
        9999,
        0xFFFF,
        0xFFFFFFFF
    };

    int i;
    printf("TEST : State Preservation\n");
    resetState();
    glCullFace(GL_FRONT);
    assert(glGetError()==GL_NO_ERROR);

    checkStatePreserved(GL_FRONT);
    for(i=0;
        i<sizeof(invalidEnums)/sizeof(invalidEnums[0]);
        i++)
    {
        glCullFace(invalidEnums[i]);

        err=glGetError();

        assert(err==GL_INVALID_ENUM);

        checkStatePreserved(GL_FRONT);
    }
    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 6 : FrontFace Combination
 * ============================================================
 *
 * Amaç
 * ----
 * glFrontFace() ile glCullFace() fonksiyonlarının
 * birlikte kullanıldığında birbirlerinin durumunu
 * bozmadığı doğrulanmaktadır.
 *
 * Tüm geçerli kombinasyonlar denenmektedir.
 * ============================================================ */

void test_cullFace_frontFaceCombination(void)
{
    GLenum frontModes[] =
    {
        GL_CCW,
        GL_CW
    };

    GLenum cullModes[] =
    {
        GL_BACK,
        GL_FRONT,
        GL_FRONT_AND_BACK
    };

    int i;
    int j;

    printf("TEST : FrontFace Combination\n");
    resetState();
    for(i=0;i<2;i++)
    {
        for(j=0;j<3;j++)
        {
            GLint front;
            GLint cull;
            glFrontFace(frontModes[i]);
            assert(glGetError()==GL_NO_ERROR);
            glCullFace(cullModes[j]);

            assert(glGetError()==GL_NO_ERROR);
            glGetIntegerv(GL_FRONT_FACE,
                          &front);
            glGetIntegerv(GL_CULL_FACE_MODE,
                          &cull);
            assert(front==frontModes[i]);
            assert(cull==cullModes[j]);
        }
    }
    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 7 : Large Invalid Enum Values
 * ============================================================
 *
 * Amaç
 * ----
 * Çok büyük GLenum değerleri gönderildiğinde
 * sürücünün çökmediği ve yalnızca
 * GL_INVALID_ENUM ürettiği doğrulanır.
 *
 * Ayrıca başarısız çağrıların mevcut durumu
 * değiştirmediği kontrol edilir.
 * ============================================================ */

void test_cullFace_largeEnum(void)
{
    GLenum values[] =
    {
        (GLenum)0x10000,
        (GLenum)0x7FFFFFFF,
        (GLenum)0x80000000,
        (GLenum)0xFFFFFFFF
    };

    int i;
    printf("TEST : Large Invalid Enum Values\n");
    resetState();
    for(i=0;
        i<sizeof(values)/sizeof(values[0]);
        i++)
    {
        GLenum err;
        glCullFace(values[i]);
        err=glGetError();

        assert(err==GL_INVALID_ENUM);
        checkStatePreserved(GL_BACK);
    }

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 8 : Rapid Fire
 * ============================================================
 *
 * Amaç
 * ----
 * glCullFace() fonksiyonunu çok kısa aralıklarla
 * art arda çağırarak sürücünün yoğun kullanım altında
 * kararlılığını doğrular.
 *
 * Test sonunda herhangi bir OpenGL hatası oluşmamalı
 * ve son durum doğru şekilde korunmalıdır.
 * ============================================================ */

void test_cullFace_rapidFire(void)
{
    const unsigned int repeat = 1000000;
    unsigned int i;
    printf("TEST : Rapid Fire\n");
    resetState();
    for(i = 0; i < repeat; i++)
    {
        glCullFace(GL_BACK);
        glCullFace(GL_FRONT);
        glCullFace(GL_FRONT_AND_BACK);
    }
    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(GL_FRONT_AND_BACK);
    resetState();

    printf("  [PASS] %u çağrı tamamlandı.\n\n", repeat * 3);
}


/* ============================================================
 * TEST 9 : Random Fuzz Test
 * ============================================================
 *
 * Amaç
 * ----
 * Rastgele GLenum değerleri gönderilerek sürücünün
 * beklenmeyen girdiler karşısındaki dayanıklılığı
 * test edilir.
 *
 * Beklenen sonuçlar:
 *
 *      GL_NO_ERROR
 *      GL_INVALID_ENUM
 *
 * Bunların dışındaki herhangi bir hata başarısızlık
 * olarak değerlendirilir.
 * ============================================================ */

void test_cullFace_randomFuzz(void)
{
    unsigned int i;
    printf("TEST : Random Fuzz Test\n");
    resetState();
    srand(12345);

    for(i = 0; i < 1000000; i++)
    {
        GLenum value;
        GLenum err;
        switch(rand() % 5)
        {
            case 0:
                value = GL_BACK;
                break;
            case 1:
                value = GL_FRONT;
                break;
            case 2:
                value = GL_FRONT_AND_BACK;
                break;
            default:
                value = (GLenum)rand();
                break;
        }

        glCullFace(value);
        err = glGetError();
        if(err != GL_NO_ERROR &&
           err != GL_INVALID_ENUM)
        {
            printf("\n[FAIL]\n");
            printf("Iteration : %u\n", i);
            printf("Enum      : 0x%X\n", value);
            printf("Error     : 0x%X\n", err);

            assert(0);
        }

        if(err == GL_INVALID_ENUM)
        {
            checkStatePreserved(GL_FRONT_AND_BACK);
        }
    }

    resetState();
    printf("  [PASS] 1,000,000 rastgele test tamamlandı.\n\n");
}


/* ============================================================
 * Tüm glCullFace Robustness Testlerini Çalıştır
 * ============================================================ */

void Run_glCullFace_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("      glCullFace Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_cullFace_basicRobustness();
    test_cullFace_stressSweep();
    test_cullFace_errorQueue();
    test_cullFace_rapidToggle();
    test_cullFace_statePreservation();
    test_cullFace_frontFaceCombination();
    test_cullFace_largeEnum();
    test_cullFace_rapidFire();
    test_cullFace_randomFuzz();

    printf("=============================================\n");
    printf("  Tüm glCullFace Robustness Testleri Başarılı\n");
    printf("=============================================\n\n");
}
