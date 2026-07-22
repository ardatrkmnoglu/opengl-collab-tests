#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
void test_bind_invalid_target_enum(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_invalid_target_enum\n");

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
            printf("  Hedef 0x%04X icin beklenen hata GL_INVALID_ENUM, alinan: 0x%X\n", bad_targets[i], err);
            all_invalid_enum = 0;
        }
    }

    if (all_invalid_enum)
        printf("  -> PASSED: Gecersiz hedefler dogru sekilde reddedildi (GL_INVALID_ENUM)\n\n");
    else
        printf("  -> FAILED\n\n");

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 2: Isim Sahiplenme (Name Adoption / Generation on Bind)
// glGenTextures ile olusturulmamis rastgele bir ID'nin bind
// edilmesi. ES 2.0'da bu gecerli bir islemdir ve yeni bir texture olusturur.
// ---------------------------------------------------------------
void test_bind_name_adoption(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_name_adoption\n");

    // Rastgele, buyuk ihtimalle kullanilmayan bir ID
    GLuint random_id = 987654;

    // Oncesinde bu ID texture degil
    GLboolean is_tex_before = glIsTexture(random_id);

    // Bind ederek ismi sahiplen (Name adoption)
    glBindTexture(GL_TEXTURE_2D, random_id);
    GLenum err = glGetError();

    // Bind edildikten sonra artik bir texture olmali
    GLboolean is_tex_after = glIsTexture(random_id);

    printf("  Oncesinde glIsTexture: %s (GL_FALSE beklenir)\n", is_tex_before ? "GL_TRUE" : "GL_FALSE");
    printf("  Bind sonrasi glIsTexture: %s (GL_TRUE beklenir)\n", is_tex_after ? "GL_TRUE" : "GL_FALSE");

    if (!is_tex_before && is_tex_after && err == GL_NO_ERROR)
        printf("  -> PASSED: Isim basariyla sahiplenildi (Name Adoption calisiyor)\n\n");
    else
        printf("  -> FAILED: Hata=0x%X\n\n", err);

    // Temizle
    glDeleteTextures(1, &random_id);
}

// ---------------------------------------------------------------
// TEST 3: Capraz Hedef Cakismasi (Cross-Target Conflict)
// Bir kere TEXTURE_2D olarak bind edilen (ve o hedefe kilitlenen)
// bir ID'nin TEXTURE_CUBE_MAP'e bind edilmeye calisilmasi durumu.
// GL_INVALID_OPERATION firlatilmalidir.
// ---------------------------------------------------------------
void test_bind_cross_target_conflict(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_cross_target_conflict\n");

    GLuint tex;
    glGenTextures(1, &tex);

    // Ilk olarak 2D'ye bind et (Artik bu ID 2D'ye kilitlendi)
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum err1 = glGetError();

    // Ayni ID'yi CUBE_MAP'e bind etmeye calis
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    GLenum err2 = glGetError();

    printf("  Ilk bind (2D) hatasi: 0x%X\n", err1);
    printf("  Ikinci bind (CUBE_MAP) hatasi: 0x%X (GL_INVALID_OPERATION 0x502 beklenir)\n", err2);

    if (err1 == GL_NO_ERROR && err2 == GL_INVALID_OPERATION)
        printf("  -> PASSED: Capraz hedef cakismasi spec'e uygun engellendi\n\n");
    else
        printf("  -> FAILED\n\n");

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 4: Aktif Texture Unit Izolasyonu (Active Texture Binding)
// Texture0 ve Texture1 unitlerine ayri ID'ler bind edildiginde
// birbirlerini etkileyip etkilemediklerini kontrol eder.
// ---------------------------------------------------------------
void test_bind_active_texture_isolation(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_active_texture_isolation\n");

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

    printf("  TEXTURE0'daki bind: %d (Beklenen: %u)\n", bound_t0, tex[0]);
    printf("  TEXTURE1'deki bind: %d (Beklenen: %u)\n", bound_t1, tex[1]);

    if ((GLuint)bound_t0 == tex[0] && (GLuint)bound_t1 == tex[1] && err == GL_NO_ERROR)
        printf("  -> PASSED: Unit izolasyonu saglam\n\n");
    else
        printf("  -> FAILED: Izolasyon bozuk veya hata var (0x%X)\n\n", err);

    glDeleteTextures(2, tex);
}

// ---------------------------------------------------------------
// TEST 5: Silinmis Ismin Yeniden Bind Edilmesi
// Bir texture silindikten sonra ayni isim tekrar bind edilebilir mi?
// ES 2.0'a gore bu, o isimle 'yeni' bir texture olusturmalidir.
// ---------------------------------------------------------------
void test_bind_deleted_texture_reanimation(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_deleted_texture_reanimation\n");

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

    printf("  Yeniden dogan texture MIN_FILTER: 0x%X (GL_NEAREST_MIPMAP_LINEAR 0x2702 beklenir)\n", min_filter);

    if (min_filter == GL_NEAREST_MIPMAP_LINEAR && err == GL_NO_ERROR)
        printf("  -> PASSED: Silinen isimle tertemiz yeni texture olusturuldu\n\n");
    else
        printf("  -> FAILED: Eski durum korunmus veya hata var (0x%X)\n\n", err);

    // Artik manuel silmeliyiz cunku genTextures'in referansi degil
    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 6: Varsayilan Texture'a (ID 0) Donus
// 0 ID'sini bind etmek, aktif texture'i deaktif etmeli ve varsayilan
// texture state'ine dondurmelidir. Sifir bind etmek hata uretmez.
// ---------------------------------------------------------------
void test_bind_zero_default(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_zero_default\n");

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Ozel bir ID'den 0'a donus
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint current_bind = -1;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_bind);
    GLenum err = glGetError();

    if (current_bind == 0 && err == GL_NO_ERROR)
        printf("  -> PASSED: 0 ID'si basariyla bind edildi ve hata firlatilmadi\n\n");
    else
        printf("  -> FAILED: Beklenen bind 0, alinan %d, Hata: 0x%X\n\n", current_bind, err);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 7: Ayni ID'nin Defalarca Bind Edilmesi (No-op Stresi)
// Performans kritik kodlarda ayni texture ust uste bind edilebilir.
// Surucunun state degisikligi olmadigini anlayip cokmeden calismasi test edilir.
// ---------------------------------------------------------------
void test_bind_repeatedly_noop_stress(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_repeatedly_noop_stress\n");

    GLuint tex;
    glGenTextures(1, &tex);

    // Ayni texture 10,000 kez baglanir
    for(int i = 0; i < 10000; i++) {
        glBindTexture(GL_TEXTURE_2D, tex);
    }

    GLenum err = glGetError();

    if (err == GL_NO_ERROR)
        printf("  -> PASSED: Ust uste ayni ID'nin bind edilmesi sikintisiz atlatildi\n\n");
    else
        printf("  -> FAILED: Stres sirasinda hata olustu: 0x%X\n\n", err);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 8: Asiri Buyuk ID Isimleri Ile Sinir Zorlamasi
// Isim sahiplenme (name adoption) ozelliginin asiri buyuk int
// degerleriyle (UINT_MAX) kullanilmasi. Surucunun bunlari
// handle edebildigini veya reddebildigini izleriz.
// ---------------------------------------------------------------
void test_bind_extreme_ids(void)
{
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST] test_bind_extreme_ids\n");

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

        printf("  ID: 0x%08X -> Hata: 0x%X, Gecerli Bind: 0x%08X\n", extreme_ids[i], err, bound);
        
        if (err != GL_NO_ERROR || (GLuint)bound != extreme_ids[i]) {
            all_ok = 0;
        }

        // Temizle
        glDeleteTextures(1, &extreme_ids[i]);
    }

    if (all_ok)
        printf("  -> PASSED: Surucu cok buyuk texture ID'lerini sorunsuzca isliyor\n\n");
    else
        printf("  -> FAILED/WARNING: Bazi ucbirim ID'ler tam desteklenmiyor olabilir\n\n");
}


// =========================================================
// ANA YAPI: init, draw, clean
// =========================================================

static GLFWwindow* window = NULL;
static int width = 640, height = 480;
static const char* windowTitle = "glBindTexture Robustness Tests - Hasan";

void init(void);
void draw(void);
void clean(void);

void init(void)
{
    printf("=====================================================\n");
    printf("  GLBINDTEXTURE ROBUSTNESS SUITE - HASAN\n");
    printf("=====================================================\n\n");

    test_bind_invalid_target_enum();
    test_bind_name_adoption();
    test_bind_cross_target_conflict();
    test_bind_active_texture_isolation();
    test_bind_deleted_texture_reanimation();
    test_bind_zero_default();
    test_bind_repeatedly_noop_stress();
    test_bind_extreme_ids();

    printf("=====================================================\n");
    printf("  TUM TESTLER TAMAMLANDI\n");
    printf("=====================================================\n");
}

void draw(void)
{
    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void clean(void)
{
    // Temizlik
}

int main(void)
{
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    setenv("GALLIUM_DRIVER", "llvmpipe", 1);
    unsetenv("WAYLAND_DISPLAY");

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    init();

    while (!glfwWindowShouldClose(window)) {
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    clean();
    glfwTerminate();
    return 0;
}
