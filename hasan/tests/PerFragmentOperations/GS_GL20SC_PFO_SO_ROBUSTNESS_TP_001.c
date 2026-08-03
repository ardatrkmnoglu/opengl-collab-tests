#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <limits.h>

/*
 * GL20SC - PerFragmentOperations - StencilOp - ROBUSTNESS
 *
 * glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass) fonksiyonunun
 * OpenGL ES 2.0 / OpenGL SC 2.0 spesifikasyonuna uygunlugunu
 * dogrulayan robustness test paketi.
 *
 * Test edilen state sorgulari:
 *   GL_STENCIL_FAIL                 -> sfail  parametresi (on yuz)
 *   GL_STENCIL_PASS_DEPTH_FAIL      -> dpfail parametresi (on yuz)
 *   GL_STENCIL_PASS_DEPTH_PASS      -> dppass parametresi (on yuz)
 *   GL_STENCIL_BACK_FAIL            -> sfail  parametresi (arka yuz)
 *   GL_STENCIL_BACK_PASS_DEPTH_FAIL -> dpfail parametresi (arka yuz)
 *   GL_STENCIL_BACK_PASS_DEPTH_PASS -> dppass parametresi (arka yuz)
 *
 * glStencilOp, glStencilOpSeparate(GL_FRONT_AND_BACK, ...) ile
 * esdegerdir; bu nedenle her cagri hem on hem arka yuz
 * state'ini ayni anda guncellemelidir.
 *
 * Gecerli op degerleri (bitisik olmayan bir enum kumesi):
 *   GL_ZERO       0x0000
 *   GL_INVERT     0x150A
 *   GL_KEEP       0x1E00
 *   GL_REPLACE    0x1E01
 *   GL_INCR       0x1E02
 *   GL_DECR       0x1E03
 *   GL_INCR_WRAP  0x8507
 *   GL_DECR_WRAP  0x8508
 *
 * NOT: glStencilFunc'tan farkli olarak 0x0000 (GL_ZERO) burada
 * GECERLI bir degerdir; bu nedenle gecersiz enum listelerinde
 * 0x0000 kullanilmaz.
 */

static const char *test_procedure = "GS_GL20SC_PFO_SO_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_SO_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_SO_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_SO_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_SO_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_SO_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_SO_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_SO_ROBUSTNESS_TC_007";

void GS_GL20SC_PFO_SO_ROBUSTNESS_TP_001_close(void);

/* Gecerli glStencilOp stencil islemleri */
static const GLenum valid_stencil_ops[] = {
    GL_ZERO,   GL_KEEP,   GL_REPLACE,    GL_INCR,
    GL_DECR,   GL_INVERT, GL_INCR_WRAP,  GL_DECR_WRAP,
};
static const int valid_stencil_op_count =
    sizeof(valid_stencil_ops) / sizeof(valid_stencil_ops[0]);

static int is_valid_stencil_op(GLenum val) {
  for (int i = 0; i < valid_stencil_op_count; i++) {
    if (val == valid_stencil_ops[i])
      return 1;
  }
  return 0;
}

static int check_op_state(GLenum pname, GLenum expected) {
  GLint val;
  glGetIntegerv(pname, &val);
  return ((GLenum)val == expected);
}

/* Yalnizca on yuz state'ini dogrular (sweep gibi yogun dongulerde
   cagri maliyetini dusuk tutmak icin kullanilir). */
static int check_stencil_op_front(GLenum sfail, GLenum dpfail, GLenum dppass) {
  return check_op_state(GL_STENCIL_FAIL, sfail) &&
         check_op_state(GL_STENCIL_PASS_DEPTH_FAIL, dpfail) &&
         check_op_state(GL_STENCIL_PASS_DEPTH_PASS, dppass);
}

/* Yalnizca arka yuz state'ini dogrular. */
static int check_stencil_op_back(GLenum sfail, GLenum dpfail, GLenum dppass) {
  return check_op_state(GL_STENCIL_BACK_FAIL, sfail) &&
         check_op_state(GL_STENCIL_BACK_PASS_DEPTH_FAIL, dpfail) &&
         check_op_state(GL_STENCIL_BACK_PASS_DEPTH_PASS, dppass);
}

/* glStencilOp her iki yuzu de guncelledigi icin varsayilan kontrol
   on ve arka yuzu birlikte dogrular. */
static int check_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass) {
  return check_stencil_op_front(sfail, dpfail, dppass) &&
         check_stencil_op_back(sfail, dpfail, dppass);
}

/* Hata mesajlarinda mevcut state'i tek seferde okumak icin. */
static void read_stencil_op(GLint *sfail, GLint *dpfail, GLint *dppass) {
  glGetIntegerv(GL_STENCIL_FAIL, sfail);
  glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, dpfail);
  glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, dppass);
}

/* ============================================================
 * TEST 1 : Gecersiz Enum Degerleri (Spec Hatasi)
 * ============================================================
 *
 * Amac
 * ----
 * Spec'e gore sfail, dpfail ve dppass parametrelerinin her biri
 * yalnizca 8 stencil isleminden (GL_ZERO, GL_KEEP, GL_REPLACE,
 * GL_INCR, GL_DECR, GL_INVERT, GL_INCR_WRAP, GL_DECR_WRAP)
 * biri olmalidir.
 *
 * Bu kumenin disindaki degerler icin GL_INVALID_ENUM
 * uretilmelidir. Gecersiz deger her uc parametre pozisyonunda
 * ayri ayri denenir; gecersiz cagri sonrasinda uc state'in de
 * degismemesi beklenir.
 *
 * Denenen gecersiz degerler:
 *   0x0001, 0xDEAD, GL_BLEND, 0x1E04, 0x8509
 *   (0x1E04 ve 0x8509 = gecerli araliklarin hemen disindaki degerler)
 * ============================================================ */
void GS_GL20SC_PFO_SO_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLenum invalid[] = {(GLenum)0x0001, (GLenum)0xDEAD, GL_BLEND, (GLenum)0x1E04,
                      (GLenum)0x8509};
  int invalid_count = sizeof(invalid) / sizeof(invalid[0]);

  int fail_count = 0;

  for (int i = 0; i < invalid_count; i++) {
    for (int pos = 0; pos < 3; pos++) {
      /* Her denemeden once bilinen gecerli bir state kurulur */
      glStencilOp(GL_REPLACE, GL_INCR, GL_DECR);
      glGetError();

      GLenum args[3] = {GL_REPLACE, GL_INCR, GL_DECR};
      args[pos] = invalid[i];

      glStencilOp(args[0], args[1], args[2]);
      GLenum err = glGetError();

      if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure,
                      "val=0x%X pos=%d beklenen=GL_INVALID_ENUM gelen=0x%X",
                      invalid[i], pos, err);
        fail_count++;
        continue;
      }

      if (!check_stencil_op(GL_REPLACE, GL_INCR, GL_DECR)) {
        GLint s, df, dp;
        read_stencil_op(&s, &df, &dp);
        TEST_LOG_FAIL(test_case_1, test_procedure,
                      "val=0x%X pos=%d state bozuldu: "
                      "sfail=0x%X dpfail=0x%X dppass=0x%X",
                      invalid[i], pos, s, df, dp);
        fail_count++;
      }
    }
  }

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
  else
    TEST_LOG_FAIL(test_case_1, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 2 : Gecerli Kombinasyonlarin Ayarlanmasi ve State Sorgusu
 * ============================================================
 *
 * Amac
 * ----
 * 8 gecerli stencil isleminin uc parametre uzerindeki tum
 * kombinasyonlari (8 x 8 x 8 = 512 kombinasyon) sirayla
 * ayarlanir; ardindan GL_STENCIL_FAIL,
 * GL_STENCIL_PASS_DEPTH_FAIL ve GL_STENCIL_PASS_DEPTH_PASS
 * sorgulariyla (ve arka yuz karsiliklariyla) dogrulanir.
 *
 * Her gecerli cagri icin GL_NO_ERROR beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SO_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  int fail_count = 0;
  int combo_count = 0;

  for (int a = 0; a < valid_stencil_op_count; a++) {
    for (int b = 0; b < valid_stencil_op_count; b++) {
      for (int c = 0; c < valid_stencil_op_count; c++) {
        GLenum sfail = valid_stencil_ops[a];
        GLenum dpfail = valid_stencil_ops[b];
        GLenum dppass = valid_stencil_ops[c];

        combo_count++;

        glStencilOp(sfail, dpfail, dppass);
        GLenum err = glGetError();

        if (err != GL_NO_ERROR) {
          TEST_LOG_FAIL(test_case_2, test_procedure,
                        "(0x%X, 0x%X, 0x%X) reddedildi: err=0x%X", sfail, dpfail,
                        dppass, err);
          fail_count++;
          continue;
        }

        if (!check_stencil_op(sfail, dpfail, dppass)) {
          GLint s, df, dp;
          read_stencil_op(&s, &df, &dp);
          TEST_LOG_FAIL(test_case_2, test_procedure,
                        "(0x%X, 0x%X, 0x%X) state hatasi: "
                        "sfail=0x%X dpfail=0x%X dppass=0x%X",
                        sfail, dpfail, dppass, s, df, dp);
          fail_count++;
        }
      }
    }
  }

  TEST_LOG_INFO("stencil op kombinasyonu denendi: %d", combo_count);

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

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
 * sistematik olarak denenir. Sweep, uc parametre pozisyonunun
 * her biri icin ayri ayri tekrarlanir (toplam 3 x 65536 cagri);
 * boylece surucunun her parametreyi bagimsiz olarak dogrulayip
 * dogrulamadigi gorulur.
 *
 * Yalnizca 8 gecerli op degeri GL_NO_ERROR uretmeli;
 * geri kalan tum degerler GL_INVALID_ENUM uretmelidir.
 *
 * Gecersiz cagrilarda state'in son gecerli degerlerde
 * kalip kalmadigi kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_SO_ROBUSTNESS_TC_003(void) {
  GLenum mode;
  int pass_count = 0;
  int fail_count = 0;

  while (glGetError() != GL_NO_ERROR)
    ;

  for (int pos = 0; pos < 3; pos++) {
    GLenum last_valid[3] = {GL_KEEP, GL_KEEP, GL_KEEP};

    glStencilOp(last_valid[0], last_valid[1], last_valid[2]);
    glGetError();

    for (mode = 0; mode < 65536; mode++) {
      GLenum expected =
          is_valid_stencil_op(mode) ? GL_NO_ERROR : GL_INVALID_ENUM;

      GLenum args[3] = {last_valid[0], last_valid[1], last_valid[2]};
      args[pos] = mode;

      glStencilOp(args[0], args[1], args[2]);
      GLenum err = glGetError();

      if (err != expected) {
        TEST_LOG_FAIL(test_case_3, test_procedure,
                      "pos=%d mode=0x%X beklenen=0x%X gelen=0x%X", pos, mode,
                      expected, err);
        fail_count++;
      } else {
        pass_count++;
      }

      if (err == GL_NO_ERROR)
        last_valid[pos] = mode;

      if (err == GL_INVALID_ENUM) {
        if (!check_stencil_op_front(last_valid[0], last_valid[1],
                                    last_valid[2])) {
          GLint s, df, dp;
          read_stencil_op(&s, &df, &dp);
          TEST_LOG_FAIL(test_case_3, test_procedure,
                        "State bozuldu: pos=%d mode=0x%X "
                        "sfail=0x%X dpfail=0x%X dppass=0x%X",
                        pos, mode, s, df, dp);
          return;
        }
      }
    }

    /* Sweep sonunda on ve arka yuz senkron mu? */
    if (!check_stencil_op(last_valid[0], last_valid[1], last_valid[2])) {
      TEST_LOG_FAIL(test_case_3, test_procedure,
                    "pos=%d sweep sonrasi on/arka yuz uyusmazligi", pos);
      return;
    }
  }

  TEST_LOG_INFO("stencil op sweep - PASS: %d, FAIL: %d", pass_count,
                fail_count);

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

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
 * her uc parametre pozisyonunda gonderilerek surucunun
 * tasmasiz sekilde GL_INVALID_ENUM uretip uretmedigi
 * test edilir.
 *
 * Ozellikle 0x00018507 gibi dusuk 16 biti gecerli bir enuma
 * (GL_INCR_WRAP) esit olan degerler de denenir; surucunun
 * karsilastirmayi 32-bit yapmasi beklenir.
 *
 * Gecersiz cagri sonrasi state'in bozulmamasi beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SO_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLenum extreme[] = {(GLenum)INT_MAX,     (GLenum)INT_MIN,
                      (GLenum)0xFFFFFFFF,  (GLenum)0x80000000,
                      (GLenum)0x10000,     (GLenum)0x00018507,
                      (GLenum)0x00011E00};
  int count = sizeof(extreme) / sizeof(extreme[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    for (int pos = 0; pos < 3; pos++) {
      glStencilOp(GL_INVERT, GL_ZERO, GL_INCR_WRAP);
      glGetError();

      GLenum args[3] = {GL_INVERT, GL_ZERO, GL_INCR_WRAP};
      args[pos] = extreme[i];

      glStencilOp(args[0], args[1], args[2]);
      GLenum err = glGetError();

      if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_4, test_procedure,
                      "val=0x%X pos=%d beklenen=GL_INVALID_ENUM gelen=0x%X",
                      extreme[i], pos, err);
        fail_count++;
        continue;
      }

      if (!check_stencil_op(GL_INVERT, GL_ZERO, GL_INCR_WRAP)) {
        GLint s, df, dp;
        read_stencil_op(&s, &df, &dp);
        TEST_LOG_FAIL(test_case_4, test_procedure,
                      "val=0x%X pos=%d state bozuldu: "
                      "sfail=0x%X dpfail=0x%X dppass=0x%X",
                      extreme[i], pos, s, df, dp);
        fail_count++;
      }
    }
  }

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_4, test_procedure);
  else
    TEST_LOG_FAIL(test_case_4, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 5 : Parametre Bagimsizligi, Atomiklik ve On/Arka Yuz Senkronu
 * ============================================================
 *
 * Amac
 * ----
 * Uc asamali dogrulama yapilir:
 *
 * Asama 1 - Parametre bagimsizligi:
 *   Her parametre tek tek degistirilir; digerlerinin
 *   etkilenmedigi (yan etki olmadigi) dogrulanir.
 *
 * Asama 2 - Atomiklik:
 *   Bir parametresi gecersiz, digerleri gecerli olan cagrilarda
 *   spec geregi komut hicbir state'i degistirmemelidir;
 *   yani gecerli parametrelerin de yazilmamasi beklenir.
 *
 * Asama 3 - On/arka yuz senkronu:
 *   glStencilOp, glStencilOpSeparate(GL_FRONT_AND_BACK, ...)
 *   ile esdegerdir. GL_STENCIL_BACK_* sorgularinin
 *   GL_STENCIL_* ile birebir ayni degerleri dondurmesi beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SO_ROBUSTNESS_TC_005(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  int fail_count = 0;

  /* ---- Asama 1: Parametre bagimsizligi ---- */
  GLenum base[3] = {GL_KEEP, GL_KEEP, GL_KEEP};
  glStencilOp(base[0], base[1], base[2]);
  glGetError();

  for (int pos = 0; pos < 3; pos++) {
    for (int i = 0; i < valid_stencil_op_count; i++) {
      GLenum args[3] = {base[0], base[1], base[2]};
      args[pos] = valid_stencil_ops[i];

      glStencilOp(args[0], args[1], args[2]);
      GLenum err = glGetError();

      if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_5, test_procedure,
                      "bagimsizlik: pos=%d op=0x%X err=0x%X", pos,
                      valid_stencil_ops[i], err);
        fail_count++;
        continue;
      }

      if (!check_stencil_op(args[0], args[1], args[2])) {
        GLint s, df, dp;
        read_stencil_op(&s, &df, &dp);
        TEST_LOG_FAIL(test_case_5, test_procedure,
                      "bagimsizlik: pos=%d op=0x%X yan etki: "
                      "sfail=0x%X dpfail=0x%X dppass=0x%X",
                      pos, valid_stencil_ops[i], s, df, dp);
        fail_count++;
      }
    }
  }

  /* ---- Asama 2: Atomiklik (kismi gecersiz cagri) ---- */
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glGetError();

  struct {
    GLenum sfail;
    GLenum dpfail;
    GLenum dppass;
  } partial[] = {
      {(GLenum)0xDEAD, GL_INVERT, GL_REPLACE},
      {GL_INVERT, (GLenum)0xDEAD, GL_REPLACE},
      {GL_INVERT, GL_REPLACE, (GLenum)0xDEAD},
      {(GLenum)0xDEAD, (GLenum)0xBEEF, GL_REPLACE},
      {(GLenum)0x1E04, (GLenum)0x1E05, (GLenum)0x1E06},
  };
  int partial_count = sizeof(partial) / sizeof(partial[0]);

  for (int i = 0; i < partial_count; i++) {
    glStencilOp(partial[i].sfail, partial[i].dpfail, partial[i].dppass);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "atomiklik: (0x%X, 0x%X, 0x%X) beklenen=GL_INVALID_ENUM "
                    "gelen=0x%X",
                    partial[i].sfail, partial[i].dpfail, partial[i].dppass,
                    err);
      fail_count++;
      continue;
    }

    if (!check_stencil_op(GL_KEEP, GL_KEEP, GL_KEEP)) {
      GLint s, df, dp;
      read_stencil_op(&s, &df, &dp);
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "atomiklik: (0x%X, 0x%X, 0x%X) sonrasi state yazildi: "
                    "sfail=0x%X dpfail=0x%X dppass=0x%X",
                    partial[i].sfail, partial[i].dpfail, partial[i].dppass, s,
                    df, dp);
      fail_count++;

      /* Sonraki iterasyon icin referans state geri yuklenir */
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
      glGetError();
    }
  }

  /* ---- Asama 3: On / arka yuz senkronu ---- */
  for (int i = 0; i < valid_stencil_op_count; i++) {
    GLenum sfail = valid_stencil_ops[i];
    GLenum dpfail = valid_stencil_ops[(i + 1) % valid_stencil_op_count];
    GLenum dppass = valid_stencil_ops[(i + 2) % valid_stencil_op_count];

    glStencilOp(sfail, dpfail, dppass);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_5, test_procedure, "senkron: err=0x%X", err);
      fail_count++;
      continue;
    }

    if (!check_stencil_op_back(sfail, dpfail, dppass)) {
      GLint bs = 0, bdf = 0, bdp = 0;
      glGetIntegerv(GL_STENCIL_BACK_FAIL, &bs);
      glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &bdf);
      glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &bdp);
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "senkron: on(0x%X, 0x%X, 0x%X) != "
                    "arka(0x%X, 0x%X, 0x%X)",
                    sfail, dpfail, dppass, bs, bdf, bdp);
      fail_count++;
    }
  }

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
  else
    TEST_LOG_FAIL(test_case_5, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 6 : Gecerli Enumlarin Komsu Sinir Degerleri
 * ============================================================
 *
 * Amac
 * ----
 * glStencilOp'un gecerli enum kumesi bitisik degildir
 * (0x0000, 0x150A, 0x1E00-0x1E03, 0x8507-0x8508). Bu nedenle
 * surucunun "aralik kontrolu" yerine tam kume kontrolu
 * yapmasi gerekir.
 *
 * Her gecerli degerin hemen alt ve ust komsulari denenir:
 *   0x0001 (GL_ZERO+1)       -> gecersiz
 *   0x1509, 0x150B           -> GL_INVERT komsulari, gecersiz
 *   0x1DFF, 0x1E04           -> GL_KEEP..GL_DECR komsulari, gecersiz
 *   0x8506, 0x8509           -> WRAP komsulari, gecersiz
 *
 * Ayrica sinir gecerli degerlerin (GL_ZERO, GL_DECR_WRAP)
 * dogru kabul edildigi tekrar dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_SO_ROBUSTNESS_TC_006(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLenum neighbours[] = {(GLenum)0x0001, (GLenum)0x1509, (GLenum)0x150B,
                         (GLenum)0x1DFF, (GLenum)0x1E04, (GLenum)0x8506,
                         (GLenum)0x8509};
  int neighbour_count = sizeof(neighbours) / sizeof(neighbours[0]);

  int fail_count = 0;

  glStencilOp(GL_ZERO, GL_INVERT, GL_DECR_WRAP);
  glGetError();

  for (int i = 0; i < neighbour_count; i++) {
    for (int pos = 0; pos < 3; pos++) {
      GLenum args[3] = {GL_ZERO, GL_INVERT, GL_DECR_WRAP};
      args[pos] = neighbours[i];

      glStencilOp(args[0], args[1], args[2]);
      GLenum err = glGetError();

      if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_6, test_procedure,
                      "komsu=0x%X pos=%d beklenen=GL_INVALID_ENUM gelen=0x%X",
                      neighbours[i], pos, err);
        fail_count++;
        continue;
      }

      if (!check_stencil_op(GL_ZERO, GL_INVERT, GL_DECR_WRAP)) {
        GLint s, df, dp;
        read_stencil_op(&s, &df, &dp);
        TEST_LOG_FAIL(test_case_6, test_procedure,
                      "komsu=0x%X pos=%d state bozuldu: "
                      "sfail=0x%X dpfail=0x%X dppass=0x%X",
                      neighbours[i], pos, s, df, dp);
        fail_count++;

        glStencilOp(GL_ZERO, GL_INVERT, GL_DECR_WRAP);
        glGetError();
      }
    }
  }

  /* Sinir gecerli degerler halen kabul ediliyor mu? */
  GLenum boundary[] = {GL_ZERO, GL_INVERT, GL_KEEP, GL_DECR, GL_INCR_WRAP,
                       GL_DECR_WRAP};
  int boundary_count = sizeof(boundary) / sizeof(boundary[0]);

  for (int i = 0; i < boundary_count; i++) {
    glStencilOp(boundary[i], boundary[i], boundary[i]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_6, test_procedure,
                    "gecerli sinir degeri reddedildi: 0x%X err=0x%X",
                    boundary[i], err);
      fail_count++;
      continue;
    }

    if (!check_stencil_op(boundary[i], boundary[i], boundary[i])) {
      GLint s, df, dp;
      read_stencil_op(&s, &df, &dp);
      TEST_LOG_FAIL(test_case_6, test_procedure,
                    "sinir degeri 0x%X state hatasi: "
                    "sfail=0x%X dpfail=0x%X dppass=0x%X",
                    boundary[i], s, df, dp);
      fail_count++;
    }
  }

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
  else
    TEST_LOG_FAIL(test_case_6, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
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
 *   gecerli sfail/dpfail/dppass kombinasyonlari ayarlanir.
 *   Her iterasyonda hem state degerleri hem de
 *   GL_STENCIL_TEST enable durumu dogrulanir.
 *
 * Asama 2 - Deterministik Fuzzing (50.000 iterasyon):
 *   LCG ile uretilen rastgele enum ucluleri gonderilir.
 *   Yalnizca GL_INVALID_ENUM veya GL_NO_ERROR kabul edilir.
 *   GL_NO_ERROR donen cagrilarda state gonderilen degerlere
 *   esit olmali; GL_INVALID_ENUM donen cagrilarda ise
 *   state son gecerli uclude kalmalidir.
 *
 * Dongu sonunda context'in saglikli oldugu ve varsayilan
 * state'e (GL_KEEP, GL_KEEP, GL_KEEP) geri donulebildigi
 * kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_SO_ROBUSTNESS_TC_007(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  int err_count = 0;

  for (int i = 0; i < 10000; i++) {
    GLenum sfail = valid_stencil_ops[i % valid_stencil_op_count];
    GLenum dpfail = valid_stencil_ops[(i / 3) % valid_stencil_op_count];
    GLenum dppass = valid_stencil_ops[(i / 7) % valid_stencil_op_count];

    glEnable(GL_STENCIL_TEST);
    glStencilOp(sfail, dpfail, dppass);

    if (!check_stencil_op(sfail, dpfail, dppass))
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

  unsigned int seed = 24680;
  int unexpected_err_count = 0;
  GLenum last_unexpected = GL_NO_ERROR;
  int first_fail = -1;

  GLenum last_good[3] = {GL_KEEP, GL_KEEP, GL_KEEP};
  glStencilOp(last_good[0], last_good[1], last_good[2]);
  glGetError();

  for (int i = 0; i < 50000; i++) {
    GLenum args[3];

    for (int p = 0; p < 3; p++) {
      seed = seed * 1103515245 + 12345;

      /* Iterasyonlarin bir kismi tamamen gecerli uclu uretir;
         boylece GL_NO_ERROR yolu da yeterince taranir. */
      if (i % 4 == 0)
        args[p] = valid_stencil_ops[(seed >> 16) % valid_stencil_op_count];
      else
        args[p] = (GLenum)((seed >> 16) % 0x10000);
    }

    glStencilOp(args[0], args[1], args[2]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
      unexpected_err_count++;
      last_unexpected = err;
      if (first_fail < 0)
        first_fail = i;
      continue;
    }

    if (err == GL_NO_ERROR) {
      /* Gecerli cagri: uc parametre de gecerli olmali ve
         state gonderilen degerlere esit olmali. */
      if (!is_valid_stencil_op(args[0]) || !is_valid_stencil_op(args[1]) ||
          !is_valid_stencil_op(args[2])) {
        err_count++;
        continue;
      }

      if (!check_stencil_op(args[0], args[1], args[2])) {
        err_count++;
        continue;
      }

      last_good[0] = args[0];
      last_good[1] = args[1];
      last_good[2] = args[2];
    } else {
      /* Gecersiz cagri: state son gecerli uclude kalmali. */
      if (!check_stencil_op(last_good[0], last_good[1], last_good[2]))
        err_count++;
    }
  }

  while (glGetError() != GL_NO_ERROR)
    ;
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  GLenum final_err = glGetError();
  int state_ok = check_stencil_op(GL_KEEP, GL_KEEP, GL_KEEP);

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

void GS_GL20SC_PFO_SO_ROBUSTNESS_TP_001_init(void) {
  GS_GL20SC_PFO_SO_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_SO_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_SO_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_SO_ROBUSTNESS_TC_004();
  GS_GL20SC_PFO_SO_ROBUSTNESS_TC_005();
  GS_GL20SC_PFO_SO_ROBUSTNESS_TC_006();
  GS_GL20SC_PFO_SO_ROBUSTNESS_TC_007();

  GS_GL20SC_PFO_SO_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_SO_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_PFO_SO_ROBUSTNESS_TP_001_close(void) {
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glDisable(GL_STENCIL_TEST);
}
