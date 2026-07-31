#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

/*
GL20SC - PerFragmentOperations - DepthFunc - ROBUSTNESS
State ve enum dogruluk testleri.
*/

static const char *test_procedure = "GS_GL20SC_PFO_DF_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_DF_ROBUSTNESS_TC_005";

void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_001_close(void);

/* Gecerli glDepthFunc modlari (0x0200 - 0x0207) */
static const GLenum valid_depth_funcs[] = {
    GL_NEVER,   GL_LESS,    GL_EQUAL,  GL_LEQUAL,
    GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS,
};
static const int valid_depth_func_count =
    sizeof(valid_depth_funcs) / sizeof(valid_depth_funcs[0]);

static int is_valid_depth_func(GLenum val) {
  for (int i = 0; i < valid_depth_func_count; i++) {
    if (val == valid_depth_funcs[i])
      return 1;
  }
  return 0;
}

/* Depth func state sorgulama */
static int check_depth_func(GLenum expected) {
  GLint val;
  glGetIntegerv(GL_DEPTH_FUNC, &val);
  return ((GLenum)val == expected);
}

// Test 1: Gecersiz enum reddi ve state korunmasi
// Gecersiz enum degerleri gondererek GL_INVALID_ENUM beklenir.
// Onceden ayarlanan GL_LEQUAL degismemis olmali.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  glDepthFunc(GL_LEQUAL);
  glGetError();

  glDepthFunc(0x0000);
  GLenum err1 = glGetError();

  glDepthFunc(0xDEAD);
  GLenum err2 = glGetError();

  glDepthFunc(GL_BLEND);
  GLenum err3 = glGetError();

  glDepthFunc(GL_NONE);
  GLenum err4 = glGetError();

  /* 0x0208 - gecerli aralığın hemen disinda */
  glDepthFunc(0x0208);
  GLenum err5 = glGetError();

  int state_ok = check_depth_func(GL_LEQUAL);

  if (err1 == GL_INVALID_ENUM && err2 == GL_INVALID_ENUM &&
      err3 == GL_INVALID_ENUM && err4 == GL_INVALID_ENUM &&
      err5 == GL_INVALID_ENUM && state_ok)
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
  else
    TEST_LOG_FAIL(test_case_1, test_procedure,
                  "err1=0x%X err2=0x%X err3=0x%X err4=0x%X err5=0x%X "
                  "state_ok=%d",
                  err1, err2, err3, err4, err5, state_ok);
}

// Test 2: Tum gecerli modlari ayarla ve sorgula
// 8 gecerli depth func modunun her biri ayarlanip geri okunur.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  int fail_count = 0;

  for (int i = 0; i < valid_depth_func_count; i++) {
    glDepthFunc(valid_depth_funcs[i]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "func=0x%X reddedildi: err=0x%X", valid_depth_funcs[i],
                    err);
      fail_count++;
      continue;
    }

    if (!check_depth_func(valid_depth_funcs[i])) {
      GLint actual;
      glGetIntegerv(GL_DEPTH_FUNC, &actual);
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "func=0x%X state hatasi: actual=0x%X",
                    valid_depth_funcs[i], actual);
      fail_count++;
    }
  }

  glDepthFunc(GL_LESS);

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_2, test_procedure);
  else
    TEST_LOG_FAIL(test_case_2, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

// Test 3: 16-bit enum sweep (0x0000..0xFFFF)
// Sadece 0x0200-0x0207 arasi GL_NO_ERROR, geri kalan
// tum degerler GL_INVALID_ENUM uretmeli.
// Gecersiz cagirilarda state bozulmamali.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_003(void) {
  GLenum mode;
  int pass_count = 0;
  int fail_count = 0;

  while (glGetError() != GL_NO_ERROR)
    ;

  glDepthFunc(GL_LESS);
  glGetError();

  GLint last_valid = GL_LESS;

  for (mode = 0; mode < 65536; mode++) {
    GLenum expected =
        is_valid_depth_func(mode) ? GL_NO_ERROR : GL_INVALID_ENUM;

    glDepthFunc(mode);
    GLenum err = glGetError();

    if (err != expected) {
      TEST_LOG_FAIL(test_case_3, test_procedure,
                    "mode=0x%X beklenen=0x%X gelen=0x%X", mode, expected, err);
      fail_count++;
    } else {
      pass_count++;
    }

    if (err == GL_NO_ERROR)
      last_valid = (GLint)mode;

    if (err == GL_INVALID_ENUM) {
      GLint actual;
      glGetIntegerv(GL_DEPTH_FUNC, &actual);
      if (actual != last_valid) {
        TEST_LOG_FAIL(test_case_3, test_procedure,
                      "State bozuldu: mode=0x%X beklenen=0x%X gercek=0x%X",
                      mode, last_valid, actual);
        return;
      }
    }
  }

  TEST_LOG_INFO("depth func sweep - PASS: %d, FAIL: %d", pass_count,
                fail_count);

  glDepthFunc(GL_LESS);

  if (fail_count != 0) {
    TEST_LOG_FAIL(test_case_3, test_procedure, "Basarisiz: %d", fail_count);
    return;
  }

  TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

// Test 4: Asiri buyuk enum degerleri (INT_MAX, INT_MIN, 0xFFFFFFFF, vb.)
// Hepsi GL_INVALID_ENUM uretmeli, state bozulmamali.
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLenum extreme[] = {(GLenum)INT_MAX, (GLenum)INT_MIN, (GLenum)0xFFFFFFFF,
                      (GLenum)0x80000000, (GLenum)0x10000};
  int count = sizeof(extreme) / sizeof(extreme[0]);

  glDepthFunc(GL_GEQUAL);
  glGetError();

  for (int i = 0; i < count; i++) {
    glDepthFunc(extreme[i]);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_4, test_procedure,
                    "val=0x%X beklenen=GL_INVALID_ENUM gelen=0x%X", extreme[i],
                    err);
      return;
    }

    if (!check_depth_func(GL_GEQUAL)) {
      TEST_LOG_FAIL(test_case_4, test_procedure, "State bozuldu: val=0x%X",
                    extreme[i]);
      return;
    }
  }

  glDepthFunc(GL_LESS);
  TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

// Test 5: State thrashing + deterministik fuzzing
// Ilk kisim: 10000 iterasyon enable/disable + gecerli func degisimi
// Ikinci kisim: 50000 iterasyonluk LCG fuzzing
void GS_GL20SC_PFO_DF_ROBUSTNESS_TC_005(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  /* Kisim 1: State thrashing */
  int err_count = 0;

  for (int i = 0; i < 10000; i++) {
    GLenum func = valid_depth_funcs[i % valid_depth_func_count];

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(func);

    if (!check_depth_func(func))
      err_count++;

    if (!glIsEnabled(GL_DEPTH_TEST))
      err_count++;

    glDisable(GL_DEPTH_TEST);
    if (glIsEnabled(GL_DEPTH_TEST))
      err_count++;
  }

  GLenum thrash_err = glGetError();

  if (thrash_err != GL_NO_ERROR || err_count != 0) {
    TEST_LOG_FAIL(test_case_5, test_procedure,
                  "Thrashing hatasi: err=0x%X uyusmazlik=%d", thrash_err,
                  err_count);
    return;
  }

  /* Kisim 2: Deterministik fuzzing */
  while (glGetError() != GL_NO_ERROR)
    ;

  unsigned int seed = 54321;
  int unexpected_err_count = 0;
  GLenum last_unexpected = GL_NO_ERROR;
  int first_fail = -1;

  for (int i = 0; i < 50000; i++) {
    seed = seed * 1103515245 + 12345;
    GLenum val;

    if (i % 5 == 0) {
      val = valid_depth_funcs[(seed >> 16) % valid_depth_func_count];
    } else {
      val = (GLenum)((seed >> 16) % 0x10000);
    }

    glDepthFunc(val);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
      unexpected_err_count++;
      last_unexpected = err;
      if (first_fail < 0)
        first_fail = i;
    }
  }

  while (glGetError() != GL_NO_ERROR)
    ;
  glDepthFunc(GL_LESS);
  GLenum final_err = glGetError();
  int state_ok = check_depth_func(GL_LESS);

  if (unexpected_err_count == 0 && final_err == GL_NO_ERROR && state_ok)
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
  else
    TEST_LOG_FAIL(test_case_5, test_procedure,
                  "Fuzzing hatasi: beklenmeyen=%d ilk=%d son=0x%X "
                  "final=0x%X state_ok=%d",
                  unexpected_err_count, first_fail, last_unexpected, final_err,
                  state_ok);
}

/* Initialization */
void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_001_init(void) {
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_004();
  GS_GL20SC_PFO_DF_ROBUSTNESS_TC_005();

  GS_GL20SC_PFO_DF_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_001_draw(void) {}

/* Cleanup */
void GS_GL20SC_PFO_DF_ROBUSTNESS_TP_001_close(void) {
  glDepthFunc(GL_LESS);
  glDisable(GL_DEPTH_TEST);
}
