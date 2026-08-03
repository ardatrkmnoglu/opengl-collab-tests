#include "../../../test_utility.h"

/*
GL20SC - ShadersAndPrograms - UniformMatrix2fv - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_003";

/* ============================================================
 * TEST GRUBU: glUniformMatrix2fv
 * ============================================================ */

/* ============================================================
 * glUniformMatrix2fv — Geçersiz Transpose
 * ============================================================
 *
 * ES 2.0 / SC 2.0: Transpose parametresi her zaman GL_FALSE
 * olmak ZORUNDADIR. GL_TRUE ile GL_INVALID_VALUE beklenir.
 * ============================================================ */
void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	GLint locMat2 = glGetUniformLocation(prog, "uMat2");
	GLfloat mat[4] = {0};
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniformMatrix2fv(locMat2, 1, GL_TRUE, mat);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "GL_TRUE transpose bayrağı kabul edildi (Spec ihlali)."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glUniformMatrix2fv — Negatif Count
 * ============================================================ */
void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	GLint locMat2 = glGetUniformLocation(prog, "uMat2");
	GLfloat mat[4] = {0};
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniformMatrix2fv(locMat2, -1, GL_FALSE, mat);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "count=-1 GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	}
}

/* ============================================================
 * glUniformMatrix2fv — Tip Uyumsuzluğu
 * ============================================================
 *
 * Mat4 lokasyonuna Mat2 verisi yazma girişimi ile sürücünün
 * tip kontrolü yapıp yapmadığı test edilir.
 * ============================================================ */
void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	GLint locMat4 = glGetUniformLocation(prog, "uMat4");
	GLfloat mat[4] = {0};
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniformMatrix2fv(locMat4, 1, GL_FALSE, mat);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(
		    test_case_3, test_procedure,
		    "Mat4 lokasyonuna Mat2 veri kopyalanamaz (Spec ihlali)."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_001();
	GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_002();
	GS_GL20SC_SP_UM2FV_ROBUSTNESS_TC_003();
}

/* Cleanup */
void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
