#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "GS_GL20SC_SP_GUFV_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_004";


/* ============================================================
 * TEST GRUBU: glGetnUniformfv / glGetnUniformiv
 * (KHR_robustness Extension)
 * ============================================================ */

/* ============================================================
 * glGetnUniformfv — Negatif Buffer Boyutu
 * ============================================================
 *
 * BufSize negatif olamaz. Bu, KHR_robustness eklentisinin
 * temel kuralıdır. Sürücünün negatif bir buffer boyutunu
 * reddedip reddetmediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_001(void) {
	GLuint prog = createDummyProgram();
	GLint locFloat = glGetUniformLocation(prog, "uFloat");
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	GLfloat data[4];
	/* BufSize negatif olamaz: KHR_robustness temel kuralı. */
	glGetnUniformfv(prog, locFloat, -1, data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Negatif bufSize GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

}

/* ============================================================
 * glGetnUniformfv — Geçersiz Program
 * ============================================================
 *
 * 0, hiçbir zaman geçerli bir program nesnesi değildir.
 * Sürücünün geçersiz bir Program ID'si üzerinden Uniform
 * sorgulayıp sorgulamadığı doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	GLfloat data[4];
	/* 0: hiçbir zaman geçerli bir program nesnesi değildir. */
	glGetnUniformfv(0, 0, (GLsizei)sizeof(data), data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Geçersiz (0) program ID'si reddedilmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glGetnUniformfv — Sıfır Buffer Boyutu
 * ============================================================
 *
 * BufSize = 0 durumunda sürücünün herhangi bir çöküm
 * yaşamadan tanımlı davranış sergilediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	GLint locFloat = glGetUniformLocation(prog, "uFloat");
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	GLfloat data[4];
	/* BufSize = 0: tanımlı edge case — hata üretmemeli ya da GL_INVALID_VALUE */
	glGetnUniformfv(prog, locFloat, 0, data);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE ||
	      err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "BufSize=0 için beklenmeyen hata kodu."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

}

/* ============================================================
 * glGetnUniformfv — Geçersiz Lokasyon
 * ============================================================
 *
 * -1 dışında tamamen geçersiz bir lokasyon değeri (0x7FFFFFFF)
 * ile çağrı yapılarak sürücünün bunu reddettiği doğrulanır.
 * GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void GS_GL20SC_SP_GUFV_ROBUSTNESS_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	GLfloat data[4];
	/* Tamamen uydurma lokasyon: sürücü reddetmeli */
	glGetnUniformfv(prog, 0x7FFFFFFF, (GLsizei)sizeof(data), data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Geçersiz lokasyon (0x7FFFFFFF) reddedilmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

}
