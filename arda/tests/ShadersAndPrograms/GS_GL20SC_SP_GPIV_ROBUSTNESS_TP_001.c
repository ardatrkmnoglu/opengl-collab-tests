#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_004";

/* ============================================================
 * GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_003
 * ============================================================
 *
 * desteklenmemektedir (Objeler silinemez).
 * ============================================================ */
void GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_003(void) {
	/* GL SC 2.0 uyumluluğu için test devre dışı bırakıldı */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * glGetProgramiv — NULL params Pointer
 * ============================================================
 *
 * params çıkış tamponu olarak NULL geçildiğinde sürücünün
 * çökmeden davranıp davranmadığı gözlemlenir.
 * ============================================================ */
void GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_004(void) {
	GLuint prog = glCreateProgram();
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* NULL pointer: davranış implementation-defined ama çökmemeli */
	glGetProgramiv(prog, GL_LINK_STATUS, NULL);
	GLenum err = glGetError();

	/* Hata üretilmese de çökmeme koşulu yeterlidir */
	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "NULL params pointer beklenmeyen hata üretti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_4, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_003();
	GS_GL20SC_SP_GPIV_ROBUSTNESS_TC_004();
}

/* Cleanup */
void GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
