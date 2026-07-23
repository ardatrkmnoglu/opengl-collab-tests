#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * TEST GRUBU: glVertexAttrib{1234}{f} / glVertexAttrib{1234}{f}v
 * ============================================================ */

/* ============================================================
 * glVertexAttrib1f — İndeks Sınır İhlali
 * ============================================================
 *
 * Donanımın limitine (max_attribs) veri yazmaya çalışarak
 * sürücünün sınır dışındaki bir Attribute indeksini reddetmesi
 * beklenir.
 * ============================================================ */
void Vertices_VertexAttrib_TC_001(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Sınır İhlali: indeksler 0 ile (max_attribs-1) arasında olmalı */
	glVertexAttrib1f((GLuint)max_attribs, 1.0f);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    "VertexAttrib", "Vertices_VertexAttrib_TC_001",
		    "max_attribs indeksinde GL_INVALID_VALUE beklendi."
		    " Actual: 0x%04X",
		    (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("VertexAttrib",
				 "Vertices_VertexAttrib_TC_001");
	}
}

/* ============================================================
 * glVertexAttrib4fv — Özel Float Değerleri
 * ============================================================
 *
 * Kasıtlı olarak zehirli kayan nokta (float) değerleri
 * (NaN, Infinity) gönderilerek sürücünün bu değerleri
 * yediğinde çökmek yerine güvenle kabul etmesi
 * veya tanımlı bir hata üretmesi beklenir.
 * ============================================================ */
void Vertices_VertexAttrib_TC_002(void) {
	GLfloat data[4] = {NAN, INFINITY, -INFINITY, 0.0f};
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* NaN/Inf değerler: çökmemeli */
	glVertexAttrib4fv(0, data);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("VertexAttrib",
			      "Vertices_VertexAttrib_TC_002",
			      "NaN/Inf değerlerinde tanımsız hata kodu."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("VertexAttrib",
				 "Vertices_VertexAttrib_TC_002");
	}
}

/* ============================================================
 * glVertexAttrib4f — Geçerli Ayarlama
 * ============================================================
 *
 * Geçerli bir indeks ve değer ile glVertexAttrib4f çağrısının
 * hata üretmeden tamamlandığını doğrular (pozitif test).
 * ============================================================ */
void Vertices_VertexAttrib_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Geçerli attribute ayarla: indeks=0, normal float değerler */
	glVertexAttrib4f(0, 1.0f, 0.5f, 0.25f, 1.0f);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("VertexAttrib", "Vertices_VertexAttrib_TC_003",
			      "Geçerli ayarlama hata üretmemeli iken üretdi."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("VertexAttrib", "Vertices_VertexAttrib_TC_003");
	}
}

/* ============================================================
 * glVertexAttrib1f — Negatif / Büyük İndeks
 * ============================================================
 *
 * 0xFFFFFFFF gibi çok büyük (aşırı değer) indeks ile çağrı
 * yapılarak sürücünün bu sınır dışı indeksi GL_INVALID_VALUE
 * ile reddettiği doğrulanır.
 * ============================================================ */
void Vertices_VertexAttrib_TC_004(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* GLuint olduğu için "negatif" = çok büyük unsigned değer */
	glVertexAttrib1f(0xFFFFFFFF, 1.0f);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    "VertexAttrib", "Vertices_VertexAttrib_TC_004",
		    "Aşırı büyük indeks (0xFFFFFFFF) GL_INVALID_VALUE "
		    "üretmedi. Actual: 0x%04X",
		    (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("VertexAttrib",
				 "Vertices_VertexAttrib_TC_004");
	}
}
