#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "GS_GL20SC_SaSR_GS_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_006";

/*
 * ES 2.0'da glGetString ile sorgulanabilen tum gecerli
 * name degerleri. Her biri NULL olmayan, null-terminated
 * bir string dondurmelidir.
 */

static const GLenum validNames[] = {
	GL_VENDOR,
	GL_RENDERER,
	GL_VERSION,
	GL_SHADING_LANGUAGE_VERSION,
	GL_EXTENSIONS
};

/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 *
 * Amaç
 * ----
 * glGetString() tum gecerli name degerleri ile
 * cagrildiginda hata uretmemeli ve NULL olmayan bir
 * isaretci dondurmelidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_001(void)
{
	GLenum err;
	int count = sizeof(validNames) / sizeof(validNames[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		const GLubyte* str = glGetString(validNames[i]);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "name=0x%X Beklenmeyen hata : 0x%X",
				      validNames[i], err);
			return;
		}

		if (str == NULL) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "name=0x%X NULL isaretci donduruldu.",
				      validNames[i]);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : Invalid name
 * ============================================================
 *
 * Amaç
 * ----
 * Gecersiz name degerleri gonderildiginde glGetString()
 * GL_INVALID_ENUM uretmeli ve NULL (0) dondurmelidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_002(void)
{
	GLenum err;
	GLenum invalidNames[] = {
		0,
		1,
		2,
		1234,
		9999,
		0xFFFF,
		0xFFFFFFFF
	};

	int count = sizeof(invalidNames) / sizeof(invalidNames[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		const GLubyte* str = glGetString(invalidNames[i]);

		err = glGetError();
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "name=0x%X Beklenen=0x%X Gelen=0x%X",
				      invalidNames[i], GL_INVALID_ENUM, err);
			return;
		}

		/* Spec: hata durumunda donen deger NULL olmalidir. */
		if (str != NULL) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "name=0x%X Hata sonrasi NULL beklenirdi.",
				      invalidNames[i]);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : name Stress Sweep
 * ============================================================
 *
 * Amaç
 * ----
 * 16-bit deger uzayindaki tum name degerleri sistematik
 * olarak denenir.
 *
 * Her cagri yalnizca GL_NO_ERROR veya GL_INVALID_ENUM
 * dondurmelidir. Gecerli sonuc donduginde isaretci NULL
 * olmamali, hata durumunda NULL olmalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_003(void)
{
	GLenum name;
	int passCount = 0;
	int failCount = 0;

	clearGLErrors();

	for (name = 0; name < 65536; name++) {
		const GLubyte* str = glGetString(name);
		GLenum err = glGetError();

		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "name=0x%X Beklenmeyen hata : 0x%X",
				      name, err);
			failCount++;
			continue;
		}

		passCount++;

		if (err == GL_NO_ERROR && str == NULL) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "name=0x%X Gecerli cagri NULL dondurdu.",
				      name);
			return;
		}

		if (err == GL_INVALID_ENUM && str != NULL) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "name=0x%X Hata sonrasi NULL beklenirdi.",
				      name);
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
 * TEST 4 : String Validity
 * ============================================================
 *
 * Amaç
 * ----
 * Gecerli name degerleri icin donen string'lerin
 * null-terminated ve makul uzunlukta oldugu dogrulanir.
 *
 * Bos olmayan bir uzunluk ve tanimli bir sonlandirici
 * beklenir.
 * ============================================================ */

void GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_004(void)
{
	int count = sizeof(validNames) / sizeof(validNames[0]);
	int i;

	clearGLErrors();

	for (i = 0; i < count; i++) {
		const GLubyte* str = glGetString(validNames[i]);
		size_t len;

		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "name=0x%X Sorgu hata uretti.",
				      validNames[i]);
			return;
		}

		if (str == NULL) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "name=0x%X NULL isaretci.",
				      validNames[i]);
			return;
		}

		/* strlen sonlandirici bulamazsa zaten cokerdi;
		   buraya ulasmak null-terminated oldugunu gosterir. */
		len = strlen((const char*)str);

		/*
		 * GL_EXTENSIONS bos olabilir (uzanti yoksa), digerleri
		 * bos olmamalidir.
		 */
		if (validNames[i] != GL_EXTENSIONS && len == 0) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "name=0x%X Bos string donduruldu.",
				      validNames[i]);
			return;
		}
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
 * glGetString() statik bir string'e isaretci dondurur.
 * Ayni name arka arkaya sorgulandiginda ayni isaretci
 * ve ayni icerik donmelidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_005(void)
{
	const int repeat = 100000;
	int i;
	const GLubyte* first;

	clearGLErrors();

	first = glGetString(GL_VERSION);
	if (glGetError() != GL_NO_ERROR || first == NULL) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Ilk sorgu basarisiz.");
		return;
	}

	for (i = 0; i < repeat; i++) {
		const GLubyte* str = glGetString(GL_VERSION);

		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %d hata uretti.", i);
			return;
		}

		if (str == NULL) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %d NULL donduruldu.", i);
			return;
		}

		/* Statik string oldugu icin isaretci ve icerik
		   her cagrida ayni olmalidir. */
		if (str != first ||
		    strcmp((const char*)str, (const char*)first) != 0) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %d Tutarsiz string.", i);
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
 * glGetString() cok sayida ard arda cagrilarak
 * surucunun yogun kullanim altinda kararliligi
 * dogrulanir.
 *
 * Test sonunda herhangi bir OpenGL hatasi olusmamalidir.
 * ============================================================ */

void GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_006(void)
{
	const unsigned int repeat = 1000000;
	unsigned int i;

	clearGLErrors();

	for (i = 0; i < repeat; i++) {
		glGetString(GL_VENDOR);
		glGetString(GL_RENDERER);
		glGetString(GL_VERSION);
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
void GS_GL20SC_SaSR_GS_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SaSR_GS_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);

}

void GS_GL20SC_SaSR_GS_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_SaSR_GS_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}