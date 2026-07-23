#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/macro.h"

static const char* test_case1 = "Texturing_BindTexture_TC_001";
static const char* test_case2 = "Texturing_BindTexture_TC_002";
static const char* test_case3 = "Texturing_BindTexture_TC_003";
static const char* test_case4 = "Texturing_BindTexture_TC_004";
static const char* test_case5 = "Texturing_BindTexture_TC_005";
static const char* test_case6 = "Texturing_BindTexture_TC_006";
static const char* test_case7 = "Texturing_BindTexture_TC_007";
static const char* test_case8 = "Texturing_BindTexture_TC_008";

static const char* test_procedure = "Texturing_BindTexture_TP_001";

/*
 * glBindTexture Robustness Test Suite
 * Hasan - OpenGL ES 2.0
 *
 * Bu test paketi, glBindTexture fonksiyonunun texture'lara ozgu
 * baglama mekanizmalarini (target kilitlenmesi, active texture izolasyonu,
 * name adoption vb.) sinamak uzere ozel olarak hazirlanmistir.
 */

// ---------------------------------------------------------------
// TEST 1: Gecersiz Hedef (Invalid Target Enum)
// TEXTURE_2D ve TEXTURE_CUBE_MAP disindaki hedeflere texture
// baglamaya calismanin GL_INVALID_ENUM uretip uretmedigi test edilir.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_001(void)
{
    while (glGetError() != GL_NO_ERROR);

    GLuint tex;
    glGenTextures(1, &tex);

    // Buffer veya Framebuffer gibi alakasiz hedefler
    GLenum bad_targets[] = {GL_ARRAY_BUFFER, GL_FRAMEBUFFER, 0x0, 0xDEAD};
    int count = sizeof(bad_targets) / sizeof(bad_targets[0]);

    int all_invalid_enum = 1;

    for (int i = 0; i < count; i++) {
        glBindTexture(bad_targets[i], tex);
        GLenum err = glGetError();
        if (err != GL_INVALID_ENUM) {
            all_invalid_enum = 0;
        }
    }

    if (all_invalid_enum)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    else
        TEST_LOG_FAIL(test_case1, test_procedure, "Gecersiz hedefler reddedilmedi");

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 2: Isim Sahiplenme (Name Adoption / Generation on Bind)
// glGenTextures ile olusturulmamis rastgele bir ID'nin bind
// edilmesi. ES 2.0'da bu gecerli bir islemdir ve yeni bir texture olusturur.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_002(void)
{
    while (glGetError() != GL_NO_ERROR);

    // Rastgele, buyuk ihtimalle kullanilmayan bir ID
    GLuint random_id = 987654;

    // Oncesinde bu ID texture degil
    GLboolean is_tex_before = glIsTexture(random_id);

    // Bind ederek ismi sahiplen (Name adoption)
    glBindTexture(GL_TEXTURE_2D, random_id);
    GLenum err = glGetError();

    // Bind edildikten sonra artik bir texture olmali
    GLboolean is_tex_after = glIsTexture(random_id);

    if (!is_tex_before && is_tex_after && err == GL_NO_ERROR)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    else
        TEST_LOG_FAIL(test_case2, test_procedure, "Name adoption calismadi: before=%d, after=%d, err=0x%X", is_tex_before, is_tex_after, err);

    // Temizle
    glDeleteTextures(1, &random_id);
}

// ---------------------------------------------------------------
// TEST 3: Capraz Hedef Cakismasi (Cross-Target Conflict)
// Bir kere TEXTURE_2D olarak bind edilen (ve o hedefe kilitlenen)
// bir ID'nin TEXTURE_CUBE_MAP'e bind edilmeye calisilmasi durumu.
// GL_INVALID_OPERATION firlatilmalidir.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_003(void)
{
    while (glGetError() != GL_NO_ERROR);

    GLuint tex;
    glGenTextures(1, &tex);

    // Ilk olarak 2D'ye bind et (Artik bu ID 2D'ye kilitlendi)
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum err1 = glGetError();

    // Ayni ID'yi CUBE_MAP'e bind etmeye calis
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    GLenum err2 = glGetError();

    if (err1 == GL_NO_ERROR && err2 == GL_INVALID_OPERATION)
        TEST_LOG_SUCCESS(test_case3, test_procedure);
    else
        TEST_LOG_FAIL(test_case3, test_procedure, "Capraz hedef hatasi: err1=0x%X, err2=0x%X (0x502 beklenir)", err1, err2);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 4: Aktif Texture Unit Izolasyonu (Active Texture Binding)
// Texture0 ve Texture1 unitlerine ayri ID'ler bind edildiginde
// birbirlerini etkileyip etkilemediklerini kontrol eder.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_004(void)
{
    while (glGetError() != GL_NO_ERROR);

    GLuint tex[2];
    glGenTextures(2, tex);

    // TEXTURE0'a tex[0]'i bind et
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);

    // TEXTURE1'e tex[1]'i bind et
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);

    // State'leri geri oku
    GLint bound_t1 = -1;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_t1); // Su an aktif unit 1 oldugu icin tex[1] dondurmeli

    glActiveTexture(GL_TEXTURE0);
    GLint bound_t0 = -1;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_t0); // Simdi unit 0, tex[0] dondurmeli

    GLenum err = glGetError();

    if ((GLuint)bound_t0 == tex[0] && (GLuint)bound_t1 == tex[1] && err == GL_NO_ERROR)
        TEST_LOG_SUCCESS(test_case4, test_procedure);
    else
        TEST_LOG_FAIL(test_case4, test_procedure, "Izolasyon bozuk: t0=%d (beklenen %u), t1=%d (beklenen %u), err=0x%X", bound_t0, tex[0], bound_t1, tex[1], err);

    glDeleteTextures(2, tex);
}

// ---------------------------------------------------------------
// TEST 5: Silinmis Ismin Yeniden Bind Edilmesi
// Bir texture silindikten sonra ayni isim tekrar bind edilebilir mi?
// ES 2.0'a gore bu, o isimle 'yeni' bir texture olusturmalidir.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_005(void)
{
    while (glGetError() != GL_NO_ERROR);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Parametre degistir (varsayilandan farkli yapmak icin)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Texture'i sil
    glDeleteTextures(1, &tex);

    // Ayni ismi tekrar bind et (Reanimation / Ismi yeniden sahiplenme)
    glBindTexture(GL_TEXTURE_2D, tex);

    // Parametreleri kontrol et: Yeni texture oldugu icin sifirlanmis
    // ve varsayilan degerine (GL_NEAREST_MIPMAP_LINEAR) donmus olmali.
    GLint min_filter = 0;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);

    GLenum err = glGetError();

    if (min_filter == GL_NEAREST_MIPMAP_LINEAR && err == GL_NO_ERROR)
        TEST_LOG_SUCCESS(test_case5, test_procedure);
    else
        TEST_LOG_FAIL(test_case5, test_procedure, "Silinen texture yeniden olusturulmadi: min_filter=0x%X (0x2702 beklenir), err=0x%X", min_filter, err);

    // Artik manuel silmeliyiz cunku genTextures'in referansi degil
    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 6: Varsayilan Texture'a (ID 0) Donus
// 0 ID'sini bind etmek, aktif texture'i deaktif etmeli ve varsayilan
// texture state'ine dondurmelidir. Sifir bind etmek hata uretmez.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_006(void)
{
    while (glGetError() != GL_NO_ERROR);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Ozel bir ID'den 0'a donus
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint current_bind = -1;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_bind);
    GLenum err = glGetError();

    if (current_bind == 0 && err == GL_NO_ERROR)
        TEST_LOG_SUCCESS(test_case6, test_procedure);
    else
        TEST_LOG_FAIL(test_case6, test_procedure, "Sifir bind hatasi: bind=%d (0 beklenir), err=0x%X", current_bind, err);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 7: Ayni ID'nin Defalarca Bind Edilmesi (No-op Stresi)
// Performans kritik kodlarda ayni texture ust uste bind edilebilir.
// Surucunun state degisikligi olmadigini anlayip cokmeden calismasi test edilir.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_007(void)
{
    while (glGetError() != GL_NO_ERROR);

    GLuint tex;
    glGenTextures(1, &tex);

    // Ayni texture 10,000 kez baglanir
    for(int i = 0; i < 10000; i++) {
        glBindTexture(GL_TEXTURE_2D, tex);
    }

    GLenum err = glGetError();

    if (err == GL_NO_ERROR)
        TEST_LOG_SUCCESS(test_case7, test_procedure);
    else
        TEST_LOG_FAIL(test_case7, test_procedure, "Tekrarli bind stresi sirasinda hata: 0x%X", err);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 8: Asiri Buyuk ID Isimleri Ile Sinir Zorlamasi
// Isim sahiplenme (name adoption) ozelliginin asiri buyuk int
// degerleriyle (UINT_MAX) kullanilmasi. Surucunun bunlari
// handle edebildigini veya reddebildigini izleriz.
// ---------------------------------------------------------------
void Texturing_BindTexture_TC_008(void)
{
    while (glGetError() != GL_NO_ERROR);

    GLuint extreme_ids[] = {
        0xFFFFFFFF,  // UINT_MAX
        0x7FFFFFFF,  // INT_MAX
        0x80000000   // Sadece isaret biti
    };

    int all_ok = 1;

    for(int i=0; i<3; i++) {
        glBindTexture(GL_TEXTURE_2D, extreme_ids[i]);
        GLenum err = glGetError();
        
        GLint bound = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
        
        if (err != GL_NO_ERROR || (GLuint)bound != extreme_ids[i]) {
            all_ok = 0;
        }

        // Temizle
        glDeleteTextures(1, &extreme_ids[i]);
    }

    if (all_ok)
        TEST_LOG_SUCCESS(test_case8, test_procedure);
    else
        TEST_LOG_FAIL(test_case8, test_procedure, "Bazi ucbirim ID'ler tam desteklenmiyor");
}
