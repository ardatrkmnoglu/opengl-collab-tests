#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <limits.h>

/*
 * GL20SC - PerFragmentOperations - StencilFunc - ROBUSTNESS
 *
 * glStencilFunc(GLenum func, GLint ref, GLuint mask) fonksiyonunun
 * OpenGL ES 2.0 / OpenGL SC 2.0 spesifikasyonuna uygunlugunu
 * dogrulayan robustness test paketi.
 *
 * Test edilen state sorgulari:
 *   GL_STENCIL_FUNC       -> func parametresi
 *   GL_STENCIL_REF        -> ref parametresi (clamped)
 *   GL_STENCIL_VALUE_MASK -> mask parametresi
 *
 * Gecerli func degerleri (0x0200 - 0x0207):
 *   GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL,
 *   GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS
 */

static const char *test_procedure = "GS_GL20SC_PFO_SF_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_SF_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_SF_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_SF_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_SF_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_SF_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_SF_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_SF_ROBUSTNESS_TC_007";

void GS_GL20SC_PFO_SF_ROBUSTNESS_TP_001_close(void);

/* Gecerli glStencilFunc karsilastirma modlari (0x0200 - 0x0207) */
static const GLenum valid_stencil_funcs[] = {
    GL_NEVER,    GL_LESS,     GL_EQUAL,    GL_LEQUAL,
    GL_GREATER,  GL_NOTEQUAL, GL_GEQUAL,   GL_ALWAYS,
};
static const int valid_stencil_func_count =
    sizeof(valid_stencil_funcs) / sizeof(valid_stencil_funcs[0]);

static int is_valid_stencil_func(GLenum val) {
  for (int i = 0; i < valid_stencil_func_count; i++) {
    if (val == valid_stencil_funcs[i])
      return 1;
  }
  return 0;
}

static GLint get_max_stencil_ref(void) {
  GLint stencil_bits = 0;
  glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
  if (stencil_bits <= 0)
    return 0;
  if (stencil_bits >= (GLint)(sizeof(GLint) * 8))
    return INT_MAX;
  return (GLint)((1u << stencil_bits) - 1u);
}

static GLint clamp_stencil_ref(GLint ref) {
  GLint max_ref = get_max_stencil_ref();
  if (ref < 0)
    return 0;
  if (ref > max_ref)
    return max_ref;
  return ref;
}

static int check_stencil_func(GLenum expected) {
  GLint val;
  glGetIntegerv(GL_STENCIL_FUNC, &val);
  return ((GLenum)val == expected);
}

static int check_stencil_ref(GLint expected) {
  GLint val;
  glGetIntegerv(GL_STENCIL_REF, &val);
  return (val == expected);
}

static int check_stencil_mask(GLuint expected) {
  GLint val;
  glGetIntegerv(GL_STENCIL_VALUE_MASK, &val);
  return ((GLuint)val == expected);
}

static int check_stencil_state(GLenum expected_func, GLint expected_ref,
                               GLuint expected_mask) {
  return check_stencil_func(expected_func) &&
         check_stencil_ref(expected_ref) &&
         check_stencil_mask(expected_mask);
}

/* ============================================================
 * TEST 1 : Gecersiz Enum Degerleri (Spec Hatasi)
 * ============================================================
 *
 * Amac
 * ----
 * Spec'e gore func parametresi yalnizca 8 karsilastirma
 * fonksiyonundan (GL_NEVER .. GL_ALWAYS) biri olmalidir.
 *
 * Bu araligin disindaki degerler icin GL_INVALID_ENUM
 * uretilmelidir. Gecersiz cagri sonrasinda func, ref ve
 * mask state'lerinin hicbirinin degismemesi beklenir.
 *
 * Denenen gecersiz degerler:
 *   0x0000, 0xDEAD, GL_BLEND, GL_NONE, 0x0208
 *   (0x0208 = gecerli araligin hemen disindaki deger)
 * ============================================================ */
void GS_GL20SC_PFO_SF_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  glStencilFunc(GL_LEQUAL, 7, 0xFF);
  glGetError();

  glStencilFunc(0x0000, 7, 0xFF);
  GLenum err1 = glGetError();

  glStencilFunc(0xDEAD, 7, 0xFF);
  GLenum err2 = glGetError();

  glStencilFunc(GL_BLEND, 7, 0xFF);
  GLenum err3 = glGetError();

  glStencilFunc(GL_NONE, 7, 0xFF);
  GLenum err4 = glGetError();

  glStencilFunc(0x0208, 7, 0xFF);
  GLenum err5 = glGetError();

  int state_ok = check_stencil_state(GL_LEQUAL, 7, 0xFF);

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

/* ============================================================
 * TEST 2 : Gecerli Modlarin Ayarlanmasi ve State Sorgusu
 * ============================================================
 *
 * Amac
 * ----
 * 8 gecerli karsilastirma fonksiyonunun her biri farkli
 * ref ve mask degerleriyle ayarlanir; ardindan
 * GL_STENCIL_FUNC, GL_STENCIL_REF ve GL_STENCIL_VALUE_MASK
 * sorgulariyla dogrulanir.
 *
 * Her gecerli cagri icin GL_NO_ERROR beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SF_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  int fail_count = 0;

  for (int i = 0; i < valid_stencil_func_count; i++) {
    GLint ref = i;
    GLuint mask = (GLuint)(0x11 * (i + 1));

    glStencilFunc(valid_stencil_funcs[i], ref, mask);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "func=0x%X reddedildi: err=0x%X", valid_stencil_funcs[i],
                    err);
      fail_count++;
      continue;
    }

    if (!check_stencil_state(valid_stencil_funcs[i], ref, mask)) {
      GLint actual_func, actual_ref, actual_mask;
      glGetIntegerv(GL_STENCIL_FUNC, &actual_func);
      glGetIntegerv(GL_STENCIL_REF, &actual_ref);
      glGetIntegerv(GL_STENCIL_VALUE_MASK, &actual_mask);
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "func=0x%X state hatasi: func=0x%X ref=%d mask=0x%X",
                    valid_stencil_funcs[i], actual_func, actual_ref,
                    actual_mask);
      fail_count++;
    }
  }

  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_2, test_procedure);
  else
    TEST_LOG_FAIL(test_case_2, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 3 : 16-bit Enum Sweep (Stress Sweep)
 * ============================================================
 *
 * Amac
 * ----
 * 16-bit GLenum uzayindaki tum degerler (0x0000..0xFFFF)
 * sistematik olarak denenir.
 *
 * Yalnizca 0x0200 - 0x0207 araligi GL_NO_ERROR uretmeli;
 * geri kalan tum degerler GL_INVALID_ENUM uretmelidir.
 *
 * Gecersiz cagrilarda func, ref ve mask state'lerinin
 * son gecerli degerlerde kalip kalmadigi kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_SF_ROBUSTNESS_TC_003(void) {
  GLenum mode;
  int pass_count = 0;
  int fail_count = 0;

  while (glGetError() != GL_NO_ERROR)
    ;

  glStencilFunc(GL_LESS, 3, 0xAA);
  glGetError();

  GLenum last_valid_func = GL_LESS;
  GLint last_ref = 3;
  GLuint last_mask = 0xAA;

  for (mode = 0; mode < 65536; mode++) {
    GLenum expected =
        is_valid_stencil_func(mode) ? GL_NO_ERROR : GL_INVALID_ENUM;

    glStencilFunc(mode, last_ref, last_mask);
    GLenum err = glGetError();

    if (err != expected) {
      TEST_LOG_FAIL(test_case_3, test_procedure,
                    "mode=0x%X beklenen=0x%X gelen=0x%X", mode, expected, err);
      fail_count++;
    } else {
      pass_count++;
    }

    if (err == GL_NO_ERROR)
      last_valid_func = mode;

    if (err == GL_INVALID_ENUM) {
      if (!check_stencil_state(last_valid_func, last_ref, last_mask)) {
        TEST_LOG_FAIL(test_case_3, test_procedure,
                      "State bozuldu: mode=0x%X func=0x%X ref=%d mask=0x%X",
                      mode, last_valid_func, last_ref, last_mask);
        return;
      }
    }
  }

  TEST_LOG_INFO("stencil func sweep - PASS: %d, FAIL: %d", pass_count,
                fail_count);

  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

  if (fail_count != 0) {
    TEST_LOG_FAIL(test_case_3, test_procedure, "Basarisiz: %d", fail_count);
    return;
  }

  TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : Asiri Buyuk Enum Degerleri
 * ============================================================
 *
 * Amac
 * ----
 * 16-bit sweep'in disinda kalan buyuk GLenum degerleri
 * (INT_MAX, INT_MIN, 0xFFFFFFFF, 0x80000000, 0x10000)
 * gonderilerek surucunun tasmasiz sekilde
 * GL_INVALID_ENUM uretip uretmedigi test edilir.
 *
 * Gecersiz cagri sonrasi state'in bozulmamasi beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SF_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLenum extreme[] = {(GLenum)INT_MAX, (GLenum)INT_MIN, (GLenum)0xFFFFFFFF,
                      (GLenum)0x80000000, (GLenum)0x10000};
  int count = sizeof(extreme) / sizeof(extreme[0]);

  glStencilFunc(GL_GEQUAL, 5, 0x55);
  glGetError();

  for (int i = 0; i < count; i++) {
    glStencilFunc(extreme[i], 5, 0x55);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_4, test_procedure,
                    "val=0x%X beklenen=GL_INVALID_ENUM gelen=0x%X", extreme[i],
                    err);
      return;
    }

    if (!check_stencil_state(GL_GEQUAL, 5, 0x55)) {
      TEST_LOG_FAIL(test_case_4, test_procedure, "State bozuldu: val=0x%X",
                    extreme[i]);
      return;
    }
  }

  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : ref Parametresi Clamping Davranisi
 * ============================================================
 *
 * Amac
 * ----
 * OpenGL ES 2.0 spec'e gore ref degeri
 * [0, 2^s - 1] araligina clamp edilir; burada s,
 * GL_STENCIL_BITS sorgusuyla alinan stencil buffer
 * bit derinligidir.
 *
 * Denenen degerler:
 *   0, max_ref          -> aynen kaydedilmeli
 *   -1, INT_MIN         -> 0'a clamp edilmeli
 *   max_ref+1, INT_MAX  -> max_ref'e clamp edilmeli
 *
 * Clamping sirasinda func ve mask state'lerinin
 * etkilenmemesi beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SF_ROBUSTNESS_TC_005(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLint max_ref = get_max_stencil_ref();
  GLint stencil_bits = 0;
  glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);

  struct {
    GLint input_ref;
    GLint expected_ref;
  } ref_tests[] = {
      {0, 0},
      {max_ref, max_ref},
      {-1, 0},
      {INT_MIN, 0},
      {max_ref + 1, max_ref},
      {INT_MAX, max_ref},
  };
  int ref_count = sizeof(ref_tests) / sizeof(ref_tests[0]);

  for (int i = 0; i < ref_count; i++) {
    glStencilFunc(GL_EQUAL, ref_tests[i].input_ref, 0xFF);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "ref=%d hata uretti: err=0x%X", ref_tests[i].input_ref,
                    err);
      return;
    }

    if (!check_stencil_ref(ref_tests[i].expected_ref)) {
      GLint actual_ref;
      glGetIntegerv(GL_STENCIL_REF, &actual_ref);
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "ref=%d beklenen=%d gercek=%d (stencil_bits=%d)",
                    ref_tests[i].input_ref, ref_tests[i].expected_ref,
                    actual_ref, stencil_bits);
      return;
    }

    if (!check_stencil_func(GL_EQUAL) || !check_stencil_mask(0xFF)) {
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "ref=%d sonrasi func/mask bozuldu", ref_tests[i].input_ref);
      return;
    }
  }

  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : mask Parametresi Sinir Degerleri
 * ============================================================
 *
 * Amac
 * ----
 * mask parametresi icin spec'te ust sinir tanimlanmaz;
 * tum bit desenleri gecerli kabul edilir.
 *
 * 0x00000000, 0x00000001, 0xFF, 0x00FF00FF,
 * 0xFFFFFFFF, 0xDEADBEEF, 0x80000000 gibi
 * sinir ve rastgele desenler gonderilir.
 *
 * Her cagri GL_NO_ERROR uretmeli ve GL_STENCIL_VALUE_MASK
 * sorgusuyla birebir eslesmelidir.
 * ============================================================ */
void GS_GL20SC_PFO_SF_ROBUSTNESS_TC_006(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLuint masks[] = {0x00000000, 0x00000001, 0x000000FF, 0x00FF00FF,
                    0xFFFFFFFF, 0xDEADBEEF, 0x80000000};
  int mask_count = sizeof(masks) / sizeof(masks[0]);

  for (int i = 0; i < mask_count; i++) {
    glStencilFunc(GL_LESS, 2, masks[i]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_6, test_procedure, "mask=0x%X err=0x%X",
                    masks[i], err);
      return;
    }

    if (!check_stencil_state(GL_LESS, 2, masks[i])) {
      GLint actual_mask;
      glGetIntegerv(GL_STENCIL_VALUE_MASK, &actual_mask);
      TEST_LOG_FAIL(test_case_6, test_procedure,
                    "mask=0x%X beklenen=0x%X gercek=0x%X", masks[i], masks[i],
                    actual_mask);
      return;
    }
  }

  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7 : State Thrashing + Deterministik Fuzzing
 * ============================================================
 *
 * Amac
 * ----
 * Surucunun state makinesinin yuk altinda bozulup
 * bozulmadigini iki asamada test eder:
 *
 * Asama 1 - State Thrashing (10.000 iterasyon):
 *   GL_STENCIL_TEST enable/disable dongusu icinde
 *   gecerli func/ref/mask kombinasyonlari ayarlanir.
 *   ref degeri spec'e gore clamp edildikten sonra
 *   (Asama 2 ile ayni mantik) state dogrulanir.
 *   mask icin stencil bit genisligine sigan degerler
 *   kullanilir (0xFF000000 gibi ust bitler bazi
 *   suruculerde sorguda korunmayabilir).
 *
 * Asama 2 - Deterministik Fuzzing (50.000 iterasyon):
 *   LCG ile uretilen rastgele func/ref/mask degerleri
 *   gonderilir. func icin yalnizca GL_INVALID_ENUM
 *   veya GL_NO_ERROR kabul edilir; ref icin clamp
 *   davranisi dogrulanir.
 *
 * Dongu sonunda context'in saglikli oldugu ve varsayilan
 * state'e geri donulebildigi kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_SF_ROBUSTNESS_TC_007(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  int err_count = 0;

  for (int i = 0; i < 10000; i++) {
    GLenum func = valid_stencil_funcs[i % valid_stencil_func_count];
    GLint ref = (GLint)(i % 256);
    GLuint mask = (GLuint)(0x11u * (unsigned)((i % 8) + 1));
    GLint expected_ref = clamp_stencil_ref(ref);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(func, ref, mask);

    if (!check_stencil_state(func, expected_ref, mask))
      err_count++;

    if (!glIsEnabled(GL_STENCIL_TEST))
      err_count++;

    glDisable(GL_STENCIL_TEST);
    if (glIsEnabled(GL_STENCIL_TEST))
      err_count++;
  }

  GLenum thrash_err = glGetError();

  if (thrash_err != GL_NO_ERROR || err_count != 0) {
    TEST_LOG_FAIL(test_case_7, test_procedure,
                  "Thrashing hatasi: err=0x%X uyusmazlik=%d", thrash_err,
                  err_count);
    return;
  }

  while (glGetError() != GL_NO_ERROR)
    ;

  unsigned int seed = 98765;
  int unexpected_err_count = 0;
  GLenum last_unexpected = GL_NO_ERROR;
  int first_fail = -1;

  for (int i = 0; i < 50000; i++) {
    seed = seed * 1103515245 + 12345;
    GLenum func;
    GLint ref;
    GLuint mask;

    if (i % 5 == 0) {
      func = valid_stencil_funcs[(seed >> 16) % valid_stencil_func_count];
    } else {
      func = (GLenum)((seed >> 16) % 0x10000);
    }

    seed = seed * 1103515245 + 12345;
    ref = (GLint)((seed >> 16) % 0x10000) - 1000;

    seed = seed * 1103515245 + 12345;
    mask = (GLuint)(seed >> 16);

    glStencilFunc(func, ref, mask);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
      unexpected_err_count++;
      last_unexpected = err;
      if (first_fail < 0)
        first_fail = i;
    }

    if (err == GL_NO_ERROR) {
      GLint expected_ref = clamp_stencil_ref(ref);

      if (!check_stencil_func(func) || !check_stencil_ref(expected_ref) ||
          !check_stencil_mask(mask))
        err_count++;
    }
  }

  while (glGetError() != GL_NO_ERROR)
    ;
  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  GLenum final_err = glGetError();
  int state_ok = check_stencil_state(GL_ALWAYS, 0, 0xFFFFFFFF);

  if (unexpected_err_count == 0 && err_count == 0 && final_err == GL_NO_ERROR &&
      state_ok)
    TEST_LOG_SUCCESS(test_case_7, test_procedure);
  else
    TEST_LOG_FAIL(test_case_7, test_procedure,
                  "Fuzzing hatasi: beklenmeyen=%d state=%d ilk=%d son=0x%X "
                  "final=0x%X state_ok=%d",
                  unexpected_err_count, err_count, first_fail, last_unexpected,
                  final_err, state_ok);
}

void GS_GL20SC_PFO_SF_ROBUSTNESS_TP_001_init(void) {
  GS_GL20SC_PFO_SF_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_SF_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_SF_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_SF_ROBUSTNESS_TC_004();
  GS_GL20SC_PFO_SF_ROBUSTNESS_TC_005();
  GS_GL20SC_PFO_SF_ROBUSTNESS_TC_006();
  GS_GL20SC_PFO_SF_ROBUSTNESS_TC_007();

  GS_GL20SC_PFO_SF_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_SF_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_PFO_SF_ROBUSTNESS_TP_001_close(void) {
  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  glDisable(GL_STENCIL_TEST);
}
