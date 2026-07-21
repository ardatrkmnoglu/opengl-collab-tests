#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* ============================================================
 * Test altyapısı
 *
 * resetState:
 * Her test öncesinde glFrontFace varsayılan değeri olan
 * GL_CCW olarak ayarlanır ve hata kuyruğu temizlenir.
 *
 * checkStatePreserved:
 * OpenGL durumunun beklenen değerde kaldığını doğrular.
 * Geçersiz çağrılar sonrasında state değişmemelidir.
 * ============================================================ */

static void resetState(void)
{
    glFrontFace(GL_CCW);
    while(glGetError()!=GL_NO_ERROR);
}

static void checkStatePreserved(GLint expected)
{
    GLint actual;
    glGetIntegerv(GL_FRONT_FACE,&actual);
    if(actual!=expected)
    {
        printf("  [FAIL] State bozuldu!\n");
        printf("         Beklenen : 0x%X\n",expected);
        printf("         Gercek   : 0x%X\n",actual);
        assert(0);
    }
}


/* ============================================================
 * TEST 1 : Error Queue Management
 * ============================================================
 *
 * Amaç
 * ----
 * Arka arkaya çok sayıda geçersiz enum gönderildiğinde
 * hata kuyruğunun bozulmadığını doğrular.
 *
 * Daha sonra geçerli bir çağrı yapılarak OpenGL'in
 * normal çalışmaya döndüğü kontrol edilir.
 * ============================================================ */

void test_frontFace_errorQueue(void)
{
    GLenum err;
    int errorCount=0;
    int i;
    printf("TEST : Error Queue Management\n");
    resetState();
    for(i=0;i<100;i++)
    {
        glFrontFace((GLenum)(0x5000+i));
    }
    while((err=glGetError())!=GL_NO_ERROR)
    {
        assert(err==GL_INVALID_ENUM);
        errorCount++;
    }
    assert(errorCount>0);
    glFrontFace(GL_CW);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_CW);
    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 2 : Rapid Toggle
 * ============================================================
 *
 * Amaç
 * ----
 * GL_CW ile GL_CCW arasında on binlerce kez geçiş
 * yaptırılarak state makinesinin bozulmadığı doğrulanır.
 *
 * Her çağrı sonrasında GL_FRONT_FACE sorgulanır.
 * ============================================================ */

void test_frontFace_rapidToggle(void)
{
    const int repeat=100000;
    int i;
    printf("TEST : Rapid Toggle\n");
    resetState();
    for(i=0;i<repeat;i++)
    {
        GLenum expected=
            (i&1)?GL_CCW:GL_CW;
        GLint current;
        glFrontFace(expected);
        assert(glGetError()==GL_NO_ERROR);
        glGetIntegerv(GL_FRONT_FACE,
                      &current);
        assert(current==expected);
    }

    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 3 : Mixed Valid / Invalid Calls
 * ============================================================
 *
 * Amaç
 * ----
 * Geçerli ve geçersiz çağrılar karıştırılarak
 * sürücünün hata durumundan doğru şekilde
 * çıkabildiği doğrulanır.
 *
 * Ayrıca geçersiz enum sonrasında state'in
 * değişmediği de kontrol edilir.
 * ============================================================ */

void test_frontFace_mixedValidity(void)
{
    GLenum sequence[]=
    {
        GL_CW,
        0x1111,
        GL_CCW,
        0x2222,
        GL_CW,
        0x3333,
        GL_CCW,
        0x4444
    };

    int count=
        sizeof(sequence)/
        sizeof(sequence[0]);

    int i;

    printf("TEST : Mixed Validity\n");
    resetState();

    for(i=0;i<count;i++)
    {
        GLenum value=sequence[i];
        GLenum expectedError=
            (value==GL_CW || value==GL_CCW)?
            GL_NO_ERROR:
            GL_INVALID_ENUM;
        glFrontFace(value);
        assert(glGetError()==expectedError);
        if(value==GL_CW)
            checkStatePreserved(GL_CW);
        if(value==GL_CCW)
            checkStatePreserved(GL_CCW);
    }

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4 : State Preservation
 * ============================================================
 *
 * Amaç
 * ----
 * Geçersiz glFrontFace() çağrılarının mevcut OpenGL
 * durumunu değiştirmediğini doğrular.
 *
 * Önce geçerli bir durum oluşturulur.
 * Daha sonra farklı geçersiz enum değerleri gönderilir.
 * Her başarısız çağrıdan sonra GL_FRONT_FACE
 * değerinin değişmediği kontrol edilir.
 * ============================================================ */

void test_frontFace_statePreservation(void)
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
    glFrontFace(GL_CW);
    assert(glGetError()==GL_NO_ERROR);
    checkStatePreserved(GL_CW);

    for(i=0;i<sizeof(invalidEnums)/sizeof(invalidEnums[0]);i++)
    {
        glFrontFace(invalidEnums[i]);
        err=glGetError();
        assert(err==GL_INVALID_ENUM);
        checkStatePreserved(GL_CW);
    }

    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 5 : Cull Face Combination
 * ============================================================
 *
 * Amaç
 * ----
 * glFrontFace() ve glCullFace() fonksiyonlarının birlikte
 * çalışırken birbirlerinin durumunu bozmadığını doğrular.
 *
 * Tüm geçerli kombinasyonlar denenir ve her çağrı
 * sonrasında OpenGL state'i kontrol edilir.
 * ============================================================ */

void test_frontFace_cullCombinations(void)
{
    GLenum frontModes[] =
    {
        GL_CCW,
        GL_CW
    };

    GLenum cullModes[] =
    {
        GL_FRONT,
        GL_BACK,
        GL_FRONT_AND_BACK
    };

    int i;
    int j;

    printf("TEST : Cull Face Combinations\n");
    resetState();
    glEnable(GL_CULL_FACE);
    for(i=0;i<2;i++)
    {
        for(j=0;j<3;j++)
        {
            GLint currentFront;
            GLint currentCull;
            glFrontFace(frontModes[i]);

            assert(glGetError()==GL_NO_ERROR);
            glCullFace(cullModes[j]);
            assert(glGetError()==GL_NO_ERROR);

            glGetIntegerv(GL_FRONT_FACE,
                          &currentFront);
            glGetIntegerv(GL_CULL_FACE_MODE,
                          &currentCull);

            assert(currentFront==frontModes[i]);
            assert(currentCull==cullModes[j]);
        }
    }

    glDisable(GL_CULL_FACE);
    resetState();
    printf("  [PASS]\n\n");
}


/* ============================================================
 * TEST 6 : Large Invalid Enum Values
 * ============================================================
 *
 * Amaç
 * ----
 * Çok büyük GLenum değerleri gönderildiğinde
 * sürücünün çökmediği ve yalnızca
 * GL_INVALID_ENUM ürettiği doğrulanır.
 *
 * Ayrıca başarısız çağrıların mevcut durumu
 * değiştirmediği de kontrol edilir.
 * ============================================================ */

void test_frontFace_largeEnum(void)
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

    for(i=0;i<sizeof(values)/sizeof(values[0]);i++)
    {
        GLenum err;
        glFrontFace(values[i]);
        err=glGetError();
        assert(err==GL_INVALID_ENUM);
        checkStatePreserved(GL_CCW);
    }

    resetState();
    printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7 : Rapid Fire
 * ============================================================
 *
 * Amaç
 * ----
 * glFrontFace() fonksiyonunu çok kısa aralıklarla
 * art arda çağırarak sürücünün yoğun kullanım altında
 * kararlılığını doğrular.
 *
 * Test sonunda herhangi bir OpenGL hatası oluşmamalı
 * ve son durum doğru şekilde korunmalıdır.
 * ============================================================ */

void test_frontFace_rapidFire(void)
{
    const unsigned int repeat = 1000000;
    unsigned int i;
    printf("TEST : Rapid Fire\n");
    resetState();

    for(i = 0; i < repeat; i++)
    {
        glFrontFace(GL_CW);
        glFrontFace(GL_CCW);
    }

    assert(glGetError() == GL_NO_ERROR);
    checkStatePreserved(GL_CCW);
    resetState();
    printf("  [PASS] %u cift cagri tamamlandi.\n\n", repeat);
}


/* ============================================================
 * TEST 8 : Random Fuzz Test
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

void test_frontFace_randomFuzz(void)
{
    unsigned int i;
    printf("TEST : Random Fuzz Test\n");
    resetState();
    srand(12345);

    for(i = 0; i < 1000000; i++)
    {
        GLenum value;
        GLenum err;
        switch(rand() % 4)
        {
            case 0:
                value = GL_CW;
                break;
            case 1:
                value = GL_CCW;
                break;
            default:
                value = (GLenum)rand();
                break;
        }

        glFrontFace(value);
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
    }

    resetState();
    printf("  [PASS] 1,000,000 rastgele test tamamlandi.\n\n");
}


/* ============================================================
 * Tum glFrontFace Robustness Testlerini Calistir
 * ============================================================ */

void Run_glFrontFace_Robustness(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("      glFrontFace Robustness Test Suite\n");
    printf("=============================================\n\n");

    test_frontFace_errorQueue();
    test_frontFace_rapidToggle();
    test_frontFace_mixedValidity();
    test_frontFace_statePreservation();
    test_frontFace_cullCombinations();
    test_frontFace_largeEnum();
    test_frontFace_rapidFire();
    test_frontFace_randomFuzz();

    printf("=============================================\n");
    printf(" Tum glFrontFace Robustness Testleri Basarili\n");
    printf("=============================================\n\n");
}