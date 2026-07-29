#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "GS_GL20SC_SaSR_GB_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_006";

/*
 * glGetBooleanv ile sorgulanabilen gecerli pname
 * degerlerinden bir alt kume. Hepsi hatasiz kabul edilmeli
 * ve GL_TRUE / GL_FALSE disinda bir deger dondurmemelidir.
 */
static const GLenum validPnames[] = {
	GL_CULL_FACE,
	GL_DEPTH_TEST,
	GL_STENCIL_TEST,
	GL_BLEND,
	GL_DITHER,
	GL_SCISSOR_TEST,
	GL_POLYGON_OFFSET_FILL,
	GL_SAMPLE_COVERAGE_INVERT,
	GL_DEPTH_WRITEMASK
};

/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 *
 * Amaç
 * ----
 * glGetBooleanv() gecerli pname degerleri ile cagrildiginda
 * hata uretmemeli ve GL_NO_ERROR dondurmelidir.
 *
 * Ayrica dondurulen degerin yalnizca GL_TRUE veya GL_FALSE
 * olabilecegi dogrulanir.
 * ============================================================ */

void GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_001(void)
{
	GLenum err;
	int count = sizeof(validPnames) / sizeof(validPnames[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		GLboolean value = GL_FALSE;

		glGetBooleanv(validPnames[i], &value);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "pname=0x%X Beklenmeyen hata : 0x%X",
				      validPnames[i], err);
			return;
		}

		if (value != GL_TRUE && value != GL_FALSE) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "pname=0x%X Gecersiz boolean deger : %d",
				      validPnames[i], (int)value);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : Invalid pname
 * ============================================================
 *
 * Amaç:
 * Gecersiz pname degerleri gonderildiginde glGetBooleanv()
 * GL_INVALID_ENUM uretmelidir.
 *
 * Ayrica hatali cagrilarin surucuyu cokertmemesi
 * dogrulanir.
 * ============================================================ */

void GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_002(void)
{
	GLenum err;
	GLenum invalidPnames[] = {
		0,
		1,
		2,
		1234,
		9999,
		0xFFFF,
		0xFFFFFFFF
	};

	int count = sizeof(invalidPnames) / sizeof(invalidPnames[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		GLboolean value = GL_FALSE;

		glGetBooleanv(invalidPnames[i], &value);

		err = glGetError();
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "pname=0x%X Beklenen=0x%X Gelen=0x%X",
				      invalidPnames[i], GL_INVALID_ENUM, err);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : pname Stress Sweep
 * ============================================================
 *
 * Amaç:
 * 16-bit deger uzayindaki tum pname degerleri sistematik
 * olarak denenir.
 *
 * Her cagri yalnizca GL_NO_ERROR veya GL_INVALID_ENUM
 * dondurmelidir. Bunlarin disindaki herhangi bir hata
 * kodu basarisizlik olarak degerlendirilir.
 * ============================================================ */

void GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_003(void)
{
	GLenum pname;
	int passCount = 0;
	int failCount = 0;

	clearGLErrors();

	for (pname = 0; pname < 65536; pname++) {
		GLboolean value = GL_FALSE;
		GLenum err;

		glGetBooleanv(pname, &value);

		err = glGetError();
		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "pname=0x%X Beklenmeyen hata : 0x%X",
				      pname, err);
			failCount++;
		} else {
			passCount++;
		}

		/* Gecerli sonuc donduyse deger yine boolean olmalidir. */
		if (err == GL_NO_ERROR &&
		    value != GL_TRUE && value != GL_FALSE) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "pname=0x%X Gecersiz boolean deger : %d",
				      pname, (int)value);
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
 * TEST 4 : Error Queue Management
 * ============================================================
 *
 * Amaç:
 * Arka arkaya cok sayida gecersiz cagri gonderildiginde
 * hata kuyrugunun bozulmadigi dogrulanir.
 *
 * Daha sonra gecerli bir cagri yapilarak surucunun
 * normal calismaya dondugu kontrol edilir.
 * ============================================================ */

void GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_004(void)
{
	GLenum err;
	int i;
	int errorCount = 0;
	GLboolean value = GL_FALSE;

	clearGLErrors();

	for (i = 0; i < 100; i++) {
		glGetBooleanv((GLenum)(0x5000 + i), &value);
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Beklenen : 0x%X Gelen : 0x%X",
				      GL_INVALID_ENUM, err);
			return;
		}
		errorCount++;
	}

	if (errorCount <= 0) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Hata kuyrugunda hic hata bulunamadi.");
		return;
	}

	glGetBooleanv(GL_DEPTH_TEST, &value);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}

	if (value != GL_TRUE && value != GL_FALSE) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Gecersiz boolean deger : %d", (int)value);
		return;
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Consistency
 * ============================================================
 *
 * Amaç:
 * Ayni pname arka arkaya cok sayida sorgulandiginda
 * her cagri ayni degeri dondurmelidir. glGetBooleanv()
 * bir okuma islemi oldugu icin state'i degistirmemeli
 * ve sonuc tutarli kalmalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_005(void)
{
	const int repeat = 100000;
	int i;
	GLboolean first = GL_FALSE;

	clearGLErrors();

	glGetBooleanv(GL_DITHER, &first);
	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Ilk sorgu hata uretti.");
		return;
	}

	for (i = 0; i < repeat; i++) {
		GLboolean value = GL_FALSE;

		glGetBooleanv(GL_DITHER, &value);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %d hata uretti.", i);
			return;
		}

		if (value != first) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %d Tutarsiz deger. Ilk : %d Simdi : %d",
				      i, (int)first, (int)value);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Rapid Fire
 * ============================================================
 *
 * Amaç:
 * glGetBooleanv() cok sayida ard arda cagrilarak
 * surucunun yogun kullanim altinda kararliligi
 * dogrulanir.
 *
 * Test sonunda herhangi bir OpenGL hatasi olusmamalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_006(void)
{
	const unsigned int repeat = 1000000;
	unsigned int i;
	GLboolean value = GL_FALSE;

	clearGLErrors();

	for (i = 0; i < repeat; i++) {
		glGetBooleanv(GL_CULL_FACE, &value);
		glGetBooleanv(GL_BLEND, &value);
		glGetBooleanv(GL_DEPTH_TEST, &value);
	}

	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_6, test_procedure,
			      "Yogun kullanim sonrasi hata olustu.");
		return;
	}

	clearGLErrors();

	TEST_LOG_INFO("%u cagri tamamlandi.", repeat * 3);
	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* Initialization */
void GS_GL20SC_SaSR_GB_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GB_ROBUSTNESS_TC_007();
	CHECK_ERROR(test_procedure);


}

void GS_GL20SC_SaSR_GB_ROBUSTNESS_TP_001_draw(void) {
}
/* Cleanup */
void GS_GL20SC_SaSR_GB_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}