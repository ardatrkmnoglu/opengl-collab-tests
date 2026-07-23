#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * TEST GRUBU: glGetProgramiv
 * ============================================================ */

/* ============================================================
 * glGetProgramiv — Geçersiz Enum
 * ============================================================
 *
 * Geçersiz bir parametre (pname = 0xDEADBEEF) ile
 * glGetProgramiv çağrılarak sürücünün bunu GL_INVALID_ENUM
 * ile reddedip reddetmediği doğrulanır.
 * ============================================================ */
void ShaderQueries_GetProgram_TC_001(void) {
	GLuint prog = glCreateProgram();
	GLint params = 0;

	glGetProgramiv(prog, 0xDEADBEEF, &params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(
		    "GetProgramiv", "ShaderQueries_GetProgram_TC_001",
		    "Geçersiz pname (0xDEADBEEF) GL_INVALID_ENUM üretmedi."
		    " Actual: 0x%04X",
		    (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetProgramiv",
				 "ShaderQueries_GetProgram_TC_001");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glGetProgramiv — Tip Karmaşası (Type Confusion)
 * ============================================================
 *
 * Sürücünün Shader nesnesini Program gibi okumaya çalışıp
 * çalışmayacağını sınıyoruz. GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void ShaderQueries_GetProgram_TC_002(void) {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	GLint params = 0;

	/* Tip Karmaşası: Shader ID'sine Program sorgusu yapılıyor. */
	glGetProgramiv(shader, GL_LINK_STATUS, &params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("GetProgramiv",
			      "ShaderQueries_GetProgram_TC_002",
			      "Shader nesnesine Program muamelesi engellenmedi."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetProgramiv",
				 "ShaderQueries_GetProgram_TC_002");
	}

	glDeleteShader(shader);
}

/* ============================================================
 * glGetProgramiv — Silinmiş Program
 * ============================================================
 *
 * glDeleteProgram() ile silinmiş bir program ID'si üzerinden
 * sorgu yapılarak sürücünün bunu doğru şekilde reddettiği
 * doğrulanır. GL_INVALID_VALUE veya GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void ShaderQueries_GetProgram_TC_003(void) {
	GLuint prog = glCreateProgram();
	glDeleteProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* hata kuyruğunu boşalt */
	}

	GLint params = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &params);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("GetProgramiv",
			      "ShaderQueries_GetProgram_TC_003",
			      "Silinmiş program ID'si ile sorgu reddedilmedi."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetProgramiv",
				 "ShaderQueries_GetProgram_TC_003");
	}
}

/* ============================================================
 * glGetProgramiv — NULL params Pointer
 * ============================================================
 *
 * params çıkış tamponu olarak NULL geçildiğinde sürücünün
 * çökmeden davranıp davranmadığı gözlemlenir.
 * ============================================================ */
void ShaderQueries_GetProgram_TC_004(void) {
	GLuint prog = glCreateProgram();
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* NULL pointer: davranış implementation-defined ama çökmemeli */
	glGetProgramiv(prog, GL_LINK_STATUS, NULL);
	GLenum err = glGetError();

	/* Hata üretilmese de çökmeme koşulu yeterlidir */
	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("GetProgramiv",
			      "ShaderQueries_GetProgram_TC_004",
			      "NULL params pointer beklenmeyen hata üretti."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("GetProgramiv",
				 "ShaderQueries_GetProgram_TC_004");
	}

	glDeleteProgram(prog);
}
