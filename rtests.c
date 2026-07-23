#include "include/rtests.h"
#include "include/helper.h"

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
int createContext(GLFWwindow **window) {
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
void destroyContext(GLFWwindow **window) {
	glfwDestroyWindow(*window);
	glfwTerminate();
}

/**************************************/
/********** Helper Functions **********/
/**************************************/

void cleanOpenGLState() {
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	while (glGetError() != GL_NO_ERROR)
		;
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
		fprintf(stderr, "\x1b[31m[ERROR]\x1b[0m Fork failed for %s\n",
			name);
		return;
	}

	if (pid == 0) {
		GLFWwindow *w;
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
				printf("\x1b[32m[PASS]\x1b[0m %s passed.\n",
				       name);
			}
		} else if (WIFSIGNALED(status)) {
			int sig = WTERMSIG(status);
			fprintf(stderr,
				"\x1b[31m[CRASH]\x1b[0m %s killed by signal "
				"%d! (Driver Vulnerability / Memory Corruption "
				"caught)\n",
				name, sig);
		}
	}
	retcode = 0;
}
#define runTest(func) runTest(func, #func)

/******************************************/
/*** Category-specific Helper Functions ***/
/******************************************/
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>

/* ============================================================
 * Hata kuyrugu
 * ============================================================ */

void clearGLErrors(void) {
	while (glGetError() != GL_NO_ERROR) {
	}
}

/* ============================================================
 * Durum kontrolleri
 * ============================================================ */

int checkViewport(const char *test_case, const char *test_procedure, GLint x,
		  GLint y, GLsizei width, GLsizei height) {
	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);

	if (viewport[0] != x || viewport[1] != y || viewport[2] != width ||
	    viewport[3] != height) {
		TEST_LOG_FAIL(test_case, test_procedure,
			      "Viewport durumu bozuldu. Beklenen: "
			      "(%d,%d,%d,%d) Gercek: (%d,%d,%d,%d)",
			      x, y, width, height, viewport[0], viewport[1],
			      viewport[2], viewport[3]);
		return 0;
	}

	return 1;
}

int checkIntState(const char *test_case, const char *test_procedure,
		  GLenum pname, GLint expected) {
	GLint actual;

	glGetIntegerv(pname, &actual);

	if (actual != expected) {
		TEST_LOG_FAIL(
		    test_case, test_procedure,
		    "State bozuldu. pname=0x%X Beklenen: 0x%X Gercek: 0x%X",
		    pname, expected, actual);
		return 0;
	}

	return 1;
}

int checkFloatState(const char *test_case, const char *test_procedure,
		    GLenum pname, GLfloat expected, GLfloat tolerance) {
	GLfloat actual;

	glGetFloatv(pname, &actual);

	if (fabsf(actual - expected) > tolerance) {
		TEST_LOG_FAIL(
		    test_case, test_procedure,
		    "State bozuldu. pname=0x%X Beklenen: %.3f Gercek: %.3f",
		    pname, expected, actual);
		return 0;
	}

	return 1;
}

int checkFloatState2(const char *test_case, const char *test_procedure,
		     GLenum pnameA, GLfloat expectedA, GLenum pnameB,
		     GLfloat expectedB, GLfloat tolerance) {
	GLfloat actualA;
	GLfloat actualB;

	glGetFloatv(pnameA, &actualA);
	glGetFloatv(pnameB, &actualB);

	if (fabsf(actualA - expectedA) > tolerance ||
	    fabsf(actualB - expectedB) > tolerance) {
		TEST_LOG_FAIL(test_case, test_procedure,
			      "State bozuldu. Beklenen: (%.3f, %.3f) Gercek: "
			      "(%.3f, %.3f)",
			      expectedA, expectedB, actualA, actualB);
		return 0;
	}

	return 1;
}

int checkDoubleState2(const char *test_case, const char *test_procedure,
		      GLenum pname, GLdouble expectedA, GLdouble expectedB,
		      GLdouble tolerance) {
	GLdouble value[2];

	glGetDoublev(pname, value);

	if (fabs(value[0] - expectedA) > tolerance ||
	    fabs(value[1] - expectedB) > tolerance) {
		TEST_LOG_FAIL(
		    test_case, test_procedure,
		    "State bozuldu. Beklenen: (%lf,%lf) Gercek: (%lf,%lf)",
		    expectedA, expectedB, value[0], value[1]);
		return 0;
	}

	return 1;
}

/* ============================================================
 * Durum sifirlama
 * ============================================================ */

void resetState_Viewport(void) {
	glViewport(0, 0, 640, 480);
	clearGLErrors();
}

void resetState_DepthRange(void) {
	glDepthRange(0.0, 1.0);
	clearGLErrors();
}

void resetState_LineWidth(void) {
	glLineWidth(1.0f);
	clearGLErrors();
}

void resetState_FrontFace(void) {
	glFrontFace(GL_CCW);
	clearGLErrors();
}

void resetState_CullFace(void) {
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	clearGLErrors();
}

void resetState_PolygonO(void) {
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_DEPTH_TEST);
	glPolygonOffset(0.0f, 0.0f);
	clearGLErrors();
}

void resetState_PixelStorei(void) {
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	clearGLErrors();
}

/* ============================================================
 * Yardimci
 * ============================================================ */

GLint randInt32(void) {
	unsigned int value;

	value = ((unsigned int)rand() & 0x7FFFu);
	value = (value << 15) | ((unsigned int)rand() & 0x7FFFu);
	value = (value << 2) | ((unsigned int)rand() & 0x3u);

	return (GLint)value;
}

/***************************************/
/****** Robustness Test Functions ******/
/***************************************/

/***********************************/
/***** Core Workflow Functions *****/
/***********************************/

void init() {
	printf("init\n");
	/* --------------- ViewportandClipping - Viewport --------------- */
	ViewportandClipping_Viewport_TC_001();
	ViewportandClipping_Viewport_TC_002();
	ViewportandClipping_Viewport_TC_003();
	ViewportandClipping_Viewport_TC_004();
	ViewportandClipping_Viewport_TC_005();
	ViewportandClipping_Viewport_TC_006();

	/* --------------- ViewportandClipping - DepthRangef --------------- */
	ViewportandClipping_DepthRangef_TC_001();
	ViewportandClipping_DepthRangef_TC_002();
	ViewportandClipping_DepthRangef_TC_003();
	ViewportandClipping_DepthRangef_TC_004();
	ViewportandClipping_DepthRangef_TC_005();
	ViewportandClipping_DepthRangef_TC_006();

	/* --------------- Rasterization - CullFace --------------- */
	Rasterizaton_CullFace_TC_001();
	Rasterizaton_CullFace_TC_002();
	Rasterizaton_CullFace_TC_003();
	Rasterizaton_CullFace_TC_004();
	Rasterizaton_CullFace_TC_005();
	Rasterizaton_CullFace_TC_006();
	Rasterizaton_CullFace_TC_007();
	Rasterizaton_CullFace_TC_008();
	Rasterizaton_CullFace_TC_009();

	/* --------------- Rasterization - FrontFace --------------- */
	Rasterizaton_FrontFace_TC_001();
	Rasterizaton_FrontFace_TC_002();
	Rasterizaton_FrontFace_TC_003();
	Rasterizaton_FrontFace_TC_004();
	Rasterizaton_FrontFace_TC_005();
	Rasterizaton_FrontFace_TC_006();
	Rasterizaton_FrontFace_TC_007();
	Rasterizaton_FrontFace_TC_008();

	/* --------------- Rasterization - LineWidth --------------- */
	Rasterizaton_LineWidth_TC_001();
	Rasterizaton_LineWidth_TC_002();
	Rasterizaton_LineWidth_TC_003();
	Rasterizaton_LineWidth_TC_004();
	Rasterizaton_LineWidth_TC_005();
	Rasterizaton_LineWidth_TC_006();
	Rasterizaton_LineWidth_TC_007();
	Rasterizaton_LineWidth_TC_008();

	/* --------------- Rasterization - PolygonOffset --------------- */
	Rasterizaton_PolygonOffset_TC_001();
	Rasterizaton_PolygonOffset_TC_002();
	Rasterizaton_PolygonOffset_TC_003();
	Rasterizaton_PolygonOffset_TC_004();
	Rasterizaton_PolygonOffset_TC_005();
	Rasterizaton_PolygonOffset_TC_006();
	Rasterizaton_PolygonOffset_TC_007();

	/* --------------- PixelRectangles - PixelStorei --------------- */
	PixelRectangles_PixelStorei_TC_001();
	PixelRectangles_PixelStorei_TC_002();
	PixelRectangles_PixelStorei_TC_003();
	PixelRectangles_PixelStorei_TC_004();
	PixelRectangles_PixelStorei_TC_005();
	PixelRectangles_PixelStorei_TC_006();

	/* --------------- SpecialFunctions - Flush --------------- */
	SpecialFunctions_Flush_TC_001();
	SpecialFunctions_Flush_TC_002();
	SpecialFunctions_Flush_TC_003();
	SpecialFunctions_Flush_TC_004();
	SpecialFunctions_Flush_TC_005();
	SpecialFunctions_Flush_TC_006();
	SpecialFunctions_Flush_TC_007();

	/* --------------- SpecialFunctions - Finish --------------- */
	SpecialFunctions_Finish_TC_001();
	SpecialFunctions_Finish_TC_002();
	SpecialFunctions_Finish_TC_003();
	SpecialFunctions_Finish_TC_004();
	SpecialFunctions_Finish_TC_005();
	SpecialFunctions_Finish_TC_006();
	SpecialFunctions_Finish_TC_007();

	/* --------------- ErrorsandStatusReset - GetError --------------- */
	ErrorsandStatusReset_GetError_TC_001();
	ErrorsandStatusReset_GetError_TC_002();
	ErrorsandStatusReset_GetError_TC_003();
	ErrorsandStatusReset_GetError_TC_004();
	ErrorsandStatusReset_GetError_TC_005();
	ErrorsandStatusReset_GetError_TC_006();
	ErrorsandStatusReset_GetError_TC_007();
}

void draw() {
	runTest(BufferObjects_BindBuffer_TC_002);
	runTest(BufferObjects_BindBuffer_TC_003);
	runTest(BufferObjects_BindBuffer_TC_004);
	runTest(BufferObjects_BindBuffer_TC_005);
	runTest(BufferObjects_BindBuffer_TC_006);
	runTest(BufferObjects_BindBuffer_TC_007);
	runTest(BufferObjects_BindBuffer_TC_008);
	runTest(BufferObjects_BindBuffer_TC_009);
	runTest(BufferObjects_BindBuffer_TC_010);
	runTest(BufferObjects_BindBuffer_TC_011);
	runTest(BufferObjects_BufferData_TC_001);
	runTest(BufferObjects_BufferData_TC_002);
	runTest(BufferObjects_BufferData_TC_003);
	runTest(BufferObjects_BufferData_TC_004);
	runTest(BufferObjects_BufferData_TC_005);
	runTest(BufferObjects_BufferData_TC_006);
	runTest(BufferObjects_BufferData_TC_007);
	runTest(BufferObjects_BufferData_TC_008);
	runTest(BufferObjects_BufferData_TC_009);
	runTest(BufferObjects_BufferData_TC_010);
	runTest(BufferObjects_BufferData_TC_011);
	runTest(BufferObjects_BufferData_TC_012);
	runTest(BufferObjects_BufferData_TC_013);
	runTest(BufferObjects_BufferData_TC_014);
	runTest(BufferObjects_BufferSubData_TC_001);
	runTest(BufferObjects_BufferSubData_TC_002);
	runTest(BufferObjects_BufferSubData_TC_003);
	runTest(BufferObjects_BufferSubData_TC_004);
	runTest(BufferObjects_BufferSubData_TC_005);
	runTest(BufferObjects_BufferSubData_TC_006);
	runTest(BufferObjects_BufferSubData_TC_007);
	runTest(BufferObjects_BufferSubData_TC_008);
	runTest(BufferObjects_BufferSubData_TC_009);
	runTest(BufferObjects_BufferSubData_TC_010);
	runTest(BufferObjects_BufferSubData_TC_011);
	runTest(BufferObjects_BufferSubData_TC_012);
	runTest(BufferObjects_BufferSubData_TC_013);
	runTest(BufferObjects_BufferSubData_TC_014);
	runTest(BufferObjects_GenBuffers_TC_001);
	runTest(BufferObjects_GenBuffers_TC_002);
	runTest(BufferObjects_GenBuffers_TC_003);
	runTest(BufferObjects_GenBuffers_TC_004);
	runTest(BufferObjects_GenBuffers_TC_005);
	runTest(BufferObjects_GenBuffers_TC_006);
	runTest(BufferObjects_GenBuffers_TC_007);
	runTest(BufferObjects_GenBuffers_TC_008);
	runTest(BufferObjects_GenBuffers_TC_009);
	runTest(BufferObjects_GetBufferParameteriv_TC_001);
	runTest(BufferObjects_GetBufferParameteriv_TC_002);
	runTest(BufferObjects_GetBufferParameteriv_TC_003);
	runTest(BufferObjects_GetBufferParameteriv_TC_004);
	runTest(BufferObjects_GetBufferParameteriv_TC_005);
	runTest(BufferObjects_GetBufferParameteriv_TC_006);
	runTest(BufferObjects_GetBufferParameteriv_TC_007);
	runTest(BufferObjects_GetBufferParameteriv_TC_008);
	runTest(BufferObjects_GetBufferParameteriv_TC_009);
	runTest(BufferObjects_GetBufferParameteriv_TC_010);
	runTest(BufferObjects_GetBufferParameteriv_TC_011);
	runTest(BufferObjects_GetBufferParameteriv_TC_012);
	runTest(BufferObjects_GetBufferParameteriv_TC_013);
	runTest(BufferObjects_GetBufferParameteriv_TC_014);
	runTest(BufferObjects_GetBufferParameteriv_TC_015);
	runTest(BufferObjects_GetBufferParameteriv_TC_016);
	runTest(FramebufferObjects_BindFramebuffer_TC_001);
	runTest(FramebufferObjects_BindFramebuffer_TC_002);
	runTest(FramebufferObjects_BindFramebuffer_TC_003);
	runTest(FramebufferObjects_BindFramebuffer_TC_004);
	runTest(FramebufferObjects_BindFramebuffer_TC_005);
	runTest(FramebufferObjects_BindFramebuffer_TC_006);
	runTest(FramebufferObjects_BindFramebuffer_TC_007);
	runTest(FramebufferObjects_BindFramebuffer_TC_008);
	runTest(FramebufferObjects_BindFramebuffer_TC_009);
	runTest(FramebufferObjects_BindFramebuffer_TC_010);
	runTest(FramebufferObjects_BindFramebuffer_TC_011);
	runTest(FramebufferObjects_BindFramebuffer_TC_012);
	runTest(FramebufferObjects_GenFramebuffers_TC_001);
	runTest(FramebufferObjects_GenFramebuffers_TC_002);
	runTest(FramebufferObjects_GenFramebuffers_TC_003);
	runTest(FramebufferObjects_GenFramebuffers_TC_004);
	runTest(FramebufferObjects_GenFramebuffers_TC_005);
	runTest(FramebufferObjects_GenFramebuffers_TC_006);
	runTest(FramebufferObjects_GenFramebuffers_TC_007);
	runTest(FramebufferObjects_GenFramebuffers_TC_008);
	runTest(FramebufferObjects_GenFramebuffers_TC_009);
	runTest(FramebufferObjects_GenFramebuffers_TC_010);
	runTest(FramebufferObjects_GenFramebuffers_TC_011);
	runTest(FramebufferObjects_GenFramebuffers_TC_012);
	runTest(FramebufferObjects_GenFramebuffers_TC_013);
	runTest(FramebufferObjects_GenFramebuffers_TC_014);
	runTest(FramebufferObjects_GenFramebuffers_TC_015);
	runTest(PerFragmentOperations_Scissor_TC_001);
	runTest(PerFragmentOperations_Scissor_TC_002);
	runTest(PerFragmentOperations_Scissor_TC_003);
	runTest(PerFragmentOperations_Scissor_TC_004);
	runTest(PerFragmentOperations_Scissor_TC_005);
	runTest(PerFragmentOperations_Scissor_TC_006);
	runTest(PixelRectangles_PixelStorei_TC_001);
	runTest(PixelRectangles_PixelStorei_TC_002);
	runTest(PixelRectangles_PixelStorei_TC_003);
	runTest(PixelRectangles_PixelStorei_TC_004);
	runTest(PixelRectangles_PixelStorei_TC_005);
	runTest(PixelRectangles_PixelStorei_TC_006);
	runTest(Rasterizaton_CullFace_TC_001);
	runTest(Rasterizaton_CullFace_TC_002);
	runTest(Rasterizaton_CullFace_TC_003);
	runTest(Rasterizaton_CullFace_TC_004);
	runTest(Rasterizaton_CullFace_TC_005);
	runTest(Rasterizaton_CullFace_TC_006);
	runTest(Rasterizaton_CullFace_TC_007);
	runTest(Rasterizaton_CullFace_TC_008);
	runTest(Rasterizaton_CullFace_TC_009);
	runTest(Rasterizaton_FrontFace_TC_001);
	runTest(Rasterizaton_FrontFace_TC_002);
	runTest(Rasterizaton_FrontFace_TC_003);
	runTest(Rasterizaton_FrontFace_TC_004);
	runTest(Rasterizaton_FrontFace_TC_005);
	runTest(Rasterizaton_FrontFace_TC_006);
	runTest(Rasterizaton_FrontFace_TC_007);
	runTest(Rasterizaton_FrontFace_TC_008);
	runTest(Rasterizaton_LineWidth_TC_001);
	runTest(Rasterizaton_LineWidth_TC_002);
	runTest(Rasterizaton_LineWidth_TC_003);
	runTest(Rasterizaton_LineWidth_TC_004);
	runTest(Rasterizaton_LineWidth_TC_005);
	runTest(Rasterizaton_LineWidth_TC_006);
	runTest(Rasterizaton_LineWidth_TC_007);
	runTest(Rasterizaton_LineWidth_TC_008);
	runTest(Rasterizaton_PolygonOffset_TC_001);
	runTest(Rasterizaton_PolygonOffset_TC_002);
	runTest(Rasterizaton_PolygonOffset_TC_003);
	runTest(Rasterizaton_PolygonOffset_TC_004);
	runTest(Rasterizaton_PolygonOffset_TC_005);
	runTest(Rasterizaton_PolygonOffset_TC_006);
	runTest(Rasterizaton_PolygonOffset_TC_007);
	runTest(ReadingPixels_ReadnPixels_TC_001);
	runTest(ReadingPixels_ReadnPixels_TC_002);
	runTest(ReadingPixels_ReadnPixels_TC_003);
	runTest(ReadingPixels_ReadnPixels_TC_004);
	runTest(ShaderQueries_GetProgram_TC_001);
	runTest(ShaderQueries_GetProgram_TC_002);
	runTest(ShaderQueries_GetProgram_TC_003);
	runTest(ShaderQueries_GetProgram_TC_004);
	runTest(ShaderQueries_GetVertexAttribPointer_TC_001);
	runTest(ShaderQueries_GetVertexAttribPointer_TC_002);
	runTest(ShaderQueries_GetVertexAttribPointer_TC_003);
	runTest(ShaderQueries_GetVertexAttribPointer_TC_004);
	runTest(ShaderQueries_GetVertexAttrib_TC_001);
	runTest(ShaderQueries_GetVertexAttrib_TC_002);
	runTest(ShaderQueries_GetVertexAttrib_TC_003);
	runTest(ShaderQueries_GetVertexAttrib_TC_004);
	runTest(ShaderQueries_GetnUniform_TC_001);
	runTest(ShaderQueries_GetnUniform_TC_002);
	runTest(ShaderQueries_GetnUniform_TC_003);
	runTest(ShaderQueries_GetnUniform_TC_004);
	runTest(ShadersAndPrograms_CreateProgram_TC_001);
	runTest(ShadersAndPrograms_CreateProgram_TC_002);
	runTest(ShadersAndPrograms_CreateProgram_TC_003);
	runTest(ShadersAndPrograms_CreateProgram_TC_004);
	runTest(ShadersAndPrograms_GetAttribLocation_TC_001);
	runTest(ShadersAndPrograms_GetAttribLocation_TC_002);
	runTest(ShadersAndPrograms_GetAttribLocation_TC_003);
	runTest(ShadersAndPrograms_GetAttribLocation_TC_004);
	runTest(ShadersAndPrograms_GetUniformLocation_TC_001);
	runTest(ShadersAndPrograms_GetUniformLocation_TC_002);
	runTest(ShadersAndPrograms_GetUniformLocation_TC_003);
	runTest(ShadersAndPrograms_GetUniformLocation_TC_004);
	runTest(ShadersAndPrograms_ProgramBinary_TC_001);
	runTest(ShadersAndPrograms_ProgramBinary_TC_002);
	runTest(ShadersAndPrograms_ProgramBinary_TC_003);
	runTest(ShadersAndPrograms_ProgramBinary_TC_004);
	runTest(ShadersAndPrograms_UniformMatrix_TC_001);
	runTest(ShadersAndPrograms_UniformMatrix_TC_002);
	runTest(ShadersAndPrograms_UniformMatrix_TC_003);
	runTest(ShadersAndPrograms_UniformMatrix_TC_004);
	runTest(ShadersAndPrograms_Uniform_TC_001);
	runTest(ShadersAndPrograms_Uniform_TC_002);
	runTest(ShadersAndPrograms_Uniform_TC_003);
	runTest(ShadersAndPrograms_Uniform_TC_004);
	runTest(ShadersAndPrograms_Uniform_TC_005);
	runTest(ShadersAndPrograms_Uniform_TC_006);
	runTest(ShadersAndPrograms_UseProgram_TC_001);
	runTest(ShadersAndPrograms_UseProgram_TC_002);
	runTest(ShadersAndPrograms_UseProgram_TC_003);
	runTest(ShadersAndPrograms_UseProgram_TC_004);
	runTest(SpecialFunctions_Finish_TC_001);
	runTest(SpecialFunctions_Finish_TC_002);
	runTest(SpecialFunctions_Finish_TC_003);
	runTest(SpecialFunctions_Finish_TC_004);
	runTest(SpecialFunctions_Finish_TC_005);
	runTest(SpecialFunctions_Finish_TC_006);
	runTest(SpecialFunctions_Finish_TC_007);
	runTest(Texturing_BindTexture_TC_001);
	runTest(Texturing_BindTexture_TC_002);
	runTest(Texturing_BindTexture_TC_003);
	runTest(Texturing_BindTexture_TC_004);
	runTest(Texturing_BindTexture_TC_005);
	runTest(Texturing_BindTexture_TC_006);
	runTest(Texturing_BindTexture_TC_007);
	runTest(Texturing_BindTexture_TC_008);
	runTest(Texturing_GenTextures_TC_001);
	runTest(Texturing_GenTextures_TC_002);
	runTest(Texturing_GenTextures_TC_003);
	runTest(Texturing_GenTextures_TC_004);
	runTest(Texturing_GenTextures_TC_005);
	runTest(Texturing_GenTextures_TC_006);
	runTest(Texturing_GenTextures_TC_007);
	runTest(Texturing_GenTextures_TC_008);
	runTest(Texturing_GenTextures_TC_009);
	runTest(Texturing_GenTextures_TC_010);
	runTest(Texturing_GenTextures_TC_011);
	runTest(Texturing_GenTextures_TC_012);
	runTest(Texturing_GenerateMipmap_TC_001);
	runTest(Texturing_GenerateMipmap_TC_002);
	runTest(Texturing_GenerateMipmap_TC_003);
	runTest(Texturing_GenerateMipmap_TC_004);
	runTest(Texturing_GenerateMipmap_TC_005);
	runTest(Texturing_GenerateMipmap_TC_006);
	runTest(Texturing_GenerateMipmap_TC_007);
	runTest(Texturing_GenerateMipmap_TC_008);
	runTest(Texturing_GenerateMipmap_TC_009);
	runTest(Texturing_GenerateMipmap_TC_010);
	runTest(Texturing_GenerateMipmap_TC_011);
	runTest(Texturing_GenerateMipmap_TC_012);
	runTest(Texturing_GenerateMipmap_TC_013);
	runTest(Texturing_GenerateMipmap_TC_014);
	runTest(Texturing_GenerateMipmap_TC_015);
	runTest(Texturing_TexStorage2D_TC_001);
	runTest(Texturing_TexStorage2D_TC_002);
	runTest(Texturing_TexStorage2D_TC_003);
	runTest(Texturing_TexStorage2D_TC_004);
	runTest(Texturing_TexStorage2D_TC_005);
	runTest(Texturing_TexSubImage2D_TC_001);
	runTest(Texturing_TexSubImage2D_TC_002);
	runTest(Texturing_TexSubImage2D_TC_003);
	runTest(Texturing_TexSubImage2D_TC_004);
	runTest(Texturing_TexSubImage2D_TC_005);
	runTest(Vertices_DisableVertexAttribArray_TC_001);
	runTest(Vertices_DisableVertexAttribArray_TC_002);
	runTest(Vertices_DisableVertexAttribArray_TC_003);
	runTest(Vertices_DisableVertexAttribArray_TC_004);
	runTest(Vertices_DrawArrays_TC_001);
	runTest(Vertices_DrawArrays_TC_002);
	runTest(Vertices_DrawArrays_TC_003);
	runTest(Vertices_DrawArrays_TC_004);
	runTest(Vertices_DrawArrays_TC_005);
	runTest(Vertices_DrawArrays_TC_006);
	runTest(Vertices_DrawElements_TC_001);
	runTest(Vertices_DrawElements_TC_002);
	runTest(Vertices_DrawElements_TC_003);
	runTest(Vertices_DrawElements_TC_004);
	runTest(Vertices_DrawRangeElements_TC_001);
	runTest(Vertices_DrawRangeElements_TC_002);
	runTest(Vertices_DrawRangeElements_TC_003);
	runTest(Vertices_DrawRangeElements_TC_004);
	runTest(Vertices_EnableVertexAttribArray_TC_001);
	runTest(Vertices_EnableVertexAttribArray_TC_002);
	runTest(Vertices_EnableVertexAttribArray_TC_003);
	runTest(Vertices_EnableVertexAttribArray_TC_004);
	runTest(Vertices_VertexAttribPointer_TC_001);
	runTest(Vertices_VertexAttribPointer_TC_002);
	runTest(Vertices_VertexAttribPointer_TC_003);
	runTest(Vertices_VertexAttribPointer_TC_004);
	runTest(Vertices_VertexAttribPointer_TC_005);
	runTest(Vertices_VertexAttrib_TC_001);
	runTest(Vertices_VertexAttrib_TC_002);
	runTest(Vertices_VertexAttrib_TC_003);
	runTest(Vertices_VertexAttrib_TC_004);
	runTest(ViewportandClipping_DepthRangef_TC_001);
	runTest(ViewportandClipping_DepthRangef_TC_002);
	runTest(ViewportandClipping_DepthRangef_TC_003);
	runTest(ViewportandClipping_DepthRangef_TC_004);
	runTest(ViewportandClipping_DepthRangef_TC_005);
	runTest(ViewportandClipping_DepthRangef_TC_006);
	runTest(ViewportandClipping_Viewport_TC_001);
	runTest(ViewportandClipping_Viewport_TC_002);
	runTest(ViewportandClipping_Viewport_TC_003);
	runTest(ViewportandClipping_Viewport_TC_004);
	runTest(ViewportandClipping_Viewport_TC_005);
	runTest(ViewportandClipping_Viewport_TC_006);
	runTest(WholeFramebufferOperations_ClearColor_TC_001);
	runTest(WholeFramebufferOperations_ClearColor_TC_002);
	runTest(WholeFramebufferOperations_ClearColor_TC_003);
	runTest(WholeFramebufferOperations_ClearColor_TC_004);
	runTest(WholeFramebufferOperations_ClearColor_TC_005);
	runTest(WholeFramebufferOperations_ClearDepthf_TC_001);
	runTest(WholeFramebufferOperations_ClearDepthf_TC_002);
	runTest(WholeFramebufferOperations_ClearDepthf_TC_003);
	runTest(WholeFramebufferOperations_ClearDepthf_TC_004);
	runTest(WholeFramebufferOperations_ClearDepthf_TC_005);
	runTest(WholeFramebufferOperations_ClearStencil_TC_001);
	runTest(WholeFramebufferOperations_ClearStencil_TC_002);
	runTest(WholeFramebufferOperations_ClearStencil_TC_003);
	runTest(WholeFramebufferOperations_ClearStencil_TC_004);
	runTest(WholeFramebufferOperations_ClearStencil_TC_005);
	runTest(WholeFramebufferOperations_Clear_TC_001);
	runTest(WholeFramebufferOperations_Clear_TC_002);
	runTest(WholeFramebufferOperations_Clear_TC_003);
	runTest(WholeFramebufferOperations_Clear_TC_004);
	runTest(WholeFramebufferOperations_Clear_TC_005);
	runTest(WholeFramebufferOperations_ColorMask_TC_001);
	runTest(WholeFramebufferOperations_ColorMask_TC_002);
	runTest(WholeFramebufferOperations_ColorMask_TC_003);
	runTest(WholeFramebufferOperations_ColorMask_TC_004);
	runTest(WholeFramebufferOperations_ColorMask_TC_005);
	runTest(WholeFramebufferOperations_DepthMask_TC_001);
	runTest(WholeFramebufferOperations_DepthMask_TC_002);
	runTest(WholeFramebufferOperations_DepthMask_TC_003);
	runTest(WholeFramebufferOperations_DepthMask_TC_004);
	runTest(WholeFramebufferOperations_StencilMaskSeparate_TC_001);
	runTest(WholeFramebufferOperations_StencilMaskSeparate_TC_002);
	runTest(WholeFramebufferOperations_StencilMaskSeparate_TC_003);
	runTest(WholeFramebufferOperations_StencilMaskSeparate_TC_004);
	runTest(WholeFramebufferOperations_StencilMaskSeparate_TC_005);
	runTest(WholeFramebufferOperations_StencilMask_TC_001);
	runTest(WholeFramebufferOperations_StencilMask_TC_002);
	runTest(WholeFramebufferOperations_StencilMask_TC_003);
	runTest(WholeFramebufferOperations_StencilMask_TC_004);
	runTest(SpecialFunctions_Flush_TC_001);
	runTest(SpecialFunctions_Flush_TC_002);
	runTest(SpecialFunctions_Flush_TC_003);
	runTest(SpecialFunctions_Flush_TC_004);
	runTest(SpecialFunctions_Flush_TC_005);
	runTest(SpecialFunctions_Flush_TC_006);
	runTest(SpecialFunctions_Flush_TC_007);
	runTest(ErrorsandStatusReset_GetError_TC_001);
	runTest(ErrorsandStatusReset_GetError_TC_002);
	runTest(ErrorsandStatusReset_GetError_TC_003);
	runTest(ErrorsandStatusReset_GetError_TC_004);
	runTest(ErrorsandStatusReset_GetError_TC_005);
	runTest(ErrorsandStatusReset_GetError_TC_006);
	runTest(ErrorsandStatusReset_GetError_TC_007);
}

void cleanup() { printf("cleanup\n"); }
