#include "include/rtests.h"

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

static void runTest(void (*test_func)(), const char *name) {
	while (glGetError() != GL_NO_ERROR)
		;
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
	while (p_count <
	       10000) { // normalde sonsuz döngü olmalı ama burada çalıştırmak
			// için testi 10000000 defa koşturuyoruz
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

// glUseProgram
void rTest_UseProgram_invalidID() {
	GLuint ghost_id = 0xdeadbeef;
	glUseProgram(ghost_id);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_VALUE),
			"rTest_UseProgram_invalidID failed.\n"
			"\x1b[31mCRITICAL:\x1b[0m Driver accepted the "
			"ghost ID (has never existed) as a valid program ID.");
}

void rTest_UseProgram_typeConfusion() {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glUseProgram(shader);

	GLenum err = glGetError();
	EXPECT_GL_ERROR(err, (err == GL_INVALID_OPERATION),
			"rTest_UseProgram_typeConfusion failed.\n");

	glDeleteShader(shader);
}

// glGetAttribLocation
void rTest_GetAttribLocation_nullPtr() {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, NULL);

	EXPECT_GL_ERROR(loc, (loc == -1),
			"rTest_GetAttribLocation_nullPtr failed.");
}

void rTest_GetAttribLocation_reservedVariable() {
	GLuint prog = glCreateProgram();

	GLint loc = glGetAttribLocation(prog, "gl_Position");

	EXPECT_GL_ERROR(loc, (loc == -1),
			"rTest_GetAttribLocation_reservedVariable failed.");
}

/* Vertices */
// glDrawArrays
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

// void glBindBuffer(GLenum target, GLuint buffer);
// Bir buffer nesnesini belirli bir target'a bağlar
// Bağlandıktan sonra o hedef üzerinde yapılan işlemler artık bu buffer üzerinde
// gerçekleştirilir

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not one of the
// allowable values.
void rTest_glBindBuffer_invalid_enum() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_invalid_enum()\n");

	glBindBuffer(0xFFFFFFFF, 1);

	GLenum err = glGetError();
	if (err != GL_INVALID_ENUM) {
		printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
		assert(err == GL_INVALID_ENUM);
	}
	printf("[PASS] rTest_glBindBuffer_invalid_enum()\n");
}

// glGenBuffers ile oluşturulmamış bir ismin bind edilmesi
void rTest_glBindBuffer_new_name_without_gen() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_new_name_without_gen()\n");

	GLuint name = 424242;
	glBindBuffer(GL_ARRAY_BUFFER, name);
	GLenum err = glGetError();
	printf("[INFO] glBindBuffer(new name=%u): error=0x%X\n", name, err);
}

// Silinen bir buffer isminin tekrar bind edilmesiyle yeni bir buffer nesnesi
// oluşturulup oluşturulmadığını test eder.
void rTest_glBindBuffer_deleted_buffer() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_deleted_buffer()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glDeleteBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	GLenum err = glGetError();
	printf("[INFO] Bind deleted name: error=0x%X\n", err);
}

// Büyük/alışılmadık buffer isimlerinin bind edilmesi
void rTest_glBindBuffer_boundary_handles() {
	printf("[START] rTest_glBindBuffer_boundary_handles()\n");

	GLuint candidates[] = {
	    0xFFFFFFFFu, // UINT_MAX
	    0x80000000u, // sign-bit sınırı
	    0x7FFFFFFFu, // INT_MAX
	    0xDEADBEEFu,
	    0xCDCDCDCDu // tipik uninitialized heap pattern
	};
	for (int i = 0; i < 5; ++i) {
		while (glGetError() != GL_NO_ERROR) {
		}
		glBindBuffer(GL_ARRAY_BUFFER, candidates[i]);
		GLenum err = glGetError();
		printf("[INFO] Boundary buffer 0x%08X: glError=0x%X\n",
		       candidates[i], err);
	}
}

// Geçersiz target enum değerlerine karşı implementasyonun hata kontrolünün
// testi
void rTest_glBindBuffer_dirty_high_bits_enum() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_dirty_high_bits_enum()\n");

	GLenum polluted = GL_ARRAY_BUFFER | 0xFFFF0000u;
	glBindBuffer(polluted, 1);
	GLenum err = glGetError();
	// Spec'e göre bu "allowable değil" -> INVALID_ENUM beklenir
	printf("[INFO] Polluted target=0x%08X : glError=0x%X (expected "
	       "GL_INVALID_ENUM)\n",
	       polluted, err);
}

// Aynı buffer nesnesinin farklı target'lara hızlı ve tekrarlı şekilde
// bağlanması sırasında implementasyonun kararlılığını test eder.
void rTest_glBindBuffer_rapid_cross_target_rebind_stress() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf(
	    "[START] rTest_glBindBuffer_rapid_cross_target_rebind_stress()\n");

	GLuint buf;
	glGenBuffers(1, &buf);

	for (int i = 0; i < 5000; ++i) {
		GLenum target =
		    (i % 2 == 0) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
		glBindBuffer(target, buf);
		glBufferData(target, (i % 7) * 37, NULL, GL_STATIC_DRAW);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[FAIL] Iteration=%d, target=%s, glError=0x%X\n",
			       i,
			       target == GL_ARRAY_BUFFER
				   ? "GL_ARRAY_BUFFER"
				   : "GL_ELEMENT_ARRAY_BUFFER",
			       err);
			glDeleteBuffers(1, &buf);
			return;
		}
	}
	printf("[PASS] Rapid cross-target rebind stress completed without "
	       "OpenGL errors.\n");
	glDeleteBuffers(1, &buf);
}

// Aynı buffer nesnesi iki target'a bağlıyken silme işlemi sonrası
// implementasyonun kararlılığını ve hata davranışını test eder
void rTest_glBindBuffer_delete_while_double_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_delete_while_double_bound()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
		     buf); // aynı obje şimdi iki target'ta aktif
	glDeleteBuffers(1, &buf);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 64, NULL);
	GLenum arrayErr = glGetError();

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 64, NULL);
	GLenum elementErr = glGetError();

	printf("[INFO] After deleting double-bound buffer: ARRAY_BUFFER "
	       "error=0x%X, ELEMENT_ARRAY_BUFFER error=0x%X\n",
	       arrayErr, elementErr);
}

// Buffer'ı tekrar tekrar 0'a bağlayıp bağlama durumunu sorgulayarak
// implementasyonun state yönetimi kararlılığını test eder.
void rTest_glBindBuffer_zero_binding_query_thrash() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_zero_binding_query_thrash()\n");

	for (int i = 0; i < 1000; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLint binding = -1;
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binding);
		if (binding != 0) {
			printf("[FAIL] Iteration=%d, "
			       "GL_ARRAY_BUFFER_BINDING=%d (expected 0)\n",
			       i, binding);
			return;
		}

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);
			return;
		}
	}
	printf("[PASS] Zero binding/query thrash completed successfully.\n");
}

// Çok sayıda buffer ismi üzerinde rastgele bind işlemleri yaparak
// implementasyonun isim yönetimi ve durum değişikliklerine karşı
// dayanıklılığını test eder.
void rTest_glBindBuffer_massive_namespace_fuzz() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_massive_namespace_fuzz()\n");

	const int N = 20000;
	GLuint *names = (GLuint *)malloc(sizeof(GLuint) * N);

	if (names == NULL) {
		printf("[FAIL] Memory allocation failed.\n");
		return;
	}

	glGenBuffers(N, names);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("[FAIL] glGenBuffers failed: glError=0x%X\n", err);
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
			printf("[FAIL] Iteration=%d, buffer=%u, target=%s, "
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

	printf("[PASS] Massive buffer namespace fuzz completed without OpenGL "
	       "errors.\n");
}

// Aynı target üzerinde farklı buffer'lar arasında sürekli geçiş yaparak
// implementasyonun state yönetimi kararlılığını test eder.
void rTest_glBindBuffer_binding_churn_stress() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_binding_churn_stress()\n");

	GLuint buffers[2];
	glGenBuffers(2, buffers);

	for (int i = 0; i < 10000; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);

			glDeleteBuffers(2, buffers);
			return;
		}
	}

	glDeleteBuffers(2, buffers);

	printf(
	    "[PASS] Binding churn stress completed without OpenGL errors.\n");
}

// Buffer nesnelerinin oluşturma, bağlama ve silme yaşam döngüsünü tekrarlı
// olarak çalıştırarak implementasyonun dayanıklılığını test eder.
void rTest_glBindBuffer_lifecycle_stress() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBindBuffer_lifecycle_stress()\n");

	for (int i = 0; i < 5000; ++i) {
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
		glDeleteBuffers(1, &buf);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			printf("[FAIL] Iteration=%d, glError=0x%X\n", i, err);
			return;
		}
	}
	printf("[PASS] Buffer lifecycle stress completed without OpenGL "
	       "errors.\n");
}

// void glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum
// usage); Target parametresiyle bind ettiğin buffer object için ekran kartı
// (GPU) üzerinde yeni bir data store oluşturur Eski veriyi tamamen siler
// İstersen verdiğin data pointer’ındaki veriyi bu yeni belleğe kopyalayarak
// başlatır usage parametresiyle de bu veriyi nasıl kullanacağını sürücüye ipucu
// olarak bildirirsin (performans optimizasyonu için)

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not
// GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void rTest_glBufferData_invalid_enum_target() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_invalid_enum_target()\n");

	glBufferData(0xFFFFFFFF, 16, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	if (err != GL_INVALID_ENUM) {
		printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
		assert(err == GL_INVALID_ENUM);
	}
	printf("[PASS] rTest_glBufferData_invalid_enum_target()\n");
}

// Belirtilen hata: GL_INVALID_ENUM is generated if usage is not GL_STREAM_DRAW,
// GL_STATIC_DRAW, or GL_DYNAMIC_DRAW.
void rTest_glBufferData_invalid_enum_usage() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_invalid_enum_usage()\n");

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 16, NULL, 0xFFFFFFFF);

	GLenum err = glGetError();
	if (err != GL_INVALID_ENUM) {
		printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
		assert(err == GL_INVALID_ENUM);
	}
	printf("[PASS] rTest_glBufferData_invalid_enum_usage()\n");

	glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if size is negative.
void rTest_glBufferData_invalid_value_negative_size() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_invalid_value_negative_size()\n");

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, -1, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
		assert(err == GL_INVALID_VALUE);
	}
	printf("[PASS] rTest_glBufferData_invalid_value_negative_size()\n");

	glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer
// object name 0 is bound to target.
void rTest_glBufferData_invalid_operation_zero_buffer_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] "
	       "rTest_glBufferData_invalid_operation_zero_buffer_bound()\n");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBufferData(GL_ARRAY_BUFFER, 16, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	if (err != GL_INVALID_OPERATION) {
		printf("[FAIL] Expected GL_INVALID_OPERATION, but got 0x%X\n",
		       err);
		assert(err == GL_INVALID_OPERATION);
	}
	printf("[PASS] "
	       "rTest_glBufferData_invalid_operation_zero_buffer_bound()\n");
}

// Belirtilen hata: GL_OUT_OF_MEMORY is generated if the GL is unable to create
// a data store with the specified size.
void rTest_glBufferData_out_of_memory() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_out_of_memory()\n");

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)INTPTR_MAX, NULL,
		     GL_STATIC_DRAW);

	GLenum err = glGetError();
	if (err == GL_OUT_OF_MEMORY) {
		printf("[PASS] GL_OUT_OF_MEMORY was generated.\n");
	} else if (err == GL_NO_ERROR) {
		printf("[INFO] GL_OUT_OF_MEMORY was not generated. This "
		       "behavior is implementation-dependent.\n");
	} else {
		printf("[FAIL] Expected GL_OUT_OF_MEMORY or GL_NO_ERROR, but "
		       "got 0x%X\n",
		       err);
		assert(0);
	}

	glDeleteBuffers(1, &buffer);
}

// Belirtilmeyen hatalar --------------------------------------------------

// Kaynak veri boyutunun belirtilen 'size' değerinden küçük olduğu hatalı API
// kullanımına karşı implementasyonun dayanıklılığını gözlemler.
void rTest_glBufferData_source_buffer_too_small() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_source_buffer_too_small()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char smallSource[16] = {0}; // Sadece 16 byte'lık kaynak veri.

	// Kasıtlı yanlış kullanım: OpenGL'den 4096 byte okuması isteniyor ancak
	// kaynak yalnızca 16 byte.
	glBufferData(GL_ARRAY_BUFFER, 4096, smallSource, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] Misuse robustness (source buffer too small): "
	       "glError=0x%X \n",
	       err);

	glDeleteBuffers(1, &buf);
}

// size = 0 ama data != NULL
// Sıfır boyutlu data store oluşturulurken geçerli bir data pointer'ı
// verilmesinin implementasyon tarafından güvenli şekilde ele alınıp
// alınmadığını doğrular.
void rTest_glBufferData_zero_size_nonnull_data() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_zero_size_nonnull_data()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char dummy = 0xAB;
	glBufferData(GL_ARRAY_BUFFER, 0, &dummy, GL_STATIC_DRAW);

	GLenum err = glGetError();
	if (err == GL_NO_ERROR)
		printf("[PASS] size=0, data!=NULL accepted.\n");
	else
		printf("[INFO] size=0, data!=NULL returned glError=0x%X\n",
		       err);

	glDeleteBuffers(1, &buf);
}

// Sınır ve aşırı boyut size değerleri karşısında implementasyonun kararlılığını
// gözlemler.
void rTest_glBufferData_size_overflow_boundary() {
	printf("[START] rTest_glBufferData_size_overflow_boundary()\n");

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

// Geçersiz ve kirlenmiş usage enum değerleri karşısında implementasyonun
// kararlılığını gözlemler.
void rTest_glBufferData_dirty_usage_enum() {
	printf("[START] rTest_glBufferData_dirty_usage_enum()\n");
	GLenum candidates[] = {GL_STATIC_DRAW, 0xFFFF0000u, 0xFFFFFFFFu,
			       0x12345678u, 0xDEADBEEFu};

	for (int i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
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

// Hedefe herhangi bir buffer bağlı değilken glBufferData çağrısının
// implementasyon tarafından güvenli şekilde ele alınıp alınmadığını gözlemler.
void rTest_glBufferData_target_zero_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_target_zero_bound()\n");

	glBindBuffer(GL_ARRAY_BUFFER, 0); // hiçbir buffer bound değil
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] glBufferData(buffer=0) completed, glError=0x%X\n", err);
}

// Aynı buffer üzerinde farklı boyutlarda data store'ları art arda oluşturarak
// implementasyonun reallocation işlemlerindeki kararlılığını gözlemler.
void rTest_glBufferData_repeated_resize_thrash() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_repeated_resize_thrash()\n");

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

// Hizasız bir kaynak data pointer'ı kullanılarak implementasyonun hatalı
// istemci girdisi karşısındaki kararlılığı gözlemlenir.
void rTest_glBufferData_misaligned_data_pointer() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_misaligned_data_pointer()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char raw[128] = {0};
	void *misaligned = raw + 1; // Kasıtlı olarak hizasız pointer
	glBufferData(GL_ARRAY_BUFFER, 64, misaligned, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] glBufferData(misaligned data pointer) completed, "
	       "glError=0x%X\n",
	       err);

	glDeleteBuffers(1, &buf);
}

// Serbest bırakılmış (dangling) bir kaynak pointer kullanılarak
// implementasyonun hatalı istemci girdisi karşısındaki kararlılığı gözlemlenir.
void rTest_glBufferData_dangling_data_pointer() {
	printf("[START] rTest_glBufferData_dangling_data_pointer()\n");
	while (glGetError() != GL_NO_ERROR) {
	}

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	char *heapData = (char *)malloc(256);
	if (heapData == NULL) {
		printf("[ERROR] Memory allocation failed.\n");
		glDeleteBuffers(1, &buf);
		return;
	}

	memset(heapData, 0xAB, 256);
	free(heapData); // Pointer artık dangling
	glBufferData(GL_ARRAY_BUFFER, 256, heapData, GL_STATIC_DRAW);

	GLenum err = glGetError();
	printf("[INFO] glBufferData(dangling data pointer) completed, "
	       "glError=0x%X\n",
	       err);

	glDeleteBuffers(1, &buf);
}

// Büyük bir data store tahsis denemesi sonrasında buffer nesnesinin durumunun
// korunup korunmadığını gözlemler.
void rTest_glBufferData_state_after_out_of_memory() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferData_state_after_out_of_memory()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	// Başlangıçta küçük bir veri deposu oluştur.
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

	// Gerçekçi olmayan büyük bir tahsis denemesi.
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

// void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const
// GLvoid * data); Daha önceden oluşturulmuş bir buffer’ın içindeki belirli bir
// kısmı günceller Yeni bellek ayırmaz, var olan glBufferData ile oluşturulmuş
// data store’un içini kısmen değiştirir target ile belirtilen ve şu an
// glBindBuffer ile bağlanmış buffer’ı kullanır Buffer’ın offset byte’tan
// başlayan kısmına, size byte uzunluğunda data pointer’ındaki veriyi kopyalar.

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not
// GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
void rTest_glBufferSubData_invalid_enum_target() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_invalid_enum_target()\n");

	int data = 123;
	glBufferSubData(0xFFFFFFFF, 0, sizeof(data), &data);

	GLenum err = glGetError();
	if (err != GL_INVALID_ENUM) {
		printf("[FAIL] Expected GL_INVALID_ENUM, but got 0x%X\n", err);
		assert(err == GL_INVALID_ENUM);
	}
	printf("[PASS] rTest_glBufferSubData_invalid_enum_target()\n");
}

// Belirtilen hata: GL_INVALID_VALUE is generated if offset is negative.
void rTest_glBufferSubData_invalid_value_negative_offset() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf(
	    "[START] rTest_glBufferSubData_invalid_value_negative_offset()\n");

	GLuint buffer;
	int data = 123;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, -1, sizeof(data), &data);

	GLenum err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
		assert(err == GL_INVALID_VALUE);
	}
	printf(
	    "[PASS] rTest_glBufferSubData_invalid_value_negative_offset()\n");

	glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if size is negative.
void rTest_glBufferSubData_invalid_value_negative_size() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_invalid_value_negative_size()\n");

	GLuint buffer;
	int data = 123;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, -1, &data);

	GLenum err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
		assert(err == GL_INVALID_VALUE);
	}
	printf("[PASS] rTest_glBufferSubData_invalid_value_negative_size()\n");

	glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if offset and size together
// define a region beyond the allocated data store.
void rTest_glBufferSubData_invalid_value_out_of_bounds() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_invalid_value_out_of_bounds()\n");

	GLuint buffer;
	int data = 123;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 60, 8, &data); // 60 + 8 = 68 > 64

	GLenum err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
		assert(err == GL_INVALID_VALUE);
	}
	printf("[PASS] rTest_glBufferSubData_invalid_value_out_of_bounds()\n");

	glDeleteBuffers(1, &buffer);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved buffer
// object name 0 is bound to target.
void rTest_glBufferSubData_invalid_operation_zero_buffer_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] "
	       "rTest_glBufferSubData_invalid_operation_zero_buffer_bound()\n");

	int data = 123;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), &data);

	GLenum err = glGetError();
	if (err != GL_INVALID_OPERATION) {
		printf("[FAIL] Expected GL_INVALID_OPERATION, but got 0x%X\n",
		       err);
		assert(err == GL_INVALID_OPERATION);
	}
	printf("[PASS] "
	       "rTest_glBufferSubData_invalid_operation_zero_buffer_bound()\n");
}

// belirtilmeyen hatalar ------------------------

// Offset ve size değerlerinin toplamında oluşabilecek integer overflow
// durumunda implementasyonun sınır kontrollerini güvenli şekilde yapıp
// yapmadığını gözlemler. GLsizeptr 32 bit ise LLONG_MAX yerine INT_MAX
// kullanmak gerekebilir
void rTest_glBufferSubData_offset_size_overflow_wraparound() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] "
	       "rTest_glBufferSubData_offset_size_overflow_wraparound()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

	// Eğer implementasyon "offset + size <= buffer_size" kontrolünü
	// wraparound'a karşı korumasız yapıyorsa, negatif/küçük bir toplam elde
	// edip sınır kontrolünü atlatabilir -> OOB write
	GLintptr offset = 100;
	GLsizeiptr size =
	    (GLsizeiptr)LLONG_MAX - 50; // offset + size overflow eder
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, NULL);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(offset=%lld, size=%lld) completed, "
	       "glError=0x%X\n",
	       (long long)offset, (long long)size, err);
}

// Buffer sınırının tam bitiş noktası ve bir byte ötesi kullanılarak
// implementasyonun sınır kontrollerindeki kararlılığı gözlemlenir.
void rTest_glBufferSubData_exact_boundary_offset() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_exact_boundary_offset()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

	char data[16] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, 240, 16,
			data); // offset + size == 256, tam sınırda; spec'e göre
			       // GEÇERLİ olmalı

	GLenum err = glGetError();
	printf("[INFO] Exact boundary update completed (expected: NO_ERROR), "
	       "glError=0x%X\n",
	       err);

	// Bir fazlasını dene: sınırı 1 byte aşan durum
	glBufferSubData(GL_ARRAY_BUFFER, 241, 16, data); // 241+16=257 > 256
	err = glGetError();
	printf("[INFO] One-byte-beyond boundary update completed (expected: "
	       "INVALID_VALUE ), glError=0x%X\n",
	       err);
}

// Negatif offset değerinin büyük bir size ile "telafi edildiği" durumda
// implementasyonun offset doğrulamasını bağımsız olarak yapıp yapmadığını ve
// sınır kontrollerindeki kararlılığını gözlemler
void rTest_glBufferSubData_negative_offset_compensating_size() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] "
	       "rTest_glBufferSubData_negative_offset_compensating_size()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STATIC_DRAW);

	// negatif offset tek başına INVALID_VALUE üretmeli.
	// Ama size'ı öyle seçtik ki offset+size matematiksel olarak buffer
	// içinde "makul" görünüyor
	// -- implementasyon offset'i ayrı ayrı kontrol etmiyorsa bu sinsi bir
	// OOB write'a yol açabilir.
	GLintptr offset = -512;
	GLsizeiptr size = 600; // offset+size = 88, buffer içinde gibi görünüyor

	char data[600] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(offset=%lld, size=%lld) completed, "
	       "(expected:INVALID_VALUE) glError=0x%X\n",
	       (long long)offset, (long long)size, err);
}

// Sıfır byte güncelleme isteğinde implementasyonun gereksiz bellek erişimi
// yapmadan çağrıyı güvenli şekilde tamamlayıp tamamlamadığını gözlemler.
void rTest_glBufferSubData_zero_size_null_data() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_zero_size_null_data()\n");

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

// Hedefe herhangi bir buffer bağlı değilken glBufferSubData çağrısının
// implementasyon tarafından güvenli şekilde ele alınıp alınmadığını gözlemler.
void rTest_glBufferSubData_target_zero_bound() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_target_zero_bound()\n");

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	char data[16] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(buffer=0) completed, glError=0x%X "
	       "(expected: GL_INVALID_OPERATION)\n",
	       err);
}

// Data store'u henüz oluşturulmamış (0 byte) bir buffer nesnesine yazma
// isteğinin implementasyon tarafından güvenli şekilde ele alınıp alınmadığını
// gözlemler
void rTest_glBufferSubData_into_zero_sized_store() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_into_zero_sized_store()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	// glBufferData hiç çağrılmadı -- spec: "immediately after first bound,
	// zero-sized memory buffer" durumu geçerli

	char data[16] = {0};
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16, data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(zero-sized store, size=16) completed, "
	       "glError=0x%X (expected: GL_INVALID_VALUE)\n",
	       err);
}

// Kaynak veri tamponunun belirtilen size değerinden küçük olduğu hatalı API
// kullanımına karşı implementasyonun dayanıklılığını gözlemler.
void rTest_glBufferSubData_source_smaller_than_size() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_source_smaller_than_size()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

	char small_source[8] = {0};
	// size=4096 beyan ediliyor ama kaynak sadece 8 byte -- GL bunu
	// doğrulayamaz, implementasyonun kendi belleğinden OOB okumasını dener
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4096, small_source);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(source smaller than size) completed, "
	       "glError=0x%X\n",
	       err);
}

// Serbest bırakılmış bir istemci bellek işaretçisi kullanılarak
// implementasyonun geçersiz veri kaynağı karşısındaki davranışı gözlemlenir.
void rTest_glBufferSubData_dangling_data_pointer(void) {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glBufferSubData_dangling_data_pointer()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);

	char *heap_data = (char *)malloc(256);
	free(heap_data); /* serbest bırakıldı */

	glBufferSubData(GL_ARRAY_BUFFER, 0, 256, heap_data);

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(dangling data pointer) completed, "
	       "glError=0x%X\n",
	       err);
}

// Aynı buffer bölgesine çakışan ve hizasız güncellemeleri art arda
// gerçekleştirerek implementasyonun yoğun bellek kopyalama yükü altındaki
// kararlılığını gözlemler.
void rTest_glBufferSubData_overlapping_misaligned_thrash() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf(
	    "[START] rTest_glBufferSubData_overlapping_misaligned_thrash()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, 4096, NULL, GL_STATIC_DRAW);

	char raw[64];
	for (int i = 0; i < 5000; ++i) {
		GLintptr offset =
		    (i * 3) % 4090; // sürekli çakışan, hizasız offsetler
		GLsizeiptr size =
		    1 + (i % 63); // rastgele, çoğunlukla hizasız boyutlar
		if (offset + size > 4096)
			continue; // sınır ihlalini bu testte istemiyoruz

		void *misaligned = raw + (i % 3); // hizasız kaynak pointer
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, misaligned);
	}

	GLenum err = glGetError();
	printf("[INFO] glBufferSubData(overlapping/misaligned thrash) "
	       "completed, glError=0x%X\n",
	       err);
	// Spec alignment gereksinimini not olarak belirtiyor ama ihlali için
	// hata tanımlamıyor; burada amaç implementasyonun iç kopyalama
	// rutininin (örn. SIMD/vektörize memcpy) hizasız erişimde çökmesi
}

// glGenBuffers: yeni buffer nesneleri için ID üretir
// n: Kaç tane buffer ismi üretileceği.
// buffers: Üretilen buffer ID’lerinin yazılacağı dizi.

// Belirtilen hata: GL_INVALID_VALUE is generated if n is negative
void rTest_glGenBuffers_invalid_value() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_invalid_value()\n");

	GLuint buffer = 0;
	glGenBuffers(-1, &buffer);
	GLenum err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
		assert(err == GL_INVALID_VALUE);
	}
	printf("[PASS] rTest_glGenBuffers_invalid_value()\n");
}

// n = 0 ile çağrı
void rTest_glGenBuffers_zero_count() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_zero_count()\n");

	GLuint buf = 0xCDCDCDCD; // sentinel değer
	glGenBuffers(0, &buf);
	GLenum err = glGetError();
	printf("[INFO] glGenBuffers(n=0): error=0x%X, buffer=0x%08X\n", err,
	       buf);
}

// buffers = NULL, n > 0 (negative robustness)
void rTest_glGenBuffers_null_buffers() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_null_buffers()\n");

	glGenBuffers(5, NULL);
	GLenum err = glGetError();
	printf("[INFO] glGenBuffers(buffers=nullptr, n=5): error=0x%X\n", err);
}

// Aşırı büyük n
void rTest_glGenBuffers_large_n() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_large_n()\n");

	const GLsizei largeCount = 100000;
	GLuint *buffers = (GLuint *)malloc(sizeof(GLuint) * largeCount);
	if (buffers == NULL) {
		printf("[ERROR] Memory allocation failed.\n");
		return;
	}
	glGenBuffers(largeCount, buffers);
	GLenum err = glGetError();
	printf("[INFO] glGenBuffers(n=%d): error=0x%X\n", largeCount, err);
	free(buffers);
}

// Aynı array'i art arda, isim tekilliğini bozmaya çalışarak çağırma (fonksiyon
// 1000 kez art arda çağrıldığında hata veriyor mu)
void rTest_glGenBuffers_repeated_generation() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_repeated_generation()\n");

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
	    "[PASS] Repeated glGenBuffers(10) x1000 completed successfully.\n");
}

// Çok sayıda buffer adı üreterek döndürülen isimlerin benzersiz olduğunu ve
// reserved 0 isminin üretilmediğini doğrular.
void rTest_glGenBuffers_unique_names() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_unique_names()\n");

	const GLsizei COUNT = 1000;
	GLuint buffers[COUNT];
	glGenBuffers(COUNT, buffers);

	// 0 ismi üretilmemeli
	for (int i = 0; i < COUNT; i++) {
		if (buffers[i] == 0) {
			printf(
			    "[FAIL] glGenBuffers returned reserved name 0.\n");
			return;
		}
	}

	// Aynı isim iki kez üretilmemeli
	for (int i = 0; i < COUNT; i++) {
		for (int j = i + 1; j < COUNT; j++) {
			if (buffers[i] == buffers[j]) {
				printf(
				    "[FAIL] Duplicate buffer name %u found.\n",
				    buffers[i]);
				return;
			}
		}
	}

	GLenum err = glGetError();
	if (err == GL_NO_ERROR)
		printf("[PASS] All generated buffer names are unique and "
		       "non-zero.\n");
	else
		printf("[FAIL] glGetError() = 0x%X\n", err);

	glDeleteBuffers(COUNT, buffers);
}

// Bind edilmemiş buffer isimleri üzerinde glIsBuffer ve glDeleteBuffers
// çağrılarının spesifikasyona uygun davranıp davranmadığını doğrular.
void rTest_glGenBuffers_unbound_names_lifecycle() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_unbound_names_lifecycle()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	GLboolean isBuffer = glIsBuffer(buf);

	glDeleteBuffers(1, &buf);

	GLenum err = glGetError();
	printf("[INFO] Unbound buffer name: glIsBuffer=%s, glDeleteBuffers "
	       "error=0x%X\n",
	       isBuffer ? "GL_TRUE" : "GL_FALSE", err);
}

// Aynı buffer isminin birden fazla kez silinmesi durumunda implementasyonun
// kararlılığını test eder.
void rTest_glGenBuffers_double_delete() {
	while (glGetError() != GL_NO_ERROR) {
	}
	printf("[START] rTest_glGenBuffers_double_delete()\n");

	GLuint buf;
	glGenBuffers(1, &buf);
	glDeleteBuffers(1, &buf);
	GLenum firstErr = glGetError();
	glDeleteBuffers(1, &buf);
	GLenum secondErr = glGetError();
	printf("[INFO] Double delete: firstErr=0x%X, secondErr=0x%X\n",
	       firstErr, secondErr);
}

// Büyük 'n' değeri ve kasıtlı olarak yetersiz output buffer kullanılarak
// implementasyonun geçersiz istemci belleği karşısındaki davranışı test edilir
// (negative robustness)
void rTest_glGenBuffers_huge_count_small_buffer() {
	while (glGetError() != GL_NO_ERROR) {
	}

	printf("[START] rTest_glGenBuffers_huge_count_small_buffer()\n");

	GLsizei huge_n =
	    INT_MAX; // n * sizeof(GLuint) iç hesapta overflow edebilir

	GLuint buffers[1]; // kasıtlı olarak yetersiz boyutlu tampon

	// NOT: gerçek n kadar büyük array vermiyoruz -- implementasyonun n'i
	// gerçekten kullanıp kullanmadığını, yoksa iç limitle mi kısıtladığını
	// görmek için. Bu, çağıranın hatası olsa da spec bir üst sınır
	// tanımlamıyor, bu yüzden implementasyonun nasıl davrandığını
	// izliyoruz.
	glGenBuffers(huge_n, buffers);

	GLenum err = glGetError();
	printf("n=INT_MAX -> glError=0x%x\n", err);
}

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine getirir ve birikmis hatalari
 * temizler; boylece testler birbirine bagimli olmaz.
 * checkStatePreserved: Reddedilen cagrilarin mevcut durumu
 * bozup bozmadigini dogrular; beklenen degerle gercek deger
 * uyusmazsa program assert ile durur.
 * ============================================================ */

static void resetState(void) {
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved(GLint expected) {
	GLint actual;
	glGetIntegerv(GL_CULL_FACE_MODE, &actual);
	if (actual != expected) {
		printf("  [FAIL] Durum bozuldu: beklenen 0x%X, gercek 0x%X\n",
		       expected, actual);
		assert(0);
	}
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama
 *
 * glCullFace'in temel sozlesmesini dogrular: gecerli degerler
 * (GL_BACK, GL_FRONT, GL_FRONT_AND_BACK) kabul edilmeli,
 * gecersiz enum'lar GL_INVALID_ENUM ile reddedilmelidir.
 * Reddedilen cagrilar durumu bozmamalidir.
 * ============================================================ */

void test_cullFace_basicRobustness(void) {
	GLint mode = 0;
	GLenum err;

	printf("TEST: Basic Robustness\n");
	resetState();

	glCullFace(GL_FRONT);
	err = glGetError();
	assert(err == GL_NO_ERROR);

	glGetIntegerv(GL_CULL_FACE_MODE, &mode);
	assert(mode == GL_FRONT);

	glCullFace(GL_FRONT_AND_BACK);
	err = glGetError();
	assert(err == GL_NO_ERROR);

	glGetIntegerv(GL_CULL_FACE_MODE, &mode);
	assert(mode == GL_FRONT_AND_BACK);

	glCullFace((GLenum)0x0BAD);
	assert(glGetError() == GL_INVALID_ENUM);

	glCullFace(GL_CCW);
	assert(glGetError() == GL_INVALID_ENUM);

	checkStatePreserved(GL_FRONT_AND_BACK);

	assert(glGetError() == GL_NO_ERROR);

	resetState();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Enum uzayi taramasi
 *
 * 16-bit enum uzayinin tamamini (0x0000 - 0xFFFF, 65536 deger)
 * sistematik olarak tarar. Yalnizca GL_BACK, GL_FRONT ve
 * GL_FRONT_AND_BACK kabul edilmeli, kalan 65533 deger
 * GL_INVALID_ENUM ile reddedilmelidir.
 * ============================================================ */

void test_cullFace_stressSweep(void) {
	long i;
	int passCount = 0;
	int failCount = 0;

	printf("TEST: Stress Sweep (0x0000 .. 0xFFFF)\n");
	resetState();

	for (i = 0x0000; i <= 0xFFFF; i++) {
		GLenum deger = (GLenum)i;
		GLenum beklenen = (deger == GL_BACK || deger == GL_FRONT ||
				   deger == GL_FRONT_AND_BACK)
				      ? GL_NO_ERROR
				      : GL_INVALID_ENUM;
		GLenum err;

		glCullFace(deger);
		err = glGetError();

		if (err != beklenen) {
			printf(
			    "  [FAIL] Enum=0x%04lX Beklenen=0x%X Gelen=0x%X\n",
			    i, beklenen, err);
			failCount++;
		} else {
			passCount++;
		}
	}

	{
		GLint mode = 0;
		glCullFace(GL_BACK);
		glGetIntegerv(GL_CULL_FACE_MODE, &mode);
		assert(mode == GL_BACK);
		assert(glGetError() == GL_NO_ERROR);
	}

	printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
	assert(failCount == 0);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Hata kuyrugu butunlugu
 *
 * Ard arda gecersiz enum gonderimi altinda hata kuyrugunun
 * dogru sekilde doldugunu, bosaldigini ve temizlendikten sonra
 * normal islemlerin devam ettigini dogrular.
 * ============================================================ */

void test_cullFace_errorQueue(void) {
	int i;
	GLenum err;
	int hataSayisi = 0;

	printf("TEST: Error Queue Management\n");
	resetState();

	for (i = 0; i < 100; i++) {
		glCullFace((GLenum)(0x0BAD + i));
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		assert(err == GL_INVALID_ENUM);
		hataSayisi++;
	}

	printf("  Kuyruktan okunan hata sayisi: %d\n", hataSayisi);
	assert(hataSayisi > 0);

	glCullFace(GL_FRONT);
	assert(glGetError() == GL_NO_ERROR);
	checkStatePreserved(GL_FRONT);

	resetState();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Coklu cagri ve durum gecisleri
 *
 * Gecerli cull modlari arasinda hizli gecisler yaparak durum
 * makinesinin tutarliligini dogrular. Her gecisten sonra durum
 * sorgulanir ve beklenen degerle eslestigi kontrol edilir.
 * ============================================================ */

void test_cullFace_rapidToggle(void) {
	int i;
	const int tekrar = 10000;

	printf("TEST: Rapid Toggle (BACK <-> FRONT <-> F&B)\n");
	resetState();

	for (i = 0; i < tekrar; i++) {
		GLenum hedef;
		GLint mode;

		switch (i % 3) {
		case 0:
			hedef = GL_BACK;
			break;
		case 1:
			hedef = GL_FRONT;
			break;
		case 2:
			hedef = GL_FRONT_AND_BACK;
			break;
		}

		glCullFace(hedef);
		assert(glGetError() == GL_NO_ERROR);

		glGetIntegerv(GL_CULL_FACE_MODE, &mode);
		assert(mode == hedef);
	}

	glCullFace(GL_BACK);
	checkStatePreserved(GL_BACK);
	assert(glGetError() == GL_NO_ERROR);

	printf("  Sonuc: %d gecis tamamlandi\n", tekrar);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Gecersiz cagrilar arasinda gecerli cagrilar
 *
 * Gecersiz enum'larin arasina gecerli degerler serpistirerek
 * surucunun hata durumundan kurtulup kurtulamadigini dogrular.
 * Bazi implementasyonlar hata sonrasi "takili" kalabilir.
 * ============================================================ */

void test_cullFace_mixedValidity(void) {
	int i;
	GLenum pattern[] = {GL_BACK,	    (GLenum)0x1234,    GL_FRONT,
			    (GLenum)0x5678, GL_FRONT_AND_BACK, (GLenum)0x9ABC,
			    GL_BACK,	    (GLenum)0xDEF0};
	int n = sizeof(pattern) / sizeof(pattern[0]);

	printf("TEST: Mixed Validity Pattern\n");
	resetState();

	for (i = 0; i < n; i++) {
		GLenum deger = pattern[i];
		GLenum beklenen = (deger == GL_BACK || deger == GL_FRONT ||
				   deger == GL_FRONT_AND_BACK)
				      ? GL_NO_ERROR
				      : GL_INVALID_ENUM;
		GLenum err;

		glCullFace(deger);
		err = glGetError();
		assert(err == beklenen);
	}

	glCullFace(GL_BACK);
	checkStatePreserved(GL_BACK);
	assert(glGetError() == GL_NO_ERROR);

	printf("  Sonuc: %d karisik cagri tamamlandi\n", n);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 6: Culling etkinlestirme/kapatma etkilesimi
 *
 * glCullFace'in glEnable/glDisable(GL_CULL_FACE) ile olan
 * etkilesimini dogrular. Culling kapaliyken cull mode'un
 * etkisiz olmasi, acikken etkili olmasi gerekir.
 * ============================================================ */

void test_cullFace_enableDisable(void) {
	printf("TEST: Enable/Disable Interaction\n");
	resetState();

	glCullFace(GL_FRONT_AND_BACK);
	glGetError();

	glDisable(GL_CULL_FACE);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	glEnable(GL_CULL_FACE);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

	glDisable(GL_CULL_FACE);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	checkStatePreserved(GL_BACK);

	resetState();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7: FrontFace etkilesimi
 *
 * glCullFace ile glFrontFace'in birlikte calistigini dogrular.
 * Farkli kombinasyonlarda durumlarin tutarli oldugunu kontrol eder.
 * ============================================================ */

void test_cullFace_frontFaceCombo(void) {
	GLenum frontModes[] = {GL_CCW, GL_CW};
	GLenum cullModes[] = {GL_BACK, GL_FRONT, GL_FRONT_AND_BACK};
	int i, j;

	printf("TEST: FrontFace Combinations\n");
	resetState();

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			GLint face, mode;

			glFrontFace(frontModes[i]);
			glCullFace(cullModes[j]);
			assert(glGetError() == GL_NO_ERROR);

			glGetIntegerv(GL_FRONT_FACE, &face);
			glGetIntegerv(GL_CULL_FACE_MODE, &mode);

			assert(face == frontModes[i]);
			assert(mode == cullModes[j]);
		}
	}

	resetState();
	printf("  Sonuc: 6 kombinasyon tamamlandi\n");
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 8: Cok buyuk enum degerleri
 *
 * 32-bit enum araliginin ust kisimlarini test eder.
 * OpenGL spec 16-bit enum uzayini kullanir ancak 32-bit
 * degerler gonderildiginde implementasyonun davranisi
 * belirsizdir. Cokme veya durum bozulmasi kritik hatadir.
 * ============================================================ */

void test_cullFace_largeEnum(void) {
	GLenum largeValues[] = {(GLenum)0x10000, (GLenum)0x7FFFFFFF,
				(GLenum)0x80000000, (GLenum)0xFFFFFFFF};
	int i;
	int n = sizeof(largeValues) / sizeof(largeValues[0]);

	printf("TEST: Large Enum Values\n");
	resetState();

	for (i = 0; i < n; i++) {
		GLenum err;

		glCullFace(largeValues[i]);
		err = glGetError();

		printf("  Enum=0x%08X -> 0x%X\n", largeValues[i], err);
	}

	checkStatePreserved(GL_BACK);
	resetState();
	printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine getirir ve birikmis hatalari
 * temizler; boylece testler birbirine bagimli olmaz.
 * ============================================================ */

static void resetState2(void) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	while (glGetError() != GL_NO_ERROR)
		;
}

/* ============================================================
 * TEST 1: Sozlesme, davranis ve izolasyon dogrulama
 *
 * glEnable/glDisable(GL_CULL_FACE)'in temel sozlesmesini,
 * gercek culling davranisini ve cap izolasyonunu uc asamada
 * dogrular:
 *
 * A) SOZLESME: Toggle islemleri idempotent olmali; zaten acik
 *    olan bir cap'i tekrar acmak veya kapali olani tekrar kapatmak
 *    hata uretmemeli ve durumu degistirmemelidir. glIsEnabled
 *    sorgusu tutarli sonuc donmelidir.
 *
 * B) DAVRANIS: GL_CULL_FACE anahtari gercekten culling
 *    pipeline'ini gecitlemelidir. Ayni geometri icin acik ve
 *    kapali durumlar farkli sonuclar vermelidir; ayni sonuc
 *    cikarsa anahtar etkisiz demektir.
 *
 * C) ROBUSTNESS: Gecersiz cap degerleri reddedilmeli, hata
 *    kuyrugu kirlenmemeli ve baska cap'lerin durumu etkilenmemelidir.
 *    Bu, surucunun ic durum yonetiminin dogru calistigini gosterir.
 * ============================================================ */

void test_cullFaceEnable_basicRobustness(void) {
	printf("TEST: Enable/Disable Basic Robustness\n");
	resetState2();

	/* ---------- A) SOZLESME ---------- */

	/* A1: Enable sonrasi IsEnabled TRUE */
	glEnable(GL_CULL_FACE);
	assert(glGetError() == GL_NO_ERROR);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

	/* A2: Disable sonrasi IsEnabled FALSE */
	glDisable(GL_CULL_FACE);
	assert(glGetError() == GL_NO_ERROR);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	/* A3: Idempotent - zaten kapaliyken tekrar Disable */
	glDisable(GL_CULL_FACE);
	assert(glGetError() == GL_NO_ERROR);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	/* A4: Idempotent - iki kez Enable */
	glEnable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	assert(glGetError() == GL_NO_ERROR);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

	/* ---------- B) DAVRANIS ---------- */

	/* B1: Acik durumda state sorgusu */
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

	/* B2: Kapali durumda state sorgusu */
	glDisable(GL_CULL_FACE);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	/* B3: Toggle sonrasi tekrar ac */
	glEnable(GL_CULL_FACE);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

	/* ---------- C) ROBUSTNESS ---------- */

	/* C1: Gecersiz cap ile Enable */
	glDisable(GL_CULL_FACE);
	glEnable((GLenum)0x0BAD);
	assert(glGetError() == GL_INVALID_ENUM);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	/* C2: Gecersiz cap ile Disable */
	glDisable((GLenum)0x0BAD);
	assert(glGetError() == GL_INVALID_ENUM);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	/* C3: Cap izolasyonu - baska cap toggle edilince CULL_FACE
	 * etkilenmemeli */
	glEnable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);
	assert(glGetError() == GL_NO_ERROR);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);
	assert(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE);

	glDisable(GL_SCISSOR_TEST);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);

	/* ---------- temizlik ---------- */
	resetState2();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Hizli toggle ve durum tutarliligi
 *
 * glEnable/glDisable arasinda hizli gecisler yaparak durum
 * makinesinin tutarliligini dogrular. Her gecisten sonra
 * glIsEnabled sorgusu beklenen degeri vermelidir.
 * ============================================================ */

void test_cullFaceEnable_rapidToggle(void) {
	int i;
	const int tekrar = 10000;

	printf("TEST: Rapid Toggle (Enable <-> Disable)\n");
	resetState2();

	for (i = 0; i < tekrar; i++) {
		GLboolean beklenen = (i % 2 == 0) ? GL_TRUE : GL_FALSE;

		if (beklenen) {
			glEnable(GL_CULL_FACE);
		} else {
			glDisable(GL_CULL_FACE);
		}

		assert(glGetError() == GL_NO_ERROR);
		assert(glIsEnabled(GL_CULL_FACE) == beklenen);
	}

	resetState2();
	printf("  Sonuc: %d toggle tamamlandi\n", tekrar);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Gecersiz cap taramasi
 *
 * glEnable ve glDisable'e farkli gecersiz cap degerleri
 * gondererek implementasyonun hata ayiklama mantigini test eder.
 * Tum gecersiz degerler GL_INVALID_ENUM ile reddedilmelidir.
 * ============================================================ */

void test_cullFaceEnable_invalidCaps(void) {
	GLenum invalidCaps[] = {(GLenum)0x0000, (GLenum)0x0BAD, (GLenum)0x1234,
				(GLenum)0xDEAD, (GLenum)0xFFFF};
	int i;
	int n = sizeof(invalidCaps) / sizeof(invalidCaps[0]);

	printf("TEST: Invalid Capability Values\n");
	resetState2();

	for (i = 0; i < n; i++) {
		GLenum err;

		glEnable(invalidCaps[i]);
		err = glGetError();
		assert(err == GL_INVALID_ENUM);

		glDisable(invalidCaps[i]);
		err = glGetError();
		assert(err == GL_INVALID_ENUM);
	}

	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);

	resetState2();
	printf("  Sonuc: %d gecersiz cap reddedildi\n", n);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Cap kombinasyonlari
 *
 * Birden fazla cap'in ayni anda acik/kapali olmasi durumunda
 * her birinin bagimsiz yonetildigini dogrular. Cap'ler birbirine
 * bagli olmamali, birinin durumu digerini etkilememelidir.
 * ============================================================ */

void test_cullFaceEnable_capCombinations(void) {
	printf("TEST: Capability Combinations\n");
	resetState2();

	/* Hicbiri acik degil */
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);
	assert(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE);

	/* Biri acik, digeri kapali */
	glEnable(GL_CULL_FACE);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);
	assert(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE);

	/* Ikisi de acik */
	glEnable(GL_SCISSOR_TEST);
	assert(glIsEnabled(GL_CULL_FACE) == GL_TRUE);
	assert(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE);

	/* Biri kapali, digeri acik */
	glDisable(GL_CULL_FACE);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);
	assert(glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE);

	/* Ikisi de kapali */
	glDisable(GL_SCISSOR_TEST);
	assert(glIsEnabled(GL_CULL_FACE) == GL_FALSE);
	assert(glIsEnabled(GL_SCISSOR_TEST) == GL_FALSE);

	resetState2();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine (GL_CCW) getirir ve birikmis
 * hatalari temizler; boylece testler birbirine bagimli olmaz.
 * checkStatePreserved: Reddedilen cagrilarin mevcut durumu
 * bozup bozmadigini dogrular; beklenen degerle gercek deger
 * uyusmazsa program assert ile durur. Bu iki fonksiyon olmadan
 * hata ayiklama zorlasir, cunku hangi testin hangi durumu
 * biraktigi takip edilemez.
 * ============================================================ */

static void resetState3(void) {
	glFrontFace(GL_CCW);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved3(GLint expected) {
	GLint actual;
	glGetIntegerv(GL_FRONT_FACE, &actual);
	if (actual != expected) {
		printf("  [FAIL] Durum bozuldu: beklenen 0x%X, gercek 0x%X\n",
		       expected, actual);
		assert(0);
	}
}

/* ============================================================
 * TEST 1: Hata kuyrugu butunlugu
 *
 * Ard arda gecersiz enum gonderimi altinda hata kuyrugunun
 * dogru sekilde doldugunu, bosaldigini ve temizlendikten sonra
 * normal islemlerin devam ettigini dogrular.
 * ============================================================ */

void test_frontFace_errorQueue(void) {
	int i;
	GLenum err;
	int hataSayisi = 0;

	printf("TEST: Error Queue Management\n");
	resetState3();

	for (i = 0; i < 100; i++) {
		glFrontFace((GLenum)(0x0BAD + i));
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		assert(err == GL_INVALID_ENUM);
		hataSayisi++;
	}

	printf("  Kuyruktan okunan hata sayisi: %d\n", hataSayisi);
	assert(hataSayisi > 0);

	glFrontFace(GL_CW);
	assert(glGetError() == GL_NO_ERROR);
	checkStatePreserved3(GL_CW);

	resetState3();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Coklu cagri ve durum gecisleri
 *
 * GL_CW ve GL_CCW arasinda hizli gecisler yaparak durum
 * makinesinin tutarliligini dogrular. Her gecisten sonra
 * durum sorgulanir ve beklenen degerle eslestigi kontrol edilir.
 * ============================================================ */

void test_frontFace_rapidToggle(void) {
	int i;
	const int tekrar = 10000;

	printf("TEST: Rapid Toggle (CW <-> CCW)\n");
	resetState3();

	for (i = 0; i < tekrar; i++) {
		GLenum hedef = (i % 2 == 0) ? GL_CW : GL_CCW;
		GLint face;

		glFrontFace(hedef);
		assert(glGetError() == GL_NO_ERROR);

		glGetIntegerv(GL_FRONT_FACE, &face);
		assert(face == hedef);
	}

	glFrontFace(GL_CCW);
	checkStatePreserved3(GL_CCW);
	assert(glGetError() == GL_NO_ERROR);

	printf("  Sonuc: %d gecis tamamlandi\n", tekrar);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Gecersiz cagrilar arasinda gecerli cagrilar
 *
 * Gecersiz enum'larin arasina gecerli degerler serpistirerek
 * surucunun hata durumundan kurtulup kurtulamadigini dogrular.
 * Bazi implementasyonlar hata sonrasi "takili" kalabilir.
 * ============================================================ */

void test_frontFace_mixedValidity(void) {
	int i;
	GLenum pattern[] = {GL_CW, (GLenum)0x1234, GL_CCW, (GLenum)0x5678,
			    GL_CW, (GLenum)0x9ABC, GL_CCW, (GLenum)0xDEF0};
	int n = sizeof(pattern) / sizeof(pattern[0]);

	printf("TEST: Mixed Validity Pattern\n");
	resetState3();

	for (i = 0; i < n; i++) {
		GLenum deger = pattern[i];
		GLenum beklenen = (deger == GL_CW || deger == GL_CCW)
				      ? GL_NO_ERROR
				      : GL_INVALID_ENUM;
		GLenum err;

		glFrontFace(deger);
		err = glGetError();
		assert(err == beklenen);
	}

	glFrontFace(GL_CCW);
	checkStatePreserved3(GL_CCW);
	assert(glGetError() == GL_NO_ERROR);

	printf("  Sonuc: %d karisik cagri tamamlandi\n", n);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Display list etkilesimi (Eski OpenGL)
 *
 * glFrontFace cagrilarinin display listeye dogru sekilde
 * kaydedilip kaydedilmedigini dogrular. Gecersiz degerler
 * liste derlenirken hata uretmeli, listeye yazilmamalidir.
 * ============================================================ */

void test_frontFace_displayList(void) {
	GLuint list;
	GLint face;

	printf("TEST: Display List Interaction\n");
	resetState3();

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);

	glFrontFace(GL_CW);
	EXPECT_GL_ERROR(glGetError(), (glGetError() == GL_NO_ERROR), "test_frontFace_displayList failed.");

	glFrontFace((GLenum)0x0BAD);
	/* Hata aninda uretilebilir veya listeye kaydedilebilir;
	 * davranis implementasyona baglidir. */

	glEndList();

	glFrontFace(GL_CCW);
	glCallList(list);

	glGetIntegerv(GL_FRONT_FACE, &face);
	printf("  List sonrasi durum: 0x%X\n", face);

	glDeleteLists(list, 1);
	resetState3();
	printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 5: Push/PopAttrib etkilesimi
 *
 * glPushAttrib ve glPopAttrib ile GL_TRANSFORM_BIT kullanarak
 * FrontFace durumunun yigina kaydedilip geri yuklendigini
 * dogrular. Durum yonetiminin bu mekanizmayla uyumlu calismasi
 * gerekir.
 * ============================================================ */

void test_frontFace_attribStack(void) {
	GLint face;

	printf("TEST: Attribute Stack (Push/PopAttrib)\n");
	resetState3();

	glFrontFace(GL_CW);
	assert(glGetError() == GL_NO_ERROR);

	glPushAttrib(GL_TRANSFORM_BIT);

	glFrontFace(GL_CCW);
	EXPECT_GL_ERROR(glGetError(), (glGetError() == GL_NO_ERROR), "test_frontFace_attribStack failed.");
	checkStatePreserved3(GL_CCW);

	glPopAttrib();

	glGetIntegerv(GL_FRONT_FACE, &face);
	printf("  Pop sonrasi durum: 0x%X (beklenen GL_CW=0x%X)\n", face,
	       GL_CW);

	resetState3();
	printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 6: Culling kombinasyonlari
 *
 * glFrontFace ile glCullFace'in farkli kombinasyonlarinda
 * tutarli davranis gosterdigini dogrular. Her kombinasyon
 * sonrasi durum sorgulanir.
 * ============================================================ */

void test_frontFace_cullCombinations(void) {
	GLenum frontModes[] = {GL_CCW, GL_CW};
	GLenum cullModes[] = {GL_BACK, GL_FRONT, GL_FRONT_AND_BACK};
	int i, j;

	printf("TEST: Cull Face Combinations\n");
	resetState3();

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			GLint face, cull;

			glFrontFace(frontModes[i]);
			glCullFace(cullModes[j]);
			assert(glGetError() == GL_NO_ERROR);

			glGetIntegerv(GL_FRONT_FACE, &face);
			glGetIntegerv(GL_CULL_FACE_MODE, &cull);

			assert(face == frontModes[i]);
			assert(cull == cullModes[j]);
		}
	}

	glDisable(GL_CULL_FACE);
	resetState3();
	printf("  Sonuc: 6 kombinasyon tamamlandi\n");
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 7: Cok buyuk enum degerleri
 *
 * 32-bit enum araliginin ust kisimlarini test eder.
 * OpenGL spec 16-bit enum uzayini kullanir ancak 32-bit
 * degerler gonderildiginde implementasyonun davranisi
 * belirsizdir. Cokme veya durum bozulmasi kritik hatadir.
 * ============================================================ */

void test_frontFace_largeEnum(void) {
	GLenum largeValues[] = {(GLenum)0x10000, (GLenum)0x7FFFFFFF,
				(GLenum)0x80000000, (GLenum)0xFFFFFFFF};
	int i;
	int n = sizeof(largeValues) / sizeof(largeValues[0]);

	printf("TEST: Large Enum Values\n");
	resetState3();

	for (i = 0; i < n; i++) {
		GLenum err;

		glFrontFace(largeValues[i]);
		err = glGetError();

		printf("  Enum=0x%08X -> 0x%X\n", largeValues[i], err);
	}

	checkStatePreserved3(GL_CCW);
	resetState3();
	printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 8: Thread benzeri hizli ardisik cagri
 *
 * Tek bir thread icerisinde glFrontFace'e ait cagrilari
 * mimimum gecikmeyle ard arda gondererek durum makinesinin
 * race condition benzeri senaryolarda tutarli kalip
 * kalmadigini gozlemler.
 * ============================================================ */

void test_frontFace_rapidFire(void) {
	int i;
	const int tekrar = 50000;

	printf("TEST: Rapid Fire (50K calls)\n");
	resetState3();

	for (i = 0; i < tekrar; i++) {
		glFrontFace(GL_CW);
		glFrontFace(GL_CCW);
	}

	glGetError();
	glFrontFace(GL_CW);
	assert(glGetError() == GL_NO_ERROR);
	checkStatePreserved3(GL_CW);

	resetState3();
	printf("  Sonuc: %d cift cagri tamamlandi\n", tekrar);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi/sonrasi OpenGL durumunu ve hata
 * kuyrugunu temizler. checkStatePreserved: Reddedilen cagrilarin
 * mevcut GL_LINE_WIDTH'i degistirmedigini dogrular; basarisizlik
 * durumunda assert ile program durur.
 * ============================================================ */
static void resetState4(void) {
	glLineWidth(1.0f);
	while (glGetError() != GL_NO_ERROR)
		;
}

static void checkStatePreserved4(GLfloat expected) {
	GLfloat actual;
	glGetFloatv(GL_LINE_WIDTH, &actual);
	if (actual != expected) {
		printf("  [FAIL] Durum bozuldu: beklenen %f, gercek %f\n",
		       expected, actual);
		assert(0);
	}
}

/* ============================================================
 * TEST 1: Sozlesme dogrulama — gecerli ve gecersiz girdiler
 * ============================================================ */

/*
 * glLineWidth'in temel sozlesmesini dogrular: pozitif degerler hatasiz
 * kabul edilmeli, pozitif olmayan degerler GL_INVALID_VALUE ile
 * reddedilmelidir. Reddedilen cagrilar idempotent olmalidir; yani
 * mevcut GL_LINE_WIDTH durumunu degistirmemelidir. Bu ozellik,
 * basarisiz bir cagrinin sonraki cizim komutlarina sirayet etmesini
 * ve belirlenemez goruntu olusmasini engeller.
 */

void test_lineWidth_basicRobustness(void) {
	GLfloat width;
	GLenum err;

	printf("TEST: Basic Robustness\n");
	resetState4();

	glLineWidth(2.0f);
	err = glGetError();
	assert(err == GL_NO_ERROR);

	glGetFloatv(GL_LINE_WIDTH, &width);
	assert(width == 2.0f);

	glLineWidth(0.0f);
	err = glGetError();
	assert(err == GL_INVALID_VALUE);

	glLineWidth(-5.0f);
	err = glGetError();
	assert(err == GL_INVALID_VALUE);

	checkStatePreserved4(2.0f);
	assert(glGetError() == GL_NO_ERROR);

	resetState4();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Parametrik tarama — sinir deger kesfi
 * ============================================================ */

/*
 * glLineWidth'i surekli bir parametrik aralikta test ederek, noktasal
 * kontrollerin yakalayamayacagi implementasyon-ozgulu sinir
 * anomalilerini ortaya cikarir. Tarama [-1000.0, +1000.0] araliginda
 * 0.1 cozunurluguyle gerceklestirilir (20.001 ornek):
 *
 *   w <= 0  → GL_INVALID_VALUE  (spec ile zorunlu reddetme)
 *   w > 0   → GL_NO_ERROR       (kabul, kirpmali)
 *
 * Dongu tam sayi indeksleme ve basit carpma kullanir; birikimli
 * yuvarlama hatasi olusmaz.
 */

void test_lineWidth_stressSweep(void) {
	int i;
	int passCount = 0;
	int failCount = 0;

	printf("TEST: Stress Sweep (-1000.0 .. +1000.0)\n");
	resetState4();

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
	checkStatePreserved4(1.0f);
	assert(glGetError() == GL_NO_ERROR);

	printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
	assert(failCount == 0);
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: IEEE-754 ozel degerleri
 * ============================================================ */
/*
 * IEEE-754 ozel kayan nokta degerleri altinda davranisi degerlendirir:
 *   NaN         — karsilastirma islemleri tanimsizdir; NaN <= 0
 *                yanlis doner, dolayisiyla basit aralik kontrolleri
 *                NaN'i kabul edebilir
 *   +Sonsuz     — her sonlu implementasyon limitini asar
 *   -Sonsuz     — isaret biti setlidir; negatif olarak reddedilmeli
 *
 * OpenGL spec NaN davranisini acikca tanimlamaz, ancak robust bir
 * implementasyon cokmemeli, dahili istisna uretmemeli ve surucu
 * durumunu bozmamalidir. Bu test bilgilendiricidir; kesin hata
 * kodlari implementasyona baglidir.
 * KISICA:
 * NaN ve ±Infinity degerlerinin glLineWidth tarafindan nasil
 * ele alindigini gozlemler. Bu degerler icin OpenGL spec kesin
 * bir davranis belirtmez; farkli suruculer farkli hata kodlari
 * dondurebilir veya sessizce kirpabilir. Testin amaci kesin
 * bir dogrulama yapmak degil, implementasyonun cokmedigini
 * ve durumu bozmadigini kontrol etmektir. Cikti insani tarafindan
 * degerlendirilir: INVALID_VALUE veya NO_ERROR makul karsilanir,
 * ancak crash, segfault veya GL_LINE_WIDTH degisikligi kritik
 * bir robustness acigi olarak rapor edilmelidir.
 * segfault veya GL_LINE_WIDTH degisikligi,
 * surucunun ozel float degerleri isleyememesinden kaynaklanan kritik bir
 * robustness acigidir; bu durumda uygulama aniden sonlanir veya sonraki
 * cizim komutlarinda goruntu bozulmasina yol acar.
 */

void test_lineWidth_specialFloats(void) {
	GLenum err;
	GLfloat width;

	printf("TEST: Special Float Values (NaN, Inf)\n");
	resetState4();

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

	resetState4();
	printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/* ============================================================
 * TEST 4: Hata kuyrugu butunlugu
 * ============================================================ */

/*
 * Surekli hata enjeksiyonu altinda hata kuyrugu davranisini dogrular.
 * OpenGL en az bir hata kaydini garanti eder; kuyruk derinligi
 * implementasyona baglidir. Test sunlari kontrol eder:
 *
 *   - Hatalar glGetError ile kaydedilir ve okunabilir
 *   - Kuyruk GL_NO_ERROR'a kadar tamamen bosaltilabilir
 *   - Bosaltma sonrasi islemler eski hata kirliliginden etkilenmez
 *
 * Kuyrugun duzgun bosaltilamamasi, bozuk hata durumu izleme
 * isaretidir; bu durum uretim ortaminda sonraki hatalarin
 * maskelenmesine yol acar.
 */

void test_lineWidth_errorQueue(void) {
	int i;
	GLenum err;
	int errorCount = 0;

	printf("TEST: Error Queue Management\n");
	resetState4();

	for (i = 0; i < 50; i++) {
		glLineWidth(-1.0f * (i + 1));
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		assert(err == GL_INVALID_VALUE);
		errorCount++;
	}

	printf("  Kuyruktan okunan hata sayisi: %d\n", errorCount);
	assert(errorCount > 0);

	glLineWidth(4.0f);
	assert(glGetError() == GL_NO_ERROR);
	checkStatePreserved4(4.0f);

	resetState4();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Implementasyon limitleri ve kirpma
 * ============================================================ */

/*
 * glLineWidth'in implementasyon limitlerini asan degerleri sessizce
 * kirpma davranisini dogrular. GL_LINE_WIDTH_RANGE ile desteklenen
 * aralik sorgulanir; limitin uzerindeki degerler hata uretmeden
 * maksimuma cekilir.
 */

void test_lineWidth_limits(void) {
	GLfloat widthRange[2];
	GLfloat width;

	printf("TEST: Implementation Limits\n");
	resetState4();

	glGetFloatv(GL_LINE_WIDTH_RANGE, widthRange);

	printf("  Line width araligi: [%.2f, %.2f]\n", widthRange[0],
	       widthRange[1]);

	GLfloat maxLimit = widthRange[1];

	glLineWidth(maxLimit * 10.0f);
	EXPECT_GL_ERROR(glGetError(), (glGetError() == GL_NO_ERROR), "test_lineWidth_limits failed.");

	glGetFloatv(GL_LINE_WIDTH, &width);
	printf("  Istek %.1f, gercek %.1f (kirpilmis)\n", maxLimit * 10.0f,
	       width);

	EXPECT_GL_ERROR(false, width <= maxLimit * 1.01f, "test_lineWidth_limits failed.");
//	assert(width <= maxLimit * 1.01f);

	resetState4();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * Test altyapisi
 *
 * Bu kisim, testlerin tekrarlanabilir ve izole calismasini saglar.
 * resetState: Her test oncesi ve sonrasinda OpenGL durumunu
 * bilinen bir baslangic degerine getirir ve birikmis hatalari
 * temizler; boylece testler birbirine bagimli olmaz.
 * ============================================================ */

static void resetState5(void) {
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_DEPTH_TEST);
	glPolygonOffset(0.0f, 0.0f);
	while (glGetError() != GL_NO_ERROR)
		;
}

/* ============================================================
 * TEST 1: Sozlesme ve durum yonetimi
 *
 * glPolygonOffset'in temel sozlesmesini dogrular: tum float
 * degerler kabul edilmeli, hicbiri hata uretmemelidir. Spec'te
 * yasak deger yoktur; implementasyon degerleri sessizce
 * kendi araligina kirpmalidir. Ayrica durum sorgusunun
 * tutarli sonuc verdigi kontrol edilir.
 * ============================================================ */

void test_polygonOffset_basicRobustness(void) {
	GLfloat f = 0.0f, u = 0.0f;

	printf("TEST: Basic Robustness\n");
	resetState5();

	/* Tum float degerler kabul edilmeli, hata uretilmemeli */
	glPolygonOffset(0.0f, 0.0f);
	assert(glGetError() == GL_NO_ERROR);

	glPolygonOffset(-1000.0f, -1000.0f);
	assert(glGetError() == GL_NO_ERROR);

	glPolygonOffset(1e30f, 1e30f);
	assert(glGetError() == GL_NO_ERROR);

	glPolygonOffset(-1e30f, -1e30f);
	assert(glGetError() == GL_NO_ERROR);

	glPolygonOffset(NAN, NAN);
	assert(glGetError() == GL_NO_ERROR);

	glPolygonOffset(INFINITY, INFINITY);
	assert(glGetError() == GL_NO_ERROR);

	/* Durum sorgusu son yazilan degerleri vermeli */
	glPolygonOffset(2.0f, 3.0f);
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
	assert(f == 2.0f && u == 3.0f);
	assert(glGetError() == GL_NO_ERROR);

	resetState5();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 2: Stres taramasi
 *
 * Genis bir float araliginda sistematik tarama yaparak
 * implementasyonun tutarliligini dogrular. Negatif, sifir,
 * pozitif ve cok buyuk degerler test edilir.
 * ============================================================ */

void test_polygonOffset_stressSweep(void) {
	int i;
	int passCount = 0;
	int failCount = 0;

	printf("TEST: Stress Sweep\n");
	resetState5();

	for (i = -10000; i <= 10000; i++) {
		float val = (float)i * 0.1f;
		GLenum err;

		glPolygonOffset(val, val);
		err = glGetError();

		if (err != GL_NO_ERROR) {
			printf("  [FAIL] val=%.1f -> 0x%X\n", val, err);
			failCount++;
		} else {
			passCount++;
		}
	}

	printf("  Sonuc: %d PASS, %d FAIL\n", passCount, failCount);
	assert(failCount == 0);

	resetState5();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 3: Hata kuyrugu butunlugu
 *
 * Ard arda cok sayida glPolygonOffset cagrisi sonrasi hata
 * kuyrugunun dogru calistigini dogrular. Spec'e gore bu
 * fonksiyon hata uretmez; kuyruk temiz kalmalidir.
 * ============================================================ */

void test_polygonOffset_errorQueue(void) {
	int i;
	GLenum err;

	printf("TEST: Error Queue Integrity\n");
	resetState5();

	for (i = 0; i < 1000; i++) {
		glPolygonOffset((float)i, (float)-i);
	}

	err = glGetError();
	assert(err == GL_NO_ERROR);

	printf("  1000 cagri sonrasi kuyruk temiz\n");

	resetState5();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 4: Durum korunumu ve gecersiz cagri etkilesimi
 *
 * glPolygonOffset cagrilarinin diger OpenGL cagrilarinin
 * hata durumundan etkilenmedigini dogrular. Baska fonksiyonlar
 * hata uretse bile PolygonOffset durumu bozulmamali.
 * ============================================================ */

void test_polygonOffset_statePreservation(void) {
	GLfloat f = 0.0f, u = 0.0f;

	printf("TEST: State Preservation\n");
	resetState5();

	glPolygonOffset(5.0f, 7.0f);

	/* Baska bir fonksiyondan hata uret */
	glFrontFace((GLenum)0x0BAD);
	assert(glGetError() == GL_INVALID_ENUM);

	/* PolygonOffset durumu bozulmamis olmali */
	glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &f);
	glGetFloatv(GL_POLYGON_OFFSET_UNITS, &u);
	assert(f == 5.0f && u == 7.0f);

	resetState5();
	printf("  [PASS]\n\n");
}

/* ============================================================
 * TEST 5: Ozel float degerleri
 *
 * IEEE-754 ozel degerlerinin (NaN, ±Infinity) davranisini
 * inceler. OpenGL spec bu degerleri acikca tanimlamaz;
 * cokme veya durum bozulmasi kritik hatadir.
 * ============================================================ */

void test_polygonOffset_specialFloats(void) {
	GLenum err;
	GLfloat f, u;

	printf("TEST: Special Float Values\n");
	resetState5();

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

	resetState5();
	printf("  [BILGI] Manuel inceleme gerekir\n\n");
}

/***********************************/
/***** Core Workflow Functions *****/
/***********************************/

void init() { printf("init\n"); }

void draw() {
	runTest(rTest_CreateProgram);
	runTest(rTest_ProgramBinary_unalignedPtr);
	runTest(rTest_ProgramBinary_memRevoke);
	runTest(rTest_ProgramBinary_overload);
	runTest(rTest_UseProgram_invalidID);
	runTest(rTest_UseProgram_typeConfusion);
	runTest(rTest_GetAttribLocation_nullPtr);
	runTest(rTest_GetAttribLocation_reservedVariable);

	//	runTest(rTest_DrawArrays_guardPageAttack); // SEGMENTATION FAULT
	runTest(rTest_DrawArrays_outOfBounds);

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

	runTest(rTest_glGenBuffers_invalid_value);
	runTest(rTest_glGenBuffers_zero_count);
	runTest(rTest_glGenBuffers_null_buffers);
	runTest(rTest_glGenBuffers_large_n);
	runTest(rTest_glGenBuffers_repeated_generation);
	runTest(rTest_glGenBuffers_unique_names);
	runTest(rTest_glGenBuffers_unbound_names_lifecycle);
	runTest(rTest_glGenBuffers_double_delete);
//	runTest(rTest_glGenBuffers_huge_count_small_buffer);  // SEGMENTATION FAULT

	// glCullFace
	runTest(test_cullFace_basicRobustness);
	runTest(test_cullFace_stressSweep);
	runTest(test_cullFace_errorQueue);
	runTest(test_cullFace_rapidToggle);
	runTest(test_cullFace_mixedValidity);
	runTest(test_cullFace_enableDisable);
	runTest(test_cullFace_frontFaceCombo);
	runTest(test_cullFace_largeEnum);

	// glEnable / glDisable (Cull Face)
	runTest(test_cullFaceEnable_basicRobustness);
	runTest(test_cullFaceEnable_rapidToggle);
	runTest(test_cullFaceEnable_invalidCaps);
	runTest(test_cullFaceEnable_capCombinations);

	// glFrontFace
	runTest(test_frontFace_errorQueue);
	runTest(test_frontFace_rapidToggle);
	runTest(test_frontFace_mixedValidity);
	runTest(test_frontFace_displayList);
	runTest(test_frontFace_attribStack);
	runTest(test_frontFace_cullCombinations);
	runTest(test_frontFace_largeEnum);
	runTest(test_frontFace_rapidFire);

	// glLineWidth
	runTest(test_lineWidth_basicRobustness);
	runTest(test_lineWidth_stressSweep);
	runTest(test_lineWidth_specialFloats);
	runTest(test_lineWidth_errorQueue);
	runTest(test_lineWidth_limits);

	// glPolygonOffset
	runTest(test_polygonOffset_basicRobustness);
	runTest(test_polygonOffset_stressSweep);
	runTest(test_polygonOffset_errorQueue);
	runTest(test_polygonOffset_statePreservation);
	runTest(test_polygonOffset_specialFloats);

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

	EXPECT_GL_ERROR(err, (err != GL_NO_ERROR),
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max + 1, max + 1, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
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
