#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * ============================================================
 *   TEST GRUBU: glClearColor
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glClearColor — Özel Float Degerleri (NaN, Infinity)
 * ============================================================
 *
 * Kayan nokta zehirlemesi (NaN ve Infinity). Spec bu konuda
 * çok net olmasa da kaliteli bir SC 2.0 sürücüsü bunu
 * sessizce yutmali veya güvenli hale getirmelidir.
 * Kesinlikle ÇÖKMEMELIDIR.
 * ============================================================ */
void WholeFramebufferOperations_ClearColor_TC_001(void) {
	// Kayan nokta zehirlemesi (NaN ve Infinity)
	// Spec bu konuda çok net olmasa da kaliteli bir SC 2.0 sürücüsü bunu
	// sessizce yutmalı veya kendi içinde güvenli bir hale getirmelidir
	// (Kesinlikle ÇÖKMEMELİDİR).
	glClearColor(NAN, INFINITY, -INFINITY, 1.5f);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR || err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearColor_TC_001",
		    "NaN/Inf değerleri atanırken beklenmeyen bir hata kodu "
		    "döndü veya sürücü kilitlendi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearColor_TC_001");
	}
}

/* ============================================================
 * glClearColor — Asiri Uc Sonlu Degerler (FLT_MAX)
 * ============================================================
 *
 * glClearColor, degerleri COPY zamaninda degil, ilgili buffer
 * sabit-noktali (fixed-point) oldugunda CLEAR zamaninda kirpar.
 * Dolayisiyla FLT_MAX / -FLT_MAX gibi asiri degerlerin state
 * olarak hatasiz ve oldugu gibi saklanmasi beklenir.
 * ============================================================ */
void WholeFramebufferOperations_ClearColor_TC_002(void) {
	glClearColor(FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearColor_TC_002",
		    "Asiri uc (FLT_MAX) degerleri atanirken beklenmeyen "
		    "bir hata döndü."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearColor_TC_002");
	}

	GLfloat color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);

	if (!(color[0] == FLT_MAX && color[1] == -FLT_MAX &&
	      color[2] == FLT_MAX && color[3] == -FLT_MAX)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearColor_TC_002",
		    "Sürücü set-zamaninda deger kirpmis olabilir; state "
		    "oldugu gibi saklanmaliydi."
		    " Actual: 0x%04X",
		    (unsigned int)color[0]);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearColor_TC_002");
	}
}

/* ============================================================
 * glClearColor — Tam Sinir Degerleri (0.0 / 1.0)
 * ============================================================
 *
 * Normal aralik [0,1]'in tam sinirlarinda hicbir kirpma veya
 * hassasiyet kaymasi (precision drift) olmadan degerlerin
 * birebir geri okunabildigi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearColor_TC_003(void) {
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearColor_TC_003",
			      "Tam sinir degerleri (0.0 / 1.0) reddedildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearColor_TC_003");
	}

	GLfloat color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);

	if (!(color[0] == 0.0f && color[1] == 1.0f && color[2] == 0.0f &&
	      color[3] == 1.0f)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearColor_TC_003",
		    "Tam sinir degerlerinde hassasiyet kaymasi tespit "
		    "edildi."
		    " Actual: 0x%04X",
		    (unsigned int)color[0]);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearColor_TC_003");
	}
}

/* ============================================================
 * glClearColor / glGetFloatv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_COLOR_CLEAR_VALUE sorgusuna NULL isaretçi geçilerek çökme
 * dayanikliligi ve state bütünlügü dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearColor_TC_004(void) {
	glGetFloatv(GL_COLOR_CLEAR_VALUE, NULL);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	GLfloat color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);

	if (!(color[0] == 0.25f && color[1] == 0.5f && color[2] == 0.75f &&
	      color[3] == 1.0f)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearColor_TC_004",
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)color[0]);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearColor_TC_004");
	}
}

/* ============================================================
 * glClearColor — Subnormal (Denormal) Degerler
 * ============================================================
 *
 * Bazi GPU'larda flush-to-zero davranisi subnormal degerlerde
 * beklenmedik istisnalara (exception) yol acabilir. En küçük
 * subnormal float degerler gönderilerek sürücünün bunu sessizce
 * ve güvenli sekilde islemesi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearColor_TC_005(void) {
	GLfloat smallestSubnormal = 1.401298464e-45f;

	glClearColor(smallestSubnormal, -smallestSubnormal, smallestSubnormal,
		     1.0f);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearColor_TC_005",
		    "Subnormal (denormal) degerler atanirken beklenmeyen "
		    "bir hata döndü (flush-to-zero istisnasi olabilir)."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearColor_TC_005");
	}
}
