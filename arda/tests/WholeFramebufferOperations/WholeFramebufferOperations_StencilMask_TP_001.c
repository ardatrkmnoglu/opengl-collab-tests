#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "WholeFramebufferOperations_StencilMask_TP_001";
static const char* test_case_1 = "WholeFramebufferOperations_StencilMask_TC_001";
static const char* test_case_2 = "WholeFramebufferOperations_StencilMask_TC_002";
static const char* test_case_3 = "WholeFramebufferOperations_StencilMask_TC_003";
static const char* test_case_4 = "WholeFramebufferOperations_StencilMask_TC_004";


/* ============================================================
 * ============================================================
 *   TEST GRUBU: glStencilMask
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glStencilMask — Tam Deger Araligi Siniri (Full Range Boundary)
 * ============================================================
 *
 * GLuint bir maske parametresidir; gecerli stencil buffer bit
 * derinligiyle sinirli olmasi gerekmez, deger oldugu gibi state
 * olarak saklanmalidir. 0xFFFFFFFF ve 0x00000000 uc noktalari
 * hatasiz kabul edilmeli ve GLint olarak sirasiyla -1 ve 0
 * seklinde okunabilmelidir.
 * ============================================================ */
void WholeFramebufferOperations_StencilMask_TC_001(void) {
	glStencilMask(0xFFFFFFFF);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "Tüm bitleri 1 olan maske (0xFFFFFFFF) reddedildi."
		    " Actual: 0x%04X",
		    err1);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLint front = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);

	if (!(front == -1)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "0xFFFFFFFF, GLint olarak -1 seklinde okunmadi."
			      " Actual: 0x%04X",
			      (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glStencilMask(0x00000000);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Sifir maske (0x00000000) reddedildi."
			      " Actual: 0x%04X",
			      err2);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);

	if (!(front == 0)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "0x00000000 dogru okunmadi."
			      " Actual: 0x%04X",
			      (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glStencilMask — Ön/Arka Yüz Senkronizasyonu
 * ============================================================
 *
 * glStencilMask cagrisi hem GL_STENCIL_WRITEMASK'i hem de
 * GL_STENCIL_BACK_WRITEMASK'i AYNI ANDA günceller (spesifikasyona
 * göre glStencilMaskSeparate(GL_FRONT_AND_BACK, mask) ile
 * esdegerdir). Sürücünün yalnizca ön yüzü güncelleyip arka yüzü
 * unuttugu yaygin bir hata sinifini yakalar.
 * ============================================================ */
void WholeFramebufferOperations_StencilMask_TC_002(void) {
	glStencilMask(0xABCD1234);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Gecerli bir desen maskesi reddedildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLint front = 0, back = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back);

	GLint expected = (GLint)0xABCD1234u;

	if (!(front == expected && back == expected)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "glStencilMask hem ön hem arka yüz maskesini ayni anda "
		    "güncellemedi (senkronizasyon hatasi)."
		    " Actual: 0x%04X",
		    (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glStencilMask / glGetIntegerv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_STENCIL_WRITEMASK sorgusuna NULL isaretçi geçilerek çökme
 * dayanikliligi ve state bütünlügü dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_StencilMask_TC_003(void) {
	glGetIntegerv(GL_STENCIL_WRITEMASK, NULL);

	glStencilMask(0x12345678);

	GLint val = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &val);

	if (!(val == 0x12345678)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)val);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glStencilMask — Degisen Desenlerle Stres Testi
 * ============================================================
 *
 * Zikzak bit deseni (0x55555555 <-> 0xAAAAAAAA) yüzlerce kez
 * art arda uygulanir; hata birikmesi veya bit karismasi olup
 * olmadigi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_StencilMask_TC_004(void) {
	GLuint patterns[2] = {0x55555555, 0xAAAAAAAA};

	for (int i = 0; i < 512; i++) {
		glStencilMask(patterns[i % 2]);
	}

	GLenum err = glGetError();
	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Zikzak desen döngüsü hata birikimine yol acti."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	// Son iterasyon i=511 (tek) -> patterns[1] = 0xAAAAAAAA
	GLint val = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &val);
	GLint expected = (GLint)0xAAAAAAAAu;

	if (!(val == expected)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "Stres döngüsü sonrasi son state beklenenle uyusmuyor."
		    " Actual: 0x%04X",
		    (unsigned int)val);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}
