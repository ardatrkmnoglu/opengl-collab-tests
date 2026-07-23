#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

/* ============================================================
 * TEST GRUBU: glDrawElements
 * ============================================================ */

/* ============================================================
 * glDrawElements — Geçersiz Type
 * ============================================================
 *
 * type parametresi sadece GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT
 * veya (eğer destekleniyorsa) GL_UNSIGNED_INT olabilir.
 * GL_FLOAT verilerek sürücünün bunu reddedip reddetmediği
 * doğrulanır.
 * ============================================================ */
void Vertices_DrawElements_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Geçersiz type: GL_FLOAT */
	glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL("DrawElements", "Vertices_DrawElements_TC_001",
			      "Geçersiz type (GL_FLOAT) GL_INVALID_ENUM üretmedi."
			      " Actual: 0x%04X", (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("DrawElements", "Vertices_DrawElements_TC_001");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glDrawElements — Negatif Count
 * ============================================================
 *
 * count parametresi negatif olamaz. Negatif (veya aşırı büyük)
 * sayı ile GL_INVALID_VALUE üretilip üretilmediği doğrulanır.
 * ============================================================ */
void Vertices_DrawElements_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Count negatif olamaz */
	glDrawElements(GL_TRIANGLES, -1, GL_UNSIGNED_SHORT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("DrawElements", "Vertices_DrawElements_TC_002",
			      "count=-1 GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X", (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("DrawElements", "Vertices_DrawElements_TC_002");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glDrawElements — Geçersiz Mode
 * ============================================================
 *
 * mode parametresi çizim primitifini (GL_TRIANGLES vb.)
 * belirtir. Alakasız bir değer (GL_TEXTURE_2D) verilerek
 * GL_INVALID_ENUM üretilmesi beklenir.
 * ============================================================ */
void Vertices_DrawElements_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Geçersiz mode */
	glDrawElements(GL_TEXTURE_2D, 3, GL_UNSIGNED_SHORT, NULL);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL("DrawElements", "Vertices_DrawElements_TC_003",
			      "Geçersiz mode GL_INVALID_ENUM üretmedi."
			      " Actual: 0x%04X", (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("DrawElements", "Vertices_DrawElements_TC_003");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glDrawElements — Buffer Bağlı Değilken Sıfır Olmayan Çizim
 * ============================================================
 *
 * Hiçbir GL_ELEMENT_ARRAY_BUFFER bağlı değilken indices
 * olarak NULL ve count > 0 verilerek sürücünün sıfır adrese
 * erişip çökmek yerine tanımlı bir davranış göstermesi beklenir.
 * ============================================================ */
void Vertices_DrawElements_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Buffer bağlı değil, count > 0, ptr = NULL */
	glDrawElements(GL_POINTS, 3, GL_UNSIGNED_BYTE, NULL);
	GLenum err = glGetError();

	/* Çökmemesi yeterlidir */
	TEST_LOG_SUCCESS("DrawElements", "Vertices_DrawElements_TC_004");

	glDeleteProgram(prog);
}
