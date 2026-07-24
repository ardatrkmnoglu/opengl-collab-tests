#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "ShadersAndPrograms_UniformMatrix_TP_001";
static const char* test_case_1 = "ShadersAndPrograms_UniformMatrix_TC_001";
static const char* test_case_2 = "ShadersAndPrograms_UniformMatrix_TC_002";
static const char* test_case_3 = "ShadersAndPrograms_UniformMatrix_TC_003";
static const char* test_case_4 = "ShadersAndPrograms_UniformMatrix_TC_004";


/* ============================================================
 * TEST GRUBU: glUniformMatrix{234}fv
 * ============================================================ */

/* ============================================================
 * glUniformMatrix4fv — Geçersiz Transpose
 * ============================================================
 *
 * ES 2.0 ve SC 2.0 kuralı: Transpose parametresi her zaman
 * GL_FALSE olmak ZORUNDADIR. GL_TRUE verilerek sürücünün
 * bunu reddedip reddetmediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_UniformMatrix_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locMat4 = glGetUniformLocation(prog, "uMat4");
	GLfloat mat[16] = {0};
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* ES 2.0/SC 2.0 kuralı: transpose=GL_TRUE geçersizdir */
	glUniformMatrix4fv(locMat4, 1, GL_TRUE, mat);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "GL_TRUE transpose bayrağı kabul edildi (Spec ihlali)."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glUniformMatrix — Tip Uyumsuzluğu
 * ============================================================
 *
 * Lokasyonu Mat3 (3x3 Matris) olarak çekip, veriyi Mat4
 * fonksiyonuyla basmaya çalışarak sürücünün tip kontrolü
 * yapıp yapmadığını test eder.
 * ============================================================ */
void ShadersAndPrograms_UniformMatrix_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	/* Mat3 lokasyonuna Mat4 yazma girişimi */
	GLint locMat3 = glGetUniformLocation(prog, "uMat3");
	GLfloat mat[16] = {0};
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	glUniformMatrix4fv(locMat3, 1, GL_FALSE, mat);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Mat3 lokasyonuna Mat4 veri kopyalanamaz (Spec ihlali)."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glUniformMatrix4fv — Negatif Count
 * ============================================================
 *
 * count parametresi negatif olamaz. count = -1 ile çağrı
 * yapılarak GL_INVALID_VALUE üretilip üretilmediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_UniformMatrix_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locMat4 = glGetUniformLocation(prog, "uMat4");
	GLfloat mat[16] = {0};
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* count < 0: spesifikasyon gereği GL_INVALID_VALUE */
	glUniformMatrix4fv(locMat4, -1, GL_FALSE, mat);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "count=-1 GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glUniformMatrix4fv — NULL Data Pointer
 * ============================================================
 *
 * value parametresi olarak NULL pointer verildiğinde sürücünün
 * çökmeden tanımlı bir davranış sergilediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_UniformMatrix_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locMat4 = glGetUniformLocation(prog, "uMat4");
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* NULL value pointer: çökmemeli */
	glUniformMatrix4fv(locMat4, 1, GL_FALSE, NULL);
	GLenum err = glGetError();

	/* Hata üretse de üretmese de çökmeme koşulunu geçer */
	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE ||
	      err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "NULL value pointer beklenmeyen hata kodu."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteProgram(prog);
}
