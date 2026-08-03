#include "../../../test_utility.h"

/*
GL20SC - ShadersAndPrograms - GetnUniformiv - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_004";

/* ============================================================
 * TEST GRUBU: glGetnUniformiv (KHR_robustness Extension)
 * ============================================================ */

/* ============================================================
 * glGetnUniformiv — Negatif Buffer Boyutu
 * ============================================================
 *
 * BufSize negatif olamaz. Sürücünün negatif buffer boyutunu
 * GL_INVALID_VALUE ile reddetmesi beklenir.
 * ============================================================ */
void GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	GLint locInt = glGetUniformLocation(prog, "uInt");
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint data[4];
	/* BufSize negatif olamaz */
	glGetnUniformiv(prog, locInt, -1, data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Negatif bufSize GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glGetnUniformiv — Geçersiz Program
 * ============================================================
 *
 * Program ID'si 0 hiçbir zaman geçerli bir program nesnesi
 * değildir. Sürücünün bunu reddetmesi beklenir.
 * ============================================================ */
void GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint data[4];
	glGetnUniformiv(0, 0, (GLsizei)sizeof(data), data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Geçersiz (0) program ID'si reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	}
}

/* ============================================================
 * glGetnUniformiv — Sıfır Buffer Boyutu
 * ============================================================
 *
 * BufSize = 0 durumunda sürücünün çökmeden tanımlı davranış
 * sergilediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	GLint locInt = glGetUniformLocation(prog, "uInt");
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint data[4];
	glGetnUniformiv(prog, locInt, 0, data);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE ||
	      err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "BufSize=0 için beklenmeyen hata kodu."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* ============================================================
 * glGetnUniformiv — Geçersiz Lokasyon
 * ============================================================
 *
 * 0x7FFFFFFF gibi tamamen geçersiz bir lokasyon ile çağrı
 * yapılarak sürücünün bunu reddettiği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint data[4];
	glGetnUniformiv(prog, 0x7FFFFFFF, (GLsizei)sizeof(data), data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Geçersiz lokasyon (0x7FFFFFFF) reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_4, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_001();
	GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_002();
	GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_003();
	GS_GL20SC_SP_GUIV_ROBUSTNESS_TC_004();
}

/* Cleanup */
void GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}