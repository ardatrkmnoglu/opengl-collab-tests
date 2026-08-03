#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_FOP_DM_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_FOP_DM_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_FOP_DM_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_FOP_DM_ROBUSTNESS_TC_004";

/* ============================================================
 * ============================================================
 *   TEST GRUBU: glDepthMask
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glDepthMask — Bayt Sinirinda Tasma (Byte Boundary Wraparound)
 * ============================================================
 *
 * ColorMask'teki ile ayni mantik: GLboolean tek bayttir. 0x100
 * alt bayta sarip 0x00 (FALSE), 0x101 ise 0x01'e sarip TRUE
 * üretmelidir.
 * ============================================================ */
void GS_GL20SC_FOP_DM_ROBUSTNESS_TC_001(void) {
	glDepthMask((GLboolean)0x100);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Bayt sinirindaki tasma degeri (0x100) hata ürettirdi."
		    " Actual: 0x%04X",
		    err1);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLboolean d = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_FALSE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "0x100 alt bayta sarip 0x00 (FALSE) olmaliydi."
			      " Actual: 0x%04X",
			      (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glDepthMask((GLboolean)0x101);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Bayt sinirindaki tasma degeri (0x101) hata ürettirdi."
		    " Actual: 0x%04X",
		    err2);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_TRUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "0x101 alt bayta sarip 0x01 (TRUE) olmaliydi."
			      " Actual: 0x%04X",
			      (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glDepthMask / glGetBooleanv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_DEPTH_WRITEMASK sorgusuna NULL isaretçi geçilerek sürücünün
 * çökmedigi ve ardindan gelen gecerli çagrilarin dogru çalistigi
 * dogrulanir.
 * ============================================================ */
void GS_GL20SC_FOP_DM_ROBUSTNESS_TC_002(void) {
	glGetBooleanv(GL_DEPTH_WRITEMASK, NULL);

	glDepthMask(GL_TRUE);

	GLboolean d = GL_FALSE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_TRUE)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glDepthMask — Diger State'lerden Izolasyon
 * ============================================================
 *
 * DepthMask FALSE yapildiktan sonra alakasiz cagrilar (ColorMask
 * degisimi, glClear) yapilir. GL_DEPTH_WRITEMASK'in bu alakasiz
 * cagrilardan etkilenmeden FALSE kalmasi beklenir. State
 * 'sizintisi' (cross-contamination) yakalamayi hedefler.
 * ============================================================ */
void GS_GL20SC_FOP_DM_ROBUSTNESS_TC_003(void) {
	glDepthMask(GL_FALSE);
	glColorMask(0xFF, 0x00, 0xFF, 0x00);
	glClear(GL_COLOR_BUFFER_BIT);

	GLboolean d = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_FALSE)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Alakasiz cagrilar (ColorMask/Clear) DEPTH_WRITEMASK "
		    "state'ini etkilememeliydi."
		    " Actual: 0x%04X",
		    (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glDepthMask — Tekrarli Asiri Deger Stresi (Stress Test)
 * ============================================================
 *
 * Binlerce kez tuhaf sentinel ve sifir degerleri arasinda
 * gidip gelinir. Amac: hata birikmesi ya da yaris durumu
 * (race) benzeri bir tutarsizlik yakalamak.
 * ============================================================ */
void GS_GL20SC_FOP_DM_ROBUSTNESS_TC_004(void) {
	for (int i = 0; i < 2048; i++) {
		glDepthMask((i % 3 == 0) ? 0xAB : 0x00);
	}

	GLenum err = glGetError();
	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Tekrarli deger degisimleri sonrasinda hata durumu "
		    "biriktirilmis olabilir."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	// Son iterasyon i=2047, 2047 % 3 == 1 (!=0) -> 0x00 gönderilmis ->
	// FALSE
	GLboolean d = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_FALSE)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Stres döngüsü sonrasi son state beklenenle uyusmuyor."
		    " Actual: 0x%04X",
		    (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_FOP_DM_ROBUSTNESS_TC_001();
	GS_GL20SC_FOP_DM_ROBUSTNESS_TC_002();
	GS_GL20SC_FOP_DM_ROBUSTNESS_TC_003();
	GS_GL20SC_FOP_DM_ROBUSTNESS_TC_004();
}

/* Cleanup */
void GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
