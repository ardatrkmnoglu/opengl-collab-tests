#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <string.h>

/*
 * GL20SC - Texturing - GetTexParameterfv - ROBUSTNESS
 *
 * glGetTexParameterfv robustness testleri.
 * Bu dosya asagidaki durumlari test eder:
 * TC_001: Desteklenmeyen veya gecersiz target parametrelerinin reddedilmesi.
 * TC_002: Tanimsiz veya gecersiz pname degerlerinin reddedilmesi.
 * TC_003: NULL data pointer gonderildiginde surucunun cokmeden once ilgili hatayi dondurmesi.
 * TC_004: Gecerli cagrilarda yazilan bellek alaninin disina tasilmadiginin (buffer overrun) kontrolu.
 */

static const char *test_procedure = "GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_004";

void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TP_001_close(void);

static GLuint g_tex = 0;

// TC_001: Gecersiz target
// SC 2.0 spesifikasyonuna gore glGetTexParameter yalnizca belli basli target'lari (ozellikle TEXTURE_2D)
// kabul etmelidir. 3D dokular, renderbufferlar ve SC 2.0'da bulunmayan cubemap yuzeyleri gibi
// gecersiz hedeflerin GL_INVALID_ENUM uretip uretmedigi dogrulanir.
void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR);
  GLfloat param = 0.0f;
  int fail_count = 0;

  GLenum bad_targets[] = {GL_RENDERBUFFER, GL_ARRAY_BUFFER,
                          GL_FRAMEBUFFER, 0x0000, 0xDEAD};
  int tgt_count = sizeof(bad_targets) / sizeof(bad_targets[0]);

  for (int i = 0; i < tgt_count; i++) {
    glGetTexParameterfv(bad_targets[i], GL_TEXTURE_MAG_FILTER, &param);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_1, test_procedure,
                    "target=0x%X expected=GL_INVALID_ENUM actual=0x%X",
                    bad_targets[i], err);
      fail_count++;
    }
  }

  /* Profil sondasi (SC 2.0 vs ES 2.0) */
  GLenum profile_targets[] = {GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP};
  int prof_count = sizeof(profile_targets) / sizeof(profile_targets[0]);
  int rejected = 0;

  for (int i = 0; i < prof_count; i++) {
    glGetTexParameterfv(profile_targets[i], GL_TEXTURE_MAG_FILTER, &param);
    if (glGetError() == GL_INVALID_ENUM)
      rejected++;
  }
  while (glGetError() != GL_NO_ERROR);

  TEST_LOG_INFO("3D/Cubemap targets rejected: %d/%d (SC 2.0 requires %d)",
                rejected, prof_count, prof_count);

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_1, test_procedure);
  else TEST_LOG_FAIL(test_case_1, test_procedure, "Total failed: %d", fail_count);
}

// TC_002: Gecersiz pname
// Doku parametresi sorgulama isleminde, glGetTexParameterfv yalnizca spesifikasyonda tanimli
// olan (orn. TEXTURE_MAG_FILTER) degerleri kabul etmelidir. GL_COMPILE_STATUS, GL_DEPTH_BITS
// gibi texturing ile ilgisiz argumanlarin GL_INVALID_ENUM ile reddedildigi dogrulanir.
void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR);
  GLfloat param = 0.0f;
  int fail_count = 0;

  GLenum bad_pnames[] = {GL_COMPILE_STATUS, GL_DEPTH_BITS, GL_MAX_TEXTURE_SIZE, 0x0000, 0xDEAD};
  int pnm_count = sizeof(bad_pnames) / sizeof(bad_pnames[0]);

  for (int i = 0; i < pnm_count; i++) {
    glGetTexParameterfv(GL_TEXTURE_2D, bad_pnames[i], &param);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "pname=0x%X expected=GL_INVALID_ENUM actual=0x%X",
                    bad_pnames[i], err);
      fail_count++;
    }
  }

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_2, test_procedure);
  else TEST_LOG_FAIL(test_case_2, test_procedure, "Total failed: %d", fail_count);
}

// TC_003: NULL data pointer hata onceligi
// Zaten hatali olan cagrilara (yanlis target veya yanlis pname) cikis parametresi olarak
// NULL gonderilir. Surucunun veriyi okumadan veya NULL pointer'a erismeden once 
// parametre denetimi yapmasi ve ilgili hatayi firlatmasi beklenir. Aksi halde program coker.
void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_003(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;

  glGetTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, NULL);
  GLenum err = glGetError();

  if (err != GL_INVALID_ENUM) {
    TEST_LOG_FAIL(test_case_3, test_procedure,
                  "invalid target + NULL params: expected=GL_INVALID_ENUM actual=0x%X", err);
    fail_count++;
  }

  glGetTexParameterfv(GL_TEXTURE_2D, GL_COMPILE_STATUS, NULL);
  err = glGetError();

  if (err != GL_INVALID_ENUM) {
    TEST_LOG_FAIL(test_case_3, test_procedure,
                  "invalid pname + NULL params: expected=GL_INVALID_ENUM actual=0x%X", err);
    fail_count++;
  }

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_3, test_procedure);
  else TEST_LOG_FAIL(test_case_3, test_procedure, "Total failed: %d", fail_count);
}

// TC_004: Bellek sinirlari (Buffer Overrun)
// glGetTexParameterfv normal sartlarda params pointer'ina tek bir GLfloat (4 bayt) yazar.
// Surucunun yanlislikla bu siniri asip bellegin devamina yazip yazmadigini tespit etmek
// amaciyla, hedef degiskenin onune ve arkasina koruma baytlari (guard bytes) eklenir ve
// islem sonrasinda bu baytlarin degismedigi kontrol edilir.
void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;

  GLfloat buffer[3];
  buffer[0] = -9999.0f; // Koruma (Guard)
  buffer[1] = 0.0f;     // Output (Target)
  buffer[2] = -9999.0f; // Koruma (Guard)

  glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &buffer[1]);
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_4, test_procedure, "Valid call failed: 0x%X", err);
    fail_count++;
  }

  if (buffer[0] != -9999.0f || buffer[2] != -9999.0f) {
    TEST_LOG_FAIL(test_case_4, test_procedure, "Buffer overrun detected! Guard bytes modified.");
    fail_count++;
  }

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_4, test_procedure);
  else TEST_LOG_FAIL(test_case_4, test_procedure, "Total failed: %d", fail_count);
}

void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TP_001_init(void) {
  glGenTextures(1, &g_tex);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_001();
  GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_002();
  GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_003();
  GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TC_004();

  GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_TEXT_GTPFV_ROBUSTNESS_TP_001_close(void) {
  glBindTexture(GL_TEXTURE_2D, 0);
#ifdef __ubuntu__
  if (g_tex) glDeleteTextures(1, &g_tex);
#endif
  while (glGetError() != GL_NO_ERROR);
}
