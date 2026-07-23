#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

static const char *test_case1 = "ReadingPixels_ReadnPixels_TC_001";
static const char *test_case2 = "ReadingPixels_ReadnPixels_TC_002";
static const char *test_case3 = "ReadingPixels_ReadnPixels_TC_003";
static const char *test_case4 = "ReadingPixels_ReadnPixels_TC_004";

static const char *test_procedure = "ReadingPixels_ReadnPixels_TP_001";

#ifndef APIENTRYP
#ifdef APIENTRY
#define APIENTRYP APIENTRY *
#else
#define APIENTRYP *
#endif
#endif

#ifndef PFNGLREADNPIXELSPROC
typedef void(APIENTRYP PFNGLREADNPIXELSPROC)(GLint x, GLint y, GLsizei width,
					     GLsizei height, GLenum format,
					     GLenum type, GLsizei bufSize,
					     void *data);
#endif
static PFNGLREADNPIXELSPROC pglReadnPixels = NULL;
#define glReadnPixels pglReadnPixels

// glReadnPixels fonksiyon isaretcisini yukler
void ReadingPixels_ReadnPixels_init(void) {
	pglReadnPixels =
	    (PFNGLREADNPIXELSPROC)glfwGetProcAddress("glReadnPixels");
	if (!pglReadnPixels)
		pglReadnPixels = (PFNGLREADNPIXELSPROC)glfwGetProcAddress(
		    "glReadnPixelsEXT");
	if (!pglReadnPixels)
		pglReadnPixels = (PFNGLREADNPIXELSPROC)glfwGetProcAddress(
		    "glReadnPixelsKHR");

	if (!pglReadnPixels) {
		printf(
		    "[HATA] glReadnPixels fonksiyon isaretcisi alinamadi!\n");
	}
}

// --- TEST 1: YETERSİZ KUTU BOYUTU (Buffer Overflow) ---
void ReadingPixels_ReadnPixels_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	unsigned char kucukKutu[3];
	int kutuBoyutu = sizeof(kucukKutu); // Sadece 3 byte

	// 10x10 (300 byte) alan istiyoruz
	glReadnPixels(0, 0, 10, 10, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu,
		      kucukKutu);

	GLenum err = glGetError();
	if (err == GL_INVALID_OPERATION) {
		TEST_LOG_SUCCESS(test_case1, test_procedure);
	} else {
		TEST_LOG_FAIL(
		    test_case1, test_procedure,
		    "Expected GL_INVALID_OPERATION (0x502), but got 0x%X", err);
	}
}

// --- TEST 2: NEGATİF BOYUT VERME (Boundary) ---
void ReadingPixels_ReadnPixels_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	unsigned char kutu[300];
	int kutuBoyutu = sizeof(kutu);

	// Genişlik ve yüksekliğe negatif değerler veriyoruz
	glReadnPixels(0, 0, -10, -10, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu,
		      kutu);

	GLenum err = glGetError();
	if (err == GL_INVALID_VALUE) {
		TEST_LOG_SUCCESS(test_case2, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case2, test_procedure,
			      "Expected GL_INVALID_VALUE (0x501), but got 0x%X",
			      err);
	}
}

// --- TEST 3: GEÇERSİZ PARAMETRE (Fuzzing) ---
void ReadingPixels_ReadnPixels_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	unsigned char kutu[3];
	int kutuBoyutu = sizeof(kutu);

	// GL_RGB yerine rastgele/tanımsız bir sayı (9999) gönderiyoruz
	glReadnPixels(0, 0, 1, 1, 9999, GL_UNSIGNED_BYTE, kutuBoyutu, kutu);

	GLenum err = glGetError();
	if (err == GL_INVALID_ENUM) {
		TEST_LOG_SUCCESS(test_case3, test_procedure);
	} else {
		TEST_LOG_FAIL(test_case3, test_procedure,
			      "Expected GL_INVALID_ENUM (0x500), but got 0x%X",
			      err);
	}
}

// --- TEST 4: EKRAN DIŞI KOORDİNAT (Out of Bounds) ---
void ReadingPixels_ReadnPixels_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	unsigned char kutu[3];
	int kutuBoyutu = sizeof(kutu);

	// x ve y koordinatlarını ekranın çok dışına taşıyoruz (-50, -50)
	glReadnPixels(-50, -50, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, kutuBoyutu,
		      kutu);

	GLenum err = glGetError();
	if (err == GL_NO_ERROR) {
		TEST_LOG_SUCCESS(test_case4, test_procedure);
	} else {
		TEST_LOG_FAIL(
		    test_case4, test_procedure,
		    "Unexpected error for out of bounds coordinates: 0x%X",
		    err);
	}
}
