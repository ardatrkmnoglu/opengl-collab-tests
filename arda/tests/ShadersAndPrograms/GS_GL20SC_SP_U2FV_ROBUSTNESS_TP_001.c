#include "../../../test_utility.h"

/*
GL20SC - ShadersAndPrograms - Uniform2fv - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_003";

/* ============================================================
 * TEST GRUBU: glUniform2fv
 * ============================================================ */

/* ============================================================
 * glUniform2fv — Negatif Count
 * ============================================================ */
void GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	GLint loc = glGetUniformLocation(prog, "uVec2");
	GLfloat data[2] = {1.0f, 2.0f};
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniform2fv(loc, -1, data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "count=-1 GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glUniform2fv — Aktif Program Yok
 * ============================================================ */
void GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_002(void) {
	glUseProgram(0);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLfloat data[2] = {1.0f, 2.0f};
	glUniform2fv(0, 1, data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Aktif program olmadan Uniform ayarlandı."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	}
}

/* ============================================================
 * glUniform2fv — Geçersiz Lokasyon
 * ============================================================ */
void GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLfloat data[2] = {1.0f, 2.0f};
	glUniform2fv(0x7FFFFFFF, 1, data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Geçersiz lokasyon (0x7FFFFFFF) reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_001();
	GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_002();
	GS_GL20SC_SP_U2FV_ROBUSTNESS_TC_003();
}

/* Cleanup */
void GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}