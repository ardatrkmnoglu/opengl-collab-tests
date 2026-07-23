#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

/* ============================================================
 * TEST GRUBU: glGetUniformLocation
 * ============================================================ */

/* ============================================================
 * glGetUniformLocation — NULL Pointer
 * ============================================================
 *
 * NULL pointer saldırısı: akıllı bir sürücü çökmeden
 * -1 dönmelidir.
 * ============================================================ */
void ShadersAndPrograms_GetUniformLocation_TC_001(void) {
	GLuint prog = createDummyProgram();

	/* NULL pointer saldırısı */
	GLint loc = glGetUniformLocation(prog, NULL);

	if (!(loc == -1)) {
		TEST_LOG_FAIL("GetUniformLocation",
			      "ShadersAndPrograms_GetUniformLocation_TC_001",
			      "NULL pointer için -1 dönmedi. Actual: %d", loc);
	} else {
		TEST_LOG_SUCCESS("GetUniformLocation",
				 "ShadersAndPrograms_GetUniformLocation_TC_001");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glGetUniformLocation — Yasaklı Ön Ek
 * ============================================================
 *
 * "gl_" ön eki spesifikasyon gereği OpenGL'e aittir.
 * Sürücünün yasaklı ön ekli uniform yerini ifşa edip
 * etmediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_GetUniformLocation_TC_002(void) {
	GLuint prog = createDummyProgram();

	/* "gl_" ön eki spesifikasyon gereği OpenGL'e aittir. */
	GLint loc = glGetUniformLocation(prog, "gl_DepthRange");

	if (!(loc == -1)) {
		TEST_LOG_FAIL("GetUniformLocation",
			      "ShadersAndPrograms_GetUniformLocation_TC_002",
			      "Yasaklı 'gl_' ön eki ifşa edildi. Actual: %d", loc);
	} else {
		TEST_LOG_SUCCESS("GetUniformLocation",
				 "ShadersAndPrograms_GetUniformLocation_TC_002");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glGetUniformLocation — Boş String
 * ============================================================
 *
 * Boş string ("") uniform ismi olarak geçersizdir ve
 * sürücünün -1 döndürmesi ya da GL_INVALID_VALUE üretmesi
 * beklenir.
 * ============================================================ */
void ShadersAndPrograms_GetUniformLocation_TC_003(void) {
	GLuint prog = createDummyProgram();
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	GLint loc = glGetUniformLocation(prog, "");
	GLenum err = glGetError();

	if (!(loc == -1 || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("GetUniformLocation",
			      "ShadersAndPrograms_GetUniformLocation_TC_003",
			      "Boş string için -1 ya da hata dönmedi."
			      " loc=%d, err=0x%04X", loc, (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetUniformLocation",
				 "ShadersAndPrograms_GetUniformLocation_TC_003");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glGetUniformLocation — Geçersiz Program
 * ============================================================
 *
 * 0 ID'sine sahip (hiç üretilmemiş) bir program üzerinde
 * sorgu yapılarak sürücünün bunu GL_INVALID_VALUE veya
 * GL_INVALID_OPERATION ile reddettiği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_GetUniformLocation_TC_004(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* 0 ID: hiçbir zaman geçerli program değildir */
	GLint loc = glGetUniformLocation(0, "uFloat");
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("GetUniformLocation",
			      "ShadersAndPrograms_GetUniformLocation_TC_004",
			      "Geçersiz (0) program ID'si reddedilmedi."
			      " loc=%d, err=0x%04X", loc, (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetUniformLocation",
				 "ShadersAndPrograms_GetUniformLocation_TC_004");
	}
}
