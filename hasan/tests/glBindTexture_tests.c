#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// void glBindTexture(GLenum target, GLuint texture);
// Bir texture nesnesini belirli bir target'a baglar.
// Baglandiktan sonra o hedef uzerinde yapilan islemler artik bu texture uzerinde gerceklestirilir.
// target: TEXTURE_2D, TEXTURE_CUBE_MAP


// Belirtilen hata: GL_INVALID_ENUM is generated if target is not one of the allowable values.
void rTest_glBindTexture_invalid_enum()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_invalid_enum()\n");

    glBindTexture(0xFFFFFFFF, 1);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
    } else {
        printf("[PASS] rTest_glBindTexture_invalid_enum()\n");
    }
}

// glGenTextures ile olusturulmamis bir ismin bind edilmesi
void rTest_glBindTexture_new_name_without_gen()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_new_name_without_gen()\n");

    GLuint name = 424242;
    glBindTexture(GL_TEXTURE_2D, name);
    GLenum err = glGetError();
    printf("[INFO] glBindTexture(new name=%u): error=0x%X\n", name, err);
}

// Silinen bir texture isminin tekrar bind edilmesiyle yeni bir texture nesnesi olusturulup
// olusturulmadigini test eder.
void rTest_glBindTexture_deleted_texture()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_deleted_texture()\n");

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDeleteTextures(1, &tex);

    // Silindikten sonra tekrar bind etmeye calisiyoruz
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum err = glGetError();
    printf("[INFO] Bind deleted texture name: error=0x%X\n", err);
}

// Buyuk/alisilamadik texture isimlerinin bind edilmesi
void rTest_glBindTexture_boundary_handles()
{
    printf("[START] rTest_glBindTexture_boundary_handles()\n");

    GLuint candidates[] = {
        0xFFFFFFFFu,   // UINT_MAX
        0x80000000u,   // sign-bit siniri
        0x7FFFFFFFu,   // INT_MAX
        0xDEADBEEFu,
        0xCDCDCDCDu    // tipik uninitialized heap pattern
    };
    for (int i = 0; i < 5; ++i) {
        while (glGetError() != GL_NO_ERROR) {}
        glBindTexture(GL_TEXTURE_2D, candidates[i]);
        GLenum err = glGetError();
        printf("[INFO] Boundary texture 0x%08X: glError=0x%X\n", candidates[i], err);
    }
}

// Gecersiz target enum degerlerine karsi implementasyonun hata kontrolunun testi
void rTest_glBindTexture_dirty_high_bits_enum()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_dirty_high_bits_enum()\n");

    GLenum polluted = GL_TEXTURE_2D | 0xFFFF0000u;
    glBindTexture(polluted, 1);
    GLenum err = glGetError();
    // Spec'e gore bu "allowable degil" -> INVALID_ENUM beklenir
    printf("[INFO] Polluted target=0x%08X : glError=0x%X (expected GL_INVALID_ENUM)\n", polluted, err);
}

// Ayni texture nesnesinin farkli target'lara hizli ve tekrarli sekilde baglanmasi sirasinda
// implementasyonun kararliligini test eder.
// GLES 2.0'da bir texture farkli target'lara bind edilemez -> GL_INVALID_OPERATION beklenir.
void rTest_glBindTexture_rapid_cross_target_rebind_stress()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_rapid_cross_target_rebind_stress()\n");

    GLuint tex;
    glGenTextures(1, &tex);

    // Once TEXTURE_2D'ye bind et
    glBindTexture(GL_TEXTURE_2D, tex);

    // Simdi ayni texture'i TEXTURE_CUBE_MAP'e bind etmeye calis
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    GLenum err = glGetError();

    // GLES 2.0 spec: "If texture was previously created with a target that doesn't match
    // that of target, an INVALID_OPERATION error is generated"
    if (err == GL_INVALID_OPERATION) {
        printf("[PASS] Cross-target rebind correctly rejected with GL_INVALID_OPERATION.\n");
    } else {
        printf("[INFO] Cross-target rebind: glError=0x%X (expected GL_INVALID_OPERATION)\n", err);
    }

    glDeleteTextures(1, &tex);
}

// Texture bind edildikten sonra silindiginde, hedefteki baglantinin otomatik olarak
// 0'a (varsayilan texture) dusurulup dusurulmedigini dogrular.
void rTest_glBindTexture_delete_while_bound()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_delete_while_bound()\n");

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDeleteTextures(1, &tex);

    // Spec'e gore: silinen texture otomatik olarak unbind edilir (binding 0'a doner)
    GLint binding = -1;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &binding);

    GLenum err = glGetError();
    printf("[INFO] After deleting bound texture: TEXTURE_BINDING_2D=%d (expected 0), error=0x%X\n",
           binding, err);
}

// Texture'i tekrar tekrar 0'a baglayip baglama durumunu sorgulayarak
// implementasyonun state yonetimi kararliligini test eder.
void rTest_glBindTexture_zero_binding_query_thrash()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_zero_binding_query_thrash()\n");

    for (int i = 0; i < 1000; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        GLint binding = -1;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &binding);
        if (binding != 0) {
            printf("[FAIL] Iteration=%d, GL_TEXTURE_BINDING_2D=%d (expected 0)\n", i, binding);
            return;
        }

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);
            return;
        }
    }
    printf("[PASS] Zero binding/query thrash completed successfully.\n");
}

// Cok sayida texture ismi uzerinde rastgele bind islemleri yaparak implementasyonun
// isim yonetimi ve durum degisikliklerine karsi dayanikliligini test eder.
void rTest_glBindTexture_massive_namespace_fuzz()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_massive_namespace_fuzz()\n");

    const int N = 20000;
    GLuint *names = (GLuint *)malloc(sizeof(GLuint) * N);

    if (names == NULL) {
        printf("[FAIL] Memory allocation failed.\n");
        return;
    }

    glGenTextures(N, names);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("[FAIL] glGenTextures failed: glError=0x%X\n", err);
        free(names);
        return;
    }

    uint32_t seed = 0x1234567u;

    for (int i = 0; i < N; ++i)
    {
        seed ^= seed << 13;
        seed ^= seed >> 17;
        seed ^= seed << 5;
        GLuint name = names[seed % N];

        glBindTexture(GL_TEXTURE_2D, name);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            printf("[FAIL] Iteration=%d, texture=%u, glError=0x%X\n",
                   i, name, err);

            glDeleteTextures(N, names);
            free(names);
            return;
        }
    }

    glDeleteTextures(N, names);
    free(names);

    printf("[PASS] Massive texture namespace fuzz completed without OpenGL errors.\n");
}

// Ayni target uzerinde farkli texture'lar arasinda surekli gecis yaparak
// implementasyonun state yonetimi kararliligini test eder.
void rTest_glBindTexture_binding_churn_stress()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_binding_churn_stress()\n");

    GLuint textures[2];
    glGenTextures(2, textures);

    for (int i = 0; i < 10000; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);

            glDeleteTextures(2, textures);
            return;
        }
    }

    glDeleteTextures(2, textures);

    printf("[PASS] Binding churn stress completed without OpenGL errors.\n");
}

// Texture nesnelerinin olusturma, baglama ve silme yasam dongusunu tekrarli olarak
// calistirarak implementasyonun dayanikliligini test eder.
void rTest_glBindTexture_lifecycle_stress()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glBindTexture_lifecycle_stress()\n");

    for (int i = 0; i < 5000; ++i)
    {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        // Bind edildikten sonra basit bir islem yap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glDeleteTextures(1, &tex);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);
            return;
        }
    }
    printf("[PASS] Texture lifecycle stress completed without OpenGL errors.\n");
}

// Gecersiz target enum degerleri dizisiyle sistematik olarak glBindTexture cagrilarak
// implementasyonun tum gecersiz degerler icin GL_INVALID_ENUM dondurup dondurmedigini test eder.
void rTest_glBindTexture_invalid_enum_sweep()
{
    printf("[START] rTest_glBindTexture_invalid_enum_sweep()\n");

    GLenum candidates[] = {
        0x0000,         // sifir
        0x0001,         // GL_POINTS (gecersiz target)
        0x1234,         // rastgele deger
        0x9999,         // rastgele deger
        0xDEAD,         // sentinel
        GL_ARRAY_BUFFER // buffer target, texture degil
    };

    for (int i = 0; i < 6; ++i) {
        while (glGetError() != GL_NO_ERROR) {}
        glBindTexture(candidates[i], 1);
        GLenum err = glGetError();
        printf("[INFO] glBindTexture(target=0x%04X): glError=0x%X (expected GL_INVALID_ENUM)\n",
               candidates[i], err);
    }
}

// =========================================================
// ANA YAPI: init, draw, clean
// =========================================================

static GLFWwindow* window = NULL;
static int width = 640, height = 480;
static int g_tests_failed = 0;
static const char* windowTitle = "glBindTexture Robustness Tests";

void init(void);
void draw(void);
void clean(void);

void init(void)
{
    printf("=========================================\n");
    printf("   GLBINDTEXTURE SAGLAMLIK TESTLERI\n");
    printf("=========================================\n\n");

    rTest_glBindTexture_invalid_enum();
    rTest_glBindTexture_new_name_without_gen();
    rTest_glBindTexture_deleted_texture();
    rTest_glBindTexture_boundary_handles();
    rTest_glBindTexture_dirty_high_bits_enum();
    rTest_glBindTexture_rapid_cross_target_rebind_stress();
    rTest_glBindTexture_delete_while_bound();
    rTest_glBindTexture_zero_binding_query_thrash();
    rTest_glBindTexture_massive_namespace_fuzz();
    rTest_glBindTexture_binding_churn_stress();
    rTest_glBindTexture_lifecycle_stress();
    rTest_glBindTexture_invalid_enum_sweep();

    printf("\n=========================================\n");
    printf("        TUM TESTLER TAMAMLANDI\n");
    printf("=========================================\n");
}

void draw(void)
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void clean(void)
{
    // Temizlik islemleri
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
    return g_tests_failed ? -1 : 0;
}
