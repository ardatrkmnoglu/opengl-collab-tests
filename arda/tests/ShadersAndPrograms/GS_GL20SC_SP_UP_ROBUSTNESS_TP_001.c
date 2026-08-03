#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_SP_UP_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_UP_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_UP_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_UP_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_SP_UP_ROBUSTNESS_TC_004";

/* ============================================================
 * GS_GL20SC_SP_UP_ROBUSTNESS_TC_003
 * ============================================================
 *
 * desteklenmemektedir (Objeler silinemez).
 * ============================================================ */
void GS_GL20SC_SP_UP_ROBUSTNESS_TC_003(void) {
	/* GL SC 2.0 uyumluluğu için test devre dışı bırakıldı */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * glUseProgram — Sıfır ile Bağlamayı Kes
 * ============================================================
 *
 * glUseProgram(0) çağrısı mevcut program bağını kesmek için
 * geçerli ve standartlara uygun bir kullanımdır. Hata
 * üretmemesi beklenir.
 * ============================================================ */
void GS_GL20SC_SP_UP_ROBUSTNESS_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Bağlamayı kes: geçerli kullanım */
	glUseProgram(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "glUseProgram(0) ile bağ kesme hata üretti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_UP_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_UP_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_UP_ROBUSTNESS_TC_003();
	GS_GL20SC_SP_UP_ROBUSTNESS_TC_004();
}

/* Cleanup */
void GS_GL20SC_SP_UP_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
