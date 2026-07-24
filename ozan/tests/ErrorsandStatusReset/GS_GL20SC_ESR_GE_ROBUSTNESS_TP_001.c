#include "../../../include/helper.h"
#include "../../../include/macro.h"
#include "../../../include/rtests.h"

static const char *test_procedure = "GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_ESR_GE_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_ESR_GE_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_ESR_GE_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_ESR_GE_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_ESR_GE_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_ESR_GE_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_ESR_GE_ROBUSTNESS_TC_007";

/* ============================================================
 * TEST 1: Basic Robustness
 * ============================================================
 */

/*
 * glGetError() hata kuyruğu boş olduğu zaman
 * GL_NO_ERROR döndürmelidir.
 *
 * Ayrıca arka arkaya yapılan çağrılar
 * herhangi bir hata üretmemelidir.
 */

void GS_GL20SC_ESR_GE_ROBUSTNESS_TC_001(void) {
	GLenum err;
	clearGLErrors();

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen : GL_NO_ERROR , error = 0x%x.", err);
		return;
	}

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen : GL_NO_ERROR , error = 0x%x.", err);
		return;
	}

	err = glGetError();
	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Beklenen : GL_NO_ERROR , error = 0x%x.", err);
		return;
	}

	TEST_LOG_SUCCESS(test_case_1, test_procedure);
}

/* ============================================================
 * TEST 2: Single Error Retrieval
 * ============================================================
 */

/*
 * Bilinçli olarak tek bir OpenGL hatası oluşturulur.
 *
 * İlk glGetError() doğru hata kodunu
 * döndürmelidir.
 *
 * İkinci glGetError() ise
 * GL_NO_ERROR döndürmelidir.
 */

void GS_GL20SC_ESR_GE_ROBUSTNESS_TC_002(void) {
	GLenum err;
	clearGLErrors();

	glEnable((GLenum)0xFFFFFFFF);

	err = glGetError();

	if (err != GL_INVALID_ENUM) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Beklenen : GL_INVALID_ENUM , error = 0x%x.",
			      err);
		return;
	}

	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_2, test_procedure,
			      "Error queue temizlenmedi. error = 0x%x.", err);
		return;
	}

	TEST_LOG_SUCCESS(test_case_2, test_procedure);
}

/* ============================================================
 * TEST 3: Consecutive Empty Queue Reads
 * ============================================================
 */

/*
 * Error queue boşaltıldıktan sonra
 * glGetError() fonksiyonu çok sayıda
 * ardışık çağrıda sürekli GL_NO_ERROR
 * döndürmelidir.
 */

void GS_GL20SC_ESR_GE_ROBUSTNESS_TC_003(void) {
	unsigned int i;
	GLenum err;
	clearGLErrors();

	for (i = 0; i < 10000; i++) {
		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_3, test_procedure,
				      "Iteration : %u , error = 0x%x.", i, err);
			return;
		}
	}

	TEST_LOG_SUCCESS(test_case_3, test_procedure);
}

/* ============================================================
 * TEST 4: Multiple Error Queue
 * ============================================================
 */

/*
 * Birden fazla OpenGL hatası oluşturulur.
 *
 * glGetError() tekrar tekrar çağrılarak
 * hata kuyruğunun doğru şekilde boşaltıldığı
 * doğrulanır.
 *
 * Son çağrı mutlaka GL_NO_ERROR döndürmelidir.
 */

void GS_GL20SC_ESR_GE_ROBUSTNESS_TC_004(void) {
	GLenum err;
	int errorCount = 0;
	clearGLErrors();

	/* Bilinçli olarak geçersiz enumlar gönder */
	glEnable((GLenum)0xFFFFFFFF);
	glDisable((GLenum)0xFFFFFFFE);
	glEnable((GLenum)0xFFFFFFFD);

	while ((err = glGetError()) != GL_NO_ERROR) {
		if (err != GL_INVALID_ENUM) {
			TEST_LOG_FAIL(test_case_4, test_procedure,
				      "Beklenmeyen error = 0x%x.", err);
			return;
		}

		errorCount++;
	}

	if (errorCount == 0) {
		TEST_LOG_FAIL(test_case_4, test_procedure, "No errors Found");
		return;
	}

	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Error queue tamamen temizlenmedi. error = 0x%x.",
			      err);
		return;
	}

	TEST_LOG_INFO("Kuyruktan okunan hata sayisi : %d", errorCount);
	TEST_LOG_SUCCESS(test_case_4, test_procedure);
}

/* ============================================================
 * TEST 5: State Preservation
 * ============================================================
 */

/*
 * glGetError() yalnızca hata bayrağını okur.
 *
 * Herhangi bir OpenGL state'ini değiştirmemelidir.
 *
 * Bu testte line width değiştirilir,
 * glGetError() çok sayıda çağrılır
 * ve state'in korunup korunmadığı doğrulanır.
 */

void GS_GL20SC_ESR_GE_ROBUSTNESS_TC_005(void) {
	GLenum err;
	unsigned int i;
	GLfloat width;
	clearGLErrors();

	glLineWidth(3.0f);

	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "glLineWidth hata uretti. error = 0x%x.", err);
		return;
	}

	glGetFloatv(GL_LINE_WIDTH, &width);

	if (width != 3.0f) {
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Line width ayarlanamadi. Gercek : %f", width);
		return;
	}

	for (i = 0; i < 10000; i++) {
		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_5, test_procedure,
				      "Iteration : %u , error = 0x%x.", i, err);
			return;
		}
	}

	if (!checkFloatState(test_case_5, test_procedure, GL_LINE_WIDTH, 3.0f,
			     1e-6f))
		return;

	glLineWidth(1.0f);
	clearGLErrors();

	TEST_LOG_SUCCESS(test_case_5, test_procedure);
}

/* ============================================================
 * TEST 6: Random Stress Test
 * ============================================================
 */

/*
 * glGetError() fonksiyonu 1.000.000 kez çağrılır.
 *
 * Amaç:
 *  - Driver crash
 *  - Memory corruption
 *  - Context kaybı
 *  - Beklenmeyen OpenGL hataları
 * gibi robustness problemlerini ortaya çıkarmaktır.
 *
 * Hata kuyruğu boş olduğundan her çağrı
 * GL_NO_ERROR döndürmelidir.
 */

void GS_GL20SC_ESR_GE_ROBUSTNESS_TC_006(void) {
	unsigned int i;
	GLenum err;
	clearGLErrors();

	for (i = 0; i < 1000000; i++) {
		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_6, test_procedure,
				      "Iteration : %u , error = 0x%x.", i, err);
			return;
		}
	}

	TEST_LOG_INFO("1,000,000 glGetError() cagrisi tamamlandi.");
	TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

/* ============================================================
 * TEST 7: Error Queue Recovery
 * ============================================================
 */

/*
 * Bu testte glGetError() fonksiyonunun
 * hata kuyruğunu tamamen temizlediği doğrulanır.
 *
 * Bir hata oluşturulur, okunur ve ardından
 * tekrar GL_NO_ERROR döndürdüğü kontrol edilir.
 */

void GS_GL20SC_ESR_GE_ROBUSTNESS_TC_007(void) {
	GLenum err;
	int i;
	clearGLErrors();

	glEnable((GLenum)0xFFFFFFFF);

	err = glGetError();

	if (err != GL_INVALID_ENUM) {
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Beklenen : GL_INVALID_ENUM, error = 0x%x.", err);
		return;
	}

	for (i = 0; i < 100; i++) {
		err = glGetError();

		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(test_case_7, test_procedure,
				      "Error queue temizlenmedi. Iteration : "
				      "%d error = 0x%x.",
				      i, err);
			return;
		}
	}

	TEST_LOG_SUCCESS(test_case_7, test_procedure);
}

/* Initialization */
void GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001_init(void) {

	CHECK_ERROR(test_procedure);
	GS_GL20SC_ESR_GE_ROBUSTNESS_TC_001();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_ESR_GE_ROBUSTNESS_TC_002();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_ESR_GE_ROBUSTNESS_TC_003();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_ESR_GE_ROBUSTNESS_TC_004();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_ESR_GE_ROBUSTNESS_TC_005();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_ESR_GE_ROBUSTNESS_TC_006();
	CHECK_ERROR(test_procedure);
	GS_GL20SC_ESR_GE_ROBUSTNESS_TC_007();
	CHECK_ERROR(test_procedure);


}

void GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001_draw(void) {
}
/* Cleanup */
void GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001_close(void) {

	CHECK_ERROR(test_procedure);

}