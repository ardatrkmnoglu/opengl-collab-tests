#ifndef RTESTS_H
#define RTESTS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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

/* --------------- glBufferSubData --------------- */
void rTest_glBufferSubData_invalid_enum_target();
void rTest_glBufferSubData_invalid_value_negative_offset();
void rTest_glBufferSubData_invalid_value_negative_size();
void rTest_glBufferSubData_invalid_value_out_of_bounds();
void rTest_glBufferSubData_invalid_operation_zero_buffer_bound();
void rTest_glBufferSubData_offset_size_overflow_wraparound();
void rTest_glBufferSubData_exact_boundary_offset();
void rTest_glBufferSubData_negative_offset_compensating_size();
void rTest_glBufferSubData_zero_size_null_data();
void rTest_glBufferSubData_target_zero_bound();
void rTest_glBufferSubData_into_zero_sized_store();
void rTest_glBufferSubData_source_smaller_than_size();
void rTest_glBufferSubData_dangling_data_pointer();
void rTest_glBufferSubData_overlapping_misaligned_thrash();

/* --------------- glBufferData --------------- */
void rTest_glBufferData_invalid_enum_target();
void rTest_glBufferData_invalid_enum_usage();
void rTest_glBufferData_invalid_value_negative_size();
void rTest_glBufferData_invalid_operation_zero_buffer_bound();
void rTest_glBufferData_out_of_memory();
void rTest_glBufferData_source_buffer_too_small();
void rTest_glBufferData_zero_size_nonnull_data();
void rTest_glBufferData_size_overflow_boundary();
void rTest_glBufferData_dirty_usage_enum();
void rTest_glBufferData_target_zero_bound();
void rTest_glBufferData_repeated_resize_thrash();
void rTest_glBufferData_misaligned_data_pointer();
void rTest_glBufferData_dangling_data_pointer();
void rTest_glBufferData_state_after_out_of_memory();

/* --------------- glBindBuffer --------------- */
void rTest_glBindBuffer_invalid_enum();
void rTest_glBindBuffer_new_name_without_gen();
void rTest_glBindBuffer_deleted_buffer();
void rTest_glBindBuffer_boundary_handles();
void rTest_glBindBuffer_dirty_high_bits_enum();
void rTest_glBindBuffer_rapid_cross_target_rebind_stress();
void rTest_glBindBuffer_delete_while_double_bound();
void rTest_glBindBuffer_zero_binding_query_thrash();
void rTest_glBindBuffer_massive_namespace_fuzz();
void rTest_glBindBuffer_binding_churn_stress();
void rTest_glBindBuffer_lifecycle_stress();

/* --------------- glGenBuffers --------------- */
void rTest_glGenBuffers_invalid_value();
void rTest_glGenBuffers_zero_count();
void rTest_glGenBuffers_null_buffers();
void rTest_glGenBuffers_large_n();
void rTest_glGenBuffers_repeated_generation();
void rTest_glGenBuffers_unique_names();
void rTest_glGenBuffers_unbound_names_lifecycle();
void rTest_glGenBuffers_double_delete();
void rTest_glGenBuffers_huge_count_small_buffer();

// glCullFace
void test_cullFace_basicRobustness(void);
void test_cullFace_stressSweep(void);
void test_cullFace_errorQueue(void);
void test_cullFace_rapidToggle(void);
void test_cullFace_mixedValidity(void);
void test_cullFace_enableDisable(void);
void test_cullFace_frontFaceCombo(void);
void test_cullFace_largeEnum(void);

// glEnable / glDisable (Cull Face)
void test_cullFaceEnable_basicRobustness(void);
void test_cullFaceEnable_rapidToggle(void);
void test_cullFaceEnable_invalidCaps(void);
void test_cullFaceEnable_capCombinations(void);

// glFrontFace
void test_frontFace_errorQueue(void);
void test_frontFace_rapidToggle(void);
void test_frontFace_mixedValidity(void);
void test_frontFace_displayList(void);
void test_frontFace_attribStack(void);
void test_frontFace_cullCombinations(void);
void test_frontFace_largeEnum(void);
void test_frontFace_rapidFire(void);

// glLineWidth
void test_lineWidth_basicRobustness(void);
void test_lineWidth_stressSweep(void);
void test_lineWidth_specialFloats(void);
void test_lineWidth_errorQueue(void);
void test_lineWidth_limits(void);

// glPolygonOffset
void test_polygonOffset_basicRobustness(void);
void test_polygonOffset_stressSweep(void);
void test_polygonOffset_errorQueue(void);
void test_polygonOffset_statePreservation(void);
void test_polygonOffset_specialFloats(void);

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
