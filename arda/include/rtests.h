#ifndef RTESTS_H
#define RTESTS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <math.h>
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
void rTest_GetAttribLocation_nullPtr(void);
void rTest_ProgramBinary_unalignedPtr(void);
GLuint create_uniform_dummy_program(void);

// glGetUniformLocation
void rTest_GetUniformLocation_nullPtr(void);
void rTest_GetUniformLocation_reservedPrefix(void);

// glUniform{1234}{if}
void rTest_Uniform_typeConfusion(void);
void rTest_Uniform_invalidLocation(void);

// glUniform{1234}{if}v
void rTest_Uniformv_negativeCount(void);
void rTest_Uniformv_arrayOutOfBounds(void);

// glUniformMatrix{234}fv
void rTest_UniformMatrix_invalidTranspose(void);
void rTest_UniformMatrix_typeMismatch(void);

// glGetVertexAttribfv / iv / Pointerv
void rTest_GetVertexAttrib_invalidEnum(void);
void rTest_GetVertexAttrib_indexOutOfBounds(void);
void rTest_GetVertexAttribPointer_invalidEnum(void);

// glGetnUniformfv / iv (Robustness Extension)
void rTest_GetnUniform_negativeBufSize(void);
void rTest_GetnUniform_invalidProgram(void);

// glGetProgramiv
void rTest_GetProgramiv_invalidEnum(void);
void rTest_GetProgramiv_typeConfusion(void);

// glVertexAttrib
void rTest_VertexAttrib_indexOutOfBounds(void);
void rTest_VertexAttribv_specialFloats(void);

// glVertexAttribPointer
void rTest_VertexAttribPointer_invalidType(void);
void rTest_VertexAttribPointer_invalidSize(void);

// glEnable/DisableVertexAttribArray
void rTest_EnableDisableVertexAttrib_bounds(void);

// glDrawArrays
void rTest_DrawArrays_outOfBounds(void);
void rTest_DrawArrays_guardPageAttack(void);

// Draw Elements / Range Elements
void rTest_DrawElements_invalidType(void);
void rTest_DrawRangeElements_invalidRange(void);

// Framebuffer Mask Operations
void rTest_ColorMask_booleanConversion(void);
void rTest_StencilMaskSeparate_invalidEnum(void);

// Framebuffer Clear Operations
void rTest_Clear_invalidBitmask(void);
void rTest_ClearColor_specialFloats(void);
void rTest_ClearDepthf_clamping(void);
void rTest_ClearStencil_bounds(void);

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
