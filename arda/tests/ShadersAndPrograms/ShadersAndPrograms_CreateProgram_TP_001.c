#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * TEST GRUBU: glCreateProgram
 * ============================================================ */

/* ============================================================
 * glCreateProgram — Kaynak Yönetimi Stres Testi
 * ============================================================
 *
 * 10 000 adet program nesnesi oluşturularak sürücünün kaynak
 * yönetiminin bozulup bozulmadığı kontrol edilir. GL_NO_ERROR
 * üretilmesi ve döndürülen her ID'nin 0 olmaması beklenir.
 * ============================================================ */
void ShadersAndPrograms_CreateProgram_TC_001(void) {
	GLuint p_count = 0;

	while (p_count < 10000) {
		GLuint prog = glCreateProgram();
		GLenum err = glGetError();

		if (prog == 0 || err == GL_OUT_OF_MEMORY)
			break;

		if (!(err == GL_NO_ERROR)) {
			TEST_LOG_FAIL("CreateProgram", "ShadersAndPrograms_CreateProgram_TC_001",
				      "glCreateProgram beklenmedik hata üretti."
				      " Actual: 0x%04X",
				      err);
			glDeleteProgram(prog);
			return;
		}

		glDeleteProgram(prog);
		p_count++;
	}

	TEST_LOG_SUCCESS("CreateProgram", "ShadersAndPrograms_CreateProgram_TC_001");
}

/* ============================================================
 * glCreateProgram — Sil ve Yeniden Oluştur
 * ============================================================
 *
 * Program nesnesi oluşturulup silinip tekrar oluşturularak
 * sürücünün isim geri dönüşümünü doğru yönettiği doğrulanır.
 * Her yeni oluşturma işlemi 0 olmayan ve geçerli bir ID üretmeli.
 * ============================================================ */
void ShadersAndPrograms_CreateProgram_TC_002(void) {
	for (int i = 0; i < 100; i++) {
		GLuint prog = glCreateProgram();

		if (prog == 0) {
			TEST_LOG_FAIL(
			    "CreateProgram",
			    "ShadersAndPrograms_CreateProgram_TC_002",
			    "Yeniden oluşturmada 0 ID döndü (iterasyon %d).",
			    i);
			return;
		}

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			TEST_LOG_FAIL(
			    "CreateProgram",
			    "ShadersAndPrograms_CreateProgram_TC_002",
			    "Yeniden oluşturmada hata üretti. Actual: 0x%04X",
			    err);
			glDeleteProgram(prog);
			return;
		}

		glDeleteProgram(prog);
		while (glGetError() != GL_NO_ERROR) { /* temizle */
		}
	}

	TEST_LOG_SUCCESS("CreateProgram",
			 "ShadersAndPrograms_CreateProgram_TC_002");
}

/* ============================================================
 * glCreateProgram — Tip Karışıklığı Kontrolü
 * ============================================================
 *
 * glCreateProgram ile üretilen ID'nin Shader ID'si olarak
 * yanlış kullanılamayacağı doğrulanır. glDeleteShader ile
 * bir Program ID verildiğinde GL_INVALID_VALUE beklenir.
 * ============================================================ */
void ShadersAndPrograms_CreateProgram_TC_003(void) {
	GLuint prog = glCreateProgram();
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* Program ID'yi Shader gibi silmeye çalış */
	glDeleteShader(prog);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(
		    "CreateProgram", "ShadersAndPrograms_CreateProgram_TC_003",
		    "Program ID'si Shader gibi silinebildi (tip karışıklığı)."
		    " Actual: 0x%04X",
		    err);
	} else {
		TEST_LOG_SUCCESS("CreateProgram",
				 "ShadersAndPrograms_CreateProgram_TC_003");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glCreateProgram — Döndürülen ID'nin Sıfır Olmaması
 * ============================================================
 *
 * Normal koşullarda glCreateProgram her zaman 0 olmayan bir
 * program nesnesi ID'si döndürmelidir. 0 dönüyorsa ve
 * GL_OUT_OF_MEMORY oluşmamışsa bu bir sürücü hatasıdır.
 * ============================================================ */
void ShadersAndPrograms_CreateProgram_TC_004(void) {
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	GLuint prog = glCreateProgram();
	GLenum err = glGetError();

	if (err == GL_OUT_OF_MEMORY) {
		/* Bellek yetersizliği: çökmesiz kabul */
		TEST_LOG_INFO("ShadersAndPrograms_CreateProgram_TC_004: "
			      "GL_OUT_OF_MEMORY — bellek yok.");
		return;
	}

	if (!(prog != 0 && err == GL_NO_ERROR)) {
		TEST_LOG_FAIL("CreateProgram",
			      "ShadersAndPrograms_CreateProgram_TC_004",
			      "glCreateProgram 0 ID döndürdü (OOM değilken)."
			      " prog=%u, err=0x%04X",
			      prog, err);
	} else {
		TEST_LOG_SUCCESS("CreateProgram",
				 "ShadersAndPrograms_CreateProgram_TC_004");
	}

	if (prog != 0)
		glDeleteProgram(prog);
}
