#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <string.h>

/*
 * GL20SC - Texturing - TexParameterfv - ROBUSTNESS
 *
 * glTexParameterfv robustness testleri.
 * Bu dosya asagidaki durumlari test eder:
 * TC_001: Desteklenmeyen veya gecersiz target parametrelerinin reddedilmesi.
 * TC_002: Tanimsiz veya gecersiz pname (parametre ismi) degerlerinin reddedilmesi.
 * TC_003: Gecerli pname'ler icin, gecersiz veya tanimsiz param (deger) verilmesinin reddedilmesi.
 * TC_004: Hatali argumanlarla NULL pointer verildiginde, programin cokmemesi (Error priority).
 * TC_005: Parametre verisinin hizasiz (unaligned) bellekten okunmasinin cokmeye neden olmamasi.
 */

static const char *test_procedure = "GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_005";

void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TP_001_close(void);

static GLuint g_tex = 0;

// TC_001: Gecersiz target
void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;
  GLfloat param = (GLfloat)GL_NEAREST;

  GLenum bad_targets[] = {GL_RENDERBUFFER, GL_ARRAY_BUFFER,
                          GL_FRAMEBUFFER, 0x0000, 0xDEAD};
  int tgt_count = sizeof(bad_targets) / sizeof(bad_targets[0]);

  for (int i = 0; i < tgt_count; i++) {
    glTexParameterfv(bad_targets[i], GL_TEXTURE_MAG_FILTER, &param);
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
    glTexParameterfv(profile_targets[i], GL_TEXTURE_MAG_FILTER, &param);
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
void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;
  GLfloat param = (GLfloat)GL_NEAREST;

  GLenum bad_pnames[] = {GL_COMPILE_STATUS, GL_DEPTH_BITS, GL_MAX_TEXTURE_SIZE, 0x0000, 0xDEAD};
  int pnm_count = sizeof(bad_pnames) / sizeof(bad_pnames[0]);

  for (int i = 0; i < pnm_count; i++) {
    glTexParameterfv(GL_TEXTURE_2D, bad_pnames[i], &param);
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
void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_003(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;
  GLenum err;

  // WRAP_S / WRAP_T icin desteklenmeyen enumlar
  GLfloat bad_wrap[] = {(GLfloat)GL_NEAREST, (GLfloat)GL_LINEAR, 0.0f, -1.0f};
  for (int i = 0; i < sizeof(bad_wrap) / sizeof(bad_wrap[0]); i++) {
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &bad_wrap[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "WRAP_S with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_wrap[i], err);
      fail_count++;
    }

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &bad_wrap[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "WRAP_T with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_wrap[i], err);
      fail_count++;
    }
  }

  // MIN_FILTER icin desteklenmeyen enumlar
  GLfloat bad_min[] = {(GLfloat)GL_CLAMP_TO_EDGE, (GLfloat)GL_REPEAT, 0.0f, -1.0f};
  for (int i = 0; i < sizeof(bad_min) / sizeof(bad_min[0]); i++) {
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &bad_min[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "MIN_FILTER with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_min[i], err);
      fail_count++;
    }
  }

  // MAG_FILTER icin desteklenmeyen enumlar
  GLfloat bad_mag[] = {(GLfloat)GL_CLAMP_TO_EDGE, (GLfloat)GL_REPEAT, (GLfloat)GL_NEAREST_MIPMAP_NEAREST, (GLfloat)GL_LINEAR_MIPMAP_LINEAR, 0.0f, -1.0f};
  for (int i = 0; i < sizeof(bad_mag) / sizeof(bad_mag[0]); i++) {
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &bad_mag[i]);
    err = glGetError();
    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_3, test_procedure, "MAG_FILTER with param=%f expected=GL_INVALID_ENUM actual=0x%X", bad_mag[i], err);
      fail_count++;
    }
  }

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_3, test_procedure);
  else TEST_LOG_FAIL(test_case_3, test_procedure, "Total failed: %d", fail_count);
}

// TC_004: NULL data pointer hata onceligi
void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;
  GLenum err;

  // Gecersiz target + NULL
  glTexParameterfv(GL_RENDERBUFFER, GL_TEXTURE_MAG_FILTER, NULL);
  err = glGetError();
  if (err != GL_INVALID_ENUM) {
    TEST_LOG_FAIL(test_case_4, test_procedure, "Invalid target + NULL params expected=GL_INVALID_ENUM actual=0x%X", err);
    fail_count++;
  }

  // Gecersiz pname + NULL
  glTexParameterfv(GL_TEXTURE_2D, GL_COMPILE_STATUS, NULL);
  err = glGetError();
  if (err != GL_INVALID_ENUM) {
    TEST_LOG_FAIL(test_case_4, test_procedure, "Invalid pname + NULL params expected=GL_INVALID_ENUM actual=0x%X", err);
    fail_count++;
  }

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_4, test_procedure);
  else TEST_LOG_FAIL(test_case_4, test_procedure, "Total failed: %d", fail_count);
}

// TC_005: Unaligned pointer okuma
void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_005(void) {
  while (glGetError() != GL_NO_ERROR);
  int fail_count = 0;

  GLubyte buffer[sizeof(GLfloat) * 2];
  memset(buffer, 0, sizeof(buffer));

  // Gecerli bir param degeri unaligned (offset=1) adrese yerlestiriliyor.
  GLfloat param = (GLfloat)GL_NEAREST;
  memcpy(buffer + 1, &param, sizeof(GLfloat));

  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (const GLfloat*)(buffer + 1));
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_5, test_procedure, "Unaligned read failed. err=0x%X", err);
    fail_count++;
  }

  if (fail_count == 0) TEST_LOG_SUCCESS(test_case_5, test_procedure);
  else TEST_LOG_FAIL(test_case_5, test_procedure, "Total failed: %d", fail_count);
}

void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TP_001_init(void) {
  glGenTextures(1, &g_tex);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_001();
  GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_002();
  GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_003();
  GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_004();
  GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TC_005();

  GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_TEXT_TPFV_ROBUSTNESS_TP_001_close(void) {
  glBindTexture(GL_TEXTURE_2D, 0);
#ifdef __ubuntu__
  if (g_tex) glDeleteTextures(1, &g_tex);
#endif
  while (glGetError() != GL_NO_ERROR);
}
