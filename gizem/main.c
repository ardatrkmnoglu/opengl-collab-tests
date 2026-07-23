#include <stdio.h>
#include <glad/gles2.h>
#include <GLFW/glfw3.h>
#include "tests/BufferObjects/tests.h"
#include "tests/Framebuffer Objects/frameBuffer_tests.h"

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





    /* --------------- GenFramebuffer --------------- */
    // FramebufferObjects_GenFramebuffers_TC_001();
    // FramebufferObjects_GenFramebuffers_TC_002();
    // FramebufferObjects_GenFramebuffers_TC_003();
    // FramebufferObjects_GenFramebuffers_TC_004();
    // FramebufferObjects_GenFramebuffers_TC_005();
    // FramebufferObjects_GenFramebuffers_TC_006();
    // FramebufferObjects_GenFramebuffers_TC_007();
    // FramebufferObjects_GenFramebuffers_TC_008();
    // FramebufferObjects_GenFramebuffers_TC_009();
    // FramebufferObjects_GenFramebuffers_TC_010();
    // FramebufferObjects_GenFramebuffers_TC_011();
    // FramebufferObjects_GenFramebuffers_TC_012();
    // FramebufferObjects_GenFramebuffers_TC_013();
    // FramebufferObjects_GenFramebuffers_TC_014();
    // FramebufferObjects_GenFramebuffers_TC_015();

    /* --------------- BindFramebuffer --------------- */
    // FramebufferObjects_BindFramebuffer_TC_001();
    // FramebufferObjects_BindFramebuffer_TC_002();
    // FramebufferObjects_BindFramebuffer_TC_003();
    // FramebufferObjects_BindFramebuffer_TC_004();
    // FramebufferObjects_BindFramebuffer_TC_005();
    // FramebufferObjects_BindFramebuffer_TC_006();
    // FramebufferObjects_BindFramebuffer_TC_007();
    // FramebufferObjects_BindFramebuffer_TC_008();
    // FramebufferObjects_BindFramebuffer_TC_009();
    // FramebufferObjects_BindFramebuffer_TC_010();
    // FramebufferObjects_BindFramebuffer_TC_011();
    // FramebufferObjects_BindFramebuffer_TC_012();







    /* --------------- BindBuffer --------------- */
    // BufferObjects_BindBuffer_TC_001();
    // BufferObjects_BindBuffer_TC_002();
    // BufferObjects_BindBuffer_TC_003();
    // BufferObjects_BindBuffer_TC_004();
    // BufferObjects_BindBuffer_TC_005();
    // BufferObjects_BindBuffer_TC_006();
    // BufferObjects_BindBuffer_TC_007();
    // BufferObjects_BindBuffer_TC_008();
    // BufferObjects_BindBuffer_TC_009();
    // BufferObjects_BindBuffer_TC_010();
    // BufferObjects_BindBuffer_TC_011();

    /* --------------- BufferData --------------- */
    // BufferObjects_BufferData_TC_001();
    // BufferObjects_BufferData_TC_002();
    // BufferObjects_BufferData_TC_003();
    // BufferObjects_BufferData_TC_004();
    // BufferObjects_BufferData_TC_005();
    // BufferObjects_BufferData_TC_006();
    // BufferObjects_BufferData_TC_007();
    // BufferObjects_BufferData_TC_008();
    // BufferObjects_BufferData_TC_009();
    // BufferObjects_BufferData_TC_010();
    // BufferObjects_BufferData_TC_011();
    // BufferObjects_BufferData_TC_012();
    // BufferObjects_BufferData_TC_013();
    // BufferObjects_BufferData_TC_014();

    /* --------------- BufferSubData --------------- */
    // BufferObjects_BufferSubData_TC_001();
    // BufferObjects_BufferSubData_TC_002();
    // BufferObjects_BufferSubData_TC_003();
    // BufferObjects_BufferSubData_TC_004();
    // BufferObjects_BufferSubData_TC_005();
    // BufferObjects_BufferSubData_TC_006();
    // BufferObjects_BufferSubData_TC_007();
    // BufferObjects_BufferSubData_TC_008();
    // BufferObjects_BufferSubData_TC_009();
    // BufferObjects_BufferSubData_TC_010();
    // BufferObjects_BufferSubData_TC_011();
    // BufferObjects_BufferSubData_TC_012();
    // BufferObjects_BufferSubData_TC_013();
    // BufferObjects_BufferSubData_TC_014();

    /* --------------- GenBuffers --------------- */
    // BufferObjects_GenBuffers_TC_001();
    // BufferObjects_GenBuffers_TC_002();
    // BufferObjects_GenBuffers_TC_003();
    // BufferObjects_GenBuffers_TC_004();
    // BufferObjects_GenBuffers_TC_005();
    // BufferObjects_GenBuffers_TC_006();
    // BufferObjects_GenBuffers_TC_007();
    // BufferObjects_GenBuffers_TC_008();
    // BufferObjects_GenBuffers_TC_009();

    /* --------------- glGetBufferParameteriv --------------- */
    // BufferObjects_GetBufferParameteriv_TC_001();
    // BufferObjects_GetBufferParameteriv_TC_002();
    // BufferObjects_GetBufferParameteriv_TC_003();
    // BufferObjects_GetBufferParameteriv_TC_004();
    // BufferObjects_GetBufferParameteriv_TC_005();
    // BufferObjects_GetBufferParameteriv_TC_006();
    // BufferObjects_GetBufferParameteriv_TC_007();
    // BufferObjects_GetBufferParameteriv_TC_008();
    // BufferObjects_GetBufferParameteriv_TC_009();
    // BufferObjects_GetBufferParameteriv_TC_010();
    // BufferObjects_GetBufferParameteriv_TC_011();
    // BufferObjects_GetBufferParameteriv_TC_012();
    // BufferObjects_GetBufferParameteriv_TC_013();
    // BufferObjects_GetBufferParameteriv_TC_014();
    // BufferObjects_GetBufferParameteriv_TC_015();
    // BufferObjects_GetBufferParameteriv_TC_016();


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
