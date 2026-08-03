#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_005";

/* ============================================================
 * TEST GRUBU: glVertexAttribPointer
 * ============================================================ */

/* ============================================================
 * glVertexAttribPointer — Geçersiz Type
 * ============================================================
 *
 * Type parametresi GL_FLOAT, GL_BYTE vb. olmalıdır.
 * Alakasız bir Enum (GL_TEXTURE_2D) verilerek sürücünün
 * bunu reddedip reddetmediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Geçersiz veri tipi (GL_TEXTURE_2D) */
	glVertexAttribPointer(0, 3, GL_TEXTURE_2D, GL_FALSE, 0, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Geçersiz veri tipi (GL_TEXTURE_2D) reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glVertexAttribPointer — Geçersiz Size
 * ============================================================
 *
 * Size parametresi yalnızca 1, 2, 3 veya 4 olabilir.
 * 5 elemanlı bir vektör geçersizdir.
 * ============================================================ */
void GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Size parametresi 5 olamaz */
	glVertexAttribPointer(0, 5, GL_FLOAT, GL_FALSE, 0, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "size=5 olan geçersiz boyut reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glVertexAttribPointer — Negatif Stride
 * ============================================================
 *
 * Stride değeri negatif olamaz. Sürücünün negatif
 * stride değerini GL_INVALID_VALUE ile reddettiği doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, -4, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Negatif stride (-4) reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glVertexAttribPointer — İndeks Sınır İhlali
 * ============================================================
 *
 * GL_MAX_VERTEX_ATTRIBS indeksine veri bağlanmaya
 * çalışılarak sürücünün sınır kontrolü doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_004(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glVertexAttribPointer((GLuint)max_attribs, 3, GL_FLOAT, GL_FALSE, 0,
			      NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Sınır dışı indeks reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glVertexAttribPointer — NULL Pointer (Taşındı)
 * ============================================================
 *
 * Buffer nesnesi bağlı değilken pointer olarak NULL
 * verilmesi genellikle geçerli veya tanımlı hatadır.
 * Sürücünün bu durumda doğru yanıt verdiği kontrol edilir.
 * ============================================================ */
void GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_005(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* VBO bağlı değilken NULL pointer vermek bazı SC sürümlerinde
	 * geçersizdir */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GLenum err = glGetError();

	/* rTest_nullPtr'deki orijinal test (err != GL_NO_ERROR) bekliyordu */
	if (!(err != GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "VBO bağlı değilken NULL pointer için hata bekleniyordu."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_001();
	GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_002();
	GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_003();
	GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_004();
	GS_GL20SC_VERT_VAP_ROBUSTNESS_TC_005();
}

/* Cleanup */
void GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
