#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "GS_GL20SC_SaSR_GI_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_006";

/*
 * ES 2.0'da glGetIntegerv ile sorgulanabilen gecerli pname
 * degerlerinden bir alt kume. Hepsi hatasiz kabul edilmeli
 * ve GL_NO_ERROR dondurmelidir.
 *
 * Not: Bazi pname'ler birden fazla deger dondurur; bu testte
 * tek elemanli olanlar kullanilmistir.
 */
static const GLenum validPnames[] = {
	GL_ACTIVE_TEXTURE,
	GL_CULL_FACE_MODE,
	GL_FRONT_FACE,
	GL_DEPTH_FUNC,
	GL_BLEND_SRC_RGB,
	GL_BLEND_DST_RGB,
	GL_STENCIL_FUNC,
	GL_STENCIL_VALUE_MASK,
	GL_UNPACK_ALIGNMENT,
	GL_PACK_ALIGNMENT,
	GL_MAX_TEXTURE_SIZE,
	GL_MAX_VERTEX_ATTRIBS,
	GL_MAX_TEXTURE_IMAGE_UNITS,
	GL_SUBPIXEL_BITS
};

/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 *
 * Amaç
 * ----
 * glGetIntegerv() gecerli pname degerleri ile cagrildiginda
 * hata uretmemeli ve GL_NO_ERROR dondurmelidir.
 *
 * Ayrica sorgu oncesi yazilan sentinel degerin uzerine
 * gercekten bir deger yazildigi dogrulanir.
 * ============================================================ */

void GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_001(void)
{
	GLenum err;
	int count = sizeof(validPnames) / sizeof(validPnames[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		GLint value = -12345;

		glGetIntegerv(validPnames[i], &value);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "pname=0x%X Beklenmeyen hata : 0x%X",
				      validPnames[i], err);
			return;
		}

		if (value == -12345) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "pname=0x%X Deger yazilmadi.",
				      validPnames[i]);
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
 * Amaç
 * ----
 * Gecersiz pname degerleri gonderildiginde glGetIntegerv()
 * GL_INVALID_ENUM uretmelidir.
 *
 * Ayrica hatali cagrilarin surucuyu cokertmemesi
 * dogrulanir.
 * ============================================================ */

void GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_002(void)
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
		GLint value = 0;

		glGetIntegerv(invalidPnames[i], &value);

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
 * Amaç
 * ----
 * 16-bit deger uzayindaki tum pname degerleri sistematik
 * olarak denenir.
 *
 * Her cagri yalnizca GL_NO_ERROR veya GL_INVALID_ENUM
 * dondurmelidir. Bunlarin disindaki herhangi bir hata
 * kodu basarisizlik olarak degerlendirilir.
 * ============================================================ */

void GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_003(void)
{
	GLenum pname;
	int passCount = 0;
	int failCount = 0;

	clearGLErrors();

	for (pname = 0; pname < 65536; pname++) {
		GLint value = 0;
		GLenum err;

		glGetIntegerv(pname, &value);

		err = glGetError();
		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "pname=0x%X Beklenmeyen hata : 0x%X",
				      pname, err);
			failCount++;
		} else {
			passCount++;
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
 * Amaç
 * ----
 * Arka arkaya cok sayida gecersiz cagri gonderildiginde
 * hata kuyrugunun bozulmadigi dogrulanir.
 *
 * Daha sonra gecerli bir cagri yapilarak surucunun
 * normal calismaya dondugu kontrol edilir.
 * ============================================================ */

void GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_004(void)
{
	GLenum err;
	int i;
	int errorCount = 0;
	GLint value = 0;

	clearGLErrors();

	for (i = 0; i < 100; i++) {
		glGetIntegerv((GLenum)(0x5000 + i), &value);
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

	value = -12345;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}

	if (value == -12345) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Gecerli sorgu deger yazmadi.");
		return;
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Consistency
 * ============================================================
 *
 * Amaç
 * ----
 * Ayni pname arka arkaya cok sayida sorgulandiginda
 * her cagri ayni degeri dondurmelidir. glGetIntegerv()
 * bir okuma islemi oldugu icin state'i degistirmemeli
 * ve sonuc tutarli kalmalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_005(void)
{
	const int repeat = 100000;
	int i;
	GLint first = 0;

	clearGLErrors();

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &first);
	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Ilk sorgu hata uretti.");
		return;
	}

	for (i = 0; i < repeat; i++) {
		GLint value = 0;

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %d hata uretti.", i);
			return;
		}

		if (value != first) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %d Tutarsiz deger. Ilk : %d Simdi : %d",
				      i, first, value);
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
 * Amaç
 * ----
 * glGetIntegerv() cok sayida ard arda cagrilarak
 * surucunun yogun kullanim altinda kararliligi
 * dogrulanir.
 *
 * Test sonunda herhangi bir OpenGL hatasi olusmamalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_006(void)
{
	const unsigned int repeat = 1000000;
	unsigned int i;
	GLint value = 0;

	clearGLErrors();

	for (i = 0; i < repeat; i++) {
		glGetIntegerv(GL_ACTIVE_TEXTURE, &value);
		glGetIntegerv(GL_DEPTH_FUNC, &value);
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
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
void GS_GL20SC_SaSR_GI_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GI_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);

}

void GS_GL20SC_SaSR_GI_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_SaSR_GI_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}