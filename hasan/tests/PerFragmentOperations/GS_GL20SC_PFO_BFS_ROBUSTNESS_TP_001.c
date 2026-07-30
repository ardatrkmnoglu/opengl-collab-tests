#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

/*
GL20SC - PerFragmentOperations - BlendFuncSeparate - ROBUSTNESS
*/

static const char *test_procedure = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_007";

/* Forward declaration for close */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001_close(void);

/*
 * glBlendFuncSeparate icin gecerli faktor degerleri.
 * Desktop GL (llvmpipe) uzerinde test edildiginden
 * ARB_blend_func_extended eklenti faktorleri de dahildir.
 *
 * GL_SRC_ALPHA_SATURATE ve GL_SRC1_ALPHA desktop GL 4.x'te
 * hem sfactor hem dfactor olarak gecerlidir.
 */
static const GLenum valid_factors[] = {
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
	/* ARB_blend_func_extended / GL 3.3+ */
	0x88F9, /* GL_SRC1_COLOR              */
	0x88FA, /* GL_ONE_MINUS_SRC1_COLOR    */
	0x88FB, /* GL_ONE_MINUS_SRC1_ALPHA    */
	/* Desktop GL 4.x - hem src hem dst icin gecerli */
	GL_SRC_ALPHA_SATURATE,		    /* 0x0308 */
	0x8589, /* GL_SRC1_ALPHA              */
};
static const int valid_count = sizeof(valid_factors) / sizeof(valid_factors[0]);

/* Bir enum degerinin gecerli faktor olup olmadigini kontrol eder */
static int is_valid_factor(GLenum val) {
	for (int i = 0; i < valid_count; i++) {
		if (val == valid_factors[i])
			return 1;
	}
	return 0;
}

/* 4'lu state sorgulama yardimcisi */
static int check_blend_state(GLenum exp_src_rgb, GLenum exp_dst_rgb,
			     GLenum exp_src_a, GLenum exp_dst_a) {
	GLint sr, dr, sa, da;
	glGetIntegerv(GL_BLEND_SRC_RGB, &sr);
	glGetIntegerv(GL_BLEND_DST_RGB, &dr);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &sa);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &da);
	return ((GLenum)sr == exp_src_rgb && (GLenum)dr == exp_dst_rgb &&
		(GLenum)sa == exp_src_a && (GLenum)da == exp_dst_a);
}

/* ============================================================
 * TEST 1 : Gecersiz Enum Degerleri (Spec Hatasi)
 * ============================================================
 *
 * Amac
 * ----
 * Dort parametreden herhangi biri gecersiz oldugunda
 * GL_INVALID_ENUM uretilmelidir.
 *
 * Her parametre pozisyonu ayri ayri test edilir.
 * Gecersiz cagri sonrasi state korunmasi dogrulanir.
 * ============================================================ */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	/* Varsayilan state'i ayarla */
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
	glGetError();

	/* Gecersiz srcRGB */
	glBlendFuncSeparate(0xDEAD, GL_ONE, GL_ONE, GL_ONE);
	GLenum err1 = glGetError();

	/* Gecersiz dstRGB */
	glBlendFuncSeparate(GL_ONE, 0xDEAD, GL_ONE, GL_ONE);
	GLenum err2 = glGetError();

	/* Gecersiz srcAlpha */
	glBlendFuncSeparate(GL_ONE, GL_ONE, 0xDEAD, GL_ONE);
	GLenum err3 = glGetError();

	/* Gecersiz dstAlpha */
	glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, 0xDEAD);
	GLenum err4 = glGetError();

	/* Hepsi gecersiz */
	glBlendFuncSeparate(0xBEEF, 0xCAFE, 0xBABE, 0xFACE);
	GLenum err5 = glGetError();

	/* State korunmasi: hala (GL_ONE, GL_ZERO, GL_ONE, GL_ZERO) olmali */
	int state_ok = check_blend_state(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

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
 * RGB ve Alpha icin farkli faktorler ayarlandiginda
 * her birinin dogru kaydedildigini dogrular.
 *
 * glBlendFunc() tum kanallari ayni ayarlarken,
 * glBlendFuncSeparate() farkli ayarlayabilmelidir.
 * ============================================================ */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int failCount = 0;

	/* Farkli faktorlerle cesitli kombinasyonlar */
	struct {
		GLenum srcRGB, dstRGB, srcAlpha, dstAlpha;
	} combos[] = {
		{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO},
		{GL_ZERO, GL_ONE, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA},
		{GL_DST_COLOR, GL_SRC_COLOR, GL_CONSTANT_ALPHA,
		 GL_ONE_MINUS_CONSTANT_ALPHA},
		{GL_SRC_ALPHA_SATURATE, GL_ONE, GL_SRC_ALPHA, GL_DST_ALPHA},
		{GL_ONE, GL_ONE, GL_ZERO, GL_ZERO},
		{GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
		 GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA},
	};
	int combo_count = sizeof(combos) / sizeof(combos[0]);

	for (int i = 0; i < combo_count; i++) {
		glBlendFuncSeparate(combos[i].srcRGB, combos[i].dstRGB,
				    combos[i].srcAlpha, combos[i].dstAlpha);
		GLenum err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "Kombo %d reddedildi: err=0x%X", i, err);
			failCount++;
			continue;
		}

		if (!check_blend_state(combos[i].srcRGB, combos[i].dstRGB,
				       combos[i].srcAlpha,
				       combos[i].dstAlpha)) {
			GLint sr, dr, sa, da;
			glGetIntegerv(GL_BLEND_SRC_RGB, &sr);
			glGetIntegerv(GL_BLEND_DST_RGB, &dr);
			glGetIntegerv(GL_BLEND_SRC_ALPHA, &sa);
			glGetIntegerv(GL_BLEND_DST_ALPHA, &da);
			TEST_LOG_FAIL(test_case_2, test_procedure,
				      "Kombo %d state hatasi: "
				      "sr=0x%X dr=0x%X sa=0x%X da=0x%X",
				      i, sr, dr, sa, da);
			failCount++;
		}
	}

	/* Varsayilana geri don */
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

	if (failCount == 0)
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	else
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Toplam basarisiz: %d", failCount);
}

/* ============================================================
 * TEST 3 : Stress Sweep (srcRGB parametresi)
 * ============================================================
 *
 * Amac
 * ----
 * 16-bit GLenum uzayindaki tum degerler srcRGB
 * olarak sistematik bicimde denenir.
 * Diger parametreler sabit (GL_ONE) tutulur.
 *
 * Gecersiz degerlerde state'in korunup korunmadigi
 * da dogrulanmaktadir.
 * ============================================================ */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_003(void) {
	GLenum mode;
	int passCount = 0;
	int failCount = 0;

	while (glGetError() != GL_NO_ERROR)
		;

	/* Bilinen bir state ayarla */
	glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
	glGetError();

	GLint lastSrcRGB = GL_ONE;

	for (mode = 0; mode < 65536; mode++) {
		GLenum expected = is_valid_factor(mode) ? GL_NO_ERROR
							: GL_INVALID_ENUM;

		glBlendFuncSeparate(mode, GL_ONE, GL_ONE, GL_ONE);
		GLenum err = glGetError();

		if (err != expected) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "srcRGB=0x%X Beklenen=0x%X Gelen=0x%X",
				      mode, expected, err);
			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_NO_ERROR) {
			lastSrcRGB = (GLint)mode;
		}

		if (err == GL_INVALID_ENUM) {
			GLint actual;
			glGetIntegerv(GL_BLEND_SRC_RGB, &actual);
			if (actual != lastSrcRGB) {
				TEST_LOG_FAIL(
				    test_case_3, test_procedure,
				    "State bozuldu: srcRGB=0x%X "
				    "Beklenen=0x%X Gercek=0x%X",
				    mode, lastSrcRGB, actual);
				return;
			}
		}
	}

	TEST_LOG_INFO("srcRGB sweep - PASS: %d, FAIL: %d", passCount,
		      failCount);

	/* Varsayilana geri don */
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

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
 * dort parametre pozisyonunun her birinde denenerek
 * surucunun cokup cokmedigini kontrol ederiz.
 * ============================================================ */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum extreme[] = {(GLenum)INT_MAX, (GLenum)INT_MIN,
			    (GLenum)0xFFFFFFFF, (GLenum)0x80000000,
			    (GLenum)0x10000};
	int count = sizeof(extreme) / sizeof(extreme[0]);

	/* Bilinen bir state ayarla */
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
			    GL_ZERO);
	glGetError();

	for (int i = 0; i < count; i++) {
		/* Her parametre pozisyonunu ayri ayri test et */
		glBlendFuncSeparate(extreme[i], GL_ONE, GL_ONE, GL_ONE);
		GLenum e1 = glGetError();

		glBlendFuncSeparate(GL_ONE, extreme[i], GL_ONE, GL_ONE);
		GLenum e2 = glGetError();

		glBlendFuncSeparate(GL_ONE, GL_ONE, extreme[i], GL_ONE);
		GLenum e3 = glGetError();

		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, extreme[i]);
		GLenum e4 = glGetError();

		if (e1 != GL_INVALID_ENUM || e2 != GL_INVALID_ENUM ||
		    e3 != GL_INVALID_ENUM || e4 != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Enum=0x%X e1=0x%X e2=0x%X "
				      "e3=0x%X e4=0x%X",
				      extreme[i], e1, e2, e3, e4);
			return;
		}

		/* State korunmasi */
		if (!check_blend_state(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
				       GL_ONE, GL_ZERO)) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "State bozuldu: Enum=0x%X", extreme[i]);
			return;
		}
	}

	/* Varsayilana geri don */
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Gecersiz Cagri Sonrasi State Korunmasi
 * ============================================================
 *
 * Amac
 * ----
 * Farkli gecerli kombinasyonlar ayarlandiktan sonra
 * gecersiz degerler (dort parametre pozisyonunun her
 * birinde) gonderildiginde state'in degismemesi gerekir.
 * ============================================================ */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	struct {
		GLenum srcRGB, dstRGB, srcAlpha, dstAlpha;
	} valid_combos[] = {
		{GL_ONE, GL_ZERO, GL_ONE, GL_ZERO},
		{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE},
		{GL_DST_COLOR, GL_CONSTANT_ALPHA, GL_SRC_ALPHA,
		 GL_ONE_MINUS_DST_ALPHA},
	};
	int combo_count = sizeof(valid_combos) / sizeof(valid_combos[0]);

	/* NOT: 0x0000 = GL_ZERO gecerli, listeye dahil etme */
	GLenum invalidEnums[] = {0xDEAD, 0xBEEF, 0x1234, 0xFFFF, 0x9999};
	int inv_count = sizeof(invalidEnums) / sizeof(invalidEnums[0]);

	for (int i = 0; i < combo_count; i++) {
		glBlendFuncSeparate(valid_combos[i].srcRGB,
				    valid_combos[i].dstRGB,
				    valid_combos[i].srcAlpha,
				    valid_combos[i].dstAlpha);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Kombo %d reddedildi: err=0x%X", i, err);
			return;
		}

		for (int j = 0; j < inv_count; j++) {
			/* Gecersiz srcRGB */
			glBlendFuncSeparate(invalidEnums[j], GL_ONE, GL_ONE,
					    GL_ONE);
			if (glGetError() != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "srcRGB hatasi: inv=0x%X", invalidEnums[j]);
				return;
			}

			/* Gecersiz dstRGB */
			glBlendFuncSeparate(GL_ONE, invalidEnums[j], GL_ONE,
					    GL_ONE);
			if (glGetError() != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "dstRGB hatasi: inv=0x%X", invalidEnums[j]);
				return;
			}

			/* Gecersiz srcAlpha */
			glBlendFuncSeparate(GL_ONE, GL_ONE, invalidEnums[j],
					    GL_ONE);
			if (glGetError() != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(test_case_5, test_procedure,
					      "srcAlpha hatasi: inv=0x%X",
					      invalidEnums[j]);
				return;
			}

			/* Gecersiz dstAlpha */
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE,
					    invalidEnums[j]);
			if (glGetError() != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(test_case_5, test_procedure,
					      "dstAlpha hatasi: inv=0x%X",
					      invalidEnums[j]);
				return;
			}

			/* State korunmasi */
			if (!check_blend_state(valid_combos[i].srcRGB,
					       valid_combos[i].dstRGB,
					       valid_combos[i].srcAlpha,
					       valid_combos[i].dstAlpha)) {
				TEST_LOG_FAIL(
				    test_case_5, test_procedure,
				    "State bozuldu: kombo=%d, inv=0x%X", i,
				    invalidEnums[j]);
				return;
			}
		}
	}

	/* Varsayilana geri don */
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Enable/Disable State Strese Sokma (State Thrashing)
 * ============================================================
 *
 * Amac
 * ----
 * GL_BLEND ozelligini binlerce kez acip kapatarak
 * ve her seferinde farkli bir blend func separate
 * ayarlayarak state makinesinin bozulup bozulmadigini
 * test eder.
 * ============================================================ */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int err_count = 0;

	for (int i = 0; i < 10000; i++) {
		GLenum srcRGB = valid_factors[i % valid_count];
		GLenum dstRGB = valid_factors[(i + 3) % valid_count];
		GLenum srcA = valid_factors[(i + 7) % valid_count];
		GLenum dstA = valid_factors[(i + 11) % valid_count];

		glEnable(GL_BLEND);
		glBlendFuncSeparate(srcRGB, dstRGB, srcA, dstA);

		if (!check_blend_state(srcRGB, dstRGB, srcA, dstA))
			err_count++;

		if (!glIsEnabled(GL_BLEND))
			err_count++;

		glDisable(GL_BLEND);
		if (glIsEnabled(GL_BLEND))
			err_count++;
	}

	GLenum err = glGetError();

	/* Varsayilana geri don */
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
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
 * glBlendFuncSeparate cagirilarak bellek veya state
 * bozulmasi tespiti yapilir.
 *
 * Dort parametre birden rastgele uretilir.
 * Her ~5 iterasyonda tumu gecerli, geri kalanlarda
 * en az biri gecersiz olabilir.
 *
 * Beklenen sonuclar: GL_NO_ERROR veya GL_INVALID_ENUM.
 * ============================================================ */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_007(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	unsigned int seed = 11111;
	int unexpected_err_count = 0;
	GLenum last_unexpected_err = GL_NO_ERROR;
	int fail_iteration = -1;

	for (int i = 0; i < 50000; i++) {
		GLenum params[4];

		if (i % 5 == 0) {
			/* Tumu gecerli */
			for (int p = 0; p < 4; p++) {
				seed = seed * 1103515245 + 12345;
				params[p] =
					valid_factors[(seed >> 16) %
						      valid_count];
			}
		} else {
			/* Rastgele (buyuk olasilikla gecersiz) */
			for (int p = 0; p < 4; p++) {
				seed = seed * 1103515245 + 12345;
				params[p] =
					(GLenum)((seed >> 16) % 0x10000);
			}
		}

		glBlendFuncSeparate(params[0], params[1], params[2], params[3]);

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
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
	GLenum final_err = glGetError();

	int state_ok = check_blend_state(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

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
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001_init(void) {
	GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_001();
	GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_002();
	GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_003();
	GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_004();
	GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_005();
	GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_006();
	GS_GL20SC_PFO_BFS_ROBUSTNESS_TC_007();

	GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_PFO_BFS_ROBUSTNESS_TP_001_close(void) {
	glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);
}
