#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

/*
GL20SC - PerFragmentOperations - BlendEquation - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_BE_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_BE_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_BE_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_BE_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_BE_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_BE_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_BE_ROBUSTNESS_TC_007";

/* Forward declaration for close */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * TEST 1 : Gecersiz Enum Degerleri (Spec Hatasi)
 * ============================================================
 *
 * Amac
 * ----
 * Spec'e gore mode parametresi GL_FUNC_ADD, GL_FUNC_SUBTRACT
 * veya GL_FUNC_REVERSE_SUBTRACT olmalidir.
 *
 * Bunlarin disindaki degerler icin GL_INVALID_ENUM
 * uretilmelidir.
 *
 * Ayrica gecersiz cagri sonrasi state'in degismemesi
 * dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	// Varsayilan state'i ayarla
	glBlendEquation(GL_FUNC_ADD);
	glGetError(); // Temizle

	// Tamamen gecersiz enum degerleri
	glBlendEquation(0x0000);
	GLenum err1 = glGetError();

	glBlendEquation(0xDEAD);
	GLenum err2 = glGetError();

	glBlendEquation(GL_NONE);
	GLenum err3 = glGetError();

	// GL_BLEND kendisi gecerli bir enum ama BlendEquation modlari arasinda degil
	glBlendEquation(GL_BLEND);
	GLenum err4 = glGetError();

	// State korunmasi kontrolu: hala GL_FUNC_ADD olmali
	GLint mode_val;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &mode_val);
	int state_ok = (mode_val == GL_FUNC_ADD);

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
 * TEST 2 : Stress Sweep
 * ============================================================
 *
 * Amac
 * ----
 * 16-bit GLenum uzayindaki tum degerler sistematik
 * olarak denenir.
 *
 * Yalnizca
 *
 *      GL_FUNC_ADD            (0x8006)
 *      GL_FUNC_SUBTRACT       (0x800A)
 *      GL_FUNC_REVERSE_SUBTRACT (0x800B)
 *
 * degerlerinin kabul edilmesi beklenmektedir.
 *
 * Gecersiz degerlerde state'in korunup korunmadigi
 * da dogrulanmaktadir.
 * ============================================================ */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TC_002(void) {
	GLenum mode;
	GLint currentMode = GL_FUNC_ADD;
	int passCount = 0;
	int failCount = 0;

	while (glGetError() != GL_NO_ERROR)
		;

	// Varsayilan state'i ayarla
	glBlendEquation(GL_FUNC_ADD);
	glGetError(); // Temizle

	for (mode = 0; mode < 65536; mode++) {
		GLenum expected = (mode == GL_FUNC_ADD ||
				   mode == GL_FUNC_SUBTRACT ||
				   mode == GL_FUNC_REVERSE_SUBTRACT)
					  ? GL_NO_ERROR
					  : GL_INVALID_ENUM;

		GLenum err;
		glBlendEquation(mode);
		err = glGetError();
		if (err != expected) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
				      mode, expected, err);

			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_NO_ERROR) {
			currentMode = (GLint)mode;
		}

		if (err == GL_INVALID_ENUM) {
			GLint actual;
			glGetIntegerv(GL_BLEND_EQUATION_RGB, &actual);
			if (actual != currentMode) {
				TEST_LOG_FAIL(
				    test_case_2, test_procedure,
				    "State bozuldu: Enum=0x%X "
				    "Beklenen_State=0x%X Gercek_State=0x%X",
				    mode, currentMode, actual);
				return;
			}
		}
	}

	TEST_LOG_INFO("PASS : %d", passCount);
	TEST_LOG_INFO("FAIL : %d", failCount);

	if (failCount != 0) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Basarisiz cagri sayisi : %d", failCount);
		return;
	}

	// Varsayilana geri don
	glBlendEquation(GL_FUNC_ADD);

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : Asiri Buyuk ve Sinir Enum Degerleri
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
void GS_GL20SC_PFO_BE_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum values[] = {(GLenum)INT_MAX, (GLenum)INT_MIN,
			   (GLenum)0xFFFFFFFF, (GLenum)0x7FFFFFFF,
			   (GLenum)0x80000000, (GLenum)0x10000};

	int count = sizeof(values) / sizeof(values[0]);

	// Bilinen bir state ayarla
	glBlendEquation(GL_FUNC_SUBTRACT);
	glGetError(); // Temizle

	for (int i = 0; i < count; i++) {
		glBlendEquation(values[i]);
		GLenum err = glGetError();

		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "Enum=0x%X Beklenen=0x%X Gelen=0x%X",
				      values[i], GL_INVALID_ENUM, err);
			return;
		}

		// State korunmasi kontrolu
		GLint mode_val;
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &mode_val);
		if (mode_val != GL_FUNC_SUBTRACT) {
			TEST_LOG_FAIL(
			    test_case_3, test_procedure,
			    "State bozuldu: Enum=0x%X Beklenen=0x%X "
			    "Gercek=0x%X",
			    values[i], GL_FUNC_SUBTRACT, mode_val);
			return;
		}
	}

	// Varsayilana geri don
	glBlendEquation(GL_FUNC_ADD);

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : Gecersiz Cagri Sonrasi State Korunmasi
 * ============================================================
 *
 * Amac
 * ----
 * Her uc gecerli mod (GL_FUNC_ADD, GL_FUNC_SUBTRACT,
 * GL_FUNC_REVERSE_SUBTRACT) ayarlandiktan sonra
 * gecersiz bir mod verildigi zaman mevcut durumun
 * degismemesi gerekir.
 *
 * Farkli gecersiz enum degerleri gonderilir ve
 * her seferinde state kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum validModes[] = {GL_FUNC_ADD, GL_FUNC_SUBTRACT,
			       GL_FUNC_REVERSE_SUBTRACT};
	GLenum invalidEnums[] = {0x0000, 0xDEAD, 0xBEEF, 0x1234, 0xFFFF};

	int validCount = sizeof(validModes) / sizeof(validModes[0]);
	int invalidCount = sizeof(invalidEnums) / sizeof(invalidEnums[0]);

	for (int i = 0; i < validCount; i++) {
		// Gecerli bir mod ayarla
		glBlendEquation(validModes[i]);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Gecerli mod ayarlanamadi: mode=0x%X, "
				      "err=0x%X",
				      validModes[i], err);
			return;
		}

		// Her gecersiz enum ile test et
		for (int j = 0; j < invalidCount; j++) {
			glBlendEquation(invalidEnums[j]);
			err = glGetError();

			if (err != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(
				    test_case_4, test_procedure,
				    "Gecersiz enum hatasi: mode=0x%X, "
				    "invalid=0x%X, err=0x%X",
				    validModes[i], invalidEnums[j], err);
				return;
			}

			// State korunmasi kontrolu
			GLint mode_val;
			glGetIntegerv(GL_BLEND_EQUATION_RGB, &mode_val);
			if ((GLenum)mode_val != validModes[i]) {
				TEST_LOG_FAIL(
				    test_case_4, test_procedure,
				    "State bozuldu: beklenen=0x%X, "
				    "gercek=0x%X, invalid=0x%X",
				    validModes[i], mode_val,
				    invalidEnums[j]);
				return;
			}
		}
	}

	// Varsayilana geri don
	glBlendEquation(GL_FUNC_ADD);

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Enable/Disable State Strese Sokma (State Thrashing)
 * ============================================================
 *
 * Amac
 * ----
 * GL_BLEND ozelligini binlerce kez ard arda acip kapatarak
 * ve her seferinde farkli bir blend equation ayarlayarak
 * surucunun durum (state) makinesinin bozulup bozulmadigini
 * test eder.
 *
 * Her cagri sonrasi GL_BLEND_EQUATION_RGB sorgulanir.
 * ============================================================ */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum modes[] = {GL_FUNC_ADD, GL_FUNC_SUBTRACT,
			  GL_FUNC_REVERSE_SUBTRACT};
	int err_count = 0;

	for (int i = 0; i < 10000; i++) {
		GLenum current_mode = modes[i % 3];

		glEnable(GL_BLEND);
		glBlendEquation(current_mode);

		GLint mode_val;
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &mode_val);
		if ((GLenum)mode_val != current_mode)
			err_count++;

		if (!glIsEnabled(GL_BLEND))
			err_count++;

		glDisable(GL_BLEND);
		if (glIsEnabled(GL_BLEND))
			err_count++;
	}

	GLenum err = glGetError();

	// Varsayilana geri don
	glBlendEquation(GL_FUNC_ADD);
	glDisable(GL_BLEND);

	if (err == GL_NO_ERROR && err_count == 0)
		TEST_LOG_SUCCESS(test_case_5, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case_5, test_procedure,
		    "State thrashing hatasi: err=0x%X, uyusmazlik=%d kez", err,
		    err_count);
}

/* ============================================================
 * TEST 6 : Deterministik Fuzzing (glBlendEquation Cagrilari)
 * ============================================================
 *
 * Amac
 * ----
 * Cok sayida deterministik (mantikli ve mantiksiz)
 * glBlendEquation cagirilarak bellek veya state bozulmasi
 * tespiti yapilir.
 *
 * Her ~5 iterasyonda gecerli bir mod, geri kalan
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
void GS_GL20SC_PFO_BE_ROBUSTNESS_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum valid_modes[] = {GL_FUNC_ADD, GL_FUNC_SUBTRACT,
				GL_FUNC_REVERSE_SUBTRACT};

	// Basit deterministik LCG (Linear Congruential Generator)
	unsigned int seed = 67890;
	int unexpected_err_count = 0;
	GLenum last_unexpected_err = GL_NO_ERROR;
	int fail_iteration = -1;

	for (int i = 0; i < 50000; i++) {
		seed = seed * 1103515245 + 12345;

		GLenum mode;
		if (i % 5 == 0) {
			// Her 5 iterasyonda gecerli bir mod gonder
			mode = valid_modes[(seed >> 16) % 3];
		} else {
			// Rastgele (buyuk olasilikla gecersiz) enum degeri
			mode = (GLenum)((seed >> 16) % 0x10000);
		}

		glBlendEquation(mode);

		GLenum err = glGetError();
		// GL_NO_ERROR veya GL_INVALID_ENUM beklenen sonuclardir
		// Bunlarin disindaki hatalar beklenmeyen hatadir
		if (err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
			unexpected_err_count++;
			last_unexpected_err = err;
			if (fail_iteration < 0)
				fail_iteration = i;
		}
	}

	// Dongu sonrasi GL context sagligini dogrula
	while (glGetError() != GL_NO_ERROR)
		;
	glBlendEquation(GL_FUNC_ADD);
	GLenum final_err = glGetError();

	// State'in dogru dondugunu dogrula
	GLint mode_val;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &mode_val);
	int state_ok = (mode_val == GL_FUNC_ADD);

	if (unexpected_err_count == 0 && final_err == GL_NO_ERROR && state_ok)
		TEST_LOG_SUCCESS(test_case_6, test_procedure);
	else
		TEST_LOG_FAIL(test_case_6, test_procedure,
			      "Fuzzing hatasi: beklenmeyen_hata=%d kez, "
			      "ilk_hata_iter=%d, son_hata=0x%X, final=0x%X, "
			      "state_ok=%d",
			      unexpected_err_count, fail_iteration,
			      last_unexpected_err, final_err, state_ok);
}

/* ============================================================
 * TEST 7 : Error Queue Management
 * ============================================================
 *
 * Amac
 * ----
 * Arka arkaya cok sayida gecersiz enum gonderildiginde
 * hata kuyrugunun bozulmadigi dogrulanir.
 *
 * Daha sonra gecerli bir cagri yapilarak surucunun
 * normal calismaya dondugu kontrol edilir.
 * ============================================================ */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TC_007(void) {
	GLenum err;
	int errorCount = 0;

	while (glGetError() != GL_NO_ERROR)
		;

	// 100 adet gecersiz enum gonder
	for (int i = 0; i < 100; i++) {
		glBlendEquation((GLenum)(0x5000 + i));
	}

	// Hata kuyrugunu bosalt ve kontrol et
	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_7, test_procedure,
				      "Beklenen : 0x%X Gelen : 0x%X",
				      GL_INVALID_ENUM, err);
			return;
		}
		errorCount++;
	}

	if (errorCount <= 0) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Hata kuyrugunda hic hata bulunamadi.");
		return;
	}

	// Gecerli bir cagri yap ve surucunun duzeldigi kontrol et
	glBlendEquation(GL_FUNC_SUBTRACT);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Beklenmeyen hata : 0x%X", err);
		return;
	}

	GLint mode_val;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, &mode_val);
	if (mode_val != GL_FUNC_SUBTRACT) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "State hatasi: beklenen=0x%X, gercek=0x%X",
			      GL_FUNC_SUBTRACT, mode_val);
		return;
	}

	// Varsayilana geri don
	glBlendEquation(GL_FUNC_ADD);

	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

/* Initialization */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_init(void) {
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_PFO_BE_ROBUSTNESS_TC_001();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_PFO_BE_ROBUSTNESS_TC_002();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_PFO_BE_ROBUSTNESS_TC_003();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_PFO_BE_ROBUSTNESS_TC_004();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_PFO_BE_ROBUSTNESS_TC_005();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_PFO_BE_ROBUSTNESS_TC_006();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_PFO_BE_ROBUSTNESS_TC_007();
	//CHECK_ERROR(test_procedure);

	GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_PFO_BE_ROBUSTNESS_TP_001_close(void) {
	// Blend state'ini varsayilana sifirla
	glBlendEquation(GL_FUNC_ADD);
	glDisable(GL_BLEND);
	//CHECK_ERROR(test_procedure);
}
