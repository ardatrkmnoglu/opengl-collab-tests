#ifndef HELPER_H
#define HELPER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "macro.h"
#include "../../../include/macro.h

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

#ifndef HELPER_H
#define HELPER_H

#include <GL/gl.h>

/* ============================================================
 * Ortak test altyapisi
 * Bu dosya, robustness test paketlerinde tekrar eden
 * yardimci fonksiyonlari tek noktada toplar.
 *
 * Durum kontrol fonksiyonlari basarisizlikta TEST_LOG_FAIL
 * basar ve 0 doner; basariliysa 1 doner.
 *
 * Kullanim:
 *
 *     if(!checkIntState(test_case_1, test_procedure,
 *                       GL_CULL_FACE_MODE, GL_BACK))
 *         return;
 * ============================================================ */

/* ---------- Hata kuyrugu ---------- */

/* OpenGL hata kuyrugunu tamamen bosaltir. */
void clearGLErrors(void);

/* ---------- Durum kontrolleri ---------- */

/* GL_VIEWPORT degerini beklenen dortlu ile karsilastirir.
   Kullananlar: Viewport, Flush, Finish */
int checkViewport(const char* test_case, const char* test_procedure,
                  GLint x, GLint y, GLsizei width, GLsizei height);

/* Tek elemanli tamsayi durum sorgusu.
   Kullananlar: FrontFace (GL_FRONT_FACE),
                CullFace  (GL_CULL_FACE_MODE),
                PixelStorei (GL_PACK_ALIGNMENT / GL_UNPACK_ALIGNMENT) */
int checkIntState(const char* test_case, const char* test_procedure,
                  GLenum pname, GLint expected);

/* Tek elemanli float durum sorgusu.
   Kullananlar: LineWidth (GL_LINE_WIDTH), GetError (GL_LINE_WIDTH) */
int checkFloatState(const char* test_case, const char* test_procedure,
                    GLenum pname, GLfloat expected, GLfloat tolerance);

/* Iki ayri float durumun ayni anda dogrulanmasi.
   Kullanan: PolygonOffset (factor + units) */
int checkFloatState2(const char* test_case, const char* test_procedure,
                     GLenum pnameA, GLfloat expectedA,
                     GLenum pnameB, GLfloat expectedB,
                     GLfloat tolerance);

/* Iki elemanli double durum sorgusu.
   Kullanan: DepthRange (GL_DEPTH_RANGE) */
int checkDoubleState2(const char* test_case, const char* test_procedure,
                      GLenum pname,
                      GLdouble expectedA, GLdouble expectedB,
                      GLdouble tolerance);

/* ---------- Durum sifirlama ---------- */

/*
 * Her test paketi farkli bir baslangic durumu gerektirdigi
 * icin sifirlama fonksiyonlari ayri ayri tanimlanmistir.
 * Hepsi islem sonunda hata kuyrugunu bosaltir.
 */

void resetState_Viewport(void);      /* glViewport(0,0,640,480)          */
void resetState_DepthRange(void);    /* glDepthRange(0.0,1.0)            */
void resetState_LineWidth(void);     /* glLineWidth(1.0f)                */
void resetState_FrontFace(void);     /* glFrontFace(GL_CCW)              */
void resetState_CullFace(void);      /* culling kapali, GL_CCW, GL_BACK  */
void resetState_PolygonO(void);      /* offset fill / depth test kapali  */
void resetState_PixelStorei(void);   /* pack / unpack alignment = 4      */

/* ---------- Yardimci ---------- */

/*
 * rand() bazi platformlarda yalnizca 15 bit uretir.
 * Tam 32-bit aralikta rastgele deger elde etmek icin
 * birden fazla cagri birlestirilir.
 */
GLint randInt32(void);

#endif /* HELPER_H */

