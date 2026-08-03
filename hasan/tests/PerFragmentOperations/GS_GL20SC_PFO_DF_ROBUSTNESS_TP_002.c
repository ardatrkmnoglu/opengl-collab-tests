#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

/*
GL20SC - PerFragmentOperations - DepthFunc - ROBUSTNESS
Cizim ve pixel okuma ile gorsel dogrulama testleri.
*/

static const char *test_procedure = "GS_GL20SC_PFO_DF_ROBUSTNESS_TP_002";
static const char *test_case_1 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_004";

void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_002_close(void);

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
static const char *depth_vs_src =
    "attribute vec3 aPos;\n"
    "uniform float uDepth;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos.xy, uDepth, 1.0);\n"
    "}\n";

static const char *depth_fs_src =
    "precision mediump float;\n"
    "uniform vec4 uColor;\n"
    "void main() {\n"
    "    gl_FragColor = uColor;\n"
    "}\n";

static GLuint g_program = 0;
static GLint g_loc_depth = -1;
static GLint g_loc_color = -1;
static GLuint g_vbo = 0;

/* Tam ekran quad vertex'leri (iki ucgen) */
static const GLfloat quad_verts[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
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
  GLuint vs = compile_shader(GL_VERTEX_SHADER, depth_vs_src);
  GLuint fs = compile_shader(GL_FRAGMENT_SHADER, depth_fs_src);

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

  g_loc_depth = glGetUniformLocation(g_program, "uDepth");
  g_loc_color = glGetUniformLocation(g_program, "uColor");

  glGenBuffers(1, &g_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts,
               GL_STATIC_DRAW);

  return 1;
}

static void draw_quad(GLfloat depth, GLfloat r, GLfloat g, GLfloat b,
                      GLfloat a) {
  glUseProgram(g_program);
  glUniform1f(g_loc_depth, depth);
  glUniform4f(g_loc_color, r, g, b, a);

  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(0);
}

static int verify_center_pixel(GLubyte exp_r, GLubyte exp_g, GLubyte exp_b,
                                int tolerance) {
  GLubyte pixel[4] = {0};
  GLsizei buf_size = sizeof(pixel);

  glReadnPixels(400, 300, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buf_size, pixel);

  int dr = abs((int)pixel[0] - (int)exp_r);
  int dg = abs((int)pixel[1] - (int)exp_g);
  int db = abs((int)pixel[2] - (int)exp_b);

  if (dr > tolerance || dg > tolerance || db > tolerance) {
    TEST_LOG_INFO("Pixel: (%d,%d,%d) beklenen: (%d,%d,%d) tolerans: %d",
                  pixel[0], pixel[1], pixel[2], exp_r, exp_g, exp_b,
                  tolerance);
    return 0;
  }
  return 1;
}

// Test 1: GL_NEVER ve GL_ALWAYS ile cizim dogrulama
// GL_ALWAYS ile kirmizi quad gorulmeli.
// GL_NEVER ile yesil quad reddedilmeli, ekran siyah kalmali.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_quad(0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(255, 0, 0, 5)) {
    TEST_LOG_FAIL(test_case_1, test_procedure,
                  "GL_ALWAYS ile kirmizi quad gorunmedi");
    return;
  }

  glDepthFunc(GL_NEVER);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_quad(0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(0, 0, 0, 5)) {
    TEST_LOG_FAIL(test_case_1, test_procedure,
                  "GL_NEVER ile quad gorulmemeli ama goruldu");
    return;
  }

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_1, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

// Test 2: GL_LESS ve GL_GREATER ile derinlik sirasi dogrulama
// GL_LESS: yakin yesil (z=0.0) cizilir, uzak mavi (z=0.5) reddedilir.
// GL_GREATER: clear depth=0.0, yesil z=0.5 gecer, mavi z=0.8 ustune yazilir.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_quad(0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  draw_quad(0.5f, 0.0f, 0.0f, 1.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(0, 255, 0, 5)) {
    TEST_LOG_FAIL(test_case_2, test_procedure,
                  "GL_LESS: yesil gorulmeli, mavi reddedilmeli");
    return;
  }

  glDepthFunc(GL_GREATER);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepthf(0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_quad(0.5f, 0.0f, 1.0f, 0.0f, 1.0f);
  draw_quad(0.8f, 0.0f, 0.0f, 1.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(0, 0, 255, 5)) {
    TEST_LOG_FAIL(test_case_2, test_procedure,
                  "GL_GREATER: mavi gorulmeli");
    return;
  }

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_2, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

// Test 3: Depth test kapali iken cizim sirasiyla belirlenmeli
// Depth test devre disi birakildiginda z degeri onemsizdir.
// Sonra cizilen obje her zaman ustte gorulur.
// Uzak bir quad (z=0.99) cizilir, sonra yakin bir quad (z=0.01)
// uzerine cizilir. z degeri fark etmez, son cizilen gorulur.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_003(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  glDisable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Once kirmizi, uzak (z=0.99) */
  draw_quad(0.99f, 1.0f, 0.0f, 0.0f, 1.0f);
  /* Sonra yesil, yakin (z=0.01) - depth test kapali, son cizilen ustte */
  draw_quad(0.01f, 0.0f, 1.0f, 0.0f, 1.0f);
  glFinish();

  if (!verify_center_pixel(0, 255, 0, 5)) {
    TEST_LOG_FAIL(test_case_3, test_procedure,
                  "Depth test kapali: son cizilen (yesil) gorunmeli");
    return;
  }

  /* Ters sirada ciz: once yakin sonra uzak */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_quad(0.01f, 0.0f, 1.0f, 0.0f, 1.0f);
  draw_quad(0.99f, 1.0f, 0.0f, 0.0f, 1.0f);
  glFinish();

  /* Depth test kapali, son cizilen (kirmizi) gorulur */
  if (!verify_center_pixel(255, 0, 0, 5)) {
    TEST_LOG_FAIL(test_case_3, test_procedure,
                  "Depth test kapali: son cizilen (kirmizi) gorunmeli");
    return;
  }

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_3, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

// Test 4: Ayni depth'te 1000 kez ust uste cizim stresi
// GL_LEQUAL ile ayni z degerinde (z=0.0) 1000 kez renk
// degistirerek quad cizilir. Son renk gorulmeli.
// Z-fighting veya state bozulmasi olmamali.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* 1000 kez ayni z=0.0 ile ciz. Renk her seferinde degisir. */
  for (int i = 0; i < 1000; i++) {
    GLfloat r = (GLfloat)((i * 7) % 256) / 255.0f;
    GLfloat g = (GLfloat)((i * 13) % 256) / 255.0f;
    GLfloat b = (GLfloat)((i * 29) % 256) / 255.0f;
    draw_quad(0.0f, r, g, b, 1.0f);
  }
  glFinish();

  /* Son iterasyon (i=999) icin beklenen renk hesapla */
  GLubyte exp_r = (GLubyte)((999 * 7) % 256);
  GLubyte exp_g = (GLubyte)((999 * 13) % 256);
  GLubyte exp_b = (GLubyte)((999 * 29) % 256);

  if (!verify_center_pixel(exp_r, exp_g, exp_b, 5)) {
    TEST_LOG_FAIL(test_case_4, test_procedure,
                  "1000x ust uste cizim: son renk (%d,%d,%d) gorunmeli",
                  exp_r, exp_g, exp_b);
    return;
  }

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_4, test_procedure, "GL hatasi: 0x%X", err);
    return;
  }

  TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* Initialization */
void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_002_init(void) {
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

  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_004();

  GS_GL20SC_PFO_DF_ROBUSTNESS_TP_002_close();
}

void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_002_draw(void) {}

/* Cleanup */
void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_002_close(void) {
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
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(0);
}
