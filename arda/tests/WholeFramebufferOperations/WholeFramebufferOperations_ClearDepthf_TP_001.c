#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * ============================================================
 *   TEST GRUBU: glClearDepthf
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glClearDepthf — Clamping Dogrulamasi
 * ============================================================
 *
 * Depth degeri matematiksel olarak [0.0, 1.0] araliginda
 * olmalidir. Sinir disina çikilarak sürücünün sessizce
 * kırpma (clamp) yapip yapamadigi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearDepthf_TC_001(void) {
	// Depth değeri matematikte sadece [0.0, 1.0] aralığında olabilir.
	// Biz sınırların çok dışına taşıyoruz.
	glClearDepthf(5000.0f);
	GLenum err = glGetError();

	// Spesifikasyon: "Değerler hata fırlatmadan sessizce [0,1] aralığına
	// kırpılır."
	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearDepthf_TC_001",
			      "Sınır dışı depth atamasında beklenmeyen bir "
			      "hata fırlatıldı."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_001");
	}

	// Sürücü gerçekten değeri 1.0'a kilitledi (clamp) mi?
	GLfloat depth = -1.0f;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depth);

	if (!(depth == 1.0f)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearDepthf_TC_001",
			      "Sürücü aşırı depth değerini [0,1] aralığına "
			      "kırpmayı başaramadı."
			      " Actual: 0x%04X",
			      (unsigned int)depth);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_001");
	}
}

/* ============================================================
 * glClearDepthf — Negatif Sinirda Kirpma (Lower Bound Clamp)
 * ============================================================
 *
 * Üst sinir kirpma testinin ayna görüntüsü: asiri negatif bir
 * deger gönderilerek sürücünün bunu 0.0'a kirpip kirpmadigi
 * dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearDepthf_TC_002(void) {
	glClearDepthf(-5000.0f);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearDepthf_TC_002",
		    "Sinir disi negatif depth atamasinda beklenmeyen bir "
		    "hata firlatildi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_002");
	}

	GLfloat depth = -1.0f;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depth);

	if (!(depth == 0.0f)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearDepthf_TC_002",
		    "Sürücü asiri negatif depth degerini 0.0'a kirpmayi "
		    "basaramadi."
		    " Actual: 0x%04X",
		    (unsigned int)depth);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_002");
	}
}

/* ============================================================
 * glClearDepthf — Tam Sinir Degerleri (0.0 / 1.0)
 * ============================================================
 *
 * Kirpma gerektirmeyen tam sinir degerlerinin (0.0 ve 1.0)
 * herhangi bir hassasiyet kaymasi olmadan birebir saklandigi
 * dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearDepthf_TC_003(void) {
	glClearDepthf(0.0f);
	GLfloat depth = -1.0f;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depth);

	if (!(depth == 0.0f)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearDepthf_TC_003",
			      "Tam alt sinir (0.0) hatali saklandi."
			      " Actual: 0x%04X",
			      (unsigned int)depth);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_003");
	}

	glClearDepthf(1.0f);
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depth);

	if (!(depth == 1.0f)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearDepthf_TC_003",
			      "Tam üst sinir (1.0) hatali saklandi."
			      " Actual: 0x%04X",
			      (unsigned int)depth);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_003");
	}
}

/* ============================================================
 * glClearDepthf — Özel Float Degerleri (NaN, Infinity)
 * ============================================================
 *
 * ClearColor testindeki NaN/Infinity zehirlemesinin depth
 * karsiligi. Sürücü kesinlikle çökmemeli, ya sessizce kabul
 * etmeli ya da GL_INVALID_VALUE ile reddetmelidir.
 * ============================================================ */
void WholeFramebufferOperations_ClearDepthf_TC_004(void) {
	glClearDepthf(NAN);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR || err1 == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearDepthf_TC_004",
			      "NaN depth degeri atanirken sürücü beklenmedik "
			      "sekilde davrandi."
			      " Actual: 0x%04X",
			      err1);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_004");
	}

	glClearDepthf(INFINITY);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR || err2 == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearDepthf_TC_004",
		    "Infinity depth degeri atanirken sürücü beklenmedik "
		    "sekilde davrandi."
		    " Actual: 0x%04X",
		    err2);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_004");
	}
}

/* ============================================================
 * glClearDepthf / glGetFloatv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_DEPTH_CLEAR_VALUE sorgusuna NULL isaretçi geçilerek çökme
 * dayanikliligi ve state bütünlügü dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearDepthf_TC_005(void) {
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, NULL);

	glClearDepthf(0.42f);

	GLfloat depth = -1.0f;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depth);

	if (!(depth == 0.42f)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearDepthf_TC_005",
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)depth);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearDepthf_TC_005");
	}
}
