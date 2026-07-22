#include "include/rtests.h"
#include <GLFW/glfw3.h>

int retcode = 0;

/**************************************/
/*********** Shader Sources ***********/
/**************************************/
static const char *vs_source = "attribute vec4 vPosition;\n"
			       "void main() {\n"
			       "    gl_Position = vPosition;\n"
			       "}\n";

static const char *fs_source = "precision mediump float;\n"
			       "void main() {\n"
			       "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
			       "}\n";

// dummy program function for draw/pipeline tests
GLuint createDummyProgram() {
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	glBindAttribLocation(prog, 0, "vPosition");

	glLinkProgram(prog);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return prog;
}

// create window context
int createContext(GLFWwindow** window) {
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	*window = glfwCreateWindow(640, 480, "test", NULL, NULL);
	if (!*window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(*window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		return -1;
	glGetError();
	return 0;
}

// destroy window context
void destroyContext(GLFWwindow** window) {
	glfwDestroyWindow(*window);
	glfwTerminate();
}

/**************************************/
/********** Helper Functions **********/
/**************************************/

static void cleanOpenGLState() {
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	while (glGetError() != GL_NO_ERROR);
}

static void runTest_old(void (*test_func)(), const char *name) {
	cleanOpenGLState();
	printf("[TEST] %s...\n", name);
	test_func();
	if (retcode == 0)
		printf("\x1b[32m[PASS]\x1b[0m %s passed.\n", name);
	retcode = 0;
}

static void runTest(void (*test_func)(), const char *name) {
	printf("[TEST] %s...\n", name);
	fflush(stdout);

	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "\x1b[31m[ERROR]\x1b[0m Fork failed for %s\n", name);
		return;
	}

	if (pid == 0) {
		GLFWwindow* w;
		createContext(&w);

		cleanOpenGLState();
		test_func();

		destroyContext(&w);
		exit(retcode);
	} else {
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			int exit_code = WEXITSTATUS(status);
			if (exit_code == 0) {
				printf("\x1b[32m[PASS]\x1b[0m %s passed.\n", name);
			}
		}
		else if (WIFSIGNALED(status)) {
			int sig = WTERMSIG(status);
			fprintf(stderr, "\x1b[31m[CRASH]\x1b[0m %s killed by signal %d! (Driver Vulnerability / Memory Corruption caught)\n", name, sig);
		}
	}
	retcode = 0;
}
#define runTest(func) runTest(func, #func)

/******************************************/
/*** Category-specific Helper Functions ***/
/******************************************/

/* ---------- glLineWidth ---------- */
static void resetState_lineWidth(void) {
	glLineWidth(1.0f);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved_lineWidth(GLfloat expected) {
	GLfloat actual;
	glGetFloatv(GL_LINE_WIDTH, &actual);
	if (actual != expected) {
		fprintf(stderr,
			"\x1b[33m[FAIL]\x1b[0m State bozuldu: beklenen %f, "
			"gercek %f\n",
			expected, actual);
		retcode = 1;
	}
}

/* ---------- glCullFace ---------- */
static void resetState_cullFace(void) {
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved_cullFace(GLint expected) {
	GLint actual;
	glGetIntegerv(GL_CULL_FACE_MODE, &actual);
	if (actual != expected) {
		fprintf(stderr,
			"\x1b[33m[FAIL]\x1b[0m State bozuldu: beklenen 0x%X, "
			"gercek 0x%X\n",
			expected, actual);
		retcode = 1;
	}
}

/* ---------- glFrontFace ---------- */
static void resetState_frontFace(void) {
	glFrontFace(GL_CCW);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved_frontFace(GLint expected) {
	GLint actual;
	glGetIntegerv(GL_FRONT_FACE, &actual);
	if (actual != expected) {
		fprintf(stderr,
			"\x1b[33m[FAIL]\x1b[0m State bozuldu: beklenen 0x%X, "
			"gercek 0x%X\n",
			expected, actual);
		retcode = 1;
	}
}

/* ---------- glEnable/Disable (Cull Face) ---------- */
static void resetState_cullFaceEnable(void) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	while (glGetError() != GL_NO_ERROR)
		;
}

/* ---------- glPolygonOffset ---------- */
static void resetState_polygonOffset(void) {
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_DEPTH_TEST);
	glPolygonOffset(0.0f, 0.0f);
	while (glGetError() != GL_NO_ERROR)
		;
}

/* ---------- glViewport ---------- */
static void resetState_viewport(void) {
	glViewport(0, 0, 640, 480);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved_viewport(GLint x, GLint y, GLsizei width,
					 GLsizei height) {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	if (viewport[0] != x || viewport[1] != y || viewport[2] != width ||
	    viewport[3] != height) {
		fprintf(stderr,
			"\x1b[33m[FAIL]\x1b[0m Viewport bozuldu: beklenen "
			"(%d,%d,%d,%d), gercek (%d,%d,%d,%d)\n",
			x, y, width, height, viewport[0], viewport[1],
			viewport[2], viewport[3]);
		retcode = 1;
	}
}

/* ---------- glDepthRange ---------- */
static void resetState_depthRange(void) {
	glDepthRange(0.0, 1.0);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved_depthRange(GLdouble expectedNear,
					   GLdouble expectedFar) {
	GLdouble depthRange[2];
	glGetDoublev(GL_DEPTH_RANGE, depthRange);
	if (fabs(depthRange[0] - expectedNear) > 0.000001 ||
	    fabs(depthRange[1] - expectedFar) > 0.000001) {
		fprintf(stderr,
			"\x1b[33m[FAIL]\x1b[0m Depth range bozuldu: beklenen "
			"(%lf,%lf), gercek (%lf,%lf)\n",
			expectedNear, expectedFar, depthRange[0],
			depthRange[1]);
		retcode = 1;
	}
}

/* ---------- glPixelStorei ---------- */
static void resetState_pixelStore(void) {
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved_pixelStore(GLenum pname, GLint expectedValue) {
	GLint value;
	glGetIntegerv(pname, &value);
	if (value != expectedValue) {
		fprintf(stderr,
			"\x1b[33m[FAIL]\x1b[0m PixelStore bozuldu: beklenen "
			"%d, gercek %d\n",
			expectedValue, value);
		retcode = 1;
	}
}

/***************************************/
/****** Robustness Test Functions ******/
/***************************************/

/****************************************/
/****** Shaders and Programs ******/
/****************************************/

/* ============================================================
 * glCreateProgram
 * ============================================================
 *
 * Program nesnesi olusturma stres testi. 10000 adet program
 * nesnesi olusturularak sürücünün kaynak yönetiminin
 * bozulup bozulmadigi kontrol edilir. Normalde sonsuz döngü
 * olmali ama çalistirmak için 10000 ile sinirlandirilmistir.
 * ============================================================ */
void rTest_CreateProgram() {
	GLuint p_count = 0;
	//	while (1) {
	while (p_count <
	       10000) { // normalde sonsuz döngü olmalı ama burada çalıştırmak
			// için testi 10000000 defa koşturuyoruz
		GLuint prog = glCreateProgram();
		GLenum err = glGetError();
		if (prog == 0 || err == GL_OUT_OF_MEMORY)
			break;

		EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
				"rTest_CreateProgram failed.");
		p_count++;
	}
}

/* ============================================================
 * glProgramBinary — Hizasiz Pointer
 * ============================================================
 *
 * glProgramBinary fonksiyonuna kasitli olarak hizasi bozulmus
 * (unaligned) bir bellek adresi verilerek sürücünün geçersiz
 * bellek erisiminde çökmeden hata üretip üretmedigi dogrulanir.
 * ============================================================ */
void rTest_ProgramBinary_unalignedPtr() {
	GLuint prog = glCreateProgram();

	void *valid_memblock = malloc(1024);
	const void *unaligned_ptr = (const void *)((char *)valid_memblock + 1);

	glProgramBinary(prog, 0x1234, unaligned_ptr, 100);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
			(err == GL_INVALID_ENUM || err == GL_INVALID_VALUE ||
			 err == GL_INVALID_OPERATION),
			"rTest_ProgramBinary_unalignedPtr failed.");
	free(valid_memblock);
}

/* ============================================================
 * glProgramBinary — Erisim Hakki Kaldirilmis Bellek
 * ============================================================
 *
 * mmap ile ayrilan bir bellek bölgesinin erisim haklari
 * mprotect(PROT_NONE) ile kaldirilir ve bu adres
 * glProgramBinary'ye verilir. Sürücünün bellek koruma
 * ihlaline karsi dayanikliligini test eder.
 * ============================================================ */
void rTest_ProgramBinary_memRevoke() {
	GLuint prog = glCreateProgram();

	size_t page_size = sysconf(_SC_PAGESIZE);
	void *mapped_memory = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
				   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	mprotect(mapped_memory, page_size, PROT_NONE);

	glProgramBinary(prog, 0x1234, mapped_memory, 1024);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
			(err == GL_INVALID_ENUM || err == GL_INVALID_VALUE ||
			 err == GL_INVALID_OPERATION),
			"rTest_ProgramBinary_memRevoke failed.");
	munmap(mapped_memory, page_size);
}

/* ============================================================
 * glProgramBinary — Asiri Yükleme (Overload)
 * ============================================================
 *
 * Çok büyük bir uzunluk degeri (2147483631) ve çöp veri
 * ile glProgramBinary çagirilarak sürücünün bu geçersiz
 * binary'yi kabul edip etmedigi dogrulanir. Link durumunun
 * GL_FALSE olmasi beklenir.
 * ============================================================ */
void rTest_ProgramBinary_overload() {
	GLuint prog = glCreateProgram();

	GLint num_formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &num_formats);

	GLenum valid_format = 0x1234;
	if (num_formats > 0)
		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS,
			      (GLint *)&valid_format);

	char buf[4] = {0xDE, 0xAD, 0xBE, 0xEF};
	GLsizei overload_len = 2147483631;

	glProgramBinary(prog, valid_format, buf, overload_len);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		EXPECT_GL_ERROR(err,
				(err == GL_INVALID_ENUM ||
				 err == GL_INVALID_VALUE ||
				 err == GL_INVALID_OPERATION),
				"rTest_ProgramBinary_overload failed.");
	}

	GLint link_status = GL_TRUE;
	glGetProgramiv(prog, GL_LINK_STATUS, &link_status);

	EXPECT_GL_ERROR(link_status, (link_status == GL_FALSE),
			"rTest_ProgramBinary_overload failed.\n"
			"\x1b[31mCRITICAL:\x1b[0m Driver accepted the garbage "
			"data as a valid Shader Program.");
}

/* ============================================================
 * glUseProgram — Geçersiz ID
 * ============================================================
 *
 * Hiç var olmamis bir program ID'si (0xdeadbeef) ile
 * glUseProgram çagirilarak sürücünün hayalet ID'yi
 * reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_UseProgram_invalidID() {
	GLuint ghost_id = 0xdeadbeef;
	glUseProgram(ghost_id);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_UseProgram_invalidID failed.\n"
			"\x1b[31mCRITICAL:\x1b[0m Driver accepted the "
			"ghost ID (has never existed) as a valid program ID.");
}

/* ============================================================
 * glUseProgram — Tip Karmasasi (Type Confusion)
 * ============================================================
 *
 * Bir Shader nesnesi ID'si ile glUseProgram çagirilarak
 * sürücünün Shader ve Program nesnelerini ayirt edip
 * edemedigi dogrulanir. GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void rTest_UseProgram_typeConfusion() {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glUseProgram(shader);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_UseProgram_typeConfusion failed.");

	glDeleteShader(shader);
}

/* ============================================================
 * glGetAttribLocation — NULL Pointer
 * ============================================================
 *
 * glGetAttribLocation fonksiyonuna NULL pointer verilerek
 * sürücünün çökmeden -1 dönüp dönmedigi dogrulanir.
 * ============================================================ */
void rTest_GetAttribLocation_nullPtr() {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, NULL);

	EXPECT_GL_ERROR(loc, (loc == -1),
			"rTest_GetAttribLocation_nullPtr failed.");
}

/* ============================================================
 * glGetAttribLocation — Rezerve Degisken
 * ============================================================
 *
 * OpenGL'e ait rezerve edilmis bir degisken adi ("gl_Position")
 * ile glGetAttribLocation çagirilarak sürücünün bu ismi
 * reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_GetAttribLocation_reservedVariable() {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, "gl_Position");

	EXPECT_GL_ERROR(loc, (loc == -1),
			"rTest_GetAttribLocation_reservedVariable failed.");
}

/****************************************/
/************* Uniforms *************/
/****************************************/

/* ============================================================
 * glGetUniformLocation — NULL Pointer
 * ============================================================
 *
 * NULL pointer saldirisi: Akilli bir sürücü çökmeden
 * -1 dönmelidir.
 * ============================================================ */
void rTest_GetUniformLocation_nullPtr(void) {
	GLuint prog = createDummyProgram();

	// NULL pointer saldırısı: Akıllı bir sürücü çökmeden -1 dönmelidir.
	GLint loc = glGetUniformLocation(prog, NULL);

	EXPECT_GL_ERROR(loc, (loc == -1),
			"rTest_GetUniformLocation_nullPtr failed.\n"
			"Sürücü NULL pointer yediğinde -1 dönmedi.");
}

/* ============================================================
 * glGetUniformLocation — Yasakli Ön Ek
 * ============================================================
 *
 * "gl_" ön eki spesifikasyon geregi OpenGL'e aittir.
 * Sürücünün yasakli ön ekli uniform yerini ifsa edip
 * etmedigi dogrulanir.
 * ============================================================ */
void rTest_GetUniformLocation_reservedPrefix(void) {
	GLuint prog = createDummyProgram();

	// Yasaklı isim saldırısı: "gl_" ön eki spesifikasyon gereği OpenGL'e
	// aittir.
	GLint loc = glGetUniformLocation(prog, "gl_DepthRange");

	EXPECT_GL_ERROR(
	    loc, (loc == -1),
	    "rTest_GetUniformLocation_reservedPrefix failed.\n"
	    "Sürücü yasaklı 'gl_' ön ekine sahip uniform yerini ifşa etti.");
}

/* ============================================================
 * glUniform — Tip Karmasasi (Type Confusion)
 * ============================================================
 *
 * Shader'da 'int' olarak tanimlanan bir degiskene 'float'
 * (glUniform1f) basmaya çalisiriz. Sürücünün tip uyumsuzlugunu
 * tespit edip GL_INVALID_OPERATION üretmesi beklenir.
 * ============================================================ */
void rTest_Uniform_typeConfusion(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locInt = glGetUniformLocation(prog, "uInt");

	// Tip Karmaşası: Shader'da 'int' olarak tanımlanan bir değişkene
	// 'float' (glUniform1f) basmaya çalışıyoruz.
	glUniform1f(locInt, 3.14f);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_Uniform_typeConfusion failed.\n"
			"Sürücü 'int' değişkene 'float' atanmasını engellemedi "
			"(Type Confusion).");
}

/* ============================================================
 * glUniform — Geçersiz Lokasyon
 * ============================================================
 *
 * location = -1 ise sürücü veriyi sessizce reddetmeli
 * (GL_NO_ERROR). Geçersiz (fakat -1 olmayan) lokasyon ise
 * GL_INVALID_OPERATION firlatmalidir.
 * ============================================================ */
void rTest_Uniform_invalidLocation(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	// Spesifikasyon Tuzağı:
	// location = -1 ise sürücü veriyi sessizce reddetmeli (GL_NO_ERROR).
	glUniform1i(-1, 42);
	GLenum err1 = glGetError();
	EXPECT_GL_ERROR(err1, (err1 == GL_NO_ERROR),
			"rTest_Uniform_invalidLocation failed.\n"
			"-1 lokasyonu sessizce yutulmalıydı (Spec Kuralı).");

	// Geçersiz (fakat -1 olmayan) lokasyon ise GL_INVALID_OPERATION
	// fırlatmalıdır.
	glUniform1i(0x7FFFFFFF, 42);
	GLenum err2 = glGetError();

	EXPECT_GL_ERROR(err2, (err2 == GL_INVALID_OPERATION),
			"rTest_Uniform_invalidLocation failed.\n"
			"Sürücü tamamen geçersiz ve devasa bir lokasyon "
			"ID'sini reddetmedi.");
}

/* ============================================================
 * glUniform4fv — Negatif Count
 * ============================================================
 *
 * Count (eleman sayisi) negatif olamaz. Sürücünün negatif
 * eleman sayisini (count = -1) reddetmesi beklenir.
 * ============================================================ */
void rTest_Uniformv_negativeCount(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locVec = glGetUniformLocation(prog, "uVec4Array");
	GLfloat data[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	// Count (Eleman sayısı) negatif olamaz.
	glUniform4fv(locVec, -1, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "rTest_Uniformv_negativeCount failed.\n"
	    "Sürücü negatif eleman sayısını (count = -1) kabul etti.");
}

/* ============================================================
 * glUniform4fv — Array Sinir Ihlali (OOB)
 * ============================================================
 *
 * 3 elemanlik diziye 4 eleman (count = 4) kopyalamaya
 * çalisilarak sürücünün sinir ihlaline izin verip
 * vermedigi dogrulanir.
 * ============================================================ */
void rTest_Uniformv_arrayOutOfBounds(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locVec = glGetUniformLocation(
	    prog, "uVec4Array"); // Shader'da boyutu 3 olarak tanımlı.
	GLfloat data[16] = {0};

	// Sınır İhlali (Out of Bounds): 3 elemanlık diziye 4 eleman (count = 4)
	// kopyalamaya çalışıyoruz.
	glUniform4fv(locVec, 4, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_Uniformv_arrayOutOfBounds failed.\n"
			"Sürücü array sınırlarını aşan (OOB) bir uniform "
			"kopyalamasına izin verdi.");
}

/* ============================================================
 * glUniformMatrix4fv — Geçersiz Transpose
 * ============================================================
 *
 * ES 2.0 ve SC 2.0 kurali: Transpose parametresi her zaman
 * GL_FALSE olmak ZORUNDADIR. GL_TRUE verilerek sürücünün
 * bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_UniformMatrix_invalidTranspose(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locMat4 = glGetUniformLocation(prog, "uMat4");
	GLfloat mat[16] = {0};

	// ES 2.0 ve SC 2.0 kuralı: Transpose parametresi her zaman GL_FALSE
	// olmak ZORUNDADIR.
	glUniformMatrix4fv(locMat4, 1, GL_TRUE, mat);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_UniformMatrix_invalidTranspose failed.\n"
			"Sürücü GL_TRUE transpose bayrağını kabul etti "
			"(Spesifikasyon ihlali).");
}

/* ============================================================
 * glUniformMatrix — Tip Uyumsuzlugu
 * ============================================================
 *
 * Lokasyonu Mat3 (3x3 Matris) olarak çekip, veriyi Mat4
 * fonksiyonuyla basmaya çalisarak sürücünün tip kontrolü
 * yapip yapmadigini test eder.
 * ============================================================ */
void rTest_UniformMatrix_typeMismatch(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	// Lokasyonu bir Mat3 (3x3 Matris) olarak çekiyoruz.
	GLint locMat3 = glGetUniformLocation(prog, "uMat3");
	GLfloat mat[16] = {0};

	// SABOTAJ: Lokasyon Mat3 iken, biz veriyi Mat4 fonskiyonuyla basmaya
	// çalışıyoruz.
	glUniformMatrix4fv(locMat3, 1, GL_FALSE, mat);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_OPERATION),
	    "rTest_UniformMatrix_typeMismatch failed.\n"
	    "Sürücü Mat3 lokasyonuna Mat4 verisi kopyalamaya çalıştı.");
}

/****************************************/
/******** Vertex Attributes ********/
/****************************************/

/* ============================================================
 * glGetVertexAttribfv — Geçersiz Enum
 * ============================================================
 *
 * 0xDEADBEEF adinda bir parametre (pname) yoktur. Sürücünün
 * geçersiz bir parametre sorgusuna GL_INVALID_ENUM ile
 * yanit vermesi beklenir.
 * ============================================================ */
void rTest_GetVertexAttrib_invalidEnum(void) {
	GLfloat params[4];
	// 0xDEADBEEF adında bir parametre (pname) yoktur.
	glGetVertexAttribfv(0, 0xDEADBEEF, params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_ENUM),
	    "rTest_GetVertexAttrib_invalidEnum failed.\n"
	    "Geçersiz bir parametre (pname) sorgusuna hata dönülmedi.");
}

/* ============================================================
 * glGetVertexAttribiv — Indeks Sinir Ihlali
 * ============================================================
 *
 * Donanim sinirina (GL_MAX_VERTEX_ATTRIBS) esit bir indeks
 * ile sorgu yapilarak sürücünün sinir kontrolü dogrulanir.
 * ============================================================ */
void rTest_GetVertexAttrib_indexOutOfBounds(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

	GLint params[4];
	// Sınır İhlali: İndeksler 0 ile (max_attribs - 1) arasında olmalıdır.
	glGetVertexAttribiv(max_attribs, GL_VERTEX_ATTRIB_ARRAY_ENABLED,
			    params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_GetVertexAttrib_indexOutOfBounds failed.\n"
			"Sürücü donanım sınırının dışındaki bir Attribute "
			"indeksini sorgulattı.");
}

/* ============================================================
 * glGetVertexAttribPointerv — Geçersiz Enum
 * ============================================================
 *
 * Bu fonksiyon yalnizca GL_VERTEX_ATTRIB_ARRAY_POINTER
 * Enum'ini kabul eder. GL_FLOAT gibi geçersiz bir Enum
 * verilerek sürücünün bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_GetVertexAttribPointer_invalidEnum(void) {
	void *ptr = NULL;
	// Bu fonksiyon yalnızca ve yalnızca GL_VERTEX_ATTRIB_ARRAY_POINTER
	// Enum'ını kabul eder.
	glGetVertexAttribPointerv(0, GL_FLOAT, &ptr);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_GetVertexAttribPointer_invalidEnum failed.\n"
			"GL_VERTEX_ATTRIB_ARRAY_POINTER harici bir Enum olarak "
			"kabul edildi.");
}

/* ============================================================
 * glGetnUniformfv — Negatif Buffer Boyutu
 * ============================================================
 *
 * BufSize negatif olamaz. Bu, KHR_robustness eklentisinin
 * temel kuralidir. Sürücünün negatif bir buffer boyutunu
 * reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_GetnUniform_negativeBufSize(void) {
	GLuint prog = createDummyProgram();
	GLint locFloat = glGetUniformLocation(prog, "uFloat");

	GLfloat data[4];
	// BufSize negatif olamaz. Bu, KHR_robustness eklentisinin temel
	// kuralıdır.
	glGetnUniformfv(prog, locFloat, -1, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "rTest_GetnUniform_negativeBufSize failed.\n"
	    "Sürücü negatif bir buffer boyutu (bufSize) kabul etti.");
}

/* ============================================================
 * glGetnUniformfv — Geçersiz Program
 * ============================================================
 *
 * 0, hiçbir zaman geçerli bir program nesnesi degildir.
 * Sürücünün geçersiz bir Program ID'si üzerinden Uniform
 * sorgulatip sorgulatmadigi dogrulanir.
 * ============================================================ */
void rTest_GetnUniform_invalidProgram(void) {
	// 0, hiçbir zaman geçerli bir program nesnesi değildir.
	GLfloat data[4];
	glGetnUniformfv(0, 0, sizeof(data), data);
	GLenum err = glGetError();

	// Geçersiz (hiç üretilmemiş) bir obje olduğu için GL_INVALID_VALUE
	// dönmelidir. Eğer obje var ama Program değilse (örn Shader ise)
	// GL_INVALID_OPERATION döner.
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION),
	    "rTest_GetnUniform_invalidProgram failed.\n"
	    "Sürücü geçersiz bir Program ID'si üzerinden Uniform sorgulattı.");
}

/* ============================================================
 * glGetProgramiv — Geçersiz Enum
 * ============================================================
 *
 * Geçersiz bir parametre (pname = 0xDEADBEEF) ile
 * glGetProgramiv çagirilarak sürücünün bunu GL_INVALID_ENUM
 * ile reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_GetProgramiv_invalidEnum(void) {
	GLuint prog = glCreateProgram();
	GLint params = 0;

	glGetProgramiv(prog, 0xDEADBEEF, &params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_GetProgramiv_invalidEnum failed.\n"
			"Geçersiz bir parametre (pname) kabul edildi.");
}

/* ============================================================
 * glGetProgramiv — Tip Karmasasi (Type Confusion)
 * ============================================================
 *
 * Sürücünün Shader nesnesini Program gibi okumaya çalisip
 * çalismayacagini siniyoruz. GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void rTest_GetProgramiv_typeConfusion(void) {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	GLint params = 0;

	// Tip Karmaşası: Sürücünün Shader nesnesini Program gibi okumaya
	// çalışıp çalışmayacağını sınıyoruz.
	glGetProgramiv(shader, GL_LINK_STATUS, &params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_GetProgramiv_typeConfusion failed.\n"
			"Sürücü bir Shader nesnesine Program muamelesi yaptı.");

	glDeleteShader(shader);
}

/* ============================================================
 * glVertexAttrib1f — Indeks Sinir Ihlali
 * ============================================================
 *
 * Donanimin limitine (max_attribs) veri yazmaya çalisarak
 * sürücünün sinir disindaki bir Attribute indeksini reddetmesi
 * beklenir.
 * ============================================================ */
void rTest_VertexAttrib_indexOutOfBounds(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

	// Sınır İhlali: İndeksler 0 ile (max_attribs - 1) arasında olmalıdır.
	// Donanımın limitine (max_attribs) veri yazmaya çalışıyoruz.
	glVertexAttrib1f(max_attribs, 1.0f);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_VertexAttrib_indexOutOfBounds failed.\n"
			"Sürücü sınır dışı bir Attribute indeksini "
			"(max_attribs) reddetmedi.");
}

/* ============================================================
 * glVertexAttrib4fv — Özel Float Degerleri
 * ============================================================
 *
 * Kasitli olarak zehirli kayan nokta (float) degerleri
 * (NaN, Infinity) gönderilerek sürücünün bu degerleri
 * yediginde çökmek yerine güvenlice kabul etmesi
 * veya tanimli bir hata üretmesi beklenir.
 * ============================================================ */
void rTest_VertexAttribv_specialFloats(void) {
	// Kasıtlı olarak zehirli kayan nokta (float) değerleri gönderiyoruz.
	GLfloat data[4] = {NAN, INFINITY, -INFINITY, 0.0f};

	// Sürücü bu değerleri yediğinde çökmek yerine güvenlice kabul etmeli
	// veya kendi iç mimarisine göre bir hata üretmelidir.
	glVertexAttrib4fv(0, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_NO_ERROR || err == GL_INVALID_VALUE),
	    "rTest_VertexAttribv_specialFloats failed.\n"
	    "Sürücü NaN/Inf değerlerinde tanımsız bir hata üretti.");
}

/* ============================================================
 * glVertexAttribPointer — Geçersiz Type
 * ============================================================
 *
 * Type parametresi GL_FLOAT, GL_BYTE vb. olmalidir.
 * Alakasiz bir Enum (GL_TEXTURE_2D) verilerek sürücünün
 * bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_VertexAttribPointer_invalidType(void) {
	// Type parametresi GL_FLOAT, GL_BYTE vb. olmalıdır.
	// Biz gidip alakasız bir Enum (GL_TEXTURE_2D) veriyoruz.
	glVertexAttribPointer(0, 3, GL_TEXTURE_2D, GL_FALSE, 0, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_VertexAttribPointer_invalidType failed.\n"
			"Geçersiz bir veri tipi (GL_TEXTURE_2D) kabul edildi.");
}

/* ============================================================
 * glVertexAttribPointer — Geçersiz Size
 * ============================================================
 *
 * Size parametresi yalnizca 1, 2, 3 veya 4 olabilir.
 * 5 elemanlı bir vektör geçersizdir.
 * ============================================================ */
void rTest_VertexAttribPointer_invalidSize(void) {
	// Size parametresi BİR TEK 1, 2, 3 veya 4 olabilir!
	// 5 elemanlı bir vektör olamaz.
	glVertexAttribPointer(0, 5, GL_FLOAT, GL_FALSE, 0, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "rTest_VertexAttribPointer_invalidSize failed.\n"
	    "Sürücü 'size = 5' olan geçersiz bir boyutu kabul etti.");
}

/* ============================================================
 * glEnable/DisableVertexAttribArray — Sinir Ihlali
 * ============================================================
 *
 * Üst sinir ihlali ve çok büyük/negatif indeks (0xFFFFFFFF)
 * ile sürücünün sinir kontrolü dogrulanir.
 * ============================================================ */
void rTest_EnableDisableVertexAttrib_bounds(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

	// Üst sınır ihlali (GL_INVALID_VALUE bekliyoruz)
	glEnableVertexAttribArray(max_attribs);
	GLenum err1 = glGetError();
	EXPECT_GL_ERROR(
	    err1, (err1 == GL_INVALID_VALUE),
	    "rTest_EnableDisableVertexAttrib_bounds failed.\n"
	    "glEnableVertexAttribArray: Sınır dışı indeks reddedilmedi.");

	// Negatif veya çok büyük sınır ihlali (0xFFFFFFFF -> UINT_MAX)
	glDisableVertexAttribArray(0xFFFFFFFF);
	GLenum err2 = glGetError();
	EXPECT_GL_ERROR(err2, (err2 == GL_INVALID_VALUE),
			"rTest_EnableDisableVertexAttrib_bounds failed.\n"
			"glDisableVertexAttribArray: Çok büyük/negatif indeks "
			"(0xFFFFFFFF) reddedilmedi.");
}

/****************************************/
/*********** Draw Calls ***********/
/****************************************/

/* ============================================================
 * glDrawArrays — Sinir Disi Çizim (OOB)
 * ============================================================
 *
 * 3 vertex'lik bir veri ile 1.000.000 vertex çizmeye
 * çalisilarak sürücünün sinir kontrolü dogrulanir.
 * ============================================================ */
void rTest_DrawArrays_outOfBounds() {
	GLfloat vertices[] = {-0.5, -0.5, 0.0, 0.5, -0.5, 0.0, 0.0, 0.5, 0.0};

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 1000000);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION),
	    "rTest_DrawArrays_outOfBounds failed.");

	glDisableVertexAttribArray(0);
}

/* ============================================================
 * glDrawArrays — Guard Page Saldirisi
 * ============================================================
 *
 * mmap ile iki sayfa bellek ayrilir, ikinci sayfa PROT_NONE
 * yapilir (guard page). Vertex verisi guard page sinirinda
 * konumlandirilir ve 300.000.000 vertex çizmeye çalisilarak
 * sürücünün bellek koruma ihlaline karsi dayanikliligi
 * dogrulanir.
 * ============================================================ */
void rTest_DrawArrays_guardPageAttack(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	size_t page_size = sysconf(_SC_PAGESIZE);

	void *memory = mmap(NULL, page_size * 2, PROT_READ | PROT_WRITE,
			    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	void *guard_page = (char *)memory + page_size;
	mprotect(guard_page, page_size, PROT_NONE);

	GLfloat *edge_data =
	    (GLfloat *)((char *)guard_page - (9 * sizeof(GLfloat)));

	edge_data[0] = 0.0f;
	edge_data[1] = 1.0f;
	edge_data[2] = 0.0f;
	edge_data[3] = -1.0f;
	edge_data[4] = -1.0f;
	edge_data[5] = 0.0f;
	edge_data[6] = 1.0f;
	edge_data[7] = -1.0f;
	edge_data[8] = 0.0f;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, edge_data);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 300000000);
	glFinish();

	GLubyte px[4];
	glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, px);

	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_OPERATION || err == GL_INVALID_VALUE),
	    "rTest_DrawArrays_guardPageAttack failed.");

	glDisableVertexAttribArray(0);
	munmap(memory, page_size * 2);
}

/* ============================================================
 * glDrawElements — Geçersiz Indeks Tipi
 * ============================================================
 *
 * 'type' parametresi yalnizca GL_UNSIGNED_BYTE,
 * GL_UNSIGNED_SHORT veya GL_UNSIGNED_INT olabilir.
 * GL_FLOAT türünde indeks okumasini isteyerek sürücünün
 * bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_DrawElements_invalidType(void) {
	// 'type' parametresi indeks dizisinin tipidir.
	// Yalnızca GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT veya GL_UNSIGNED_INT
	// olabilir. Biz float türünde bir indeks okumasını istiyoruz.
	glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_DrawElements_invalidType failed.\n"
			"İndeks verisi olarak GL_FLOAT kabul edildi.");
}

/* ============================================================
 * glDrawRangeElements — Geçersiz Aralik
 * ============================================================
 *
 * 'end' degeri kesinlikle 'start' degerinden küçük olamaz.
 * start = 10, end = 5 göndererek mantiksal bir imkansizlik
 * yaratilir.
 * ============================================================ */
void rTest_DrawRangeElements_invalidRange(void) {
	// glDrawRangeElements(mode, start, end, count, type, indices)
	// KURAL: 'end' değeri kesinlikle 'start' değerinden KÜÇÜK OLAMAZ!
	// start = 10, end = 5 göndererek mantıksal bir imkansızlık yaratıyoruz.
	glDrawRangeElements(GL_TRIANGLES, 10, 5, 3, GL_UNSIGNED_SHORT, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "rTest_DrawRangeElements_invalidRange failed.\n"
	    "Sürücü 'end < start' olan mantıksız bir aralığı kabul etti.");
}

/****************************************/
/****** Framebuffer Operations ******/
/****************************************/

/* ============================================================
 * glColorMask — Boolean Dönüsüm
 * ============================================================
 *
 * Kasitli olarak 1 ve 0 yerine 'tuhaf' sayilar (0xFF, 0x02,
 * 0x80) gönderilerek OpenGL kuralina göre 0 disindaki her
 * seyin GL_TRUE kabul edilmesi dogrulanir.
 * ============================================================ */
void rTest_ColorMask_booleanConversion(void) {
	// Kasıtlı olarak 1 ve 0 yerine 'tuhaf' sayılar (0xFF, 0x02, 0x80)
	// gönderiyoruz. OpenGL kurallarına göre 0 dışındaki her şey GL_TRUE
	// kabul edilmelidir.
	glColorMask(0xFF, 0x02, 0x00, 0x80);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"rTest_ColorMask_booleanConversion failed.\n"
			"Standart dışı değerler girildiğinde hata üretildi "
			"(Spesifikasyona aykırı).");

	// Sürücü gerçekten bu değerleri GL_TRUE ve GL_FALSE olarak kırptı mı?
	GLboolean mask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);

	EXPECT_GL_ERROR(mask[0],
			(mask[0] == GL_TRUE && mask[1] == GL_TRUE &&
			 mask[2] == GL_FALSE && mask[3] == GL_TRUE),
			"rTest_ColorMask_booleanConversion failed.\n"
			"Sürücü '!= 0' kuralını ihlal etti veya değerleri "
			"doğru cast etmedi.");
}

/* ============================================================
 * glStencilMaskSeparate — Geçersiz Enum
 * ============================================================
 *
 * 'face' parametresi yalnizca GL_FRONT, GL_BACK veya
 * GL_FRONT_AND_BACK olabilir. GL_TEXTURE_2D verilerek
 * sürücünün bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_StencilMaskSeparate_invalidEnum(void) {
	// 'face' parametresi YALNIZCA GL_FRONT, GL_BACK veya GL_FRONT_AND_BACK
	// olabilir.
	glStencilMaskSeparate(GL_TEXTURE_2D, 0xFFFFFFFF);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_ENUM),
	    "rTest_StencilMaskSeparate_invalidEnum failed.\n"
	    "Geçersiz face parametresi (GL_TEXTURE_2D) reddedilmedi.");
}

/* ============================================================
 * glClear — Geçersiz Bit Maskesi
 * ============================================================
 *
 * glClear yalnizca COLOR, DEPTH ve STENCIL bitlerinin
 * mantiksal OR kombinasyonunu kabul eder. Tüm bitleri '1'
 * yaparak (0xFFFFFFFF) ve kirletilmis bir maske göndererek
 * sürücünün bunu reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_Clear_invalidBitmask(void) {
	// glClear yalnızca COLOR, DEPTH ve STENCIL bitlerinin mantıksal OR
	// (Veya) kombinasyonunu kabul eder. Tüm bitleri '1' yaparak
	// (0xFFFFFFFF) sürücüye yasaklı bitler yolluyoruz.
	glClear(0xFFFFFFFF);
	GLenum err1 = glGetError();

	EXPECT_GL_ERROR(err1, (err1 == GL_INVALID_VALUE),
			"rTest_Clear_invalidBitmask failed.\n"
			"glClear, tanımsız olan geçersiz maske bitlerini yuttu "
			"(0xFFFFFFFF).");

	// İnce Suikast: Sadece bir tane geçersiz bit (örneğin 0x04) ekleyerek
	// kirletiyoruz.
	glClear(GL_COLOR_BUFFER_BIT | 0x00000004);
	GLenum err2 = glGetError();

	EXPECT_GL_ERROR(
	    err2, (err2 == GL_INVALID_VALUE),
	    "rTest_Clear_invalidBitmask failed.\n"
	    "Kirletilmiş mantıksal maske kombinasyonu reddedilmedi.");
}

/* ============================================================
 * glClearColor — Özel Float Degerleri (NaN, Infinity)
 * ============================================================
 *
 * Kayan nokta zehirlemesi (NaN ve Infinity). Spec bu konuda
 * çok net olmasa da kaliteli bir SC 2.0 sürücüsü bunu
 * sessizce yutmali veya güvenli hale getirmelidir.
 * Kesinlikle ÇÖKMEMELIDIR.
 * ============================================================ */
void rTest_ClearColor_specialFloats(void) {
	// Kayan nokta zehirlemesi (NaN ve Infinity)
	// Spec bu konuda çok net olmasa da kaliteli bir SC 2.0 sürücüsü bunu
	// sessizce yutmalı veya kendi içinde güvenli bir hale getirmelidir
	// (Kesinlikle ÇÖKMEMELİDİR).
	glClearColor(NAN, INFINITY, -INFINITY, 1.5f);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR || err == GL_INVALID_VALUE),
			"rTest_ClearColor_specialFloats failed.\n"
			"NaN/Inf değerleri atanırken beklenmeyen bir hata kodu "
			"döndü veya sürücü kilitlendi.");
}

/* ============================================================
 * glClearDepthf — Clamping Dogrulamasi
 * ============================================================
 *
 * Depth degeri matematiksel olarak [0.0, 1.0] araliginda
 * olmalidir. Sinir disina çikilarak sürücünün sessizce
 * kırpma (clamp) yapip yapamadigi dogrulanir.
 * ============================================================ */
void rTest_ClearDepthf_clamping(void) {
	// Depth değeri matematikte sadece [0.0, 1.0] aralığında olabilir.
	// Biz sınırların çok dışına taşıyoruz.
	glClearDepthf(5000.0f);
	GLenum err = glGetError();

	// Spesifikasyon: "Değerler hata fırlatmadan sessizce [0,1] aralığına
	// kırpılır."
	EXPECT_GL_ERROR(
	    err, (err == GL_NO_ERROR),
	    "rTest_ClearDepthf_clamping failed.\n"
	    "Sınır dışı depth atamasında beklenmeyen bir hata fırlatıldı.");

	// Sürücü gerçekten değeri 1.0'a kilitledi (clamp) mi?
	GLfloat depth = -1.0f;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depth);

	EXPECT_GL_ERROR(
	    depth, (depth == 1.0f),
	    "rTest_ClearDepthf_clamping failed.\n"
	    "Sürücü aşırı depth değerini [0,1] aralığına kırpmayı başaramadı.");
}

/* ============================================================
 * glClearStencil — Sinir Degerleri
 * ============================================================
 *
 * Stencil degeri integer'dir, ancak mevcut Stencil Buffer bit
 * sayisina göre maskelenir. Asiri büyük ve negatif sayilar
 * vererek state okumasinin bozulup bozulmadigini test eder.
 * ============================================================ */
void rTest_ClearStencil_bounds(void) {
	// Stencil değeri integer'dır, ancak mevcut Stencil Buffer bit sayısına
	// göre maskelenir. Aşırı büyük ve negatif sayılar vererek state
	// okumasının bozulup bozulmadığını sınıyoruz.
	glClearStencil(-1); // Genelde tüm bitleri 1 yapan maske görevi görür
			    // (Two's complement)
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"rTest_ClearStencil_bounds failed.\n"
			"Negatif değer atamasında hata fırlatıldı.");
}

/****************************************/
/******** Buffer Objects ********/
/****************************************/

/* ============================================================
 * glGenBuffers — Negatif n Degeri
 * ============================================================
 *
 * Belirtilen hata: GL_INVALID_VALUE is generated if n is
 * negative. Negatif n degeri ile çagrilarak sürücünün bunu
 * GL_INVALID_VALUE ile reddedip reddetmedigi dogrulanir.
 * ============================================================ */
void rTest_glGenBuffers_invalid_value() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer = 0;
	glGenBuffers(-1, &buffer);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_glGenBuffers_invalid_value failed. Expected "
			"GL_INVALID_VALUE.");
}

/* ============================================================
 * glGenBuffers — Sifir Count
 * ============================================================
 *
 * n = 0 ile çagri yapildiginda sürücünün tanimli davranip
 * davranmadigi gözlemlenir.
 * ============================================================ */
void rTest_glGenBuffers_zero_count() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf = 0xCDCDCDCD; // sentinel değer
	glGenBuffers(0, &buf);
	GLenum err = glGetError();
	printf("[INFO] glGenBuffers(n=0): error=0x%X, buffer=0x%08X\n", err,
	       buf);
}

/* ============================================================
 * glGenBuffers — NULL Buffers Pointer
 * ============================================================
 *
 * buffers = NULL, n > 0 (negative robustness): sürücünün
 * NULL pointer ile çökmeden davranip davranmadigi gözlemlenir.
 * ============================================================ */
void rTest_glGenBuffers_null_buffers() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glGenBuffers(5, NULL);
	GLenum err = glGetError();
	printf("[INFO] glGenBuffers(buffers=nullptr, n=5): error=0x%X\n", err);
}

/* ============================================================
 * glGenBuffers — Asiri Büyük n
 * ============================================================
 *
 * 100.000 adet buffer ismi üreterek sürücünün büyük tahsis
 * isteklerini kararli bir sekilde ele alip almadigi dogrulanir.
 * ============================================================ */
void rTest_glGenBuffers_large_n() {
	while (glGetError() != GL_NO_ERROR) {
	}

	const GLsizei largeCount = 100000;
	GLuint *buffers = (GLuint *)malloc(sizeof(GLuint) * largeCount);
	if (buffers == NULL) {
		printf("[INFO] Memory allocation failed.\n");
		return;
	}
	glGenBuffers(largeCount, buffers);
	GLenum err = glGetError();
	printf("[INFO] glGenBuffers(n=%d): error=0x%X\n", largeCount, err);
	free(buffers);
}

/* ============================================================
 * glGenBuffers — Tekrarli Üretim
 * ============================================================
 *
 * Ayni array'i art arda 1000 kez çagirarak isim tekilligini
 * bozmaya çalisir. Sürücünün tekrarli üretimde hata verip
 * vermediği gözlemlenir.
 * ============================================================ */
void rTest_glGenBuffers_repeated_generation() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffers[10];
	for (int i = 0; i < 1000; ++i) {
		glGenBuffers(10, buffers);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[INFO] glGenBuffers failed at iteration %d: "
			       "error=0x%X\n",
			       i, err);
			return;
		}
	}
	printf(
	    "[INFO] Repeated glGenBuffers(10) x1000 completed successfully.\n");
}

/* ============================================================
 * glGenBuffers — Benzersiz Isimler
 * ============================================================
 *
 * 1000 adet buffer adi üreterek döndürülen isimlerin
 * benzersiz oldugunu ve reserved 0 isminin üretilmedigini
 * dogrular.
 * ============================================================ */
void rTest_glGenBuffers_unique_names() {
	while (glGetError() != GL_NO_ERROR) {
	}

	const GLsizei COUNT = 1000;
	GLuint buffers[COUNT];
	glGenBuffers(COUNT, buffers);

	// 0 ismi üretilmemeli
	for (int i = 0; i < COUNT; i++) {
		if (buffers[i] == 0) {
			EXPECT_GL_ERROR(
			    0, 0,
			    "rTest_glGenBuffers_unique_names failed. Reserved "
			    "name 0 was generated.");
			glDeleteBuffers(COUNT, buffers);
			return;
		}
	}

	// Aynı isim iki kez üretilmemeli
	for (int i = 0; i < COUNT; i++) {
		for (int j = i + 1; j < COUNT; j++) {
			if (buffers[i] == buffers[j]) {
				EXPECT_GL_ERROR(
				    buffers[i], 0,
				    "rTest_glGenBuffers_unique_names failed. "
				    "Duplicate buffer name found.");
				glDeleteBuffers(COUNT, buffers);
				return;
			}
		}
	}

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"rTest_glGenBuffers_unique_names failed.");

	glDeleteBuffers(COUNT, buffers);
}

/* ============================================================
 * glGenBuffers — Bind Edilmemis Isim Yasam Döngüsü
 * ============================================================
 *
 * Bind edilmemis buffer isimleri üzerinde glIsBuffer ve
 * glDeleteBuffers çagrilarinin spesifikasyona uygun
 * davranip davranmadigini dogrular.
 * ============================================================ */
void rTest_glGenBuffers_unbound_names_lifecycle() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	GLboolean isBuffer = glIsBuffer(buf);

	glDeleteBuffers(1, &buf);

	GLenum err = glGetError();
	printf("[INFO] Unbound buffer name: glIsBuffer=%s, glDeleteBuffers "
	       "error=0x%X\n",
	       isBuffer ? "GL_TRUE" : "GL_FALSE", err);
}

/* ============================================================
 * glGenBuffers — Çift Silme
 * ============================================================
 *
 * Ayni buffer isminin birden fazla kez silinmesi durumunda
 * implementasyonun kararlilığını test eder.
 * ============================================================ */
void rTest_glGenBuffers_double_delete() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glDeleteBuffers(1, &buf);
	GLenum firstErr = glGetError();
	glDeleteBuffers(1, &buf);
	GLenum secondErr = glGetError();
	printf("[INFO] Double delete: firstErr=0x%X, secondErr=0x%X\n",
	       firstErr, secondErr);
}

/* ============================================================
 * glGenBuffers — Devasa Count, Küçük Buffer
 * ============================================================
 *
 * Büyük 'n' degeri (INT_MAX) ve kasitli olarak yetersiz
 * output buffer kullanilarak implementasyonun geçersiz
 * istemci bellegi karsisindaki davranisi test edilir.
 * ============================================================ */
void rTest_glGenBuffers_huge_count_small_buffer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLsizei huge_n = INT_MAX;
	GLuint buffers[1];

	glGenBuffers(huge_n, buffers);

	GLenum err = glGetError();
	printf("[INFO] n=INT_MAX -> glError=0x%x\n", err);
}

/* ============================================================
 * glBindBuffer — Geçersiz Enum
 * ============================================================
 *
 * Belirtilen hata: GL_INVALID_ENUM is generated if target
 * is not one of the allowable values. 0xFFFFFFFF ile
 * çagrilarak sürücünün bunu reddetmesi beklenir.
 * ============================================================ */
void rTest_glBindBuffer_invalid_enum() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glBindBuffer(0xFFFFFFFF, 1);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_glBindBuffer_invalid_enum failed. Expected "
			"GL_INVALID_ENUM.");
}

/* ============================================================
 * glBindBuffer — Gen Olmadan Yeni Isim
 * ============================================================
 *
 * glGenBuffers ile olusturulmamis bir ismin bind edilmesi
 * durumunda sürücünün davranisi gözlemlenir.
 * ============================================================ */
void rTest_glBindBuffer_new_name_without_gen() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint name = 424242;
	glBindBuffer(GL_ARRAY_BUFFER, name);
	GLenum err = glGetError();
	printf("[INFO] glBindBuffer(new name=%u): error=0x%X\n", name, err);
}

/* ============================================================
 * glBindBuffer — Silinmis Buffer
 * ============================================================
 *
 * Silinen bir buffer isminin tekrar bind edilmesiyle yeni
 * bir buffer nesnesi olusturulup olusturulmadigini test eder.
 * ============================================================ */
void rTest_glBindBuffer_deleted_buffer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glDeleteBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	GLenum err = glGetError();
	printf("[INFO] Bind deleted name: error=0x%X\n", err);
}

/* ============================================================
 * glBindBuffer — Sinir Buffer Isimleri
 * ============================================================
 *
 * Büyük/alisılmadik buffer isimlerinin (UINT_MAX, INT_MAX,
 * 0xDEADBEEF, vb.) bind edilmesi durumunda sürücünün
 * davranisi gözlemlenir.
 * ============================================================ */
void rTest_glBindBuffer_boundary_handles() {
	GLuint candidates[] = {0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu,
			       0xDEADBEEFu, 0xCDCDCDCDu};
	for (int i = 0; i < 5; ++i) {
		while (glGetError() != GL_NO_ERROR) {
		}
		glBindBuffer(GL_ARRAY_BUFFER, candidates[i]);
		GLenum err = glGetError();
		printf("[INFO] Boundary buffer 0x%08X: glError=0x%X\n",
		       candidates[i], err);
	}
}

/* ============================================================
 * glBindBuffer — Kirli Yüksek Bitler
 * ============================================================
 *
 * Geçersiz target enum degerlerine karsi implementasyonun
 * hata kontrolünün testi. GL_ARRAY_BUFFER | 0xFFFF0000u
 * gibi kirletilmis bir enum kullanilir.
 * ============================================================ */
void rTest_glBindBuffer_dirty_high_bits_enum() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLenum polluted = GL_ARRAY_BUFFER | 0xFFFF0000u;
	glBindBuffer(polluted, 1);
	GLenum err = glGetError();
	printf("[INFO] Polluted target=0x%08X : glError=0x%X (expected "
	       "GL_INVALID_ENUM)\n",
	       polluted, err);
}

/* ============================================================
 * glBindBuffer — Hizli Çapraz Hedef Rebind Stres Testi
 * ============================================================
 *
 * Ayni buffer nesnesinin farkli target'lara hizli ve
 * tekrarli sekilde baglanmasi sirasinda implementasyonun
 * kararlilığını test eder.
 * ============================================================ */
void rTest_glBindBuffer_rapid_cross_target_rebind_stress() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);

	for (int i = 0; i < 5000; ++i) {
		GLenum target =
		    (i % 2 == 0) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
		glBindBuffer(target, buf);
		glBufferData(target, (i % 7) * 37, NULL, GL_STATIC_DRAW);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[INFO] Iteration=%d, target=%s, glError=0x%X\n",
			       i,
			       target == GL_ARRAY_BUFFER
				   ? "GL_ARRAY_BUFFER"
				   : "GL_ELEMENT_ARRAY_BUFFER",
			       err);
			glDeleteBuffers(1, &buf);
			return;
		}
	}
	printf("[INFO] Rapid cross-target rebind stress completed without "
	       "OpenGL errors.\n");
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glBindBuffer — Çift Bind Sirasinda Silme
 * ============================================================
 *
 * Ayni buffer nesnesi iki target'a bagliyken silme islemi
 * sonrasi implementasyonun kararlilığını ve hata davranisini
 * test eder.
 * ============================================================ */
void rTest_glBindBuffer_delete_while_double_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
	glDeleteBuffers(1, &buf);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 64, NULL);
	GLenum arrayErr = glGetError();

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 64, NULL);
	GLenum elementErr = glGetError();

	printf("[INFO] After deleting double-bound buffer: ARRAY_BUFFER "
	       "error=0x%X, ELEMENT_ARRAY_BUFFER error=0x%X\n",
	       arrayErr, elementErr);
}

/* ============================================================
 * glBindBuffer — Sifir Baglama Sorgu Stresi
 * ============================================================
 *
 * Buffer'i tekrar tekrar 0'a baglayip baglama durumunu
 * sorgulayarak implementasyonun state yönetimi kararlilığını
 * test eder.
 * ============================================================ */
void rTest_glBindBuffer_zero_binding_query_thrash() {
	while (glGetError() != GL_NO_ERROR) {
	}

	for (int i = 0; i < 1000; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLint binding = -1;
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding);
		if (binding != 0) {
			EXPECT_GL_ERROR(
			    binding, 0,
			    "rTest_glBindBuffer_zero_binding_query_thrash "
			    "failed. GL_ARRAY_BUFFER_BINDING != 0.");
			return;
		}

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			EXPECT_GL_ERROR(err, 0,
					"rTest_glBindBuffer_zero_binding_query_"
					"thrash failed.");
			return;
		}
	}
	printf("[INFO] Zero binding/query thrash completed successfully.\n");
}

/* ============================================================
 * glBindBuffer — Devasa Isim Alani Fuzz
 * ============================================================
 *
 * Çok sayida buffer ismi üzerinde rastgele bind islemleri
 * yaparak implementasyonun isim yönetimi ve durum
 * degisikliklerine karsi dayanikliligini test eder.
 * ============================================================ */
void rTest_glBindBuffer_massive_namespace_fuzz() {
	while (glGetError() != GL_NO_ERROR) {
	}

	const int N = 20000;
	GLuint *names = (GLuint *)malloc(sizeof(GLuint) * N);

	if (names == NULL) {
		printf("[INFO] Memory allocation failed.\n");
		return;
	}

	glGenBuffers(N, names);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("[INFO] glGenBuffers failed: glError=0x%X\n", err);
		free(names);
		return;
	}

	uint32_t seed = 0x1234567u;

	for (int i = 0; i < N; ++i) {
		seed ^= seed << 13;
		seed ^= seed >> 17;
		seed ^= seed << 5;
		GLuint name = names[seed % N];

		seed ^= seed << 13;
		seed ^= seed >> 17;
		seed ^= seed << 5;
		GLenum target =
		    (seed & 1) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

		glBindBuffer(target, name);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[INFO] Iteration=%d, buffer=%u, target=%s, "
			       "glError=0x%X\n",
			       i, name,
			       target == GL_ARRAY_BUFFER
				   ? "GL_ARRAY_BUFFER"
				   : "GL_ELEMENT_ARRAY_BUFFER",
			       err);

			glDeleteBuffers(N, names);
			free(names);
			return;
		}
	}

	glDeleteBuffers(N, names);
	free(names);

	printf("[INFO] Massive buffer namespace fuzz completed without OpenGL "
	       "errors.\n");
}

/* ============================================================
 * glBindBuffer — Baglama Degisim Stresi
 * ============================================================
 *
 * Ayni target üzerinde farkli buffer'lar arasinda sürekli
 * geçis yaparak implementasyonun state yönetimi kararlilığını
 * test eder.
 * ============================================================ */
void rTest_glBindBuffer_binding_churn_stress() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffers[2];
	glGenBuffers(2, buffers);

	for (int i = 0; i < 10000; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			EXPECT_GL_ERROR(
			    err, 0,
			    "rTest_glBindBuffer_binding_churn_stress failed.");
			glDeleteBuffers(2, buffers);
			return;
		}
	}

	glDeleteBuffers(2, buffers);
	printf(
	    "[INFO] Binding churn stress completed without OpenGL errors.\n");
}

/* ============================================================
 * glBindBuffer — Yasam Döngüsü Stresi
 * ============================================================
 *
 * Buffer nesnelerinin olusturma, baglama ve silme yasam
 * döngüsünü tekrarli olarak çalistirarak implementasyonun
 * dayanikliligini test eder.
 * ============================================================ */
void rTest_glBindBuffer_lifecycle_stress() {
	while (glGetError() != GL_NO_ERROR) {
	}

	for (int i = 0; i < 5000; ++i) {
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
		glDeleteBuffers(1, &buf);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			EXPECT_GL_ERROR(
			    err, 0,
			    "rTest_glBindBuffer_lifecycle_stress failed.");
			return;
		}
	}
	printf("[INFO] Buffer lifecycle stress completed without OpenGL "
	       "errors.\n");
}

/* ============================================================
 * glBufferData — Geçersiz Target Enum
 * ============================================================
 *
 * GL_INVALID_ENUM is generated if target is not
 * GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
 * ============================================================ */
void rTest_glBufferData_invalid_enum_target() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glBufferData(0xFFFFFFFF, 16, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_glBufferData_invalid_enum_target failed. "
			"Expected GL_INVALID_ENUM.");
}

/* ============================================================
 * glBufferData — Geçersiz Usage Enum
 * ============================================================
 *
 * GL_INVALID_ENUM is generated if usage is not
 * GL_STREAM_DRAW, GL_STATIC_DRAW, or GL_DYNAMIC_DRAW.
 * ============================================================ */
void rTest_glBufferData_invalid_enum_usage() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 16, NULL, 0xFFFFFFFF);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_glBufferData_invalid_enum_usage failed. "
			"Expected GL_INVALID_ENUM.");

	glDeleteBuffers(1, &buffer);
}

/* ============================================================
 * glBufferData — Negatif Boyut
 * ============================================================
 *
 * GL_INVALID_VALUE is generated if size is negative.
 * ============================================================ */
void rTest_glBufferData_invalid_value_negative_size() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, -1, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_glBufferData_invalid_value_negative_size "
			"failed. Expected GL_INVALID_VALUE.");

	glDeleteBuffers(1, &buffer);
}

/* ============================================================
 * glBufferData — Sifir Buffer Bagli
 * ============================================================
 *
 * GL_INVALID_OPERATION is generated if the reserved buffer
 * object name 0 is bound to target.
 * ============================================================ */
void rTest_glBufferData_invalid_operation_zero_buffer_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBufferData(GL_ARRAY_BUFFER, 16, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_glBufferData_invalid_operation_zero_buffer_"
			"bound failed. Expected GL_INVALID_OPERATION.");
}

/* ============================================================
 * glBufferData — Bellek Yetersizligi
 * ============================================================
 *
 * GL_OUT_OF_MEMORY is generated if the GL is unable to
 * create a data store with the specified size.
 * ============================================================ */
void rTest_glBufferData_out_of_memory() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)INTPTR_MAX, NULL,
		     GL_STATIC_DRAW);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_OUT_OF_MEMORY || err == GL_NO_ERROR),
			"rTest_glBufferData_out_of_memory failed. Expected "
			"GL_OUT_OF_MEMORY or GL_NO_ERROR.");
	if (err == GL_OUT_OF_MEMORY)
		printf("[INFO] GL_OUT_OF_MEMORY was generated.\n");
	else if (err == GL_NO_ERROR)
		printf("[INFO] GL_OUT_OF_MEMORY was not generated "
		       "(implementation-dependent).\n");

	glDeleteBuffers(1, &buffer);
}

/* ============================================================
 * glBufferData — Kaynak Boyutu Yetersiz
 * ============================================================
 *
 * Kaynak veri boyutunun belirtilen 'size' degerinden küçük
 * oldugu hatali API kullanimina karsi implementasyonun
 * dayanikliligini gözlemler.
 * ============================================================ */
void rTest_glBufferData_source_buffer_too_small() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char smallSource[16] = {0};
	glBufferData(GL_ARRAY_BUFFER, 4096, smallSource, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] Misuse robustness (source buffer too small): "
	       "glError=0x%X\n",
	       err);

	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glBufferData — Sifir Boyut, Non-NULL Data
 * ============================================================
 *
 * Sifir boyutlu data store olusturulurken geçerli bir data
 * pointer'i verilmesinin implementasyon tarafindan güvenli
 * sekilde ele alinip alinmadigini dogrular.
 * ============================================================ */
void rTest_glBufferData_zero_size_nonnull_data() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char dummy = 0xAB;
	glBufferData(GL_ARRAY_BUFFER, 0, &dummy, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] size=0, data!=NULL: glError=0x%X\n", err);

	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glBufferData — Boyut Tasmasi Sinir Testi
 * ============================================================
 *
 * Sinir ve asiri boyut size degerleri karsisinda
 * implementasyonun kararlilığını gözlemler.
 * ============================================================ */
void rTest_glBufferData_size_overflow_boundary() {
	GLsizeiptr candidates[] = {-1, INT_MIN, (GLsizeiptr)INT_MAX + 1,
				   LLONG_MAX};

	for (int i = 0; i < 4; ++i) {
		while (glGetError() != GL_NO_ERROR) {
		}

		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, candidates[i], NULL,
			     GL_STATIC_DRAW);

		GLenum err = glGetError();
		printf("[INFO] Boundary size test: size=%lld, glError=0x%X\n",
		       (long long)candidates[i], err);

		glDeleteBuffers(1, &buf);
	}
}

/* ============================================================
 * glBufferData — Kirli Usage Enum
 * ============================================================
 *
 * Geçersiz ve kirletilmis usage enum degerleri karsisinda
 * implementasyonun kararlilığını gözlemler.
 * ============================================================ */
void rTest_glBufferData_dirty_usage_enum() {
	GLenum candidates[] = {GL_STATIC_DRAW, 0xFFFF0000u, 0xFFFFFFFFu,
			       0x12345678u, 0xDEADBEEFu};

	for (int i = 0; i < (int)(sizeof(candidates) / sizeof(candidates[0]));
	     i++) {
		while (glGetError() != GL_NO_ERROR) {
		}

		GLenum usage = candidates[i];
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, 64, NULL, usage);

		GLenum err = glGetError();
		printf("[INFO] glBufferData(usage=0x%08X) completed, "
		       "glError=0x%X\n",
		       usage, err);

		glDeleteBuffers(1, &buf);
	}
}

/* ============================================================
 * glBufferData — Sifir Buffer Bagli (Target)
 * ============================================================
 *
 * Hedefe herhangi bir buffer bagli degilken glBufferData
 * çagrisinin güvenli sekilde ele alinip alinmadigini
 * gözlemler.
 * ============================================================ */
void rTest_glBufferData_target_zero_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] glBufferData(buffer=0) completed, glError=0x%X\n", err);
}

/* ============================================================
 * glBufferData — Tekrarli Boyut Degisim Stresi
 * ============================================================
 *
 * Ayni buffer üzerinde farkli boyutlarda data store'lari
 * art arda olusturarak implementasyonun reallocation
 * islemlerindeki kararlilığını gözlemler.
 * ============================================================ */
void rTest_glBufferData_repeated_resize_thrash() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	const GLsizeiptr sizes[] = {0, 1, 16, 64, 256, 4096, 65536, 1048576};
	for (int i = 0; i < 10000; ++i) {
		GLsizeiptr size = sizes[i % (sizeof(sizes) / sizeof(sizes[0]))];
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[INFO] Iteration=%d, size=%lld, glError=0x%X\n",
			       i, (long long)size, err);
			break;
		}
	}
	printf("[INFO] Repeated resize stress test completed.\n");

	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glBufferData — Hizasiz Data Pointer
 * ============================================================
 *
 * Hizasi bozulmus bir kaynak data pointer'i kullanilarak
 * implementasyonun hatali istemci girdisi karsisindaki
 * kararlilığı gözlemlenir.
 * ============================================================ */
void rTest_glBufferData_misaligned_data_pointer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char raw[128] = {0};
	void *misaligned = raw + 1;
	glBufferData(GL_ARRAY_BUFFER, 64, misaligned, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] glBufferData(misaligned data pointer) completed, "
	       "glError=0x%X\n",
	       err);

	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glBufferData — Dangling Data Pointer
 * ============================================================
 *
 * Serbest birakilmis (dangling) bir kaynak pointer
 * kullanilarak implementasyonun hatali istemci girdisi
 * karsisindaki kararlilığı gözlemlenir.
 * ============================================================ */
void rTest_glBufferData_dangling_data_pointer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char *heapData = (char *)malloc(256);
	if (heapData == NULL) {
		printf("[INFO] Memory allocation failed.\n");
		glDeleteBuffers(1, &buf);
		return;
	}

	memset(heapData, 0xAB, 256);
	free(heapData);
	glBufferData(GL_ARRAY_BUFFER, 256, heapData, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] glBufferData(dangling data pointer) completed, "
	       "glError=0x%X\n",
	       err);

	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glBufferData — OOM Sonrasi State
 * ============================================================
 *
 * Büyük bir data store tahsis denemesi sonrasinda buffer
 * nesnesinin durumunun korunup korunmadigini gözlemler.
 * ============================================================ */
void rTest_glBufferData_state_after_out_of_memory() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)1 << 40, NULL,
		     GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] glBufferData(huge size) completed, glError=0x%X\n", err);

	GLint sizeAfter = -1;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeAfter);
	printf("[INFO] Buffer size after allocation attempt: %d bytes\n",
	       sizeAfter);

	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glBufferSubData — Geçersiz Target Enum
 * ============================================================
 *
 * GL_INVALID_ENUM is generated if target is not
 * GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
 * ============================================================ */
void rTest_glBufferSubData_invalid_enum_target() {
	while (glGetError() != GL_NO_ERROR) {
	}

	int data = 123;
	glBufferSubData(0xFFFFFFFF, 0, sizeof(data), &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_glBufferSubData_invalid_enum_target failed. "
			"Expected GL_INVALID_ENUM.");
}

/* ============================================================
 * glBufferSubData — Negatif Offset
 * ============================================================
 *
 * GL_INVALID_VALUE is generated if offset is negative.
 * ============================================================ */
void rTest_glBufferSubData_invalid_value_negative_offset() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer;
	int data = 123;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, -1, sizeof(data), &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_glBufferSubData_invalid_value_negative_offset "
			"failed. Expected GL_INVALID_VALUE.");

	glDeleteBuffers(1, &buffer);
}

/* ============================================================
 * glBufferSubData — Negatif Boyut
 * ============================================================
 *
 * GL_INVALID_VALUE is generated if size is negative.
 * ============================================================ */
void rTest_glBufferSubData_invalid_value_negative_size() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer;
	int data = 123;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, -1, &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_glBufferSubData_invalid_value_negative_size "
			"failed. Expected GL_INVALID_VALUE.");

	glDeleteBuffers(1, &buffer);
}

/* ============================================================
 * glBufferSubData — Sinir Disi (Out of Bounds)
 * ============================================================
 *
 * GL_INVALID_VALUE is generated if offset and size together
 * define a region beyond the allocated data store.
 * ============================================================ */
void rTest_glBufferSubData_invalid_value_out_of_bounds() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer;
	int data = 123;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 60, 8, &data); // 60 + 8 = 68 > 64

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_glBufferSubData_invalid_value_out_of_bounds "
			"failed. Expected GL_INVALID_VALUE.");

	glDeleteBuffers(1, &buffer);
}

/* ============================================================
 * glBufferSubData — Sifir Buffer Bagli
 * ============================================================
 *
 * GL_INVALID_OPERATION is generated if the reserved buffer
 * object name 0 is bound to target.
 * ============================================================ */
void rTest_glBufferSubData_invalid_operation_zero_buffer_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	int data = 123;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_glBufferSubData_invalid_operation_zero_buffer_"
			"bound failed. Expected GL_INVALID_OPERATION.");
}

/* ============================================================
 * glBufferSubData — Offset+Size Overflow (Wraparound)
 * ============================================================
 *
 * Offset ve size degerlerinin toplaminda olusabilecek
 * integer overflow durumunda implementasyonun sinir
 * kontrollerini güvenli sekilde yapip yapmadigini gözlemler.
 * ============================================================ */
void rTest_glBufferSubData_offset_size_overflow_wraparound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

	GLintptr offset = 100;
	GLsizeiptr size = (GLsizeiptr)LLONG_MAX - 50;
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, NULL);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(offset=%lld, size=%lld) completed, "
	       "glError=0x%X\n",
	       (long long)offset, (long long)size, err);
}

/* ============================================================
 * glBufferSubData — Tam Sinir Offset
 * ============================================================
 *
 * Buffer sinirinin tam bitis noktasi ve bir byte ötesi
 * kullanilarak implementasyonun sinir kontrollerindeki
 * kararlilığı gözlemlenir.
 * ============================================================ */
void rTest_glBufferSubData_exact_boundary_offset() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

	char data[16] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, 240, 16, data);

	GLenum err = glGetError();
	printf("[INFO] Exact boundary update completed (expected: NO_ERROR), "
	       "glError=0x%X\n",
	       err);

	glBufferSubData(GL_ARRAY_BUFFER, 241, 16, data);
	err = glGetError();
	printf("[INFO] One-byte-beyond boundary update completed (expected: "
	       "INVALID_VALUE), glError=0x%X\n",
	       err);
}

/* ============================================================
 * glBufferSubData — Negatif Offset Telafisi
 * ============================================================
 *
 * Negatif offset degerinin büyük bir size ile "telafi
 * edildigi" durumda implementasyonun offset dogrulamasini
 * bagimsiz olarak yapip yapmadigini gözlemler.
 * ============================================================ */
void rTest_glBufferSubData_negative_offset_compensating_size() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

	GLintptr offset = -512;
	GLsizeiptr size = 600;

	char data[600] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(offset=%lld, size=%lld) completed, "
	       "(expected:INVALID_VALUE) glError=0x%X\n",
	       (long long)offset, (long long)size, err);
}

/* ============================================================
 * glBufferSubData — Sifir Boyut, NULL Data
 * ============================================================
 *
 * Sifir byte güncelleme isteginde implementasyonun gereksiz
 * bellek erisimi yapmadan çagriyi güvenli sekilde
 * tamamlayip tamamlamadigi gözlemlenir.
 * ============================================================ */
void rTest_glBufferSubData_zero_size_null_data() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 0, NULL);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(size=0, data=NULL) completed, "
	       "glError=0x%X\n",
	       err);
}

/* ============================================================
 * glBufferSubData — Sifir Buffer Bagli (Target)
 * ============================================================
 *
 * Hedefe herhangi bir buffer bagli degilken glBufferSubData
 * çagrisinin güvenli sekilde ele alinip alinmadigini
 * gözlemler.
 * ============================================================ */
void rTest_glBufferSubData_target_zero_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	char data[16] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(buffer=0) completed, glError=0x%X "
	       "(expected: GL_INVALID_OPERATION)\n",
	       err);
}

/* ============================================================
 * glBufferSubData — Sifir Boyutlu Store'a Yazma
 * ============================================================
 *
 * Data store'u henüz olusturulmamis (0 byte) bir buffer
 * nesnesine yazma isteginin güvenli sekilde ele alinip
 * alinmadigini gözlemler.
 * ============================================================ */
void rTest_glBufferSubData_into_zero_sized_store() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char data[16] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(zero-sized store, size=16) completed, "
	       "glError=0x%X (expected: GL_INVALID_VALUE)\n",
	       err);
}

/* ============================================================
 * glBufferSubData — Kaynak Boyutu Yetersiz
 * ============================================================
 *
 * Kaynak veri tamponunun belirtilen size degerinden küçük
 * oldugu hatali API kullanimina karsi implementasyonun
 * dayanikliligini gözlemler.
 * ============================================================ */
void rTest_glBufferSubData_source_smaller_than_size() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

	char small_source[8] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4096, small_source);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(source smaller than size) completed, "
	       "glError=0x%X\n",
	       err);
}

/* ============================================================
 * glBufferSubData — Dangling Data Pointer
 * ============================================================
 *
 * Serbest birakilmis bir istemci bellek isareçisi
 * kullanilarak implementasyonun geçersiz veri kaynagi
 * karsisindaki davranisi gözlemlenir.
 * ============================================================ */
void rTest_glBufferSubData_dangling_data_pointer(void) {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

	char *heap_data = (char *)malloc(256);
	free(heap_data);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 256, heap_data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(dangling data pointer) completed, "
	       "glError=0x%X\n",
	       err);
}

/* ============================================================
 * glBufferSubData — Çakisan/Hizasiz Güncelleme Stresi
 * ============================================================
 *
 * Ayni buffer bölgesine çakisan ve hizasiz güncellemeleri
 * art arda gerçeklestirerek implementasyonun yogun bellek
 * kopyalama yükü altindaki kararlilığını gözlemler.
 * ============================================================ */
void rTest_glBufferSubData_overlapping_misaligned_thrash() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

	char raw[64];
	for (int i = 0; i < 5000; ++i) {
		GLintptr offset = (i * 3) % 4090;
		GLsizeiptr size = 1 + (i % 63);
		if (offset + size > 4096)
			continue;

		void *misaligned = raw + (i % 3);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, misaligned);
	}

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(overlapping/misaligned thrash) "
	       "completed, glError=0x%X\n",
	       err);
}

/* ============================================================
 * glGetBufferParameteriv — Geçersiz Target Enum
 * ============================================================
 *
 * GL_INVALID_ENUM is generated if target is not
 * GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
 * ============================================================ */
void rTest_glGetBufferParameteriv_invalid_enum_target() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLint data = -1;
	glGetBufferParameteriv((GLenum)0xFFFFFFFF, GL_BUFFER_SIZE, &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_glGetBufferParameteriv_invalid_enum_target "
			"failed. Expected GL_INVALID_ENUM.");
}

/* ============================================================
 * glGetBufferParameteriv — Geçersiz Value Enum
 * ============================================================
 *
 * GL_INVALID_ENUM is generated if value is not
 * GL_BUFFER_SIZE or GL_BUFFER_USAGE.
 * ============================================================ */
void rTest_glGetBufferParameteriv_invalid_enum_value() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buffer;
	GLint data = -1;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, (GLenum)0xFFFFFFFF, &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_glGetBufferParameteriv_invalid_enum_value "
			"failed. Expected GL_INVALID_ENUM.");

	glDeleteBuffers(1, &buffer);
}

/* ============================================================
 * glGetBufferParameteriv — Sifir Buffer Bagli
 * ============================================================
 *
 * GL_INVALID_OPERATION is generated if the reserved buffer
 * object name 0 is bound to target.
 * ============================================================ */
void rTest_glGetBufferParameteriv_invalid_operation_zero_buffer_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLint data = -1;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_glGetBufferParameteriv_invalid_operation_zero_"
			"buffer_bound failed. Expected GL_INVALID_OPERATION.");
}

/* ============================================================
 * glGetBufferParameteriv — Geçersiz Target
 * ============================================================
 *
 * target GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER disinda bir
 * enum oldugunda GL_INVALID_ENUM üretilip data'nin
 * degismedigini dogrular.
 * ============================================================ */
void rTest_glGetBufferParameteriv_invalid_target() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

	GLint sentinel = 0x7EADBEEF;
	GLint data = sentinel;
	glGetBufferParameteriv(GL_TEXTURE_2D, GL_BUFFER_SIZE, &data);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_glGetBufferParameteriv_invalid_target failed. "
			"Expected GL_INVALID_ENUM for invalid target.");
	EXPECT_GL_ERROR(data, (data == sentinel),
			"rTest_glGetBufferParameteriv_invalid_target failed. "
			"Data was modified despite GL_INVALID_ENUM.");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Geçersiz Value
 * ============================================================
 *
 * value GL_BUFFER_SIZE/GL_BUFFER_USAGE disinda bir enum
 * oldugunda GL_INVALID_ENUM üretilip data'nin degismedigini
 * dogrular.
 * ============================================================ */
void rTest_glGetBufferParameteriv_invalid_value() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

	GLenum bogusValues[] = {GL_BUFFER_ACCESS, 0, 0xFFFFFFFF,
				GL_ARRAY_BUFFER, 0xDEADBEEF};
	int n = sizeof(bogusValues) / sizeof(bogusValues[0]);

	for (int i = 0; i < n; i++) {
		while (glGetError() != GL_NO_ERROR) {
		}
		GLint sentinel = 0x7EADBEEF;
		GLint data = sentinel;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, bogusValues[i], &data);
		GLenum err = glGetError();
		printf("[INFO] value=0x%X => err=0x%X, data-modified=%s\n",
		       bogusValues[i], err, (data != sentinel) ? "yes" : "no");
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Reserved 0 Bagli (ARRAY_BUFFER)
 * ============================================================
 *
 * Reserved isim 0 target'a bind edilmisken GL_INVALID_OPERATION
 * üretilip üretilmedigini dogrular.
 * ============================================================ */
void rTest_glGetBufferParameteriv_reserved_name_zero_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	while (glGetError() != GL_NO_ERROR) {
	}

	GLint sentinel = 0x7EADBEEF;
	GLint data = sentinel;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_glGetBufferParameteriv_reserved_name_zero_bound "
			"failed. Expected GL_INVALID_OPERATION.");
}

/* ============================================================
 * glGetBufferParameteriv — Reserved 0 Bagli (ELEMENT_ARRAY)
 * ============================================================
 *
 * GL_ELEMENT_ARRAY_BUFFER hedefi için de ayni reserved-0
 * davranisini dogrular.
 * ============================================================ */
void rTest_glGetBufferParameteriv_element_array_zero_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	while (glGetError() != GL_NO_ERROR) {
	}

	GLint data = -1;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_glGetBufferParameteriv_element_array_zero_bound "
			"failed. Expected GL_INVALID_OPERATION.");
}

/* ============================================================
 * glGetBufferParameteriv — NULL Data Pointer
 * ============================================================
 *
 * data parametresi NULL iken çagirildiginda implementasyonun
 * segfault yerine tanimli/tutarli davranip davranmadigini
 * gözlemler.
 * ============================================================ */
void rTest_glGetBufferParameteriv_null_data_pointer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
	while (glGetError() != GL_NO_ERROR) {
	}

	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, NULL);

	GLenum err = glGetError();
	printf("[INFO] NULL data pointer => err=0x%X\n", err);
	printf("[INFO] Implementation did not crash on NULL output pointer.\n");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Dangling Data Pointer
 * ============================================================
 *
 * data, geçersiz/erisilemez (dangling) bir bellek adresi
 * oldugunda implementasyonun bellek koruma ihlaline karsi
 * davranisini test eder.
 * ============================================================ */
void rTest_glGetBufferParameteriv_dangling_data_pointer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_DRAW);
	while (glGetError() != GL_NO_ERROR) {
	}

	GLint *freedPtr = (GLint *)malloc(sizeof(GLint));
	free(freedPtr);

	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, freedPtr);

	GLenum err = glGetError();
	printf("[INFO] Dangling data pointer call completed. err=0x%X\n", err);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Boyut Tutarliligi
 * ============================================================
 *
 * GL_BUFFER_SIZE sorgusunun, glBufferData ile ayrilan gerçek
 * boyutla tutarli olup olmadigini ve sifir boyutlu bir
 * bufferda dogru sekilde 0 döndürüp döndürmedigini dogrular.
 * ============================================================ */
void rTest_glGetBufferParameteriv_size_consistency() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	GLint initialSize = -1;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &initialSize);
	GLenum errInitial = glGetError();
	printf("[INFO] Initial (pre-BufferData) size=%d, err=0x%X\n",
	       initialSize, errInitial);

	const GLsizeiptr allocSize = 256;
	glBufferData(GL_ARRAY_BUFFER, allocSize, NULL, GL_STATIC_DRAW);

	GLint size = -1;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"rTest_glGetBufferParameteriv_size_consistency failed. "
			"Unexpected error querying size.");
	EXPECT_GL_ERROR(size, (size == (GLint)allocSize),
			"rTest_glGetBufferParameteriv_size_consistency failed. "
			"Size mismatch.");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Usage Tutarliligi
 * ============================================================
 *
 * GL_BUFFER_USAGE'in initial degerinin GL_STATIC_DRAW
 * oldugunu ve glBufferData sonrasi degisen usage degerlerinin
 * dogru yansitilip yansitilmadigini dogrular.
 * ============================================================ */
void rTest_glGetBufferParameteriv_usage_initial_and_updates() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	GLint initialUsage = -1;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &initialUsage);
	printf("[INFO] Initial usage (pre-BufferData) = 0x%X (expected "
	       "GL_STATIC_DRAW=0x%X)\n",
	       initialUsage, GL_STATIC_DRAW);

	GLenum usages[] = {GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW};
	int n = sizeof(usages) / sizeof(usages[0]);

	for (int i = 0; i < n; i++) {
		glBufferData(GL_ARRAY_BUFFER, 16, NULL, usages[i]);
		GLint got = -1;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &got);
		printf("[INFO] Set usage=0x%X => queried=0x%X\n", usages[i],
		       got);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Her Ikisi de Geçersiz
 * ============================================================
 *
 * target ve value her ikisi de geçersiz oldugunda hangi
 * hatanin üretildigini gözlemler. Implementasyon tutarli
 * bir siraya sahip olmali (crash olmamali).
 * ============================================================ */
void rTest_glGetBufferParameteriv_both_invalid() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLint data = 0x1234;
	glGetBufferParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &data);
	GLenum err = glGetError();

	printf("[INFO] Both target and value invalid => err=0x%X (data=%d)\n",
	       err, data);
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_ENUM),
	    "rTest_glGetBufferParameteriv_both_invalid failed. Expected "
	    "GL_INVALID_ENUM for combined invalid params.");
}

/* ============================================================
 * glGetBufferParameteriv — Silme Sonrasi Binding Reversal
 * ============================================================
 *
 * Silinmis bir buffer'in binding'inin 0'a döndügü spec
 * davranisini dogrular. Silme sonrasi sorgu
 * GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void rTest_glGetBufferParameteriv_after_delete_binding_reverts() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_DRAW);
	glDeleteBuffers(1, &buf);

	GLenum errDelete = glGetError();

	GLint data = -1;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &data);

	GLenum errQuery = glGetError();
	printf(
	    "[INFO] errDelete=0x%X, post-delete query errQuery=0x%X, data=%d\n",
	    errDelete, errQuery, data);

	EXPECT_GL_ERROR(
	    errQuery, (errQuery == GL_INVALID_OPERATION),
	    "rTest_glGetBufferParameteriv_after_delete_binding_reverts failed. "
	    "Expected GL_INVALID_OPERATION after delete.");
}

/* ============================================================
 * glGetBufferParameteriv — Ayni Buffer Çoklu Target
 * ============================================================
 *
 * Ayni buffer nesnesi hem GL_ARRAY_BUFFER hem
 * GL_ELEMENT_ARRAY_BUFFER hedeflerine ayni anda bind
 * edildiginde her iki target üzerinden sorgunun tutarli
 * sonuç verip vermedigini dogrular.
 * ============================================================ */
void rTest_glGetBufferParameteriv_same_buffer_multiple_targets() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 100, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
	GLint sizeViaArray = -1, sizeViaElement = -1;
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeViaArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE,
			       &sizeViaElement);

	printf("[INFO] sizeViaArray=%d, sizeViaElement=%d\n", sizeViaArray,
	       sizeViaElement);

	EXPECT_GL_ERROR(
	    sizeViaArray,
	    (sizeViaArray == sizeViaElement && sizeViaArray == 100),
	    "rTest_glGetBufferParameteriv_same_buffer_multiple_targets failed. "
	    "Inconsistent buffer state across targets.");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Hizasiz Data Pointer
 * ============================================================
 *
 * data çikis parametresi unaligned bir adres oldugunda
 * implementasyonun crash olmadan davranip davranmadigini
 * test eder.
 * ============================================================ */
void rTest_glGetBufferParameteriv_unaligned_data_pointer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 16, NULL, GL_STATIC_DRAW);

	while (glGetError() != GL_NO_ERROR) {
	}

	unsigned char raw[64];
	memset(raw, 0xAA, sizeof(raw));
	GLint *unaligned = (GLint *)(raw + 1);

	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, unaligned);

	GLenum err = glGetError();
	printf("[INFO] Unaligned data pointer => err=0x%X\n", err);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buf);
}

/* ============================================================
 * glGetBufferParameteriv — Hata Durumu Stresi
 * ============================================================
 *
 * Çok sayida ardisik geçersiz çagriyla error state'inin
 * (glGetError kuyrugu) tasip tasmadigini kontrol eder.
 * ============================================================ */
void rTest_glGetBufferParameteriv_error_state_stress() {
	while (glGetError() != GL_NO_ERROR) {
	}

	GLint data;
	const int ITER = 10000;
	for (int i = 0; i < ITER; i++) {
		glGetBufferParameteriv(GL_TEXTURE_2D, GL_BUFFER_SIZE, &data);
	}

	GLenum err1 = glGetError();
	GLenum err2 = glGetError();
	printf("[INFO] After %d invalid calls: err1=0x%X, err2=0x%X\n", ITER,
	       err1, err2);

	EXPECT_GL_ERROR(err1, (err1 == GL_INVALID_ENUM),
			"rTest_glGetBufferParameteriv_error_state_stress "
			"failed. Expected GL_INVALID_ENUM.");
	EXPECT_GL_ERROR(
	    err2, (err2 == GL_NO_ERROR),
	    "rTest_glGetBufferParameteriv_error_state_stress failed. Error "
	    "flag should be sticky single-flag per spec.");
}

/****************************************/
/********** Rasterization **********/
/****************************************/

#ifndef GL_ALIASED_LINE_WIDTH_RANGE
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#endif

/* ============================================================
 * glLineWidth — Temel Robustness
 * ============================================================
 *
 * glLineWidth'in temel sözlesmesini dogrular: pozitif degerler
 * hatasiz kabul edilmeli, pozitif olmayan degerler
 * GL_INVALID_VALUE ile reddedilmelidir. Reddedilen çagrilar
 * mevcut GL_LINE_WIDTH durumunu degistirmemelidir.
 * ============================================================ */
void test_lineWidth_basicRobustness(void) {
	GLfloat width;
	GLenum err;

	printf("TEST: Basic Robustness\n");
	resetState_lineWidth();

	glLineWidth(2.0f);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_NO_ERROR),
	    "test_lineWidth_basicRobustness failed. Valid width rejected.");

	glGetFloatv(GL_LINE_WIDTH, &width);
	EXPECT_GL_ERROR(
	    width, (width == 2.0f),
	    "test_lineWidth_basicRobustness failed. Width not set correctly.");

	glLineWidth(0.0f);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "test_lineWidth_basicRobustness failed. Zero width accepted.");

	glLineWidth(-5.0f);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "test_lineWidth_basicRobustness failed. Negative width accepted.");

	checkStatePreserved_lineWidth(2.0f);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(
		    _e, (_e == GL_NO_ERROR),
		    "test_lineWidth_basicRobustness failed. Residual error.");
	}

	resetState_lineWidth();
}

/* ============================================================
 * glLineWidth — Parametrik Tarama
 * ============================================================
 *
 * [-1000.0, +1000.0] araliginda 0.1 çözünürlükle tarama
 * yaparak implementasyona özgü sinir anomalilerini ortaya
 * çikarir. w <= 0 → GL_INVALID_VALUE, w > 0 → GL_NO_ERROR.
 * ============================================================ */
void test_lineWidth_stressSweep(void) {
	int i;
	int passCount = 0;
	int failCount = 0;

	printf("TEST: Stress Sweep (-1000.0 .. +1000.0)\n");
	resetState_lineWidth();

	for (i = -10000; i <= 10000; i++) {
		float w = (float)i * 0.1f;
		GLenum expected = (w <= 0.0f) ? GL_INVALID_VALUE : GL_NO_ERROR;
		GLenum err;

		glLineWidth(w);
		err = glGetError();

		if (err != expected) {
			printf("  [FAIL] w=%.1f, beklenen 0x%X, gelen 0x%X\n",
			       w, expected, err);
			failCount++;
		} else {
			passCount++;
		}
	}

	glLineWidth(1.0f);
	checkStatePreserved_lineWidth(1.0f);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(
		    _e, (_e == GL_NO_ERROR),
		    "test_lineWidth_stressSweep failed. Residual error.");
	}

	printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
	EXPECT_GL_ERROR(failCount, (failCount == 0),
			"test_lineWidth_stressSweep failed.");

	resetState_lineWidth();
}

/* ============================================================
 * glLineWidth — IEEE-754 Özel Degerleri
 * ============================================================
 *
 * NaN, +Infinity, -Infinity degerlerinin glLineWidth
 * tarafindan nasil ele alindigini gözlemler. OpenGL spec
 * bu degerler için kesin bir davranis belirtmez; farkli
 * sürücüler farkli hata kodlari döndürebilir veya sessizce
 * kirpabilir. Testin amaci implementasyonun çökmedigini
 * ve durumu bozmadigini kontrol etmektir.
 * ============================================================ */
void test_lineWidth_specialFloats(void) {
	GLenum err;
	GLfloat width;

	printf("TEST: Special Float Values (NaN, Inf)\n");
	resetState_lineWidth();

	glLineWidth(NAN);
	err = glGetError();
	printf("  NaN       -> 0x%X (beklenen INVALID_VALUE=0x%X)\n", err,
	       GL_INVALID_VALUE);

	glLineWidth(INFINITY);
	err = glGetError();
	printf("  +INFINITY -> 0x%X\n", err);

	glLineWidth(-INFINITY);
	err = glGetError();
	printf("  -INFINITY -> 0x%X (beklenen INVALID_VALUE=0x%X)\n", err,
	       GL_INVALID_VALUE);

	glGetFloatv(GL_LINE_WIDTH, &width);
	printf("  Son durum: width=%f (beklenen 1.0)\n", width);

	resetState_lineWidth();
	printf("  [BILGI] Manuel inceleme gerekir\n");
}

/* ============================================================
 * glLineWidth — Hata Kuyrugu Bütünlügü
 * ============================================================
 *
 * Sürekli hata enjeksiyonu altinda hata kuyrugu davranisini
 * dogrular. Hatalar glGetError ile kaydedilir, kuyruk
 * GL_NO_ERROR'a kadar tamamen bosaltilabilir olmalidir.
 * ============================================================ */
void test_lineWidth_errorQueue(void) {
	int i;
	GLenum err;
	int errorCount = 0;

	printf("TEST: Error Queue Management\n");
	resetState_lineWidth();

	for (i = 0; i < 50; i++) {
		glLineWidth(-1.0f * (i + 1));
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
				"test_lineWidth_errorQueue failed. Unexpected "
				"error code in queue.");
		errorCount++;
	}

	printf("  Kuyruktan okunan hata sayisi: %d\n", errorCount);
	EXPECT_GL_ERROR(
	    errorCount, (errorCount > 0),
	    "test_lineWidth_errorQueue failed. No errors recorded.");

	glLineWidth(4.0f);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(
		    _e, (_e == GL_NO_ERROR),
		    "test_lineWidth_errorQueue failed. Post-drain error.");
	}
	checkStatePreserved_lineWidth(4.0f);

	resetState_lineWidth();
}

/* ============================================================
 * glLineWidth — Implementasyon Limitleri
 * ============================================================
 *
 * GL_ALIASED_LINE_WIDTH_RANGE ile desteklenen aralik
 * sorgulanir. Aralik üstü pozitif degerin hata üretmedigini
 * ve GL_LINE_WIDTH sorgusunun specified degeri döndürdügünü
 * dogrular.
 * ============================================================ */
void test_lineWidth_limits(void) {
	GLfloat range[2];
	GLfloat width;
	GLfloat request;
	GLenum err;

	printf("TEST: Implementation Limits (Aliased Line Width Range)\n");
	resetState_lineWidth();

	range[0] = range[1] = -1.0f;
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, range);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_lineWidth_limits failed. Range query error.");

	printf("  ALIASED_LINE_WIDTH_RANGE: [%.2f, %.2f]\n", range[0],
	       range[1]);

	EXPECT_GL_ERROR(
	    range[0], (range[0] > 0.0f),
	    "test_lineWidth_limits failed. Min range not positive.");
	EXPECT_GL_ERROR(range[1], (range[1] >= range[0]),
			"test_lineWidth_limits failed. Max < Min.");
	EXPECT_GL_ERROR(
	    range[0], (range[0] <= 1.0f),
	    "test_lineWidth_limits failed. Range does not include 1.0.");
	EXPECT_GL_ERROR(
	    range[1], (range[1] >= 1.0f),
	    "test_lineWidth_limits failed. Range does not include 1.0.");

	request = range[1] * 10.0f;
	glLineWidth(request);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_lineWidth_limits failed. Over-range positive "
			"width rejected.");

	glGetFloatv(GL_LINE_WIDTH, &width);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_lineWidth_limits failed.");
	}
	if (fabsf(width - request) > request * 1e-5f) {
		printf("  [FAIL] state erken clamp'lenmis: istek %.1f, sorgu "
		       "%.1f\n",
		       request, width);
		retcode = 1;
	}
	printf("  Istek %.1f -> GL_LINE_WIDTH %.1f (specified korunuyor)\n",
	       request, width);

	glLineWidth(range[0]);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(
		    _e, (_e == GL_NO_ERROR),
		    "test_lineWidth_limits failed. Min range rejected.");
	}

	resetState_lineWidth();
}

/* ============================================================
 * glCullFace — Temel Robustness
 * ============================================================
 *
 * glCullFace() fonksiyonunun kabul ettigi üç geçerli enum
 * degeri dogrulanir. Ardindan geçersiz enum degerleri
 * gönderilerek GL_INVALID_ENUM üretildigi ve mevcut state'in
 * degismedigi kontrol edilir.
 * ============================================================ */
void test_cullFace_basicRobustness(void) {
	GLenum err;
	printf("TEST : Basic Robustness\n");
	resetState_cullFace();
	glCullFace(GL_BACK);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFace_basicRobustness failed.");
	}
	checkStatePreserved_cullFace(GL_BACK);

	glCullFace(GL_FRONT);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFace_basicRobustness failed.");
	}
	checkStatePreserved_cullFace(GL_FRONT);

	glCullFace(GL_FRONT_AND_BACK);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFace_basicRobustness failed.");
	}
	checkStatePreserved_cullFace(GL_FRONT_AND_BACK);

	glCullFace((GLenum)0x0BAD);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_ENUM),
	    "test_cullFace_basicRobustness failed. Invalid enum accepted.");
	checkStatePreserved_cullFace(GL_FRONT_AND_BACK);

	glCullFace(GL_CCW);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"test_cullFace_basicRobustness failed. GL_CCW accepted "
			"as cull mode.");
	checkStatePreserved_cullFace(GL_FRONT_AND_BACK);
	resetState_cullFace();
}

/* ============================================================
 * glCullFace — Stres Taramasi
 * ============================================================
 *
 * 16-bit GLenum uzayindaki tüm degerler sistematik olarak
 * denenir. Yalnizca GL_BACK, GL_FRONT, GL_FRONT_AND_BACK
 * degerlerinin kabul edilmesi beklenir.
 * ============================================================ */
void test_cullFace_stressSweep(void) {
	GLenum mode;
	int passCount = 0;
	int failCount = 0;
	printf("TEST : Stress Sweep\n");
	resetState_cullFace();

	for (mode = 0; mode < 65536; mode++) {
		glCullFace(GL_BACK);
		while (glGetError() != GL_NO_ERROR)
			;

		GLenum expected = (mode == GL_BACK || mode == GL_FRONT ||
				   mode == GL_FRONT_AND_BACK)
				      ? GL_NO_ERROR
				      : GL_INVALID_ENUM;

		GLenum err;
		glCullFace(mode);
		err = glGetError();
		if (err != expected) {
			printf("  [FAIL] Enum=0x%X Beklenen=0x%X Gelen=0x%X\n",
			       mode, expected, err);
			failCount++;
		} else {
			passCount++;
		}

		if (err == GL_INVALID_ENUM) {
			checkStatePreserved_cullFace(GL_BACK);
		}
	}

	printf("  PASS : %d\n", passCount);
	printf("  FAIL : %d\n", failCount);

	EXPECT_GL_ERROR(failCount, (failCount == 0),
			"test_cullFace_stressSweep failed.");
	resetState_cullFace();
}

/* ============================================================
 * glCullFace — Hata Kuyrugu Yönetimi
 * ============================================================
 *
 * Arka arkaya çok sayida geçersiz enum gönderildiginde hata
 * kuyrugunun bozulmadigi dogrulanir. Daha sonra geçerli bir
 * çagri yapilarak sürücünün normal çalismaya döndügü kontrol
 * edilir.
 * ============================================================ */
void test_cullFace_errorQueue(void) {
	GLenum err;
	int i;
	int errorCount = 0;
	printf("TEST : Error Queue Management\n");
	resetState_cullFace();
	for (i = 0; i < 100; i++) {
		glCullFace((GLenum)(0x5000 + i));
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_cullFace_errorQueue failed. Unexpected "
				"error in queue.");
		errorCount++;
	}
	EXPECT_GL_ERROR(errorCount, (errorCount > 0),
			"test_cullFace_errorQueue failed. No errors recorded.");
	glCullFace(GL_FRONT);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFace_errorQueue failed.");
	}
	checkStatePreserved_cullFace(GL_FRONT);
	resetState_cullFace();
}

/* ============================================================
 * glCullFace — Hizli Geçis (Rapid Toggle)
 * ============================================================
 *
 * GL_BACK, GL_FRONT ve GL_FRONT_AND_BACK arasinda yüz binlerce
 * kez geçis yapilarak OpenGL durum makinesinin kararlilığı
 * dogrulanir.
 * ============================================================ */
void test_cullFace_rapidToggle(void) {
	const int repeat = 100000;
	int i;
	printf("TEST : Rapid Toggle\n");
	resetState_cullFace();
	for (i = 0; i < repeat; i++) {
		GLenum expected;
		GLint current;
		switch (i % 3) {
		case 0:
			expected = GL_BACK;
			break;
		case 1:
			expected = GL_FRONT;
			break;
		default:
			expected = GL_FRONT_AND_BACK;
			break;
		}
		glCullFace(expected);
		{
			GLenum _e = glGetError();
			EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
					"test_cullFace_rapidToggle failed.");
		}
		glGetIntegerv(GL_CULL_FACE_MODE, &current);
		EXPECT_GL_ERROR(
		    current, (current == (GLint)expected),
		    "test_cullFace_rapidToggle failed. State mismatch.");
	}
	resetState_cullFace();
}

/* ============================================================
 * glCullFace — Durum Korunumu (State Preservation)
 * ============================================================
 *
 * Geçersiz glCullFace() çagrilarinin mevcut
 * GL_CULL_FACE_MODE degerini degistirmedigi dogrulanir.
 * ============================================================ */
void test_cullFace_statePreservation(void) {
	GLenum err;
	GLenum invalidEnums[] = {0, 1, 2, 1234, 9999, 0xFFFF, 0xFFFFFFFF};
	int i;
	printf("TEST : State Preservation\n");
	resetState_cullFace();
	glCullFace(GL_FRONT);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFace_statePreservation failed.");
	}
	checkStatePreserved_cullFace(GL_FRONT);
	for (i = 0; i < (int)(sizeof(invalidEnums) / sizeof(invalidEnums[0]));
	     i++) {
		glCullFace(invalidEnums[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_cullFace_statePreservation failed. "
				"Invalid enum not rejected.");
		checkStatePreserved_cullFace(GL_FRONT);
	}
	resetState_cullFace();
}

/* ============================================================
 * glCullFace — FrontFace Kombinasyonlari
 * ============================================================
 *
 * glFrontFace() ile glCullFace() fonksiyonlarinin birlikte
 * kullanildiginda birbirlerinin durumunu bozmadigi dogrulanir.
 * ============================================================ */
void test_cullFace_frontFaceCombination(void) {
	GLenum frontModes[] = {GL_CCW, GL_CW};
	GLenum cullModes[] = {GL_BACK, GL_FRONT, GL_FRONT_AND_BACK};
	int i, j;
	printf("TEST : FrontFace Combination\n");
	resetState_cullFace();
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			GLint front, cull;
			glFrontFace(frontModes[i]);
			{
				GLenum _e = glGetError();
				EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
						"test_cullFace_"
						"frontFaceCombination failed.");
			}
			glCullFace(cullModes[j]);
			{
				GLenum _e = glGetError();
				EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
						"test_cullFace_"
						"frontFaceCombination failed.");
			}
			glGetIntegerv(GL_FRONT_FACE, &front);
			glGetIntegerv(GL_CULL_FACE_MODE, &cull);
			EXPECT_GL_ERROR(front, (front == (GLint)frontModes[i]),
					"test_cullFace_frontFaceCombination "
					"failed. Front mismatch.");
			EXPECT_GL_ERROR(cull, (cull == (GLint)cullModes[j]),
					"test_cullFace_frontFaceCombination "
					"failed. Cull mismatch.");
		}
	}
	resetState_cullFace();
}

/* ============================================================
 * glCullFace — Büyük Geçersiz Enum
 * ============================================================
 *
 * Çok büyük GLenum degerleri gönderildiginde sürücünün
 * çökmedigini ve GL_INVALID_ENUM ürettigini dogrular.
 * ============================================================ */
void test_cullFace_largeEnum(void) {
	GLenum values[] = {(GLenum)0x10000, (GLenum)0x7FFFFFFF,
			   (GLenum)0x80000000, (GLenum)0xFFFFFFFF};
	int i;
	printf("TEST : Large Invalid Enum Values\n");
	resetState_cullFace();
	for (i = 0; i < (int)(sizeof(values) / sizeof(values[0])); i++) {
		GLenum err;
		glCullFace(values[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_cullFace_largeEnum failed.");
		checkStatePreserved_cullFace(GL_BACK);
	}
	resetState_cullFace();
}

/* ============================================================
 * glCullFace — Rapid Fire
 * ============================================================
 *
 * glCullFace() fonksiyonunu çok kisa araliklarla art arda
 * çagirarak sürücünün yogun kullanim altinda kararlilığını
 * dogrular.
 * ============================================================ */
void test_cullFace_rapidFire(void) {
	const unsigned int repeat = 1000000;
	unsigned int i;
	printf("TEST : Rapid Fire\n");
	resetState_cullFace();
	for (i = 0; i < repeat; i++) {
		glCullFace(GL_BACK);
		glCullFace(GL_FRONT);
		glCullFace(GL_FRONT_AND_BACK);
	}
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFace_rapidFire failed.");
	}
	checkStatePreserved_cullFace(GL_FRONT_AND_BACK);
	resetState_cullFace();
	printf("  %u cagri tamamlandi.\n", repeat * 3);
}

/* ============================================================
 * glCullFace — Rastgele Fuzz Testi
 * ============================================================
 *
 * Rastgele GLenum degerleri gönderilerek sürücünün
 * beklenmeyen girdiler karsisindaki dayaniklilığı test edilir.
 * ============================================================ */
void test_cullFace_randomFuzz(void) {
	unsigned int i;
	printf("TEST : Random Fuzz Test\n");
	resetState_cullFace();
	srand(12345);
	for (i = 0; i < 1000000; i++) {
		GLenum value;
		GLenum err;
		switch (rand() % 5) {
		case 0:
			value = GL_BACK;
			break;
		case 1:
			value = GL_FRONT;
			break;
		case 2:
			value = GL_FRONT_AND_BACK;
			break;
		default:
			value = (GLenum)rand();
			break;
		}
		glCullFace(value);
		err = glGetError();
		EXPECT_GL_ERROR(
		    err, (err == GL_NO_ERROR || err == GL_INVALID_ENUM),
		    "test_cullFace_randomFuzz failed. Unexpected error code.");
	}
	resetState_cullFace();
	printf("  1,000,000 rastgele test tamamlandi.\n");
}

/* ============================================================
 * glEnable/Disable — Temel Robustness
 * ============================================================
 *
 * glEnable/glDisable(GL_CULL_FACE)'in temel sözlesmesini,
 * gerçek culling davranisini ve cap izolasyonunu dogrular.
 * ============================================================ */
void test_cullFaceEnable_basicRobustness(void) {
	printf("TEST: Enable/Disable Basic Robustness\n");
	resetState_cullFaceEnable();

	glEnable(GL_CULL_FACE);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFaceEnable_basicRobustness failed.");
	}
	EXPECT_GL_ERROR(
	    glIsEnabled(GL_CULL_FACE), (glIsEnabled(GL_CULL_FACE) == GL_TRUE),
	    "test_cullFaceEnable_basicRobustness failed. Enable did not work.");

	glDisable(GL_CULL_FACE);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFaceEnable_basicRobustness failed.");
	}
	EXPECT_GL_ERROR(glIsEnabled(GL_CULL_FACE),
			(glIsEnabled(GL_CULL_FACE) == GL_FALSE),
			"test_cullFaceEnable_basicRobustness failed. Disable "
			"did not work.");

	glDisable(GL_CULL_FACE);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFaceEnable_basicRobustness failed. "
				"Idempotent disable.");
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFaceEnable_basicRobustness failed. "
				"Double enable.");
	}

	glDisable(GL_CULL_FACE);
	glEnable((GLenum)0x0BAD);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_INVALID_ENUM),
				"test_cullFaceEnable_basicRobustness failed. "
				"Invalid cap accepted.");
	}
	EXPECT_GL_ERROR(glIsEnabled(GL_CULL_FACE),
			(glIsEnabled(GL_CULL_FACE) == GL_FALSE),
			"test_cullFaceEnable_basicRobustness failed. Invalid "
			"cap changed state.");

	glEnable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_cullFaceEnable_basicRobustness failed.");
	}
	glDisable(GL_SCISSOR_TEST);
	EXPECT_GL_ERROR(glIsEnabled(GL_CULL_FACE),
			(glIsEnabled(GL_CULL_FACE) == GL_TRUE),
			"test_cullFaceEnable_basicRobustness failed. Cap "
			"isolation broken.");

	resetState_cullFaceEnable();
}

/* ============================================================
 * glEnable/Disable — Hizli Toggle
 * ============================================================ */
void test_cullFaceEnable_rapidToggle(void) {
	int i;
	const int tekrar = 10000;
	printf("TEST: Rapid Toggle (Enable <-> Disable)\n");
	resetState_cullFaceEnable();
	for (i = 0; i < tekrar; i++) {
		GLboolean beklenen = (i % 2 == 0) ? GL_TRUE : GL_FALSE;
		if (beklenen)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		{
			GLenum _e = glGetError();
			EXPECT_GL_ERROR(
			    _e, (_e == GL_NO_ERROR),
			    "test_cullFaceEnable_rapidToggle failed.");
		}
		EXPECT_GL_ERROR(
		    glIsEnabled(GL_CULL_FACE),
		    (glIsEnabled(GL_CULL_FACE) == beklenen),
		    "test_cullFaceEnable_rapidToggle failed. State mismatch.");
	}
	resetState_cullFaceEnable();
	printf("  Sonuc: %d toggle tamamlandi\n", tekrar);
}

/* ============================================================
 * glEnable/Disable — Geçersiz Cap Taramasi
 * ============================================================ */
void test_cullFaceEnable_invalidCaps(void) {
	GLenum invalidCaps[] = {(GLenum)0x0000, (GLenum)0x0BAD, (GLenum)0x1234,
				(GLenum)0xDEAD, (GLenum)0xFFFF};
	int i;
	int n = sizeof(invalidCaps) / sizeof(invalidCaps[0]);
	printf("TEST: Invalid Capability Values\n");
	resetState_cullFaceEnable();
	for (i = 0; i < n; i++) {
		GLenum err;
		glEnable(invalidCaps[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_cullFaceEnable_invalidCaps failed. "
				"Enable accepted invalid cap.");
		glDisable(invalidCaps[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_cullFaceEnable_invalidCaps failed. "
				"Disable accepted invalid cap.");
	}
	EXPECT_GL_ERROR(
	    glIsEnabled(GL_CULL_FACE), (glIsEnabled(GL_CULL_FACE) == GL_FALSE),
	    "test_cullFaceEnable_invalidCaps failed. State changed.");
	resetState_cullFaceEnable();
	printf("  Sonuc: %d gecersiz cap reddedildi\n", n);
}

/* ============================================================
 * glEnable/Disable — Cap Kombinasyonlari
 * ============================================================ */
void test_cullFaceEnable_capCombinations(void) {
	printf("TEST: Capability Combinations\n");
	resetState_cullFaceEnable();

	EXPECT_GL_ERROR(glIsEnabled(GL_CULL_FACE),
			(glIsEnabled(GL_CULL_FACE) == GL_FALSE),
			"test_cullFaceEnable_capCombinations failed.");
	EXPECT_GL_ERROR(glIsEnabled(GL_SCISSOR_TEST),
			(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE),
			"test_cullFaceEnable_capCombinations failed.");

	glEnable(GL_CULL_FACE);
	EXPECT_GL_ERROR(glIsEnabled(GL_CULL_FACE),
			(glIsEnabled(GL_CULL_FACE) == GL_TRUE),
			"test_cullFaceEnable_capCombinations failed.");
	EXPECT_GL_ERROR(glIsEnabled(GL_SCISSOR_TEST),
			(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE),
			"test_cullFaceEnable_capCombinations failed.");

	glEnable(GL_SCISSOR_TEST);
	EXPECT_GL_ERROR(glIsEnabled(GL_CULL_FACE),
			(glIsEnabled(GL_CULL_FACE) == GL_TRUE),
			"test_cullFaceEnable_capCombinations failed.");
	EXPECT_GL_ERROR(glIsEnabled(GL_SCISSOR_TEST),
			(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE),
			"test_cullFaceEnable_capCombinations failed.");

	glDisable(GL_CULL_FACE);
	EXPECT_GL_ERROR(glIsEnabled(GL_CULL_FACE),
			(glIsEnabled(GL_CULL_FACE) == GL_FALSE),
			"test_cullFaceEnable_capCombinations failed.");
	EXPECT_GL_ERROR(glIsEnabled(GL_SCISSOR_TEST),
			(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE),
			"test_cullFaceEnable_capCombinations failed.");

	glDisable(GL_SCISSOR_TEST);
	resetState_cullFaceEnable();
}

/* ============================================================
 * glFrontFace — Hata Kuyrugu Yönetimi
 * ============================================================ */
void test_frontFace_errorQueue(void) {
	GLenum err;
	int errorCount = 0;
	int i;
	printf("TEST : Error Queue Management\n");
	resetState_frontFace();
	for (i = 0; i < 100; i++) {
		glFrontFace((GLenum)(0x5000 + i));
	}
	while ((err = glGetError()) != GL_NO_ERROR) {
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_frontFace_errorQueue failed.");
		errorCount++;
	}
	EXPECT_GL_ERROR(errorCount, (errorCount > 0),
			"test_frontFace_errorQueue failed. No errors.");
	glFrontFace(GL_CW);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_frontFace_errorQueue failed.");
	}
	checkStatePreserved_frontFace(GL_CW);
	resetState_frontFace();
}

/* ============================================================
 * glFrontFace — Hizli Geçis
 * ============================================================ */
void test_frontFace_rapidToggle(void) {
	const int repeat = 100000;
	int i;
	printf("TEST : Rapid Toggle\n");
	resetState_frontFace();
	for (i = 0; i < repeat; i++) {
		GLenum expected = (i & 1) ? GL_CCW : GL_CW;
		GLint current;
		glFrontFace(expected);
		{
			GLenum _e = glGetError();
			EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
					"test_frontFace_rapidToggle failed.");
		}
		glGetIntegerv(GL_FRONT_FACE, &current);
		EXPECT_GL_ERROR(current, (current == (GLint)expected),
				"test_frontFace_rapidToggle failed.");
	}
	resetState_frontFace();
}

/* ============================================================
 * glFrontFace — Karisik Geçerli/Geçersiz Çagrilar
 * ============================================================ */
void test_frontFace_mixedValidity(void) {
	GLenum sequence[] = {GL_CW, 0x1111, GL_CCW, 0x2222,
			     GL_CW, 0x3333, GL_CCW, 0x4444};
	int count = sizeof(sequence) / sizeof(sequence[0]);
	int i;
	printf("TEST : Mixed Validity\n");
	resetState_frontFace();
	for (i = 0; i < count; i++) {
		GLenum value = sequence[i];
		GLenum expectedError = (value == GL_CW || value == GL_CCW)
					   ? GL_NO_ERROR
					   : GL_INVALID_ENUM;
		glFrontFace(value);
		{
			GLenum _e = glGetError();
			EXPECT_GL_ERROR(_e, (_e == expectedError),
					"test_frontFace_mixedValidity failed.");
		}
		if (value == GL_CW)
			checkStatePreserved_frontFace(GL_CW);
		if (value == GL_CCW)
			checkStatePreserved_frontFace(GL_CCW);
	}
	resetState_frontFace();
}

/* ============================================================
 * glFrontFace — Durum Korunumu
 * ============================================================ */
void test_frontFace_statePreservation(void) {
	GLenum err;
	GLenum invalidEnums[] = {0, 1, 2, 1234, 9999, 0xFFFF, 0xFFFFFFFF};
	int i;
	printf("TEST : State Preservation\n");
	resetState_frontFace();
	glFrontFace(GL_CW);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_frontFace_statePreservation failed.");
	}
	checkStatePreserved_frontFace(GL_CW);
	for (i = 0; i < (int)(sizeof(invalidEnums) / sizeof(invalidEnums[0]));
	     i++) {
		glFrontFace(invalidEnums[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_frontFace_statePreservation failed.");
		checkStatePreserved_frontFace(GL_CW);
	}
	resetState_frontFace();
}

/* ============================================================
 * glFrontFace — CullFace Kombinasyonlari
 * ============================================================ */
void test_frontFace_cullCombinations(void) {
	GLenum frontModes[] = {GL_CCW, GL_CW};
	GLenum cullModes[] = {GL_FRONT, GL_BACK, GL_FRONT_AND_BACK};
	int i, j;
	printf("TEST : Cull Face Combinations\n");
	resetState_frontFace();
	glEnable(GL_CULL_FACE);
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			GLint currentFront, currentCull;
			glFrontFace(frontModes[i]);
			{
				GLenum _e = glGetError();
				EXPECT_GL_ERROR(
				    _e, (_e == GL_NO_ERROR),
				    "test_frontFace_cullCombinations failed.");
			}
			glCullFace(cullModes[j]);
			{
				GLenum _e = glGetError();
				EXPECT_GL_ERROR(
				    _e, (_e == GL_NO_ERROR),
				    "test_frontFace_cullCombinations failed.");
			}
			glGetIntegerv(GL_FRONT_FACE, &currentFront);
			glGetIntegerv(GL_CULL_FACE_MODE, &currentCull);
			EXPECT_GL_ERROR(
			    currentFront,
			    (currentFront == (GLint)frontModes[i]),
			    "test_frontFace_cullCombinations failed.");
			EXPECT_GL_ERROR(
			    currentCull, (currentCull == (GLint)cullModes[j]),
			    "test_frontFace_cullCombinations failed.");
		}
	}
	glDisable(GL_CULL_FACE);
	resetState_frontFace();
}

/* ============================================================
 * glFrontFace — Büyük Geçersiz Enum
 * ============================================================ */
void test_frontFace_largeEnum(void) {
	GLenum values[] = {(GLenum)0x10000, (GLenum)0x7FFFFFFF,
			   (GLenum)0x80000000, (GLenum)0xFFFFFFFF};
	int i;
	printf("TEST : Large Invalid Enum Values\n");
	resetState_frontFace();
	for (i = 0; i < (int)(sizeof(values) / sizeof(values[0])); i++) {
		GLenum err;
		glFrontFace(values[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
				"test_frontFace_largeEnum failed.");
		checkStatePreserved_frontFace(GL_CCW);
	}
	resetState_frontFace();
}

/* ============================================================
 * glFrontFace — Rapid Fire
 * ============================================================ */
void test_frontFace_rapidFire(void) {
	const unsigned int repeat = 1000000;
	unsigned int i;
	printf("TEST : Rapid Fire\n");
	resetState_frontFace();
	for (i = 0; i < repeat; i++) {
		glFrontFace(GL_CW);
		glFrontFace(GL_CCW);
	}
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_frontFace_rapidFire failed.");
	}
	checkStatePreserved_frontFace(GL_CCW);
	resetState_frontFace();
	printf("  %u cift cagri tamamlandi.\n", repeat);
}

/* ============================================================
 * glFrontFace — Rastgele Fuzz Testi
 * ============================================================ */
void test_frontFace_randomFuzz(void) {
	unsigned int i;
	printf("TEST : Random Fuzz Test\n");
	resetState_frontFace();
	srand(12345);
	for (i = 0; i < 1000000; i++) {
		GLenum value;
		GLenum err;
		switch (rand() % 4) {
		case 0:
			value = GL_CW;
			break;
		case 1:
			value = GL_CCW;
			break;
		default:
			value = (GLenum)rand();
			break;
		}
		glFrontFace(value);
		err = glGetError();
		EXPECT_GL_ERROR(err,
				(err == GL_NO_ERROR || err == GL_INVALID_ENUM),
				"test_frontFace_randomFuzz failed.");
	}
	resetState_frontFace();
	printf("  1,000,000 rastgele test tamamlandi.\n");
}

/* ============================================================
 * glPolygonOffset — Temel Robustness
 * ============================================================ */
void test_polygonOffset_basicRobustness(void) {
	GLfloat f = 0.0f, u = 0.0f;
	printf("TEST: Basic Robustness\n");
	resetState_polygonOffset();
	glPolygonOffset(0.0f, 0.0f);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_polygonOffset_basicRobustness failed.");
	}
	glPolygonOffset(-1000.0f, -1000.0f);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_polygonOffset_basicRobustness failed.");
	}
	glPolygonOffset(1e30f, 1e30f);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_polygonOffset_basicRobustness failed.");
	}
	glPolygonOffset(NAN, NAN);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_polygonOffset_basicRobustness failed.");
	}
	glPolygonOffset(INFINITY, INFINITY);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_NO_ERROR),
				"test_polygonOffset_basicRobustness failed.");
	}
	glPolygonOffset(2.0f, 3.0f);
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
	EXPECT_GL_ERROR(
	    f, (f == 2.0f && u == 3.0f),
	    "test_polygonOffset_basicRobustness failed. State mismatch.");
	resetState_polygonOffset();
}

/* ============================================================
 * glPolygonOffset — Stres Taramasi
 * ============================================================ */
void test_polygonOffset_stressSweep(void) {
	int i;
	int passCount = 0;
	int failCount = 0;
	printf("TEST: Stress Sweep\n");
	resetState_polygonOffset();
	for (i = -10000; i <= 10000; i++) {
		float val = (float)i * 0.1f;
		glPolygonOffset(val, val);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("  [FAIL] val=%.1f -> 0x%X\n", val, err);
			failCount++;
		} else {
			passCount++;
		}
	}
	printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
	EXPECT_GL_ERROR(failCount, (failCount == 0),
			"test_polygonOffset_stressSweep failed.");
	resetState_polygonOffset();
}

/* ============================================================
 * glPolygonOffset — Hata Kuyrugu Bütünlügü
 * ============================================================ */
void test_polygonOffset_errorQueue(void) {
	int i;
	printf("TEST: Error Queue Integrity\n");
	resetState_polygonOffset();
	for (i = 0; i < 1000; i++) {
		glPolygonOffset((float)i, (float)-i);
	}
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_polygonOffset_errorQueue failed.");
	printf("  1000 cagri sonrasi kuyruk temiz\n");
	resetState_polygonOffset();
}

/* ============================================================
 * glPolygonOffset — Durum Korunumu
 * ============================================================ */
void test_polygonOffset_statePreservation(void) {
	GLfloat f = 0.0f, u = 0.0f;
	printf("TEST: State Preservation\n");
	resetState_polygonOffset();
	glPolygonOffset(5.0f, 7.0f);
	glFrontFace((GLenum)0x0BAD);
	{
		GLenum _e = glGetError();
		EXPECT_GL_ERROR(_e, (_e == GL_INVALID_ENUM),
				"test_polygonOffset_statePreservation failed.");
	}
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
	EXPECT_GL_ERROR(
	    f, (f == 5.0f && u == 7.0f),
	    "test_polygonOffset_statePreservation failed. State corrupted.");
	resetState_polygonOffset();
}

/* ============================================================
 * glPolygonOffset — Özel Float Degerleri
 * ============================================================ */
void test_polygonOffset_specialFloats(void) {
	GLenum err;
	GLfloat f, u;
	printf("TEST: Special Float Values\n");
	resetState_polygonOffset();
	glPolygonOffset(NAN, NAN);
	err = glGetError();
	printf("  NaN       -> 0x%X\n", err);
	glPolygonOffset(INFINITY, INFINITY);
	err = glGetError();
	printf("  +INFINITY -> 0x%X\n", err);
	glPolygonOffset(-INFINITY, -INFINITY);
	err = glGetError();
	printf("  -INFINITY -> 0x%X\n", err);
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
	printf("  Son durum: factor=%f, units=%f\n", f, u);
	resetState_polygonOffset();
	printf("  [BILGI] Manuel inceleme gerekir\n");
}

/****************************************/
/***** Viewport and Clipping *****/
/****************************************/

/* ============================================================
 * glViewport — Temel Robustness
 * ============================================================ */
void test_viewport_basicRobustness(void) {
	GLint viewport[4];
	GLenum err;
	printf("TEST: Basic Robustness\n");
	resetState_viewport();
	glViewport(10, 20, 640, 480);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_viewport_basicRobustness failed.");
	glGetIntegerv(GL_VIEWPORT, viewport);
	EXPECT_GL_ERROR(
	    viewport[0],
	    (viewport[0] == 10 && viewport[1] == 20 && viewport[2] == 640 &&
	     viewport[3] == 480),
	    "test_viewport_basicRobustness failed. Viewport not set.");
	glViewport(10, 20, -1, 480);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "test_viewport_basicRobustness failed. Negative width accepted.");
	checkStatePreserved_viewport(10, 20, 640, 480);
	glViewport(10, 20, 640, -1);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "test_viewport_basicRobustness failed. Negative height accepted.");
	checkStatePreserved_viewport(10, 20, 640, 480);
	resetState_viewport();
}

/* ============================================================
 * glViewport — Negatif Boyut Taramasi
 * ============================================================ */
void test_viewport_negativeDimensions(void) {
	int w, h;
	int passCount = 0;
	int failCount = 0;
	printf("TEST: Negative Dimension Sweep\n");
	resetState_viewport();
	for (w = -100; w <= 100; w++) {
		for (h = -100; h <= 100; h++) {
			GLenum expected =
			    (w < 0 || h < 0) ? GL_INVALID_VALUE : GL_NO_ERROR;
			glViewport(0, 0, w, h);
			GLenum err = glGetError();
			if (err != expected) {
				failCount++;
			} else {
				passCount++;
			}
		}
	}
	printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
	EXPECT_GL_ERROR(failCount, (failCount == 0),
			"test_viewport_negativeDimensions failed.");
	resetState_viewport();
}

/* ============================================================
 * glViewport — Sinir Koordinatlari
 * ============================================================ */
void test_viewport_boundaryCoordinates(void) {
	GLint viewport[4];
	GLenum err;
	printf("TEST: Boundary Coordinates\n");
	resetState_viewport();
	GLint coordinates[][2] = {{0, 0},
				  {-1, -1},
				  {INT_MAX, INT_MAX},
				  {INT_MIN, INT_MIN},
				  {INT_MAX, INT_MIN},
				  {INT_MIN, INT_MAX},
				  {-1000000, -1000000},
				  {1000000, 1000000}};
	int count = sizeof(coordinates) / sizeof(coordinates[0]);
	for (int i = 0; i < count; i++) {
		glViewport(coordinates[i][0], coordinates[i][1], 640, 480);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
				"test_viewport_boundaryCoordinates failed.");
	}
	resetState_viewport();
}

/* ============================================================
 * glViewport — Maksimum Boyutlar
 * ============================================================ */
void test_viewport_limits(void) {
	GLint maxViewport[2];
	GLenum err;
	printf("TEST: Maximum Viewport Limits\n");
	resetState_viewport();
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewport);
	printf("  Desteklenen maksimum viewport : %d x %d\n", maxViewport[0],
	       maxViewport[1]);
	glViewport(0, 0, maxViewport[0], maxViewport[1]);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_viewport_limits failed. Max viewport rejected.");
	glViewport(0, 0, maxViewport[0] * 2, maxViewport[1] * 2);
	err = glGetError();
	if (err != GL_NO_ERROR)
		printf("  Uyari : Buyuk viewport Error=0x%X\n", err);
	glViewport(0, 0, INT_MAX, INT_MAX);
	err = glGetError();
	if (err != GL_NO_ERROR)
		printf("  Uyari : INT_MAX Error=0x%X\n", err);
	resetState_viewport();
}

/* ============================================================
 * glViewport — Hata Kuyrugu ve Durum Korunumu
 * ============================================================ */
void test_viewport_errorQueue(void) {
	GLenum err;
	printf("TEST: Error Queue and State Preservation\n");
	resetState_viewport();
	glViewport(50, 50, 400, 300);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_viewport_errorQueue failed.");
	glViewport(50, 50, -1, 300);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_INVALID_VALUE),
	    "test_viewport_errorQueue failed. Negative width accepted.");
	checkStatePreserved_viewport(50, 50, 400, 300);
	glViewport(0, 0, 640, 480);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_NO_ERROR),
	    "test_viewport_errorQueue failed. Post-error call failed.");
	resetState_viewport();
}

/* ============================================================
 * glViewport — Rastgele Stres Testi
 * ============================================================ */
void test_viewport_stress(void) {
	unsigned int i;
	printf("TEST: Random Stress Test\n");
	resetState_viewport();
	srand(12345);
	for (i = 0; i < 1000000; i++) {
		GLint x = (rand() % 2000000000) - 1000000000;
		GLint y = (rand() % 2000000000) - 1000000000;
		GLsizei width = (rand() % 2000000000) - 1000000000;
		GLsizei height = (rand() % 2000000000) - 1000000000;
		GLenum expected =
		    (width < 0 || height < 0) ? GL_INVALID_VALUE : GL_NO_ERROR;
		glViewport(x, y, width, height);
		GLenum err = glGetError();
		EXPECT_GL_ERROR(err, (err == expected),
				"test_viewport_stress failed.");
	}
	resetState_viewport();
	printf("  1,000,000 rastgele viewport testi tamamlandi.\n");
}

/* ============================================================
 * glDepthRange — Temel Robustness
 * ============================================================ */
void test_depthRange_basicRobustness(void) {
	GLdouble depthRange[2];
	GLenum err;
	printf("TEST: Basic Robustness\n");
	resetState_depthRange();
	glDepthRange(0.0, 1.0);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_depthRange_basicRobustness failed.");
	glGetDoublev(GL_DEPTH_RANGE, depthRange);
	EXPECT_GL_ERROR(depthRange[0],
			(fabs(depthRange[0] - 0.0) < 0.000001 &&
			 fabs(depthRange[1] - 1.0) < 0.000001),
			"test_depthRange_basicRobustness failed.");
	glDepthRange(1.0, 0.0);
	err = glGetError();
	EXPECT_GL_ERROR(
	    err, (err == GL_NO_ERROR),
	    "test_depthRange_basicRobustness failed. Reversed range rejected.");
	resetState_depthRange();
}

/* ============================================================
 * glDepthRange — Parametrik Tarama
 * ============================================================ */
void test_depthRange_parameterSweep(void) {
	int nearStep, farStep;
	int passCount = 0;
	int failCount = 0;
	printf("TEST: Parameter Sweep\n");
	resetState_depthRange();
	for (nearStep = -10; nearStep <= 20; nearStep++) {
		for (farStep = -10; farStep <= 20; farStep++) {
			GLdouble nearVal = nearStep / 10.0;
			GLdouble farVal = farStep / 10.0;
			glDepthRange(nearVal, farVal);
			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				failCount++;
			} else {
				passCount++;
			}
		}
	}
	printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
	EXPECT_GL_ERROR(failCount, (failCount == 0),
			"test_depthRange_parameterSweep failed.");
	resetState_depthRange();
}

/* ============================================================
 * glDepthRange — Özel Kayan Nokta Degerleri
 * ============================================================ */
void test_depthRange_specialValues(void) {
	GLenum err;
	printf("TEST: Special Floating Point Values\n");
	resetState_depthRange();
	GLdouble tests[][2] = {{0.0, 1.0},	    {1.0, 0.0},
			       {-1.0, 2.0},	    {-DBL_MAX, DBL_MAX},
			       {DBL_MAX, -DBL_MAX}, {DBL_MIN, DBL_MAX},
			       {-DBL_MIN, DBL_MIN}, {1000000.0, -1000000.0}};
	int count = sizeof(tests) / sizeof(tests[0]);
	for (int i = 0; i < count; i++) {
		glDepthRange(tests[i][0], tests[i][1]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
				"test_depthRange_specialValues failed.");
	}
	resetState_depthRange();
}

/* ============================================================
 * glDepthRange — State Query Dogrulamasi
 * ============================================================ */
void test_depthRange_stateQuery(void) {
	GLdouble depthRange[2];
	GLenum err;
	printf("TEST: State Query\n");
	resetState_depthRange();
	GLdouble values[][2] = {
	    {0.0, 1.0}, {1.0, 0.0}, {0.25, 0.75}, {-1.0, 2.0}, {5.0, -5.0}};
	int count = sizeof(values) / sizeof(values[0]);
	for (int i = 0; i < count; i++) {
		glDepthRange(values[i][0], values[i][1]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
				"test_depthRange_stateQuery failed.");
		glGetDoublev(GL_DEPTH_RANGE, depthRange);
		EXPECT_GL_ERROR(
		    depthRange[0],
		    (depthRange[0] >= 0.0 && depthRange[0] <= 1.0),
		    "test_depthRange_stateQuery failed. Near out of range.");
		EXPECT_GL_ERROR(
		    depthRange[1],
		    (depthRange[1] >= 0.0 && depthRange[1] <= 1.0),
		    "test_depthRange_stateQuery failed. Far out of range.");
	}
	resetState_depthRange();
}

/* ============================================================
 * glDepthRange — Hata Kuyrugu ve Durum Korunumu
 * ============================================================ */
void test_depthRange_errorQueue(void) {
	GLdouble depthRange[2];
	GLenum err;
	printf("TEST: Error Queue and State Preservation\n");
	resetState_depthRange();
	glDepthRange(0.20, 0.80);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_depthRange_errorQueue failed.");
	glGetDoublev(GL_DEPTH_RANGE, depthRange);
	EXPECT_GL_ERROR(depthRange[0],
			(depthRange[0] >= 0.0 && depthRange[0] <= 1.0),
			"test_depthRange_errorQueue failed.");
	glDepthRange(-1000.0, 1000.0);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_depthRange_errorQueue failed.");
	resetState_depthRange();
}

/* ============================================================
 * glDepthRange — Rastgele Stres Testi
 * ============================================================ */
void test_depthRange_stress(void) {
	unsigned int i;
	printf("TEST: Random Stress Test\n");
	resetState_depthRange();
	srand(12345);
	for (i = 0; i < 10000; i++) {
		GLdouble nearValue =
		    ((GLdouble)rand() / RAND_MAX) * 20.0 - 10.0;
		GLdouble farValue = ((GLdouble)rand() / RAND_MAX) * 20.0 - 10.0;
		glDepthRange(nearValue, farValue);
		GLenum err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
				"test_depthRange_stress failed.");
	}
	resetState_depthRange();
	printf("  1,000,000 rastgele test tamamlandi.\n");
}

/****************************************/
/******* Pixel Rectangles *******/
/****************************************/

/* ============================================================
 * glPixelStorei — Temel Robustness
 * ============================================================ */
void test_pixelStore_basicRobustness(void) {
	GLenum err;
	GLint validValues[] = {1, 2, 4, 8};
	printf("TEST: Basic Robustness\n");
	resetState_pixelStore();
	for (int i = 0; i < 4; i++) {
		glPixelStorei(GL_PACK_ALIGNMENT, validValues[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
				"test_pixelStore_basicRobustness failed. "
				"PACK_ALIGNMENT rejected.");
		checkStatePreserved_pixelStore(GL_PACK_ALIGNMENT,
					       validValues[i]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, validValues[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
				"test_pixelStore_basicRobustness failed. "
				"UNPACK_ALIGNMENT rejected.");
		checkStatePreserved_pixelStore(GL_UNPACK_ALIGNMENT,
					       validValues[i]);
	}
	resetState_pixelStore();
}

/* ============================================================
 * glPixelStorei — Geçersiz Hizalama Degerleri
 * ============================================================ */
void test_pixelStore_invalidAlignment(void) {
	GLenum err;
	GLint invalidValues[] = {0,  3,	 5,  6,	  7,	   9,
				 -1, -2, 10, 100, INT_MAX, INT_MIN};
	printf("TEST: Invalid Alignment Values\n");
	resetState_pixelStore();
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	for (int i = 0;
	     i < (int)(sizeof(invalidValues) / sizeof(invalidValues[0])); i++) {
		glPixelStorei(GL_PACK_ALIGNMENT, invalidValues[i]);
		err = glGetError();
		EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
				"test_pixelStore_invalidAlignment failed.");
		checkStatePreserved_pixelStore(GL_PACK_ALIGNMENT, 4);
	}
	resetState_pixelStore();
}

/* ============================================================
 * glPixelStorei — Geçersiz pname Degerleri
 * ============================================================ */
void test_pixelStore_invalidPname(void) {
	GLenum err;
	GLenum pname;
	printf("TEST: Invalid pname Values\n");
	resetState_pixelStore();
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	checkStatePreserved_pixelStore(GL_PACK_ALIGNMENT, 4);
	for (pname = 0; pname < 10000; pname++) {
		if (pname == GL_PACK_ALIGNMENT || pname == GL_UNPACK_ALIGNMENT)
			continue;
		glPixelStorei(pname, 4);
		err = glGetError();
		if (err != GL_INVALID_ENUM && err != GL_NO_ERROR) {
			EXPECT_GL_ERROR(err, 0,
					"test_pixelStore_invalidPname failed. "
					"Unexpected error.");
		}
		checkStatePreserved_pixelStore(GL_PACK_ALIGNMENT, 4);
	}
	resetState_pixelStore();
}

/* ============================================================
 * glPixelStorei — Durum Korunumu
 * ============================================================ */
void test_pixelStore_statePreservation(void) {
	GLenum err;
	GLint value;
	printf("TEST: State Preservation\n");
	resetState_pixelStore();
	glPixelStorei(GL_PACK_ALIGNMENT, 8);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_pixelStore_statePreservation failed.");
	glGetIntegerv(GL_PACK_ALIGNMENT, &value);
	EXPECT_GL_ERROR(value, (value == 8),
			"test_pixelStore_statePreservation failed.");
	glPixelStorei(GL_PACK_ALIGNMENT, 3);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"test_pixelStore_statePreservation failed. Invalid "
			"value accepted.");
	glGetIntegerv(GL_PACK_ALIGNMENT, &value);
	EXPECT_GL_ERROR(value, (value == 8),
			"test_pixelStore_statePreservation failed. State "
			"changed on invalid call.");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_pixelStore_statePreservation failed.");
	glPixelStorei(GL_UNPACK_ALIGNMENT, -5);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"test_pixelStore_statePreservation failed.");
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &value);
	EXPECT_GL_ERROR(
	    value, (value == 2),
	    "test_pixelStore_statePreservation failed. Unpack state changed.");
	resetState_pixelStore();
}

/* ============================================================
 * glPixelStorei — Hata Kuyrugu ve Durum Korunumu
 * ============================================================ */
void test_pixelStore_errorQueue(void) {
	GLenum err;
	printf("TEST: Error Queue and State Preservation\n");
	resetState_pixelStore();
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_pixelStore_errorQueue failed.");
	glPixelStorei(GL_PACK_ALIGNMENT, 3);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"test_pixelStore_errorQueue failed.");
	glPixelStorei(GL_TEXTURE_2D, 4);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"test_pixelStore_errorQueue failed.");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
	err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR),
			"test_pixelStore_errorQueue failed.");
	checkStatePreserved_pixelStore(GL_UNPACK_ALIGNMENT, 8);
	resetState_pixelStore();
}

/* ============================================================
 * glPixelStorei — Rastgele Stres Testi
 * ============================================================ */
void test_pixelStore_stress(void) {
	unsigned int i;
	printf("TEST: Random Stress Test\n");
	resetState_pixelStore();
	srand(12345);
	GLenum validPnames[] = {GL_PACK_ALIGNMENT, GL_UNPACK_ALIGNMENT};
	for (i = 0; i < 1000000; i++) {
		GLenum pname;
		if (rand() % 2)
			pname = validPnames[rand() % 2];
		else
			pname = (GLenum)rand();
		GLint value = (rand() % 200) - 100;
		glPixelStorei(pname, value);
		GLenum err = glGetError();
		EXPECT_GL_ERROR(
		    err,
		    (err == GL_NO_ERROR || err == GL_INVALID_ENUM ||
		     err == GL_INVALID_VALUE),
		    "test_pixelStore_stress failed. Unexpected error code.");
	}
	resetState_pixelStore();
	printf("  1,000,000 rastgele test tamamlandi.\n");
}

/***********************************/
/***** Core Workflow Functions *****/
/***********************************/

void init() { printf("init\n"); }

void draw() {
	/* Shaders and Programs */
	runTest(rTest_CreateProgram);
	runTest(rTest_ProgramBinary_unalignedPtr);
	runTest(rTest_ProgramBinary_memRevoke);
	runTest(rTest_ProgramBinary_overload);
	runTest(rTest_UseProgram_invalidID);
	runTest(rTest_UseProgram_typeConfusion);
	runTest(rTest_GetAttribLocation_nullPtr);
	runTest(rTest_GetAttribLocation_reservedVariable);

	/* Uniforms */
	runTest(rTest_GetUniformLocation_nullPtr);
	runTest(rTest_GetUniformLocation_reservedPrefix);
	runTest(rTest_Uniform_typeConfusion);
	runTest(rTest_Uniform_invalidLocation);
	runTest(rTest_Uniformv_negativeCount);
	runTest(rTest_Uniformv_arrayOutOfBounds);
	runTest(rTest_UniformMatrix_invalidTranspose);
	runTest(rTest_UniformMatrix_typeMismatch);

	/* Vertex Attributes */
	runTest(rTest_GetVertexAttrib_invalidEnum);
	runTest(rTest_GetVertexAttrib_indexOutOfBounds);
	runTest(rTest_GetVertexAttribPointer_invalidEnum);
	runTest(rTest_GetnUniform_negativeBufSize);
	runTest(rTest_GetnUniform_invalidProgram);
	runTest(rTest_GetProgramiv_invalidEnum);
	runTest(rTest_GetProgramiv_typeConfusion);
	runTest(rTest_VertexAttrib_indexOutOfBounds);
	runTest(rTest_VertexAttribv_specialFloats);
	runTest(rTest_VertexAttribPointer_invalidType);
	runTest(rTest_VertexAttribPointer_invalidSize);
	runTest(rTest_EnableDisableVertexAttrib_bounds);

	/* Draw Calls */
	runTest(rTest_DrawElements_invalidType);
	runTest(rTest_DrawRangeElements_invalidRange);
	runTest(rTest_DrawArrays_guardPageAttack); // SEGMENTATION FAULT
	runTest(rTest_DrawArrays_outOfBounds);

	/* Framebuffer Operations */
	runTest(rTest_ColorMask_booleanConversion);
	runTest(rTest_StencilMaskSeparate_invalidEnum);
	runTest(rTest_Clear_invalidBitmask);
	runTest(rTest_ClearColor_specialFloats);
	runTest(rTest_ClearDepthf_clamping);
	runTest(rTest_ClearStencil_bounds);

	/* Buffer Objects — glGenBuffers */
	runTest(rTest_glGenBuffers_invalid_value);
	runTest(rTest_glGenBuffers_zero_count);
	runTest(rTest_glGenBuffers_null_buffers);
	runTest(rTest_glGenBuffers_large_n);
	runTest(rTest_glGenBuffers_repeated_generation);
	runTest(rTest_glGenBuffers_unique_names);
	runTest(rTest_glGenBuffers_unbound_names_lifecycle);
	runTest(rTest_glGenBuffers_double_delete);
	runTest(rTest_glGenBuffers_huge_count_small_buffer); //SEGMENTATION FAULT

	/* Buffer Objects — glBindBuffer */
	runTest(rTest_glBindBuffer_invalid_enum);
	runTest(rTest_glBindBuffer_new_name_without_gen);
	runTest(rTest_glBindBuffer_deleted_buffer);
	runTest(rTest_glBindBuffer_boundary_handles);
	runTest(rTest_glBindBuffer_dirty_high_bits_enum);
	runTest(rTest_glBindBuffer_rapid_cross_target_rebind_stress);
	runTest(rTest_glBindBuffer_delete_while_double_bound);
	runTest(rTest_glBindBuffer_zero_binding_query_thrash);
	runTest(rTest_glBindBuffer_massive_namespace_fuzz);
	runTest(rTest_glBindBuffer_binding_churn_stress);
	runTest(rTest_glBindBuffer_lifecycle_stress);

	/* Buffer Objects — glBufferData */
	runTest(rTest_glBufferData_invalid_enum_target);
	runTest(rTest_glBufferData_invalid_enum_usage);
	runTest(rTest_glBufferData_invalid_value_negative_size);
	runTest(rTest_glBufferData_invalid_operation_zero_buffer_bound);
	runTest(rTest_glBufferData_out_of_memory);
	runTest(rTest_glBufferData_source_buffer_too_small);
	runTest(rTest_glBufferData_zero_size_nonnull_data);
	runTest(rTest_glBufferData_size_overflow_boundary);
	runTest(rTest_glBufferData_dirty_usage_enum);
	runTest(rTest_glBufferData_target_zero_bound);
	runTest(rTest_glBufferData_repeated_resize_thrash);
	runTest(rTest_glBufferData_misaligned_data_pointer);
	runTest(rTest_glBufferData_dangling_data_pointer);
	runTest(rTest_glBufferData_state_after_out_of_memory);

	/* Buffer Objects — glBufferSubData */
	runTest(rTest_glBufferSubData_invalid_enum_target);
	runTest(rTest_glBufferSubData_invalid_value_negative_offset);
	runTest(rTest_glBufferSubData_invalid_value_negative_size);
	runTest(rTest_glBufferSubData_invalid_value_out_of_bounds);
	runTest(rTest_glBufferSubData_invalid_operation_zero_buffer_bound);
	runTest(rTest_glBufferSubData_offset_size_overflow_wraparound);
	runTest(rTest_glBufferSubData_exact_boundary_offset);
	runTest(rTest_glBufferSubData_negative_offset_compensating_size);
	runTest(rTest_glBufferSubData_zero_size_null_data);
	runTest(rTest_glBufferSubData_target_zero_bound);
	runTest(rTest_glBufferSubData_into_zero_sized_store);
	runTest(rTest_glBufferSubData_source_smaller_than_size);
	runTest(rTest_glBufferSubData_dangling_data_pointer);
	runTest(rTest_glBufferSubData_overlapping_misaligned_thrash);

	/* Buffer Objects — glGetBufferParameteriv */
	runTest(rTest_glGetBufferParameteriv_invalid_enum_target);
	runTest(rTest_glGetBufferParameteriv_invalid_enum_value);
	runTest(
	    rTest_glGetBufferParameteriv_invalid_operation_zero_buffer_bound);
	runTest(rTest_glGetBufferParameteriv_invalid_target);
	runTest(rTest_glGetBufferParameteriv_invalid_value);
	runTest(rTest_glGetBufferParameteriv_reserved_name_zero_bound);
	runTest(rTest_glGetBufferParameteriv_element_array_zero_bound);
	runTest(rTest_glGetBufferParameteriv_null_data_pointer); // SEGMENTATION FAULT
	runTest(rTest_glGetBufferParameteriv_dangling_data_pointer); // HEAP CORRUPTION
	runTest(rTest_glGetBufferParameteriv_size_consistency);
	runTest(rTest_glGetBufferParameteriv_usage_initial_and_updates);
	runTest(rTest_glGetBufferParameteriv_both_invalid);
	runTest(rTest_glGetBufferParameteriv_after_delete_binding_reverts);
	runTest(rTest_glGetBufferParameteriv_same_buffer_multiple_targets);
	runTest(rTest_glGetBufferParameteriv_unaligned_data_pointer);
	runTest(rTest_glGetBufferParameteriv_error_state_stress);

	/* Rasterization — glLineWidth */
	runTest(test_lineWidth_basicRobustness);
	runTest(test_lineWidth_stressSweep);
	runTest(test_lineWidth_specialFloats);
	runTest(test_lineWidth_errorQueue);
	runTest(test_lineWidth_limits);

	/* Rasterization — glCullFace */
	runTest(test_cullFace_basicRobustness);
	runTest(test_cullFace_stressSweep); // SONSUZ DÖNGÜ
	runTest(test_cullFace_errorQueue);
	runTest(test_cullFace_rapidToggle);
	runTest(test_cullFace_statePreservation);
	runTest(test_cullFace_frontFaceCombination);
	runTest(test_cullFace_largeEnum);
	runTest(test_cullFace_rapidFire);
	runTest(test_cullFace_randomFuzz);

	/* Rasterization — glEnable/Disable */
	runTest(test_cullFaceEnable_basicRobustness);
	runTest(test_cullFaceEnable_rapidToggle);
	runTest(test_cullFaceEnable_invalidCaps);
	runTest(test_cullFaceEnable_capCombinations);

	/* Rasterization — glFrontFace */
	runTest(test_frontFace_errorQueue);
	runTest(test_frontFace_rapidToggle);
	runTest(test_frontFace_mixedValidity);
	runTest(test_frontFace_statePreservation);
	runTest(test_frontFace_cullCombinations);
	runTest(test_frontFace_largeEnum);
	runTest(test_frontFace_rapidFire);
	runTest(test_frontFace_randomFuzz);

	/* Rasterization — glPolygonOffset */
	runTest(test_polygonOffset_basicRobustness);
	runTest(test_polygonOffset_stressSweep);
	runTest(test_polygonOffset_errorQueue);
	runTest(test_polygonOffset_statePreservation);
	runTest(test_polygonOffset_specialFloats);

	/* Viewport and Clipping — glViewport */
	runTest(test_viewport_basicRobustness);
	runTest(test_viewport_negativeDimensions);
	runTest(test_viewport_boundaryCoordinates);
	runTest(test_viewport_limits);
	runTest(test_viewport_errorQueue);
	runTest(test_viewport_stress);

	/* Viewport and Clipping — glDepthRange */
	runTest(test_depthRange_basicRobustness);
	runTest(test_depthRange_parameterSweep);
	runTest(test_depthRange_specialValues);
	runTest(test_depthRange_stateQuery);
	runTest(test_depthRange_errorQueue);
	runTest(test_depthRange_stress); // SONSUZ DÖNGÜ

	/* Pixel Rectangles — glPixelStorei */
	runTest(test_pixelStore_basicRobustness);
	runTest(test_pixelStore_invalidAlignment);
	runTest(test_pixelStore_invalidPname);
	runTest(test_pixelStore_statePreservation);
	runTest(test_pixelStore_errorQueue);
	runTest(test_pixelStore_stress);

#ifdef RUN_EXTESTS
	runTest(rTest_invalidEnum);
	runTest(rTest_invalidValue);
	runTest(rTest_invalidPrecision);
	runTest(rTest_errorFlood);
	runTest(rTest_shaderCompilerError);
	runTest(rTest_maxTextureLimit);
	runTest(rTest_missingAttrib);
	runTest(rTest_NaNVertices);
	runTest(rTest_outOfMemory);
	runTest(rTest_stateRecovr);
	runTest(rTest_drawWOProgram);
	runTest(rTest_oobDraw);
	runTest(rTest_nullPtr);
#endif
}

void cleanup() { printf("cleanup\n"); }

/*********************************/
/***** Other Exemplary Tests *****/
/*********************************/
// API misuse tests
void rTest_invalidEnum() {
	glEnable(0xffffffff);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_ENUM),
			"rTest_invalidEnum failed.");
}
void rTest_invalidValue() {
	glLineWidth(-1.0f);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_invalidValue failed.");
}

// state machine robustness
void rTest_stateRecovr() {
	glEnable(0xdeadbeef);
	glGetError();
	glEnable(GL_BLEND);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR), "rTest_stateRecovr failed.");
}

// resource management robustness
void rTest_outOfMemory() {
	GLuint tex;
	glGenTextures(1, &tex);
	for (int i = 0; i < 10000; i++) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8192, 8192, 0, GL_RGBA,
			     GL_UNSIGNED_BYTE, NULL);
		GLenum err = glGetError();
		if (err == GL_OUT_OF_MEMORY) {
			break;
		}
	}
}

// buffer and memory safety
void rTest_nullPtr() {
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err != GL_NO_ERROR), "rTest_nullPtr failed.");
}
void rTest_oobDraw() {
	GLfloat data[6] = {0};
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, data);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 1000);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err != GL_NO_ERROR), "rTest_oobDraw failed.");
}

// shader robustness
void rTest_shaderCompilerError() {
	const char *bad = "void main() { gl_FragColor = vec4(1.0) }";
	GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s, 1, &bad, NULL);
	glCompileShader(s);
	GLint status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);
	EXPECT_GL_ERROR(status, (status == GL_FALSE),
			"rTest_shaderCompilerError failed. Bad shader compiled "
			"successfully.");
}
void rTest_invalidPrecision() {
	const char *bad = "precision superhighp float; void main(){}";
	GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s, 1, &bad, NULL);
	glCompileShader(s);
	GLint status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);
	EXPECT_GL_ERROR(status, (status == GL_FALSE),
			"rTest_invalidPrecision failed. Invalid precision "
			"compiled successfully.");
}

// draw pipeline robustness
void rTest_drawWOProgram() {
	glUseProgram(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err, (err != GL_NO_ERROR),
			"rTest_drawWOProgram failed.");
}
void rTest_missingAttrib() {
	GLuint prog = glCreateProgram();
	glUseProgram(prog);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err != GL_NO_ERROR),
			"rTest_missingAttrib failed.");
}

// limit and capability tests
void rTest_maxTextureLimit() {
	GLint max;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max + 1, max + 1, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_maxTextureLimit failed.");
}

// error handling robustness
void rTest_errorFlood() {
	for (int i = 0; i < 10000; i++) {
		glEnable(0xffffffff);
	}

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err != GL_NO_ERROR), "rTest_errorFlood failed.");
}

// degenerate geometry/number handling
void rTest_NaNVertices() {
	GLfloat bad_data[6];
	bad_data[0] = 0.0f / 0.0f;  // NaN
	bad_data[1] = 1.0f / 0.0f;  // +Infinity
	bad_data[2] = -1.0f / 0.0f; // -Infinity
	bad_data[3] = 1.0f;
	bad_data[4] = 1.0f;
	bad_data[5] = 1.0f;

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, bad_data);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_NO_ERROR), "rTest_NaNVertices failed.");
}
