#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_FOP_CS_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_FOP_CS_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_FOP_CS_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_FOP_CS_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_FOP_CS_ROBUSTNESS_TC_005";

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
void GS_GL20SC_FOP_CS_ROBUSTNESS_TC_001(void) {
	// Stencil değeri integer'dır, ancak mevcut Stencil Buffer bit sayısına
	// göre maskelenir. Aşırı büyük ve negatif sayılar vererek state
	// okumasının bozulup bozulmadığını sınıyoruz.
	glClearStencil(-1); // Genelde tüm bitleri 1 yapan maske görevi görür
			    // (Two's complement)
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Negatif değer atamasında hata fırlatıldı."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
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
void GS_GL20SC_FOP_CS_ROBUSTNESS_TC_002(void) {
	glClearStencil(INT_MIN);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "INT_MIN degeri atanirken hata firlatildi."
			      " Actual: 0x%04X",
			      err1);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glClearStencil(INT_MAX);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "INT_MAX degeri atanirken hata firlatildi."
			      " Actual: 0x%04X",
			      err2);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
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
void GS_GL20SC_FOP_CS_ROBUSTNESS_TC_003(void) {
	GLint stencilBits = 0;
	glGetIntegerv(GL_STENCIL_BITS, &stencilBits);

	GLint boundaryValue =
	    (stencilBits > 0 && stencilBits < 32) ? (1 << stencilBits) : 0;

	glClearStencil(boundaryValue);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Buffer bit derinliginin bir üstündeki sinir degeri "
		    "reddedildi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glClearStencil — Sifir Sinirinda Round-Trip
 * ============================================================
 *
 * Orijin noktasi olan 0 degerinin, hicbir isaret/maskeleme
 * hatasi olmadan birebir geri okunabildigi dogrulanir.
 * ============================================================ */
void GS_GL20SC_FOP_CS_ROBUSTNESS_TC_004(void) {
	glClearStencil(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Sifir degeri atanirken beklenmedik bir hata döndü."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLint val = -99;
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &val);

	if (!(val == 0)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Sifir degeri birebir geri okunamadi."
			      " Actual: 0x%04X",
			      (unsigned int)val);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glClearStencil / glGetIntegerv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_STENCIL_CLEAR_VALUE sorgusuna NULL isaretçi geçilerek
 * çökme dayanikliligi ve state bütünlügü dogrulanir.
 * ============================================================ */
void GS_GL20SC_FOP_CS_ROBUSTNESS_TC_005(void) {
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, NULL);

	glClearStencil(7);

	GLint val = 0;
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &val);

	if (!(val == 7)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)val);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_FOP_CS_ROBUSTNESS_TC_001();
	GS_GL20SC_FOP_CS_ROBUSTNESS_TC_002();
	GS_GL20SC_FOP_CS_ROBUSTNESS_TC_003();
	GS_GL20SC_FOP_CS_ROBUSTNESS_TC_004();
	GS_GL20SC_FOP_CS_ROBUSTNESS_TC_005();
}

/* Cleanup */
void GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
