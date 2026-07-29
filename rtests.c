#include "include/helper.h"
#include "test_utility.h"

/* ============================================================
 * Forward declarations — Gizem / BufferObjects
 * ============================================================ */
extern void GS_GL20SC_BO_BB_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_BO_BB_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_BO_BB_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_BO_BD_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_BO_BD_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_BO_BD_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_BO_GB_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_BO_GB_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_BO_GB_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Gizem / FramebufferObjects
 * ============================================================ */
extern void GS_GL20SC_FO_BF_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FO_BF_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FO_BF_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FO_GF_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FO_GF_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FO_GF_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Ozan / ErrorsandStatusReset
 * ============================================================ */
extern void GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Ozan / PixelRectangles
 * ============================================================ */
extern void GS_GL20SC_PR_PS_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_PR_PS_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_PR_PS_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Ozan / Rasterization
 * ============================================================ */
extern void GS_GL20SC_R_CF_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_R_CF_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_R_CF_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_R_FF_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_R_FF_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_R_FF_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_R_LW_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_R_LW_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_R_LW_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_R_PO_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_R_PO_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_R_PO_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Ozan / SpecialFunctions
 * ============================================================ */
extern void GS_GL20SC_SF_FI_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SF_FI_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SF_FI_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SF_FL_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SF_FL_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SF_FL_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Ozan / ViewportandClipping
 * ============================================================ */
extern void GS_GL20SC_VC_DR_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VC_DR_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VC_DR_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VC_V_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VC_V_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VC_V_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Hasan / PerFragmentOperations
 * ============================================================ */
extern void GS_GL20SC_PFO_S_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_PFO_S_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_PFO_S_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Hasan / ReadingPixels
 * ============================================================ */
extern void GS_GL20SC_RP_RNP_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_RP_RNP_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_RP_RNP_ROBUSTNESS_TP_001_close(void);

/* ============================================================
 * Forward declarations — Hasan / Texturing
 * ============================================================ */
extern void GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_TEXT_GM_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_TEXT_GM_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_TEXT_GM_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_TEXT_GT_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_TEXT_GT_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_TEXT_GT_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_TEXT_TSI_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_TEXT_TSI_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_TEXT_TSI_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001_close(void);

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

static void runTest(void (*init_func)(), void (*draw_func)(),
		    void (*close_func)(), const char *name) {
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
		init_func();
		draw_func();
		close_func();

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
#define runTest(tp) runTest(tp##_init, tp##_draw, tp##_close, #tp)

/******************************************/
/*** Category-specific Helper Functions ***/
/******************************************/
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

int checkFloatArray2(const char *test_case, const char *test_procedure,
		     GLenum pname, GLfloat expectedA, GLfloat expectedB,
		     GLfloat tolerance) {
	GLfloat value[2];

	glGetFloatv(pname, value);

	if (fabsf(value[0] - expectedA) > tolerance ||
	    fabsf(value[1] - expectedB) > tolerance) {
		TEST_LOG_FAIL(
		    test_case, test_procedure,
		    "State bozuldu. Beklenen: (%.6f,%.6f) Gercek: (%.6f,%.6f)",
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

void init() {}

void draw() {
	runTest(GS_GL20SC_FOP_CC_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_CD_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_CM_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_C_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_SM_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_CP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GUFV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GUL_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GVAFV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_PB_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U1FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U1F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U1IV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U1I_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U2F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U2IV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U2I_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U3FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U3IV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U4FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U4F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U4IV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U4I_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_UM3FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_UM4FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_UP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_VAPV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DA_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DE_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DRE_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DVAA_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_EVAA_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA1F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA2FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA2F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA3FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA3F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA4FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_CC_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_CD_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_CM_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_C_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_SM_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DA_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DE_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DRE_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_DVAA_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_EVAA_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VA1F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_CP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GUFV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GUL_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_GVAFV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_PB_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_U1F_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_UM4FV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_UP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SP_VAPV_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_BO_BB_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_BO_BD_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_BO_BSD_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_BO_GB_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_BO_GBP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FO_BF_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_FO_GF_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_ESR_GE_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_PR_PS_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_R_CF_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_R_FF_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_R_LW_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_R_PO_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SF_FI_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_SF_FL_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VC_DR_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_VC_V_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_PFO_S_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_RP_RNP_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_TEXT_BT_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_TEXT_GM_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_TEXT_GT_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_TEXT_TSI_ROBUSTNESS_TP_001);
	runTest(GS_GL20SC_TEXT_TS_ROBUSTNESS_TP_001);
}

void cleanup() { printf("cleanup\n"); }
