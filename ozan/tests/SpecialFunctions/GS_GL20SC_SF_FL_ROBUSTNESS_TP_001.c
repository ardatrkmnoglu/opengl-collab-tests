#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char *test_procedure = "GS_GL20SC_SF_FL_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_SF_FL_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_SF_FL_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_SF_FL_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_SF_FL_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_SF_FL_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_SF_FL_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_SF_FL_ROBUSTNESS_TC_007";
/* ============================================================
 * TEST 1 : Basic Robustness
 * ============================================================
 */

/*
 * glFlush() daha önce gönderilmiş OpenGL
 * komutlarını GPU'ya göndermelidir.
 *
 * Fonksiyon hata üretmemeli ve
 * geçerli komutlardan sonra
 * GL_NO_ERROR döndürmelidir.
 */

void GS_GL20SC_SF_FL_ROBUSTNESS_TC_001(void) {
	GLenum err;

	clearGLErrors();

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();

	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure, "Error : 0x%X", err);
		return;
	}

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2 : State Preservation
 * ============================================================
 */

/*
 * glFlush() yalnızca komutların yürütülmesini
 * başlatmalıdır.
 * OpenGL state'ini değiştirmemelidir.
 *
 * Viewport ayarlanır.
 * glFlush() çağrılır.
 *
 * Viewport'un aynı kaldığı doğrulanır.
 */

void GS_GL20SC_SF_FL_ROBUSTNESS_TC_002(void) {
	GLenum err;

	clearGLErrors();

	glViewport(10, 20, 320, 240);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_2, test_procedure, "Error : 0x%X", err);
		return;
	}

	glFlush();

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_2, test_procedure, "Error : 0x%X", err);
		return;
	}

	if (!checkViewport(test_case_2, test_procedure, 10, 20, 320, 240))
		return;

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3 : Error Queue Preservation
 * ============================================================
 */

/*
 * glFlush() hata kuyruğunu değiştirmemelidir.
 *
 * Geçerli OpenGL komutlarından sonra çağrıldığında
 * GL_NO_ERROR üretmeli ve hata kuyruğunu
 * bozmamalıdır.
 */

void GS_GL20SC_SF_FL_ROBUSTNESS_TC_003(void) {
	GLenum err;

	clearGLErrors();

	glClear(GL_COLOR_BUFFER_BIT);

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_3, test_procedure, "Error : 0x%X", err);
		return;
	}

	glFlush();

	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Beklenmeyen hata kodu : 0x%X", err);
		return;
	}

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_3, test_procedure, "Error : 0x%X", err);
		return;
	}

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4 : Repeated Invocation
 * ============================================================
 */

/*
 * glFlush() arka arkaya çok sayıda çağrıldığında
 * kararlı çalışmalıdır.
 *
 * Her çağrı başarılı olmalı ve
 * herhangi bir OpenGL hatası üretmemelidir.
 */

void GS_GL20SC_SF_FL_ROBUSTNESS_TC_004(void) {
	unsigned int i;
	GLenum err;

	clearGLErrors();

	for (i = 0; i < 10000; i++) {
		glFlush();

		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Iteration : %u Error : 0x%X", i, err);
			return;
		}
	}

	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5 : Command Submission Robustness
 * ============================================================
 */

/*
 * glFlush() gönderilen OpenGL komutlarını
 * GPU'ya iletmelidir.
 *
 * Bu testte framebuffer farklı renklerle
 * temizlenir ve her temizleme işleminden sonra
 * glFlush() çağrılır.
 *
 * Amaç;
 * - Komut gönderiminin güvenli şekilde
 *   gerçekleştirildiğini,
 * - Beklenmeyen OpenGL hatalarının
 *   oluşmadığını doğrulamaktır.
 */

void GS_GL20SC_SF_FL_ROBUSTNESS_TC_005(void) {
	unsigned int i;
	GLenum err;

	clearGLErrors();

	for (i = 0; i < 5000; i++) {
		GLfloat color = (GLfloat)(i % 255) / 255.0f;

		glClearColor(color, 0.0f, 1.0f - color, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glFlush();
		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %u Error : 0x%X", i, err);
			return;
		}
	}

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6 : Stress Test
 * ============================================================
 */

/*
 * glFlush() 1.000.000 kez çağrılır.
 *
 * Amaç:
 *  - Driver crash
 *  - Memory corruption
 *  - Context kaybı
 *  - Beklenmeyen OpenGL hataları
 *
 * gibi robustness problemlerini ortaya çıkarmaktır.
 *
 * Her çağrı GL_NO_ERROR üretmelidir.
 */

void GS_GL20SC_SF_FL_ROBUSTNESS_TC_006(void) {
	unsigned int i;
	GLenum err;

	clearGLErrors();

	for (i = 0; i < 1000000; i++) {
		glFlush();

		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Iteration : %u Error : 0x%X", i, err);
			return;
		}
	}

	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7 : Consecutive Command Submission
 * ============================================================
 */

/*
 * glFlush() ardışık komut gönderimlerinde
 * kararlı çalışmalıdır.
 *
 * Her iterasyonda yeni bir OpenGL komutu
 * gönderilir ve ardından glFlush() çağrılır.
 *
 * Amaç;
 * komut kuyruğunun sürekli kullanımında
 * sürücünün kararlı kaldığını doğrulamaktır.
 */

void GS_GL20SC_SF_FL_ROBUSTNESS_TC_007(void) {
	unsigned int i;
	GLenum err;

	clearGLErrors();

	for (i = 0; i < 1000; i++) {
		GLfloat color = (GLfloat)(i % 100) / 100.0f;

		glClearColor(color, 1.0f - color, 0.5f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		glFlush();

		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_7, test_procedure,
				      "Iteration : %u Error : 0x%X", i, err);
			return;
		}
	}

	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

/* Initialization */
void GS_GL20SC_SF_FL_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_FL_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_FL_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_FL_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_FL_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_FL_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_FL_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_SF_FL_ROBUSTNESS_TC_007();
	CHECK_ERROR(test_procedure);

}

void GS_GL20SC_SF_FL_ROBUSTNESS_TP_001_draw(void) {
}
/* Cleanup */
void GS_GL20SC_SF_FL_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}