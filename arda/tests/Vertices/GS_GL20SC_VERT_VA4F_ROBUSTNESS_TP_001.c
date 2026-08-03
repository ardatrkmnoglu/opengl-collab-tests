#include "../../../test_utility.h"

/*
GL20SC - Vertices - VertexAttrib4f - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_003";

/* ============================================================
 * TEST GRUBU: glVertexAttrib4f
 * ============================================================ */

/* ============================================================
 * glVertexAttrib4f — İndeks Sınır İhlali
 * ============================================================
 *
 * Donanımın limitine (max_attribs) veri yazmaya çalışarak
 * sürücünün sınır dışındaki bir Attribute indeksini reddetmesi
 * beklenir.
 * ============================================================ */
void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_001(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glVertexAttrib4f((GLuint)max_attribs, 1.0f, 2.0f, 3.0f, 4.0f);
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
 * glVertexAttrib4f — Aşırı Büyük İndeks
 * ============================================================
 *
 * 0xFFFFFFFF gibi çok büyük bir indeks ile çağrı yapılarak
 * sürücünün bu sınır dışı indeksi GL_INVALID_VALUE ile
 * reddettiği doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glVertexAttrib4f(0xFFFFFFFF, 1.0f, 2.0f, 3.0f, 4.0f);
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
 * glVertexAttrib4f — Özel Float Değerleri
 * ============================================================
 *
 * Kasıtlı olarak zehirli kayan nokta (float) değerleri
 * (NaN, Infinity) gönderilerek sürücünün bu değerleri
 * yediğinde çökmek yerine güvenle kabul etmesi veya
 * tanımlı bir hata üretmesi beklenir.
 * ============================================================ */
void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glVertexAttrib4f(0, NAN, INFINITY, -INFINITY, 0.0f);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "NaN/Inf değerlerinde tanımsız hata kodu."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_001();
	GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_002();
	GS_GL20SC_VERT_VA4F_ROBUSTNESS_TC_003();
}

/* Cleanup */
void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
