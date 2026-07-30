#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

/*
GL20SC - PerFragmentOperations - BlendFunc - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_BF_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_BF_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_BF_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_BF_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_BF_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_BF_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_BF_ROBUSTNESS_TC_007";

/* Forward declaration for close */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001_close(void);

/*
 * glBlendFunc icin gecerli sfactor ve dfactor degerleri.
 *
 * Ortak (hem sfactor hem dfactor icin gecerli):
 *   GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
 *   GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA,
 *   GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
 *   GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
 *   GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA
 *
 * Yalnizca sfactor icin gecerli:
 *   GL_SRC_ALPHA_SATURATE
 */

static const GLenum valid_common[] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA,
	/*
	 * Desktop GL (ARB_blend_func_extended / GL 3.3+)
	 * eklenti faktorleri. llvmpipe gibi desktop suruculer
	 * bunlari kabul eder.
	 */
	0x88F9, /* GL_SRC1_COLOR              */
	0x88FA, /* GL_ONE_MINUS_SRC1_COLOR    */
	0x88FB, /* GL_ONE_MINUS_SRC1_ALPHA    */
	/*
	 * Desktop GL 4.x'te asagidaki iki faktor de
	 * hem sfactor hem dfactor olarak gecerlidir.
	 * ES 2.0'da yalnizca sfactor idi.
	 */
	GL_SRC_ALPHA_SATURATE,		    /* 0x0308 */
	0x8589, /* GL_SRC1_ALPHA              */
};
static const int valid_common_count = sizeof(valid_common) / sizeof(valid_common[0]);

/* Desktop GL'de sfactor-only faktor kalmadi */
static const GLenum valid_sfactor_only[] = { 0 }; /* bos - sentinel */
static const int valid_sfactor_only_count = 0;

/* Bir enum degerinin sfactor icin gecerli olup olmadigini kontrol eder */
static int is_valid_sfactor(GLenum val) {
	int i;
	for (i = 0; i < valid_common_count; i++) {
		if (val == valid_common[i])
			return 1;
	}
	for (i = 0; i < valid_sfactor_only_count; i++) {
		if (val == valid_sfactor_only[i])
			return 1;
	}
	return 0;
}

/* Bir enum degerinin dfactor icin gecerli olup olmadigini kontrol eder */
static int is_valid_dfactor(GLenum val) {
	int i;
	for (i = 0; i < valid_common_count; i++) {
		if (val == valid_common[i])
			return 1;
	}
	return 0;
}

/* ============================================================
 * TEST 1 : Gecersiz Enum Degerleri (Spec Hatasi)
 * ============================================================
 *
 * Amac
 * ----
 * Spec'e gore sfactor veya dfactor parametresi gecerli
 * blend faktor enum degerlerinden biri olmalidir.
 *
 * Bunlarin disindaki degerler icin GL_INVALID_ENUM
 * uretilmelidir.
 *
 * Ayrica gecersiz cagri sonrasi state'in degismemesi
 * dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	/* Varsayilan state'i ayarla */
	glBlendFunc(GL_ONE, GL_ZERO);
	glGetError();

	/* Gecersiz sfactor, gecerli dfactor */
	glBlendFunc(0xDEAD, GL_ONE);
	GLenum err1 = glGetError();

	/* Gecerli sfactor, gecersiz dfactor */
	glBlendFunc(GL_ONE, 0xBEEF);
	GLenum err2 = glGetError();

	/* Ikisi de gecersiz */
	glBlendFunc(0x1234, 0x5678);
	GLenum err3 = glGetError();

	/* GL_SRC_ALPHA_SATURATE dfactor olarak gecersiz (yalnizca sfactor icin gecerli) */
	glBlendFunc(GL_ONE, GL_SRC_ALPHA_SATURATE);
	GLenum err4 = glGetError();

	/* State korunmasi kontrolu: hala GL_ONE / GL_ZERO olmali */
	GLint src_val, dst_val;
	glGetIntegerv(GL_BLEND_SRC_RGB, &src_val);
	glGetIntegerv(GL_BLEND_DST_RGB, &dst_val);
	int state_ok = (src_val == GL_ONE && dst_val == GL_ZERO);

	if (err1 == GL_INVALID_ENUM && err2 == GL_INVALID_ENUM &&
	    err3 == GL_INVALID_ENUM && err4 == GL_INVALID_ENUM && state_ok)
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	else
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Gecersiz enum reddedilmedi: err1=0x%X, "
			      "err2=0x%X, err3=0x%X, err4=0x%X, state_ok=%d",
			      err1, err2, err3, err4, state_ok);
}

/* ============================================================
 * TEST 2 : Gecerli Kombinasyonlar ile State Dogrulamasi
 * ============================================================
 *
 * Amac
 * ----
 * Tum gecerli sfactor x dfactor kombinasyonlari
 * ayarlanip GL_BLEND_SRC_RGB / GL_BLEND_DST_RGB
 * sorgusuyla durumun dogru kaydedildigini dogrulariz.
 * ============================================================ */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int failCount = 0;
	int i, j;

	/* Tum ortak sfactor x dfactor kombinasyonlari */
	for (i = 0; i < valid_common_count; i++) {
		for (j = 0; j < valid_common_count; j++) {
			glBlendFunc(valid_common[i], valid_common[j]);
			GLenum err = glGetError();

			if (err != GL_NO_ERROR) {
				TEST_LOG_FAIL(test_case_2, test_procedure,
					      "Gecerli kombo reddedildi: "
					      "sfactor=0x%X, dfactor=0x%X, "
					      "err=0x%X",
					      valid_common[i],
					      valid_common[j], err);
				failCount++;
				continue;
			}

			GLint src_val, dst_val;
			glGetIntegerv(GL_BLEND_SRC_RGB, &src_val);
			glGetIntegerv(GL_BLEND_DST_RGB, &dst_val);

			if ((GLenum)src_val != valid_common[i] ||
			    (GLenum)dst_val != valid_common[j]) {
				TEST_LOG_FAIL(
				    test_case_2, test_procedure,
				    "State uyusmazligi: sfactor=0x%X(0x%X), "
				    "dfactor=0x%X(0x%X)",
				    valid_common[i], src_val,
				    valid_common[j], dst_val);
				failCount++;
			}
		}
	}


	/* Varsayilana geri don */
	glBlendFunc(GL_ONE, GL_ZERO);

	if (failCount == 0)
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	else
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Toplam basarisiz kombinasyon: %d", failCount);
}

/* ============================================================
 * TEST 3 : Stress Sweep
 * ============================================================
 *
 * Amac
 * ----
 * 16-bit GLenum uzayindaki tum degerler sfactor
 * olarak sistematik bicimde denenir.
 * dfactor sabit (GL_ONE) tutulur.
 *
 * Yalnizca gecerli sfactor degerlerinin kabul
 * edilmesi beklenmektedir.
 *
 * Gecersiz degerlerde state'in korunup korunmadigi
 * da dogrulanmaktadir.
 * ============================================================ */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TC_003(void) {
	GLenum mode;
	GLint currentSrc = GL_ONE;
	int passCount = 0;
	int failCount = 0;

	while (glGetError() != GL_NO_ERROR)
		;

	/* Bilinen bir state ayarla */
	glBlendFunc(GL_ONE, GL_ONE);
	glGetError();

	for (mode = 0; mode < 65536; mode++) {
		GLenum expected = is_valid_sfactor(mode) ? GL_NO_ERROR
							 : GL_INVALID_ENUM;

		glBlendFunc(mode, GL_ONE);
		GLenum err = glGetError();

		if (err != expected) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "sfactor=0x%X Beklenen=0x%X Gelen=0x%X",
				      mode, expected, err);
			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_NO_ERROR) {
			currentSrc = (GLint)mode;
		}

		if (err == GL_INVALID_ENUM) {
			GLint actual;
			glGetIntegerv(GL_BLEND_SRC_RGB, &actual);
			if (actual != currentSrc) {
				TEST_LOG_FAIL(
				    test_case_3, test_procedure,
				    "State bozuldu: sfactor=0x%X "
				    "Beklenen_State=0x%X Gercek_State=0x%X",
				    mode, currentSrc, actual);
				return;
			}
		}
	}

	TEST_LOG_INFO("sfactor sweep - PASS: %d, FAIL: %d", passCount,
		      failCount);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Basarisiz cagri sayisi: %d", failCount);
		return;
	}

	/* Simdi dfactor icin ayni taramayi yap (sfactor sabit GL_ONE) */
	passCount = 0;
	failCount = 0;
	GLint currentDst = GL_ONE;

	glBlendFunc(GL_ONE, GL_ONE);
	glGetError();

	for (mode = 0; mode < 65536; mode++) {
		GLenum expected = is_valid_dfactor(mode) ? GL_NO_ERROR
							 : GL_INVALID_ENUM;

		glBlendFunc(GL_ONE, mode);
		GLenum err = glGetError();

		if (err != expected) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "dfactor=0x%X Beklenen=0x%X Gelen=0x%X",
				      mode, expected, err);
			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_NO_ERROR) {
			currentDst = (GLint)mode;
		}

		if (err == GL_INVALID_ENUM) {
			GLint actual;
			glGetIntegerv(GL_BLEND_DST_RGB, &actual);
			if (actual != currentDst) {
				TEST_LOG_FAIL(
				    test_case_3, test_procedure,
				    "State bozuldu: dfactor=0x%X "
				    "Beklenen_State=0x%X Gercek_State=0x%X",
				    mode, currentDst, actual);
				return;
			}
		}
	}

	TEST_LOG_INFO("dfactor sweep - PASS: %d, FAIL: %d", passCount,
		      failCount);

	/* Varsayilana geri don */
	glBlendFunc(GL_ONE, GL_ZERO);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "dfactor basarisiz cagri sayisi: %d", failCount);
		return;
	}

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : Asiri Buyuk ve Sinir Enum Degerleri
 * ============================================================
 *
 * Amac
 * ----
 * INT_MAX, INT_MIN, UINT_MAX gibi asiri degerlerle
 * cagirilarak surucunun cokup cokmedigini kontrol
 * ederiz.
 *
 * Hepsi GL_INVALID_ENUM uretmelidir.
 *
 * Ayrica basarisiz cagrilarin mevcut durumu
 * degistirmedigi kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum extreme_values[] = {(GLenum)INT_MAX, (GLenum)INT_MIN,
				   (GLenum)0xFFFFFFFF, (GLenum)0x7FFFFFFF,
				   (GLenum)0x80000000, (GLenum)0x10000};

	int count = sizeof(extreme_values) / sizeof(extreme_values[0]);

	/* Bilinen bir state ayarla */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGetError();

	for (int i = 0; i < count; i++) {
		/* Gecersiz sfactor */
		glBlendFunc(extreme_values[i], GL_ONE);
		GLenum err1 = glGetError();

		/* Gecersiz dfactor */
		glBlendFunc(GL_ONE, extreme_values[i]);
		GLenum err2 = glGetError();

		if (err1 != GL_INVALID_ENUM || err2 != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Enum=0x%X err_src=0x%X err_dst=0x%X",
				      extreme_values[i], err1, err2);
			return;
		}

		/* State korunmasi kontrolu */
		GLint src_val, dst_val;
		glGetIntegerv(GL_BLEND_SRC_RGB, &src_val);
		glGetIntegerv(GL_BLEND_DST_RGB, &dst_val);
		if (src_val != GL_SRC_ALPHA ||
		    dst_val != GL_ONE_MINUS_SRC_ALPHA) {
			TEST_LOG_FAIL(
			    test_case_4, test_procedure,
			    "State bozuldu: Enum=0x%X src=0x%X dst=0x%X",
			    extreme_values[i], src_val, dst_val);
			return;
		}
	}

	/* Varsayilana geri don */
	glBlendFunc(GL_ONE, GL_ZERO);

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Gecersiz Cagri Sonrasi State Korunmasi
 * ============================================================
 *
 * Amac
 * ----
 * Farkli gecerli sfactor/dfactor kombinasyonlari
 * ayarlandiktan sonra gecersiz degerler gonderildiginde
 * mevcut durumun degismemesi gerekir.
 *
 * Her gecerli kombinasyon icin birden fazla gecersiz
 * enum denenir ve her seferinde state kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	/* Test edilecek gecerli kombinasyonlar */
	struct {
		GLenum sfactor;
		GLenum dfactor;
	} valid_combos[] = {
		{GL_ONE, GL_ZERO},
		{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA},
		{GL_DST_COLOR, GL_CONSTANT_ALPHA},
		{GL_SRC_ALPHA_SATURATE, GL_ONE},
	};
	int combo_count = sizeof(valid_combos) / sizeof(valid_combos[0]);

	/* NOT: 0x0000 = GL_ZERO gecerli bir blend factor, listeye DAHIL ETME */
	GLenum invalidEnums[] = {0xDEAD, 0xBEEF, 0x1234, 0xFFFF, 0x9999};
	int invalid_count = sizeof(invalidEnums) / sizeof(invalidEnums[0]);

	for (int i = 0; i < combo_count; i++) {
		/* Gecerli kombinasyonu ayarla */
		glBlendFunc(valid_combos[i].sfactor, valid_combos[i].dfactor);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(
			    test_case_5, test_procedure,
			    "Gecerli kombo reddedildi: src=0x%X, dst=0x%X, "
			    "err=0x%X",
			    valid_combos[i].sfactor, valid_combos[i].dfactor,
			    err);
			return;
		}

		/* Her gecersiz enum ile test et */
		for (int j = 0; j < invalid_count; j++) {
			/* Gecersiz sfactor */
			glBlendFunc(invalidEnums[j], GL_ONE);
			err = glGetError();
			if (err != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "sfactor hatasi: invalid=0x%X, err=0x%X",
				    invalidEnums[j], err);
				return;
			}

			/* Gecersiz dfactor */
			glBlendFunc(GL_ONE, invalidEnums[j]);
			err = glGetError();
			if (err != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "dfactor hatasi: invalid=0x%X, err=0x%X",
				    invalidEnums[j], err);
				return;
			}

			/* State korunmasi kontrolu */
			GLint src_val, dst_val;
			glGetIntegerv(GL_BLEND_SRC_RGB, &src_val);
			glGetIntegerv(GL_BLEND_DST_RGB, &dst_val);
			if ((GLenum)src_val != valid_combos[i].sfactor ||
			    (GLenum)dst_val != valid_combos[i].dfactor) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "State bozuldu: beklenen=(%X,%X), "
				    "gercek=(%X,%X), invalid=0x%X",
				    valid_combos[i].sfactor,
				    valid_combos[i].dfactor, src_val,
				    dst_val, invalidEnums[j]);
				return;
			}
		}
	}

	/* Varsayilana geri don */
	glBlendFunc(GL_ONE, GL_ZERO);

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Enable/Disable State Strese Sokma (State Thrashing)
 * ============================================================
 *
 * Amac
 * ----
 * GL_BLEND ozelligini binlerce kez ard arda acip kapatarak
 * ve her seferinde farkli bir blend func ayarlayarak
 * surucunun durum (state) makinesinin bozulup bozulmadigini
 * test eder.
 *
 * Her cagri sonrasi GL_BLEND_SRC_RGB / GL_BLEND_DST_RGB
 * sorgulanir.
 * ============================================================ */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int err_count = 0;

	for (int i = 0; i < 10000; i++) {
		GLenum src = valid_common[i % valid_common_count];
		GLenum dst =
			valid_common[(i + 7) %
				     valid_common_count]; /* Farkli bir offset */

		glEnable(GL_BLEND);
		glBlendFunc(src, dst);

		GLint src_val, dst_val;
		glGetIntegerv(GL_BLEND_SRC_RGB, &src_val);
		glGetIntegerv(GL_BLEND_DST_RGB, &dst_val);

		if ((GLenum)src_val != src || (GLenum)dst_val != dst)
			err_count++;

		if (!glIsEnabled(GL_BLEND))
			err_count++;

		glDisable(GL_BLEND);
		if (glIsEnabled(GL_BLEND))
			err_count++;
	}

	GLenum err = glGetError();

	/* Varsayilana geri don */
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	if (err == GL_NO_ERROR && err_count == 0)
		TEST_LOG_SUCCESS(test_case_6, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case_6, test_procedure,
		    "State thrashing hatasi: err=0x%X, uyusmazlik=%d kez", err,
		    err_count);
}

/* ============================================================
 * TEST 7 : Deterministik Fuzzing (glBlendFunc Cagrilari)
 * ============================================================
 *
 * Amac
 * ----
 * Cok sayida deterministik (mantikli ve mantiksiz)
 * glBlendFunc cagirilarak bellek veya state bozulmasi
 * tespiti yapilir.
 *
 * Her ~5 iterasyonda gecerli bir kombinasyon, geri kalan
 * iterasyonlarda rastgele (buyuk olasilikla gecersiz)
 * enum degerleri gonderilir.
 *
 * Beklenen sonuclar:
 *
 *      GL_NO_ERROR
 *      GL_INVALID_ENUM
 *
 * Bunlarin disindaki herhangi bir hata basarisizlik
 * olarak degerlendirilir.
 * ============================================================ */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TC_007(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	/* Basit deterministik LCG (Linear Congruential Generator) */
	unsigned int seed = 54321;
	int unexpected_err_count = 0;
	GLenum last_unexpected_err = GL_NO_ERROR;
	int fail_iteration = -1;

	for (int i = 0; i < 50000; i++) {
		seed = seed * 1103515245 + 12345;
		GLenum sfactor;
		GLenum dfactor;

		if (i % 5 == 0) {
			/* Gecerli kombinasyon gonder */
			sfactor = valid_common[(seed >> 16) %
					       valid_common_count];
			seed = seed * 1103515245 + 12345;
			dfactor = valid_common[(seed >> 16) %
					       valid_common_count];
		} else {
			/* Rastgele (buyuk olasilikla gecersiz) enum degerleri */
			sfactor = (GLenum)((seed >> 16) % 0x10000);
			seed = seed * 1103515245 + 12345;
			dfactor = (GLenum)((seed >> 16) % 0x10000);
		}

		glBlendFunc(sfactor, dfactor);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			unexpected_err_count++;
			last_unexpected_err = err;
			if (fail_iteration < 0)
				fail_iteration = i;
		}
	}

	/* Dongu sonrasi GL context sagligini dogrula */
	while (glGetError() != GL_NO_ERROR)
		;
	glBlendFunc(GL_ONE, GL_ZERO);
	GLenum final_err = glGetError();

	/* State'in dogru dondugunu dogrula */
	GLint src_val, dst_val;
	glGetIntegerv(GL_BLEND_SRC_RGB, &src_val);
	glGetIntegerv(GL_BLEND_DST_RGB, &dst_val);
	int state_ok = (src_val == GL_ONE && dst_val == GL_ZERO);

	if (unexpected_err_count == 0 && final_err == GL_NO_ERROR && state_ok)
		TEST_LOG_SUCCESS(test_case_7, test_procedure);
	else
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Fuzzing hatasi: beklenmeyen_hata=%d kez, "
			      "ilk_hata_iter=%d, son_hata=0x%X, final=0x%X, "
			      "state_ok=%d",
			      unexpected_err_count, fail_iteration,
			      last_unexpected_err, final_err, state_ok);
}

/* Initialization */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001_init(void) {
	GS_GL20SC_PFO_BF_ROBUSTNESS_TC_001();
	GS_GL20SC_PFO_BF_ROBUSTNESS_TC_002();
	GS_GL20SC_PFO_BF_ROBUSTNESS_TC_003();
	GS_GL20SC_PFO_BF_ROBUSTNESS_TC_004();
	GS_GL20SC_PFO_BF_ROBUSTNESS_TC_005();
	GS_GL20SC_PFO_BF_ROBUSTNESS_TC_006();
	GS_GL20SC_PFO_BF_ROBUSTNESS_TC_007();

	GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_PFO_BF_ROBUSTNESS_TP_001_close(void) {
	/* Blend state'ini varsayilana sifirla */
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);
}
