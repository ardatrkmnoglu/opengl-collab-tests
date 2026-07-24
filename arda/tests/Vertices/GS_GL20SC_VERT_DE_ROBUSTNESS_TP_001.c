#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "GS_GL20SC_VERT_DE_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_VERT_DE_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_VERT_DE_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_VERT_DE_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_VERT_DE_ROBUSTNESS_TC_004";


/* ============================================================
 * TEST GRUBU: glDrawElements
 * ============================================================ */

/* ============================================================
 * glDrawElements — Geçersiz Type
 * ============================================================
 *
 * type parametresi sadece GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT
 * veya (eğer destekleniyorsa) GL_UNSIGNED_INT olabilir.
 * GL_FLOAT verilerek sürücünün bunu reddedip reddetmediği
 * doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_DE_ROBUSTNESS_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Geçersiz type: GL_FLOAT */
	glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Geçersiz type (GL_FLOAT) GL_INVALID_ENUM üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

}

/* ============================================================
 * glDrawElements — Negatif Count
 * ============================================================
 *
 * count parametresi negatif olamaz. Negatif (veya aşırı büyük)
 * sayı ile GL_INVALID_VALUE üretilip üretilmediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_DE_ROBUSTNESS_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Count negatif olamaz */
	glDrawElements(GL_TRIANGLES, -1, GL_UNSIGNED_SHORT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "count=-1 GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

}

/* ============================================================
 * glDrawElements — Geçersiz Mode
 * ============================================================
 *
 * mode parametresi çizim primitifini (GL_TRIANGLES vb.)
 * belirtir. Alakasız bir değer (GL_TEXTURE_2D) verilerek
 * GL_INVALID_ENUM üretilmesi beklenir.
 * ============================================================ */
void GS_GL20SC_VERT_DE_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Geçersiz mode */
	glDrawElements(GL_TEXTURE_2D, 3, GL_UNSIGNED_SHORT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Geçersiz mode GL_INVALID_ENUM üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

}

/* ============================================================
 * glDrawElements — Buffer Bağlı Değilken Sıfır Olmayan Çizim
 * ============================================================
 *
 * Hiçbir GL_ELEMENT_ARRAY_BUFFER bağlı değilken indices
 * olarak NULL ve count > 0 verilerek sürücünün sıfır adrese
 * erişip çökmek yerine tanımlı bir davranış göstermesi beklenir.
 * ============================================================ */
void GS_GL20SC_VERT_DE_ROBUSTNESS_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Buffer bağlı değil, count > 0, ptr = NULL */
	glDrawElements(GL_POINTS, 3, GL_UNSIGNED_BYTE, NULL);
	GLenum err = glGetError();

	/* Çökmemesi yeterlidir */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);

}
