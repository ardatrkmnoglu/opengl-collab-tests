#include <stdio.h>
#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include "tests/BufferObjects/tests.h"
#include "tests/FramebufferObjects/frameBuffer_tests.h"

const char* vertexShaderSource = "#version 100\n"
"attribute vec3 aPos;\n"
"void main()\n{\ngl_Position = vec4(aPos, 1.0);\n}\n";

const char* fragmentShaderSource = "#version 100\n"
"precision mediump float;\n"
"void main()\n{\ngl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n}\n";

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

GLuint VBO;
GLuint shaderProgram;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void init(void);
void draw(void);
void cleanup(void);

int main(void)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL ES 2.0 Testleri", NULL, NULL);

    if (window == NULL)
    {
        printf("Pencere olusturulamadi!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLES2((GLADloadfunc)glfwGetProcAddress))
    {
        printf("GLAD yuklenemedi!\n");
        glfwTerminate();
        return -1;
        }

    init();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
    glfwTerminate();

    return 0;
}

void init(void)
{
    /* TESTLER */
    /* Test procedure'ları toplu çalıştıran init fonksiyonları örnek olarak yazılmıştır.
    Tek bir fonksiyonu da çağırabilirsiniz. Test isimlerini tests/../*.h veya tests/../*c dosyalarından bulabilirsiniz
    Örneğin Buffer Objects - Bind Buffer 001 testi için:
    GS_GL20SC_BO_BB_ROBUSTNESS_TC_001(); */

    /* --------------- BindRenderbuffer --------------- */
    // GS_GL20SC_FO_BR_ROBUSTNESS_TP_001_init();




    // Alttaki fonksiyonlar, entegre edilmiş fonksiyonlar. Onlarla ileri bittiğinde init içindeki checkerrors fonksiyonları yorum satırı kalsın,
    // test fonksiyonlarını yorum satırından çıkar, .hh dosyasında ...init() fonksiyonunu tanımla v aşağıdakileri init() fonksiyonu ile çağır.

    /* --------------- GenFramebuffer --------------- */
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_001();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_002();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_003();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_004();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_005();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_006();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_007();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_008();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_009();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_010();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_011();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_012();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_013();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_014();
    // GS_GL20SC_FO_GF_ROBUSTNESS_TC_015();

    /* --------------- BindFramebuffer --------------- */
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_001();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_002();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_003();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_004();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_005();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_006();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_007();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_008();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_009();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_010();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_011();
    // GS_GL20SC_FO_BF_ROBUSTNESS_TC_012();

    /* --------------- BindBuffer --------------- */
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_001();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_002();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_003();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_004();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_005();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_006();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_007();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_008();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_009();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_010();
    // GS_GL20SC_BO_BB_ROBUSTNESS_TC_011();

    /* --------------- BufferData --------------- */
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_001();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_002();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_003();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_004();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_005();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_006();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_007();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_008();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_009();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_010();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_011();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_012();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_013();
    // GS_GL20SC_BO_BD_ROBUSTNESS_TC_014();

    /* --------------- BufferSubData --------------- */
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_001();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_002();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_003();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_004();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_005();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_006();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_007();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_008();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_009();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_010();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_011();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_012();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_013();
    // GS_GL20SC_BO_BSD_ROBUSTNESS_TC_014();

    /* --------------- GenBuffers --------------- */
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_001();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_002();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_003();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_004();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_005();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_006();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_007();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_008();
    // GS_GL20SC_BO_GB_ROBUSTNESS_TC_009();

    /* --------------- glGetBufferParameteriv --------------- */
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_001();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_002();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_003();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_004();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_005();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_006();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_007();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_008();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_009();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_010();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_011();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_012();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_013();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_014();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_015();
    // GS_GL20SC_GBP_GB_ROBUSTNESS_TC_016();


    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex Shader Derleme Hatasi:\n%s\n", infoLog);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment Shader Derleme Hatasi:\n%s\n", infoLog);
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindAttribLocation(shaderProgram, 0, "aPos");
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader Programi Link Hatasi:\n%s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw(void)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void cleanup(void)
{
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}
