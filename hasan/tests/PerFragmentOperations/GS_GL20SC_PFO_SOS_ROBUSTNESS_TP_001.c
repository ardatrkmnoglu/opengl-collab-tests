#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <limits.h>

/*
 * GL20SC - PerFragmentOperations - StencilOpSeparate - ROBUSTNESS
 *
 * glStencilOpSeparate(GLenum face, GLenum sfail,
 *                     GLenum dpfail, GLenum dppass)
 *
 * Fonksiyonun ayirt edici ozelligi, stencil islemlerinin
 * on ve arka yuz icin BAGIMSIZ olarak ayarlanabilmesidir.
 * Bu nedenle bu paketin agirlik merkezi "yuz izolasyonu"dur:
 * bir yuze yazmak digerini kesinlikle etkilememelidir.
 *
 * Test edilen state sorgulari:
 *   On yuz  : GL_STENCIL_FAIL
 *             GL_STENCIL_PASS_DEPTH_FAIL
 *             GL_STENCIL_PASS_DEPTH_PASS
 *   Arka yuz: GL_STENCIL_BACK_FAIL
 *             GL_STENCIL_BACK_PASS_DEPTH_FAIL
 *             GL_STENCIL_BACK_PASS_DEPTH_PASS
 *
 * Gecerli face degerleri:
 *   GL_FRONT           0x0404
 *   GL_BACK            0x0405
 *   GL_FRONT_AND_BACK  0x0408
 *
 * Gecerli op degerleri (bitisik olmayan enum kumesi):
 *   GL_ZERO 0x0000, GL_INVERT 0x150A, GL_KEEP 0x1E00,
 *   GL_REPLACE 0x1E01, GL_INCR 0x1E02, GL_DECR 0x1E03,
 *   GL_INCR_WRAP 0x8507, GL_DECR_WRAP 0x8508
 *
 */

static const char *test_procedure = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_007";
static const char *test_case_8 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_008";
static const char *test_case_9 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_009";
static const char *test_case_10 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_010";
static const char *test_case_11 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_011";
static const char *test_case_12 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_012";
static const char *test_case_13 = "GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_013";

void GS_GL20SC_PFO_SOS_ROBUSTNESS_TP_001_close(void);

/* ------------------------------------------------------------ *
 * Ortak veri ve yardimci fonksiyonlar
 * ------------------------------------------------------------ */

static const GLenum valid_faces[] = {GL_FRONT, GL_BACK, GL_FRONT_AND_BACK};
static const int valid_face_count =
    sizeof(valid_faces) / sizeof(valid_faces[0]);

static const GLenum valid_ops[] = {
    GL_ZERO, GL_KEEP,   GL_REPLACE,   GL_INCR,
    GL_DECR, GL_INVERT, GL_INCR_WRAP, GL_DECR_WRAP,
};
static const int valid_op_count = sizeof(valid_ops) / sizeof(valid_ops[0]);

static int is_valid_face(GLenum val) {
  for (int i = 0; i < valid_face_count; i++) {
    if (val == valid_faces[i])
      return 1;
  }
  return 0;
}

static int is_valid_op(GLenum val) {
  for (int i = 0; i < valid_op_count; i++) {
    if (val == valid_ops[i])
      return 1;
  }
  return 0;
}

/* On yuz uclusunu tek seferde okur. */
static void get_front_ops(GLint out[3]) {
  glGetIntegerv(GL_STENCIL_FAIL, &out[0]);
  glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &out[1]);
  glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &out[2]);
}

/* Arka yuz uclusunu tek seferde okur. */
static void get_back_ops(GLint out[3]) {
  glGetIntegerv(GL_STENCIL_BACK_FAIL, &out[0]);
  glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &out[1]);
  glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &out[2]);
}

/* Okunan uclu ile beklenen ucluyu karsilastirir. */
static int triple_eq(const GLint got[3], const GLenum expected[3]) {
  return (GLenum)got[0] == expected[0] && (GLenum)got[1] == expected[1] &&
         (GLenum)got[2] == expected[2];
}

/* Her iki yuzu bilinen bir baslangica cekip hata kuyrugunu bosaltir.
   Kasitli olarak on ve arka yuz FARKLI degerlere ayarlanabilsin diye
   iki ayri cagri kullanilir. */
static void set_faces(const GLenum front[3], const GLenum back[3]) 
{
  glStencilOpSeparate(GL_FRONT, front[0], front[1], front[2]);
  glStencilOpSeparate(GL_BACK, back[0], back[1], back[2]);
  while (glGetError() != GL_NO_ERROR)
    ;
}

static void reset_stencil_ops(void) {
  glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
  while (glGetError() != GL_NO_ERROR)
    ;
}

/* ============================================================
 * TEST 1 : Varsayilan Baslangic Durumu
 * ============================================================
 *
 * Context olusturuldugunda alti stencil op state'inin
 * de GL_KEEP olmasi.
 *
 * Spec'e gore GL_STENCIL_FAIL, GL_STENCIL_PASS_DEPTH_FAIL,
 * GL_STENCIL_PASS_DEPTH_PASS ve bunlarin GL_STENCIL_BACK_*
 * karsiliklarinin baslangic degeri GL_KEEP'tir.
 *
 * Bu test paketin ILK calisan testidir; henuz hicbir
 * glStencilOpSeparate cagrisi yapilmamis olmalidir.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR)
    ;

  GLenum expected[3] = {GL_KEEP, GL_KEEP, GL_KEEP};
  GLint front[3], back[3];

  get_front_ops(front);
  get_back_ops(back);

  GLenum err = glGetError();

  if (err == GL_NO_ERROR && triple_eq(front, expected) &&
      triple_eq(back, expected))
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
  else
    TEST_LOG_FAIL(test_case_1, test_procedure,
                  "Varsayilan state hatali: on(0x%X, 0x%X, 0x%X) "
                  "arka(0x%X, 0x%X, 0x%X) err=0x%X",
                  front[0], front[1], front[2], back[0], back[1], back[2], err);
}

/* ============================================================
 * TEST 2 : face Parametresi - 16-bit Enum Sweep
 * ============================================================
 *
 * 0x0000..0xFFFF araligindaki TUM degerler icinde yalnizca
 * 3 face degerinin kabul edilmesi.
 *
 * op parametreleri sabit ve gecerli (GL_KEEP) tutulur;
 * dolayisiyla uretilen tek hata kaynagi face olabilir.
 * Bu, "hangi parametre reddedildi" belirsizligini ortadan kaldirir.
 *
 * Sweep, ayri bir sinir testi gerektirmeyecek sekilde tum
 * kritik komsulari zaten kapsar; ozellikle:
 *   0x0403          -> GL_FRONT'un bir alti
 *   0x0406 / 0x0407 -> GL_BACK (0x0405) ile GL_FRONT_AND_BACK
 *                      (0x0408) arasindaki BOSLUK
 *   0x0409          -> GL_FRONT_AND_BACK'in bir ustu
 * face degerlerini aralik olarak kontrol eden hatali suruculer
 * (0x0404 <= face <= 0x0408) tam olarak bu bosluktan yakalanir.
 *
 * Bu test yalnizca hata kodunu dogrular; state korunumu
 * TC_009'un konusudur.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_002(void) {
  reset_stencil_ops();

  GLenum face;
  int accepted_count = 0;
  int fail_count = 0;
  int first_fail = -1;

  for (face = 0; face < 65536; face++) {
    GLenum expected = is_valid_face(face) ? GL_NO_ERROR : GL_INVALID_ENUM;

    glStencilOpSeparate(face, GL_KEEP, GL_KEEP, GL_KEEP);
    GLenum err = glGetError();

    if (err == GL_NO_ERROR)
      accepted_count++;

    if (err != expected) {
      if (first_fail < 0) {
        first_fail = (int)face;
        TEST_LOG_FAIL(test_case_2, test_procedure,
                      "face=0x%X beklenen=0x%X gelen=0x%X", face, expected,
                      err);
      }
      fail_count++;
    }
  }

  TEST_LOG_INFO("face sweep - kabul edilen deger sayisi: %d (beklenen: %d)",
                accepted_count, valid_face_count);

  if (fail_count == 0 && accepted_count == valid_face_count)
    TEST_LOG_SUCCESS(test_case_2, test_procedure);
  else
    TEST_LOG_FAIL(test_case_2, test_procedure,
                  "Basarisiz: %d, kabul edilen: %d", fail_count,
                  accepted_count);
}

/* ============================================================
 * TEST 3 : op Parametreleri - 16-bit Enum Sweep
 * ============================================================
 *
 * 0x0000..0xFFFF araligindaki TUM degerler icinde yalnizca
 * 8 op degerinin kabul edilmesi.
 *
 * Sweep uc op pozisyonunun her biri icin ayri tekrarlanir
 * (3 x 65536 cagri); boylece yalnizca ilk op parametresini
 * dogrulayan (digerlerini atlayan) suruculer de yakalanir.
 *
 * Her pozisyonda kabul edilen deger sayisinin tam olarak 8
 * olmasi beklenir; bu sayac, "fazladan enum kabul eden"
 * suruculeri hata kodu karsilastirmasindan bagimsiz olarak
 * da yakalar.
 *
 * Gecerli kumenin bitisik olmamasi nedeniyle kritik olan
 * komsu degerler (0x0001, 0x1509, 0x150B, 0x1DFF, 0x1E04,
 * 0x8506, 0x8509) sweep icinde zaten denenmis olur.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_003(void) {
  reset_stencil_ops();

  int fail_count = 0;

  for (int pos = 0; pos < 3; pos++) {
    GLenum mode;
    int accepted_count = 0;
    int first_fail = -1;

    for (mode = 0; mode < 65536; mode++) {
      GLenum expected = is_valid_op(mode) ? GL_NO_ERROR : GL_INVALID_ENUM;

      GLenum args[3] = {GL_KEEP, GL_KEEP, GL_KEEP};
      args[pos] = mode;

      glStencilOpSeparate(GL_FRONT_AND_BACK, args[0], args[1], args[2]);
      GLenum err = glGetError();

      if (err == GL_NO_ERROR)
        accepted_count++;

      if (err != expected) {
        if (first_fail < 0) {
          first_fail = (int)mode;
          TEST_LOG_FAIL(test_case_3, test_procedure,
                        "pos=%d mode=0x%X beklenen=0x%X gelen=0x%X", pos, mode,
                        expected, err);
        }
        fail_count++;
      }
    }

    TEST_LOG_INFO("op sweep pos=%d - kabul edilen: %d (beklenen: %d)", pos,
                  accepted_count, valid_op_count);

    if (accepted_count != valid_op_count)
      fail_count++;
  }

  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_3, test_procedure);
  else
    TEST_LOG_FAIL(test_case_3, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 4 : 32-bit Tasma Davranisi
 * ============================================================
 *
 * 16-bit sweep'in disinda kalan degerlerin tasma olmadan
 * reddedilmesi.
 *
 * Ozellikle dusuk 16 biti gecerli bir enuma esit olan
 * degerler secilmistir:
 *
 *   0x00010404 -> alt 16 bit = GL_FRONT
 *   0x00018507 -> alt 16 bit = GL_INCR_WRAP
 *   0xFFFF0408 -> alt 16 bit = GL_FRONT_AND_BACK
 *
 * Karsilastirmayi 16-bit'e kirpan bir surucu bu degerleri
 * yanlislikla kabul eder. Deger hem face hem op
 * pozisyonlarinda denenir.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_004(void) {
  reset_stencil_ops();

  GLenum extreme[] = {(GLenum)INT_MAX,    (GLenum)INT_MIN,
                      (GLenum)0xFFFFFFFF, (GLenum)0x80000000,
                      (GLenum)0x00010000, (GLenum)0x00010404,
                      (GLenum)0x00018507, (GLenum)0xFFFF0408};
  int count = sizeof(extreme) / sizeof(extreme[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    /* face pozisyonu */
    glStencilOpSeparate(extreme[i], GL_KEEP, GL_KEEP, GL_KEEP);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_4, test_procedure,
                    "face=0x%X beklenen=GL_INVALID_ENUM gelen=0x%X", extreme[i],
                    err);
      fail_count++;
    }

    /* op pozisyonlari */
    for (int pos = 0; pos < 3; pos++) {
      GLenum args[3] = {GL_KEEP, GL_KEEP, GL_KEEP};
      args[pos] = extreme[i];

      glStencilOpSeparate(GL_FRONT_AND_BACK, args[0], args[1], args[2]);
      err = glGetError();

      if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_4, test_procedure,
                      "op=0x%X pos=%d beklenen=GL_INVALID_ENUM gelen=0x%X",
                      extreme[i], pos, err);
        fail_count++;
      }
    }
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_4, test_procedure);
  else
    TEST_LOG_FAIL(test_case_4, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 5 : GL_FRONT Yuz Izolasyonu
 * ============================================================
 *
 * GL_FRONT ile yapilan yazmanin arka yuz state'ine
 * HIC dokunmamasi.
 *
 * Arka yuz, on yuzden farkli ve kolay ayirt edilir bir
 * ucluye (GL_INVERT, GL_ZERO, GL_DECR_WRAP) sabitlenir.
 * Ardindan 8 gecerli op degeri on yuze yazilir ve her
 * yazmadan sonra arka yuzun degismedigi dogrulanir.
 *
 * Bu, glStencilOpSeparate'i icten glStencilOp'a yonlendiren
 * (yani her iki yuzu birden yazan) hatali implementasyonlari
 * dogrudan yakalar.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_005(void) {
  reset_stencil_ops();

  GLenum back_ref[3] = {GL_INVERT, GL_ZERO, GL_DECR_WRAP};
  GLenum front_init[3] = {GL_KEEP, GL_KEEP, GL_KEEP};

  set_faces(front_init, back_ref);

  int fail_count = 0;

  for (int i = 0; i < valid_op_count; i++) {
    GLenum front_new[3] = {valid_ops[i], valid_ops[(i + 3) % valid_op_count],
                           valid_ops[(i + 5) % valid_op_count]};

    glStencilOpSeparate(GL_FRONT, front_new[0], front_new[1], front_new[2]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_5, test_procedure, "GL_FRONT yazma hatasi: 0x%X",
                    err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_ops(front);
    get_back_ops(back);

    if (!triple_eq(front, front_new)) {
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "on yuz yazilmadi: beklenen(0x%X, 0x%X, 0x%X) "
                    "gelen(0x%X, 0x%X, 0x%X)",
                    front_new[0], front_new[1], front_new[2], front[0],
                    front[1], front[2]);
      fail_count++;
    }

    if (!triple_eq(back, back_ref)) {
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "arka yuze sizinti: beklenen(0x%X, 0x%X, 0x%X) "
                    "gelen(0x%X, 0x%X, 0x%X)",
                    back_ref[0], back_ref[1], back_ref[2], back[0], back[1],
                    back[2]);
      fail_count++;
    }
  }

  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
  else
    TEST_LOG_FAIL(test_case_5, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 6 : GL_BACK Yuz Izolasyonu
 * ============================================================
 *
 * GL_BACK ile yapilan yazmanin on yuz state'ine
 * HIC dokunmamasi.
 *
 * TC_005'in simetrigidir. On yuz ayirt edici bir ucluye
 * (GL_REPLACE, GL_INCR_WRAP, GL_INVERT) sabitlenir ve
 * arka yuze yapilan her yazmadan sonra on yuz dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_006(void) {
  reset_stencil_ops();

  GLenum front_ref[3] = {GL_REPLACE, GL_INCR_WRAP, GL_INVERT};
  GLenum back_init[3] = {GL_KEEP, GL_KEEP, GL_KEEP};

  set_faces(front_ref, back_init);

  int fail_count = 0;

  for (int i = 0; i < valid_op_count; i++) {
    GLenum back_new[3] = {valid_ops[i], valid_ops[(i + 3) % valid_op_count],
                          valid_ops[(i + 5) % valid_op_count]};

    glStencilOpSeparate(GL_BACK, back_new[0], back_new[1], back_new[2]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_6, test_procedure, "GL_BACK yazma hatasi: 0x%X",
                    err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_ops(front);
    get_back_ops(back);

    if (!triple_eq(back, back_new)) {
      TEST_LOG_FAIL(test_case_6, test_procedure,
                    "arka yuz yazilmadi: beklenen(0x%X, 0x%X, 0x%X) "
                    "gelen(0x%X, 0x%X, 0x%X)",
                    back_new[0], back_new[1], back_new[2], back[0], back[1],
                    back[2]);
      fail_count++;
    }

    if (!triple_eq(front, front_ref)) {
      TEST_LOG_FAIL(test_case_6, test_procedure,
                    "on yuze sizinti: beklenen(0x%X, 0x%X, 0x%X) "
                    "gelen(0x%X, 0x%X, 0x%X)",
                    front_ref[0], front_ref[1], front_ref[2], front[0],
                    front[1], front[2]);
      fail_count++;
    }
  }

  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
  else
    TEST_LOG_FAIL(test_case_6, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 7 : GL_FRONT_AND_BACK Yazma Davranışı
 * ============================================================
 *
 * glStencilOpSeparate() fonksiyonunda face parametresi
 * GL_FRONT_AND_BACK olarak verildiğinde, stencil
 * operasyonlarının hem ön (front) hem de arka (back)
 * yüz için aynı anda güncellenmesi gerekir.
 *
 * Test önce ön ve arka yüzü farklı değerlere ayırır,
 * ardından GL_FRONT_AND_BACK ile tek bir yazma işlemi
 * gerçekleştirir ve her iki yüzün de aynı stencil
 * operasyonlarını içerdiğini doğrular.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_007(void) {
  reset_stencil_ops();

  int fail_count = 0;

  for (int i = 0; i < valid_op_count; i++) {
    /* Adim 1: iki yuzu birbirinden ayir */
    GLenum front_seed[3] = {GL_ZERO, GL_ZERO, GL_ZERO};
    GLenum back_seed[3] = {GL_INVERT, GL_INVERT, GL_INVERT};
    set_faces(front_seed, back_seed);

    GLint chk_front[3], chk_back[3];
    get_front_ops(chk_front);
    get_back_ops(chk_back);

    if (triple_eq(chk_front, back_seed)) {
      TEST_LOG_FAIL(test_case_7, test_procedure,
                    "on/arka yuz ayrilamadi, test anlamsiz");
      fail_count++;
      break;
    }

    /* Adim 2: tek cagri ile ikisini birden yaz */
    GLenum both[3] = {valid_ops[i], valid_ops[(i + 2) % valid_op_count],
                      valid_ops[(i + 4) % valid_op_count]};

    glStencilOpSeparate(GL_FRONT_AND_BACK, both[0], both[1], both[2]);
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_7, test_procedure,
                    "GL_FRONT_AND_BACK yazma hatasi: 0x%X", err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_ops(front);
    get_back_ops(back);

    if (!triple_eq(front, both) || !triple_eq(back, both)) {
      TEST_LOG_FAIL(test_case_7, test_procedure,
                    "beklenen(0x%X, 0x%X, 0x%X) on(0x%X, 0x%X, 0x%X) "
                    "arka(0x%X, 0x%X, 0x%X)",
                    both[0], both[1], both[2], front[0], front[1], front[2],
                    back[0], back[1], back[2]);
      fail_count++;
    }
  }

  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_7, test_procedure);
  else
    TEST_LOG_FAIL(test_case_7, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 8 : Tum Gecerli Kombinasyonlarin Erisilebilirligi
 * ============================================================
 *
 * 8 x 8 x 8 = 512 gecerli op uclusunun her birinin
 * her gecerli face degeriyle yazilabilmesi.
 *
 * Yalnizca hedeflenen yuzun dogru degeri aldigi kontrol edilir
 * Toplam 3 x 512 = 1536
 * gecerli cagri yapilir ve hicbirinin reddedilmemesi beklenir.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_008(void) {
  reset_stencil_ops();

  int fail_count = 0;
  int combo_count = 0;

  for (int f = 0; f < valid_face_count; f++) {
    GLenum face = valid_faces[f];

    for (int a = 0; a < valid_op_count; a++) {
      for (int b = 0; b < valid_op_count; b++) {
        for (int c = 0; c < valid_op_count; c++) {
          GLenum expected[3] = {valid_ops[a], valid_ops[b], valid_ops[c]};

          combo_count++;

          glStencilOpSeparate(face, expected[0], expected[1], expected[2]);
          GLenum err = glGetError();

          if (err != GL_NO_ERROR) {
            if (fail_count == 0)
              TEST_LOG_FAIL(test_case_8, test_procedure,
                            "face=0x%X (0x%X, 0x%X, 0x%X) reddedildi: err=0x%X",
                            face, expected[0], expected[1], expected[2], err);
            fail_count++;
            continue;
          }

          GLint got[3];
          if (face == GL_BACK)
            get_back_ops(got);
          else
            get_front_ops(got);

          if (!triple_eq(got, expected)) {
            if (fail_count == 0)
              TEST_LOG_FAIL(test_case_8, test_procedure,
                            "face=0x%X beklenen(0x%X, 0x%X, 0x%X) "
                            "gelen(0x%X, 0x%X, 0x%X)",
                            face, expected[0], expected[1], expected[2], got[0],
                            got[1], got[2]);
            fail_count++;
          }
        }
      }
    }
  }

  TEST_LOG_INFO("denenen kombinasyon: %d", combo_count);

  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_8, test_procedure);
  else
    TEST_LOG_FAIL(test_case_8, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 9 : Hatali Cagrinin Atomikligi
 * ============================================================
 *
 * Hata ureten bir cagrinin HICBIR state'i degistirmemesi.
 *
 * GL semantiginde hata ureten bir komut, hata bayragi disinda
 * GL state'ini degistirmez. Burada kritik senaryo, ilk uc
 * parametresi gecerli olup son parametresi gecersiz olan
 * cagrilardir: parametreleri sirayla isleyip son parametrede
 * hata veren bir surucu, ilk iki op degerini coktan yazmis olur.
 *
 * Denenen desenler:
 *   gecersiz face + gecerli op'lar
 *   gecerli face + son op gecersiz   (en kritik durum)
 *   gecerli face + ortadaki op gecersiz
 *   gecersiz face + gecersiz op'lar
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_009(void) {
  reset_stencil_ops();

  GLenum front_ref[3] = {GL_ZERO, GL_REPLACE, GL_INCR};
  GLenum back_ref[3] = {GL_INVERT, GL_DECR, GL_INCR_WRAP};

  struct {
    GLenum face;
    GLenum sfail;
    GLenum dpfail;
    GLenum dppass;
    const char *desc;
  } bad_calls[] = {
      {(GLenum)0xDEAD, GL_KEEP, GL_KEEP, GL_KEEP, "gecersiz face"},
      {GL_FRONT, GL_KEEP, GL_KEEP, (GLenum)0xDEAD, "FRONT son op gecersiz"},
      {GL_BACK, GL_KEEP, GL_KEEP, (GLenum)0xDEAD, "BACK son op gecersiz"},
      {GL_FRONT, GL_KEEP, (GLenum)0x1E04, GL_KEEP, "FRONT orta op gecersiz"},
      {GL_FRONT_AND_BACK, (GLenum)0x0001, GL_KEEP, GL_KEEP,
       "FRONT_AND_BACK ilk op gecersiz"},
      {(GLenum)0x0406, (GLenum)0xBEEF, (GLenum)0xBEEF, (GLenum)0xBEEF,
       "hepsi gecersiz"},
  };
  int count = sizeof(bad_calls) / sizeof(bad_calls[0]);

  int fail_count = 0;

  for (int i = 0; i < count; i++) {
    set_faces(front_ref, back_ref);

    glStencilOpSeparate(bad_calls[i].face, bad_calls[i].sfail,
                        bad_calls[i].dpfail, bad_calls[i].dppass);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_9, test_procedure,
                    "%s: beklenen=GL_INVALID_ENUM gelen=0x%X",
                    bad_calls[i].desc, err);
      fail_count++;
      continue;
    }

    GLint front[3], back[3];
    get_front_ops(front);
    get_back_ops(back);

    if (!triple_eq(front, front_ref) || !triple_eq(back, back_ref)) {
      TEST_LOG_FAIL(test_case_9, test_procedure,
                    "%s: state yazildi - on(0x%X, 0x%X, 0x%X) "
                    "arka(0x%X, 0x%X, 0x%X)",
                    bad_calls[i].desc, front[0], front[1], front[2], back[0],
                    back[1], back[2]);
      fail_count++;
    }
  }

  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_9, test_procedure);
  else
    TEST_LOG_FAIL(test_case_9, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 10 : Hata Kuyrugu Tekilligi
 * ============================================================
 *
 * Birden fazla parametresi gecersiz olan TEK bir cagrinin,
 * kuyruga birden fazla hata birakmamasi.
 *
 * Dort parametresi de gecersiz olan bir cagri yapildiktan
 * sonra ilk glGetError() GL_INVALID_ENUM dondurmeli, hemen
 * ardindan gelen glGetError() ise GL_NO_ERROR dondurmelidir.
 * Aksi halde hata kuyrugunda birikme olur ve sonraki testler
 * "hayalet" hatalarla karsilasir.
 *
 * NOT: Spec, implementasyonlarin birden fazla hata bayragi
 * tutmasina izin verir; ancak tek bir gecersiz cagri icin
 * beklenen davranis tek hata kaydidir. Bu test o nedenle
 * bir robustness beklentisidir.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_010(void) {
  reset_stencil_ops();

  int fail_count = 0;

  /* Dort parametre de gecersiz */
  glStencilOpSeparate((GLenum)0xDEAD, (GLenum)0xDEAD, (GLenum)0xDEAD,
                      (GLenum)0xDEAD);

  GLenum first = glGetError();
  GLenum second = glGetError();

  if (first != GL_INVALID_ENUM) {
    TEST_LOG_FAIL(test_case_10, test_procedure,
                  "ilk hata beklenen=GL_INVALID_ENUM gelen=0x%X", first);
    fail_count++;
  }

  if (second != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_10, test_procedure,
                  "kuyrukta artik hata var: 0x%X", second);
    fail_count++;
  }

  /* Kuyrukta baska bir sey kaldiysa temizle ve sayisini bildir */
  int leftover = 0;
  while (glGetError() != GL_NO_ERROR) {
    leftover++;
    if (leftover > 32)
      break;
  }

  if (leftover != 0) {
    TEST_LOG_FAIL(test_case_10, test_procedure, "fazladan hata sayisi: %d",
                  leftover);
    fail_count++;
  }

  /* Gecerli bir cagri kuyruga hic hata birakmamali */
  glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
  GLenum clean = glGetError();

  if (clean != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_10, test_procedure,
                  "gecerli cagri hata uretti: 0x%X", clean);
    fail_count++;
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_10, test_procedure);
  else
    TEST_LOG_FAIL(test_case_10, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 11 : Komsu Stencil State'lerine Mudahale Etmeme
 * ============================================================
 *
 * glStencilOpSeparate'in stencil op DISINDAKI stencil
 * state'lerini bozmamasi.
 *
 * glStencilFunc ve glStencilMask ile ayarlanan
 *   GL_STENCIL_FUNC / GL_STENCIL_REF / GL_STENCIL_VALUE_MASK
 *   GL_STENCIL_WRITEMASK (+ GL_STENCIL_BACK_* karsiliklari)
 * degerleri, bir dizi gecerli ve gecersiz
 * glStencilOpSeparate cagrisindan sonra aynen korunmalidir.
 *
 * Referans degerler varsayilmaz; ayarlandiktan hemen sonra
 * GERI OKUNUR (ornegin ref degeri stencil bit derinligine
 * gore clamp edilebilir) ve karsilastirma bu okunan
 * degerlere gore yapilir.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_011(void) {
  reset_stencil_ops();

  static const GLenum neighbour_pnames[] = {
      GL_STENCIL_FUNC,       GL_STENCIL_REF,
      GL_STENCIL_VALUE_MASK, GL_STENCIL_WRITEMASK,
      GL_STENCIL_BACK_FUNC,  GL_STENCIL_BACK_REF,
      GL_STENCIL_BACK_VALUE_MASK, GL_STENCIL_BACK_WRITEMASK};
  const int pname_count =
      sizeof(neighbour_pnames) / sizeof(neighbour_pnames[0]);

  glStencilFunc(GL_LEQUAL, 5, 0x5A);
  glStencilMask(0x0F);
  while (glGetError() != GL_NO_ERROR)
    ;

  /* Referans degerler geri okunarak alinir */
  GLint before[8];
  for (int i = 0; i < pname_count; i++)
    glGetIntegerv(neighbour_pnames[i], &before[i]);

  /* Gecerli ve gecersiz cagrilardan olusan karisik bir dizi */
  for (int i = 0; i < valid_op_count; i++) {
    glStencilOpSeparate(GL_FRONT, valid_ops[i], valid_ops[i], valid_ops[i]);
    glStencilOpSeparate(GL_BACK, valid_ops[i], GL_KEEP, valid_ops[i]);
    glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, valid_ops[i], GL_KEEP);
    glStencilOpSeparate((GLenum)0xDEAD, valid_ops[i], valid_ops[i],
                        valid_ops[i]);
    glStencilOpSeparate(GL_FRONT, (GLenum)0xDEAD, GL_KEEP, GL_KEEP);
  }
  while (glGetError() != GL_NO_ERROR)
    ;

  GLint after[8];
  for (int i = 0; i < pname_count; i++)
    glGetIntegerv(neighbour_pnames[i], &after[i]);

  int fail_count = 0;

  for (int i = 0; i < pname_count; i++) {
    if (before[i] != after[i]) {
      TEST_LOG_FAIL(test_case_11, test_procedure,
                    "pname=0x%X bozuldu: once=0x%X sonra=0x%X",
                    neighbour_pnames[i], before[i], after[i]);
      fail_count++;
    }
  }

  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  glStencilMask(0xFFFFFFFF);
  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_11, test_procedure);
  else
    TEST_LOG_FAIL(test_case_11, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 12 : glStencilOp ile Esdegerlik (Differential Test)
 * ============================================================
 *
 * glStencilOp(a, b, c) cagrisinin
 * glStencilOpSeparate(GL_FRONT_AND_BACK, a, b, c)
 * ile birebir ayni state'i uretmesi.
 *
 * Spec bu iki cagriyi esdeger tanimlar. Test, ayni ucluyu
 * once bir yolla sonra digeriyle uygular ve alti state
 * sorgusunun sonucunu KARSILASTIRIR. Beklenen degerler
 * sabit kodlanmaz; iki yolun ciktisi birbirine gore
 * dogrulanir. Bu sayede iki API yolunun ayni ic state'e
 * baglandigi kanitlanir.
 *
 * Her karsilastirmadan once state kasitli olarak bozulur
 * ki "hicbir sey yazmayan" bir implementasyon iki yolda da
 * ayni sonucu vererek testi gecemesin.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_012(void) {
  reset_stencil_ops();

  int fail_count = 0;

  for (int i = 0; i < valid_op_count; i++) {
    GLenum triple[3] = {valid_ops[i], valid_ops[(i + 1) % valid_op_count],
                        valid_ops[(i + 6) % valid_op_count]};

    /* Yol A: glStencilOp */
    GLenum dirty_front[3] = {GL_ZERO, GL_ZERO, GL_ZERO};
    GLenum dirty_back[3] = {GL_INVERT, GL_INVERT, GL_INVERT};
    set_faces(dirty_front, dirty_back);

    glStencilOp(triple[0], triple[1], triple[2]);
    GLenum err_a = glGetError();

    GLint a_front[3], a_back[3];
    get_front_ops(a_front);
    get_back_ops(a_back);

    /* Yol B: glStencilOpSeparate(GL_FRONT_AND_BACK, ...) */
    set_faces(dirty_front, dirty_back);

    glStencilOpSeparate(GL_FRONT_AND_BACK, triple[0], triple[1], triple[2]);
    GLenum err_b = glGetError();

    GLint b_front[3], b_back[3];
    get_front_ops(b_front);
    get_back_ops(b_back);

    if (err_a != GL_NO_ERROR || err_b != GL_NO_ERROR) {
      TEST_LOG_FAIL(test_case_12, test_procedure,
                    "(0x%X, 0x%X, 0x%X) hata: StencilOp=0x%X Separate=0x%X",
                    triple[0], triple[1], triple[2], err_a, err_b);
      fail_count++;
      continue;
    }

    if (!triple_eq(a_front, triple) || !triple_eq(a_back, triple)) {
      TEST_LOG_FAIL(test_case_12, test_procedure,
                    "glStencilOp yazamadi: on(0x%X, 0x%X, 0x%X) "
                    "arka(0x%X, 0x%X, 0x%X)",
                    a_front[0], a_front[1], a_front[2], a_back[0], a_back[1],
                    a_back[2]);
      fail_count++;
      continue;
    }

    for (int k = 0; k < 3; k++) {
      if (a_front[k] != b_front[k] || a_back[k] != b_back[k]) {
        TEST_LOG_FAIL(test_case_12, test_procedure,
                      "esdegerlik bozuk (idx=%d): StencilOp on=0x%X arka=0x%X, "
                      "Separate on=0x%X arka=0x%X",
                      k, a_front[k], a_back[k], b_front[k], b_back[k]);
        fail_count++;
        break;
      }
    }
  }

  reset_stencil_ops();

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_12, test_procedure);
  else
    TEST_LOG_FAIL(test_case_12, test_procedure, "Toplam basarisiz: %d",
                  fail_count);
}

/* ============================================================
 * TEST 13 : Golge Modelli Deterministik Fuzzing
 * ============================================================
 *
 * Rastgele parametre akisi altinda surucu state'inin
 * bagimsiz bir referans modelle ayni kalmasi.
 *
 * 30.000 iterasyon boyunca LCG ile face ve uc op degeri
 * uretilir. Test tarafinda tutulan golge model, cagrinin
 * gecerli olup olmadigini KENDISI hesaplar:
 *
 *   gecerli = is_valid_face(face) && uc op'un da gecerli olmasi
 *
 * Beklenen hata kodu bu hesaba gore tam olarak dogrulanir
 * (GL_NO_ERROR ya da GL_INVALID_ENUM). Gecerli cagrilarda
 * golge model guncellenir, gecersiz cagrilarda degismez;
 * her iterasyonda alti state sorgusu modelle karsilastirilir.
 *
 * Rastgele face secimi sayesinde yuzler arasi sizinti
 * (crosstalk) da bu akis icinde surekli kontrol edilmis olur.
 * ============================================================ */
void GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_013(void) {
  reset_stencil_ops();

  GLenum shadow_front[3] = {GL_KEEP, GL_KEEP, GL_KEEP};
  GLenum shadow_back[3] = {GL_KEEP, GL_KEEP, GL_KEEP};

  unsigned int seed = 13579;
  int err_mismatch = 0;
  int state_mismatch = 0;
  int valid_call_count = 0;
  int first_fail = -1;

  for (int i = 0; i < 30000; i++) {
    GLenum face;
    GLenum ops[3];

    seed = seed * 1103515245 + 12345;
    /* Iterasyonlarin yaklasik ucte biri gecerli face uretir */
    if (i % 3 == 0)
      face = valid_faces[(seed >> 16) % valid_face_count];
    else
      face = (GLenum)((seed >> 16) % 0x10000);

    for (int p = 0; p < 3; p++) {
      seed = seed * 1103515245 + 12345;
      /* Gecerli yolun yeterince taranmasi icin op'larin cogu gecerli */
      if ((seed >> 16) % 4 != 0)
        ops[p] = valid_ops[(seed >> 8) % valid_op_count];
      else
        ops[p] = (GLenum)((seed >> 16) % 0x10000);
    }

    int should_pass = is_valid_face(face) && is_valid_op(ops[0]) &&
                      is_valid_op(ops[1]) && is_valid_op(ops[2]);
    GLenum expected_err = should_pass ? GL_NO_ERROR : GL_INVALID_ENUM;

    glStencilOpSeparate(face, ops[0], ops[1], ops[2]);
    GLenum err = glGetError();

    if (err != expected_err) {
      if (first_fail < 0) {
        first_fail = i;
        TEST_LOG_FAIL(test_case_13, test_procedure,
                      "i=%d face=0x%X ops(0x%X, 0x%X, 0x%X) "
                      "beklenen=0x%X gelen=0x%X",
                      i, face, ops[0], ops[1], ops[2], expected_err, err);
      }
      err_mismatch++;
      /* Model ile surucu ayrismis olabilir; modeli yeniden hizala */
      reset_stencil_ops();
      shadow_front[0] = shadow_front[1] = shadow_front[2] = GL_KEEP;
      shadow_back[0] = shadow_back[1] = shadow_back[2] = GL_KEEP;
      continue;
    }

    if (should_pass) {
      valid_call_count++;

      if (face == GL_FRONT || face == GL_FRONT_AND_BACK) {
        shadow_front[0] = ops[0];
        shadow_front[1] = ops[1];
        shadow_front[2] = ops[2];
      }
      if (face == GL_BACK || face == GL_FRONT_AND_BACK) {
        shadow_back[0] = ops[0];
        shadow_back[1] = ops[1];
        shadow_back[2] = ops[2];
      }
    }

    GLint front[3], back[3];
    get_front_ops(front);
    get_back_ops(back);

    if (!triple_eq(front, shadow_front) || !triple_eq(back, shadow_back)) {
      if (first_fail < 0) {
        first_fail = i;
        TEST_LOG_FAIL(test_case_13, test_procedure,
                      "i=%d model uyusmazligi: face=0x%X | on beklenen(0x%X, "
                      "0x%X, 0x%X) gelen(0x%X, 0x%X, 0x%X) | arka "
                      "beklenen(0x%X, 0x%X, 0x%X) gelen(0x%X, 0x%X, 0x%X)",
                      i, face, shadow_front[0], shadow_front[1],
                      shadow_front[2], front[0], front[1], front[2],
                      shadow_back[0], shadow_back[1], shadow_back[2], back[0],
                      back[1], back[2]);
      }
      state_mismatch++;

      shadow_front[0] = (GLenum)front[0];
      shadow_front[1] = (GLenum)front[1];
      shadow_front[2] = (GLenum)front[2];
      shadow_back[0] = (GLenum)back[0];
      shadow_back[1] = (GLenum)back[1];
      shadow_back[2] = (GLenum)back[2];
    }
  }

  TEST_LOG_INFO("fuzzing - gecerli cagri sayisi: %d / 30000", valid_call_count);

  while (glGetError() != GL_NO_ERROR)
    ;
  reset_stencil_ops();

  GLenum final_err = glGetError();
  GLenum keep[3] = {GL_KEEP, GL_KEEP, GL_KEEP};
  GLint front[3], back[3];
  get_front_ops(front);
  get_back_ops(back);
  int recovered = triple_eq(front, keep) && triple_eq(back, keep);

  if (err_mismatch == 0 && state_mismatch == 0 && final_err == GL_NO_ERROR &&
      recovered)
    TEST_LOG_SUCCESS(test_case_13, test_procedure);
  else
    TEST_LOG_FAIL(test_case_13, test_procedure,
                  "hata_kodu_uyusmazligi=%d state_uyusmazligi=%d ilk=%d "
                  "final=0x%X toparlanma=%d",
                  err_mismatch, state_mismatch, first_fail, final_err,
                  recovered);
}

void GS_GL20SC_PFO_SOS_ROBUSTNESS_TP_001_init(void) {
  /* TC_001 varsayilan state'i dogruladigi icin ILK calismalidir. */
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_001();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_002();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_003();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_004();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_005();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_006();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_007();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_008();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_009();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_010();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_011();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_012();
  GS_GL20SC_PFO_SOS_ROBUSTNESS_TC_013();

  GS_GL20SC_PFO_SOS_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_SOS_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_PFO_SOS_ROBUSTNESS_TP_001_close(void) {
  glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  glStencilMask(0xFFFFFFFF);
  glDisable(GL_STENCIL_TEST);
  while (glGetError() != GL_NO_ERROR)
    ;
}
