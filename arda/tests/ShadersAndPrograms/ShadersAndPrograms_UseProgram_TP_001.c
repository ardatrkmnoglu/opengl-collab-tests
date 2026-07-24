#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "ShadersAndPrograms_UseProgram_TP_001";
static const char* test_case_1 = "ShadersAndPrograms_UseProgram_TC_001";
static const char* test_case_2 = "ShadersAndPrograms_UseProgram_TC_002";
static const char* test_case_3 = "ShadersAndPrograms_UseProgram_TC_003";
static const char* test_case_4 = "ShadersAndPrograms_UseProgram_TC_004";


/* ============================================================
 * TEST GRUBU: glUseProgram
 * ============================================================ */

/* ============================================================
 * glUseProgram — Geçersiz ID
 * ============================================================
 *
 * Hiç var olmamış bir program ID'si (0xdeadbeef) ile
 * glUseProgram çağrılarak sürücünün hayalet ID'yi
 * reddedip reddetmediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_UseProgram_TC_001(void) {
	GLuint ghost_id = 0xdeadbeef;
	glUseProgram(ghost_id);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "CRITICAL: Sürücü hiç üretilmemiş bir program ID'sini"
		    " kabul etti. Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glUseProgram — Tip Karmaşası (Type Confusion)
 * ============================================================
 *
 * Bir Shader nesnesi ID'si ile glUseProgram çağrılarak
 * sürücünün Shader ve Program nesnelerini ayırt edip
 * edemediği doğrulanır. GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void ShadersAndPrograms_UseProgram_TC_002(void) {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glUseProgram(shader);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Shader ID'si Program olarak kabul edildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteShader(shader);
}

/* ============================================================
 * glUseProgram — Silinmiş Program
 * ============================================================
 *
 * Bağlantısız ve ardından silinen bir program ID'si ile
 * glUseProgram çağrılarak sürücünün bunu reddettiği doğrulanır.
 * GL_INVALID_VALUE veya GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void ShadersAndPrograms_UseProgram_TC_003(void) {
	GLuint prog = glCreateProgram();
	glDeleteProgram(prog); /* bağlamdan kop, silinmesini iste */
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	glUseProgram(prog);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "Silinmiş program ID'si ile glUseProgram reddedilmedi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glUseProgram — Sıfır ile Bağlamayı Kes
 * ============================================================
 *
 * glUseProgram(0) çağrısı mevcut program bağını kesmek için
 * geçerli ve standartlara uygun bir kullanımdır. Hata
 * üretmemesi beklenir.
 * ============================================================ */
void ShadersAndPrograms_UseProgram_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Bağlamayı kes: geçerli kullanım */
	glUseProgram(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "glUseProgram(0) ile bağ kesme hata üretti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDeleteProgram(prog);
}
