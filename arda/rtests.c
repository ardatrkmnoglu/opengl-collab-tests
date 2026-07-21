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

static void runTest(void (*test_func)(), const char* name) {
	while(glGetError() != GL_NO_ERROR);
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

//	runTest(rTest_DrawArrays_guardPageAttack); // SEGMENTATION FAULT
	runTest(rTest_DrawArrays_outOfBounds);

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

