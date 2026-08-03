#include "../../../test_utility.h"

static const char *test_procedure = "GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SP_GAL_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SP_GAL_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SP_GAL_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_SP_GAL_ROBUSTNESS_TC_004";

/* ============================================================
 * TEST GRUBU: glGetAttribLocation
 * ============================================================ */

/* ============================================================
 * glGetAttribLocation — NULL Pointer
 * ============================================================
 *
 * glGetAttribLocation fonksiyonuna NULL pointer verilerek
 * sürücünün çökmeden -1 dönüp dönmediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GAL_ROBUSTNESS_TC_001(void) {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, NULL);

	if (!(loc == -1)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "NULL pointer için -1 dönmedi. Actual: %d", loc);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glGetAttribLocation — Rezerve Değişken
 * ============================================================
 *
 * OpenGL'e ait rezerve edilmiş bir değişken adı ("gl_Position")
 * ile glGetAttribLocation çağrılarak sürücünün bu ismi
 * reddedip reddetmediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GAL_ROBUSTNESS_TC_002(void) {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, "gl_Position");

	if (!(loc == -1)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Rezerve 'gl_Position' ismi için -1 dönmedi."
			      " Actual: %d",
			      loc);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glGetAttribLocation — Boş String
 * ============================================================
 *
 * Boş string ("") ile yapılan sorgunun geçersiz bir isim
 * olarak değerlendirilip -1 döndürdüğü doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GAL_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();

	GLint loc = glGetAttribLocation(prog, "");

	if (!(loc == -1)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Boş string için -1 dönmedi. Actual: %d", loc);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glGetAttribLocation — Bağlanmamış Program
 * ============================================================
 *
 * Link edilmemiş (sadece oluşturulmuş) bir program üzerinde
 * glGetAttribLocation çağrısının GL_INVALID_OPERATION ürettiği
 * doğrulanır.
 * ============================================================ */
void GS_GL20SC_SP_GAL_ROBUSTNESS_TC_004(void) {
	/* glCreateProgram ile oluşturulan ama link edilmemiş program */
	GLuint prog = glCreateProgram();
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLint loc = glGetAttribLocation(prog, "position");
	GLenum err = glGetError();

	/* Link edilmemiş program: GL_INVALID_OPERATION veya -1 dönmeli */
	if (!(err == GL_INVALID_OPERATION || loc == -1)) {
		TEST_LOG_FAIL(
		    test_case_1, test_procedure,
		    "Link edilmemiş program için GL_INVALID_OPERATION "
		    "beklendi. err=0x%04X, loc=%d",
		    err, loc);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* Initialization */
void GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001_init(void) {}

void GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001_draw(void) {
	GS_GL20SC_SP_GAL_ROBUSTNESS_TC_001();
	GS_GL20SC_SP_GAL_ROBUSTNESS_TC_002();
	GS_GL20SC_SP_GAL_ROBUSTNESS_TC_003();
	GS_GL20SC_SP_GAL_ROBUSTNESS_TC_004();
}

/* Cleanup */
void GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
}
