#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_007";

/*
 * ES 2.0'da glIsEnabled ile sorgulanabilen tum gecerli
 * capability degerleri. Hepsi hatasiz kabul edilmeli ve
 * GL_TRUE / GL_FALSE disinda bir deger dondurmemelidir.
 */
static const GLenum validCaps[] = {
	GL_BLEND,
	GL_CULL_FACE,
	GL_DEPTH_TEST,
	GL_DITHER,
	GL_POLYGON_OFFSET_FILL,
	GL_SAMPLE_ALPHA_TO_COVERAGE,
	GL_SAMPLE_COVERAGE,
	GL_SCISSOR_TEST,
	GL_STENCIL_TEST
};

/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 *
 * Amaç
 * ----
 * glIsEnabled() tum gecerli capability degerleri ile
 * cagrildiginda hata uretmemeli ve donen deger yalnizca
 * GL_TRUE veya GL_FALSE olmalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_001(void)
{
	GLenum err;
	int count = sizeof(validCaps) / sizeof(validCaps[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		GLboolean result = glIsEnabled(validCaps[i]);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "cap=0x%X Beklenmeyen hata : 0x%X",
				      validCaps[i], err);
			return;
		}

		if (result != GL_TRUE && result != GL_FALSE) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "cap=0x%X Gecersiz boolean deger : %d",
				      validCaps[i], (int)result);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : Invalid cap
 * ============================================================
 *
 * Amaç
 * ----
 * Gecersiz capability degerleri gonderildiginde
 * glIsEnabled() GL_INVALID_ENUM uretmeli ve GL_FALSE (0)
 * dondurmelidir.
 * ============================================================ */

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_002(void)
{
	GLenum err;
	GLenum invalidCaps[] = {
		0,
		1,
		2,
		1234,
		9999,
		0xFFFF,
		0xFFFFFFFF
	};

	int count = sizeof(invalidCaps) / sizeof(invalidCaps[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		GLboolean result = glIsEnabled(invalidCaps[i]);

		err = glGetError();
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "cap=0x%X Beklenen=0x%X Gelen=0x%X",
				      invalidCaps[i], GL_INVALID_ENUM, err);
			return;
		}

		/* Spec: hata durumunda donen deger 0 (GL_FALSE) olmalidir. */
		if (result != GL_FALSE) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "cap=0x%X Hata sonrasi deger GL_FALSE degil : %d",
				      invalidCaps[i], (int)result);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : cap Stress Sweep
 * ============================================================
 *
 * Amaç
 * ----
 * 16-bit deger uzayindaki tum cap degerleri sistematik
 * olarak denenir.
 *
 * Her cagri yalnizca GL_NO_ERROR veya GL_INVALID_ENUM
 * dondurmelidir. Bunlarin disindaki herhangi bir hata
 * kodu basarisizlik olarak degerlendirilir.
 * ============================================================ */

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_003(void)
{
	GLenum cap;
	int passCount = 0;
	int failCount = 0;

	clearGLErrors();

	for (cap = 0; cap < 65536; cap++) {
		GLboolean result = glIsEnabled(cap);
		GLenum err = glGetError();

		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "cap=0x%X Beklenmeyen hata : 0x%X",
				      cap, err);
			failCount++;
		} else {
			passCount++;
		}

		/* Gecerli sonuc donduyse deger yine boolean olmalidir. */
		if (err == GL_NO_ERROR &&
		    result != GL_TRUE && result != GL_FALSE) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "cap=0x%X Gecersiz boolean deger : %d",
				      cap, (int)result);
			return;
		}
	}

	TEST_LOG_INFO("PASS : %d", passCount);
	TEST_LOG_INFO("FAIL : %d", failCount);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Basarisiz cagri sayisi : %d", failCount);
		return;
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : Enable / Disable Consistency
 * ============================================================
 *
 * Amaç
 * ----
 * Bir capability glEnable ile acildiginda glIsEnabled
 * GL_TRUE, glDisable ile kapatildiginda GL_FALSE
 * dondurmelidir.
 *
 * Tum gecerli cap degerleri icin bu tutarlilik dogrulanir.
 * ============================================================ */

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_004(void)
{
	int count = sizeof(validCaps) / sizeof(validCaps[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		GLenum cap = validCaps[i];
		GLboolean result;

		glEnable(cap);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "glEnable hata uretti. cap=0x%X", cap);
			return;
		}

		result = glIsEnabled(cap);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "glIsEnabled hata uretti. cap=0x%X", cap);
			return;
		}
		if (result != GL_TRUE) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "cap=0x%X Enable sonrasi GL_TRUE beklenirdi. Gelen : %d",
				      cap, (int)result);
			return;
		}

		glDisable(cap);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "glDisable hata uretti. cap=0x%X", cap);
			return;
		}

		result = glIsEnabled(cap);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "glIsEnabled hata uretti. cap=0x%X", cap);
			return;
		}
		if (result != GL_FALSE) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "cap=0x%X Disable sonrasi GL_FALSE beklenirdi. Gelen : %d",
				      cap, (int)result);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Error Queue Management
 * ============================================================
 *
 * Amaç
 * ----
 * Arka arkaya cok sayida gecersiz cagri gonderildiginde
 * hata kuyrugunun bozulmadigi dogrulanir.
 *
 * Daha sonra gecerli bir cagri yapilarak surucunun
 * normal calismaya dondugu kontrol edilir.
 * ============================================================ */

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_005(void)
{
	GLenum err;
	int i;
	int errorCount = 0;
	GLboolean result;

	clearGLErrors();

	for (i = 0; i < 100; i++) {
		glIsEnabled((GLenum)(0x5000 + i));
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Beklenen : 0x%X Gelen : 0x%X",
				      GL_INVALID_ENUM, err);
			return;
		}
		errorCount++;
	}

	if (errorCount <= 0) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Hata kuyrugunda hic hata bulunamadi.");
		return;
	}

	result = glIsEnabled(GL_DITHER);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}

	if (result != GL_TRUE && result != GL_FALSE) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Gecersiz boolean deger : %d", (int)result);
		return;
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Consistency
 * ============================================================
 *
 * Amaç
 * ----
 * Ayni cap arka arkaya cok sayida sorgulandiginda
 * her cagri ayni degeri dondurmelidir. glIsEnabled()
 * bir okuma islemi oldugu icin state'i degistirmemeli
 * ve sonuc tutarli kalmalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_006(void)
{
	const int repeat = 100000;
	int i;
	GLboolean first;

	clearGLErrors();

	first = glIsEnabled(GL_DITHER);
	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_6, test_procedure,
			      "Ilk sorgu hata uretti.");
		return;
	}

	for (i = 0; i < repeat; i++) {
		GLboolean result = glIsEnabled(GL_DITHER);

		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Iteration : %d hata uretti.", i);
			return;
		}

		if (result != first) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Iteration : %d Tutarsiz deger. Ilk : %d Simdi : %d",
				      i, (int)first, (int)result);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7 : Rapid Fire
 * ============================================================
 *
 * Amaç
 * ----
 * glIsEnabled() cok sayida ard arda cagrilarak
 * surucunun yogun kullanim altinda kararliligi
 * dogrulanir.
 *
 * Test sonunda herhangi bir OpenGL hatasi olusmamalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_007(void)
{
	const unsigned int repeat = 1000000;
	unsigned int i;

	clearGLErrors();

	for (i = 0; i < repeat; i++) {
		glIsEnabled(GL_BLEND);
		glIsEnabled(GL_DEPTH_TEST);
		glIsEnabled(GL_DITHER);
	}

	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Yogun kullanim sonrasi hata olustu.");
		return;
	}

	clearGLErrors();

	TEST_LOG_INFO("%u cagri tamamlandi.", repeat * 3);
	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

/* Initialization */
void GS_GL20SC_SaSR_IE_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_IE_ROBUSTNESS_TC_007();
	CHECK_ERROR(test_procedure);

}

void GS_GL20SC_SaSR_IE_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_SaSR_IE_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}