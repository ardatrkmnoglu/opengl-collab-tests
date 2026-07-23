#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * ============================================================
 *   TEST GRUBU: glStencilMaskSeparate
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glStencilMaskSeparate — Geçersiz Enum
 * ============================================================
 *
 * 'face' parametresi yalnizca GL_FRONT, GL_BACK veya
 * GL_FRONT_AND_BACK olabilir. GL_TEXTURE_2D verilerek
 * sürücünün bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_StencilMaskSeparate_TC_001(void) {
	// 'face' parametresi YALNIZCA GL_FRONT, GL_BACK veya GL_FRONT_AND_BACK
	// olabilir.
	glStencilMaskSeparate(GL_TEXTURE_2D, 0xFFFFFFFF);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_001",
		    "Geçersiz face parametresi (GL_TEXTURE_2D) reddedilmedi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_StencilMaskSeparate_TC_001");
	}
}

/* ============================================================
 * glStencilMaskSeparate — GL_FRONT_AND_BACK Sinir Degeri
 * ============================================================
 *
 * GL_FRONT_AND_BACK, gecerli enum kümesinin bir üyesidir ve
 * hem ön hem arka yüz maskesini ayni degere ayarlamalidir.
 * Bu, 'gecersiz enum' testinin tam tersi sinir durumudur:
 * gecerli en genis kapsamli deger.
 * ============================================================ */
void WholeFramebufferOperations_StencilMaskSeparate_TC_002(void) {
	glStencilMaskSeparate(GL_FRONT_AND_BACK, 0x0F0F0F0F);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_002",
		    "Gecerli GL_FRONT_AND_BACK degeri reddedildi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_002");
	}

	GLint front = 0, back = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back);

	if (!(front == 0x0F0F0F0F && back == 0x0F0F0F0F)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_002",
		    "GL_FRONT_AND_BACK her iki yüzü de ayni degere "
		    "ayarlamadi."
		    " Actual: 0x%04X",
		    (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_002");
	}
}

/* ============================================================
 * glStencilMaskSeparate — Yüzler Arasi Izolasyon
 * ============================================================
 *
 * Ön yüze bir deger, arka yüze baska bir deger ayri ayri
 * atanir. Bir yüze yapilan atamanin digerini 'ezmedigini'
 * dogrulamak, sik görülen bir sürücü hatasini yakalar.
 * ============================================================ */
void WholeFramebufferOperations_StencilMaskSeparate_TC_003(void) {
	glStencilMaskSeparate(GL_FRONT, 0xFFFFFFFF);
	glStencilMaskSeparate(GL_BACK, 0x00000000);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_003",
		    "Ayri yüz atamalari sirasinda beklenmedik hata olustu."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_003");
	}

	GLint front = 0, back = -1;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back);

	if (!(front == -1 && back == 0)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_003",
		    "GL_BACK atamasi GL_FRONT degerini ezmis olabilir "
		    "(ya da tam tersi)."
		    " Actual: 0x%04X",
		    (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_003");
	}
}

/* ============================================================
 * glStencilMaskSeparate — Maske Sentinel Sinir Degeri
 * ============================================================
 *
 * En yüksek bitin (0x80000000) set edildigi bir maske gönderilir.
 * Isaretli/isaretsiz tamsayi yorumlama hatalarini (sign
 * extension buglarini) yakalamayi hedefler.
 * ============================================================ */
void WholeFramebufferOperations_StencilMaskSeparate_TC_004(void) {
	glStencilMaskSeparate(GL_FRONT, 0x80000000);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_StencilMaskSeparate_TC_004",
			      "En yüksek bit set edilmis gecerli bir maske "
			      "reddedildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_004");
	}

	GLint front = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);
	GLint expected = (GLint)0x80000000u;

	if (!(front == expected)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_StencilMaskSeparate_TC_004",
			      "0x80000000 degeri isaretli/isaretsiz yorumlama "
			      "hatasiyla bozulmus olabilir."
			      " Actual: 0x%04X",
			      (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_004");
	}
}

/* ============================================================
 * glStencilMaskSeparate / glGetIntegerv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_STENCIL_BACK_WRITEMASK sorgusuna NULL isaretçi geçilerek
 * çökme dayanikliligi ve state bütünlügü dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_StencilMaskSeparate_TC_005(void) {
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, NULL);

	glStencilMaskSeparate(GL_BACK, 0x2222);

	GLint val = 0;
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &val);

	if (!(val == 0x2222)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_005",
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu."
		    " Actual: 0x%04X",
		    (unsigned int)val);
	} else {
		TEST_LOG_SUCCESS(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_StencilMaskSeparate_TC_005");
	}
}
