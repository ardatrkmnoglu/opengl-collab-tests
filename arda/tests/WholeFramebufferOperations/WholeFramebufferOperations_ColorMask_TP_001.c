#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char* test_procedure = "WholeFramebufferOperations_ColorMask_TP_001";
static const char* test_case_1 = "WholeFramebufferOperations_ColorMask_TC_001";
static const char* test_case_2 = "WholeFramebufferOperations_ColorMask_TC_002";
static const char* test_case_3 = "WholeFramebufferOperations_ColorMask_TC_003";
static const char* test_case_4 = "WholeFramebufferOperations_ColorMask_TC_004";
static const char* test_case_5 = "WholeFramebufferOperations_ColorMask_TC_005";


/* ============================================================
 * ============================================================
 *   TEST GRUBU: glColorMask
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glColorMask — Boolean Dönüsüm
 * ============================================================
 *
 * Kasitli olarak 1 ve 0 yerine 'tuhaf' sayilar (0xFF, 0x02,
 * 0x80) gönderilerek OpenGL kuralina göre 0 disindaki her
 * seyin GL_TRUE kabul edilmesi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ColorMask_TC_001(void) {
	glColorMask(0xFF, 0x02, 0x00, 0x80);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "Standart dışı değerler girildiğinde hata üretildi "
		    "(Spesifikasyona aykırı)."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	// Sürücü gerçekten bu değerleri GL_TRUE ve GL_FALSE olarak kırptı mı?
	GLboolean mask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);

	if (!(mask[0] == GL_TRUE && mask[1] == GL_TRUE && mask[2] == GL_FALSE &&
	      mask[3] == GL_TRUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "Sürücü '!= 0' kuralını ihlal etti veya değerleri "
		    "doğru cast etmedi."
		    " Actual: 0x%04X",
		    (unsigned int)mask[0]);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glColorMask — Bayt Sinirinda Tasma (Byte Boundary Wraparound)
 * ============================================================
 *
 * GLboolean tipi tek bayttir (0-255). 0x100 gibi bir deger
 * bayt sinirini asip 0x00'a sarmalidir (tasma). 0x101 ise
 * 0x01'e sarmali ve GL_TRUE üretmelidir. Programcinin yanlislikla
 * büyük bir sayi gönderip 'dogal olarak true olur' varsaymasi
 * senaryosunu simüle eder.
 * ============================================================ */
void WholeFramebufferOperations_ColorMask_TC_002(void) {
	// 0x100 -> alt bayt 0x00 (FALSE), 0x101 -> alt bayt 0x01 (TRUE)
	// 0xFF00 -> alt bayt 0x00 (FALSE), 0xFF01 -> alt bayt 0x01 (TRUE)
	glColorMask((GLboolean)0x100, (GLboolean)0x101, (GLboolean)0xFF00,
		    (GLboolean)0xFF01);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Bayt sinirindaki tasma degerleri hata ürettirdi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	GLboolean mask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);

	if (!(mask[0] == GL_FALSE && mask[1] == GL_TRUE &&
	      mask[2] == GL_FALSE && mask[3] == GL_TRUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "Sürücü bayt sinirindaki tasmayi dogru yorumlamadi "
		    "(0x100 -> FALSE, 0x101 -> TRUE olmali)."
		    " Actual: 0x%04X",
		    (unsigned int)mask[0]);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glColorMask / glGetBooleanv — NULL Pointer Dayanikliligi
 * ============================================================
 *
 * glGetBooleanv'e NULL çikti isaretçisi geçilir. Kaliteli bir
 * sürücü çökmemeli ve dahili state'i bozmamalidir. Ardindan
 * gecerli bir sorgu ile state'in hala tutarli oldugu dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_ColorMask_TC_003(void) {
	// NULL isaretçi ile çagri - çökme (crash) testi. State bozulmamali.
	glGetBooleanv(GL_COLOR_WRITEMASK, NULL);

	// Sürücü hayattaysa, normal bir çagri ile devam edebilmeli.
	glColorMask(GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE);

	GLboolean mask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);

	if (!(mask[0] == GL_TRUE && mask[1] == GL_FALSE && mask[2] == GL_TRUE &&
	      mask[3] == GL_FALSE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "NULL isaretçi sorgusu sonrasi sürücü state'i bozuldu "
		    "veya tutarsiz hale geldi."
		    " Actual: 0x%04X",
		    (unsigned int)mask[0]);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glColorMask — Kanal Sirasi Bütünlügü (Channel Order Integrity)
 * ============================================================
 *
 * R/G/B/A kanallarinin birbirine karismadigini (ör. R ile A'nin
 * yer degistirmedigini) dogrulamak için yalnizca tek bir kanal
 * TRUE yapilip digerlerinin FALSE kaldigi sinanir.
 * ============================================================ */
void WholeFramebufferOperations_ColorMask_TC_004(void) {
	// Sadece Red kanali acik
	glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
	GLboolean mask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);

	if (!(mask[0] == GL_TRUE && mask[1] == GL_FALSE &&
	      mask[2] == GL_FALSE && mask[3] == GL_FALSE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Yalnizca R kanali TRUE olmali, digerleri FALSE "
			      "kalmaliydi (kanal karismasi olabilir)."
			      " Actual: 0x%04X",
			      (unsigned int)mask[0]);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	// Sadece Alpha kanali acik
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);

	if (!(mask[0] == GL_FALSE && mask[1] == GL_FALSE &&
	      mask[2] == GL_FALSE && mask[3] == GL_TRUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Yalnizca A kanali TRUE olmali, digerleri FALSE "
			      "kalmaliydi (R/A karismasi supheli)."
			      " Actual: 0x%04X",
			      (unsigned int)mask[3]);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glColorMask — Tekrarli Asiri Deger Stresi (Stress Test)
 * ============================================================
 *
 * Binlerce kez art arda, uc noktalar arasinda (0x00 <-> 0xFF)
 * gidip gelen çagrilar yapilir. Amaç: hata durumunun
 * biriktigi (sticky error) veya state'in araya sikistigi bir
 * durum olusup olusmadigini yakalamak.
 * ============================================================ */
void WholeFramebufferOperations_ColorMask_TC_005(void) {
	for (int i = 0; i < 1024; i++) {
		GLboolean v = (i % 2 == 0) ? 0xFF : 0x00;
		glColorMask(v, v, v, v);
	}

	GLenum err = glGetError();
	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
			      "Tekrarli uc deger degisimleri sonrasinda hata "
			      "durumu biriktirilmis olabilir."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

	// Son iterasyon (i=1023, tek) 0x00 yollamis olmali -> hepsi FALSE
	GLboolean mask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);

	if (!(mask[0] == GL_FALSE && mask[1] == GL_FALSE &&
	      mask[2] == GL_FALSE && mask[3] == GL_FALSE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure, 
		    "Stres döngüsü sonrasi son state beklenenle uyusmuyor."
		    " Actual: 0x%04X",
		    (unsigned int)mask[0]);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}
