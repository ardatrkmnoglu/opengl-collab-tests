#ifndef RTESTS_H
#define RTESTS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
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
				(unsigned int)actual_err);                                   \
			retcode = 1;                                           \
			continue;                                              \
		}                                                              \
	} while (0)

void init(void);
void draw(void);
void cleanup(void);

/* --------------- Shaders and Programs --------------- */
void rTest_CreateProgram(void);
void rTest_ProgramBinary_unalignedPtr(void);
void rTest_ProgramBinary_memRevoke(void);
void rTest_ProgramBinary_overload(void);
void rTest_UseProgram_invalidID(void);
void rTest_UseProgram_typeConfusion(void);
void rTest_GetAttribLocation_nullPtr(void);
void rTest_GetAttribLocation_reservedVariable(void);

/* --------------- glGetProgramiv --------------- */
void rTest_GetProgramiv_invalidEnum(void);
void rTest_GetProgramiv_typeConfusion(void);

/* --------------- glGetUniformLocation --------------- */
void rTest_GetUniformLocation_nullPtr(void);
void rTest_GetUniformLocation_reservedPrefix(void);

/* --------------- glUniform{1234}{if} --------------- */
void rTest_Uniform_typeConfusion(void);
void rTest_Uniform_invalidLocation(void);

/* --------------- glUniform{1234}{if}v --------------- */
void rTest_Uniformv_negativeCount(void);
void rTest_Uniformv_arrayOutOfBounds(void);

/* --------------- glUniformMatrix{234}fv --------------- */
void rTest_UniformMatrix_invalidTranspose(void);
void rTest_UniformMatrix_typeMismatch(void);

/* --------------- glGetVertexAttribfv / iv / Pointerv --------------- */
void rTest_GetVertexAttrib_invalidEnum(void);
void rTest_GetVertexAttrib_indexOutOfBounds(void);
void rTest_GetVertexAttribPointer_invalidEnum(void);

/* --------------- glGetnUniformfv / iv (Robustness Extension) --------------- */
void rTest_GetnUniform_negativeBufSize(void);
void rTest_GetnUniform_invalidProgram(void);

/* --------------- glVertexAttrib --------------- */
void rTest_VertexAttrib_indexOutOfBounds(void);
void rTest_VertexAttribv_specialFloats(void);

/* --------------- glVertexAttribPointer --------------- */
void rTest_VertexAttribPointer_invalidType(void);
void rTest_VertexAttribPointer_invalidSize(void);

/* --------------- glEnable/DisableVertexAttribArray --------------- */
void rTest_EnableDisableVertexAttrib_bounds(void);

/* --------------- glDrawArrays --------------- */
void rTest_DrawArrays_outOfBounds(void);
void rTest_DrawArrays_guardPageAttack(void);

/* --------------- glDrawElements / glDrawRangeElements --------------- */
void rTest_DrawElements_invalidType(void);
void rTest_DrawRangeElements_invalidRange(void);

/* --------------- Framebuffer Mask Operations --------------- */
void rTest_ColorMask_booleanConversion(void);
void rTest_StencilMaskSeparate_invalidEnum(void);

/* --------------- Framebuffer Clear Operations --------------- */
void rTest_Clear_invalidBitmask(void);
void rTest_ClearColor_specialFloats(void);
void rTest_ClearDepthf_clamping(void);
void rTest_ClearStencil_bounds(void);

/* --------------- glGenBuffers --------------- */
void rTest_glGenBuffers_invalid_value(void);
void rTest_glGenBuffers_zero_count(void);
void rTest_glGenBuffers_null_buffers(void);
void rTest_glGenBuffers_large_n(void);
void rTest_glGenBuffers_repeated_generation(void);
void rTest_glGenBuffers_unique_names(void);
void rTest_glGenBuffers_unbound_names_lifecycle(void);
void rTest_glGenBuffers_double_delete(void);
void rTest_glGenBuffers_huge_count_small_buffer(void);

/* --------------- glBindBuffer --------------- */
void rTest_glBindBuffer_invalid_enum(void);
void rTest_glBindBuffer_new_name_without_gen(void);
void rTest_glBindBuffer_deleted_buffer(void);
void rTest_glBindBuffer_boundary_handles(void);
void rTest_glBindBuffer_dirty_high_bits_enum(void);
void rTest_glBindBuffer_rapid_cross_target_rebind_stress(void);
void rTest_glBindBuffer_delete_while_double_bound(void);
void rTest_glBindBuffer_zero_binding_query_thrash(void);
void rTest_glBindBuffer_massive_namespace_fuzz(void);
void rTest_glBindBuffer_binding_churn_stress(void);
void rTest_glBindBuffer_lifecycle_stress(void);

/* --------------- glBufferData --------------- */
void rTest_glBufferData_invalid_enum_target(void);
void rTest_glBufferData_invalid_enum_usage(void);
void rTest_glBufferData_invalid_value_negative_size(void);
void rTest_glBufferData_invalid_operation_zero_buffer_bound(void);
void rTest_glBufferData_out_of_memory(void);
void rTest_glBufferData_source_buffer_too_small(void);
void rTest_glBufferData_zero_size_nonnull_data(void);
void rTest_glBufferData_size_overflow_boundary(void);
void rTest_glBufferData_dirty_usage_enum(void);
void rTest_glBufferData_target_zero_bound(void);
void rTest_glBufferData_repeated_resize_thrash(void);
void rTest_glBufferData_misaligned_data_pointer(void);
void rTest_glBufferData_dangling_data_pointer(void);
void rTest_glBufferData_state_after_out_of_memory(void);

/* --------------- glBufferSubData --------------- */
void rTest_glBufferSubData_invalid_enum_target(void);
void rTest_glBufferSubData_invalid_value_negative_offset(void);
void rTest_glBufferSubData_invalid_value_negative_size(void);
void rTest_glBufferSubData_invalid_value_out_of_bounds(void);
void rTest_glBufferSubData_invalid_operation_zero_buffer_bound(void);
void rTest_glBufferSubData_offset_size_overflow_wraparound(void);
void rTest_glBufferSubData_exact_boundary_offset(void);
void rTest_glBufferSubData_negative_offset_compensating_size(void);
void rTest_glBufferSubData_zero_size_null_data(void);
void rTest_glBufferSubData_target_zero_bound(void);
void rTest_glBufferSubData_into_zero_sized_store(void);
void rTest_glBufferSubData_source_smaller_than_size(void);
void rTest_glBufferSubData_dangling_data_pointer(void);
void rTest_glBufferSubData_overlapping_misaligned_thrash(void);

/* --------------- glGetBufferParameteriv --------------- */
void rTest_glGetBufferParameteriv_invalid_enum_target(void);
void rTest_glGetBufferParameteriv_invalid_enum_value(void);
void rTest_glGetBufferParameteriv_invalid_operation_zero_buffer_bound(void);
void rTest_glGetBufferParameteriv_invalid_target(void);
void rTest_glGetBufferParameteriv_invalid_value(void);
void rTest_glGetBufferParameteriv_reserved_name_zero_bound(void);
void rTest_glGetBufferParameteriv_element_array_zero_bound(void);
void rTest_glGetBufferParameteriv_null_data_pointer(void);
void rTest_glGetBufferParameteriv_dangling_data_pointer(void);
void rTest_glGetBufferParameteriv_size_consistency(void);
void rTest_glGetBufferParameteriv_usage_initial_and_updates(void);
void rTest_glGetBufferParameteriv_both_invalid(void);
void rTest_glGetBufferParameteriv_after_delete_binding_reverts(void);
void rTest_glGetBufferParameteriv_same_buffer_multiple_targets(void);
void rTest_glGetBufferParameteriv_unaligned_data_pointer(void);
void rTest_glGetBufferParameteriv_error_state_stress(void);

/* --------------- glLineWidth --------------- */
void test_lineWidth_basicRobustness(void);
void test_lineWidth_stressSweep(void);
void test_lineWidth_specialFloats(void);
void test_lineWidth_errorQueue(void);
void test_lineWidth_limits(void);

/* --------------- glCullFace --------------- */
void test_cullFace_basicRobustness(void);
void test_cullFace_stressSweep(void);
void test_cullFace_errorQueue(void);
void test_cullFace_rapidToggle(void);
void test_cullFace_statePreservation(void);
void test_cullFace_frontFaceCombination(void);
void test_cullFace_largeEnum(void);
void test_cullFace_rapidFire(void);
void test_cullFace_randomFuzz(void);

/* --------------- glEnable / glDisable (Cull Face) --------------- */
void test_cullFaceEnable_basicRobustness(void);
void test_cullFaceEnable_rapidToggle(void);
void test_cullFaceEnable_invalidCaps(void);
void test_cullFaceEnable_capCombinations(void);

/* --------------- glFrontFace --------------- */
void test_frontFace_errorQueue(void);
void test_frontFace_rapidToggle(void);
void test_frontFace_mixedValidity(void);
void test_frontFace_statePreservation(void);
void test_frontFace_cullCombinations(void);
void test_frontFace_largeEnum(void);
void test_frontFace_rapidFire(void);
void test_frontFace_randomFuzz(void);

/* --------------- glPolygonOffset --------------- */
void test_polygonOffset_basicRobustness(void);
void test_polygonOffset_stressSweep(void);
void test_polygonOffset_errorQueue(void);
void test_polygonOffset_statePreservation(void);
void test_polygonOffset_specialFloats(void);

/* --------------- glViewport --------------- */
void test_viewport_basicRobustness(void);
void test_viewport_negativeDimensions(void);
void test_viewport_boundaryCoordinates(void);
void test_viewport_limits(void);
void test_viewport_errorQueue(void);
void test_viewport_stress(void);

/* --------------- glDepthRange --------------- */
void test_depthRange_basicRobustness(void);
void test_depthRange_parameterSweep(void);
void test_depthRange_specialValues(void);
void test_depthRange_stateQuery(void);
void test_depthRange_errorQueue(void);
void test_depthRange_stress(void);

/* --------------- glPixelStorei --------------- */
void test_pixelStore_basicRobustness(void);
void test_pixelStore_invalidAlignment(void);
void test_pixelStore_invalidPname(void);
void test_pixelStore_statePreservation(void);
void test_pixelStore_errorQueue(void);
void test_pixelStore_stress(void);

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
