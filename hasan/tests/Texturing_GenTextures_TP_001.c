#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

static const char *test_case1 = "Texturing_GenTextures_TC_001";
static const char *test_case2 = "Texturing_GenTextures_TC_002";
static const char *test_case3 = "Texturing_GenTextures_TC_003";
static const char *test_case4 = "Texturing_GenTextures_TC_004";
static const char *test_case5 = "Texturing_GenTextures_TC_005";
static const char *test_case6 = "Texturing_GenTextures_TC_006";
static const char *test_case7 = "Texturing_GenTextures_TC_007";
static const char *test_case8 = "Texturing_GenTextures_TC_008";
static const char *test_case9 = "Texturing_GenTextures_TC_009";
static const char *test_case10 = "Texturing_GenTextures_TC_010";
static const char *test_case11 = "Texturing_GenTextures_TC_011";
static const char *test_case12 = "Texturing_GenTextures_TC_012";

static const char *test_procedure = "Texturing_GenTextures_TP_001";

// Static variables for cleanup in close()
static GLuint g_tex1 = 0;
static GLuint g_tex2_first = 0;
static GLuint g_tex2_batch[10] = {0};
static GLuint g_tex3_names[5] = {0};
static GLuint g_tex4_ids[4] = {0};
static GLuint g_tex5 = 0;
static GLuint g_tex6_singles[5] = {0};
static GLuint g_tex6_batch[5] = {0};
static GLuint g_tex7_a = 0;
static GLuint g_tex7_b = 0;
static GLuint g_tex9 = 0;
static GLuint g_tex12[1000] = {0};

/*
 * glGenTextures Robustness Test Suite
 * Hasan - OpenGL ES 2.0
 *
 * Bu test paketi, glGenTextures fonksiyonunun texture'a ozgu
 * sinir durumlarini ve davranislarini test eder.
 */

// ---------------------------------------------------------------
// TEST 1: glIsTexture Durum Zinciri
// Bir texture ID'sinin yasam dongusundeki her asamada
// glIsTexture'in dogru sonuc dondurup dondurmedigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex1);

	// Asama 1: Uretildi ama bind edilmedi -> GL_FALSE olmali
	GLboolean after_gen = glIsTexture(g_tex1);

	// Asama 2: Bind edildi -> GL_TRUE olmali
	glBindTexture(GL_TEXTURE_2D, g_tex1);
	GLboolean after_bind = glIsTexture(g_tex1);

	GLenum err = glGetError();

	if (!after_gen && after_bind && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case1, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case1, test_procedure,
		    "Durum zinciri bozuldu: gen=%d, bind=%d, err=0x%X",
		    after_gen, after_bind, err);
}


// ---------------------------------------------------------------
// TEST 2: Aktif Bind Sirasinda Uretim
// Bir texture aktif olarak bind edilmis iken yeni texture'lar
// uretmenin mevcut baglantiyı bozup bozmadigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex2_first);
	glBindTexture(GL_TEXTURE_2D, g_tex2_first);

	// Binding aktifken yeni texture'lar uret
	glGenTextures(10, g_tex2_batch);

	// Mevcut binding bozulmamis olmali
	GLint current_binding = -1;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_binding);

	GLenum err = glGetError();

	if ((GLuint)current_binding == g_tex2_first && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case2, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case2, test_procedure,
		    "Binding bozuldu: aktif=%d, beklenen=%u, err=0x%X",
		    current_binding, g_tex2_first, err);
}

// ---------------------------------------------------------------
// TEST 3: Ayni Diziye Ust Uste Yazma
// Ayni buffer'a art arda glGenTextures cagrildiginda eski
// isimlerin gecersiz kalip kalmadigini gozlemler.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	// Ilk uretim
	glGenTextures(5, g_tex3_names);

	// Ayni array'e ikinci uretim (eski isimler kaybolur)
	glGenTextures(5, g_tex3_names);

	GLenum err = glGetError();

	if (err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case3, test_procedure);
	else
		TEST_LOG_FAIL(test_case3, test_procedure,
			      "Ust uste yazmada hata: 0x%X", err);
}

// ---------------------------------------------------------------
// TEST 4: TEXTURE_2D ve CUBE_MAP Karisik Uretim-Bind Senaryosu
// Ayni batch'te uretilen ID'lerden bazilari 2D, bazilari CUBE_MAP
// olarak bind edildiginde birbirlerini etkileyip etkilemedigini test eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(4, g_tex4_ids);

	// 0 ve 1 -> TEXTURE_2D
	glBindTexture(GL_TEXTURE_2D, g_tex4_ids[0]);
	glBindTexture(GL_TEXTURE_2D, g_tex4_ids[1]);

	// 2 ve 3 -> TEXTURE_CUBE_MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex4_ids[2]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex4_ids[3]);

	GLenum err = glGetError();

	// Simdi 0'i CUBE_MAP'e bind etmeye calis -> GL_INVALID_OPERATION
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex4_ids[0]);
	GLenum cross_err = glGetError();

	if (err == GL_NO_ERROR && cross_err == GL_INVALID_OPERATION)
		TEST_LOG_SUCCESS(test_case4, test_procedure);
	else
		TEST_LOG_FAIL(test_case4, test_procedure,
			      "Capraz hedef hatasi: err=0x%X, cross_err=0x%X "
			      "(0x502 beklenir)",
			      err, cross_err);
}

// ---------------------------------------------------------------
// TEST 5: Hata Bayraginin Korunmasi
// Onceden var olan bir GL hata bayragi uzerinde glGenTextures
// cagrisinin hata durumunu temizleyip temizledigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	// Kasitli olarak bir hata uret
	glBindTexture(0xDEAD, 0); // GL_INVALID_ENUM uretir
	GLenum planted_err = glGetError();

	while (glGetError() != GL_NO_ERROR)
		;		  // onceki hatayi temizle
	glBindTexture(0xBEEF, 0); // yeni bir hata dik

	glGenTextures(1, &g_tex5);

	// Dikilen hata hala mevcut olmali
	GLenum surviving_err = glGetError();

	if (planted_err == GL_INVALID_ENUM && surviving_err == GL_INVALID_ENUM)
		TEST_LOG_SUCCESS(test_case5, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case5, test_procedure,
		    "Onceki hata korunamadi: planted=0x%X, surviving=0x%X",
		    planted_err, surviving_err);
}

// ---------------------------------------------------------------
// TEST 6: Tekli vs Toplu Uretim Tutarliligi
// 5 adet tek tek uretilmis ve 5 adet toplu uretilmis ID'nin
// birbirleriyle veya 0 ile cakismadigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	for (int i = 0; i < 5; i++) {
		glGenTextures(1, &g_tex6_singles[i]);
	}

	glGenTextures(5, g_tex6_batch);

	GLuint all[10];
	memcpy(all, g_tex6_singles, 5 * sizeof(GLuint));
	memcpy(all + 5, g_tex6_batch, 5 * sizeof(GLuint));

	int ok = 1;
	for (int i = 0; i < 10; i++) {
		if (all[i] == 0) {
			ok = 0;
			break;
		}
		for (int j = i + 1; j < 10; j++) {
			if (all[i] == all[j]) {
				ok = 0;
				break;
			}
		}
		if (!ok)
			break;
	}

	GLenum err = glGetError();

	if (ok && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case6, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case6, test_procedure,
		    "Tekli ve toplu uretim tutarsiz veya 0 ID: err=0x%X", err);
}

// ---------------------------------------------------------------
// TEST 7: TexImage2D ile Uretim Arasi Etkilesim
// Aktif bir texture'a glTexImage2D ile veri yuklenirken araya
// glGenTextures girmesinin VRAM/state durumunu bozmadigini test eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_007(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex7_a);
	glBindTexture(GL_TEXTURE_2D, g_tex7_a);

	GLubyte red_pixels[16] = {255, 0, 0, 255, 255, 0, 0, 255,
				  255, 0, 0, 255, 255, 0, 0, 255};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, red_pixels);
	GLenum err1 = glGetError();

	// Veri yuklendi, simdi araya gen giriyor
	glGenTextures(1, &g_tex7_b);

	// tex_a hala aktif bind olmali
	GLint bound_now = -1;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_now);

	GLenum err2 = glGetError();

	if (err1 == GL_NO_ERROR && err2 == GL_NO_ERROR &&
	    (GLuint)bound_now == g_tex7_a)
		TEST_LOG_SUCCESS(test_case7, test_procedure);
	else
		TEST_LOG_FAIL(test_case7, test_procedure,
			      "Etkilesim hatasi: err1=0x%X, err2=0x%X, "
			      "bound=%d (beklenen %u)",
			      err1, err2, bound_now, g_tex7_a);
}

// ---------------------------------------------------------------
// TEST 8: Negatif n Degeri (Spec Tanimi: GL_INVALID_VALUE)
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_008(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLuint dummy = 0;
	glGenTextures(-1, &dummy);
	GLenum err = glGetError();

	if (err == GL_INVALID_VALUE)
		TEST_LOG_SUCCESS(test_case8, test_procedure);
	else
		TEST_LOG_FAIL(test_case8, test_procedure,
			      "Beklenen GL_INVALID_VALUE (0x501), alinan: 0x%X",
			      err);
}

// ---------------------------------------------------------------
// TEST 9: Texture Parametre Durumu (Varsayilan Degerler)
// Yeni uretilip bind edilen bir texture'in varsayilan filtre
// parametrelerinin dogru olup olmadigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_009(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex9);
	glBindTexture(GL_TEXTURE_2D, g_tex9);

	GLint min_filter = 0, mag_filter = 0, wrap_s = 0, wrap_t = 0;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &mag_filter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap_s);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrap_t);

	GLenum err = glGetError();

	if (min_filter == GL_NEAREST_MIPMAP_LINEAR && mag_filter == GL_LINEAR &&
	    wrap_s == GL_REPEAT && wrap_t == GL_REPEAT && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case9, test_procedure);
	else
		TEST_LOG_FAIL(test_case9, test_procedure,
			      "Varsayilan parametreler hatali: min=0x%X, "
			      "mag=0x%X, s=0x%X, t=0x%X, err=0x%X",
			      min_filter, mag_filter, wrap_s, wrap_t, err);
}

// ---------------------------------------------------------------
// TEST 10: n < 0 ise Beklenen Hata: GL_INVALID_VALUE
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_010(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLuint texture = 0;
	glGenTextures(-1, &texture);
	GLenum err = glGetError();

	if (err == GL_INVALID_VALUE)
		TEST_LOG_SUCCESS(test_case10, test_procedure);
	else
		TEST_LOG_FAIL(test_case10, test_procedure,
			      "Beklenen GL_INVALID_VALUE (0x501), alinan: 0x%X",
			      err);
}

// ---------------------------------------------------------------
// TEST 11: textures = NULL, n > 0 (negative robustness)
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_011(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(5, NULL);
	GLenum err = glGetError();

	if (err == GL_NO_ERROR || err == GL_INVALID_VALUE ||
	    err == GL_INVALID_OPERATION)
		TEST_LOG_SUCCESS(test_case11, test_procedure);
	else
		TEST_LOG_FAIL(test_case11, test_procedure,
			      "Sistem cökmedi veya beklenmeyen hata: 0x%X",
			      err);
}

// ---------------------------------------------------------------
// TEST 12: Unique Names Test (Toplu Benzersizlik Kontrolü)
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_012(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	const GLsizei COUNT = 1000;
	glGenTextures(COUNT, g_tex12);

	int duplicate_or_zero = 0;

	for (int i = 0; i < COUNT; i++) {
		if (g_tex12[i] == 0) {
			duplicate_or_zero = 1;
			break;
		}
	}

	if (!duplicate_or_zero) {
		for (int i = 0; i < COUNT; i++) {
			for (int j = i + 1; j < COUNT; j++) {
				if (g_tex12[i] == g_tex12[j]) {
					duplicate_or_zero = 1;
					break;
				}
			}
			if (duplicate_or_zero)
				break;
		}
	}

	GLenum err = glGetError();
	if (!duplicate_or_zero && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case12, test_procedure);
	else
		TEST_LOG_FAIL(test_case12, test_procedure,
			      "Cift veya 0 ID saptandi veya hata: 0x%X", err);
}

/* Cleanup */
void Texturing_GenTextures_close(void) {
#ifdef __ubuntu__
	if (g_tex1)
		glDeleteTextures(1, &g_tex1);
	if (g_tex2_first)
		glDeleteTextures(1, &g_tex2_first);
	glDeleteTextures(10, g_tex2_batch);
	glDeleteTextures(5, g_tex3_names);
	glDeleteTextures(4, g_tex4_ids);
	if (g_tex5)
		glDeleteTextures(1, &g_tex5);
	glDeleteTextures(5, g_tex6_singles);
	glDeleteTextures(5, g_tex6_batch);
	if (g_tex7_a)
		glDeleteTextures(1, &g_tex7_a);
	if (g_tex7_b)
		glDeleteTextures(1, &g_tex7_b);
	if (g_tex9)
		glDeleteTextures(1, &g_tex9);
	glDeleteTextures(1000, g_tex12);
#endif
}
