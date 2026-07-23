#include "../../../include/macro.h"
#include "../../../include/rtests.h"

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
void WholeFramebufferOperations_DepthMask_TC_001(void) {
	glDepthMask((GLboolean)0x100);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_DepthMask_TC_001",
		    "Bayt sinirindaki tasma degeri (0x100) hata ürettirdi."
		    " Actual: 0x%04X",
		    err1);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_001");
	}

	GLboolean d = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_FALSE)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_DepthMask_TC_001",
			      "0x100 alt bayta sarip 0x00 (FALSE) olmaliydi."
			      " Actual: 0x%04X",
			      (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_001");
	}

	glDepthMask((GLboolean)0x101);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_DepthMask_TC_001",
		    "Bayt sinirindaki tasma degeri (0x101) hata ürettirdi."
		    " Actual: 0x%04X",
		    err2);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_001");
	}

	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_TRUE)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_DepthMask_TC_001",
			      "0x101 alt bayta sarip 0x01 (TRUE) olmaliydi."
			      " Actual: 0x%04X",
			      (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_001");
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
void WholeFramebufferOperations_DepthMask_TC_002(void) {
	glGetBooleanv(GL_DEPTH_WRITEMASK, NULL);

	glDepthMask(GL_TRUE);

	GLboolean d = GL_FALSE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_TRUE)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_DepthMask_TC_002",
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_002");
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
void WholeFramebufferOperations_DepthMask_TC_003(void) {
	glDepthMask(GL_FALSE);
	glColorMask(0xFF, 0x00, 0xFF, 0x00);
	glClear(GL_COLOR_BUFFER_BIT);

	GLboolean d = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_FALSE)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_DepthMask_TC_003",
		    "Alakasiz cagrilar (ColorMask/Clear) DEPTH_WRITEMASK "
		    "state'ini etkilememeliydi."
		    " Actual: 0x%04X",
		    (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_003");
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
void WholeFramebufferOperations_DepthMask_TC_004(void) {
	for (int i = 0; i < 2048; i++) {
		glDepthMask((i % 3 == 0) ? 0xAB : 0x00);
	}

	GLenum err = glGetError();
	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_DepthMask_TC_004",
		    "Tekrarli deger degisimleri sonrasinda hata durumu "
		    "biriktirilmis olabilir."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_004");
	}

	// Son iterasyon i=2047, 2047 % 3 == 1 (!=0) -> 0x00 gönderilmis ->
	// FALSE
	GLboolean d = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &d);

	if (!(d == GL_FALSE)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_DepthMask_TC_004",
		    "Stres döngüsü sonrasi son state beklenenle uyusmuyor."
		    " Actual: 0x%04X",
		    (unsigned int)d);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_DepthMask_TC_004");
	}
}
