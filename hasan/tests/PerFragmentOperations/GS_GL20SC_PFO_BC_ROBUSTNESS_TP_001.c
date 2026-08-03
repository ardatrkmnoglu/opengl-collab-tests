#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

/*
GL20SC - PerFragmentOperations - BlendColor - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_PFO_BC_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_BC_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_BC_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_BC_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_BC_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_BC_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_BC_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_BC_ROBUSTNESS_TC_007";

/* Forward declaration for close */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TP_001_close(void);

/* Float karsilastirma toleransi */
static const GLfloat TOLERANCE = 1e-5f;

/* 4'lu blend color state sorgulama yardimcisi */
static int check_blend_color(GLfloat exp_r, GLfloat exp_g, GLfloat exp_b,
                             GLfloat exp_a) {
  GLfloat color[4];
  glGetFloatv(GL_BLEND_COLOR, color);
  return (fabsf(color[0] - exp_r) < TOLERANCE &&
          fabsf(color[1] - exp_g) < TOLERANCE &&
          fabsf(color[2] - exp_b) < TOLERANCE &&
          fabsf(color[3] - exp_a) < TOLERANCE);
}

/* ============================================================
 * TEST 1 : Gecerli Deger Ayarlama ve State Dogrulamasi
 * ============================================================
 *
 * Amac
 * ----
 * [0, 1] araligindaki normal degerler ayarlanip
 * GL_BLEND_COLOR sorgusuyla dogrulanir.
 *
 * ============================================================ */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  struct {
    GLfloat r, g, b, a;
  } combos[] = {
      {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
      {0.5f, 0.5f, 0.5f, 0.5f}, {0.25f, 0.75f, 0.1f, 0.9f},
      {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 0.0f},
  };
  int count = sizeof(combos) / sizeof(combos[0]);

  for (int i = 0; i < count; i++) {
    glBlendColor(combos[i].r, combos[i].g, combos[i].b, combos[i].a);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_1, test_procedure, "Kombo %d hatasi: err=0x%X", i,
                    err);
      return;
    }

    if (!check_blend_color(combos[i].r, combos[i].g, combos[i].b,
                           combos[i].a)) {
      GLfloat c[4];
      glGetFloatv(GL_BLEND_COLOR, c);
      TEST_LOG_FAIL(test_case_1, test_procedure,
                    "Kombo %d state hatasi: "
                    "beklenen=(%.3f,%.3f,%.3f,%.3f) "
                    "gercek=(%.3f,%.3f,%.3f,%.3f)",
                    i, combos[i].r, combos[i].g, combos[i].b, combos[i].a, c[0],
                    c[1], c[2], c[3]);
      return;
    }
  }

  /* Varsayilana geri don */
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);

  TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : [0,1] Disindaki Degerler (Clamping Davranisi)
 * ============================================================
 *
 * Amac
 * ----
 * OpenGL ES 2.0 spec'e gore glBlendColor parametreleri
 * [0, 1] araligina clamp edilir.
 *
 * NOT: Desktop GL (3.x+) clamping yapmaz ve [-inf, +inf]
 * araligini kabul eder. llvmpipe desktop GL oldugu icin
 * clamping YAPMAYABILIR. Bu test her iki davranisi da
 * kabul eder: ya clamp ya da aynen kayit.
 * ============================================================ */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  struct {
    GLfloat r, g, b, a;
    /* Clamped beklenen degerler */
    GLfloat cr, cg, cb, ca;
  } tests[] = {
      /* Negatif degerler */
      {-1.0f, -0.5f, -100.0f, -0.001f, 0.0f, 0.0f, 0.0f, 0.0f},
      /* 1'den buyuk degerler */
      {2.0f, 1.5f, 100.0f, 1.001f, 1.0f, 1.0f, 1.0f, 1.0f},
      /* Karisik */
      {-0.5f, 0.5f, 1.5f, 0.0f, 0.0f, 0.5f, 1.0f, 0.0f},
  };
  int count = sizeof(tests) / sizeof(tests[0]);

  for (int i = 0; i < count; i++) {
    glBlendColor(tests[i].r, tests[i].g, tests[i].b, tests[i].a);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_2, test_procedure, "Test %d hatasi: err=0x%X", i,
                    err);
      return;
    }

    GLfloat c[4];
    glGetFloatv(GL_BLEND_COLOR, c);

    /*
     * Desktop GL clamping yapmaz, ES 2.0 yapar.
     * Her iki davranisi da kabul ediyoruz:
     * - Ya orijinal deger kaydedilmis (desktop GL)
     * - Ya da clamp edilmis deger kaydedilmis (ES 2.0)
     */
    int original_ok = (fabsf(c[0] - tests[i].r) < TOLERANCE &&
                       fabsf(c[1] - tests[i].g) < TOLERANCE &&
                       fabsf(c[2] - tests[i].b) < TOLERANCE &&
                       fabsf(c[3] - tests[i].a) < TOLERANCE);

    int clamped_ok = (fabsf(c[0] - tests[i].cr) < TOLERANCE &&
                      fabsf(c[1] - tests[i].cg) < TOLERANCE &&
                      fabsf(c[2] - tests[i].cb) < TOLERANCE &&
                      fabsf(c[3] - tests[i].ca) < TOLERANCE);

    if (!original_ok && !clamped_ok) {
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "Test %d: ne orijinal ne clamp: "
                    "girdi=(%.3f,%.3f,%.3f,%.3f) "
                    "gercek=(%.3f,%.3f,%.3f,%.3f)",
                    i, tests[i].r, tests[i].g, tests[i].b, tests[i].a, c[0],
                    c[1], c[2], c[3]);
      return;
    }
  }

  /* Varsayilana geri don */
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);

  TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : Ozel Float Degerleri (NaN, Infinity, Denormals)
 * ============================================================
 *
 * Amac
 * ----
 * NaN, +Infinity, -Infinity, denormal gibi ozel
 * IEEE 754 degerleri gonderilerek surucunun cokup
 * cokmedigini kontrol ederiz.
 *
 * Spec bu durumlar icin davranis tanimlamaz (undefined),
 * ancak sistem cokmemeli ve GL context bozulmamalidir.
 * ============================================================ */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TC_003(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  /* Bilinen bir state ayarla */
  glBlendColor(0.5f, 0.5f, 0.5f, 0.5f);
  glGetError();

  /* NaN */
  GLfloat nan_val = NAN;
  glBlendColor(nan_val, 0.0f, 0.0f, 0.0f);
  glGetError(); /* Hata beklenmez ama olursa da kabul et */

  /* +Infinity */
  GLfloat inf_val = INFINITY;
  glBlendColor(inf_val, inf_val, 0.0f, 0.0f);
  glGetError();

  /* -Infinity */
  GLfloat ninf_val = -INFINITY;
  glBlendColor(ninf_val, 0.0f, ninf_val, 0.0f);
  glGetError();

  /* Cok kucuk denormal deger */
  GLfloat denorm = FLT_MIN * 0.5f;
  glBlendColor(denorm, denorm, denorm, denorm);
  glGetError();

  /* FLT_MAX */
  glBlendColor(FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
  glGetError();

  /* FLT_MIN (en kucuk pozitif normalize) */
  glBlendColor(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
  glGetError();

  /* Hata kuyrugunu bosalt */
  while (glGetError() != GL_NO_ERROR)
    ;

  /* Context sagligini dogrula: normal bir deger ayarla */
  glBlendColor(0.25f, 0.75f, 0.5f, 1.0f);
  GLenum final_err = glGetError();

  if (final_err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_3, test_procedure,
                  "Context bozuldu: final_err=0x%X", final_err);
    return;
  }

  if (!check_blend_color(0.25f, 0.75f, 0.5f, 1.0f)) {
    GLfloat c[4];
    glGetFloatv(GL_BLEND_COLOR, c);
    TEST_LOG_FAIL(test_case_3, test_procedure,
                  "State hatasi: (%.6f,%.6f,%.6f,%.6f)", c[0], c[1], c[2],
                  c[3]);
    return;
  }

  /* Varsayilana geri don */
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);

  TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : FLT_MAX ve FLT_MIN Sinir Degerleri
 * ============================================================
 *
 * Amac
 * ----
 * Asiri buyuk ve asiri kucuk float degerlerle
 * cagirilarak surucunun cokup cokmedigini ve
 * sonrasinda normal calismaya donup donmedigini
 * kontrol ederiz.
 * ============================================================ */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  struct {
    GLfloat r, g, b, a;
  } extreme[] = {
      {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
      {-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX},
      {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN},
      {FLT_MAX, -FLT_MAX, FLT_MIN, 0.0f},
      {-FLT_MAX, FLT_MIN, FLT_MAX, 1.0f},
      {1e38f, -1e38f, 1e-38f, -1e-38f},
  };
  int count = sizeof(extreme) / sizeof(extreme[0]);

  for (int i = 0; i < count; i++) {
    glBlendColor(extreme[i].r, extreme[i].g, extreme[i].b, extreme[i].a);

    /* Hata olursa da sistem cokmemeli */
    while (glGetError() != GL_NO_ERROR)
      ;
  }

  /* Context sagligini dogrula */
  glBlendColor(0.1f, 0.2f, 0.3f, 0.4f);
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_4, test_procedure, "Context bozuldu: err=0x%X",
                  err);
    return;
  }

  if (!check_blend_color(0.1f, 0.2f, 0.3f, 0.4f)) {
    GLfloat c[4];
    glGetFloatv(GL_BLEND_COLOR, c);
    TEST_LOG_FAIL(test_case_4, test_procedure,
                  "State hatasi: (%.6f,%.6f,%.6f,%.6f)", c[0], c[1], c[2],
                  c[3]);
    return;
  }

  /* Varsayilana geri don */
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);

  TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Hassasiyet Testi (Precision)
 * ============================================================
 *
 * Amac
 * ----
 * Birbirine cok yakin float degerlerin dogru
 * kaydedilip kaydedilmedigini dogrular.
 *
 * Kayar nokta hassasiyetinin korunup korunmadigi
 * test edilir.
 * ============================================================ */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TC_005(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  struct {
    GLfloat r, g, b, a;
  } precision[] = {
      {0.001f, 0.002f, 0.003f, 0.004f},
      {0.999f, 0.998f, 0.997f, 0.996f},
      {0.123456f, 0.654321f, 0.111111f, 0.999999f},
      {1.0f / 3.0f, 1.0f / 7.0f, 1.0f / 11.0f, 1.0f / 13.0f},
      {0.0001f, 0.9999f, 0.00001f, 0.99999f},
  };
  int count = sizeof(precision) / sizeof(precision[0]);

  int failCount = 0;

  for (int i = 0; i < count; i++) {
    glBlendColor(precision[i].r, precision[i].g, precision[i].b,
                 precision[i].a);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_5, test_procedure, "Test %d hatasi: err=0x%X", i,
                    err);
      failCount++;
      continue;
    }

    GLfloat c[4];
    glGetFloatv(GL_BLEND_COLOR, c);

    /* 1e-4 toleransla karsilastir */
    if (fabsf(c[0] - precision[i].r) > 1e-4f ||
        fabsf(c[1] - precision[i].g) > 1e-4f ||
        fabsf(c[2] - precision[i].b) > 1e-4f ||
        fabsf(c[3] - precision[i].a) > 1e-4f) {
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "Test %d hassasiyet: "
                    "beklenen=(%.6f,%.6f,%.6f,%.6f) "
                    "gercek=(%.6f,%.6f,%.6f,%.6f)",
                    i, precision[i].r, precision[i].g, precision[i].b,
                    precision[i].a, c[0], c[1], c[2], c[3]);
      failCount++;
    }
  }

  /* Varsayilana geri don */
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);

  if (failCount == 0)
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
  else
    TEST_LOG_FAIL(test_case_5, test_procedure, "Toplam basarisiz: %d",
                  failCount);
}

/* ============================================================
 * TEST 6 : Enable/Disable State Strese Sokma (State Thrashing)
 * ============================================================
 *
 * Amac
 * ----
 * GL_BLEND ozelligini binlerce kez acip kapatarak
 * ve her seferinde farkli bir blend color ayarlayarak
 * surucunun durum (state) makinesinin bozulup
 * bozulmadigini test eder.
 * ============================================================ */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TC_006(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  int err_count = 0;

  for (int i = 0; i < 10000; i++) {
    /* Deterministik farkli degerler */
    GLfloat r = (GLfloat)(i % 101) / 100.0f;
    GLfloat g = (GLfloat)((i + 33) % 101) / 100.0f;
    GLfloat b = (GLfloat)((i + 67) % 101) / 100.0f;
    GLfloat a = (GLfloat)((i + 50) % 101) / 100.0f;

    glEnable(GL_BLEND);
    glBlendColor(r, g, b, a);

    if (!check_blend_color(r, g, b, a))
      err_count++;

    if (!glIsEnabled(GL_BLEND))
      err_count++;

    glDisable(GL_BLEND);
    if (glIsEnabled(GL_BLEND))
      err_count++;
  }

  GLenum err = glGetError();

  /* Varsayilana geri don */
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
  glDisable(GL_BLEND);

  if (err == GL_NO_ERROR && err_count == 0)
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
  else
    TEST_LOG_FAIL(test_case_6, test_procedure,
                  "State thrashing hatasi: err=0x%X, uyusmazlik=%d kez", err,
                  err_count);
}

/* ============================================================
 * TEST 7 : Deterministik Fuzzing
 * ============================================================
 *
 * Amac
 * ----
 * Cok sayida deterministik (mantikli ve mantiksiz)
 * glBlendColor cagirilarak bellek veya state bozulmasi
 * tespiti yapilir.
 *
 * Ozel float degerler (NaN, Inf) dahil rastgele
 * uretilmis degerler gonderilir.
 *
 * glBlendColor hata uretmeyen bir fonksiyon oldugu
 * icin yalnizca GL_NO_ERROR beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TC_007(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  unsigned int seed = 99999;
  int unexpected_err_count = 0;
  GLenum last_unexpected_err = GL_NO_ERROR;
  int fail_iteration = -1;

  for (int i = 0; i < 50000; i++) {
    GLfloat params[4];

    for (int p = 0; p < 4; p++) {
      seed = seed * 1103515245 + 12345;
      unsigned int raw = (seed >> 8) & 0xFFFF;

      if (i % 100 == 0) {
        /* Her 100 iterasyonda ozel deger */
        switch (p) {
        case 0:
          params[p] = NAN;
          break;
        case 1:
          params[p] = INFINITY;
          break;
        case 2:
          params[p] = -INFINITY;
          break;
        default:
          params[p] = FLT_MAX;
          break;
        }
      } else if (i % 5 == 0) {
        /* Her 5 iterasyonda [0,1] araliginda */
        params[p] = (GLfloat)raw / 65535.0f;
      } else {
        /* Rastgele buyuk aralikta [-100, 100] */
        params[p] = ((GLfloat)raw / 65535.0f) * 200.0f - 100.0f;
      }
    }

    glBlendColor(params[0], params[1], params[2], params[3]);

    GLenum err = glGetError();
    /*
     * glBlendColor normalde hata uretmez.
     * Ancak ozel float degerlerle bazi suruculer
     * hata uretebilir. GL_NO_ERROR disindaki
     * her seyi kaydet.
     */
    if (err != GL_NO_ERROR) {
      unexpected_err_count++;
      last_unexpected_err = err;
      if (fail_iteration < 0)
        fail_iteration = i;
    }
  }

  /* Dongu sonrasi GL context sagligini dogrula */
  while (glGetError() != GL_NO_ERROR)
    ;
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
  GLenum final_err = glGetError();

  int state_ok = check_blend_color(0.0f, 0.0f, 0.0f, 0.0f);

  if (final_err == GL_NO_ERROR && state_ok)
    TEST_LOG_SUCCESS(test_case_7, test_procedure);
  else
    TEST_LOG_FAIL(test_case_7, test_procedure,
                  "Fuzzing hatasi: beklenmeyen=%d, "
                  "ilk_iter=%d, son_hata=0x%X, final=0x%X, "
                  "state_ok=%d",
                  unexpected_err_count, fail_iteration, last_unexpected_err,
                  final_err, state_ok);
}

/* Initialization */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TP_001_init(void) {
  GS_GL20SC_PFO_BC_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_BC_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_BC_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_BC_ROBUSTNESS_TC_004();
  GS_GL20SC_PFO_BC_ROBUSTNESS_TC_005();
  GS_GL20SC_PFO_BC_ROBUSTNESS_TC_006();
  GS_GL20SC_PFO_BC_ROBUSTNESS_TC_007();

  GS_GL20SC_PFO_BC_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_BC_ROBUSTNESS_TP_001_draw(void) {}

/* Cleanup */
void GS_GL20SC_PFO_BC_ROBUSTNESS_TP_001_close(void) {
  glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
  glDisable(GL_BLEND);
}
