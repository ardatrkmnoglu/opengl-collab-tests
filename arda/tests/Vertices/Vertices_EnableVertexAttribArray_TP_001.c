#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "Vertices_EnableVertexAttribArray_TP_001";
static const char* test_case_1 = "Vertices_EnableVertexAttribArray_TC_001";
static const char* test_case_2 = "Vertices_EnableVertexAttribArray_TC_002";
static const char* test_case_3 = "Vertices_EnableVertexAttribArray_TC_003";
static const char* test_case_4 = "Vertices_EnableVertexAttribArray_TC_004";


/* ============================================================
 * TEST GRUBU: glEnableVertexAttribArray
 * ============================================================ */

/* ============================================================
 * glEnableVertexAttribArray — Sınır İhlali
 * ============================================================
 *
 * GL_MAX_VERTEX_ATTRIBS indeksine veri bağlanmaya
 * çalışılarak sürücünün sınır kontrolü doğrulanır.
 * ============================================================ */
void Vertices_EnableVertexAttribArray_TC_001(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Üst sınır ihlali (GL_INVALID_VALUE bekliyoruz) */
	glEnableVertexAttribArray((GLuint)max_attribs);
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
 * glEnableVertexAttribArray — Geçerli İndeks
 * ============================================================
 *
 * Geçerli bir indeksin başarıyla enable edilebildiği kontrol
 * edilir. Pozitif test.
 * ============================================================ */
void Vertices_EnableVertexAttribArray_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glEnableVertexAttribArray(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Geçerli indeks etkinleştirilirken hata oluştu."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	/* Temizle */
	glDisableVertexAttribArray(0);
}

/* ============================================================
 * glEnableVertexAttribArray — Double Enable
 * ============================================================
 *
 * Zaten etkin olan bir indekse tekrar enable yapıldığında
 * hata üretmemesi ve sessizce başarması beklenir.
 * ============================================================ */
void Vertices_EnableVertexAttribArray_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(1);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Çift etkinleştirme hata üretti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	/* Temizle */
	glDisableVertexAttribArray(1);
}

/* ============================================================
 * glEnableVertexAttribArray — Stres Toggle
 * ============================================================
 *
 * Aynı indeks çok defa enable/disable yapılarak
 * durum makinesinin doğruluğu sınanır.
 * ============================================================ */
void Vertices_EnableVertexAttribArray_TC_004(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	for (int i = 0; i < 1000; i++) {
		glEnableVertexAttribArray(0);
		glDisableVertexAttribArray(0);
	}

	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Art arda aç/kapat işlemleri hata üretti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}
