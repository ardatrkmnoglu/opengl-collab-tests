#include "../../../include/macro.h"
#include "../../../include/rtests.h"

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
void Vertices_DisableVertexAttribArray_TC_001(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Üst sınır ihlali (GL_INVALID_VALUE bekliyoruz) */
	glDisableVertexAttribArray((GLuint)max_attribs);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("DisableVertexAttrib",
			      "Vertices_DisableVertexAttribArray_TC_001",
			      "Sınır dışı indeks reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("DisableVertexAttrib",
				 "Vertices_DisableVertexAttribArray_TC_001");
	}
}

/* ============================================================
 * glDisableVertexAttribArray — Geçerli İndeks
 * ============================================================
 *
 * Geçerli bir indeksin başarıyla disable edilebildiği kontrol
 * edilir. Pozitif test.
 * ============================================================ */
void Vertices_DisableVertexAttribArray_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glDisableVertexAttribArray(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "DisableVertexAttrib",
		    "Vertices_DisableVertexAttribArray_TC_002",
		    "Geçerli indeks devre dışı bırakılırken hata oluştu."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("DisableVertexAttrib",
				 "Vertices_DisableVertexAttribArray_TC_002");
	}
}

/* ============================================================
 * glDisableVertexAttribArray — Double Disable
 * ============================================================
 *
 * Zaten devre dışı olan bir indekse tekrar disable
 * yapıldığında hata üretmemesi beklenir.
 * ============================================================ */
void Vertices_DisableVertexAttribArray_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(1);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("DisableVertexAttrib",
			      "Vertices_DisableVertexAttribArray_TC_003",
			      "Çift devre dışı bırakma hata üretti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("DisableVertexAttrib",
				 "Vertices_DisableVertexAttribArray_TC_003");
	}
}

/* ============================================================
 * glDisableVertexAttribArray — Max Index Sınır İçi
 * ============================================================
 *
 * (max_attribs - 1) olan en üst sınır indeksinin hata üretmeden
 * devre dışı bırakılıp bırakılamadığı doğrulanır.
 * ============================================================ */
void Vertices_DisableVertexAttribArray_TC_004(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glDisableVertexAttribArray((GLuint)(max_attribs - 1));
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "DisableVertexAttrib", "Vertices_DisableVertexAttribArray_TC_004",
		    "Son geçerli indeks devre dışı bırakılırken hata oluştu."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("DisableVertexAttrib",
				 "Vertices_DisableVertexAttribArray_TC_004");
	}
}
