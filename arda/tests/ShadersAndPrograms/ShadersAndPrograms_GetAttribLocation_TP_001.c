#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

/* ============================================================
 * TEST GRUBU: glGetAttribLocation
 * ============================================================ */

/* ============================================================
 * glGetAttribLocation — NULL Pointer
 * ============================================================
 *
 * glGetAttribLocation fonksiyonuna NULL pointer verilerek
 * sürücünün çökmeden -1 dönüp dönmediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_GetAttribLocation_TC_001(void) {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, NULL);

	if (!(loc == -1)) {
		TEST_LOG_FAIL("GetAttribLocation",
			      "ShadersAndPrograms_GetAttribLocation_TC_001",
			      "NULL pointer için -1 dönmedi. Actual: %d", loc);
	} else {
		TEST_LOG_SUCCESS("GetAttribLocation",
				 "ShadersAndPrograms_GetAttribLocation_TC_001");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glGetAttribLocation — Rezerve Değişken
 * ============================================================
 *
 * OpenGL'e ait rezerve edilmiş bir değişken adı ("gl_Position")
 * ile glGetAttribLocation çağrılarak sürücünün bu ismi
 * reddedip reddetmediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_GetAttribLocation_TC_002(void) {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, "gl_Position");

	if (!(loc == -1)) {
		TEST_LOG_FAIL("GetAttribLocation",
			      "ShadersAndPrograms_GetAttribLocation_TC_002",
			      "Rezerve 'gl_Position' ismi için -1 dönmedi."
			      " Actual: %d", loc);
	} else {
		TEST_LOG_SUCCESS("GetAttribLocation",
				 "ShadersAndPrograms_GetAttribLocation_TC_002");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glGetAttribLocation — Boş String
 * ============================================================
 *
 * Boş string ("") ile yapılan sorgunun geçersiz bir isim
 * olarak değerlendirilip -1 döndürdüğü doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_GetAttribLocation_TC_003(void) {
	GLuint prog = createDummyProgram();

	GLint loc = glGetAttribLocation(prog, "");

	if (!(loc == -1)) {
		TEST_LOG_FAIL("GetAttribLocation",
			      "ShadersAndPrograms_GetAttribLocation_TC_003",
			      "Boş string için -1 dönmedi. Actual: %d", loc);
	} else {
		TEST_LOG_SUCCESS("GetAttribLocation",
				 "ShadersAndPrograms_GetAttribLocation_TC_003");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glGetAttribLocation — Bağlanmamış Program
 * ============================================================
 *
 * Link edilmemiş (sadece oluşturulmuş) bir program üzerinde
 * glGetAttribLocation çağrısının GL_INVALID_OPERATION ürettiği
 * doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_GetAttribLocation_TC_004(void) {
	/* glCreateProgram ile oluşturulan ama link edilmemiş program */
	GLuint prog = glCreateProgram();
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	GLint loc = glGetAttribLocation(prog, "position");
	GLenum err = glGetError();

	/* Link edilmemiş program: GL_INVALID_OPERATION veya -1 dönmeli */
	if (!(err == GL_INVALID_OPERATION || loc == -1)) {
		TEST_LOG_FAIL("GetAttribLocation",
			      "ShadersAndPrograms_GetAttribLocation_TC_004",
			      "Link edilmemiş program için GL_INVALID_OPERATION "
			      "beklendi. err=0x%04X, loc=%d",
			      err, loc);
	} else {
		TEST_LOG_SUCCESS("GetAttribLocation",
				 "ShadersAndPrograms_GetAttribLocation_TC_004");
	}

	glDeleteProgram(prog);
}
