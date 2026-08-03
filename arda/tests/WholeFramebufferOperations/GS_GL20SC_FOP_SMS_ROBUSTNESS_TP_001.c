#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_005";

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
void GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_001(void) {
	// 'face' parametresi YALNIZCA GL_FRONT, GL_BACK veya GL_FRONT_AND_BACK
	// olabilir.
	glStencilMaskSeparate(GL_TEXTURE_2D, 0xFFFFFFFF);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Geçersiz face parametresi (GL_TEXTURE_2D) reddedilmedi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
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
void GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_002(void) {
	glStencilMaskSeparate(GL_FRONT_AND_BACK, 0x0F0F0F0F);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Gecerli GL_FRONT_AND_BACK degeri reddedildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLint front = 0, back = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back);

	if (!(front == 0x0F0F0F0F && back == 0x0F0F0F0F)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "GL_FRONT_AND_BACK her iki yüzü de ayni degere "
			      "ayarlamadi."
			      " Actual: 0x%04X",
			      (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
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
void GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_003(void) {
	glStencilMaskSeparate(GL_FRONT, 0xFFFFFFFF);
	glStencilMaskSeparate(GL_BACK, 0x00000000);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Ayri yüz atamalari sirasinda beklenmedik hata olustu."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLint front = 0, back = -1;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back);

	if (!(front == -1 && back == 0)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "GL_BACK atamasi GL_FRONT degerini ezmis olabilir "
		    "(ya da tam tersi)."
		    " Actual: 0x%04X",
		    (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
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
void GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_004(void) {
	glStencilMaskSeparate(GL_FRONT, 0x80000000);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "En yüksek bit set edilmis gecerli bir maske "
			      "reddedildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLint front = 0;
	glGetIntegerv(GL_STENCIL_WRITEMASK, &front);
	GLint expected = (GLint)0x80000000u;

	if (!(front == expected)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "0x80000000 degeri isaretli/isaretsiz yorumlama "
			      "hatasiyla bozulmus olabilir."
			      " Actual: 0x%04X",
			      (unsigned int)front);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glStencilMaskSeparate / glGetIntegerv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * GL_STENCIL_BACK_WRITEMASK sorgusuna NULL isaretçi geçilerek
 * çökme dayanikliligi ve state bütünlügü dogrulanir.
 * ============================================================ */
void GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_005(void) {
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, NULL);

	glStencilMaskSeparate(GL_BACK, 0x2222);

	GLint val = 0;
	glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &val);

	if (!(val == 0x2222)) {
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
void GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_001();
	GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_002();
	GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_003();
	GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_004();
	GS_GL20SC_FOP_SMS_ROBUSTNESS_TC_005();
}

/* Cleanup */
void GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
