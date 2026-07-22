#include "include/rtests.h"

int retcode = 0;

/**************************************/
/*********** Shader Sources ***********/
/**************************************/
static const char* vs_source =
	"attribute vec4 vPosition;\n"
	"void main() {\n"
	"    gl_Position = vPosition;\n"
	"}\n";

static const char* fs_source =
	"precision mediump float;\n"
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

static void cleanOpenGLState() {
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	while (glGetError() != GL_NO_ERROR);
}

static void runTest(void (*test_func)(), const char* name) {
	cleanOpenGLState();
	printf("[TEST] %s...\n", name);
	test_func();
	if (retcode == 0)
		printf("\x1b[32m[PASS]\x1b[0m %s passed.\n", name);
	retcode = 0;
}
#define runTest(func) runTest(func, #func)


/***************************************/
/****** Robustness Test Functions ******/
/***************************************/

/* Shaders and Programs */
// glCreateProgram
void rTest_CreateProgram() {
	GLuint p_count = 0;
//	while (1) {
	while (p_count < 10000) {  // normalde sonsuz döngü olmalı ama burada çalıştırmak için testi 10000000 defa koşturuyoruz
		GLuint prog = glCreateProgram();
		GLenum err = glGetError();
		if (prog == 0 || err == GL_OUT_OF_MEMORY)
			break;

		assert(err == GL_NO_ERROR);
		p_count++;
	}
}

// glProgramBinary
void rTest_ProgramBinary_unalignedPtr() {
	GLuint prog = glCreateProgram();

	void* valid_memblock = malloc(1024);
	const void* unaligned_ptr = (const void*)((char*)valid_memblock + 1);

	glProgramBinary(prog, 0x1234, unaligned_ptr, 100);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
			(err == GL_INVALID_ENUM ||
			 err == GL_INVALID_VALUE ||
			 err == GL_INVALID_OPERATION),
			"rTest_ProgramBinary_unalignedPtr failed.");
	free(valid_memblock);
}

void rTest_ProgramBinary_memRevoke() {
	GLuint prog = glCreateProgram();

	size_t page_size = sysconf(_SC_PAGESIZE);
	void* mapped_memory = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
				   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	mprotect(mapped_memory, page_size, PROT_NONE);

	glProgramBinary(prog, 0x1234, mapped_memory, 1024);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
			(err == GL_INVALID_ENUM ||
			 err == GL_INVALID_VALUE ||
			 err == GL_INVALID_OPERATION),
			"rTest_ProgramBinary_memRevoke failed.");
	munmap(mapped_memory, page_size);
}

void rTest_ProgramBinary_overload() {
	GLuint prog = glCreateProgram();

	GLint num_formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &num_formats);

	GLenum valid_format = 0x1234;
	if (num_formats > 0)
		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS,
			      (GLint*)&valid_format);

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

	EXPECT_GL_ERROR(link_status,
			(link_status == GL_FALSE),
			"rTest_ProgramBinary_overload failed.\n"
			"\x1b[31mCRITICAL:\x1b[0m Driver accepted the garbage "
			"data as a valid Shader Program.");
}

// glUseProgram
void rTest_UseProgram_invalidID() {
	GLuint ghost_id = 0xdeadbeef;
	glUseProgram(ghost_id);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
			(err == GL_INVALID_VALUE),
			"rTest_UseProgram_invalidID failed.\n"
			"\x1b[31mCRITICAL:\x1b[0m Driver accepted the "
			"ghost ID (has never existed) as a valid program ID.");
}

void rTest_UseProgram_typeConfusion() {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glUseProgram(shader);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
		 	(err == GL_INVALID_OPERATION),
		 	"rTest_UseProgram_typeConfusion failed.\n");

	glDeleteShader(shader);
}

// glGetAttribLocation
void rTest_GetAttribLocation_nullPtr() {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, NULL);

	EXPECT_GL_ERROR(
		loc,
		(loc == -1),
		"rTest_GetAttribLocation_nullPtr failed."
	);
}

void rTest_GetAttribLocation_reservedVariable() {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, "gl_Position");

	EXPECT_GL_ERROR(
		loc,
		(loc == -1),
		"rTest_GetAttribLocation_reservedVariable failed."
	);
}

void rTest_GetUniformLocation_nullPtr(void) {
	GLuint prog = createDummyProgram();

	// NULL pointer saldırısı: Akıllı bir sürücü çökmeden -1 dönmelidir.
	GLint loc = glGetUniformLocation(prog, NULL);

	EXPECT_GL_ERROR(
		loc,
		(loc == -1),
		"rTest_GetUniformLocation_nullPtr failed.\n"
		"Sürücü NULL pointer yediğinde -1 dönmedi."
	);
}

void rTest_GetUniformLocation_reservedPrefix(void) {
	GLuint prog = createDummyProgram();

	// Yasaklı isim saldırısı: "gl_" ön eki spesifikasyon gereği OpenGL'e aittir.
	GLint loc = glGetUniformLocation(prog, "gl_DepthRange");

	EXPECT_GL_ERROR(
		loc,
		(loc == -1),
		"rTest_GetUniformLocation_reservedPrefix failed.\n"
		"Sürücü yasaklı 'gl_' ön ekine sahip uniform yerini ifşa etti."
	);
}

void rTest_Uniform_typeConfusion(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locInt = glGetUniformLocation(prog, "uInt");

	// Tip Karmaşası: Shader'da 'int' olarak tanımlanan bir değişkene 'float' (glUniform1f) basmaya çalışıyoruz.
	glUniform1f(locInt, 3.14f);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_OPERATION),
		"rTest_Uniform_typeConfusion failed.\n"
		"Sürücü 'int' değişkene 'float' atanmasını engellemedi (Type Confusion)."
	);
}

void rTest_Uniform_invalidLocation(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	// Spesifikasyon Tuzağı:
	// location = -1 ise sürücü veriyi sessizce reddetmeli (GL_NO_ERROR).
	glUniform1i(-1, 42);
	GLenum err1 = glGetError();
	EXPECT_GL_ERROR(err1,
		 (err1 == GL_NO_ERROR),
		 "rTest_Uniform_invalidLocation failed.\n"
		 "-1 lokasyonu sessizce yutulmalıydı (Spec Kuralı).");

	// Geçersiz (fakat -1 olmayan) lokasyon ise GL_INVALID_OPERATION fırlatmalıdır.
	glUniform1i(0x7FFFFFFF, 42);
	GLenum err2 = glGetError();

	EXPECT_GL_ERROR(
		err2,
		(err2 == GL_INVALID_OPERATION),
		"rTest_Uniform_invalidLocation failed.\n"
		"Sürücü tamamen geçersiz ve devasa bir lokasyon ID'sini reddetmedi."
	);
}

void rTest_Uniformv_negativeCount(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locVec = glGetUniformLocation(prog, "uVec4Array");
	GLfloat data[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	// Count (Eleman sayısı) negatif olamaz.
	glUniform4fv(locVec, -1, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE),
		"rTest_Uniformv_negativeCount failed.\n"
		"Sürücü negatif eleman sayısını (count = -1) kabul etti."
	);
}

void rTest_Uniformv_arrayOutOfBounds(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locVec = glGetUniformLocation(prog, "uVec4Array"); // Shader'da boyutu 3 olarak tanımlı.
	GLfloat data[16] = {0};

	// Sınır İhlali (Out of Bounds): 3 elemanlık diziye 4 eleman (count = 4) kopyalamaya çalışıyoruz.
	glUniform4fv(locVec, 4, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_OPERATION),
		"rTest_Uniformv_arrayOutOfBounds failed.\n"
		"Sürücü array sınırlarını aşan (OOB) bir uniform kopyalamasına izin verdi."
	);
}

void rTest_UniformMatrix_invalidTranspose(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	GLint locMat4 = glGetUniformLocation(prog, "uMat4");
	GLfloat mat[16] = {0};

	// ES 2.0 ve SC 2.0 kuralı: Transpose parametresi her zaman GL_FALSE olmak ZORUNDADIR.
	glUniformMatrix4fv(locMat4, 1, GL_TRUE, mat);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE),
		"rTest_UniformMatrix_invalidTranspose failed.\n"
		"Sürücü GL_TRUE transpose bayrağını kabul etti (Spesifikasyon ihlali)."
	);
}

void rTest_UniformMatrix_typeMismatch(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	// Lokasyonu bir Mat3 (3x3 Matris) olarak çekiyoruz.
	GLint locMat3 = glGetUniformLocation(prog, "uMat3");
	GLfloat mat[16] = {0};

	// SABOTAJ: Lokasyon Mat3 iken, biz veriyi Mat4 fonskiyonuyla basmaya çalışıyoruz.
	glUniformMatrix4fv(locMat3, 1, GL_FALSE, mat);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_OPERATION),
		"rTest_UniformMatrix_typeMismatch failed.\n"
		"Sürücü Mat3 lokasyonuna Mat4 verisi kopyalamaya çalıştı."
	);
}

void rTest_GetVertexAttrib_invalidEnum(void) {
	GLfloat params[4];
	// 0xDEADBEEF adında bir parametre (pname) yoktur.
	glGetVertexAttribfv(0, 0xDEADBEEF, params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_ENUM),
		"rTest_GetVertexAttrib_invalidEnum failed.\n"
		"Geçersiz bir parametre (pname) sorgusuna hata dönülmedi."
	);
}

void rTest_GetVertexAttrib_indexOutOfBounds(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

	GLint params[4];
	// Sınır İhlali: İndeksler 0 ile (max_attribs - 1) arasında olmalıdır.
	glGetVertexAttribiv(max_attribs, GL_VERTEX_ATTRIB_ARRAY_ENABLED, params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE),
		"rTest_GetVertexAttrib_indexOutOfBounds failed.\n"
		"Sürücü donanım sınırının dışındaki bir Attribute indeksini sorgulattı."
	);
}

void rTest_GetVertexAttribPointer_invalidEnum(void) {
	void *ptr = NULL;
	// Bu fonksiyon yalnızca ve yalnızca GL_VERTEX_ATTRIB_ARRAY_POINTER Enum'ını kabul eder.
	glGetVertexAttribPointerv(0, GL_FLOAT, &ptr);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_ENUM),
		"rTest_GetVertexAttribPointer_invalidEnum failed.\n"
		"GL_VERTEX_ATTRIB_ARRAY_POINTER harici bir Enum olarak kabul edildi."
	);
}

void rTest_GetnUniform_negativeBufSize(void) {
	GLuint prog = createDummyProgram();
	GLint locFloat = glGetUniformLocation(prog, "uFloat");

	GLfloat data[4];
	// BufSize negatif olamaz. Bu, KHR_robustness eklentisinin temel kuralıdır.
	glGetnUniformfv(prog, locFloat, -1, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE),
		"rTest_GetnUniform_negativeBufSize failed.\n"
		"Sürücü negatif bir buffer boyutu (bufSize) kabul etti."
	);
}

void rTest_GetnUniform_invalidProgram(void) {
	// 0, hiçbir zaman geçerli bir program nesnesi değildir.
	GLfloat data[4];
	glGetnUniformfv(0, 0, sizeof(data), data);
	GLenum err = glGetError();

	// Geçersiz (hiç üretilmemiş) bir obje olduğu için GL_INVALID_VALUE dönmelidir.
	// Eğer obje var ama Program değilse (örn Shader ise) GL_INVALID_OPERATION döner.
	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION),
		"rTest_GetnUniform_invalidProgram failed.\n"
		"Sürücü geçersiz bir Program ID'si üzerinden Uniform sorgulattı."
	);
}

void rTest_GetProgramiv_invalidEnum(void) {
	GLuint prog = glCreateProgram();
	GLint params = 0;

	glGetProgramiv(prog, 0xDEADBEEF, &params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_ENUM),
		"rTest_GetProgramiv_invalidEnum failed.\n"
		"Geçersiz bir parametre (pname) kabul edildi."
	);
}

void rTest_GetProgramiv_typeConfusion(void) {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	GLint params = 0;

	// Tip Karmaşası: Sürücünün Shader nesnesini Program gibi okumaya çalışıp çalışmayacağını sınıyoruz.
	glGetProgramiv(shader, GL_LINK_STATUS, &params);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_OPERATION),
		"rTest_GetProgramiv_typeConfusion failed.\n"
		"Sürücü bir Shader nesnesine Program muamelesi yaptı."
	);

	glDeleteShader(shader);
}

/* Vertices */
// glDrawArrays
void rTest_DrawArrays_outOfBounds() {
	GLfloat vertices[] = {
		-0.5, -0.5,  0.0,
		 0.5, -0.5,  0.0,
		 0.0,  0.5,  0.0
	};

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 1000000);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
		 	(err == GL_INVALID_VALUE ||
		 	 err == GL_INVALID_OPERATION),
		 	"rTest_DrawArrays_outOfBounds failed.");

	glDisableVertexAttribArray(0);
}

void rTest_DrawArrays_guardPageAttack(void) {
	GLuint prog = createDummyProgram();
	glUseProgram(prog);

	size_t page_size = sysconf(_SC_PAGESIZE);

	void* memory = mmap(NULL, page_size * 2, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	void* guard_page = (char*)memory + page_size;
	mprotect(guard_page, page_size, PROT_NONE);

	GLfloat* edge_data = (GLfloat*)((char*)guard_page - (9 * sizeof(GLfloat)));

	edge_data[0] = 0.0f; edge_data[1] = 1.0f; edge_data[2] = 0.0f;
	edge_data[3] =-1.0f; edge_data[4] =-1.0f; edge_data[5] = 0.0f;
	edge_data[6] = 1.0f; edge_data[7] =-1.0f; edge_data[8] = 0.0f;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, edge_data);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 300000000);
	glFinish();

	GLubyte px[4];
	glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, px);

	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_OPERATION || err == GL_INVALID_VALUE),
		"rTest_DrawArrays_guardPageAttack failed.");

	glDisableVertexAttribArray(0);
	munmap(memory, page_size * 2);
}

void rTest_VertexAttrib_indexOutOfBounds(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

	// Sınır İhlali: İndeksler 0 ile (max_attribs - 1) arasında olmalıdır.
	// Donanımın limitine (max_attribs) veri yazmaya çalışıyoruz.
	glVertexAttrib1f(max_attribs, 1.0f);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE),
		"rTest_VertexAttrib_indexOutOfBounds: Sürücü sınır dışı bir Attribute indeksini (max_attribs) reddetmedi!"
	);
}

void rTest_VertexAttribv_specialFloats(void) {
	// Kasıtlı olarak zehirli kayan nokta (float) değerleri gönderiyoruz.
	GLfloat data[4] = {NAN, INFINITY, -INFINITY, 0.0f};

	// Sürücü bu değerleri yediğinde çökmek yerine güvenlice kabul etmeli
	// veya kendi iç mimarisine göre bir hata üretmelidir.
	glVertexAttrib4fv(0, data);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_NO_ERROR || err == GL_INVALID_VALUE),
		"rTest_VertexAttribv_specialFloats: Sürücü NaN/Inf değerlerinde tanımsız bir hata üretti!"
	);
}

void rTest_VertexAttribPointer_invalidType(void) {
	// Type parametresi GL_FLOAT, GL_BYTE vb. olmalıdır.
	// Biz gidip alakasız bir Enum (GL_TEXTURE_2D) veriyoruz.
	glVertexAttribPointer(0, 3, GL_TEXTURE_2D, GL_FALSE, 0, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_ENUM),
		"rTest_VertexAttribPointer_invalidType: Geçersiz bir veri tipi (GL_TEXTURE_2D) kabul edildi!"
	);
}

void rTest_VertexAttribPointer_invalidSize(void) {
	// Size parametresi BİR TEK 1, 2, 3 veya 4 olabilir!
	// 5 elemanlı bir vektör olamaz.
	glVertexAttribPointer(0, 5, GL_FLOAT, GL_FALSE, 0, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE),
		"rTest_VertexAttribPointer_invalidSize: Sürücü 'size = 5' olan geçersiz bir boyutu kabul etti!"
	);
}

void rTest_EnableDisableVertexAttrib_bounds(void) {
	GLint max_attribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

	// Üst sınır ihlali (GL_INVALID_VALUE bekliyoruz)
	glEnableVertexAttribArray(max_attribs);
	GLenum err1 = glGetError();
	EXPECT_GL_ERROR(
		err1,
		(err1 == GL_INVALID_VALUE),
		"glEnableVertexAttribArray: Sınır dışı indeks reddedilmedi!"
	);

	// Negatif veya çok büyük sınır ihlali (0xFFFFFFFF -> UINT_MAX)
	glDisableVertexAttribArray(0xFFFFFFFF);
	GLenum err2 = glGetError();
	EXPECT_GL_ERROR(
		err2,
		(err2 == GL_INVALID_VALUE),
		"glDisableVertexAttribArray: Çok büyük/negatif indeks (0xFFFFFFFF) reddedilmedi!"
	);
}

void rTest_DrawElements_invalidType(void) {
	// 'type' parametresi indeks dizisinin tipidir.
	// Yalnızca GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT veya GL_UNSIGNED_INT olabilir.
	// Biz float türünde bir indeks okumasını istiyoruz.
	glDrawElements(GL_TRIANGLES, 3, GL_FLOAT, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_ENUM),
		"rTest_DrawElements_invalidType: İndeks verisi olarak GL_FLOAT kabul edildi!"
	);
}

void rTest_DrawRangeElements_invalidRange(void) {
	// glDrawRangeElements(mode, start, end, count, type, indices)
	// KURAL: 'end' değeri kesinlikle 'start' değerinden KÜÇÜK OLAMAZ!
	// start = 10, end = 5 göndererek mantıksal bir imkansızlık yaratıyoruz.
	glDrawRangeElements(GL_TRIANGLES, 10, 5, 3, GL_UNSIGNED_SHORT, NULL);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(
		err,
		(err == GL_INVALID_VALUE),
		"rTest_DrawRangeElements_invalidRange: Sürücü 'end < start' olan mantıksız bir aralığı kabul etti!"
	);
}

void rTest_ColorMask_booleanConversion(void) {
	// Kasıtlı olarak 1 ve 0 yerine 'tuhaf' sayılar (0xFF, 0x02, 0x80) gönderiyoruz.
	// OpenGL kurallarına göre 0 dışındaki her şey GL_TRUE kabul edilmelidir.
	glColorMask(0xFF, 0x02, 0x00, 0x80);
	GLenum err = glGetError();
	
	EXPECT_GL_ERROR(
		err, 
		(err == GL_NO_ERROR), 
		"rTest_ColorMask_booleanConversion: Standart dışı değerler girildiğinde hata üretildi (Spesifikasyona aykırı)!"
	);
	
	// Sürücü gerçekten bu değerleri GL_TRUE ve GL_FALSE olarak kırptı mı?
	GLboolean mask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, mask);
	
	EXPECT_GL_ERROR(
		mask[0], 
		(mask[0] == GL_TRUE && mask[1] == GL_TRUE && mask[2] == GL_FALSE && mask[3] == GL_TRUE),
		"rTest_ColorMask_booleanConversion: Sürücü '!= 0' kuralını ihlal etti veya değerleri doğru cast etmedi!"
	);
}

void rTest_StencilMaskSeparate_invalidEnum(void) {
	// 'face' parametresi YALNIZCA GL_FRONT, GL_BACK veya GL_FRONT_AND_BACK olabilir.
	glStencilMaskSeparate(GL_TEXTURE_2D, 0xFFFFFFFF);
	GLenum err = glGetError();
	
	EXPECT_GL_ERROR(
		err, 
		(err == GL_INVALID_ENUM), 
		"rTest_StencilMaskSeparate_invalidEnum: Geçersiz face parametresi (GL_TEXTURE_2D) reddedilmedi!"
	);
}

void rTest_Clear_invalidBitmask(void) {
	// glClear yalnızca COLOR, DEPTH ve STENCIL bitlerinin mantıksal OR (Veya) kombinasyonunu kabul eder.
	// Tüm bitleri '1' yaparak (0xFFFFFFFF) sürücüye yasaklı bitler yolluyoruz.
	glClear(0xFFFFFFFF);
	GLenum err1 = glGetError();
	
	EXPECT_GL_ERROR(
		err1, 
		(err1 == GL_INVALID_VALUE), 
		"rTest_Clear_invalidBitmask: glClear, tanımsız olan geçersiz maske bitlerini yuttu (0xFFFFFFFF)!"
	);

	// İnce Suikast: Sadece bir tane geçersiz bit (örneğin 0x04) ekleyerek kirletiyoruz.
	glClear(GL_COLOR_BUFFER_BIT | 0x00000004);
	GLenum err2 = glGetError();

	EXPECT_GL_ERROR(
		err2, 
		(err2 == GL_INVALID_VALUE), 
		"rTest_Clear_invalidBitmask: Kirletilmiş mantıksal maske kombinasyonu reddedilmedi!"
	);
}

void rTest_ClearColor_specialFloats(void) {
	// Kayan nokta zehirlemesi (NaN ve Infinity)
	// Spec bu konuda çok net olmasa da kaliteli bir SC 2.0 sürücüsü bunu sessizce yutmalı 
	// veya kendi içinde güvenli bir hale getirmelidir (Kesinlikle ÇÖKMEMELİDİR).
	glClearColor(NAN, INFINITY, -INFINITY, 1.5f);
	GLenum err = glGetError();
	
	EXPECT_GL_ERROR(
		err, 
		(err == GL_NO_ERROR || err == GL_INVALID_VALUE), 
		"rTest_ClearColor_specialFloats: NaN/Inf değerleri atanırken beklenmeyen bir hata kodu döndü veya sürücü kilitlendi!"
	);
}

void rTest_ClearDepthf_clamping(void) {
	// Depth değeri matematikte sadece [0.0, 1.0] aralığında olabilir.
	// Biz sınırların çok dışına taşıyoruz.
	glClearDepthf(5000.0f);
	GLenum err = glGetError();

	// Spesifikasyon: "Değerler hata fırlatmadan sessizce [0,1] aralığına kırpılır."
	EXPECT_GL_ERROR(
		err,
		(err == GL_NO_ERROR),
		"rTest_ClearDepthf_clamping: Sınır dışı depth atamasında beklenmeyen bir hata fırlatıldı!"
	);

	// Sürücü gerçekten değeri 1.0'a kilitledi (clamp) mi?
	GLfloat depth = -1.0f;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depth);

	EXPECT_GL_ERROR(
		depth,
		(depth == 1.0f),
		"rTest_ClearDepthf_clamping: Sürücü aşırı depth değerini [0,1] aralığına kırpmayı başaramadı!"
	);
}

void rTest_ClearStencil_bounds(void) {
	// Stencil değeri integer'dır, ancak mevcut Stencil Buffer bit sayısına göre maskelenir.
	// Aşırı büyük ve negatif sayılar vererek state okumasının bozulup bozulmadığını sınıyoruz.
	glClearStencil(-1); // Genelde tüm bitleri 1 yapan maske görevi görür (Two's complement)
	GLenum err = glGetError();
	
	EXPECT_GL_ERROR(
		err, 
		(err == GL_NO_ERROR), 
		"rTest_ClearStencil_bounds: Negatif değer atamasında hata fırlatıldı!"
	);
}

/***********************************/
/***** Core Workflow Functions *****/
/***********************************/

void init() {
	printf("init\n");
}

void draw() {
	runTest(rTest_CreateProgram);
	runTest(rTest_ProgramBinary_unalignedPtr);
	runTest(rTest_ProgramBinary_memRevoke);
	runTest(rTest_ProgramBinary_overload);
	runTest(rTest_UseProgram_invalidID);
	runTest(rTest_UseProgram_typeConfusion);
	runTest(rTest_GetAttribLocation_nullPtr);
	runTest(rTest_GetAttribLocation_reservedVariable);
	runTest(rTest_GetUniformLocation_nullPtr);
	runTest(rTest_GetUniformLocation_reservedPrefix);
	runTest(rTest_Uniform_typeConfusion);
	runTest(rTest_Uniform_invalidLocation);
	runTest(rTest_Uniformv_negativeCount);
	runTest(rTest_Uniformv_arrayOutOfBounds);
	runTest(rTest_UniformMatrix_invalidTranspose);
	runTest(rTest_UniformMatrix_typeMismatch);
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
	runTest(rTest_DrawElements_invalidType);
	runTest(rTest_DrawRangeElements_invalidRange);
//	runTest(rTest_DrawArrays_guardPageAttack); // SEGMENTATION FAULT
	runTest(rTest_DrawArrays_outOfBounds);     // SEGMENTATION FAULT
	runTest(rTest_ColorMask_booleanConversion);
	runTest(rTest_StencilMaskSeparate_invalidEnum);
	runTest(rTest_Clear_invalidBitmask);
	runTest(rTest_ClearColor_specialFloats);
	runTest(rTest_ClearDepthf_clamping);
	runTest(rTest_ClearStencil_bounds);


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

void cleanup() {
	printf("cleanup\n");
}

/*********************************/
/***** Other Exemplary Tests *****/
/*********************************/
// API misuse tests
void rTest_invalidEnum() {
	glEnable(0xffffffff);
	GLenum err = glGetError();
	assert(err == GL_INVALID_ENUM);
}
void rTest_invalidValue() {
	glLineWidth(-1.0f);
	GLenum err = glGetError();
	assert(err == GL_INVALID_VALUE);
}

// state machine robustness
void rTest_stateRecovr() {
	glEnable(0xdeadbeef);
	glGetError();
	glEnable(GL_BLEND);
	GLenum err = glGetError();
	assert(err == GL_NO_ERROR);
}

// resource management robustness
void rTest_outOfMemory() {
	GLuint tex;
	glGenTextures(1, &tex);
	for (int i = 0; i < 10000; i++) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8192, 8192, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
	EXPECT_GL_ERROR(err,
			(err != GL_NO_ERROR),
			"rTest_nullPtr failed.");
}
void rTest_oobDraw() {
	GLfloat data[6] = {0};
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, data);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 1000);
	GLenum err = glGetError();
	EXPECT_GL_ERROR(err,
			(err != GL_NO_ERROR),
			"rTest_oobDraw failed.");
//	assert(err != GL_NO_ERROR);
}

// shader robustness
void rTest_shaderCompilerError() {
	const char *bad = "void main() { gl_FragColor = vec4(1.0) }";
	GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s, 1, &bad, NULL);
	glCompileShader(s);
	GLint status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);
	assert(status == GL_FALSE);
}
void rTest_invalidPrecision() {
	const char *bad = "precision superhighp float; void main(){}";
	GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s, 1, &bad, NULL);
	glCompileShader(s);
	GLint status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);
	assert(status == GL_FALSE);
}

// draw pipeline robustness
void rTest_drawWOProgram() {
	glUseProgram(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum err = glGetError();

	EXPECT_GL_ERROR(err,
			(err != GL_NO_ERROR),
			"rTest_drawWOProgram failed.");
//	assert(err != GL_NO_ERROR);
}
void rTest_missingAttrib() {
	GLuint prog = glCreateProgram();
	glUseProgram(prog);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum err = glGetError();
	assert(err != GL_NO_ERROR);
}

// limit and capability tests
void rTest_maxTextureLimit() {
	GLint max;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max + 1, max + 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	GLenum err = glGetError();
	assert(err == GL_INVALID_VALUE);
}

// error handling robustness
void rTest_errorFlood() {
	for (int i = 0; i < 10000; i++) {
		glEnable(0xffffffff);
	}

	GLenum err = glGetError();
	assert(err != GL_NO_ERROR);
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
	assert(err == GL_NO_ERROR);
}

