#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "Vertices_DrawRangeElements_TP_001";
static const char* test_case_1 = "Vertices_DrawRangeElements_TC_001";
static const char* test_case_2 = "Vertices_DrawRangeElements_TC_002";
static const char* test_case_3 = "Vertices_DrawRangeElements_TC_003";
static const char* test_case_4 = "Vertices_DrawRangeElements_TC_004";


/* ============================================================
 * TEST GRUBU: glDrawRangeElements
 * ============================================================ */

/* ============================================================
 * glDrawRangeElements — Geçersiz Aralık (end < start)
 * ============================================================
 *
 * end indeksinin start indeksinden küçük olması spesifikasyon
 * gereği hatadır. GL_INVALID_VALUE üretilmesi beklenir.
 * ============================================================ */
void Vertices_DrawRangeElements_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* end (5) < start (10) geçersizdir */
	glDrawRangeElements(GL_TRIANGLES, 10, 5, 3, GL_UNSIGNED_SHORT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "end < start durumu GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glDrawRangeElements — Geçersiz Type
 * ============================================================
 *
 * type parametresi sadece GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT
 * veya GL_UNSIGNED_INT olabilir. Geçersiz type (GL_FLOAT) ile
 * çağrı yapılarak GL_INVALID_ENUM üretilmesi beklenir.
 * ============================================================ */
void Vertices_DrawRangeElements_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Geçersiz type: GL_FLOAT */
	glDrawRangeElements(GL_TRIANGLES, 0, 10, 3, GL_FLOAT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Geçersiz type (GL_FLOAT) GL_INVALID_ENUM üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glDrawRangeElements — Negatif Count
 * ============================================================
 *
 * count parametresi negatif olamaz. GL_INVALID_VALUE üretip
 * üretmediği kontrol edilir.
 * ============================================================ */
void Vertices_DrawRangeElements_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* count negatif olamaz */
	glDrawRangeElements(GL_TRIANGLES, 0, 10, -1, GL_UNSIGNED_SHORT, NULL);
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
 * glDrawRangeElements — Maksimum Range
 * ============================================================
 *
 * end değeri 0xFFFFFFFF (aşırı büyük) verilerek sürücünün
 * içsel buffer aşımı yapıp yapmadığı, çökmeyip hayatta
 * kalabildiği doğrulanır.
 * ============================================================ */
void Vertices_DrawRangeElements_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Aşırı büyük end parametresi */
	glDrawRangeElements(GL_POINTS, 0, 0xFFFFFFFF, 1, GL_UNSIGNED_BYTE, NULL);
	GLenum err = glGetError();

	/* Hata üretebilir veya üretmeyebilir, ama çökmemesi başarısıdır. */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);

	glDeleteProgram(prog);
}
