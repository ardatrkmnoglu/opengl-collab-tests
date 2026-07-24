#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

static const char *test_case1 = "Texturing_TexSubImage2D_TC_001";
static const char *test_case2 = "Texturing_TexSubImage2D_TC_002";
static const char *test_case3 = "Texturing_TexSubImage2D_TC_003";
static const char *test_case4 = "Texturing_TexSubImage2D_TC_004";
static const char *test_case5 = "Texturing_TexSubImage2D_TC_005";

static const char *test_procedure = "Texturing_TexSubImage2D_TP_001";

static GLuint g_tex1 = 0;
static GLuint g_tex2 = 0;
static GLuint g_tex3 = 0;
static GLuint g_tex4 = 0;
static GLuint g_tex5 = 0;

// --- TEST 1: Negatif Boyut Testi (Invalid Value) ---
void Texturing_TexSubImage2D_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex1);
	glBindTexture(GL_TEXTURE_2D, g_tex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
	while (glGetError() != GL_NO_ERROR)
		;

	uint8_t dummy_data[4] = {0};
	// Genişlik ve yükseklik negatif olamaz (-10, -10)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, -10, -10, GL_RGBA,
			GL_UNSIGNED_BYTE, dummy_data);

	GLenum err = glGetError();
	if (err == GL_INVALID_VALUE) {
		TEST_LOG_SUCCESS(test_case1, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case1, test_procedure,
			      "Beklenen GL_INVALID_VALUE (0x501), alinan: 0x%X",
			      err);
	}
}

// --- TEST 2: Sınır Aşımı (Out of Bounds) Testi ---
void Texturing_TexSubImage2D_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex2);
	glBindTexture(GL_TEXTURE_2D, g_tex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
	while (glGetError() != GL_NO_ERROR)
		;

	uint8_t dummy_data[4] = {0};
	// xoffset (200) + width (100) = 300 > 256 (Out of Bounds)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 200, 0, 100, 10, GL_RGBA,
			GL_UNSIGNED_BYTE, dummy_data);

	GLenum err = glGetError();
	if (err == GL_INVALID_VALUE) {
		TEST_LOG_SUCCESS(test_case2, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case2, test_procedure,
			      "Beklenen GL_INVALID_VALUE (0x501), alinan: 0x%X",
			      err);
	}
}

// --- TEST 3: Format Uyuşmazlığı Testi ---
void Texturing_TexSubImage2D_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex3);
	glBindTexture(GL_TEXTURE_2D, g_tex3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
	while (glGetError() != GL_NO_ERROR)
		;

	uint8_t dummy_data[4] = {0};
	// Ana doku GL_RGBA iken GL_RGB formatı gönderiyoruz
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 10, 10, GL_RGB,
			GL_UNSIGNED_BYTE, dummy_data);

	GLenum err = glGetError();
	if (err == GL_INVALID_OPERATION) {
		TEST_LOG_SUCCESS(test_case3, test_procedure);
	} else {
		TEST_LOG_FAIL(
		    test_case3, test_procedure,
		    "Beklenen GL_INVALID_OPERATION (0x502), alinan: 0x%X", err);
	}
}

// --- TEST 4: Geçersiz Hedef (Enum) Testi ---
void Texturing_TexSubImage2D_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex4);
	glBindTexture(GL_TEXTURE_2D, g_tex4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
	while (glGetError() != GL_NO_ERROR)
		;

	uint8_t dummy_data[4] = {0};
	// GL_TEXTURE_2D yerine GL_POINTS hedefi veriyoruz
	glTexSubImage2D(GL_POINTS, 0, 0, 0, 10, 10, GL_RGBA, GL_UNSIGNED_BYTE,
			dummy_data);

	GLenum err = glGetError();
	if (err == GL_INVALID_ENUM) {
		TEST_LOG_SUCCESS(test_case4, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case4, test_procedure,
			      "Beklenen GL_INVALID_ENUM (0x500), alinan: 0x%X",
			      err);
	}
}

// --- TEST 5: Sürekli Güncelleme Stres Testi ---
void Texturing_TexSubImage2D_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int texSize = 512;
	glGenTextures(1, &g_tex5);
	glBindTexture(GL_TEXTURE_2D, g_tex5);

	GLubyte *initialData = (GLubyte *)malloc(texSize * texSize * 4);
	if (initialData) {
		for (int i = 0; i < texSize * texSize * 4; i += 4) {
			initialData[i] = 50;
			initialData[i + 1] = 50;
			initialData[i + 2] = 50;
			initialData[i + 3] = 255;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0,
			     GL_RGBA, GL_UNSIGNED_BYTE, initialData);
		free(initialData);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int subSize = 128;
	GLubyte *subData = (GLubyte *)malloc(subSize * subSize * 4);
	int error_occurred = 0;

	if (subData) {
		for (int step = 0; step < 100; step++) {
			for (int i = 0; i < subSize * subSize * 4; i += 4) {
				subData[i] = rand() % 256;
				subData[i + 1] = rand() % 256;
				subData[i + 2] = rand() % 256;
				subData[i + 3] = 255;
			}

			int offsetX = (texSize - subSize) / 2;
			int offsetY = (texSize - subSize) / 2;

			glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY,
					subSize, subSize, GL_RGBA,
					GL_UNSIGNED_BYTE, subData);

			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				error_occurred = 1;
				break;
			}
		}
		free(subData);
	}

	if (!error_occurred) {
		TEST_LOG_SUCCESS(test_case5, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case5, test_procedure,
			      "Surekli glTexSubImage2D guncellemesi sirasinda "
			      "hata olustu");
	}
}

/* Cleanup */
void Texturing_TexSubImage2D_close(void) {
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
}
