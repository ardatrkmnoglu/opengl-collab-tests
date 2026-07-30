#include "../../../test_utility.h"

/*
GL20SC - ShadersAndPrograms - Uniform3i - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_U3I_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_U3I_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_U3I_ROBUSTNESS_TC_003";

/* ============================================================
 * TEST GRUBU: glUniform3i
 * ============================================================ */

/* ============================================================
 * glUniform3i — Aktif Program Yok
 * ============================================================
 *
 * Hiçbir program aktif değilken Uniform ayarlanmaya
 * çalışılarak GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void GS_GL20SC_SP_U3I_ROBUSTNESS_TC_001(void) {
	glUseProgram(0);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniform3i(0, 1, 2, 3);
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
 * glUniform3i — Geçersiz Lokasyon
 * ============================================================
 *
 * Geçersiz bir lokasyon (0x7FFFFFFF) ile çağrı yapılarak
 * sürücünün bunu reddedip reddetmediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_U3I_ROBUSTNESS_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniform3i(0x7FFFFFFF, 1, 2, 3);
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
 * glUniform3i — Tip Karmaşası
 * ============================================================
 *
 * Shader'da float olarak tanımlı değişkene ivec3 boyutunda
 * veri basmaya çalışarak tip uyumsuzluğu test edilir.
 * ============================================================ */
void GS_GL20SC_SP_U3I_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locFloat = glGetUniformLocation(prog, "uFloat");
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUniform3i(locFloat, 1, 2, 3);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "float değişkene ivec3 veri yazıldı."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_U3I_ROBUSTNESS_TC_001();
	GS_GL20SC_SP_U3I_ROBUSTNESS_TC_002();
	GS_GL20SC_SP_U3I_ROBUSTNESS_TC_003();
}

/* Cleanup */
void GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
