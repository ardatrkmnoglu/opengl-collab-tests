#ifndef RTESTS_H
#define RTESTS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

extern int retcode;

#define EXPECT_GL_ERROR(actual_err, condition_expr, msg)                       \
	do {                                                                   \
		if (!(condition_expr)) {                                       \
			fprintf(stderr,                                        \
				"\x1b[33m[FAIL]\x1b[0m %s (Line: %d)\n",       \
				msg, __LINE__);                                \
			fprintf(stderr, "   > Expected: %s\n",                 \
				#condition_expr);                              \
			fprintf(stderr, "   > Actual  : 0x%04X\n",             \
				actual_err);                                   \
			retcode = 1;                                           \
			continue;                                              \
		}                                                              \
	} while (0)

void init(void);
void draw(void);
void cleanup(void);

void rTest_CreateProgram(void);
void rTest_ProgramBinary_unalignedPtr(void);
void rTest_ProgramBinary_memRevoke(void);
void rTest_ProgramBinary_overload(void);
void rTest_UseProgram_invalidID(void);
void rTest_UseProgram_typeConfusion(void);
void rTest_GetAttribLocation_nullPtr(void);
void rTest_GetAttribLocation_reservedVariable(void);
void rTest_DrawArrays_outOfBounds(void);
void rTest_DrawArrays_guardPageAttack(void);
void rTest_GetAttribLocation_nullPtr(void);
void rTest_ProgramBinary_unalignedPtr(void);

#ifdef RUN_EXTESTS
void rTest_invalidEnum(void);
void rTest_invalidValue(void);
void rTest_invalidPrecision(void);
void rTest_errorFlood(void);
void rTest_shaderCompilerError(void);
void rTest_maxTextureLimit(void);
void rTest_missingAttrib(void);
void rTest_NaNVertices(void);
void rTest_outOfMemory(void);
void rTest_stateRecovr(void);
void rTest_drawWOProgram(void);
void rTest_oobDraw(void);
void rTest_nullPtr(void);
#endif

#endif // RTESTS_H
