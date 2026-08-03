#include "../../../test_utility.h"

/*
GL20SC - Vertices - VertexAttrib1fv - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_003";

/* ============================================================
 * TEST GRUBU: glVertexAttrib1fv
 * ============================================================ */

/* ============================================================
 * glVertexAttrib1fv — İndeks Sınır İhlali
 * ============================================================ */
void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_001(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLfloat val[1] = {1.0f};
	glVertexAttrib1fv((GLuint)max_attribs, val);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "max_attribs indeksinde GL_INVALID_VALUE beklendi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glVertexAttrib1fv — Aşırı Büyük İndeks
 * ============================================================ */
void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLfloat val[1] = {1.0f};
	glVertexAttrib1fv(0xFFFFFFFF, val);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    test_case_2, test_procedure,
		    "Aşırı büyük indeks (0xFFFFFFFF) GL_INVALID_VALUE "
		    "üretmedi. Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	}
}

/* ============================================================
 * glVertexAttrib1fv — Özel Float Değerleri
 * ============================================================
 *
 * NaN değeri gönderilerek sürücünün çökmediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLfloat val[1] = {NAN};
	glVertexAttrib1fv(0, val);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "NaN değerinde tanımsız hata kodu."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_001();
	GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_002();
	GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TC_003();
}

/* Cleanup */
void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
