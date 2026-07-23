#ifndef HELPER_H
#define HELPER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* ------------------------------------------------------------------ *
 * createDummyProgram
 *
 * Minimalist bir vertex / fragment shader çifti derleyip linklayan
 * ve hazır GLuint program ID'si döndüren yardımcı fonksiyon.
 * "vPosition" attribute'u index 0'a bağlanmış olarak gelir.
 * ------------------------------------------------------------------ */
GLuint createDummyProgram(void);

/* ------------------------------------------------------------------ *
 * createContext
 *
 * Görünmez (GLFW_VISIBLE=GL_FALSE) bir GLFW penceresi açar,
 * OpenGL ES 2.0 bağlamını başlatır ve GLEW'i ilk haline getirir.
 * Başarı: 0 | Hata: -1
 * ------------------------------------------------------------------ */
int createContext(GLFWwindow **window);

/* ------------------------------------------------------------------ *
 * destroyContext
 *
 * GLFW penceresini yok eder ve glfwTerminate() çağırır.
 * ------------------------------------------------------------------ */
void destroyContext(GLFWwindow **window);

/* ------------------------------------------------------------------ *
 * cleanOpenGLState
 *
 * Testler arası izolasyon için OpenGL durumunu sıfırlar:
 *   - Aktif program → 0
 *   - GL_ARRAY_BUFFER ve GL_ELEMENT_ARRAY_BUFFER binding → 0
 *   - Attribute array'ler (0 ve 1) → disabled
 *   - Hata kuyruğu → boşaltılır
 * ------------------------------------------------------------------ */
void cleanOpenGLState(void);

#endif /* HELPER_H */
