#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/*
 * ============================================================================
 * glTexSubImage2D Visual Robustness Test
 * Hasan - OpenGL ES 2.0
 *
 * Amac: VRAM'e statik olarak atilmis (buyuk bir texture) bellege,
 * oyun dongusu icerisinde surekli `glTexSubImage2D` ile mudahale ederek
 * (guncelleme yaparak) surucunun bunu ne kadar iyi tolere ettigini,
 * cokus veya gorsel bozulma olup olmadigini test etmek.
 * ============================================================================
 */

static const char* vertexShaderSource =
    "attribute vec2 inPosition;\n"
    "attribute vec2 inTexCoord;\n"
    "varying vec2 vTexCoord;\n"
    "void main()\n"
    "{\n"
    "   vTexCoord = inTexCoord;\n"
    "   gl_Position = vec4(inPosition, 0.0, 1.0);\n"
    "}\n";

static const char* fragmentShaderSource =
    "precision mediump float;\n"
    "varying vec2 vTexCoord;\n"
    "uniform sampler2D uTexture;\n"
    "void main()\n"
    "{\n"
    "   gl_FragColor = texture2D(uTexture, vTexCoord);\n"
    "}\n";

// Shader derleme yardimcisi
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Shader Derleme Hatasi:\n%s\n", infoLog);
    }
    return shader;
}

// Rastgele renk uretici
void fillRandomColors(GLubyte* data, int width, int height) {
    for (int i = 0; i < width * height * 4; i += 4) {
        data[i]     = rand() % 256; // R
        data[i + 1] = rand() % 256; // G
        data[i + 2] = rand() % 256; // B
        data[i + 3] = 255;          // A
    }
}

int main(void)
{
    // Cihazin gercek gucunu test etmek istenirse asagidaki iki satir kaldirilabilir.
    // Ancak macOS gibi ortamlarda sorun yasamamak icin llvmpipe aktif birakildi.
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    setenv("GALLIUM_DRIVER", "llvmpipe", 1);
    unsetenv("WAYLAND_DISPLAY");

    if (!glfwInit()) {
        printf("GLFW baslatilamadi!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "glTexSubImage2D Visual Tolerance Test", NULL, NULL);
    if (!window) {
        printf("Pencere olusturulamadi!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync
    srand(time(NULL));

    // Shader Programi Olustur
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glBindAttribLocation(shaderProgram, 0, "inPosition");
    glBindAttribLocation(shaderProgram, 1, "inTexCoord");
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Dortgen Verisi (Ekrani kaplayacak)
    float vertices[] = {
        // x, y,       u, v
        -0.8f, -0.8f,  0.0f, 0.0f,
         0.8f, -0.8f,  1.0f, 0.0f,
        -0.8f,  0.8f,  0.0f, 1.0f,
         0.8f,  0.8f,  1.0f, 1.0f
    };

    // 1. ANA TEXTURE (STATIK)
    // Buyuk bir alan ayiriyoruz (1024x1024). Bu islem normalde
    // GL_STATIC_DRAW gibi surucuye "bunu vram'e gom" mesaji verir.
    int texSize = 1024;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Baslangicta gri renk ile dolduralim
    GLubyte* initialData = (GLubyte*)malloc(texSize * texSize * 4);
    for(int i=0; i<texSize*texSize*4; i+=4) {
        initialData[i] = 50; initialData[i+1] = 50; initialData[i+2] = 50; initialData[i+3] = 255;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, initialData);
    free(initialData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Alt yama (SubImage) boyutu: Ekranda surekli guncellenecek kare
    int subSize = 256;
    GLubyte* subData = (GLubyte*)malloc(subSize * subSize * 4);

    double lastTime = glfwGetTime();
    int frames = 0;

    printf("Gorsel test basladi. Pencerde gri arka plan uzerinde rastgele renkli bir karenin surekli guncellendigini gormelisiniz.\n");

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();

        // Her saniyede bir FPS yazdir ve dokunun ortasini guncelle
        if (currentTime - lastTime >= 0.1) { 
            // VRAM'e "hatali/kaba" ulasim simulasyonu (surekli statik alana veri basiyoruz)
            fillRandomColors(subData, subSize, subSize);
            
            // Ortadan bir noktaya yama yapiyoruz
            int offsetX = (texSize - subSize) / 2;
            int offsetY = (texSize - subSize) / 2;

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, subSize, subSize, GL_RGBA, GL_UNSIGNED_BYTE, subData);

            lastTime = currentTime;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Geometriyi ciz
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), &vertices[0]);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), &vertices[2]);

        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Temizlik
    free(subData);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    return 0;
}
