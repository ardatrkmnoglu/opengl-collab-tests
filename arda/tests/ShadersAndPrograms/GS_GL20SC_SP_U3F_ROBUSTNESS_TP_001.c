#include "../../../test_utility.h"

/*
GL20SC - ShadersAndPrograms - Uniform3f - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_U3F_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_U3F_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_U3F_ROBUSTNESS_TC_003";

/* ============================================================
 * TEST GRUBU: glUniform3f
 * ============================================================ */

/* ============================================================
 * glUniform3f — Aktif Program Yok
 * ============================================================ */
void GS_GL20SC_SP_U3F_ROBUSTNESS_TC_001(void) {
	glUseProgram(0);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniform3f(0, 1.0f, 2.0f, 3.0f);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Aktif program olmadan Uniform ayarlandı."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glUniform3f — Geçersiz Lokasyon
 * ============================================================ */
void GS_GL20SC_SP_U3F_ROBUSTNESS_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniform3f(0x7FFFFFFF, 1.0f, 2.0f, 3.0f);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Geçersiz lokasyon (0x7FFFFFFF) reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	}
}

/* ============================================================
 * glUniform3f — Tip Karmaşası
 * ============================================================
 *
 * Shader'da vec4 olarak tanımlı değişkene vec3 boyutunda
 * veri basmaya çalışarak boyut uyumsuzluğu test edilir.
 * ============================================================ */
void GS_GL20SC_SP_U3F_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locVec4 = glGetUniformLocation(prog, "uVec4");
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniform3f(locVec4, 1.0f, 2.0f, 3.0f);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "vec4 değişkene vec3 veri yazıldı (boyut uyumsuz)."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_U3F_ROBUSTNESS_TC_001();
	GS_GL20SC_SP_U3F_ROBUSTNESS_TC_002();
	GS_GL20SC_SP_U3F_ROBUSTNESS_TC_003();
}

/* Cleanup */
void GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
