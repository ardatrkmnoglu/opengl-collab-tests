#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// glGenTextures: yeni texture nesneleri icin ID uretir
// n: Kac tane texture ismi uretilecegi.
// textures: Uretilen texture ID'lerinin yazilacagi dizi.


// Belirtilen hata: GL_INVALID_VALUE is generated if n is negative.
void rTest_glGenTextures_invalid_value()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_invalid_value()\n");

    GLuint texture = 0;
    glGenTextures(-1, &texture);
    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
    } else {
        printf("[PASS] rTest_glGenTextures_invalid_value()\n");
    }
}

// n = 0 ile cagri
void rTest_glGenTextures_zero_count()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_zero_count()\n");

    GLuint tex = 0xCDCDCDCD; // sentinel deger
    glGenTextures(0, &tex);
    GLenum err = glGetError();
    printf("[INFO] glGenTextures(n=0): error=0x%X, texture=0x%08X\n", err, tex);
}

// textures = NULL, n > 0 (negative robustness)
void rTest_glGenTextures_null_textures()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_null_textures()\n");

    glGenTextures(5, NULL);
    GLenum err = glGetError();
    printf("[INFO] glGenTextures(textures=nullptr, n=5): error=0x%X\n", err);
}

// Asiri buyuk n
void rTest_glGenTextures_large_n()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_large_n()\n");

    const GLsizei largeCount = 100000;
    GLuint *textures = (GLuint *)malloc(sizeof(GLuint) * largeCount);
    if (textures == NULL)
    {
        printf("[ERROR] Memory allocation failed.\n");
        return;
    }
    glGenTextures(largeCount, textures);
    GLenum err = glGetError();
    printf("[INFO] glGenTextures(n=%d): error=0x%X\n", largeCount, err);

    glDeleteTextures(largeCount, textures);
    free(textures);
}

// Ayni array'i art arda, isim tekilligini bozmaya calisarak cagrima (fonksiyon 1000 kez art arda cagrildiginda hata veriyor mu)
void rTest_glGenTextures_repeated_generation()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_repeated_generation()\n");

    GLuint textures[10];
    for (int i = 0; i < 1000; ++i)
    {
        glGenTextures(10, textures);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("[FAIL] glGenTextures failed at iteration %d: error=0x%X\n", i, err);
            return;
        }
    }
    printf("[PASS] Repeated glGenTextures(10) x1000 completed successfully.\n");
}

// Cok sayida texture adi ureterek dondurulen isimlerin benzersiz oldugunu ve reserved 0 isminin uretilmedigini dogrular.
void rTest_glGenTextures_unique_names()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_unique_names()\n");

    const GLsizei COUNT = 1000;
    GLuint textures[1000];
    glGenTextures(COUNT, textures);

    // 0 ismi uretilmemeli
    for (int i = 0; i < COUNT; i++) {
        if (textures[i] == 0) {
            printf("[FAIL] glGenTextures returned reserved name 0.\n");
            glDeleteTextures(COUNT, textures);
            return;
        }
    }

    // Ayni isim iki kez uretilmemeli
    for (int i = 0; i < COUNT; i++) {
        for (int j = i + 1; j < COUNT; j++) {
            if (textures[i] == textures[j]) {
                printf("[FAIL] Duplicate texture name %u found.\n", textures[i]);
                glDeleteTextures(COUNT, textures);
                return;
            }
        }
    }

    GLenum err = glGetError();
    if (err == GL_NO_ERROR) printf("[PASS] All generated texture names are unique and non-zero.\n");
    else printf("[FAIL] glGetError() = 0x%X\n", err);

    glDeleteTextures(COUNT, textures);
}

// Bind edilmemis texture isimleri uzerinde glIsTexture ve glDeleteTextures cagrilarinin
// spesifikasyona uygun davranip davranmadigini dogrular.
void rTest_glGenTextures_unbound_names_lifecycle()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_unbound_names_lifecycle()\n");

    GLuint tex;
    glGenTextures(1, &tex);

    // Spec'e gore: glGenTextures ile uretilmis ama henuz bind edilmemis isim, glIsTexture'da GL_FALSE donmeli
    GLboolean isTexture = glIsTexture(tex);

    glDeleteTextures(1, &tex);

    GLenum err = glGetError();
    printf("[INFO] Unbound texture name: glIsTexture=%s, glDeleteTextures error=0x%X\n",
           isTexture ? "GL_TRUE" : "GL_FALSE", err);
}

// Ayni texture isminin birden fazla kez silinmesi durumunda implementasyonun kararliligini test eder.
void rTest_glGenTextures_double_delete()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_double_delete()\n");

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDeleteTextures(1, &tex);
    GLenum firstErr = glGetError();
    glDeleteTextures(1, &tex);
    GLenum secondErr = glGetError();
    printf("[INFO] Double delete: firstErr=0x%X, secondErr=0x%X\n", firstErr, secondErr);
}

// Buyuk 'n' degeri ve kasitli olarak yetersiz output buffer kullanilarak implementasyonun
// gecersiz istemci bellegi karsisindaki davranisi test edilir (negative robustness)
void rTest_glGenTextures_huge_count_small_buffer()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_huge_count_small_buffer()\n");

    GLsizei huge_n = INT32_MAX; // n * sizeof(GLuint) ic hesapta overflow edebilir

    GLuint textures[1]; // kasitli olarak yetersiz boyutlu tampon

    // NOT: gercek n kadar buyuk array vermiyoruz -- implementasyonun n'i gercekten
    // kullanip kullanmadigini, yoksa ic limitle mi kisitladigini gormek icin.
    glGenTextures(huge_n, textures);

    GLenum err = glGetError();
    printf("n=INT_MAX -> glError=0x%x\n", err);
}

// Silinen texture'in isminin yeniden uretilip uretilemeyecegini kontrol eder.
// glDeleteTextures sonrasi ayni ID'nin tekrar glGenTextures ile donup donmedigini gozlemler.
void rTest_glGenTextures_name_reuse_after_delete()
{
    while (glGetError() != GL_NO_ERROR) {}
    printf("[START] rTest_glGenTextures_name_reuse_after_delete()\n");

    GLuint tex1, tex2;
    glGenTextures(1, &tex1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glDeleteTextures(1, &tex1);

    // Silindikten sonra ayni isim geri donebilir mi?
    glGenTextures(1, &tex2);

    GLenum err = glGetError();
    printf("[INFO] First name=%u, second name=%u, reused=%s, error=0x%X\n",
           tex1, tex2, (tex1 == tex2) ? "YES" : "NO", err);

    glDeleteTextures(1, &tex2);
}

// =========================================================
// ANA YAPI: init, draw, clean
// =========================================================

static GLFWwindow* window = NULL;
static int width = 640, height = 480;
static int g_tests_failed = 0;
static const char* windowTitle = "glGenTextures Robustness Tests";

void init(void);
void draw(void);
void clean(void);

void init(void)
{
    printf("=========================================\n");
    printf("   GLGENTEXTURES SAGLAMLIK TESTLERI\n");
    printf("=========================================\n\n");

    rTest_glGenTextures_invalid_value();
    rTest_glGenTextures_zero_count();
    rTest_glGenTextures_null_textures();
    rTest_glGenTextures_large_n();
    rTest_glGenTextures_repeated_generation();
    rTest_glGenTextures_unique_names();
    rTest_glGenTextures_unbound_names_lifecycle();
    rTest_glGenTextures_double_delete();
    rTest_glGenTextures_huge_count_small_buffer();
    rTest_glGenTextures_name_reuse_after_delete();

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
