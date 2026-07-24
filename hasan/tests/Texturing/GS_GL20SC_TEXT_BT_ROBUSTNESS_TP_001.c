#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

static const char *test_case_1 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_007";
static const char *test_case_8 = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_008";

static const char *test_procedure = "GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001";

// Static texture handles for cleanup in close()
static GLuint g_tex1 = 0;
static GLuint g_tex2_random = 987654;
static GLuint g_tex3 = 0;
static GLuint g_tex4[2] = {0};
static GLuint g_tex5 = 0;
static GLuint g_tex6 = 0;
static GLuint g_tex7 = 0;

/* Forward declaration for close */
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_close(void);

/*
 * glBindTexture Robustness Test Suite
 * Hasan - OpenGL ES 2.0
 *
 * Bu test paketi, glBindTexture fonksiyonunun texture'lara ozgu
 * baglama mekanizmalarini (target kilitlenmesi, active texture izolasyonu,
 * name adoption vb.) sinamak uzere ozel olarak hazirlanmistir.
 */

// ---------------------------------------------------------------
// TEST 1: Gecersiz Hedef (Invalid Target Enum)
// TEXTURE_2D ve TEXTURE_CUBE_MAP disindaki hedeflere texture
// baglamaya calismanin GL_INVALID_ENUM uretip uretmedigi test edilir.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex1);

	// Buffer veya Framebuffer gibi alakasiz hedefler
	GLenum bad_targets[] = {GL_ARRAY_BUFFER, GL_FRAMEBUFFER, 0x0, 0xDEAD};
	int count = sizeof(bad_targets) / sizeof(bad_targets[0]);

	int all_invalid_enum = 1;

	for (int i = 0; i < count; i++) {
		glBindTexture(bad_targets[i], g_tex1);
		GLenum err = glGetError();
		if (err != GL_INVALID_ENUM) {
			all_invalid_enum = 0;
		}
	}

	if (all_invalid_enum)
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	else
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Gecersiz hedefler reddedilmedi");
}

// ---------------------------------------------------------------
// TEST 2: Isim Sahiplenme (Name Adoption / Generation on Bind)
// glGenTextures ile olusturulmamis rastgele bir ID'nin bind
// edilmesi. ES 2.0'da bu gecerli bir islemdir ve yeni bir texture olusturur.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	// Oncesinde bu ID texture degil
	GLboolean is_tex_before = glIsTexture(g_tex2_random);

	// Bind ederek ismi sahiplen (Name adoption)
	glBindTexture(GL_TEXTURE_2D, g_tex2_random);
	GLenum err = glGetError();

	// Bind edildikten sonra artik bir texture olmali
	GLboolean is_tex_after = glIsTexture(g_tex2_random);

	if (!is_tex_before && is_tex_after && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case_2, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case_2, test_procedure,
		    "Name adoption calismadi: before=%d, after=%d, err=0x%X",
		    is_tex_before, is_tex_after, err);
}

// ---------------------------------------------------------------
// TEST 3: Capraz Hedef Cakismasi (Cross-Target Conflict)
// Bir kere TEXTURE_2D olarak bind edilen (ve o hedefe kilitlenen)
// bir ID'nin TEXTURE_CUBE_MAP'e bind edilmeye calisilmasi durumu.
// GL_INVALID_OPERATION firlatilmalidir.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex3);

	// Ilk olarak 2D'ye bind et (Artik bu ID 2D'ye kilitlendi)
	glBindTexture(GL_TEXTURE_2D, g_tex3);
	GLenum err1 = glGetError();

	// Ayni ID'yi CUBE_MAP'e bind etmeye calis
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex3);
	GLenum err2 = glGetError();

	if (err1 == GL_NO_ERROR && err2 == GL_INVALID_OPERATION)
		TEST_LOG_SUCCESS(test_case_3, test_procedure);
	else
		TEST_LOG_FAIL(test_case_3, test_procedure,
			      "Capraz hedef hatasi: err1=0x%X, err2=0x%X "
			      "(0x502 beklenir)",
			      err1, err2);
}

// ---------------------------------------------------------------
// TEST 4: Aktif Texture Unit Izolasyonu (Active Texture Binding)
// Texture0 ve Texture1 unitlerine ayri ID'ler bind edildiginde
// birbirlerini etkileyip etkilemediklerini kontrol eder.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(2, g_tex4);

	// TEXTURE0'a tex[0]'i bind et
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_tex4[0]);

	// TEXTURE1'e tex[1]'i bind et
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_tex4[1]);

	// State'leri geri oku
	GLint bound_t1 = -1;
	glGetIntegerv(
	    GL_TEXTURE_BINDING_2D,
	    &bound_t1); // Su an aktif unit 1 oldugu icin tex[1] dondurmeli

	glActiveTexture(GL_TEXTURE0);
	GLint bound_t0 = -1;
	glGetIntegerv(GL_TEXTURE_BINDING_2D,
		      &bound_t0); // Simdi unit 0, tex[0] dondurmeli

	GLenum err = glGetError();

	if ((GLuint)bound_t0 == g_tex4[0] && (GLuint)bound_t1 == g_tex4[1] &&
	    err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case_4, test_procedure);
	else
		TEST_LOG_FAIL(test_case_4, test_procedure,
			      "Izolasyon bozuk: t0=%d (beklenen %u), t1=%d "
			      "(beklenen %u), err=0x%X",
			      bound_t0, g_tex4[0], bound_t1, g_tex4[1], err);
}

// ---------------------------------------------------------------
// TEST 5: Tekrarli Bind Edilmede Parametre Korunumu (State Persistence)
// Bir texture ozellestirildikten (GL_LINEAR yapildiktan) sonra tekrar
// bind edildiginde parametrelerinin korundugu doğrulanir.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex5);
	glBindTexture(GL_TEXTURE_2D, g_tex5);

	// Parametre degistir (varsayilandan farkli yapmak icin GL_LINEAR)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Ayni dokuyu tekrar bind et (Re-bind state persistence)
	glBindTexture(GL_TEXTURE_2D, g_tex5);

	// Parametreleri kontrol et: Tekrar bind edildiginde GL_LINEAR korunmus olmali
	GLint min_filter = 0;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);

	GLenum err = glGetError();

	if (min_filter == GL_LINEAR && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case_5, test_procedure);
	else
		TEST_LOG_FAIL(test_case_5, test_procedure,
			      "Re-bind durumunda parametre korunamadi: "
			      "min_filter=0x%X (0x2601 beklenir), err=0x%X",
			      min_filter, err);
}

// ---------------------------------------------------------------
// TEST 6: Varsayilan Texture'a (ID 0) Donus
// 0 ID'sini bind etmek, aktif texture'i deaktif etmeli ve varsayilan
// texture state'ine dondurmelidir. Sifir bind etmek hata uretmez.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex6);
	glBindTexture(GL_TEXTURE_2D, g_tex6);

	// Ozel bir ID'den 0'a donus
	glBindTexture(GL_TEXTURE_2D, 0);

	GLint current_bind = -1;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_bind);
	GLenum err = glGetError();

	if (current_bind == 0 && err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case_6, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case_6, test_procedure,
		    "Sifir bind hatasi: bind=%d (0 beklenir), err=0x%X",
		    current_bind, err);
}

// ---------------------------------------------------------------
// TEST 7: Ayni ID'nin Defalarca Bind Edilmesi (No-op Stresi)
// Performans kritik kodlarda ayni texture ust uste bind edilebilir.
// Surucunun state degisikligi olmadigini anlayip cokmeden calismasi test
// edilir.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_007(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex7);

	// Ayni texture 10,000 kez baglanir
	for (int i = 0; i < 10000; i++) {
		glBindTexture(GL_TEXTURE_2D, g_tex7);
	}

	GLenum err = glGetError();

	if (err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case_7, test_procedure);
	else
		TEST_LOG_FAIL(test_case_7, test_procedure,
			      "Tekrarli bind stresi sirasinda hata: 0x%X", err);
}

// ---------------------------------------------------------------
// TEST 8: Asiri Buyuk ID Isimleri Ile Sinir Zorlamasi
// Isim sahiplenme (name adoption) ozelliginin asiri buyuk int
// degerleriyle (UINT_MAX) kullanilmasi. Surucunun bunlari
// handle edebildigini veya reddebildigini izleriz.
// ---------------------------------------------------------------
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_008(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLuint extreme_ids[] = {
	    0xFFFFFFFF, // UINT_MAX
	    0x7FFFFFFF, // INT_MAX
	    0x80000000	// Sadece isaret biti
	};

	int all_ok = 1;

	for (int i = 0; i < 3; i++) {
		glBindTexture(GL_TEXTURE_2D, extreme_ids[i]);
		GLenum err = glGetError();

		GLint bound = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);

		if (err != GL_NO_ERROR || (GLuint)bound != extreme_ids[i]) {
			all_ok = 0;
		}
	}

	if (all_ok)
		TEST_LOG_SUCCESS(test_case_8, test_procedure);
	else
		TEST_LOG_FAIL(test_case_8, test_procedure,
			      "Bazi ucbirim ID'ler tam desteklenmiyor");
}

/* Initialization */
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_init(void) {
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_001();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_002();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_003();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_004();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_005();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_006();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_007();
	//CHECK_ERROR(test_procedure);
	GS_GL20SC_TEXT_BT_ROBUSTNESS_TC_008();
	//CHECK_ERROR(test_procedure);

	GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_draw(void) {
}

/* Cleanup */
void GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_close(void) {
#ifdef __ubuntu__
	if (g_tex1)
		glDeleteTextures(1, &g_tex1);
	if (g_tex2_random)
		glDeleteTextures(1, &g_tex2_random);
	if (g_tex3)
		glDeleteTextures(1, &g_tex3);
	if (g_tex4[0] || g_tex4[1])
		glDeleteTextures(2, g_tex4);
	if (g_tex5)
		glDeleteTextures(1, &g_tex5);
	if (g_tex6)
		glDeleteTextures(1, &g_tex6);
	if (g_tex7)
		glDeleteTextures(1, &g_tex7);
#endif
	//CHECK_ERROR(test_procedure);
}
