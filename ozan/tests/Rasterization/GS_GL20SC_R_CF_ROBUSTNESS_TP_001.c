#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char *test_procedure = "GS_GL20SC_R_CF_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_007";
static const char *test_case_8 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_008";
static const char *test_case_9 = "GS_GL20SC_R_CF_ROBUSTNESS_TC_009";

/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 *
 * Amaç
 * ----
 * glCullFace() fonksiyonunun kabul ettiği üç geçerli
 * enum değeri doğrulanır.
 *
 * Ardından geçersiz enum değerleri gönderilerek
 * GL_INVALID_ENUM üretildiği ve mevcut state'in
 * değişmediği kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_001(void) {
	GLenum err;

	resetState_CullFace();
	glCullFace(GL_BACK);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_CULL_FACE_MODE,
			   GL_BACK))
		return;

	glCullFace(GL_FRONT);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_CULL_FACE_MODE,
			   GL_FRONT))
		return;

	glCullFace(GL_FRONT_AND_BACK);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_CULL_FACE_MODE,
			   GL_FRONT_AND_BACK))
		return;

	glCullFace((GLenum)0x0BAD);
	err = glGetError();

	if (err != GL_INVALID_ENUM) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen : 0x%X Gelen : 0x%X", GL_INVALID_ENUM,
			      err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_CULL_FACE_MODE,
			   GL_FRONT_AND_BACK))
		return;

	glCullFace(GL_CCW);
	err = glGetError();

	if (err != GL_INVALID_ENUM) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen : 0x%X Gelen : 0x%X", GL_INVALID_ENUM,
			      err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_CULL_FACE_MODE,
			   GL_FRONT_AND_BACK))
		return;

	resetState_CullFace();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : Stress Sweep
 * ============================================================
 *
 * Amaç
 * ----
 * 16-bit GLenum uzayındaki tüm değerler sistematik
 * olarak denenir.
 *
 * Yalnızca
 *
 *      GL_BACK
 *      GL_FRONT
 *      GL_FRONT_AND_BACK
 *
 * değerlerinin kabul edilmesi beklenmektedir.
 *
 * Geçersiz değerlerde state'in korunup korunmadığı
 * da doğrulanmaktadır.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_002(void) {
	GLenum mode;
	GLint currentMode = GL_BACK;
	int passCount = 0;
	int failCount = 0;

	resetState_CullFace();

	for (mode = 0; mode < 65536; mode++) {
		GLenum expected = (mode == GL_BACK || mode == GL_FRONT ||
				   mode == GL_FRONT_AND_BACK)
				      ? GL_NO_ERROR
				      : GL_INVALID_ENUM;

		GLenum err;
		glCullFace(mode);
		err = glGetError();
		if (err != expected) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
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
					   GL_CULL_FACE_MODE, currentMode))
				return;
		}
	}

	TEST_LOG_INFO("PASS : %d", passCount);
	TEST_LOG_INFO("FAIL : %d", failCount);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Basarisiz cagri sayisi : %d", failCount);
		return;
	}
	resetState_CullFace();

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : Error Queue Management
 * ============================================================
 *
 * Amaç
 * ----
 * Arka arkaya çok sayıda geçersiz enum gönderildiğinde
 * hata kuyruğunun bozulmadığı doğrulanır.
 *
 * Daha sonra geçerli bir çağrı yapılarak sürücünün
 * normal çalışmaya döndüğü kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_003(void) {
	GLenum err;
	int i;
	int errorCount = 0;

	resetState_CullFace();
	for (i = 0; i < 100; i++) {
		glCullFace((GLenum)(0x5000 + i));
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "Beklenen : 0x%X Gelen : 0x%X",
				      GL_INVALID_ENUM, err);
			return;
		}

		errorCount++;
	}
	if (errorCount <= 0) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Hata kuyrugunda hic hata bulunamadi.");
		return;
	}
	glCullFace(GL_FRONT);

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_3, test_procedure, GL_CULL_FACE_MODE,
			   GL_FRONT))
		return;

	resetState_CullFace();

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : Rapid Toggle
 * ============================================================
 *
 * Amaç
 * ----
 * GL_BACK, GL_FRONT ve GL_FRONT_AND_BACK arasında
 * yüz binlerce kez geçiş yapılarak OpenGL durum
 * makinesinin kararlılığı doğrulanır.
 *
 * Her çağrıdan sonra GL_CULL_FACE_MODE sorgulanır.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_004(void) {
	const int repeat = 100000;
	int i;

	resetState_CullFace();
	for (i = 0; i < repeat; i++) {
		GLenum expected;
		GLint current;

		switch (i % 3) {
		case 0:
			expected = GL_BACK;
			break;

		case 1:
			expected = GL_FRONT;
			break;

		default:
			expected = GL_FRONT_AND_BACK;
			break;
		}

		glCullFace(expected);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Iteration : %d Enum : 0x%X", i,
				      expected);
			return;
		}
		glGetIntegerv(GL_CULL_FACE_MODE, &current);
		if (current != (GLint)expected) {
			TEST_LOG_FAIL(
			    test_case_4, test_procedure,
			    "Iteration : %d Beklenen : 0x%X Gercek : 0x%X", i,
			    expected, current);
			return;
		}
	}

	resetState_CullFace();

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : State Preservation
 * ============================================================
 *
 * Amaç
 * ----
 * Geçersiz glCullFace() çağrılarının mevcut
 * GL_CULL_FACE_MODE değerini değiştirmediği
 * doğrulanmaktadır.
 *
 * Önce geçerli bir durum oluşturulur.
 * Daha sonra farklı geçersiz enum değerleri
 * gönderilir.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_005(void) {
	GLenum err;
	GLenum invalidEnums[] = {0, 1, 2, 1234, 9999, 0xFFFF, 0xFFFFFFFF};

	int count = sizeof(invalidEnums) / sizeof(invalidEnums[0]);
	int i;

	resetState_CullFace();
	glCullFace(GL_FRONT);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}

	if (!checkIntState(test_case_5, test_procedure, GL_CULL_FACE_MODE,
			   GL_FRONT))
		return;

	for (i = 0; i < count; i++) {
		glCullFace(invalidEnums[i]);

		err = glGetError();

		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
				      invalidEnums[i], GL_INVALID_ENUM, err);
			return;
		}

		if (!checkIntState(test_case_5, test_procedure,
				   GL_CULL_FACE_MODE, GL_FRONT))
			return;
	}
	resetState_CullFace();

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : FrontFace Combination
 * ============================================================
 *
 * Amaç
 * ----
 * glFrontFace() ile glCullFace() fonksiyonlarının
 * birlikte kullanıldığında birbirlerinin durumunu
 * bozmadığı doğrulanmaktadır.
 *
 * Tüm geçerli kombinasyonlar denenmektedir.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_006(void) {
	GLenum frontModes[] = {GL_CCW, GL_CW};

	GLenum cullModes[] = {GL_BACK, GL_FRONT, GL_FRONT_AND_BACK};

	int i;
	int j;

	resetState_CullFace();
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			GLint front;
			GLint cull;
			glFrontFace(frontModes[i]);
			if (glGetError() != GL_NO_ERROR) {
				TEST_LOG_FAIL(
				    test_case_6, test_procedure,
				    "glFrontFace basarisiz. Enum : 0x%X",
				    frontModes[i]);
				return;
			}
			glCullFace(cullModes[j]);

			if (glGetError() != GL_NO_ERROR) {
				TEST_LOG_FAIL(
				    test_case_6, test_procedure,
				    "glCullFace basarisiz. Enum : 0x%X",
				    cullModes[j]);
				return;
			}
			glGetIntegerv(GL_FRONT_FACE, &front);
			glGetIntegerv(GL_CULL_FACE_MODE, &cull);
			if (front != (GLint)frontModes[i]) {
				TEST_LOG_FAIL(test_case_6, test_procedure,
					      "FrontFace bozuldu. Beklenen : "
					      "0x%X Gercek : 0x%X",
					      frontModes[i], front);
				return;
			}
			if (cull != (GLint)cullModes[j]) {
				TEST_LOG_FAIL(test_case_6, test_procedure,
					      "CullFace bozuldu. Beklenen : "
					      "0x%X Gercek : 0x%X",
					      cullModes[j], cull);
				return;
			}
		}
	}
	resetState_CullFace();

	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7 : Large Invalid Enum Values
 * ============================================================
 *
 * Amaç
 * ----
 * Çok büyük GLenum değerleri gönderildiğinde
 * sürücünün çökmediği ve yalnızca
 * GL_INVALID_ENUM ürettiği doğrulanır.
 *
 * Ayrıca başarısız çağrıların mevcut durumu
 * değiştirmediği kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_007(void) {
	GLenum values[] = {(GLenum)0x10000, (GLenum)0x7FFFFFFF,
			   (GLenum)0x80000000, (GLenum)0xFFFFFFFF};

	int count = sizeof(values) / sizeof(values[0]);
	int i;

	resetState_CullFace();
	for (i = 0; i < count; i++) {
		GLenum err;
		glCullFace(values[i]);
		err = glGetError();

		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_7, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
				      values[i], GL_INVALID_ENUM, err);
			return;
		}
		if (!checkIntState(test_case_7, test_procedure,
				   GL_CULL_FACE_MODE, GL_BACK))
			return;
	}

	resetState_CullFace();

	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

/* ============================================================
 * TEST 8 : Rapid Fire
 * ============================================================
 *
 * Amaç
 * ----
 * glCullFace() fonksiyonunu çok kısa aralıklarla
 * art arda çağırarak sürücünün yoğun kullanım altında
 * kararlılığını doğrular.
 *
 * Test sonunda herhangi bir OpenGL hatası oluşmamalı
 * ve son durum doğru şekilde korunmalıdır.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_008(void) {
	const unsigned int repeat = 1000000;
	unsigned int i;

	resetState_CullFace();
	for (i = 0; i < repeat; i++) {
		glCullFace(GL_BACK);
		glCullFace(GL_FRONT);
		glCullFace(GL_FRONT_AND_BACK);
	}
	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_8, test_procedure,
			      "Yogun kullanim sonrasi hata olustu.");
		return;
	}
	if (!checkIntState(test_case_8, test_procedure, GL_CULL_FACE_MODE,
			   GL_FRONT_AND_BACK))
		return;

	resetState_CullFace();

	TEST_LOG_INFO("%u cagri tamamlandi.", repeat * 3);
	TEST_LOG_SUCCESS(test_case_8, test_procedure);
}

/* ============================================================
 * TEST 9 : Random Fuzz Test
 * ============================================================
 *
 * Amaç
 * ----
 * Rastgele GLenum değerleri gönderilerek sürücünün
 * beklenmeyen girdiler karşısındaki dayanıklılığı
 * test edilir.
 *
 * Beklenen sonuçlar:
 *
 *      GL_NO_ERROR
 *      GL_INVALID_ENUM
 *
 * Bunların dışındaki herhangi bir hata başarısızlık
 * olarak değerlendirilir.
 * ============================================================ */

void GS_GL20SC_R_CF_ROBUSTNESS_TC_009(void) {
	unsigned int i;
	GLint lastValid = GL_BACK;

	resetState_CullFace();

	for (i = 0; i < 1000000; i++) {
		GLenum value;
		GLenum err;
		switch (i % 5) {
			case 0:
				value = GL_BACK;
				break;
			case 1:
				value = GL_FRONT;
				break;
			case 2:
				value = GL_FRONT_AND_BACK;
				break;
			default:
				value = (GLenum)(i % 65536);
				break;
		}

		glCullFace(value);
		err = glGetError();
		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_9, test_procedure,
					  "Iteration : %u Enum : 0x%X Error : 0x%X",
					  i, value, err);

			return;
		}

		if (err == GL_NO_ERROR) {
			lastValid = (GLint)value;
		}

		if (err == GL_INVALID_ENUM) {
			if (!checkIntState(test_case_9, test_procedure,
					   GL_CULL_FACE_MODE, lastValid))
				return;
		}
	}

	resetState_CullFace();

	TEST_LOG_INFO("1,000,000 test tamamlandi.");
	TEST_LOG_SUCCESS(test_case_9, test_procedure);
}

/* Initialization */
void GS_GL20SC_R_CF_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_007();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_008();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_CF_ROBUSTNESS_TC_009();
	CHECK_ERROR(test_procedure);

}

void GS_GL20SC_R_CF_ROBUSTNESS_TP_001_draw(void) {
}
/* Cleanup */
void GS_GL20SC_R_CF_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}