#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

static const char *test_case_1 = "GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_005";

static const char *test_procedure = "GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001";

static GLuint g_tex1 = 0;
static GLuint g_tex2 = 0;
static GLuint g_tex3 = 0;
static GLuint g_tex4 = 0;
static GLuint g_tex5 = 0;

#ifndef GL_RGBA8_OES
#define GL_RGBA8_OES 0x8058
#endif

static PFNGLTEXSTORAGE2DEXTPROC pglTexStorage2D = NULL;

/* Forward declaration for close */
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_close(void);

// Test 1: Negatif Boyut Testi (Invalid Value)
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex1);
	glBindTexture(GL_TEXTURE_2D, g_tex1);
	while (glGetError() != GL_NO_ERROR)
		;

	// Genişlik olarak kasten negatif bir değer (-256) veriyoruz
	if (pglTexStorage2D) {
		pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, -256, 256);
	}

	GLenum err = glGetError();
	if (err == GL_INVALID_VALUE) {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen GL_INVALID_VALUE (0x501), alinan: 0x%X",
			      err);
	}
}

// Test 2: Geçersiz İç Format Testi (Invalid Enum)
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex2);
	glBindTexture(GL_TEXTURE_2D, g_tex2);
	while (glGetError() != GL_NO_ERROR)
		;

	// internalformat parametresine geçersiz/uydurma bir enum (0x9999)
	// veriyoruz
	if (pglTexStorage2D) {
		pglTexStorage2D(GL_TEXTURE_2D, 1, 0x9999, 256, 256);
	}

	GLenum err = glGetError();
	if (err == GL_INVALID_ENUM) {
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Beklenen GL_INVALID_ENUM (0x500), alinan: 0x%X",
			      err);
	}
}

// Test 3: Değiştirilemezlik (Immutability) Testi (Invalid Operation)
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex3);
	glBindTexture(GL_TEXTURE_2D, g_tex3);
	while (glGetError() != GL_NO_ERROR)
		;

	if (pglTexStorage2D) {
		// İlk tahsis: Başarılı olması beklenir
		pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, 256, 256);
		while (glGetError() != GL_NO_ERROR)
			;

		// İkinci tahsis: Aynı doku üzerinde tekrar ayırma işlemi
		// deniyoruz
		pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, 512, 512);
	}

	GLenum err = glGetError();
	if (err == GL_INVALID_OPERATION) {
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	} else {
		TEST_LOG_FAIL(
		    test_case_3, test_procedure,
		    "Beklenen GL_INVALID_OPERATION (0x502), alinan: 0x%X", err);
	}
}

// Test 4 : level ile boyut ilişkisini test etme
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex4);
	glBindTexture(GL_TEXTURE_2D, g_tex4);
	while (glGetError() != GL_NO_ERROR)
		;

	if (pglTexStorage2D) {
		pglTexStorage2D(GL_TEXTURE_2D, 2, GL_RGBA8_OES, 500, 500);
	}

	GLenum err = glGetError();
	if (err == GL_INVALID_OPERATION) {
		TEST_LOG_SUCCESS(test_case_4, test_procedure);
	} else {
		TEST_LOG_FAIL(
		    test_case_4, test_procedure,
		    "Beklenen GL_INVALID_OPERATION (0x502), alinan: 0x%X", err);
	}
}

// Test 5 : max level'i aşmaya çalışmak
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex5);
	glBindTexture(GL_TEXTURE_2D, g_tex5);
	while (glGetError() != GL_NO_ERROR)
		;

	GLsizei w = 64;
	GLsizei h = 64;
	GLint levels = 8;

	if (pglTexStorage2D) {
		pglTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8_OES, w, h);
	}

	GLenum err = glGetError();
	if (err == GL_INVALID_OPERATION) {
		TEST_LOG_SUCCESS(test_case_5, test_procedure);
	} else {
		TEST_LOG_FAIL(
		    test_case_5, test_procedure,
		    "Beklenen GL_INVALID_OPERATION (0x502), alinan: 0x%X", err);
	}
}

/* Initialization */
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_init(void) {
	pglTexStorage2D =
	    (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress("glTexStorage2DEXT");
	if (!pglTexStorage2D)
		pglTexStorage2D = (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress(
		    "glTexStorage2D");

	if (!pglTexStorage2D) {
		printf("[HATA] glTexStorage2DEXT bulunamadi!\n");
	}

	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_001();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_002();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_003();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_004();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_TS_ROBUSTNESS_TC_005();
	//CHECK_ERROR(test_procedure);

	GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_close(void) {
#ifdef __ubuntu__
	if (g_tex1)
		glDeleteTextures(1, &g_tex1);
	if (g_tex2)
		glDeleteTextures(1, &g_tex2);
	if (g_tex3)
		glDeleteTextures(1, &g_tex3);
	if (g_tex4)
		glDeleteTextures(1, &g_tex4);
	if (g_tex5)
		glDeleteTextures(1, &g_tex5);
#endif
	//CHECK_ERROR(test_procedure);
}
