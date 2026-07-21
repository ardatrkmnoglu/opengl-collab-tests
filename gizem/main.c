#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "tests/tests.h"

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
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

    // --------------- glBufferSubData ---------------
    // rTest_glBufferSubData_invalid_enum_target();
    // rTest_glBufferSubData_invalid_value_negative_offset();
    // rTest_glBufferSubData_invalid_value_negative_size();
    // rTest_glBufferSubData_invalid_value_out_of_bounds();
    // rTest_glBufferSubData_invalid_operation_zero_buffer_bound();
    // rTest_glBufferSubData_offset_size_overflow_wraparound();
    // rTest_glBufferSubData_exact_boundary_offset();
    // rTest_glBufferSubData_negative_offset_compensating_size();
    // rTest_glBufferSubData_zero_size_null_data();
    // rTest_glBufferSubData_target_zero_bound();
    // rTest_glBufferSubData_into_zero_sized_store();
    // rTest_glBufferSubData_source_smaller_than_size();
    // rTest_glBufferSubData_dangling_data_pointer();
    // rTest_glBufferSubData_overlapping_misaligned_thrash();

    // --------------- glBufferData ---------------
    // rTest_glBufferData_invalid_enum_target();
    // rTest_glBufferData_invalid_enum_usage();
    // rTest_glBufferData_invalid_value_negative_size();
    // rTest_glBufferData_invalid_operation_zero_buffer_bound();
    // rTest_glBufferData_out_of_memory();
    // rTest_glBufferData_source_buffer_too_small();
    // rTest_glBufferData_zero_size_nonnull_data();
    // rTest_glBufferData_size_overflow_boundary();
    // rTest_glBufferData_dirty_usage_enum();
    // rTest_glBufferData_target_zero_bound();
    // rTest_glBufferData_repeated_resize_thrash();
    // rTest_glBufferData_misaligned_data_pointer();
    // rTest_glBufferData_dangling_data_pointer();
    // rTest_glBufferData_state_after_out_of_memory();

    // --------------- glBindBuffer ---------------
    // rTest_glBindBuffer_invalid_enum();
    // rTest_glBindBuffer_new_name_without_gen();
    // rTest_glBindBuffer_deleted_buffer();
    // rTest_glBindBuffer_boundary_handles();
    // rTest_glBindBuffer_dirty_high_bits_enum();
    // rTest_glBindBuffer_rapid_cross_target_rebind_stress();
    // rTest_glBindBuffer_delete_while_double_bound();
    // rTest_glBindBuffer_zero_binding_query_thrash();
    // rTest_glBindBuffer_massive_namespace_fuzz();
    // rTest_glBindBuffer_binding_churn_stress();
    // rTest_glBindBuffer_lifecycle_stress();

    // --------------- glGenBuffers ---------------
    // rTest_glGenBuffers_invalid_value();
    // rTest_glGenBuffers_zero_count();
    // rTest_glGenBuffers_null_buffers();
    // rTest_glGenBuffers_large_n();
    // rTest_glGenBuffers_repeated_generation();
    // rTest_glGenBuffers_unique_names();
    // rTest_glGenBuffers_unbound_names_lifecycle();
    // rTest_glGenBuffers_double_delete();
    // rTest_glGenBuffers_huge_count_small_buffer();


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
