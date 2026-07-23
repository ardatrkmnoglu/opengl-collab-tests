#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * TEST GRUBU: glGetVertexAttribfv / glGetVertexAttribiv
 * ============================================================ */

/* ============================================================
 * glGetVertexAttribfv — Geçersiz Enum
 * ============================================================
 *
 * 0xDEADBEEF adında bir parametre (pname) yoktur. Sürücünün
 * geçersiz bir parametre sorgusuna GL_INVALID_ENUM ile
 * yanıt vermesi beklenir.
 * ============================================================ */
void ShaderQueries_GetVertexAttrib_TC_001(void) {
	GLfloat params[4] = {0};

	glGetVertexAttribfv(0, 0xDEADBEEF, params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(
		    "GetVertexAttrib", "ShaderQueries_GetVertexAttrib_TC_001",
		    "Geçersiz pname (0xDEADBEEF) GL_INVALID_ENUM üretmedi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("GetVertexAttrib",
				 "ShaderQueries_GetVertexAttrib_TC_001");
	}
}

/* ============================================================
 * glGetVertexAttribiv — İndeks Sınır İhlali
 * ============================================================
 *
 * Donanım sınırına (GL_MAX_VERTEX_ATTRIBS) eşit bir indeks
 * ile sorgu yapılarak sürücünün sınır kontrolü doğrulanır.
 * ============================================================ */
void ShaderQueries_GetVertexAttrib_TC_002(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

	GLint params[4] = {0};
	/* Sınır İhlali: İndeksler 0 ile (max_attribs - 1) arasında olmalıdır.
	 */
	glGetVertexAttribiv(max_attribs, GL_VERTEX_ATTRIB_ARRAY_ENABLED,
			    params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("GetVertexAttrib",
			      "ShaderQueries_GetVertexAttrib_TC_002",
			      "max_attribs indeksi GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("GetVertexAttrib",
				 "ShaderQueries_GetVertexAttrib_TC_002");
	}
}

/* ============================================================
 * glGetVertexAttribfv — Geçerli Sorgu
 * ============================================================
 *
 * Attribute 0 için GL_VERTEX_ATTRIB_ARRAY_ENABLED sorgusunun
 * hatasız tamamlandığını doğrular. Temel pozitif test.
 * ============================================================ */
void ShaderQueries_GetVertexAttrib_TC_003(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint enabled = -1;
	glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "GetVertexAttrib", "ShaderQueries_GetVertexAttrib_TC_003",
		    "Geçerli sorgu hata üretmemeli iken hata üretildi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("GetVertexAttrib",
				 "ShaderQueries_GetVertexAttrib_TC_003");
	}
}

/* ============================================================
 * glGetVertexAttribiv — Tüm Geçerli pname Değerleri
 * ============================================================
 *
 * Spesifikasyonun tanımladığı tüm geçerli pname enum'ları
 * sırayla sorgulanarak hata üretilmediği doğrulanır.
 * ============================================================ */
void ShaderQueries_GetVertexAttrib_TC_004(void) {
	GLenum valid_pnames[] = {GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
				 GL_VERTEX_ATTRIB_ARRAY_ENABLED,
				 GL_VERTEX_ATTRIB_ARRAY_SIZE,
				 GL_VERTEX_ATTRIB_ARRAY_STRIDE,
				 GL_VERTEX_ATTRIB_ARRAY_TYPE,
				 GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
				 GL_CURRENT_VERTEX_ATTRIB};
	int n = (int)(sizeof(valid_pnames) / sizeof(valid_pnames[0]));
	int fail_count = 0;

	for (int i = 0; i < n; i++) {
		while (glGetError() != GL_NO_ERROR) { /* temizle */
		}
		GLint val = 0;
		glGetVertexAttribiv(0, valid_pnames[i], &val);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL("GetVertexAttrib",
				      "ShaderQueries_GetVertexAttrib_TC_004",
				      "pname=0x%04X hata üretmemeli iken "
				      "üretdi. Actual: 0x%04X",
				      (unsigned int)valid_pnames[i],
				      err);
			fail_count++;
		}
	}

	if (fail_count == 0) {
		TEST_LOG_SUCCESS("GetVertexAttrib",
				 "ShaderQueries_GetVertexAttrib_TC_004");
	}
}
