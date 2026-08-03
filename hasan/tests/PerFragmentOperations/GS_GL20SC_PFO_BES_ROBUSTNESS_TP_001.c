#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

/*
GL20SC - PerFragmentOperations - BlendEquationSeparate - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_BES_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_BES_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_BES_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_BES_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_BES_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_BES_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_BES_ROBUSTNESS_TC_007";

/* Forward declaration for close */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001_close(void);

/*
 * glBlendEquationSeparate icin gecerli mod degerleri.
 *
 *   GL_FUNC_ADD              (0x8006)
 *   GL_FUNC_SUBTRACT         (0x800A)
 *   GL_FUNC_REVERSE_SUBTRACT (0x800B)
 */
static const GLenum valid_modes[] = {
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	/* Desktop GL (EXT_blend_minmax / GL 1.4+) */
	0x8007, /* GL_MIN */
	0x8008, /* GL_MAX */
};
static const int valid_mode_count =
	sizeof(valid_modes) / sizeof(valid_modes[0]);

/* Gecerli mod mu kontrol eder */
static int is_valid_mode(GLenum val) {
	for (int i = 0; i < valid_mode_count; i++) {
		if (val == valid_modes[i])
			return 1;
	}
	return 0;
}

/* 2'li state sorgulama yardimcisi */
static int check_eq_state(GLenum exp_rgb, GLenum exp_alpha) {
	GLint rgb, alpha;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &rgb);
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &alpha);
	return ((GLenum)rgb == exp_rgb && (GLenum)alpha == exp_alpha);
}

/* ============================================================
 * TEST 1 : Gecersiz Enum Degerleri (Spec Hatasi)
 * ============================================================
 *
 * Amac
 * ----
 * modeRGB veya modeAlpha parametresinden herhangi biri
 * gecersiz oldugunda GL_INVALID_ENUM uretilmelidir.
 *
 * Her parametre pozisyonu ayri ayri test edilir.
 * Gecersiz cagri sonrasi state korunmasi dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	/* Varsayilan state'i ayarla */
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glGetError();

	/* Gecersiz modeRGB */
	glBlendEquationSeparate(0xDEAD, GL_FUNC_ADD);
	GLenum err1 = glGetError();

	/* Gecersiz modeAlpha */
	glBlendEquationSeparate(GL_FUNC_ADD, 0xDEAD);
	GLenum err2 = glGetError();

	/* Ikisi de gecersiz */
	glBlendEquationSeparate(0xBEEF, 0xCAFE);
	GLenum err3 = glGetError();

	/* Gecerli ama BlendEquation modlari arasinda olmayan enum */
	glBlendEquationSeparate(GL_BLEND, GL_FUNC_ADD);
	GLenum err4 = glGetError();

	glBlendEquationSeparate(GL_FUNC_ADD, GL_BLEND);
	GLenum err5 = glGetError();

	/* State korunmasi: hala (GL_FUNC_ADD, GL_FUNC_ADD) olmali */
	int state_ok = check_eq_state(GL_FUNC_ADD, GL_FUNC_ADD);

	if (err1 == GL_INVALID_ENUM && err2 == GL_INVALID_ENUM &&
	    err3 == GL_INVALID_ENUM && err4 == GL_INVALID_ENUM &&
	    err5 == GL_INVALID_ENUM && state_ok)
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	else
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "err1=0x%X, err2=0x%X, err3=0x%X, "
			      "err4=0x%X, err5=0x%X, state_ok=%d",
			      err1, err2, err3, err4, err5, state_ok);
}

/* ============================================================
 * TEST 2 : RGB ve Alpha Kanallarinin Bagimsiz Ayarlanmasi
 * ============================================================
 *
 * Amac
 * ----
 * RGB ve Alpha icin farkli modlar ayarlandiginda
 * her birinin dogru kaydedildigini dogrular.
 *
 * Tum gecerli modeRGB x modeAlpha kombinasyonlari
 * test edilir (3x3 = 9 kombinasyon).
 * ============================================================ */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int failCount = 0;

	for (int i = 0; i < valid_mode_count; i++) {
		for (int j = 0; j < valid_mode_count; j++) {
			glBlendEquationSeparate(valid_modes[i], valid_modes[j]);
			GLenum err = glGetError();

			if (err != GL_NO_ERROR) {
				TEST_LOG_FAIL(
				    test_case_2, test_procedure,
				    "Kombo reddedildi: rgb=0x%X, alpha=0x%X, "
				    "err=0x%X",
				    valid_modes[i], valid_modes[j], err);
				failCount++;
				continue;
			}

			if (!check_eq_state(valid_modes[i], valid_modes[j])) {
				GLint rgb, alpha;
				glGetIntegerv(GL_BLEND_EQUATION_RGB, &rgb);
				glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &alpha);
				TEST_LOG_FAIL(
				    test_case_2, test_procedure,
				    "State hatasi: beklenen=(%X,%X), "
				    "gercek=(%X,%X)",
				    valid_modes[i], valid_modes[j], rgb, alpha);
				failCount++;
			}
		}
	}

	/* Varsayilana geri don */
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

	if (failCount == 0)
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	else
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Basarisiz kombinasyon: %d", failCount);
}

/* ============================================================
 * TEST 3 : Stress Sweep (modeRGB parametresi)
 * ============================================================
 *
 * Amac
 * ----
 * 16-bit GLenum uzayindaki tum degerler modeRGB
 * olarak sistematik bicimde denenir.
 * modeAlpha sabit (GL_FUNC_ADD) tutulur.
 *
 * Yalnizca GL_FUNC_ADD, GL_FUNC_SUBTRACT,
 * GL_FUNC_REVERSE_SUBTRACT kabul edilmelidir.
 *
 * Gecersiz degerlerde state korunmasi dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TC_003(void) {
	GLenum mode;
	int passCount = 0;
	int failCount = 0;

	while (glGetError() != GL_NO_ERROR)
		;

	/* Bilinen bir state ayarla */
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glGetError();

	GLint lastRGB = GL_FUNC_ADD;

	for (mode = 0; mode < 65536; mode++) {
		GLenum expected = is_valid_mode(mode) ? GL_NO_ERROR
						      : GL_INVALID_ENUM;

		glBlendEquationSeparate(mode, GL_FUNC_ADD);
		GLenum err = glGetError();

		if (err != expected) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "modeRGB=0x%X Beklenen=0x%X Gelen=0x%X",
				      mode, expected, err);
			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_NO_ERROR) {
			lastRGB = (GLint)mode;
		}

		if (err == GL_INVALID_ENUM) {
			GLint actual;
			glGetIntegerv(GL_BLEND_EQUATION_RGB, &actual);
			if (actual != lastRGB) {
				TEST_LOG_FAIL(
				    test_case_3, test_procedure,
				    "State bozuldu: modeRGB=0x%X "
				    "Beklenen=0x%X Gercek=0x%X",
				    mode, lastRGB, actual);
				return;
			}
		}
	}

	TEST_LOG_INFO("modeRGB sweep - PASS: %d, FAIL: %d", passCount,
		      failCount);

	/* Varsayilana geri don */
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Basarisiz cagri sayisi: %d", failCount);
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
 * INT_MAX, INT_MIN, UINT_MAX gibi asiri degerler
 * her iki parametre pozisyonunda denenerek
 * surucunun cokup cokmedigini kontrol ederiz.
 * ============================================================ */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum extreme[] = {(GLenum)INT_MAX, (GLenum)INT_MIN,
			    (GLenum)0xFFFFFFFF, (GLenum)0x80000000,
			    (GLenum)0x10000};
	int count = sizeof(extreme) / sizeof(extreme[0]);

	/* Bilinen bir state ayarla */
	glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT);
	glGetError();

	for (int i = 0; i < count; i++) {
		/* Gecersiz modeRGB */
		glBlendEquationSeparate(extreme[i], GL_FUNC_ADD);
		GLenum e1 = glGetError();

		/* Gecersiz modeAlpha */
		glBlendEquationSeparate(GL_FUNC_ADD, extreme[i]);
		GLenum e2 = glGetError();

		if (e1 != GL_INVALID_ENUM || e2 != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Enum=0x%X e1=0x%X e2=0x%X",
				      extreme[i], e1, e2);
			return;
		}

		/* State korunmasi */
		if (!check_eq_state(GL_FUNC_SUBTRACT,
				    GL_FUNC_REVERSE_SUBTRACT)) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "State bozuldu: Enum=0x%X", extreme[i]);
			return;
		}
	}

	/* Varsayilana geri don */
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Gecersiz Cagri Sonrasi State Korunmasi
 * ============================================================
 *
 * Amac
 * ----
 * Her gecerli modeRGB x modeAlpha kombinasyonu
 * ayarlandiktan sonra gecersiz degerler (her iki
 * pozisyonda) gonderildiginde state'in degismemesi
 * gerekir.
 * ============================================================ */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum invalidEnums[] = {0xDEAD, 0xBEEF, 0x1234, 0xFFFF, 0x9999};
	int inv_count = sizeof(invalidEnums) / sizeof(invalidEnums[0]);

	for (int i = 0; i < valid_mode_count; i++) {
		for (int j = 0; j < valid_mode_count; j++) {
			/* Gecerli kombinasyonu ayarla */
			glBlendEquationSeparate(valid_modes[i], valid_modes[j]);
			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "Kombo reddedildi: rgb=0x%X alpha=0x%X "
				    "err=0x%X",
				    valid_modes[i], valid_modes[j], err);
				return;
			}

			/* Her gecersiz enum ile test et */
			for (int k = 0; k < inv_count; k++) {
				/* Gecersiz modeRGB */
				glBlendEquationSeparate(invalidEnums[k],
							GL_FUNC_ADD);
				if (glGetError() != GL_INVALID_ENUM) {
					TEST_LOG_FAIL(
					    test_case_5, test_procedure,
					    "modeRGB hatasi: inv=0x%X",
					    invalidEnums[k]);
					return;
				}

				/* Gecersiz modeAlpha */
				glBlendEquationSeparate(GL_FUNC_ADD,
							invalidEnums[k]);
				if (glGetError() != GL_INVALID_ENUM) {
					TEST_LOG_FAIL(
					    test_case_5, test_procedure,
					    "modeAlpha hatasi: inv=0x%X",
					    invalidEnums[k]);
					return;
				}

				/* State korunmasi */
				if (!check_eq_state(valid_modes[i],
						    valid_modes[j])) {
					TEST_LOG_FAIL(
					    test_case_5, test_procedure,
					    "State bozuldu: rgb=0x%X "
					    "alpha=0x%X inv=0x%X",
					    valid_modes[i], valid_modes[j],
					    invalidEnums[k]);
					return;
				}
			}
		}
	}

	/* Varsayilana geri don */
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Enable/Disable State Strese Sokma (State Thrashing)
 * ============================================================
 *
 * Amac
 * ----
 * GL_BLEND ozelligini binlerce kez acip kapatarak
 * ve her seferinde farkli bir blend equation separate
 * ayarlayarak state makinesinin bozulup bozulmadigini
 * test eder.
 * ============================================================ */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int err_count = 0;

	for (int i = 0; i < 10000; i++) {
		GLenum modeRGB = valid_modes[i % valid_mode_count];
		GLenum modeAlpha =
			valid_modes[(i + 1) % valid_mode_count];

		glEnable(GL_BLEND);
		glBlendEquationSeparate(modeRGB, modeAlpha);

		if (!check_eq_state(modeRGB, modeAlpha))
			err_count++;

		if (!glIsEnabled(GL_BLEND))
			err_count++;

		glDisable(GL_BLEND);
		if (glIsEnabled(GL_BLEND))
			err_count++;
	}

	GLenum err = glGetError();

	/* Varsayilana geri don */
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
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
 * TEST 7 : Deterministik Fuzzing
 * ============================================================
 *
 * Amac
 * ----
 * Cok sayida deterministik (mantikli ve mantiksiz)
 * glBlendEquationSeparate cagirilarak bellek veya state
 * bozulmasi tespiti yapilir.
 *
 * Her ~5 iterasyonda her iki mod gecerli, geri kalanlarda
 * rastgele (buyuk olasilikla gecersiz) enum degerleri
 * gonderilir.
 *
 * Beklenen sonuclar: GL_NO_ERROR veya GL_INVALID_ENUM.
 * ============================================================ */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TC_007(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	unsigned int seed = 77777;
	int unexpected_err_count = 0;
	GLenum last_unexpected_err = GL_NO_ERROR;
	int fail_iteration = -1;

	for (int i = 0; i < 50000; i++) {
		GLenum modeRGB, modeAlpha;

		seed = seed * 1103515245 + 12345;

		if (i % 5 == 0) {
			/* Tumu gecerli */
			modeRGB = valid_modes[(seed >> 16) % valid_mode_count];
			seed = seed * 1103515245 + 12345;
			modeAlpha =
				valid_modes[(seed >> 16) % valid_mode_count];
		} else {
			/* Rastgele (buyuk olasilikla gecersiz) */
			modeRGB = (GLenum)((seed >> 16) % 0x10000);
			seed = seed * 1103515245 + 12345;
			modeAlpha = (GLenum)((seed >> 16) % 0x10000);
		}

		glBlendEquationSeparate(modeRGB, modeAlpha);

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
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	GLenum final_err = glGetError();

	int state_ok = check_eq_state(GL_FUNC_ADD, GL_FUNC_ADD);

	if (unexpected_err_count == 0 && final_err == GL_NO_ERROR && state_ok)
		TEST_LOG_SUCCESS(test_case_7, test_procedure);
	else
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Fuzzing hatasi: beklenmeyen=%d, "
			      "ilk_iter=%d, son_hata=0x%X, final=0x%X, "
			      "state_ok=%d",
			      unexpected_err_count, fail_iteration,
			      last_unexpected_err, final_err, state_ok);
}

/* Initialization */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001_init(void) {
	GS_GL20SC_PFO_BES_ROBUSTNESS_TC_001();
	GS_GL20SC_PFO_BES_ROBUSTNESS_TC_002();
	GS_GL20SC_PFO_BES_ROBUSTNESS_TC_003();
	GS_GL20SC_PFO_BES_ROBUSTNESS_TC_004();
	GS_GL20SC_PFO_BES_ROBUSTNESS_TC_005();
	GS_GL20SC_PFO_BES_ROBUSTNESS_TC_006();
	GS_GL20SC_PFO_BES_ROBUSTNESS_TC_007();

	GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_PFO_BES_ROBUSTNESS_TP_001_close(void) {
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glDisable(GL_BLEND);
}
