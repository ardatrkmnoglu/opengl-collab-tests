#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "GS_GL20SC_SF_H_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_SF_H_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_SF_H_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_SF_H_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_SF_H_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_SF_H_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_SF_H_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_SF_H_ROBUSTNESS_TC_007";

/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 *
 * Amaç
 * ----
 * glHint() fonksiyonunun tek gecerli target'i olan
 * GL_GENERATE_MIPMAP_HINT ile kabul ettigi uc gecerli
 * mode degeri dogrulanir.
 *
 * Ardindan gecersiz mode ve gecersiz target degerleri
 * gonderilerek GL_INVALID_ENUM uretildigi ve mevcut
 * state'in degismedigi kontrol edilir.
 * ============================================================ */

void GS_GL20SC_SF_H_ROBUSTNESS_TC_001(void)
{
	GLenum err;
	resetState_Hint();

	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure, "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_FASTEST))
		return;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure, "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_NICEST))
		return;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure, "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE))
		return;

	/* Gecersiz mode */
	glHint(GL_GENERATE_MIPMAP_HINT, (GLenum)0x0BAD);
	err = glGetError();
	if (err != GL_INVALID_ENUM) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen : 0x%X Gelen : 0x%X", GL_INVALID_ENUM, err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE))
		return;

	/* Gecersiz target */
	glHint((GLenum)0x0BAD, GL_FASTEST);
	err = glGetError();
	if (err != GL_INVALID_ENUM) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen : 0x%X Gelen : 0x%X", GL_INVALID_ENUM, err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE))
		return;

	resetState_Hint();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : Mode Stress Sweep
 * ============================================================
 *
 * Amaç
 * ----
 * 16-bit deger uzayindaki tum mode degerleri gecerli
 * target ile sistematik olarak denenir.
 *
 * Yalnizca
 *
 *      GL_FASTEST
 *      GL_NICEST
 *      GL_DONT_CARE
 *
 * degerlerinin kabul edilmesi beklenmektedir.
 *
 * Gecersiz mode degerlerinde state'in korunup korunmadigi
 * da dogrulanmaktadir.
 * ============================================================ */

void GS_GL20SC_SF_H_ROBUSTNESS_TC_002(void)
{
	GLenum mode;
	GLint currentMode = GL_DONT_CARE;
	int passCount = 0;
	int failCount = 0;

	resetState_Hint();

	for (mode = 0; mode < 65536; mode++) {
		GLenum expected =
		    (mode == GL_FASTEST ||
		     mode == GL_NICEST ||
		     mode == GL_DONT_CARE)
			? GL_NO_ERROR
			: GL_INVALID_ENUM;

		GLenum err;
		glHint(GL_GENERATE_MIPMAP_HINT, mode);
		err = glGetError();
		if (err != expected) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "Mode=0x%X Beklenen=0x%X Gelen=0x%X",
				      mode, expected, err);
			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_NO_ERROR) {
			currentMode = (GLint)mode;
		}

		if (err == GL_INVALID_ENUM) {
			if (!checkIntState(test_case_2, test_procedure,
					   GL_GENERATE_MIPMAP_HINT, currentMode))
				return;
		}
	}

	TEST_LOG_INFO("PASS : %d", passCount);
	TEST_LOG_INFO("FAIL : %d", failCount);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_2, test_procedure, "Basarisiz cagri sayisi : %d", failCount);
		return;
	}

	resetState_Hint();

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : Target Stress Sweep
 * ============================================================
 *
 * Amaç
 * ----
 * 16-bit deger uzayindaki tum target degerleri gecerli
 * bir mode ile denenir.
 *
 * ES 2.0'da yalnizca GL_GENERATE_MIPMAP_HINT gecerlidir;
 * diger tum target degerleri GL_INVALID_ENUM uretmelidir.
 *
 * Gecersiz target sonrasi state'in korunup korunmadigi
 * dogrulanir.
 * ============================================================ */

void GS_GL20SC_SF_H_ROBUSTNESS_TC_003(void)
{
	GLenum target;
	int passCount = 0;
	int failCount = 0;

	resetState_Hint();

	for (target = 0; target < 65536; target++) {
		GLenum expected =
		    (target == GL_GENERATE_MIPMAP_HINT)
			? GL_NO_ERROR
			: GL_INVALID_ENUM;

		GLenum err;
		glHint(target, GL_NICEST);
		err = glGetError();
		if (err != expected) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "Target=0x%X Beklenen=0x%X Gelen=0x%X",
				      target, expected, err);
			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_INVALID_ENUM) {
			if (!checkIntState(test_case_3, test_procedure,
					   GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE))
				return;
		}
	}

	TEST_LOG_INFO("PASS : %d", passCount);
	TEST_LOG_INFO("FAIL : %d", failCount);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_3, test_procedure, "Basarisiz cagri sayisi : %d", failCount);
		return;
	}

	resetState_Hint();

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

void GS_GL20SC_SF_H_ROBUSTNESS_TC_004(void)
{
	GLenum err;
	int i;
	int errorCount = 0;

	resetState_Hint();

	for (i = 0; i < 100; i++) {
		glHint((GLenum)(0x5000 + i), GL_FASTEST);
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Beklenen : 0x%X Gelen : 0x%X", GL_INVALID_ENUM, err);
			return;
		}
		errorCount++;
	}

	if (errorCount <= 0) {
		TEST_LOG_FAIL(test_case_4, test_procedure, "Hata kuyrugunda hic hata bulunamadi.");
		return;
	}

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_4, test_procedure, "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_4, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_NICEST))
		return;

	resetState_Hint();

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : State Preservation
 * ============================================================
 *
 * Amaç
 * ----
 * Gecersiz glHint() cagrilarinin mevcut
 * GL_GENERATE_MIPMAP_HINT degerini degistirmedigi
 * dogrulanmaktadir.
 *
 * Once gecerli bir durum olusturulur.
 * Daha sonra farkli gecersiz enum degerleri gonderilir.
 * ============================================================ */

void GS_GL20SC_SF_H_ROBUSTNESS_TC_005(void)
{
	GLenum err;
	GLenum invalidEnums[] = {
		0,
		1,
		2,
		1234,
		9999,
		0xFFFF,
		0xFFFFFFFF
	};

	int count = sizeof(invalidEnums) / sizeof(invalidEnums[0]);
	int i;

	resetState_Hint();

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_5, test_procedure, "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_5, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_NICEST))
		return;

	for (i = 0; i < count; i++) {
		/* Gecersiz mode */
		glHint(GL_GENERATE_MIPMAP_HINT, invalidEnums[i]);
		err = glGetError();
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Mode=0x%X Beklenen=0x%X Gelen=0x%X",
				      invalidEnums[i], GL_INVALID_ENUM, err);
			return;
		}
		if (!checkIntState(test_case_5, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_NICEST))
			return;

		/* Gecersiz target */
		glHint(invalidEnums[i], GL_FASTEST);
		err = glGetError();
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Target=0x%X Beklenen=0x%X Gelen=0x%X",
				      invalidEnums[i], GL_INVALID_ENUM, err);
			return;
		}
		if (!checkIntState(test_case_5, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_NICEST))
			return;
	}

	resetState_Hint();

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Rapid Toggle
 * ============================================================
 *
 * Amaç
 * ----
 * GL_FASTEST, GL_NICEST ve GL_DONT_CARE arasinda
 * yuz binlerce kez gecis yapilarak OpenGL durum
 * makinesinin kararliligi dogrulanir.
 *
 * Her cagridan sonra GL_GENERATE_MIPMAP_HINT sorgulanir.
 * ============================================================ */

void GS_GL20SC_SF_H_ROBUSTNESS_TC_006(void)
{
	const int repeat = 100000;
	int i;

	resetState_Hint();

	for (i = 0; i < repeat; i++) {
		GLenum expected;
		GLint current;

		switch (i % 3) {
		case 0:
			expected = GL_FASTEST;
			break;
		case 1:
			expected = GL_NICEST;
			break;
		default:
			expected = GL_DONT_CARE;
			break;
		}

		glHint(GL_GENERATE_MIPMAP_HINT, expected);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Iteration : %d Mode : 0x%X", i, expected);
			return;
		}

		glGetIntegerv(GL_GENERATE_MIPMAP_HINT, &current);
		if (current != (GLint)expected) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Iteration : %d Beklenen : 0x%X Gercek : 0x%X",
				      i, expected, current);
			return;
		}
	}

	resetState_Hint();

	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7 : Rapid Fire
 * ============================================================
 *
 * Amaç
 * ----
 * glHint() fonksiyonu cok sayida ard arda cagrilarak
 * surucunun yogun kullanim altinda kararliligi
 * dogrulanir.
 *
 * Test sonunda herhangi bir OpenGL hatasi olusmamali
 * ve son durum dogru sekilde korunmalidir.
 * ============================================================ */

void GS_GL20SC_SF_H_ROBUSTNESS_TC_007(void)
{
	const unsigned int repeat = 1000000;
	unsigned int i;

	resetState_Hint();

	for (i = 0; i < repeat; i++) {
		glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);
	}

	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure, "Yogun kullanim sonrasi hata olustu.");
		return;
	}
	if (!checkIntState(test_case_7, test_procedure, GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE))
		return;

	resetState_Hint();

	TEST_LOG_INFO("%u cagri tamamlandi.", repeat * 3);
	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

/* Initialization */
void GS_GL20SC_SF_H_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_H_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_H_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_H_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_H_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_H_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_H_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_H_ROBUSTNESS_TC_007();
	CHECK_ERROR(test_procedure);


}

void GS_GL20SC_SF_H_ROBUSTNESS_TP_001_draw(void) {
}
/* Cleanup */
void GS_GL20SC_SF_H_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}