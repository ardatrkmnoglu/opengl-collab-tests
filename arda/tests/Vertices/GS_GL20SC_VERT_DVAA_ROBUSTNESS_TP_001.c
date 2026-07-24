#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "GS_GL20SC_VERT_DVAA_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_004";


/* ============================================================
 * TEST GRUBU: glDisableVertexAttribArray
 * ============================================================ */

/* ============================================================
 * glDisableVertexAttribArray — Sınır İhlali
 * ============================================================
 *
 * GL_MAX_VERTEX_ATTRIBS indeksini devre dışı bırakmaya
 * çalışarak sürücünün sınır kontrolü doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_001(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Üst sınır ihlali (GL_INVALID_VALUE bekliyoruz) */
	glDisableVertexAttribArray((GLuint)max_attribs);
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
 * glDisableVertexAttribArray — Geçerli İndeks
 * ============================================================
 *
 * Geçerli bir indeksin başarıyla disable edilebildiği kontrol
 * edilir. Pozitif test.
 * ============================================================ */
void GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glDisableVertexAttribArray(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
		    "Geçerli indeks devre dışı bırakılırken hata oluştu."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glDisableVertexAttribArray — Double Disable
 * ============================================================
 *
 * Zaten devre dışı olan bir indekse tekrar disable
 * yapıldığında hata üretmemesi beklenir.
 * ============================================================ */
void GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(1);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Çift devre dışı bırakma hata üretti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glDisableVertexAttribArray — Max Index Sınır İçi
 * ============================================================
 *
 * (max_attribs - 1) olan en üst sınır indeksinin hata üretmeden
 * devre dışı bırakılıp bırakılamadığı doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_DVAA_ROBUSTNESS_TC_004(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glDisableVertexAttribArray((GLuint)(max_attribs - 1));
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
		    "Son geçerli indeks devre dışı bırakılırken hata oluştu."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}
