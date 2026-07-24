#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char *test_procedure = "GS_GL20SC_R_PO_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_R_PO_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_R_PO_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_R_PO_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_R_PO_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_R_PO_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_R_PO_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_R_PO_ROBUSTNESS_TC_007";

/*
 * Ortak checkFloatState2 helper'i icin dosyaya ozel kisayol.
 * Factor ve units degerleri her cagrida birlikte dogrulanir.
 */

static int checkOffset(const char *test_case, GLfloat factor, GLfloat units) {
	return checkFloatState2(test_case, test_procedure,
				GL_POLYGON_OFFSET_FACTOR, factor,
				GL_POLYGON_OFFSET_UNITS, units, 1e-6f);
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama
 *
 * glPolygonOffset'in temel sozlesmesini dogrular.
 * Tum float degerler kabul edilmeli ve hicbir durumda
 * GL hata kodu uretilmemelidir. Ayrica son yazilan
 * factor ve units degerleri durum sorgusunda geri
 * alinabilmelidir.
 * ============================================================ */

void GS_GL20SC_R_PO_ROBUSTNESS_TC_001(void) {
	GLenum err;

	resetState_PolygonO();

	glPolygonOffset(0.0f, 0.0f);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata: 0x%X", err);
		return;
	}

	glPolygonOffset(2.0f, 3.0f);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata: 0x%X", err);
		return;
	}
	if (!checkOffset(test_case_1, 2.0f, 3.0f))
		return;

	glPolygonOffset(-1000.0f, -500.0f);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata: 0x%X", err);
		return;
	}
	if (!checkOffset(test_case_1, -1000.0f, -500.0f))
		return;

	glPolygonOffset(1000.0f, 500.0f);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata: 0x%X", err);
		return;
	}
	if (!checkOffset(test_case_1, 1000.0f, 500.0f))
		return;

	resetState_PolygonO();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2: Parametrik stres taramasi
 *
 * Genis bir float araliginda factor ve units
 * parametreleri sistematik olarak taranir.
 * Tum cagrilar GL_NO_ERROR donmelidir.
 * ============================================================ */

void GS_GL20SC_R_PO_ROBUSTNESS_TC_002(void) {
	int i;
	int passCount = 0;
	int failCount = 0;

	resetState_PolygonO();

	for (i = -10000; i <= 10000; i++) {

		GLfloat value = (GLfloat)i * 0.1f;
		GLenum err;

		glPolygonOffset(value, value);
		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "%.1f -> 0x%X", value, err);
			failCount++;
		} else {
			passCount++;
		}
	}

	glPolygonOffset(0.0f, 0.0f);
	if (!checkOffset(test_case_2, 0.0f, 0.0f))
		return;

	TEST_LOG_INFO("Sonuc: %d PASS, %d FAIL", passCount, failCount);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Basarisiz cagri sayisi: %d", failCount);
		return;
	}

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3: Hata kuyrugu butunlugu
 *
 * Ard arda cok sayida glPolygonOffset cagrisi sonrasi hata
 * kuyrugunun dogru calistigini dogrular. Spec'e gore bu
 * fonksiyon hata uretmez; kuyruk temiz kalmalidir.
 * ============================================================ */

void GS_GL20SC_R_PO_ROBUSTNESS_TC_003(void) {
	int i;
	GLenum err;

	resetState_PolygonO();

	for (i = 0; i < 1000; i++) {
		glPolygonOffset((GLfloat)i, (GLfloat)(-i));
	}

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Kuyrukta beklenmeyen hata: 0x%X", err);
		return;
	}
	if (!checkOffset(test_case_3, 999.0f, -999.0f))
		return;

	TEST_LOG_INFO("1000 cagri sonrasi kuyruk temiz");

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4: Durum korunumu
 *
 * Baska OpenGL fonksiyonlari hata uretse bile
 * glPolygonOffset durumunun degismedigini dogrular.
 * ============================================================ */

void GS_GL20SC_R_PO_ROBUSTNESS_TC_004(void) {
	GLenum err;

	resetState_PolygonO();

	glPolygonOffset(5.0f, 7.0f);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Beklenmeyen hata: 0x%X", err);
		return;
	}

	glFrontFace((GLenum)0x0BAD);
	err = glGetError();
	if (err != GL_INVALID_ENUM) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Beklenen: 0x%X Gelen: 0x%X", GL_INVALID_ENUM,
			      err);
		return;
	}

	if (!checkOffset(test_case_4, 5.0f, 7.0f))
		return;

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Hata kuyrugu temiz degil: 0x%X", err);
		return;
	}

	resetState_PolygonO();

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5: IEEE-754 ozel float degerleri
 *
 * NaN ve ±Infinity degerlerinin glPolygonOffset
 * tarafindan nasil ele alindigini gozlemler.
 * OpenGL spec bu degerler icin kesin davranis
 * tanimlamaz. Test bilgilendiricidir.
 * ============================================================ */

void GS_GL20SC_R_PO_ROBUSTNESS_TC_005(void) {
	GLenum err;
	GLfloat factor, units;

	resetState_PolygonO();

	glPolygonOffset(NAN, NAN);
	err = glGetError();
	TEST_LOG_INFO("NaN             -> 0x%X", err);

	glPolygonOffset(INFINITY, INFINITY);
	err = glGetError();
	TEST_LOG_INFO("+INFINITY       -> 0x%X", err);

	glPolygonOffset(-INFINITY, -INFINITY);
	err = glGetError();
	TEST_LOG_INFO("-INFINITY       -> 0x%X", err);

	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &factor);
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &units);

	TEST_LOG_INFO("factor=%.3f units=%.3f", factor, units);

	resetState_PolygonO();

	TEST_LOG_INFO("Manuel inceleme gerekir");
	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6: Hizli durum gecisleri
 *
 * Farkli factor ve units degerleri arasinda hizli gecisler
 * yaparak durum makinesinin tutarliligini dogrular.
 * Her gecisten sonra durum sorgulanir.
 * ============================================================ */

void GS_GL20SC_R_PO_ROBUSTNESS_TC_006(void) {
	int i;
	const int tekrar = 10000;

	resetState_PolygonO();

	for (i = 0; i < tekrar; i++) {

		GLfloat factor = (i % 2 == 0) ? 1.0f : -1.0f;
		GLfloat units = (i % 2 == 0) ? 2.0f : -2.0f;

		glPolygonOffset(factor, units);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Iteration: %d factor=%.3f units=%.3f", i,
				      factor, units);
			return;
		}

		if (!checkOffset(test_case_6, factor, units))
			return;
	}

	resetState_PolygonO();

	TEST_LOG_INFO("Sonuc: %d gecis tamamlandi", tekrar);

	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7: Enable/Disable etkilesimi
 *
 * glPolygonOffset durumunun
 * GL_POLYGON_OFFSET_FILL acik veya kapali olsa bile
 * korunup korunmadigini dogrular.
 * ============================================================ */

void GS_GL20SC_R_PO_ROBUSTNESS_TC_007(void) {
	GLenum err;

	resetState_PolygonO();

	glPolygonOffset(4.0f, 8.0f);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Beklenmeyen hata: 0x%X", err);
		return;
	}

	glEnable(GL_POLYGON_OFFSET_FILL);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "glEnable hata uretti: 0x%X", err);
		return;
	}
	if (!checkOffset(test_case_7, 4.0f, 8.0f))
		return;

	glDisable(GL_POLYGON_OFFSET_FILL);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "glDisable hata uretti: 0x%X", err);
		return;
	}
	if (!checkOffset(test_case_7, 4.0f, 8.0f))
		return;

	glEnable(GL_POLYGON_OFFSET_FILL);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "glEnable hata uretti: 0x%X", err);
		return;
	}
	if (!checkOffset(test_case_7, 4.0f, 8.0f))
		return;

	resetState_PolygonO();

	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}
