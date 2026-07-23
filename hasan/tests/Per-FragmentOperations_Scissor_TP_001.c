#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

static const char* test_procedure = "Per-FragmentOperations_Scissor_TP_001";

/*
 * ============================================================================
 * glScissor & GL_SCISSOR_TEST Robustness Test Suite
 * Hasan - OpenGL ES 2.0
 *
 * Bu test paketi, Per-Fragment operasyonlarindan "Scissor Test" mekanizmasini
 * sinamak uzere yazilmistir. Fuzzing ve sinir testlerini icerir.
 * ============================================================================
 */

// ---------------------------------------------------------------
// TEST 1: Negatif Genislik ve Yukseklik (Spec Hatasi)
// Spec'e gore width veya height 0'dan kucukse GL_INVALID_VALUE uretilmelidir.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_001(void)
{
    static const char* test_case = "PerFragmentOperations_Scissor_TC_001";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_scissor_negative_dimensions\n", test_procedure, test_case);

    glScissor(0, 0, -1, 100);
    GLenum err1 = glGetError();

    glScissor(0, 0, 100, -50);
    GLenum err2 = glGetError();

    glScissor(0, 0, -1, -1);
    GLenum err3 = glGetError();

    printf("  Genislik -1 hatasi: 0x%X (Beklenen: 0x501 GL_INVALID_VALUE)\n", err1);
    printf("  Yukseklik -50 hatasi: 0x%X (Beklenen: 0x501)\n", err2);
    printf("  Ikisi de negatif hatasi: 0x%X (Beklenen: 0x501)\n", err3);

    if (err1 == GL_INVALID_VALUE && err2 == GL_INVALID_VALUE && err3 == GL_INVALID_VALUE)
        printf("  -> PASSED: Negatif boyutlar basariyla reddedildi\n\n");
    else
        printf("  -> FAILED\n\n");
}

// ---------------------------------------------------------------
// TEST 2: Negatif Koordinatlar (Gecerli Kullanim)
// Spec'e gore x (left) ve y (bottom) degerleri negatif OLABILIR.
// Bu durumda kutunun bir kismi ekran disinda kalir. Hata VERILMEMELIDIR.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_002(void)
{
    static const char* test_case = "PerFragmentOperations_Scissor_TC_002";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_scissor_negative_coordinates\n", test_procedure, test_case);

    glScissor(-100, -50, 200, 200);
    GLenum err = glGetError();

    GLint box[4];
    glGetIntegerv(GL_SCISSOR_BOX, box);

    printf("  Negatif (x,y) set hatasi: 0x%X (Beklenen: 0x0 GL_NO_ERROR)\n", err);
    printf("  Kaydedilen Box: x=%d, y=%d, w=%d, h=%d\n", box[0], box[1], box[2], box[3]);

    if (err == GL_NO_ERROR && box[0] == -100 && box[1] == -50)
        printf("  -> PASSED: Negatif x,y degerleri tolere edildi ve dogru kaydedildi\n\n");
    else
        printf("  -> FAILED\n\n");
}

// ---------------------------------------------------------------
// TEST 3: Asiri Buyuk Degerlerle (INT_MAX/INT_MIN) Sinir Zorlamasi
// Tamsayi tasmasi (integer overflow) yaratip surucunun cokup
// cokmedigini kontrol ederiz.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_003(void)
{
    static const char* test_case = "PerFragmentOperations_Scissor_TC_003";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_scissor_extreme_limits\n", test_procedure, test_case);

    // Boyutlar (w, h) negatif olamaz, bu yuzden onlara INT_MAX veriyoruz.
    // Koordinatlar (x, y) negatif olabilir.
    
    glScissor(INT_MIN, INT_MIN, INT_MAX, INT_MAX);
    GLenum err1 = glGetError();

    glScissor(INT_MAX, INT_MAX, INT_MAX, INT_MAX);
    GLenum err2 = glGetError();

    printf("  (INT_MIN, INT_MIN, INT_MAX, INT_MAX) hatasi: 0x%X\n", err1);
    printf("  (INT_MAX, INT_MAX, INT_MAX, INT_MAX) hatasi: 0x%X\n", err2);
    
    // Spec bu durumlarda hata tanimlamaz, ancak sistemin cokmemesi esastir.
    if (err1 == GL_NO_ERROR && err2 == GL_NO_ERROR)
        printf("  -> PASSED: Asiri buyuk limitler cokus yaratmadan islendi\n\n");
    else
        printf("  -> FAILED/WARNING: Beklenmeyen hata veya durum (0x%X)\n\n", err1 != GL_NO_ERROR ? err1 : err2);
}

// ---------------------------------------------------------------
// TEST 4: Sifir Boyutlu Scissor Kutusu
// Genislik veya yuksekligin tam 0 olmasi yasaldir.
// Hata vermemesi ve basariyla kaydedilmesi gerekir.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_004(void)
{
    static const char* test_case = "PerFragmentOperations_Scissor_TC_004";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_scissor_zero_dimensions\n", test_procedure, test_case);

    glScissor(10, 10, 0, 0);
    GLenum err = glGetError();
    
    GLint box[4];
    glGetIntegerv(GL_SCISSOR_BOX, box);

    if (err == GL_NO_ERROR && box[2] == 0 && box[3] == 0)
        printf("  -> PASSED: 0 boyutlu kutu basariyla islendi\n\n");
    else
        printf("  -> FAILED: Hata=0x%X, kaydedilen boyut: %dx%d\n\n", err, box[2], box[3]);
}

// ---------------------------------------------------------------
// TEST 5: Enable/Disable State Strese Sokma (State Thrashing)
// GL_SCISSOR_TEST ozelligini binlerce kez ard arda acip kapatarak
// surucunun durum (state) makinesinin bozulup bozulmadigini test eder.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_005(void)
{
    static const char* test_case = "PerFragmentOperations_Scissor_TC_005";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_scissor_enable_disable_thrash\n", test_procedure, test_case);

    int err_count = 0;
    for (int i = 0; i < 10000; i++) {
        glEnable(GL_SCISSOR_TEST);
        if (!glIsEnabled(GL_SCISSOR_TEST)) err_count++;

        glDisable(GL_SCISSOR_TEST);
        if (glIsEnabled(GL_SCISSOR_TEST)) err_count++;
    }

    GLenum err = glGetError();

    if (err == GL_NO_ERROR && err_count == 0)
        printf("  -> PASSED: 10,000 kez Enable/Disable hatasiz tamamlandi\n\n");
    else
        printf("  -> FAILED: Hata=0x%X, State uyusmazligi=%d kez\n\n", err, err_count);
}

// ---------------------------------------------------------------
// TEST 6: Rastgele Fuzzing (Rastgele glScissor Cagriları)
// Cok sayida tamamen rastgele (mantikli ve mantiksiz) glScissor cagirilarak
// bellek veya state bozulmasi tespiti yapilir.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_006(void)
{
    static const char* test_case = "PerFragmentOperations_Scissor_TC_006";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_scissor_random_fuzzing\n", test_procedure, test_case);

    srand(12345); // Sabit seed (tekrar edilebilirlik)

    for (int i = 0; i < 50000; i++) {
        GLint x = (rand() % 4000) - 2000; // -2000 ile 2000 arasi
        GLint y = (rand() % 4000) - 2000;
        
        // %10 ihtimalle negatif boyutlar göndererek GL_INVALID_VALUE tetiklet (kasten)
        GLsizei w = (rand() % 100 < 10) ? -rand() % 500 : rand() % 4000;
        GLsizei h = (rand() % 100 < 10) ? -rand() % 500 : rand() % 4000;

        glScissor(x, y, w, h);
        
        // Her 1000 adimda bir hata kuyrugunu temizle (cok birikmesin)
        if (i % 1000 == 0) {
            while (glGetError() != GL_NO_ERROR);
        }
    }

    // Dongu cokmeden bittiyse surucu saglamdir.
    printf("  -> PASSED: 50,000 rastgele glScissor cagrisi cokus olmadan tamamlandi\n\n");
}


// =========================================================
// ANA YAPI: init, draw, clean
// =========================================================

static GLFWwindow* window = NULL;
static int width = 800, height = 600;
static const char* windowTitle = "glScissor Robustness Tests - Hasan";

void init(void);
void draw(void);
void clean(void);

void init(void)
{
    printf("=====================================================\n");
    printf("  %s ROBUSTNESS SUITE - HASAN\n", test_procedure);
    printf("=====================================================\n\n");

    PerFragmentOperations_Scissor_TC_001();
    PerFragmentOperations_Scissor_TC_002();
    PerFragmentOperations_Scissor_TC_003();
    PerFragmentOperations_Scissor_TC_004();
    PerFragmentOperations_Scissor_TC_005();
    PerFragmentOperations_Scissor_TC_006();

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
