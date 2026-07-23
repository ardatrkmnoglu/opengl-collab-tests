#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static const char* test_procedure = "Texturing_TexSubImage2D_TP_001";

// --- GLOBAL DEĞİŞKENLER (draw ve cleanup için) ---
GLFWwindow* window;
GLuint shaderProgram;
GLuint vbo;
GLuint texture_id;

const int BASE_TEX_WIDTH = 256;
const int BASE_TEX_HEIGHT = 256;

// --- SHADER KODLARI ---
const char* vertexShaderSource =
    "attribute vec2 position;\n"
    "attribute vec2 texCoord;\n"
    "varying vec2 v_texCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(position, 0.0, 1.0);\n"
    "   v_texCoord = texCoord;\n"
    "}\n";

const char* fragmentShaderSource =
    "precision mediump float;\n"
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "   gl_FragColor = texture2D(u_texture, v_texCoord);\n"
    "}\n";

// --- YARDIMCI FONKSİYONLAR ---
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

// OpenGL hata durumunu kontrol edip ekrana yazdıran fonksiyon
void check_gl_error(const char* test_name) {
    GLenum err = glGetError();
    printf("[%s] Test Sonucu: ", test_name);
    if (err == GL_NO_ERROR) {
        printf("HATA YOK (Beklenmeyen durum veya gecerli islem)\n");
    } else if (err == GL_INVALID_VALUE) {
        printf("BASARILI (GL_INVALID_VALUE yakalandi)\n");
    } else if (err == GL_INVALID_OPERATION) {
        printf("BASARILI (GL_INVALID_OPERATION yakalandi)\n");
    } else if (err == GL_INVALID_ENUM) {
        printf("BASARILI (GL_INVALID_ENUM yakalandi)\n");
    } else {
        printf("Bilinmeyen Hata Kodu: 0x%x\n", err);
    }
}

// --- ROBUSTNESS TEST FONKSİYONLARI ---

void Texturing_TexSubImage2D_TC_001(void) {
    static const char* test_case = "Texturing_TexSubImage2D_TC_001";
    printf("[TEST][%s][%s] Negatif Boyut Testi Basliyor...\n", test_procedure, test_case);
    uint8_t dummy_data[4] = {0};
    // Genişlik ve yükseklik negatif olamaz
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, -10, -10, GL_RGBA, GL_UNSIGNED_BYTE, dummy_data);
    check_gl_error("Negatif Boyut Testi");
}

void Texturing_TexSubImage2D_TC_002(void) {
    static const char* test_case = "Texturing_TexSubImage2D_TC_002";
    printf("[TEST][%s][%s] Sinir Asimi (Out of Bounds) Testi Basliyor...\n", test_procedure, test_case);
    uint8_t dummy_data[4] = {0};
    // xoffset (200) + width (100) = 300. Bu değer ana dokunun (256) dışına taşıyor.
    glTexSubImage2D(GL_TEXTURE_2D, 0, 200, 0, 100, 10, GL_RGBA, GL_UNSIGNED_BYTE, dummy_data);
    check_gl_error("Sinir Asimi (Out of Bounds) Testi");
}

void Texturing_TexSubImage2D_TC_003(void) {
    static const char* test_case = "Texturing_TexSubImage2D_TC_003";
    printf("[TEST][%s][%s] Format Uyusmazligi Testi Basliyor...\n", test_procedure, test_case);
    uint8_t dummy_data[4] = {0};
    // Ana doku GL_RGBA ve GL_UNSIGNED_BYTE olarak oluşturuldu.
    // Biz burada GL_RGB formatı göndererek uyuşmazlık yaratıyoruz.
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 10, 10, GL_RGB, GL_UNSIGNED_BYTE, dummy_data);
    check_gl_error("Format Uyusmazligi Testi");
}

void Texturing_TexSubImage2D_TC_004(void) {
    static const char* test_case = "Texturing_TexSubImage2D_TC_004";
    printf("[TEST][%s][%s] Gecersiz Hedef (Enum) Testi Basliyor...\n", test_procedure, test_case);
    uint8_t dummy_data[4] = {0};
    // Geçersiz bir target gönderiyoruz (GL_TEXTURE_2D yerine GL_POINTS)
    glTexSubImage2D(GL_POINTS, 0, 0, 0, 10, 10, GL_RGBA, GL_UNSIGNED_BYTE, dummy_data);
    check_gl_error("Gecersiz Hedef (Enum) Testi");
}

// --- TEMEL DÖNGÜ FONKSİYONLARI ---

int init(void) {
    // Çevre değişkenlerini ayarlama (Ubuntu vm için)
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    setenv("GALLIUM_DRIVER", "llvmpipe", 1);
    unsetenv("WAYLAND_DISPLAY");

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(512, 512, "Robustness Tests", NULL, NULL);
    if (!window) return -1;

    glfwMakeContextCurrent(window);

    // Shader ve VBO Kurulumu
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    float vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Ana Dokuyu Oluşturma (Siyah ve Opak)
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    uint8_t* base_data = (uint8_t*)calloc(BASE_TEX_WIDTH * BASE_TEX_HEIGHT * 4, sizeof(uint8_t));
    for(int i = 3; i < BASE_TEX_WIDTH * BASE_TEX_HEIGHT * 4; i += 4) {
        base_data[i] = 255;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BASE_TEX_WIDTH, BASE_TEX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, base_data);
    free(base_data);

    printf("--- ROBUSTNESS TESTLERI BASLIYOR (%s) ---\n", test_procedure);
    // Hata tamponunu temizlemek için öylesine bir glGetError çağırıyoruz
    glGetError();

    // Testleri Çağırma
    Texturing_TexSubImage2D_TC_001();
    Texturing_TexSubImage2D_TC_002();
    Texturing_TexSubImage2D_TC_003();
    Texturing_TexSubImage2D_TC_004();

    printf("--- TESTLER TAMAMLANDI ---\n\n");

    // Testlerden sonra ekranı doğru görebilmek için GEÇERLİ bir işlem yapıyoruz
    uint8_t* sub_data = (uint8_t*)malloc(64 * 64 * 4);
    for (int i = 0; i < 64 * 64 * 4; i += 4) {
        sub_data[i] = 255; sub_data[i+1] = 0; sub_data[i+2] = 0; sub_data[i+3] = 255;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 50, 50, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, sub_data);
    free(sub_data);

    return 0;
}

void draw(void) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void cleanup(void) {
    printf("Temizlik islemleri yapiliyor...\n");
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &texture_id);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(void) {
    if (init() != 0) {
        printf("Baslatma (Init) hatasi!\n");
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
    return 0;
}
