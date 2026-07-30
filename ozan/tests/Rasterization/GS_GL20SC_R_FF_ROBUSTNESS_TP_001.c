#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char *test_procedure = "GS_GL20SC_R_FF_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_007";
static const char *test_case_8 = "GS_GL20SC_R_FF_ROBUSTNESS_TC_008";

/* ============================================================
 * TEST 1 : Error Queue Management
 * ============================================================
 *
 * Amaç
 * ----
 * Arka arkaya çok sayıda geçersiz enum gönderildiğinde
 * hata kuyruğunun bozulmadığını doğrular.
 *
 * Daha sonra geçerli bir çağrı yapılarak OpenGL'in
 * normal çalışmaya döndüğü kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_FF_ROBUSTNESS_TC_001(void) {
	GLenum err;
	int errorCount = 0;
	int i;

	resetState_FrontFace();
	for (i = 0; i < 100; i++) {
		glFrontFace((GLenum)(0x5000 + i));
	}
	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_1, test_procedure,
				      "Beklenen : 0x%X Gelen : 0x%X",
				      GL_INVALID_ENUM, err);
			return;
		}
		errorCount++;
	}
	if (errorCount <= 0) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Hata kuyrugunda hic hata bulunamadi.");
		return;
	}
	glFrontFace(GL_CW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_1, test_procedure, GL_FRONT_FACE, GL_CW))
		return;

	resetState_FrontFace();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : Rapid Toggle
 * ============================================================
 *
 * Amaç
 * ----
 * GL_CW ile GL_CCW arasında on binlerce kez geçiş
 * yaptırılarak state makinesinin bozulmadığı doğrulanır.
 *
 * Her çağrı sonrasında GL_FRONT_FACE sorgulanır.
 * ============================================================ */

void GS_GL20SC_R_FF_ROBUSTNESS_TC_002(void) {
	const int repeat = 100000;
	int i;

	resetState_FrontFace();
	for (i = 0; i < repeat; i++) {
		GLenum expected = (i & 1) ? GL_CCW : GL_CW;
		GLint current;
		glFrontFace(expected);
		if (glGetError() != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "Iteration : %d Enum : 0x%X", i,
				      expected);
			return;
		}
		glGetIntegerv(GL_FRONT_FACE, &current);
		if (current != (GLint)expected) {
			TEST_LOG_FAIL(
			    test_case_2, test_procedure,
			    "Iteration : %d Beklenen : 0x%X Gercek : 0x%X", i,
			    expected, current);
			return;
		}
	}

	resetState_FrontFace();

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : Mixed Valid / Invalid Calls
 * ============================================================
 *
 * Amaç
 * ----
 * Geçerli ve geçersiz çağrılar karıştırılarak
 * sürücünün hata durumundan doğru şekilde
 * çıkabildiği doğrulanır.
 *
 * Ayrıca geçersiz enum sonrasında state'in
 * değişmediği de kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_FF_ROBUSTNESS_TC_003(void) {
	GLenum sequence[] = {GL_CW, 0x1111, GL_CCW, 0x2222,
			     GL_CW, 0x3333, GL_CCW, 0x4444};

	int count = sizeof(sequence) / sizeof(sequence[0]);

	int i;

	resetState_FrontFace();

	for (i = 0; i < count; i++) {
		GLenum err;
		GLenum value = sequence[i];
		GLenum expectedError = (value == GL_CW || value == GL_CCW)
					   ? GL_NO_ERROR
					   : GL_INVALID_ENUM;
		glFrontFace(value);
		err = glGetError();
		if (err != expectedError) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
				      value, expectedError, err);
			return;
		}
		if (value == GL_CW) {
			if (!checkIntState(test_case_3, test_procedure,
					   GL_FRONT_FACE, GL_CW))
				return;
		}
		if (value == GL_CCW) {
			if (!checkIntState(test_case_3, test_procedure,
					   GL_FRONT_FACE, GL_CW))
				return;
		}
	}

	resetState_FrontFace();

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : State Preservation
 * ============================================================
 *
 * Amaç
 * ----
 * Geçersiz glFrontFace() çağrılarının mevcut OpenGL
 * durumunu değiştirmediğini doğrular.
 *
 * Önce geçerli bir durum oluşturulur.
 * Daha sonra farklı geçersiz enum değerleri gönderilir.
 * Her başarısız çağrıdan sonra GL_FRONT_FACE
 * değerinin değişmediği kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_FF_ROBUSTNESS_TC_004(void) {
	GLenum err;
	GLenum invalidEnums[] = {0, 1, 2, 1234, 9999, 0xFFFF, 0xFFFFFFFF};

	int count = sizeof(invalidEnums) / sizeof(invalidEnums[0]);
	int i;

	resetState_FrontFace();
	glFrontFace(GL_CW);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}
	if (!checkIntState(test_case_4, test_procedure, GL_FRONT_FACE, GL_CW))
		return;

	for (i = 0; i < count; i++) {
		glFrontFace(invalidEnums[i]);
		err = glGetError();
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
				      invalidEnums[i], GL_INVALID_ENUM, err);
			return;
		}
		if (!checkIntState(test_case_4, test_procedure, GL_FRONT_FACE,
				   GL_CW))
			return;
	}

	resetState_FrontFace();

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Cull Face Combination
 * ============================================================
 *
 * Amaç
 * ----
 * glFrontFace() ve glCullFace() fonksiyonlarının birlikte
 * çalışırken birbirlerinin durumunu bozmadığını doğrular.
 *
 * Tüm geçerli kombinasyonlar denenir ve her çağrı
 * sonrasında OpenGL state'i kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_FF_ROBUSTNESS_TC_005(void) {
	GLenum frontModes[] = {GL_CCW, GL_CW};

	GLenum cullModes[] = {GL_FRONT, GL_BACK, GL_FRONT_AND_BACK};

	int i;
	int j;

	resetState_FrontFace();
	glEnable(GL_CULL_FACE);
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			GLint currentFront;
			GLint currentCull;
			glFrontFace(frontModes[i]);

			if (glGetError() != GL_NO_ERROR) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "glFrontFace basarisiz. Enum : 0x%X",
				    frontModes[i]);
				glDisable(GL_CULL_FACE);
				return;
			}
			glCullFace(cullModes[j]);
			if (glGetError() != GL_NO_ERROR) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "glCullFace basarisiz. Enum : 0x%X",
				    cullModes[j]);
				glDisable(GL_CULL_FACE);
				return;
			}

			glGetIntegerv(GL_FRONT_FACE, &currentFront);
			glGetIntegerv(GL_CULL_FACE_MODE, &currentCull);

			if (currentFront != (GLint)frontModes[i]) {
				TEST_LOG_FAIL(test_case_5, test_procedure,
					      "FrontFace bozuldu. Beklenen : "
					      "0x%X Gercek : 0x%X",
					      frontModes[i], currentFront);
				glDisable(GL_CULL_FACE);
				return;
			}
			if (currentCull != (GLint)cullModes[j]) {
				TEST_LOG_FAIL(test_case_5, test_procedure,
					      "CullFace bozuldu. Beklenen : "
					      "0x%X Gercek : 0x%X",
					      cullModes[j], currentCull);
				glDisable(GL_CULL_FACE);
				return;
			}
		}
	}

	glDisable(GL_CULL_FACE);
	resetState_FrontFace();

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Large Invalid Enum Values
 * ============================================================
 *
 * Amaç
 * ----
 * Çok büyük GLenum değerleri gönderildiğinde
 * sürücünün çökmediği ve yalnızca
 * GL_INVALID_ENUM ürettiği doğrulanır.
 *
 * Ayrıca başarısız çağrıların mevcut durumu
 * değiştirmediği de kontrol edilir.
 * ============================================================ */

void GS_GL20SC_R_FF_ROBUSTNESS_TC_006(void) {
	GLenum values[] = {(GLenum)0x10000, (GLenum)0x7FFFFFFF,
			   (GLenum)0x80000000, (GLenum)0xFFFFFFFF};

	int count = sizeof(values) / sizeof(values[0]);
	int i;

	resetState_FrontFace();

	for (i = 0; i < count; i++) {
		GLenum err;
		glFrontFace(values[i]);
		err = glGetError();
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
				      values[i], GL_INVALID_ENUM, err);
			return;
		}
		if (!checkIntState(test_case_6, test_procedure, GL_FRONT_FACE,
				   GL_CCW))
			return;
	}

	resetState_FrontFace();

	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7 : Rapid Fire
 * ============================================================
 *
 * Amaç
 * ----
 * glFrontFace() fonksiyonunu çok kısa aralıklarla
 * art arda çağırarak sürücünün yoğun kullanım altında
 * kararlılığını doğrular.
 *
 * Test sonunda herhangi bir OpenGL hatası oluşmamalı
 * ve son durum doğru şekilde korunmalıdır.
 * ============================================================ */

void GS_GL20SC_R_FF_ROBUSTNESS_TC_007(void) {
	const unsigned int repeat = 1000000;
	unsigned int i;

	resetState_FrontFace();

	for (i = 0; i < repeat; i++) {
		glFrontFace(GL_CW);
		glFrontFace(GL_CCW);
	}

	if (glGetError() != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Yogun kullanim sonrasi hata olustu.");
		return;
	}
	if (!checkIntState(test_case_7, test_procedure, GL_FRONT_FACE, GL_CCW))
		return;

	resetState_FrontFace();

	TEST_LOG_INFO("%u cift cagri tamamlandi.", repeat);
	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

/* ============================================================
 * TEST 8 : Random Fuzz Test
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

void GS_GL20SC_R_FF_ROBUSTNESS_TC_008(void) {
	unsigned int i;

	resetState_FrontFace();

	for (i = 0; i < 1000000; i++) {
		GLenum value;
		GLenum err;
		switch (i % 4) {
			case 0:
				value = GL_CW;
				break;
			case 1:
				value = GL_CCW;
				break;
			default:
				value = (GLenum)(i % 65536);
				break;
		}

		glFrontFace(value);
		err = glGetError();
		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_8, test_procedure,
					  "Iteration : %u Enum : 0x%X Error : 0x%X",
					  i, value, err);
			return;
		}
	}

	resetState_FrontFace();

	TEST_LOG_INFO("1,000,000 test tamamlandi.");
	TEST_LOG_SUCCESS(test_case_8, test_procedure);
}

/* Initialization */
void GS_GL20SC_R_FF_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_007();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_R_FF_ROBUSTNESS_TC_008();
	CHECK_ERROR(test_procedure);

}

void GS_GL20SC_R_FF_ROBUSTNESS_TP_001_draw(void) {
}
/* Cleanup */
void GS_GL20SC_R_FF_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}