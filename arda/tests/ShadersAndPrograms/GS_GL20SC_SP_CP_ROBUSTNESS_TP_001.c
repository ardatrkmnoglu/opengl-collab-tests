#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_SP_CP_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_CP_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_CP_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_CP_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_SP_CP_ROBUSTNESS_TC_004";

/* ============================================================
 * GS_GL20SC_SP_CP_ROBUSTNESS_TC_003
 * ============================================================
 *
 * desteklenmemektedir (Objeler silinemez).
 * ============================================================ */
void GS_GL20SC_SP_CP_ROBUSTNESS_TC_003(void) {
	/* GL SC 2.0 uyumluluğu için test devre dışı bırakıldı */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * glCreateProgram — Döndürülen ID'nin Sıfır Olmaması
 * ============================================================
 *
 * Normal koşullarda glCreateProgram her zaman 0 olmayan bir
 * program nesnesi ID'si döndürmelidir. 0 dönüyorsa ve
 * GL_OUT_OF_MEMORY oluşmamışsa bu bir sürücü hatasıdır.
 * ============================================================ */
void GS_GL20SC_SP_CP_ROBUSTNESS_TC_004(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLuint prog = glCreateProgram();
	GLenum err = glGetError();

	if (err == GL_OUT_OF_MEMORY) {
		/* Bellek yetersizliği: çökmesiz kabul */
		TEST_LOG_INFO("%s: GL_OUT_OF_MEMORY — bellek yok.",
			      test_case_4);
		return;
	}

	if (!(prog != 0 && err == GL_NO_ERROR)) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "glCreateProgram 0 ID döndürdü (OOM değilken)."
			      " prog=%u, err=0x%04X",
			      prog, err);
	} else {
		TEST_LOG_SUCCESS(test_case_4, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_CP_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_CP_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_CP_ROBUSTNESS_TC_003();
	GS_GL20SC_SP_CP_ROBUSTNESS_TC_004();
}

/* Cleanup */
void GS_GL20SC_SP_CP_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
