#include "../../../include/rtests.h"
#include "../../../include/helper.h"
#include "../../../include/macro.h"

static const char* test_procedure = "GS_GL20SC_VERT_DA_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_VERT_DA_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_VERT_DA_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_VERT_DA_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_VERT_DA_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_VERT_DA_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_VERT_DA_ROBUSTNESS_TC_006";


/* ============================================================
 * TEST GRUBU: glDrawArrays
 * ============================================================ */

/* ============================================================
 * glDrawArrays — Out of Bounds (Eksik Veri)
 * ============================================================
 *
 * Buffer boyutu az iken çok sayıda vertex çizmeye çalışarak
 * sürücünün sınırları taşıp çökmeyip GL_NO_ERROR veya tanımlı
 * hata (genellikle OOB çizimlerinde tanımsız davranış ama
 * SC driverları çökmeyi engellemeli) ürettiği doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_DA_ROBUSTNESS_TC_001(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLfloat data[] = { 0.0f, 0.0f, 0.0f }; // 1 vertex (x,y,z)
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Buffer'da 1 vertex var ama 100 tane çizmeye çalış */
	glDrawArrays(GL_TRIANGLES, 0, 100);
	GLenum err = glGetError();

	/* Çökmediyse başarılıdır. Hata döndürebilir veya döndürmeyebilir (undefined) */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);
}

/* ============================================================
 * glDrawArrays — Guard Page Attack
 * ============================================================
 *
 * (Mevcut testten uyarlandı)
 * Sayfa sonuna hizalanmış veriden sonra korumalı bir sayfa
 * koyarak glDrawArrays çağrılır. Sürücü sınır dışına çıkarsa
 * segfault üretir ve runner bunu yakalar.
 * ============================================================ */
void GS_GL20SC_VERT_DA_ROBUSTNESS_TC_002(void) {
	/* Çok platformlu (mmap gerektiren) test.
	   Burada sembolik uygulandı; eğer runner çöküşü yakalarsa
	   segfault olarak loglanır. Çökmezse geçer. */
	long page_size = sysconf(_SC_PAGESIZE);
	if (page_size == -1) {
		TEST_LOG_INFO("GS_GL20SC_VERT_DA_ROBUSTNESS_TC_002: sysconf başarısız.");
		return;
	}

	void *mem = mmap(NULL, page_size * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED) {
		TEST_LOG_INFO("GS_GL20SC_VERT_DA_ROBUSTNESS_TC_002: mmap başarısız.");
		return;
	}

	/* İkinci sayfayı korumalı yap (guard page) */
	mprotect((char*)mem + page_size, page_size, PROT_NONE);

	/* Veriyi ilk sayfanın tam sonuna hizala (1 vertex: 12 byte) */
	void *data_ptr = (char*)mem + page_size - 12;

	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, data_ptr);

	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* 2 vertex çizmeye çalış -> ikinci vertex guard page'e taşar */
	glDrawArrays(GL_POINTS, 0, 2);
	GLenum err = glGetError();

	/* Çökmezse başarılıdır */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);

	glDisableVertexAttribArray(0);
	munmap(mem, page_size * 2);
}

/* ============================================================
 * glDrawArrays — Eksik Attribute
 * ============================================================
 *
 * Shader programının kullandığı bir attribute enable edilmemiş
 * (veya veri bağlanmamış) iken çizim yapılmaya çalışılır.
 * ============================================================ */
void GS_GL20SC_VERT_DA_ROBUSTNESS_TC_003(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Attribute'lar bağlanmadan çizim komutu gönderilir */
	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum err = glGetError();

	/* Sürücü çökmemeli. GL_INVALID_OPERATION fırlatabilir veya yutabilir */
	TEST_LOG_SUCCESS(test_case_1, test_procedure);

}

/* ============================================================
 * glDrawArrays — NaN Verilerle Çizim
 * ============================================================
 *
 * VBO içerisine yerleştirilmiş NaN/Inf verileriyle çizim
 * yapılır. GPU'nun veya sürücünün matematik istisnası üretmeden
 * çökmeyi engellediği doğrulanır.
 * ============================================================ */
void GS_GL20SC_VERT_DA_ROBUSTNESS_TC_004(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLfloat data[] = { NAN, INFINITY, -INFINITY };
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	glDrawArrays(GL_POINTS, 0, 1);
	GLenum err = glGetError();

	TEST_LOG_SUCCESS(test_case_1, test_procedure);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);
}

/* ============================================================
 * glDrawArrays — Programsız Çizim
 * ============================================================
 *
 * glUseProgram(0) iken çizim komutu gönderilir.
 * OpenGL ES 2.0/SC 2.0'da GL_INVALID_OPERATION üretmelidir.
 * ============================================================ */
void GS_GL20SC_VERT_DA_ROBUSTNESS_TC_005(void) {
	glUseProgram(0);

	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	glDrawArrays(GL_POINTS, 0, 1);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "Programsız çizimde GL_INVALID_OPERATION bekleniyordu."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}
}

/* ============================================================
 * glDrawArrays — Negatif/Aşırı OOB
 * ============================================================
 *
 * Negatif count veya geçersiz first değeriyle çizim.
 * count = -1 (max unsigned int) olacağından GPU'yu çökertme riski vardır.
 * ============================================================ */
void GS_GL20SC_VERT_DA_ROBUSTNESS_TC_006(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	while (glGetError() != GL_NO_ERROR) { /* temizle */ }

	/* Count negatif olamaz: GL_INVALID_VALUE */
	glDrawArrays(GL_POINTS, 0, -1);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE)) {
		TEST_LOG_FAIL(test_case_1, test_procedure,
			      "count=-1 GL_INVALID_VALUE üretmedi."
			      " Actual: 0x%04X", err);
	} else {
		TEST_LOG_SUCCESS(test_case_1, test_procedure);
	}

}
