#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

static const char *test_case1 = "PerFragmentOperations_Scissor_TC_001";
static const char *test_case2 = "PerFragmentOperations_Scissor_TC_002";
static const char *test_case3 = "PerFragmentOperations_Scissor_TC_003";
static const char *test_case4 = "PerFragmentOperations_Scissor_TC_004";
static const char *test_case5 = "PerFragmentOperations_Scissor_TC_005";
static const char *test_case6 = "PerFragmentOperations_Scissor_TC_006";

static const char *test_procedure = "Per-FragmentOperations_Scissor_TP_001";

/*
 * ============================================================================
 * glScissor & GL_SCISSOR_TEST Robustness Test Suite
 * Hasan - OpenGL ES 2.0
 *
 * Bu test paketi, Per-Fragment operasyonlarindan "Scissor Test" mekanizmasini
 * sinamak uzere yazilmistir. Fuzzing ve sinir testlerini icerir.
 * ============================================================================
 */

// ---------------------------------------------------------------
// TEST 1: Negatif Genislik ve Yukseklik (Spec Hatasi)
// Spec'e gore width veya height 0'dan kucukse GL_INVALID_VALUE uretilmelidir.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glScissor(0, 0, -1, 100);
	GLenum err1 = glGetError();

	glScissor(0, 0, 100, -50);
	GLenum err2 = glGetError();

	glScissor(0, 0, -1, -1);
	GLenum err3 = glGetError();

	if (err1 == GL_INVALID_VALUE && err2 == GL_INVALID_VALUE &&
	    err3 == GL_INVALID_VALUE)
		TEST_LOG_SUCCESS(test_case1, test_procedure);
	else
		TEST_LOG_FAIL(test_case1, test_procedure,
			      "Negatif boyutlar reddedilmedi: err1=0x%X, "
			      "err2=0x%X, err3=0x%X",
			      err1, err2, err3);
}

// ---------------------------------------------------------------
// TEST 2: Negatif Koordinatlar (Gecerli Kullanim)
// Spec'e gore x (left) ve y (bottom) degerleri negatif OLABILIR.
// Bu durumda kutunun bir kismi ekran disinda kalir. Hata VERILMEMELIDIR.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glScissor(-100, -50, 200, 200);
	GLenum err = glGetError();

	GLint box[4];
	glGetIntegerv(GL_SCISSOR_BOX, box);

	if (err == GL_NO_ERROR && box[0] == -100 && box[1] == -50)
		TEST_LOG_SUCCESS(test_case2, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case2, test_procedure,
		    "Negatif koordinat hatasi: err=0x%X, box=(%d,%d,%d,%d)",
		    err, box[0], box[1], box[2], box[3]);
}

// ---------------------------------------------------------------
// TEST 3: Asiri Buyuk Degerlerle (INT_MAX/INT_MIN) Sinir Zorlamasi
// Tamsayi tasmasi (integer overflow) yaratip surucunun cokup
// cokmedigini kontrol ederiz.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	// Boyutlar (w, h) negatif olamaz, bu yuzden onlara INT_MAX veriyoruz.
	// Koordinatlar (x, y) negatif olabilir.

	glScissor(INT_MIN, INT_MIN, INT_MAX, INT_MAX);
	GLenum err1 = glGetError();

	glScissor(INT_MAX, INT_MAX, INT_MAX, INT_MAX);
	GLenum err2 = glGetError();

	// Spec bu durumlarda hata tanimlamaz, ancak sistemin cokmemesi esastir.
	if (err1 == GL_NO_ERROR && err2 == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case3, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case3, test_procedure,
		    "Asiri buyuk limitlerde hata: err1=0x%X, err2=0x%X", err1,
		    err2);
}

// ---------------------------------------------------------------
// TEST 4: Sifir Boyutlu Scissor Kutusu
// Genislik veya yuksekligin tam 0 olmasi yasaldir.
// Hata vermemesi ve basariyla kaydedilmesi gerekir.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glScissor(10, 10, 0, 0);
	GLenum err = glGetError();

	GLint box[4];
	glGetIntegerv(GL_SCISSOR_BOX, box);

	if (err == GL_NO_ERROR && box[2] == 0 && box[3] == 0)
		TEST_LOG_SUCCESS(test_case4, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case4, test_procedure,
		    "Sifir boyutlu kutu hatasi: err=0x%X, boyut=%dx%d", err,
		    box[2], box[3]);
}

// ---------------------------------------------------------------
// TEST 5: Enable/Disable State Strese Sokma (State Thrashing)
// GL_SCISSOR_TEST ozelligini binlerce kez ard arda acip kapatarak
// surucunun durum (state) makinesinin bozulup bozulmadigini test eder.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	int err_count = 0;
	for (int i = 0; i < 10000; i++) {
		glEnable(GL_SCISSOR_TEST);
		if (!glIsEnabled(GL_SCISSOR_TEST))
			err_count++;

		glDisable(GL_SCISSOR_TEST);
		if (glIsEnabled(GL_SCISSOR_TEST))
			err_count++;
	}

	GLenum err = glGetError();

	if (err == GL_NO_ERROR && err_count == 0)
		TEST_LOG_SUCCESS(test_case5, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case5, test_procedure,
		    "State thrashing hatasi: err=0x%X, uyusmazlik=%d kez", err,
		    err_count);
}

// ---------------------------------------------------------------
// TEST 6: Rastgele Fuzzing (Rastgele glScissor Cagriları)
// Cok sayida tamamen rastgele (mantikli ve mantiksiz) glScissor cagirilarak
// bellek veya state bozulmasi tespiti yapilir.
// ---------------------------------------------------------------
void PerFragmentOperations_Scissor_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	srand(12345); // Sabit seed (tekrar edilebilirlik)

	for (int i = 0; i < 50000; i++) {
		GLint x = (rand() % 4000) - 2000; // -2000 ile 2000 arasi
		GLint y = (rand() % 4000) - 2000;

		// %10 ihtimalle negatif boyutlar göndererek GL_INVALID_VALUE
		// tetiklet (kasten)
		GLsizei w = (rand() % 100 < 10) ? -rand() % 500 : rand() % 4000;
		GLsizei h = (rand() % 100 < 10) ? -rand() % 500 : rand() % 4000;

		glScissor(x, y, w, h);

		// Her 1000 adimda bir hata kuyrugunu temizle (cok birikmesin)
		if (i % 1000 == 0) {
			while (glGetError() != GL_NO_ERROR)
				;
		}
	}

	// Dongu cokmeden bittiyse surucu saglamdir.
	TEST_LOG_SUCCESS(test_case6, test_procedure);
}
