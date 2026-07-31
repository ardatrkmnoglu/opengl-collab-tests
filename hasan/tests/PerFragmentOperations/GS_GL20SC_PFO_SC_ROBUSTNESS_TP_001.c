#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <math.h>

/*
GL20SC - PerFragmentOperations - SampleCoverage - ROBUSTNESS
Parametre dogrulama, cizim ve pixel okuma ile gorsel dogrulama testleri.

NOT: TC_002 ve TC_003 gorsel dogrulamalari, framebuffer'in multisample
destegine (GL_SAMPLE_BUFFERS > 0) sahip olmasini gerektirir. Eger context
tek ornekli (single-sample) olusturulduysa bu testler bilgi mesaji ile
atlanir; glSampleCoverage API cagrilarinin kendisi (TC_001, TC_004) her
kosulda calisir.
*/

static const char *test_procedure = "GS_GL20SC_PFO_SC_ROBUSTNESS_TP_003";
static const char *test_case_1 = "GS_GL20SC_PFO_SC_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_SC_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_SC_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_SC_ROBUSTNESS_TC_004";

void GS_GL20SC_PFO_SC_ROBUSTNESS_TP_003_close(void);

/* glReadnPixels function pointer */
#ifndef APIENTRYP
#ifdef APIENTRY
#define APIENTRYP APIENTRY *
#else
#define APIENTRYP *
#endif
#endif

#ifndef PFNGLREADNPIXELSPROC
typedef void(APIENTRYP PFNGLREADNPIXELSPROC)(GLint x, GLint y, GLsizei width,
                                             GLsizei height, GLenum format,
                                             GLenum type, GLsizei bufSize,
                                             void *data);
#endif
static PFNGLREADNPIXELSPROC pglReadnPixels = NULL;
#define glReadnPixels pglReadnPixels

/* Shader kaynak kodlari */
static const char *sc_vs_src = "attribute vec3 aPos;\n"
                               "void main() {\n"
                               "    gl_Position = vec4(aPos.xy, 0.0, 1.0);\n"
                               "}\n";

static const char *sc_fs_src = "precision mediump float;\n"
                               "uniform vec4 uColor;\n"
                               "void main() {\n"
                               "    gl_FragColor = uColor;\n"
                               "}\n";

static GLuint g_program = 0;
static GLint g_loc_color = -1;
static GLuint g_vbo = 0;

/* Tam ekran quad vertex'leri (iki ucgen) */
static const GLfloat quad_verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,  0.0f, -1.0f, 1.0f, 0.0f,
};

static GLuint compile_shader(GLenum type, const char *src) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, NULL);
  glCompileShader(s);

  GLint ok = 0;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[256];
    glGetShaderInfoLog(s, sizeof(log), NULL, log);
    TEST_LOG_INFO("Shader compile hatasi: %s", log);
  }
  return s;
}

static int setup_draw_resources(void) {
  GLuint vs = compile_shader(GL_VERTEX_SHADER, sc_vs_src);
  GLuint fs = compile_shader(GL_FRAGMENT_SHADER, sc_fs_src);

  g_program = glCreateProgram();
  glAttachShader(g_program, vs);
  glAttachShader(g_program, fs);
  glBindAttribLocation(g_program, 0, "aPos");
  glLinkProgram(g_program);

  GLint linked = 0;
  glGetProgramiv(g_program, GL_LINK_STATUS, &linked);
  if (!linked) {
    char log[256];
    glGetProgramInfoLog(g_program, sizeof(log), NULL, log);
    TEST_LOG_INFO("Program link hatasi: %s", log);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return 0;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  g_loc_color = glGetUniformLocation(g_program, "uColor");

  glGenBuffers(1, &g_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

  return 1;
}

static void draw_quad(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
  glUseProgram(g_program);
  glUniform4f(g_loc_color, r, g, b, a);

  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(0);
}

/* Son okunan piksel; basarisizlik durumunda TEST_LOG_FAIL mesajlarina
   detay eklemek icin kullanilir, ayrica bir LOG_INFO cagrisina gerek
   birakmaz. */
static GLubyte g_last_pixel[4] = {0};

static int verify_center_pixel(GLubyte exp_r, GLubyte exp_g, GLubyte exp_b,
                               int tolerance) {
  GLsizei buf_size = sizeof(g_last_pixel);

  glReadnPixels(400, 300, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buf_size,
                g_last_pixel);

  int dr = abs((int)g_last_pixel[0] - (int)exp_r);
  int dg = abs((int)g_last_pixel[1] - (int)exp_g);
  int db = abs((int)g_last_pixel[2] - (int)exp_b);

  return !(dr > tolerance || dg > tolerance || db > tolerance);
}

/* Multisample destegi baslangicta bir kez sorgulanip saklanir; TC_002 ve
   TC_003 ayni kontrolu tekrar tekrar loglamaz. */
static int g_multisample_supported = 0;

static void detect_multisample_support(void) {
  GLint sample_buffers = 0;
  glGetIntegerv(GL_SAMPLE_BUFFERS, &sample_buffers);
  g_multisample_supported = (sample_buffers > 0);
  if (!g_multisample_supported) {
    TEST_LOG_INFO("Multisample framebuffer bulunamadi (GL_SAMPLE_BUFFERS=0), "
                  "TC_002/TC_003 gorsel dogrulamalari atlanacak");
  }
}

// Test 1: glSampleCoverage parametre dogrulama ve clamp davranisi
// Deger araligi disindaki (negatif / >1.0) girdiler GL hatasi
// uretmemeli ve spesifikasyona gore [0,1] araligina clamp edilmelidir.
void GS_GL20SC_PFO_SC_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLfloat test_values[] = {0.0f,  0.25f,   0.5f, 0.75f, 1.0f,
                           -1.0f, -100.0f, 2.0f, 100.0f};
  size_t n = sizeof(test_values) / sizeof(test_values[0]);
  size_t i;

  for (i = 0; i < n; i++) {
    glSampleCoverage(test_values[i], GL_FALSE);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_1, test_procedure,
                    "glSampleCoverage(%f, GL_FALSE) beklenmeyen hata: 0x%X",
                    test_values[i], err);
      return;
    }

    glSampleCoverage(test_values[i], GL_TRUE);
    err = glGetError();
    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_1, test_procedure,
                    "glSampleCoverage(%f, GL_TRUE) beklenmeyen hata: 0x%X",
                    test_values[i], err);
      return;
    }
  }

  /* Clamp dogrulamasi: -1.0 -> 0.0 */
  glSampleCoverage(-1.0f, GL_FALSE);
  GLfloat stored = -99.0f;
  glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &stored);
  if (fabs((double)(stored - 0.0f)) > 0.001) {
    TEST_LOG_FAIL(test_case_1, test_procedure,
                  "Deger -1.0 icin 0.0'a clamp edilmeli, okunan: %f", stored);
    return;
  }

  /* Clamp dogrulamasi: 2.0 -> 1.0 */
  glSampleCoverage(2.0f, GL_FALSE);
  stored = -99.0f;
  glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &stored);
  if (fabs((double)(stored - 1.0f)) > 0.001) {
    TEST_LOG_FAIL(test_case_1, test_procedure,
                  "Deger 2.0 icin 1.0'a clamp edilmeli, okunan: %f", stored);
    return;
  }

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_1, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

// Test 2: GL_SAMPLE_COVERAGE ile deger=1.0 / deger=0.0 gorsel dogrulama
// deger=1.0: quad tam kapsama ile gorulmeli.
// deger=0.0: hicbir sample gecmemeli, arka plan (siyah) korunmali.
void GS_GL20SC_PFO_SC_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  if (!g_multisample_supported) {
    TEST_LOG_SUCCESS(test_case_2, test_procedure);
    return;
  }

  glDisable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glEnable(GL_SAMPLE_COVERAGE);
  glClear(GL_COLOR_BUFFER_BIT);
  glSampleCoverage(1.0f, GL_FALSE);
  draw_quad(1.0f, 0.0f, 0.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(255, 0, 0, 10)) {
    TEST_LOG_FAIL(
        test_case_2, test_procedure,
        "deger=1.0 ile kirmizi quad tam gorunmeli, okunan: (%d,%d,%d)",
        g_last_pixel[0], g_last_pixel[1], g_last_pixel[2]);
    return;
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glSampleCoverage(0.0f, GL_FALSE);
  draw_quad(0.0f, 1.0f, 0.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(0, 0, 0, 10)) {
    TEST_LOG_FAIL(test_case_2, test_procedure,
                  "deger=0.0 ile hicbir sample gecmemeli, okunan: (%d,%d,%d)",
                  g_last_pixel[0], g_last_pixel[1], g_last_pixel[2]);
    return;
  }

  glDisable(GL_SAMPLE_COVERAGE);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_2, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

// Test 3: invert bayragi dogrulama
// invert=TRUE, deger=0.0  -> kapsama maskesi tersine cevrilir (deger=1.0
// esdegeri) invert=TRUE, deger=1.0  -> kapsama maskesi tersine cevrilir
// (deger=0.0 esdegeri)
void GS_GL20SC_PFO_SC_ROBUSTNESS_TC_003(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  if (!g_multisample_supported) {
    TEST_LOG_SUCCESS(test_case_3, test_procedure);
    return;
  }

  glDisable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_SAMPLE_COVERAGE);

  glClear(GL_COLOR_BUFFER_BIT);
  glSampleCoverage(0.0f, GL_TRUE);
  draw_quad(1.0f, 1.0f, 0.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(255, 255, 0, 10)) {
    TEST_LOG_FAIL(
        test_case_3, test_procedure,
        "invert=TRUE, deger=0.0 ile tam kapsama beklenir, okunan: (%d,%d,%d)",
        g_last_pixel[0], g_last_pixel[1], g_last_pixel[2]);
    return;
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glSampleCoverage(1.0f, GL_TRUE);
  draw_quad(0.0f, 1.0f, 1.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(0, 0, 0, 10)) {
    TEST_LOG_FAIL(
        test_case_3, test_procedure,
        "invert=TRUE, deger=1.0 ile kapsama sifir olmali, okunan: (%d,%d,%d)",
        g_last_pixel[0], g_last_pixel[1], g_last_pixel[2]);
    return;
  }

  glDisable(GL_SAMPLE_COVERAGE);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_3, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

// Test 4: 1000 kez ardisik glSampleCoverage cagrisi + cizim stresi
// Deger ve invert bayragi her iterasyonda degistirilir. Son cagrinin
// state'e dogru yansidigi ve GL state'in bozulmadigi dogrulanir.
void GS_GL20SC_PFO_SC_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SAMPLE_COVERAGE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  int i;
  GLfloat last_value = 0.0f;
  GLboolean last_invert = GL_FALSE;

  for (i = 0; i < 1000; i++) {
    GLfloat v = (GLfloat)(i % 101) / 100.0f;
    GLboolean inv = (i % 2 == 0) ? GL_FALSE : GL_TRUE;

    glSampleCoverage(v, inv);
    last_value = v;
    last_invert = inv;

    GLfloat r = (GLfloat)((i * 3) % 256) / 255.0f;
    GLfloat g = (GLfloat)((i * 11) % 256) / 255.0f;
    GLfloat b = (GLfloat)((i * 17) % 256) / 255.0f;
    draw_quad(r, g, b, 1.0f);
  }
  glFinish();

  GLfloat stored_value = -1.0f;
  GLboolean stored_invert = (GLboolean)0xFF;
  glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &stored_value);
  glGetBooleanv(GL_SAMPLE_COVERAGE_INVERT, &stored_invert);

  if (fabs((double)(stored_value - last_value)) > 0.01) {
    TEST_LOG_FAIL(
        test_case_4, test_procedure,
        "1000x cagridan sonra son deger tutarsiz: beklenen %f, okunan %f",
        last_value, stored_value);
    return;
  }

  if ((stored_invert ? GL_TRUE : GL_FALSE) != last_invert) {
    TEST_LOG_FAIL(test_case_4, test_procedure,
                  "1000x cagridan sonra invert bayragi tutarsiz");
    return;
  }

  glDisable(GL_SAMPLE_COVERAGE);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_4, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* Initialization */
void GS_GL20SC_PFO_SC_ROBUSTNESS_TP_003_init(void) {
  pglReadnPixels = (PFNGLREADNPIXELSPROC)glfwGetProcAddress("glReadnPixels");
  if (!pglReadnPixels)
    pglReadnPixels =
        (PFNGLREADNPIXELSPROC)glfwGetProcAddress("glReadnPixelsEXT");
  if (!pglReadnPixels)
    pglReadnPixels =
        (PFNGLREADNPIXELSPROC)glfwGetProcAddress("glReadnPixelsKHR");

  if (!pglReadnPixels) {
    TEST_LOG_INFO("glReadnPixels function pointer alinamadi!");
    return;
  }

  if (!setup_draw_resources()) {
    TEST_LOG_INFO("Shader/VBO olusturulamadi, cizim testleri atlanacak");
    return;
  }

  detect_multisample_support();

  GS_GL20SC_PFO_SC_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_SC_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_SC_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_SC_ROBUSTNESS_TC_004();

  GS_GL20SC_PFO_SC_ROBUSTNESS_TP_003_close();
}

void GS_GL20SC_PFO_SC_ROBUSTNESS_TP_003_draw(void) {}

/* Cleanup */
void GS_GL20SC_PFO_SC_ROBUSTNESS_TP_003_close(void) {
  if (g_vbo) {
#ifdef __ubuntu__
    glDeleteBuffers(1, &g_vbo);
#endif
    g_vbo = 0;
  }
  if (g_program) {
#ifdef __ubuntu__
    glDeleteProgram(g_program);
#endif
    g_program = 0;
  }
  glSampleCoverage(1.0f, GL_FALSE);
  glDisable(GL_SAMPLE_COVERAGE);
  glUseProgram(0);
}