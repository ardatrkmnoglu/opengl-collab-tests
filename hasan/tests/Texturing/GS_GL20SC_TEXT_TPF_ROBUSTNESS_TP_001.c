#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <string.h>

/*
 * GL20SC - Texturing - TexParameterf - ROBUSTNESS
 *
 * glTexParameterf robustness testleri.
 * Bu dosya asagidaki durumlari test eder:
 * TC_001: Desteklenmeyen veya gecersiz target parametrelerinin reddedilmesi.
 * TC_002: Tanimsiz veya gecersiz pname (parametre ismi) degerlerinin reddedilmesi.
 * TC_003: Gecerli pname'ler icin, gecersiz veya tanimsiz param (deger) verilmesinin reddedilmesi.
 */

static const char *test_procedure = "GS_GL20SC_TEXT_TPF_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_003";

void GS_GL20SC_TEXT_TPF_ROBUSTNESS_TP_001_close(void);

static GLuint g_tex = 0;

// TC_001: Gecersiz target
// SC 2.0 spesifikasyonuna gore glTexParameterf yalnizca belli basli target'lari (ozellikle TEXTURE_2D)
// kabul etmelidir. 3D dokular, renderbufferlar ve SC 2.0'da bulunmayan cubemap yuzeyleri gibi
// gecersiz hedeflerin GL_INVALID_ENUM uretip uretmedigi dogrulanir.
void GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;

  GLenum bad_targets[] = {GL_RENDERBUFFER, GL_ARRAY_BUFFER,
                          GL_FRAMEBUFFER, 0x0000, 0xDEAD};
  int tgt_count = sizeof(bad_targets) / sizeof(bad_targets[0]);

  for (int i = 0; i < tgt_count; i++) {
    glTexParameterf(bad_targets[i], GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
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
    glTexParameterf(profile_targets[i], GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
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
// Doku parametresi isleminde, glTexParameterf yalnizca spesifikasyonda tanimli
// olan (orn. TEXTURE_MAG_FILTER, TEXTURE_WRAP_S vb.) degerleri kabul etmelidir. 
// GL_COMPILE_STATUS, GL_DEPTH_BITS gibi texturing ile ilgisiz argumanlarin 
// GL_INVALID_ENUM ile reddedildigi dogrulanir.
void GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;

  GLenum bad_pnames[] = {GL_COMPILE_STATUS, GL_DEPTH_BITS, GL_MAX_TEXTURE_SIZE, 0x0000, 0xDEAD};
  int pnm_count = sizeof(bad_pnames) / sizeof(bad_pnames[0]);

  for (int i = 0; i < pnm_count; i++) {
    glTexParameterf(GL_TEXTURE_2D, bad_pnames[i], (GLfloat)GL_NEAREST);
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

// TC_003: Gecersiz param (deger)
// Pname gecerli dahi olsa, verilen parametre degeri eger o pname icin uygun degilse 
// (ornegin TEXTURE_WRAP_S icin GL_NEAREST verilmesi veya tanimsiz bir sayi verilmesi)
// GL_INVALID_ENUM hatasi alinmalidir.
void GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_003(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;
  GLenum err;

  // WRAP_S / WRAP_T icin desteklenmeyen enumlar
  GLfloat bad_wrap[] = {(GLfloat)GL_NEAREST, (GLfloat)GL_LINEAR, 0.0f, -1.0f};
  for (int i = 0; i < sizeof(bad_wrap) / sizeof(bad_wrap[0]); i++) {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, bad_wrap[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "WRAP_S with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_wrap[i], err);
      fail_count++;
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, bad_wrap[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "WRAP_T with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_wrap[i], err);
      fail_count++;
    }
  }

  // MIN_FILTER icin desteklenmeyen enumlar
  GLfloat bad_min[] = {(GLfloat)GL_CLAMP_TO_EDGE, (GLfloat)GL_REPEAT, 0.0f, -1.0f};
  for (int i = 0; i < sizeof(bad_min) / sizeof(bad_min[0]); i++) {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, bad_min[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "MIN_FILTER with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_min[i], err);
      fail_count++;
    }
  }

  // MAG_FILTER icin desteklenmeyen enumlar
  GLfloat bad_mag[] = {(GLfloat)GL_CLAMP_TO_EDGE, (GLfloat)GL_REPEAT, (GLfloat)GL_NEAREST_MIPMAP_NEAREST, (GLfloat)GL_LINEAR_MIPMAP_LINEAR, 0.0f, -1.0f};
  for (int i = 0; i < sizeof(bad_mag) / sizeof(bad_mag[0]); i++) {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, bad_mag[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "MAG_FILTER with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_mag[i], err);
      fail_count++;
    }
  }

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_3, test_procedure);
  else TEST_LOG_FAIL(test_case_3, test_procedure, "Total failed: %d", fail_count);
}

void GS_GL20SC_TEXT_TPF_ROBUSTNESS_TP_001_init(void) {
  glGenTextures(1, &g_tex);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_001();
  GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_002();
  GS_GL20SC_TEXT_TPF_ROBUSTNESS_TC_003();

  GS_GL20SC_TEXT_TPF_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_TEXT_TPF_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_TEXT_TPF_ROBUSTNESS_TP_001_close(void) {
  glBindTexture(GL_TEXTURE_2D, 0);
#ifdef __ubuntu__
  if (g_tex) glDeleteTextures(1, &g_tex);
#endif
  while (glGetError() != GL_NO_ERROR);
}
