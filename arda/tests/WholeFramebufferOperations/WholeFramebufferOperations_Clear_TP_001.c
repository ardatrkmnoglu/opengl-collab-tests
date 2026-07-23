#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * ============================================================
 *   TEST GRUBU: glClear
 * ============================================================
 * ============================================================ */

/* ============================================================
 * glClear — Geçersiz Bit Maskesi
 * ============================================================
 *
 * glClear yalnizca COLOR, DEPTH ve STENCIL bitlerinin
 * mantiksal OR kombinasyonunu kabul eder. Tüm bitleri '1'
 * yaparak (0xFFFFFFFF) ve kirletilmis bir maske göndererek
 * sürücünün bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void WholeFramebufferOperations_Clear_TC_001(void) {
	// glClear yalnızca COLOR, DEPTH ve STENCIL bitlerinin mantıksal OR
	// (Veya) kombinasyonunu kabul eder. Tüm bitleri '1' yaparak
	// (0xFFFFFFFF) sürücüye yasaklı bitler yolluyoruz.
	glClear(0xFFFFFFFF);
	GLenum err1 = glGetError();

	if (!(err1 == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations", "WholeFramebufferOperations_Clear_TC_001",
		    "glClear, tanımsız olan geçersiz maske bitlerini yuttu "
		    "(0xFFFFFFFF)."
		    " Actual: 0x%04X",
		    err1);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_001");
	}

	// İnce Suikast: Sadece bir tane geçersiz bit (örneğin 0x04) ekleyerek
	// kirletiyoruz.
	glClear(GL_COLOR_BUFFER_BIT | 0x00000004);
	GLenum err2 = glGetError();

	if (!(err2 == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations", "WholeFramebufferOperations_Clear_TC_001",
		    "Kirletilmiş mantıksal maske kombinasyonu reddedilmedi."
		    " Actual: 0x%04X",
		    err2);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_001");
	}
}

/* ============================================================
 * glClear — Sifir Bit Maskesi (No-Op Siniri)
 * ============================================================
 *
 * glClear(0) çagrisi, 'sifir veya daha fazla bitin OR
 * kombinasyonu' tanimina uyar ve hicbir buffer temizlenmeden
 * hatasiz dönmelidir. Bos küme, gecerli kümenin bir alt
 * kümesidir; bu sinir durumunun yanlislikla reddedilmedigini
 * dogrular.
 * ============================================================ */
void WholeFramebufferOperations_Clear_TC_002(void) {
	glClear(0);
	GLenum err = glGetError();

	if (!(err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_Clear_TC_002",
		    "Bos bit maskesi (0), gecersiz mantiksal kombinasyon "
		    "olarak yanlislikla reddedildi."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_002");
	}
}

/* ============================================================
 * glClear — Tek Tek Gecerli Bitlerin Kapsayici Testi
 * ============================================================
 *
 * GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT ve GL_STENCIL_BUFFER_BIT
 * tek basina, birbirinden bagimsiz olarak denenir. Framebuffer
 * yapilandirmasinda ilgili buffer bulunmasa bile spesifikasyona
 * göre hata degil sessiz no-op beklenir.
 * ============================================================ */
void WholeFramebufferOperations_Clear_TC_003(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_Clear_TC_003",
			      "Tek basina GL_COLOR_BUFFER_BIT reddedildi."
			      " Actual: 0x%04X",
			      err1);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_003");
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_Clear_TC_003",
			      "Tek basina GL_DEPTH_BUFFER_BIT reddedildi."
			      " Actual: 0x%04X",
			      err2);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_003");
	}

	glClear(GL_STENCIL_BUFFER_BIT);
	GLenum err3 = glGetError();

	if (!(err3 == GL_NO_ERROR)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_Clear_TC_003",
			      "Tek basina GL_STENCIL_BUFFER_BIT reddedildi."
			      " Actual: 0x%04X",
			      err3);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_003");
	}
}

/* ============================================================
 * glClear — En Yüksek Bit Sentinel Degeri (MSB)
 * ============================================================
 *
 * 0x80000000, GLbitfield isaretsiz bir tam sayi olmasina ragmen,
 * isaretli (signed) tam sayiya çevrildiginde negatif olur. Bu,
 * sürücünün bitfield'i yanlislikla 'int' olarak yorumlayip
 * isaret genisletme (sign extension) hatasi yapip yapmadigini
 * sinar.
 * ============================================================ */
void WholeFramebufferOperations_Clear_TC_004(void) {
	glClear(0x80000000);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_Clear_TC_004",
			      "En yüksek bit (MSB) sentinel degeri gecerli bir "
			      "kombinasyon olmamasina ragmen kabul edildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_004");
	}
}

/* ============================================================
 * glClear — Gecersiz Cagri Sonrasi Hata Durumunun Yapiskan
 * Olmamasi (Error State Not Sticky)
 * ============================================================
 *
 * Önce gecersiz bir maske ile hata üretilir, ardindan hemen
 * gecerli bir glClear cagrisi yapilir. Ilk hatanin ikinci,
 * tamamen gecerli olan cagriyi da 'kirletmedigini' (sürekli
 * hata döndürmedigini) dogrulamak, hata durumunun dogru
 * yönetildigini gösterir.
 * ============================================================ */
void WholeFramebufferOperations_Clear_TC_005(void) {
	glClear(0xFFFFFFFF);
	GLenum err1 = glGetError();

	if (!(err1 == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("WholeFramebufferOperations",
			      "WholeFramebufferOperations_Clear_TC_005",
			      "Ilk (kasitli gecersiz) cagri beklenen hatayi "
			      "üretmedi."
			      " Actual: 0x%04X",
			      err1);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_005");
	}

	glClear(GL_COLOR_BUFFER_BIT);
	GLenum err2 = glGetError();

	if (!(err2 == GL_NO_ERROR)) {
		TEST_LOG_FAIL(
		    "WholeFramebufferOperations",
		    "WholeFramebufferOperations_Clear_TC_005",
		    "Önceki gecersiz cagridan kalan hata durumu, sonraki "
		    "gecerli cagriyi da etkiledi (yapiskan hata)."
		    " Actual: 0x%04X",
		    err2);
	} else {
		TEST_LOG_SUCCESS("WholeFramebufferOperations",
				 "WholeFramebufferOperations_Clear_TC_005");
	}
}
