#include "../../../test_utility.h"

/*
GL20SC - ShadersAndPrograms - GetVertexAttribiv - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_003";

/* ============================================================
 * TEST GRUBU: glGetVertexAttribiv
 * ============================================================ */

/* ============================================================
 * glGetVertexAttribiv — Geçersiz Enum
 * ============================================================
 *
 * 0xDEADBEEF geçersiz bir pname değeridir. Sürücünün
 * GL_INVALID_ENUM üretmesi beklenir.
 * ============================================================ */
void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint params[4] = {0};
	glGetVertexAttribiv(0, 0xDEADBEEF, params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Geçersiz pname (0xDEADBEEF) GL_INVALID_ENUM üretmedi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glGetVertexAttribiv — İndeks Sınır İhlali
 * ============================================================
 *
 * GL_MAX_VERTEX_ATTRIBS değerine eşit bir indeks ile
 * sorgu yapılarak sürücünün sınır kontrolü doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_002(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint params[4] = {0};
	glGetVertexAttribiv(max_attribs, GL_VERTEX_ATTRIB_ARRAY_ENABLED,
			    params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "max_attribs indeksi GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	}
}

/* ============================================================
 * glGetVertexAttribiv — Aşırı Büyük İndeks
 * ============================================================
 *
 * 0xFFFFFFFF gibi çok büyük bir indeks ile sorgu yapılarak
 * sürücünün GL_INVALID_VALUE ile reddettiği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint params[4] = {0};
	glGetVertexAttribiv(0xFFFFFFFF, GL_VERTEX_ATTRIB_ARRAY_ENABLED,
			    params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    test_case_3, test_procedure,
		    "Aşırı büyük indeks (0xFFFFFFFF) GL_INVALID_VALUE "
		    "üretmedi. Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_001();
	GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_002();
	GS_GL20SC_SP_GVAIV_ROBUSTNESS_TC_003();
}

/* Cleanup */
void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}