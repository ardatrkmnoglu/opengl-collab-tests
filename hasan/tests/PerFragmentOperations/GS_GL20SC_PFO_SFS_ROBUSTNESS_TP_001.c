#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <limits.h>

/*
 * GL20SC - PerFragmentOperations - StencilFuncSeparate - ROBUSTNESS
 *
 * glStencilFuncSeparate(GLenum face, GLenum func,
 *                       GLint ref, GLuint mask)
 *
 *
 * Test edilen state sorgulari:
 *   On yuz  : GL_STENCIL_FUNC
 *             GL_STENCIL_REF
 *             GL_STENCIL_VALUE_MASK
 *   Arka yuz: GL_STENCIL_BACK_FUNC
 *             GL_STENCIL_BACK_REF
 *             GL_STENCIL_BACK_VALUE_MASK
 *
 */

static const char *test_procedure = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_007";
static const char *test_case_8 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_008";
static const char *test_case_9 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_009";
static const char *test_case_10 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_010";
static const char *test_case_11 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_011";
static const char *test_case_12 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_012";
static const char *test_case_13 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_013";
static const char *test_case_14 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_014";
static const char *test_case_15 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_015";
static const char *test_case_16 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_016";
static const char *test_case_17 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_017";
static const char *test_case_18 = "GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_018";

void GS_GL20SC_PFO_SFS_ROBUSTNESS_TP_001_close(void);

/* ------------------------------------------------------------ *
 * Ortak veri ve yardimci fonksiyonlar
 * ------------------------------------------------------------ */

static const GLenum valid_faces[] = {GL_FRONT, GL_BACK, GL_FRONT_AND_BACK};
static const int valid_face_count =
    sizeof(valid_faces) / sizeof(valid_faces[0]);

/* Gecerli karsilastirma fonksiyonlari - BITISIK aralik 0x0200..0x0207 */
static const GLenum valid_funcs[] = {
    GL_NEVER,   GL_LESS,     GL_EQUAL,  GL_LEQUAL,
    GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS,
};
static const int valid_func_count = sizeof(valid_funcs) / sizeof(valid_funcs[0]);

static int is_valid_face(GLenum val) {
  for (int i = 0; i < valid_face_count; i++) {
    if (val == valid_faces[i])
      return 1;
  }
  return 0;
}

static int is_valid_func(GLenum val) {
  for (int i = 0; i < valid_func_count; i++) {
    if (val == valid_funcs[i])
      return 1;
  }
  return 0;
}

/* Stencil buffer bit derinligine gore ref ust siniri. */
static GLint get_max_stencil_ref(void) {
  GLint stencil_bits = 0;
  glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
  if (stencil_bits <= 0)
    return 0;
  if (stencil_bits >= (GLint)(sizeof(GLint) * 8 - 1))
    return INT_MAX;
  return (GLint)((1u << stencil_bits) - 1u);
}

static GLint clamp_ref(GLint ref) {
  GLint max_ref = get_max_stencil_ref();
  if (ref < 0)
    return 0;
  if (ref > max_ref)
    return max_ref;
  return ref;
}

/* On yuz uclusunu (func, ref, mask) tek seferde okur. */
static void get_front_state(GLint out[3]) {
  glGetIntegerv(GL_STENCIL_FUNC, &out[0]);
  glGetIntegerv(GL_STENCIL_REF, &out[1]);
  glGetIntegerv(GL_STENCIL_VALUE_MASK, &out[2]);
}

/* Arka yuz uclusunu tek seferde okur. */
static void get_back_state(GLint out[3]) {
  glGetIntegerv(GL_STENCIL_BACK_FUNC, &out[0]);
  glGetIntegerv(GL_STENCIL_BACK_REF, &out[1]);
  glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &out[2]);
}

static int state_eq(const GLint got[3], GLenum func, GLint ref, GLuint mask) {
  return (GLenum)got[0] == func && got[1] == ref && (GLuint)got[2] == mask;
}

/* Iki yuzu birbirinden farkli degerlere ayirir, hata kuyrugunu bosaltir. */
static void set_faces(GLenum ffunc, GLint fref, GLuint fmask, GLenum bfunc,
                      GLint bref, GLuint bmask) {
  glStencilFuncSeparate(GL_FRONT, ffunc, fref, fmask);
  glStencilFuncSeparate(GL_BACK, bfunc, bref, bmask);
  while (glGetError() != GL_NO_ERROR)
    ;
}

static void reset_stencil_func(void) {
  glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, 0xFFFFFFFF);
  while (glGetError() != GL_NO_ERROR)
    ;
}

/* ============================================================
 * TEST 1 : Varsayilan Baslangic Durumu
 * ============================================================
 *
 *  Context olusturuldugunda her iki yuzun de
 *           func=GL_ALWAYS ve ref=0 ile baslamasi.
 *
 * Spec, GL_STENCIL_VALUE_MASK'in baslangic degerini
 * "tum bitleri 1" olarak tanimlar. Sorgu sonucunun
 * 0xFFFFFFFF mi yoksa (2^s - 1) mi dondurulecegi
 * implementasyona gore degisebildigi icin bu deger
 * KATI sekilde karsilastirilmaz; iki kabul edilebilir
 * degerden biri olmasi yeterlidir ve gercek deger
 * INFO olarak raporlanir.
 *
 * Bu test paketin ILK calisan testidir.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLint stencil_bits = 0;
  glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
  TEST_LOG_INFO("GL_STENCIL_BITS = %d, max_ref = %d", stencil_bits,
                get_max_stencil_ref());

  GLint front[3], back[3];
  get_front_state(front);
  get_back_state(back);

  GLenum err = glGetError();

  GLuint mask_alt = (GLuint)get_max_stencil_ref();
  int front_mask_ok =
      ((GLuint)front[2] == 0xFFFFFFFFu) || ((GLuint)front[2] == mask_alt);
  int back_mask_ok =
      ((GLuint)back[2] == 0xFFFFFFFFu) || ((GLuint)back[2] == mask_alt);

  TEST_LOG_INFO("varsayilan value mask - on: 0x%X arka: 0x%X", front[2],
                back[2]);

  int ok = (err == GL_NO_ERROR) && ((GLenum)front[0] == GL_ALWAYS) &&
           ((GLenum)back[0] == GL_ALWAYS) && (front[1] == 0) &&
           (back[1] == 0) && front_mask_ok && back_mask_ok;

  if (ok)
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
  else
    TEST_LOG_FAIL(test_case_1, test_procedure,
                  "Varsayilan state hatali: on(func=0x%X ref=%d mask=0x%X) "
                  "arka(func=0x%X ref=%d mask=0x%X) err=0x%X",
                  front[0], front[1], front[2], back[0], back[1], back[2], err);
}

/* ============================================================
 * TEST 2 : face Parametresi - Gecersiz Enum Hata Kodu
 * ============================================================
 *
 *  Gecersiz face degerlerinin GL_INVALID_ENUM uretmesi.
 *
 * Diger uc parametre gecerli tutulur; tek hata kaynagi face'tir.
 * Secilen degerler face enum kumesinin cevresini tarar:
 *
 *   0x0403 -> GL_FRONT'un bir alti
 *   0x0406 / 0x0407 -> GL_BACK ile GL_FRONT_AND_BACK arasindaki
 *                      BOSLUK (masaustu GL'de GL_LEFT / GL_RIGHT)
 *   0x0409 -> GL_FRONT_AND_BACK'in bir ustu
 *
 * face degerlerini aralik olarak kontrol eden bir surucu
 * 0x0406 ve 0x0407'yi yanlislikla kabul eder.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_002(void) {
  reset_stencil_func();

  GLenum invalid_faces[] = {(GLenum)0x0000, (GLenum)0x0403, (GLenum)0x0406,
                            (GLenum)0x0407, (GLenum)0x0409, GL_BLEND,
                            (GLenum)0xDEAD};
  int count = sizeof(invalid_faces) / sizeof(invalid_faces[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    glStencilFuncSeparate(invalid_faces[i], GL_ALWAYS, 0, 0xFF);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "face=0x%X beklenen=GL_INVALID_ENUM gelen=0x%X",
                    invalid_faces[i], err);
      fail_count++;
    }
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_2, test_procedure);
  else
    TEST_LOG_FAIL(test_case_2, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 3 : face Parametresi - 16-bit Enum Sweep
 * ============================================================
 *
 * TEK KONU: 0x0000..0xFFFF araliginda yalnizca 3 face degerinin
 *           kabul edilmesi.
 *
 * func / ref / mask sabit ve gecerli tutulur; boylece olusabilecek
 * tek hata kaynagi face olur. Kabul edilen deger sayisi ayrica
 * sayilir ve tam olarak 3 olmasi beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_003(void) {
  reset_stencil_func();

  GLenum face;
  int accepted_count = 0;
  int fail_count = 0;
  int first_fail = -1;

  for (face = 0; face < 65536; face++) {
    GLenum expected = is_valid_face(face) ? GL_NO_ERROR : GL_INVALID_ENUM;

    glStencilFuncSeparate(face, GL_ALWAYS, 0, 0xFF);
    GLenum err = glGetError();

    if (err == GL_NO_ERROR)
      accepted_count++;

    if (err != expected) {
      if (first_fail < 0) {
        first_fail = (int)face;
        TEST_LOG_FAIL(test_case_3, test_procedure,
                      "face=0x%X beklenen=0x%X gelen=0x%X", face, expected,
                      err);
      }
      fail_count++;
    }
  }

  TEST_LOG_INFO("face sweep - kabul edilen: %d (beklenen: %d)", accepted_count,
                valid_face_count);

  reset_stencil_func();

  if (fail_count == 0 && accepted_count == valid_face_count)
    TEST_LOG_SUCCESS(test_case_3, test_procedure);
  else
    TEST_LOG_FAIL(test_case_3, test_procedure,
                  "Basarisiz: %d, kabul edilen: %d", fail_count,
                  accepted_count);
}

/* ============================================================
 * TEST 4 : func Parametresi - Bitisik Aralik Sinirlari
 * ============================================================
 *
 * TEK KONU: Gecerli func araliginin (0x0200..0x0207) TAM olarak
 *           bu sinirlarda bitmesi.
 *
 * glStencilOp'un dagitik enum kumesinden farkli olarak
 * karsilastirma fonksiyonlari bitisik bir araliktir. Bu nedenle
 * en olasi hata "bir eksik / bir fazla" aralik kontroludur.
 *
 * Test edilen sinirlar:
 *   0x01FE, 0x01FF -> araligin hemen altindaki iki deger
 *   0x0200, 0x0207 -> gecerli uc noktalar (kabul edilmeli)
 *   0x0208, 0x0209 -> araligin hemen ustundeki iki deger
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_004(void) {
  reset_stencil_func();

  struct {
    GLenum func;
    GLenum expected;
    const char *desc;
  } boundary[] = {
      {(GLenum)0x01FE, GL_INVALID_ENUM, "aralik alti -2"},
      {(GLenum)0x01FF, GL_INVALID_ENUM, "aralik alti -1"},
      {(GLenum)0x0200, GL_NO_ERROR, "GL_NEVER (alt sinir)"},
      {(GLenum)0x0207, GL_NO_ERROR, "GL_ALWAYS (ust sinir)"},
      {(GLenum)0x0208, GL_INVALID_ENUM, "aralik ustu +1"},
      {(GLenum)0x0209, GL_INVALID_ENUM, "aralik ustu +2"},
  };
  int count = sizeof(boundary) / sizeof(boundary[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    glStencilFuncSeparate(GL_FRONT_AND_BACK, boundary[i].func, 0, 0xFF);
    GLenum err = glGetError();

    if (err != boundary[i].expected) {
      TEST_LOG_FAIL(test_case_4, test_procedure,
                    "%s (0x%X): beklenen=0x%X gelen=0x%X", boundary[i].desc,
                    boundary[i].func, boundary[i].expected, err);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_4, test_procedure);
  else
    TEST_LOG_FAIL(test_case_4, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 5 : func Parametresi - 16-bit Enum Sweep
 * ============================================================
 *
 * TEK KONU: Kabul edilen func degerlerinin kumesinin
 *           TAM OLARAK 0x0200..0x0207 olmasi.
 *
 * Sweep sirasinda kabul edilen degerlerin en kucugu, en buyugu
 * ve sayisi kaydedilir. Yalnizca hata kodu karsilastirmasi
 * yapilmaz; kumenin kendisi de dogrulanir:
 *
 *   sayi == 8 && min == 0x0200 && max == 0x0207
 *
 * Bu sayede "fazladan enum kabul eden" ya da "araligi kaydiran"
 * suruculer iki bagimsiz yoldan yakalanir.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_005(void) {
  reset_stencil_func();

  GLenum mode;
  int accepted_count = 0;
  GLenum accepted_min = 0xFFFF;
  GLenum accepted_max = 0;
  int fail_count = 0;
  int first_fail = -1;

  for (mode = 0; mode < 65536; mode++) {
    GLenum expected = is_valid_func(mode) ? GL_NO_ERROR : GL_INVALID_ENUM;

    glStencilFuncSeparate(GL_FRONT_AND_BACK, mode, 0, 0xFF);
    GLenum err = glGetError();

    if (err == GL_NO_ERROR) {
      accepted_count++;
      if (mode < accepted_min)
        accepted_min = mode;
      if (mode > accepted_max)
        accepted_max = mode;
    }

    if (err != expected) {
      if (first_fail < 0) {
        first_fail = (int)mode;
        TEST_LOG_FAIL(test_case_5, test_procedure,
                      "func=0x%X beklenen=0x%X gelen=0x%X", mode, expected,
                      err);
      }
      fail_count++;
    }
  }

  TEST_LOG_INFO("func sweep - kabul edilen: %d, min=0x%X, max=0x%X",
                accepted_count, accepted_min, accepted_max);

  int set_ok = (accepted_count == valid_func_count) &&
               (accepted_min == GL_NEVER) && (accepted_max == GL_ALWAYS);

  reset_stencil_func();

  if (fail_count == 0 && set_ok)
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
  else
    TEST_LOG_FAIL(test_case_5, test_procedure,
                  "Basarisiz: %d, kume: sayi=%d min=0x%X max=0x%X", fail_count,
                  accepted_count, accepted_min, accepted_max);
}

/* ============================================================
 * TEST 6 : 32-bit Tasma Davranisi
 * ============================================================
 *
 * TEK KONU: 16-bit sweep'in disinda kalan degerlerin
 *           tasma olmadan reddedilmesi.
 *
 * Alt 16 biti gecerli bir enuma esit olan degerler ozellikle
 * secilmistir:
 *
 *   0x00010404 -> alt 16 bit = GL_FRONT
 *   0x00010207 -> alt 16 bit = GL_ALWAYS
 *   0xFFFF0200 -> alt 16 bit = GL_NEVER
 *
 * Karsilastirmayi 16-bit'e kirpan bir surucu bunlari kabul eder.
 * Degerler hem face hem func pozisyonunda denenir.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_006(void) {
  reset_stencil_func();

  GLenum extreme[] = {(GLenum)INT_MAX,    (GLenum)INT_MIN,
                      (GLenum)0xFFFFFFFF, (GLenum)0x80000000,
                      (GLenum)0x00010000, (GLenum)0x00010404,
                      (GLenum)0x00010207, (GLenum)0xFFFF0200};
  int count = sizeof(extreme) / sizeof(extreme[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    glStencilFuncSeparate(extreme[i], GL_ALWAYS, 0, 0xFF);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_6, test_procedure,
                    "face=0x%X beklenen=GL_INVALID_ENUM gelen=0x%X", extreme[i],
                    err);
      fail_count++;
    }

    glStencilFuncSeparate(GL_FRONT_AND_BACK, extreme[i], 0, 0xFF);
    err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_6, test_procedure,
                    "func=0x%X beklenen=GL_INVALID_ENUM gelen=0x%X", extreme[i],
                    err);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
  else
    TEST_LOG_FAIL(test_case_6, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 7 : ref Parametresi Hicbir Zaman Hata Uretmez
 * ============================================================
 *
 * TEK KONU: ref parametresinin GLint uzayindaki HICBIR degeri
 *           icin hata uretilmemesi.
 *
 * ref bir enum degildir; spec'te gecersiz ref diye bir kavram
 * yoktur. Aralik disi degerler sessizce clamp edilir.
 * Bu test yalnizca hata kodunu inceler; clamp degerinin
 * dogrulugu TC_008'in konusudur.
 *
 * Denenen degerler isaret sinirlarini kapsar:
 *   INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX - 1, INT_MAX
 *   ve isaret biti set olan bit desenleri.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_007(void) {
  reset_stencil_func();

  GLint refs[] = {INT_MIN,   INT_MIN + 1, -65536,      -256,        -1,
                  0,         1,           255,         65535,       INT_MAX - 1,
                  INT_MAX,   (GLint)0x80000000,        (GLint)0xFFFFFFFF};
  int count = sizeof(refs) / sizeof(refs[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    for (int f = 0; f < valid_face_count; f++) {
      glStencilFuncSeparate(valid_faces[f], GL_LEQUAL, refs[i], 0xFF);
      GLenum err = glGetError();

      if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_7, test_procedure,
                      "face=0x%X ref=%d hata uretti: 0x%X", valid_faces[f],
                      refs[i], err);
        fail_count++;
      }
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_7, test_procedure);
  else
    TEST_LOG_FAIL(test_case_7, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 8 : ref Clamp Egrisi
 * ============================================================
 *
 * TEK KONU: ref degerinin [0, 2^s - 1] araligina dogru sekilde
 *           clamp edilmesi.
 *
 * Tek tek sinir degerleri denemek yerine, clamp sinirinin
 * ETRAFINDAKI SUREKLI ARALIK taranir:
 *
 *   [-8 .. +8]                 -> negatif taraf 0'a clamp
 *   [max_ref-8 .. max_ref+8]   -> ust taraf max_ref'e clamp
 *
 * Boylece "bir eksik/bir fazla clamp" hatasi (ornegin max_ref
 * yerine max_ref-1'e clamp) yakalanir. Bir de tam ortadaki
 * degerin hic degistirilmedigi kontrol edilir.
 *
 * Clamp davranisi her iki yuz icin ayri ayri dogrulanir;
 * arka yuzun clamp mantiginin unutulmus olmasi tipik bir
 * implementasyon hatasidir.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_008(void) {
  reset_stencil_func();

  GLint max_ref = get_max_stencil_ref();
  int fail_count = 0;

  for (int f = 0; f < valid_face_count; f++) {
    GLenum face = valid_faces[f];

    for (int phase = 0; phase < 2; phase++) {
      GLint center = (phase == 0) ? 0 : max_ref;

      for (int d = -8; d <= 8; d++) {
        /* INT tasmasini onle */
        if (center > 0 && d > 0 && center > INT_MAX - d)
          continue;
        if (center < 0 && d < 0 && center < INT_MIN - d)
          continue;

        GLint ref = center + d;
        GLint expected = clamp_ref(ref);

        glStencilFuncSeparate(face, GL_EQUAL, ref, 0xFF);
        GLenum err = glGetError();

        if (err != GL_NO_ERROR) {
          TEST_LOG_FAIL(test_case_8, test_procedure,
                        "face=0x%X ref=%d hata: 0x%X", face, ref, err);
          fail_count++;
          continue;
        }

        GLint state[3];
        if (face == GL_BACK)
          get_back_state(state);
        else
          get_front_state(state);

        if (state[1] != expected) {
          TEST_LOG_FAIL(test_case_8, test_procedure,
                        "face=0x%X ref=%d beklenen=%d gelen=%d (max_ref=%d)",
                        face, ref, expected, state[1], max_ref);
          fail_count++;
        }
      }
    }
  }

  /* Aralik ortasindaki bir deger hic degistirilmemeli */
  if (max_ref > 1) {
    GLint mid = max_ref / 2;
    glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, mid, 0xFF);
    glGetError();

    GLint front[3], back[3];
    get_front_state(front);
    get_back_state(back);

    if (front[1] != mid || back[1] != mid) {
      TEST_LOG_FAIL(test_case_8, test_procedure,
                    "aralik ici deger degistirildi: ref=%d on=%d arka=%d", mid,
                    front[1], back[1]);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_8, test_procedure);
  else
    TEST_LOG_FAIL(test_case_8, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 9 : Clamp Esiginin Ikili Arama ile Kesfi
 * ============================================================
 *
 * TEK KONU: Surucunun GERCEK clamp esiginin, GL_STENCIL_BITS
 *           sorgusundan turetilen degerle ayni olmasi.
 *
 * Onceki testler "beklenen" degeri GL_STENCIL_BITS'ten turetip
 * dogrulugunu varsayar. Bu test tersini yapar: esik degeri
 * hicbir varsayim yapmadan, ikili arama ile DENEYSEL olarak
 * bulur.
 *
 *   Aranan: readback(ref) == ref esitliginin saglandigi
 *           en buyuk ref degeri
 *
 * Clamp monoton oldugu icin ikili arama gecerlidir ve
 * yaklasik 31 adimda sonuca ulasir. Bulunan deger
 * (2^GL_STENCIL_BITS - 1) ile karsilastirilir. Ikisi
 * tutmuyorsa ya GL_STENCIL_BITS yanlis raporlaniyordur
 * ya da clamp yanlis genislikte yapiliyordur; her iki
 * durum da raporlanir.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_009(void) {
  reset_stencil_func();

  GLint stencil_bits = 0;
  glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
  GLint spec_max = get_max_stencil_ref();

  /* readback(ref) == ref saglayan en buyuk degeri ara */
  GLint lo = 0;
  GLint hi = INT_MAX;

  while (lo < hi) {
    /* Yukari yuvarlayan orta nokta. (hi - lo + 1) ifadesi lo=0, hi=INT_MAX
       durumunda GLint'te tasar; bu nedenle toplama unsigned yapilir. */
    GLint mid = lo + (GLint)(((unsigned)(hi - lo) + 1u) / 2u);

    glStencilFuncSeparate(GL_FRONT, GL_EQUAL, mid, 0xFF);
    if (glGetError() != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_9, test_procedure,
                    "ikili arama sirasinda hata: ref=%d", mid);
      reset_stencil_func();
      return;
    }

    GLint state[3];
    get_front_state(state);

    if (state[1] == mid)
      lo = mid;
    else
      hi = mid - 1;
  }

  GLint measured_max = lo;

  TEST_LOG_INFO("clamp esigi - olculen: %d, GL_STENCIL_BITS'ten: %d (bits=%d)",
                measured_max, spec_max, stencil_bits);

  /* Esigin hemen ustundeki degerin de esige clamp edildigini dogrula */
  int above_ok = 1;
  if (measured_max < INT_MAX) {
    glStencilFuncSeparate(GL_FRONT, GL_EQUAL, measured_max + 1, 0xFF);
    glGetError();

    GLint state[3];
    get_front_state(state);
    above_ok = (state[1] == measured_max);

    if (!above_ok)
      TEST_LOG_FAIL(test_case_9, test_procedure,
                    "esik ustu clamp hatali: ref=%d gelen=%d beklenen=%d",
                    measured_max + 1, state[1], measured_max);
  }

  reset_stencil_func();

  if (measured_max == spec_max && above_ok)
    TEST_LOG_SUCCESS(test_case_9, test_procedure);
  else
    TEST_LOG_FAIL(test_case_9, test_procedure,
                  "olculen=%d != beklenen=%d (GL_STENCIL_BITS=%d)",
                  measured_max, spec_max, stencil_bits);
}

/* ============================================================
 * TEST 10 : mask Parametresi Isaretsiz Olarak Islenmeli
 * ============================================================
 *
 * TEK KONU: mask parametresinin GLuint olarak islenmesi ve
 *           her bit deseninin birebir geri okunmasi.
 *
 * mask icin spec'te gecersiz deger yoktur; hicbir cagri hata
 * uretmemelidir. Kritik nokta, isaret biti set olan
 * degerlerdir: mask'i icten GLint'e ceviren bir surucu
 * 0x80000000 ve ustunu yanlis isler.
 *
 * Denenen desenler bilincli olarak isaret sinirini kapsar:
 *   0x00000000, 0x00000001, 0x7FFFFFFF (isaret biti 0'in ucu)
 *   0x80000000 (isaret biti 1'in basi), 0xFFFFFFFF
 *   ve karisik desenler.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_010(void) {
  reset_stencil_func();

  GLuint masks[] = {0x00000000u, 0x00000001u, 0x000000FFu, 0x00FF00FFu,
                    0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xAAAAAAAAu,
                    0x55555555u, 0xDEADBEEFu, 0xFFFFFFFFu};
  int count = sizeof(masks) / sizeof(masks[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_LESS, 1, masks[i]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_10, test_procedure, "mask=0x%X hata uretti: 0x%X",
                    masks[i], err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_state(front);
    get_back_state(back);

    if ((GLuint)front[2] != masks[i] || (GLuint)back[2] != masks[i]) {
      TEST_LOG_FAIL(test_case_10, test_procedure,
                    "mask=0x%X geri okuma: on=0x%X arka=0x%X", masks[i],
                    front[2], back[2]);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_10, test_procedure);
  else
    TEST_LOG_FAIL(test_case_10, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 11 : GL_FRONT Yuz Izolasyonu
 * ============================================================
 *
 * TEK KONU: GL_FRONT ile yapilan yazmanin arka yuzun
 *           func / ref / mask degerlerine dokunmamasi.
 *
 * Arka yuz kolay ayirt edilir bir uucluye sabitlenir ve
 * on yuze yapilan her yazmadan sonra arka yuzun uc alani da
 * kontrol edilir. Sadece func'in degil, ref ve mask'in da
 * sizmadigi dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_011(void) {
  reset_stencil_func();

  GLenum back_func = GL_NOTEQUAL;
  GLint back_ref = clamp_ref(3);
  GLuint back_mask = 0x0F0Fu;

  set_faces(GL_ALWAYS, 0, 0xFFFFFFFFu, back_func, 3, back_mask);

  int fail_count = 0;

  for (int i = 0; i < valid_func_count; i++) {
    GLenum func = valid_funcs[i];
    GLint ref_in = i + 1;
    GLint ref_expected = clamp_ref(ref_in);
    GLuint mask = (GLuint)(0x11u * (unsigned)(i + 1));

    glStencilFuncSeparate(GL_FRONT, func, ref_in, mask);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_11, test_procedure, "GL_FRONT yazma hatasi: 0x%X",
                    err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_state(front);
    get_back_state(back);

    if (!state_eq(front, func, ref_expected, mask)) {
      TEST_LOG_FAIL(test_case_11, test_procedure,
                    "on yuz yazilmadi: beklenen(0x%X, %d, 0x%X) "
                    "gelen(0x%X, %d, 0x%X)",
                    func, ref_expected, mask, front[0], front[1], front[2]);
      fail_count++;
    }

    if (!state_eq(back, back_func, back_ref, back_mask)) {
      TEST_LOG_FAIL(test_case_11, test_procedure,
                    "arka yuze sizinti: beklenen(0x%X, %d, 0x%X) "
                    "gelen(0x%X, %d, 0x%X)",
                    back_func, back_ref, back_mask, back[0], back[1], back[2]);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_11, test_procedure);
  else
    TEST_LOG_FAIL(test_case_11, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 12 : GL_BACK Yuz Izolasyonu
 * ============================================================
 *
 * TEK KONU: GL_BACK ile yapilan yazmanin on yuzun
 *           func / ref / mask degerlerine dokunmamasi.
 *
 * TC_011'in simetrigidir. Ayrica arka yuz icin de ref
 * clamp'inin uygulandigi dogrulanir; bazi implementasyonlar
 * clamp mantigini yalnizca on yuz yolunda uygular.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_012(void) {
  reset_stencil_func();

  GLenum front_func = GL_GREATER;
  GLint front_ref = clamp_ref(2);
  GLuint front_mask = 0xF0F0u;

  set_faces(front_func, 2, front_mask, GL_ALWAYS, 0, 0xFFFFFFFFu);

  int fail_count = 0;

  for (int i = 0; i < valid_func_count; i++) {
    GLenum func = valid_funcs[i];
    /* Bilincli olarak clamp gerektiren degerler de gonderilir */
    GLint ref_in = (i % 2 == 0) ? (i + 1) : (-(i + 1));
    GLint ref_expected = clamp_ref(ref_in);
    GLuint mask = (GLuint)(0x22u * (unsigned)(i + 1));

    glStencilFuncSeparate(GL_BACK, func, ref_in, mask);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_12, test_procedure, "GL_BACK yazma hatasi: 0x%X",
                    err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_state(front);
    get_back_state(back);

    if (!state_eq(back, func, ref_expected, mask)) {
      TEST_LOG_FAIL(test_case_12, test_procedure,
                    "arka yuz hatali: gonderilen ref=%d beklenen(0x%X, %d, "
                    "0x%X) gelen(0x%X, %d, 0x%X)",
                    ref_in, func, ref_expected, mask, back[0], back[1],
                    back[2]);
      fail_count++;
    }

    if (!state_eq(front, front_func, front_ref, front_mask)) {
      TEST_LOG_FAIL(test_case_12, test_procedure,
                    "on yuze sizinti: beklenen(0x%X, %d, 0x%X) "
                    "gelen(0x%X, %d, 0x%X)",
                    front_func, front_ref, front_mask, front[0], front[1],
                    front[2]);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_12, test_procedure);
  else
    TEST_LOG_FAIL(test_case_12, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 13 : GL_FRONT_AND_BACK Birlesik Yazma
 * ============================================================
 *
 * TEK KONU: GL_FRONT_AND_BACK'in her iki yuzu de ayni
 *           func / ref / mask degerleriyle guncellemesi.
 *
 * Her adimdan once iki yuz KASITLI olarak birbirinden farkli
 * degerlere ayrilir. Bu ayirma adimi olmasa, hicbir sey
 * yapmayan bir implementasyon da testi gecerdi.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_013(void) {
  reset_stencil_func();

  int fail_count = 0;

  for (int i = 0; i < valid_func_count; i++) {
    /* Adim 1: iki yuzu ayir ve ayrildigini dogrula */
    set_faces(GL_NEVER, 0, 0x00FFu, GL_ALWAYS, clamp_ref(1), 0xFF00u);

    GLint chk_front[3], chk_back[3];
    get_front_state(chk_front);
    get_back_state(chk_back);

    if (chk_front[0] == chk_back[0] && chk_front[2] == chk_back[2]) {
      TEST_LOG_FAIL(test_case_13, test_procedure,
                    "on/arka yuz ayrilamadi, test anlamsiz");
      fail_count++;
      break;
    }

    /* Adim 2: tek cagri ile ikisini birden yaz */
    GLenum func = valid_funcs[i];
    GLint ref_in = i;
    GLint ref_expected = clamp_ref(ref_in);
    GLuint mask = (GLuint)(0x33u * (unsigned)(i + 1));

    glStencilFuncSeparate(GL_FRONT_AND_BACK, func, ref_in, mask);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_13, test_procedure,
                    "GL_FRONT_AND_BACK yazma hatasi: 0x%X", err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_state(front);
    get_back_state(back);

    if (!state_eq(front, func, ref_expected, mask) ||
        !state_eq(back, func, ref_expected, mask)) {
      TEST_LOG_FAIL(test_case_13, test_procedure,
                    "beklenen(0x%X, %d, 0x%X) on(0x%X, %d, 0x%X) "
                    "arka(0x%X, %d, 0x%X)",
                    func, ref_expected, mask, front[0], front[1], front[2],
                    back[0], back[1], back[2]);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_13, test_procedure);
  else
    TEST_LOG_FAIL(test_case_13, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 14 : Yazma Sirasinin Sonucu Etkilememesi
 * ============================================================
 *
 * TEK KONU: Iki yuze yapilan yazmalarin SIRADAN bagimsiz olmasi.
 *
 * Farkli yuzlere yapilan yazmalar birbirinden bagimsiz oldugu
 * icin su iki dizinin sonucu ayni olmalidir:
 *
 *   A) glStencilFuncSeparate(GL_FRONT, ...)
 *      glStencilFuncSeparate(GL_BACK,  ...)
 *
 *   B) glStencilFuncSeparate(GL_BACK,  ...)
 *      glStencilFuncSeparate(GL_FRONT, ...)
 *
 * Beklenen degerler sabit kodlanmaz; iki sirali dizinin
 * urettigi alti state birbirine gore karsilastirilir.
 * Bu, ic state'in tek bir paylasilan alana yazildigi
 * (yani face bilgisinin kaybedildigi) implementasyonlari
 * yakalar.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_014(void) {
  reset_stencil_func();

  int fail_count = 0;

  for (int i = 0; i < valid_func_count; i++) {
    GLenum ffunc = valid_funcs[i];
    GLenum bfunc = valid_funcs[(i + 4) % valid_func_count];
    GLint fref = i;
    GLint bref = i + 2;
    GLuint fmask = (GLuint)(0x0Fu * (unsigned)(i + 1));
    GLuint bmask = (GLuint)(0xF0u * (unsigned)(i + 1));

    /* Sira A: once FRONT, sonra BACK */
    reset_stencil_func();
    glStencilFuncSeparate(GL_FRONT, ffunc, fref, fmask);
    glStencilFuncSeparate(GL_BACK, bfunc, bref, bmask);
    GLenum err_a = glGetError();

    GLint a_front[3], a_back[3];
    get_front_state(a_front);
    get_back_state(a_back);

    /* Sira B: once BACK, sonra FRONT */
    reset_stencil_func();
    glStencilFuncSeparate(GL_BACK, bfunc, bref, bmask);
    glStencilFuncSeparate(GL_FRONT, ffunc, fref, fmask);
    GLenum err_b = glGetError();

    GLint b_front[3], b_back[3];
    get_front_state(b_front);
    get_back_state(b_back);

    if (err_a != GL_NO_ERROR || err_b != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_14, test_procedure,
                    "i=%d hata: siraA=0x%X siraB=0x%X", i, err_a, err_b);
      fail_count++;
      continue;
    }

    for (int k = 0; k < 3; k++) {
      if (a_front[k] != b_front[k] || a_back[k] != b_back[k]) {
        TEST_LOG_FAIL(test_case_14, test_procedure,
                      "sira bagimliligi (idx=%d): A on=0x%X arka=0x%X | "
                      "B on=0x%X arka=0x%X",
                      k, a_front[k], a_back[k], b_front[k], b_back[k]);
        fail_count++;
        break;
      }
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_14, test_procedure);
  else
    TEST_LOG_FAIL(test_case_14, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 15 : Hatali Cagrinin Atomikligi
 * ============================================================
 *
 * TEK KONU: Hata ureten bir cagrinin hicbir state'i
 *           degistirmemesi.
 *
 * Bu fonksiyonda atomiklik ozellikle kritiktir: ref ve mask
 * KENDI BASLARINA hata uretmedikleri icin, parametreleri
 * sirayla isleyen bir surucu once ref ve mask'i yazip
 * sonra gecersiz func'a takilabilir. Sonuc, yarim yazilmis
 * bir state olur.
 *
 * Denenen desenler bu senaryoyu hedefler:
 *   gecersiz face + tamamen gecerli func/ref/mask
 *   gecerli face + gecersiz func + FARKLI ref/mask
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_015(void) {
  reset_stencil_func();

  GLenum front_func = GL_LEQUAL;
  GLint front_ref = clamp_ref(4);
  GLuint front_mask = 0x1234u;
  GLenum back_func = GL_GEQUAL;
  GLint back_ref = clamp_ref(6);
  GLuint back_mask = 0x5678u;

  struct {
    GLenum face;
    GLenum func;
    GLint ref;
    GLuint mask;
    const char *desc;
  } bad_calls[] = {
      {(GLenum)0xDEAD, GL_NEVER, 1, 0xAAAAu, "gecersiz face"},
      {(GLenum)0x0406, GL_ALWAYS, 2, 0xBBBBu, "face bosluk degeri"},
      {GL_FRONT, (GLenum)0x0208, 3, 0xCCCCu, "FRONT + gecersiz func"},
      {GL_BACK, (GLenum)0x01FF, 4, 0xDDDDu, "BACK + gecersiz func"},
      {GL_FRONT_AND_BACK, (GLenum)0xDEAD, 5, 0xEEEEu,
       "FRONT_AND_BACK + gecersiz func"},
      {(GLenum)0xDEAD, (GLenum)0xDEAD, 6, 0xFFFFu, "face ve func gecersiz"},
  };
  int count = sizeof(bad_calls) / sizeof(bad_calls[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    set_faces(front_func, front_ref, front_mask, back_func, back_ref,
              back_mask);

    glStencilFuncSeparate(bad_calls[i].face, bad_calls[i].func,
                          bad_calls[i].ref, bad_calls[i].mask);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_15, test_procedure,
                    "%s: beklenen=GL_INVALID_ENUM gelen=0x%X",
                    bad_calls[i].desc, err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_state(front);
    get_back_state(back);

    if (!state_eq(front, front_func, front_ref, front_mask) ||
        !state_eq(back, back_func, back_ref, back_mask)) {
      TEST_LOG_FAIL(test_case_15, test_procedure,
                    "%s: state yazildi - on(0x%X, %d, 0x%X) "
                    "arka(0x%X, %d, 0x%X)",
                    bad_calls[i].desc, front[0], front[1], front[2], back[0],
                    back[1], back[2]);
      fail_count++;
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_15, test_procedure);
  else
    TEST_LOG_FAIL(test_case_15, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 16 : glStencilFunc ile Esdegerlik (Differential Test)
 * ============================================================
 *
 * TEK KONU: glStencilFunc(f, r, m) cagrisinin
 *           glStencilFuncSeparate(GL_FRONT_AND_BACK, f, r, m)
 *           ile birebir ayni state'i uretmesi.
 *
 * Spec bu iki cagriyi esdeger tanimlar. Beklenen degerler
 * sabit kodlanmaz; ayni parametreler iki farkli API yolundan
 * uygulanip alti state sorgusunun sonucu birbirine gore
 * karsilastirilir.
 *
 * Clamp gerektiren ref degerleri de kullanilir; boylece
 * iki yolun AYNI clamp mantigini calistirdigi dogrulanir.
 * Karsilastirmadan once state kasitli olarak bozulur.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_016(void) {
  reset_stencil_func();

  GLint max_ref = get_max_stencil_ref();
  /* max_ref == INT_MAX ise +1 tasar; bu durumda INT_MAX kullanilir */
  GLint above_max = (max_ref == INT_MAX) ? INT_MAX : max_ref + 1;

  GLint test_refs[] = {0, 1, max_ref, above_max, -1, INT_MAX, INT_MIN};
  int ref_count = sizeof(test_refs) / sizeof(test_refs[0]);

  int fail_count = 0;

  for (int i = 0; i < valid_func_count; i++) {
    GLenum func = valid_funcs[i];
    GLint ref = test_refs[i % ref_count];
    GLuint mask = (GLuint)(0x0F0Fu * (unsigned)(i + 1));

    /* Yol A: glStencilFunc */
    set_faces(GL_NEVER, 0, 0x1111u, GL_ALWAYS, clamp_ref(1), 0x2222u);
    glStencilFunc(func, ref, mask);
    GLenum err_a = glGetError();

    GLint a_front[3], a_back[3];
    get_front_state(a_front);
    get_back_state(a_back);

    /* Yol B: glStencilFuncSeparate(GL_FRONT_AND_BACK, ...) */
    set_faces(GL_NEVER, 0, 0x1111u, GL_ALWAYS, clamp_ref(1), 0x2222u);
    glStencilFuncSeparate(GL_FRONT_AND_BACK, func, ref, mask);
    GLenum err_b = glGetError();

    GLint b_front[3], b_back[3];
    get_front_state(b_front);
    get_back_state(b_back);

    if (err_a != GL_NO_ERROR || err_b != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_16, test_procedure,
                    "func=0x%X ref=%d hata: StencilFunc=0x%X Separate=0x%X",
                    func, ref, err_a, err_b);
      fail_count++;
      continue;
    }

    /* Yol A gercekten yazmis mi? (bos implementasyon korumasi) */
    if (!state_eq(a_front, func, clamp_ref(ref), mask)) {
      TEST_LOG_FAIL(test_case_16, test_procedure,
                    "glStencilFunc yazamadi: beklenen(0x%X, %d, 0x%X) "
                    "gelen(0x%X, %d, 0x%X)",
                    func, clamp_ref(ref), mask, a_front[0], a_front[1],
                    a_front[2]);
      fail_count++;
      continue;
    }

    for (int k = 0; k < 3; k++) {
      if (a_front[k] != b_front[k] || a_back[k] != b_back[k]) {
        TEST_LOG_FAIL(test_case_16, test_procedure,
                      "esdegerlik bozuk (idx=%d, ref=%d): StencilFunc on=0x%X "
                      "arka=0x%X | Separate on=0x%X arka=0x%X",
                      k, ref, a_front[k], a_back[k], b_front[k], b_back[k]);
        fail_count++;
        break;
      }
    }
  }

  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_16, test_procedure);
  else
    TEST_LOG_FAIL(test_case_16, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 17 : Stencil Op ve WriteMask State'lerine Mudahale Etmeme
 * ============================================================
 *
 * glStencilFuncSeparate'in stencil op ve writemask
 *           state'lerini bozmamasi.
 *
 * glStencilOpSeparate ve glStencilMaskSeparate ile ayarlanan
 *   GL_STENCIL_FAIL / PASS_DEPTH_FAIL / PASS_DEPTH_PASS
 *   GL_STENCIL_WRITEMASK (+ GL_STENCIL_BACK_* karsiliklari)
 * degerleri, bir dizi gecerli ve gecersiz
 * glStencilFuncSeparate cagrisindan sonra aynen korunmalidir.
 *
 * On ve arka yuz KASITLI olarak farkli degerlere ayarlanir;
 * boylece "yuz bilgisi kaybi" da ayni anda gorunur olur.
 * Referans degerler varsayilmaz, ayarlandiktan sonra
 * geri okunur.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_017(void) {
  reset_stencil_func();

  static const GLenum neighbour_pnames[] = {
      GL_STENCIL_FAIL,
      GL_STENCIL_PASS_DEPTH_FAIL,
      GL_STENCIL_PASS_DEPTH_PASS,
      GL_STENCIL_WRITEMASK,
      GL_STENCIL_BACK_FAIL,
      GL_STENCIL_BACK_PASS_DEPTH_FAIL,
      GL_STENCIL_BACK_PASS_DEPTH_PASS,
      GL_STENCIL_BACK_WRITEMASK};
  const int pname_count =
      sizeof(neighbour_pnames) / sizeof(neighbour_pnames[0]);

  glStencilOpSeparate(GL_FRONT, GL_INCR, GL_DECR, GL_INVERT);
  glStencilOpSeparate(GL_BACK, GL_REPLACE, GL_ZERO, GL_INCR_WRAP);
  glStencilMaskSeparate(GL_FRONT, 0x0Fu);
  glStencilMaskSeparate(GL_BACK, 0xF0u);
  while (glGetError() != GL_NO_ERROR)
    ;

  GLint before[8];
  for (int i = 0; i < pname_count; i++)
    glGetIntegerv(neighbour_pnames[i], &before[i]);

  /* Gecerli ve gecersiz cagrilardan olusan karisik bir dizi */
  for (int i = 0; i < valid_func_count; i++) {
    glStencilFuncSeparate(GL_FRONT, valid_funcs[i], i, 0xAAAAu);
    glStencilFuncSeparate(GL_BACK, valid_funcs[i], -i, 0x5555u);
    glStencilFuncSeparate(GL_FRONT_AND_BACK, valid_funcs[i], INT_MAX,
                          0xFFFFFFFFu);
    glStencilFuncSeparate((GLenum)0xDEAD, valid_funcs[i], i, 0x1234u);
    glStencilFuncSeparate(GL_FRONT, (GLenum)0x0208, i, 0x1234u);
  }
  while (glGetError() != GL_NO_ERROR)
    ;

  GLint after[8];
  for (int i = 0; i < pname_count; i++)
    glGetIntegerv(neighbour_pnames[i], &after[i]);

  int fail_count = 0;

  for (int i = 0; i < pname_count; i++) {
    if (before[i] != after[i]) {
      TEST_LOG_FAIL(test_case_17, test_procedure,
                    "pname=0x%X bozuldu: once=0x%X sonra=0x%X",
                    neighbour_pnames[i], before[i], after[i]);
      fail_count++;
    }
  }

  glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilMaskSeparate(GL_FRONT_AND_BACK, 0xFFFFFFFFu);
  reset_stencil_func();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_17, test_procedure);
  else
    TEST_LOG_FAIL(test_case_17, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 18 : Karma API Akisi ile Golge Modelli Fuzzing
 * ============================================================
 *
 * TEK KONU: Rastgele parametre akisi altinda surucu state'inin
 *           bagimsiz bir referans modelle ayni kalmasi.
 *
 * Onceki paketlerdeki fuzzing'den farki, akisin TEK bir API
 * cagrisindan degil, ayni state'i paylasan IKI FARKLI API
 * yolundan olusmasidir:
 *
 *   glStencilFuncSeparate(face, ...)  ve  glStencilFunc(...)
 *
 * Ikisi rastgele sirayla karistirilir. Golge model, beklenen
 * hata kodunu ve clamp'lenmis ref degerini kendisi hesaplar;
 * her iterasyonda alti state sorgusu modelle karsilastirilir.
 * Boylece iki API yolunun ayni ic state'e tutarli sekilde
 * bagli oldugu yuk altinda dogrulanmis olur.
 * ============================================================ */
void GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_018(void) {
  reset_stencil_func();

  /* Golge model: [func, ref, mask] */
  GLenum sf_func = GL_ALWAYS, sb_func = GL_ALWAYS;
  GLint sf_ref = 0, sb_ref = 0;
  GLuint sf_mask = 0xFFFFFFFFu, sb_mask = 0xFFFFFFFFu;

  /* Modeli surucunun gercek baslangicina hizala */
  GLint init_front[3], init_back[3];
  get_front_state(init_front);
  get_back_state(init_back);
  sf_func = (GLenum)init_front[0];
  sf_ref = init_front[1];
  sf_mask = (GLuint)init_front[2];
  sb_func = (GLenum)init_back[0];
  sb_ref = init_back[1];
  sb_mask = (GLuint)init_back[2];

  unsigned int seed = 31337;
  int err_mismatch = 0;
  int state_mismatch = 0;
  int separate_calls = 0;
  int plain_calls = 0;
  int first_fail = -1;

  for (int i = 0; i < 25000; i++) {
    GLenum face, func;
    GLint ref;
    GLuint mask;

    seed = seed * 1103515245 + 12345;
    int use_separate = ((seed >> 16) & 1);

    seed = seed * 1103515245 + 12345;
    if (i % 3 == 0)
      face = valid_faces[(seed >> 16) % valid_face_count];
    else
      face = (GLenum)((seed >> 16) % 0x10000);

    seed = seed * 1103515245 + 12345;
    /* func'larin cogunlugu gecerli olsun ki gecerli yol da taransin */
    if ((seed >> 16) % 4 != 0)
      func = valid_funcs[(seed >> 8) % valid_func_count];
    else
      func = (GLenum)((seed >> 16) % 0x10000);

    seed = seed * 1103515245 + 12345;
    ref = (GLint)((seed >> 16) % 0x10000) - 2048; /* negatifleri de kapsar */

    seed = seed * 1103515245 + 12345;
    mask = (GLuint)seed;

    int should_pass;
    GLenum expected_err;

    if (use_separate) {
      separate_calls++;
      should_pass = is_valid_face(face) && is_valid_func(func);
      expected_err = should_pass ? GL_NO_ERROR : GL_INVALID_ENUM;
      glStencilFuncSeparate(face, func, ref, mask);
    } else {
      plain_calls++;
      /* glStencilFunc her iki yuzu birden etkiler; face parametresi yok */
      should_pass = is_valid_func(func);
      expected_err = should_pass ? GL_NO_ERROR : GL_INVALID_ENUM;
      glStencilFunc(func, ref, mask);
    }

    GLenum err = glGetError();

    if (err != expected_err) {
      if (first_fail < 0) {
        first_fail = i;
        TEST_LOG_FAIL(test_case_18, test_procedure,
                      "i=%d api=%s face=0x%X func=0x%X beklenen=0x%X gelen=0x%X",
                      i, use_separate ? "Separate" : "Func", face, func,
                      expected_err, err);
      }
      err_mismatch++;
      /* Model ile surucu ayrismis olabilir; yeniden hizala */
      reset_stencil_func();
      sf_func = sb_func = GL_ALWAYS;
      sf_ref = sb_ref = 0;
      {
        GLint f[3], b[3];
        get_front_state(f);
        get_back_state(b);
        sf_mask = (GLuint)f[2];
        sb_mask = (GLuint)b[2];
        sf_func = (GLenum)f[0];
        sb_func = (GLenum)b[0];
        sf_ref = f[1];
        sb_ref = b[1];
      }
      continue;
    }

    if (should_pass) {
      GLint clamped = clamp_ref(ref);
      int touch_front = !use_separate || face == GL_FRONT ||
                        face == GL_FRONT_AND_BACK;
      int touch_back =
          !use_separate || face == GL_BACK || face == GL_FRONT_AND_BACK;

      if (touch_front) {
        sf_func = func;
        sf_ref = clamped;
        sf_mask = mask;
      }
      if (touch_back) {
        sb_func = func;
        sb_ref = clamped;
        sb_mask = mask;
      }
    }

    GLint front[3], back[3];
    get_front_state(front);
    get_back_state(back);

    if (!state_eq(front, sf_func, sf_ref, sf_mask) ||
        !state_eq(back, sb_func, sb_ref, sb_mask)) {
      if (first_fail < 0) {
        first_fail = i;
        TEST_LOG_FAIL(test_case_18, test_procedure,
                      "i=%d api=%s model uyusmazligi: on beklenen(0x%X, %d, "
                      "0x%X) gelen(0x%X, %d, 0x%X) | arka beklenen(0x%X, %d, "
                      "0x%X) gelen(0x%X, %d, 0x%X)",
                      i, use_separate ? "Separate" : "Func", sf_func, sf_ref,
                      sf_mask, front[0], front[1], front[2], sb_func, sb_ref,
                      sb_mask, back[0], back[1], back[2]);
      }
      state_mismatch++;

      /* Modeli surucuye gore yeniden hizala ki hata katlanarak buyumesin */
      sf_func = (GLenum)front[0];
      sf_ref = front[1];
      sf_mask = (GLuint)front[2];
      sb_func = (GLenum)back[0];
      sb_ref = back[1];
      sb_mask = (GLuint)back[2];
    }
  }

  TEST_LOG_INFO("fuzzing - Separate: %d, StencilFunc: %d", separate_calls,
                plain_calls);

  while (glGetError() != GL_NO_ERROR)
    ;
  reset_stencil_func();

  GLenum final_err = glGetError();
  GLint front[3], back[3];
  get_front_state(front);
  get_back_state(back);
  int recovered = state_eq(front, GL_ALWAYS, 0, 0xFFFFFFFFu) &&
                  state_eq(back, GL_ALWAYS, 0, 0xFFFFFFFFu);

  if (err_mismatch == 0 && state_mismatch == 0 && final_err == GL_NO_ERROR &&
      recovered)
    TEST_LOG_SUCCESS(test_case_18, test_procedure);
  else
    TEST_LOG_FAIL(test_case_18, test_procedure,
                  "hata_kodu_uyusmazligi=%d state_uyusmazligi=%d ilk=%d "
                  "final=0x%X toparlanma=%d",
                  err_mismatch, state_mismatch, first_fail, final_err,
                  recovered);
}

void GS_GL20SC_PFO_SFS_ROBUSTNESS_TP_001_init(void) {
  /* TC_001 varsayilan state'i dogruladigi icin ILK calismalidir. */
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_004();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_005();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_006();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_007();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_008();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_009();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_010();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_011();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_012();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_013();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_014();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_015();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_016();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_017();
  GS_GL20SC_PFO_SFS_ROBUSTNESS_TC_018();

  GS_GL20SC_PFO_SFS_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_SFS_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_PFO_SFS_ROBUSTNESS_TP_001_close(void) {
  glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, 0xFFFFFFFF);
  glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilMaskSeparate(GL_FRONT_AND_BACK, 0xFFFFFFFF);
  glDisable(GL_STENCIL_TEST);
  while (glGetError() != GL_NO_ERROR)
    ;
}
