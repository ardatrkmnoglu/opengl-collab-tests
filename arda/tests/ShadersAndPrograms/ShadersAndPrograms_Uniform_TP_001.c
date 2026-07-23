#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

/* ============================================================
 * TEST GRUBU: glUniform{1234}{if} / glUniform{1234}{if}v
 * ============================================================ */

/* ============================================================
 * glUniform — Tip Karmaşası (Type Confusion)
 * ============================================================
 *
 * Shader'da 'int' olarak tanımlanan bir değişkene 'float'
 * (glUniform1f) basmaya çalışırız. Sürücünün tip uyumsuzluğunu
 * tespit edip GL_INVALID_OPERATION üretmesi beklenir.
 * ============================================================ */
void ShadersAndPrograms_Uniform_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locInt = glGetUniformLocation(prog, "uInt");
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Tip Karmaşası: 'int' değişkene 'float' basmaya çalışıyoruz. */
	glUniform1f(locInt, 3.14f);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("Uniform", "ShadersAndPrograms_Uniform_TC_001",
			      "'int' değişkene 'float' atanması engellenmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS("Uniform", "ShadersAndPrograms_Uniform_TC_001");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glUniform — Geçersiz Lokasyon
 * ============================================================
 *
 * location = -1 ise sürücü veriyi sessizce reddetmeli
 * (GL_NO_ERROR). Geçersiz (fakat -1 olmayan) lokasyon ise
 * GL_INVALID_OPERATION fırlatmalıdır.
 * ============================================================ */
void ShadersAndPrograms_Uniform_TC_002(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Spec: location=-1 sessizce yutulmalı */
	glUniform1i(-1, 42);
	GLenum err1 = glGetError();

	if (!(err1 == GL_NO_ERROR)) {
		TEST_LOG_FAIL("Uniform", "ShadersAndPrograms_Uniform_TC_002",
			      "-1 lokasyonu sessizce yutulmalıydı (Spec kuralı)."
			      " Actual: 0x%04X", err1);
	} else {
		TEST_LOG_SUCCESS("Uniform", "ShadersAndPrograms_Uniform_TC_002[-1]");
	}

	/* Geçersiz ama -1 olmayan lokasyon: GL_INVALID_OPERATION beklenir */
	glUniform1i(0x7FFFFFFF, 42);
	GLenum err2 = glGetError();

	if (!(err2 == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("Uniform", "ShadersAndPrograms_Uniform_TC_002",
			      "Devasa geçersiz lokasyon ID'si reddedilmedi."
			      " Actual: 0x%04X", err2);
	} else {
		TEST_LOG_SUCCESS("Uniform", "ShadersAndPrograms_Uniform_TC_002[0x7FFFFFFF]");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glUniform4fv — Negatif Count
 * ============================================================
 *
 * Count (eleman sayısı) negatif olamaz. Sürücünün negatif
 * eleman sayısını (count = -1) reddetmesi beklenir.
 * ============================================================ */
void ShadersAndPrograms_Uniform_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locVec = glGetUniformLocation(prog, "uVec4Array");
	GLfloat data[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Count negatif olamaz */
	glUniform4fv(locVec, -1, data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL("Uniform", "ShadersAndPrograms_Uniform_TC_003",
			      "count=-1 GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS("Uniform", "ShadersAndPrograms_Uniform_TC_003");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glUniform4fv — Array Sınır İhlali (OOB)
 * ============================================================
 *
 * 3 elemanlık diziye 4 eleman (count = 4) kopyalamaya
 * çalışılarak sürücünün sınır ihlaline izin verip
 * vermediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_Uniform_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	/* Shader'da boyutu 3 olarak tanımlı */
	GLint locVec = glGetUniformLocation(prog, "uVec4Array");
	GLfloat data[16] = {0};
	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Sınır İhlali: 3 elemanlık diziye 4 eleman yazmaya çalış */
	glUniform4fv(locVec, 4, data);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("Uniform", "ShadersAndPrograms_Uniform_TC_004",
			      "Array OOB (count=4, boyut=3) engellenemedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS("Uniform", "ShadersAndPrograms_Uniform_TC_004");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glCompileShader — Söz Dizimi Hatası
 * ============================================================
 *
 * (RUN_EXTESTS'ten taşındı — rTest_shaderCompilerError)
 *
 * Syntax hatası içeren bir fragment shader'ın derleme
 * durumunun GL_FALSE olması gerektiği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_Uniform_TC_005(void) {
	/* Syntax hatası: eksik noktalı virgül */
	const char *bad = "void main() { gl_FragColor = vec4(1.0) }";
	GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s, 1, &bad, NULL);
	glCompileShader(s);

	GLint status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);

	if (!(status == GL_FALSE)) {
		TEST_LOG_FAIL("ShadersAndPrograms",
			      "ShadersAndPrograms_Uniform_TC_005",
			      "Hatalı shader başarıyla derlendi (derleme kabul edilmemeli)."
			      " status=%d", status);
	} else {
		TEST_LOG_SUCCESS("ShadersAndPrograms",
				 "ShadersAndPrograms_Uniform_TC_005");
	}

	glDeleteShader(s);
}

/* ============================================================
 * glCompileShader — Geçersiz Hassasiyet Niteleyicisi
 * ============================================================
 *
 * (RUN_EXTESTS'ten taşındı — rTest_invalidPrecision)
 *
 * "superhighp" gibi tanımsız bir precision qualifier içeren
 * shader'ın derlenmemesi gerektiği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_Uniform_TC_006(void) {
	const char *bad = "precision superhighp float; void main(){}";
	GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s, 1, &bad, NULL);
	glCompileShader(s);

	GLint status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);

	if (!(status == GL_FALSE)) {
		TEST_LOG_FAIL("ShadersAndPrograms",
			      "ShadersAndPrograms_Uniform_TC_006",
			      "Geçersiz precision qualifier ile shader derlendi."
			      " status=%d", status);
	} else {
		TEST_LOG_SUCCESS("ShadersAndPrograms",
				 "ShadersAndPrograms_Uniform_TC_006");
	}

	glDeleteShader(s);
}
