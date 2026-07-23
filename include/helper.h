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

/* ============================================================
 * Test altyapisi
 *
 * resetState:
 * Her test oncesi viewport varsayilan degerlere dondurulur
 * ve hata kuyrugu temizlenir.
 *
 * checkStatePreserved:
 * Gecersiz glViewport cagrilarindan sonra viewport state'inin
 * degismedigini dogrular.
 *
 * randInt32:
 * rand() fonksiyonu bazi platformlarda yalnizca 15 bit
 * uretir. Tam 32-bit aralikta rastgele deger elde etmek
 * icin birden fazla cagri birlestirilir.
 * ============================================================ */

static void resetState_Viewport(void)
{
 glViewport(0,0,640,480);

 while(glGetError()!=GL_NO_ERROR);
}

static int checkStatePreserved_Viewport(const char* test_case,
                               GLint x,GLint y,
                               GLsizei width,GLsizei height)
{
 GLint viewport[4];

 glGetIntegerv(GL_VIEWPORT,viewport);

 if(viewport[0]!=x ||
    viewport[1]!=y ||
    viewport[2]!=width ||
    viewport[3]!=height)
 {
  TEST_LOG_FAIL(test_case, test_procedure,
                "Viewport durumu bozuldu. Beklenen: (%d,%d,%d,%d) Gercek: (%d,%d,%d,%d)",
                x,y,width,height,
                viewport[0],viewport[1],viewport[2],viewport[3]);
  return 0;
 }

 return 1;
}

static GLint randInt32_Viewport(void) {
 unsigned int value;

 value = ((unsigned int)rand() & 0x7FFFu);
 value = (value << 15) | ((unsigned int)rand() & 0x7FFFu);
 value = (value << 2)  | ((unsigned int)rand() & 0x3u);

 return (GLint)value;
}

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi/sonrasi OpenGL durumunu varsayilan
 * hale getirir ve hata kuyrugunu temizler.
 *
 * checkStatePreserved: Beklenen depth range durumunun
 * bozulmadigini dogrular.
 * ============================================================ */

static void resetState_DepthRange(void)
{
 glDepthRange(0.0,1.0);

 while(glGetError()!=GL_NO_ERROR);
}

static int checkStatePreserved_DepthRange(const char* test_case,
                               GLdouble expectedNear,GLdouble expectedFar)
{
 GLdouble depthRange[2];

 glGetDoublev(GL_DEPTH_RANGE,depthRange);

 if(fabs(depthRange[0]-expectedNear)>1e-6 ||
    fabs(depthRange[1]-expectedFar)>1e-6)
 {
  TEST_LOG_FAIL(test_case, test_procedure,
                "Depth range durumu bozuldu. Beklenen: (%lf,%lf) Gercek: (%lf,%lf)",
                expectedNear,expectedFar,depthRange[0],depthRange[1]);
  return 0;
 }

 return 1;
}

/* ============================================================
 * Test altyapısı
 *
 * resetState:
 *   Her testten önce OpenGL hata kuyruğu temizlenir.
 *
 * checkViewportPreserved:
 *   glFlush() çağrısından sonra OpenGL state'inin
 *   değişmediğini doğrular.
 * ============================================================
 */
static int checkViewportPreserved_Flush(const char* test_case,
                                  GLint x,GLint y,GLsizei width,GLsizei height)
{
 GLint viewport[4];

 glGetIntegerv(GL_VIEWPORT, viewport);

 if(viewport[0] != x || viewport[1] != y || viewport[2] != width ||viewport[3] != height)
 {
  TEST_LOG_FAIL(test_case, test_procedure,
                "Viewport state bozuldu. Beklenen: (%d,%d,%d,%d) Gercek: (%d,%d,%d,%d)",
                x, y, width, height,
                viewport[0], viewport[1], viewport[2], viewport[3]);
  return 0;
 }

 return 1;

 /* ============================================================
 * Test altyapısı
 *
 * resetState:
 *   Her testten önce OpenGL hata kuyruğu temizlenir.
 *
 * checkViewportPreserved:
 *   glFinish() çağrısından sonra OpenGL state'inin
 *   değişmediğini doğrular.
 * ============================================================
 */

 static int checkViewportPreserved_Finish(const char* test_case,
                                   GLint x,GLint y,GLsizei width,GLsizei height)
 {
  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);

  if(viewport[0] != x || viewport[1] != y || viewport[2] != width || viewport[3] != height)
  {
   TEST_LOG_FAIL(test_case, test_procedure,
                 "Viewport state bozuldu. Beklenen: (%d,%d,%d,%d) Gercek: (%d,%d,%d,%d)",
                 x, y, width, height,
                 viewport[0], viewport[1], viewport[2], viewport[3]);
   return 0;
  }

  return 1;
 }

 /* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine getirir ve birikmis hatalari
 * temizler; boylece testler birbirine bagimli olmaz.
 * checkStatePreserved: PolygonOffset durumunun beklenen
 * factor ve units degerlerini korudugunu dogrular.
 * ============================================================ */

 static void resetState_PolygonO(void)
 {
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_DEPTH_TEST);
  glPolygonOffset(0.0f, 0.0f);
  while (glGetError() != GL_NO_ERROR);
 }

 static int checkStatePreserved__PolygonO(const char* test_case, GLfloat expectedFactor, GLfloat expectedUnits)
 {
  GLfloat factor, units;

  glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &factor);
  glGetFloatv(GL_POLYGON_OFFSET_UNITS, &units);

  if (fabsf(factor - expectedFactor) > 1e-6f ||
      fabsf(units - expectedUnits) > 1e-6f) {

   TEST_LOG_FAIL(test_case, test_procedure,
                 "Durum bozuldu. factor: beklenen %.3f gercek %.3f, units: beklenen %.3f gercek %.3f",
                 expectedFactor, factor, expectedUnits, units);
   return 0;
      }

  return 1;
 }



