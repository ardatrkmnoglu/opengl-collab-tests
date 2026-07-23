#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * TEST GRUBU: glGetVertexAttribPointerv
 * ============================================================ */

/* ============================================================
 * glGetVertexAttribPointerv — Geçersiz Enum
 * ============================================================
 *
 * Bu fonksiyon yalnızca GL_VERTEX_ATTRIB_ARRAY_POINTER
 * Enum'ini kabul eder. GL_FLOAT gibi geçersiz bir Enum
 * verilerek sürücünün bunu reddedip reddetmediği doğrulanır.
 * ============================================================ */
void ShaderQueries_GetVertexAttribPointer_TC_001(void) {
	void *ptr = NULL;

	/* Yalnızca GL_VERTEX_ATTRIB_ARRAY_POINTER geçerlidir. */
	glGetVertexAttribPointerv(0, GL_FLOAT, &ptr);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL("GetVertexAttribPointer",
			      "ShaderQueries_GetVertexAttribPointer_TC_001",
			      "GL_FLOAT pname GL_INVALID_ENUM üretmedi."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetVertexAttribPointer",
				 "ShaderQueries_GetVertexAttribPointer_TC_001");
	}
}

/* ============================================================
 * glGetVertexAttribPointerv — Geçerli Sorgu
 * ============================================================
 *
 * GL_VERTEX_ATTRIB_ARRAY_POINTER ile yapılan sorgunun
 * hatasız tamamlandığını doğrular (pozitif test).
 * ============================================================ */
void ShaderQueries_GetVertexAttribPointer_TC_002(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	void *ptr = (void *)0xDEAD; /* sentinel */
	glGetVertexAttribPointerv(0, GL_VERTEX_ATTRIB_ARRAY_POINTER, &ptr);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("GetVertexAttribPointer",
			      "ShaderQueries_GetVertexAttribPointer_TC_002",
			      "Geçerli sorgu hata üretmemeli iken üretdi."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetVertexAttribPointer",
				 "ShaderQueries_GetVertexAttribPointer_TC_002");
	}
}

/* ============================================================
 * glGetVertexAttribPointerv — İndeks Sınır Dışı
 * ============================================================
 *
 * GL_MAX_VERTEX_ATTRIBS değerine eşit indeks ile çağrı
 * yapılarak sınır kontrolünün çalıştığı doğrulanır.
 * GL_INVALID_VALUE beklenir.
 * ============================================================ */
void ShaderQueries_GetVertexAttribPointer_TC_003(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	void *ptr = NULL;
	glGetVertexAttribPointerv((GLuint)max_attribs,
				  GL_VERTEX_ATTRIB_ARRAY_POINTER, &ptr);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    "GetVertexAttribPointer",
		    "ShaderQueries_GetVertexAttribPointer_TC_003",
		    "max_attribs indeksinde GL_INVALID_VALUE beklendi."
		    " Actual: 0x%04X",
		    (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetVertexAttribPointer",
				 "ShaderQueries_GetVertexAttribPointer_TC_003");
	}
}

/* ============================================================
 * glGetVertexAttribPointerv — NULL Çıkış Tamponu
 * ============================================================
 *
 * pointer çıkış parametresi NULL olarak verildiğinde sürücünün
 * çökmeden davranıp davranmadığı gözlemlenir. Spec bu durumu
 * tam olarak tanımlamasa da sürücü çökmemelidir.
 * ============================================================ */
void ShaderQueries_GetVertexAttribPointer_TC_004(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* NULL pointer: davranış implementation-defined, ama çökmemeli */
	glGetVertexAttribPointerv(0, GL_VERTEX_ATTRIB_ARRAY_POINTER, NULL);
	GLenum err = glGetError();

	/* Hata üretse de üretmese de çökmeme koşulunu geçer */
	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("GetVertexAttribPointer",
			      "ShaderQueries_GetVertexAttribPointer_TC_004",
			      "NULL pointer beklenmeyen hata kodu üretdi."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetVertexAttribPointer",
				 "ShaderQueries_GetVertexAttribPointer_TC_004");
	}
}
