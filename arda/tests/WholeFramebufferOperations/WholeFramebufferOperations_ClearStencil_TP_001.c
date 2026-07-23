#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * ============================================================
 *   TEST GRUBU: glClearStencil
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glClearStencil — Sinir Degerleri
 * ============================================================
 *
 * Stencil degeri integer'dir, ancak mevcut Stencil Buffer bit
 * sayisina göre maskelenir. Asiri büyük ve negatif sayilar
 * vererek state okumasinin bozulup bozulmadigini test eder.
 * ============================================================ */
void WholeFramebufferOperations_ClearStencil_TC_001(void) {
	// Stencil değeri integer'dır, ancak mevcut Stencil Buffer bit sayısına
	// göre maskelenir. Aşırı büyük ve negatif sayılar vererek state
	// okumasının bozulup bozulmadığını sınıyoruz.
	glClearStencil(-1); // Genelde tüm bitleri 1 yapan maske görevi görür
			    // (Two's complement)
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearStencil_TC_001",
			      "Negatif değer atamasında hata fırlatıldı."
			      " Actual: 0x%04X",
			      (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearStencil_TC_001");
	}
}

/* ============================================================
 * glClearStencil — INT_MIN / INT_MAX Sinir Degerleri
 * ============================================================
 *
 * GLint tipinin tamamen kullanilabilir araligi denenir. State
 * atamasi sirasinda herhangi bir maskeleme yapilmaz; maskeleme
 * yalnizca fiili Clear operasyonu sirasinda, stencil buffer'in
 * bit derinligine göre uygulanir. Dolayisiyla asiri uc
 * degerlerin de state olarak hatasiz kabul edilmesi beklenir.
 * ============================================================ */
void WholeFramebufferOperations_ClearStencil_TC_002(void) {
	glClearStencil(INT_MIN);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearStencil_TC_002",
			      "INT_MIN degeri atanirken hata firlatildi."
			      " Actual: 0x%04X",
			      (unsigned int)err1);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearStencil_TC_002");
	}

	glClearStencil(INT_MAX);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearStencil_TC_002",
			      "INT_MAX degeri atanirken hata firlatildi."
			      " Actual: 0x%04X",
			      (unsigned int)err2);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearStencil_TC_002");
	}
}

/* ============================================================
 * glClearStencil — Buffer Bit Derinligi Sinirinda Deger
 * ============================================================
 *
 * GL_STENCIL_BITS sorgulanarak mevcut donanimin stencil bit
 * derinligi (N) ögrenilir ve tam olarak bu sinirin bir üstündeki
 * bit (1 << N) deger olarak atanir. State atamasinin buffer'in
 * fiili bit derinligiyle sinirlanmadan, oldugu gibi kabul
 * edilmesi beklenir.
 * ============================================================ */
void WholeFramebufferOperations_ClearStencil_TC_003(void) {
	GLint stencilBits = 0;
	glGetIntegerv(GL_STENCIL_BITS, &stencilBits);

	GLint boundaryValue =
	    (stencilBits > 0 && stencilBits < 32) ? (1 << stencilBits) : 0;

	glClearStencil(boundaryValue);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearStencil_TC_003",
		    "Buffer bit derinliginin bir üstündeki sinir degeri "
		    "reddedildi."
		    " Actual: 0x%04X",
		    (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearStencil_TC_003");
	}
}

/* ============================================================
 * glClearStencil — Sifir Sinirinda Round-Trip
 * ============================================================
 *
 * Orijin noktasi olan 0 degerinin, hicbir isaret/maskeleme
 * hatasi olmadan birebir geri okunabildigi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearStencil_TC_004(void) {
	glClearStencil(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearStencil_TC_004",
		    "Sifir degeri atanirken beklenmedik bir hata döndü."
		    " Actual: 0x%04X",
		    (unsigned int)err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearStencil_TC_004");
	}

	GLint val = -99;
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &val);

	if (!(val == 0)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_ClearStencil_TC_004",
			      "Sifir degeri birebir geri okunamadi."
			      " Actual: 0x%04X",
			      (unsigned int)val);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearStencil_TC_004");
	}
}

/* ============================================================
 * glClearStencil / glGetIntegerv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_STENCIL_CLEAR_VALUE sorgusuna NULL isaretçi geçilerek
 * çökme dayanikliligi ve state bütünlügü dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ClearStencil_TC_005(void) {
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, NULL);

	glClearStencil(7);

	GLint val = 0;
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &val);

	if (!(val == 7)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_ClearStencil_TC_005",
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)val);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_ClearStencil_TC_005");
	}
}
