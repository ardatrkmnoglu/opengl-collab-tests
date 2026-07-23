#ifndef RTESTS_H
#define RTESTS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "helper.h"
#include "macro.h"

extern int retcode;

void init(void);
void draw(void);
void cleanup(void);

/* --------------- Shaders and Programs --------------- */
void ShadersAndPrograms_CreateProgram_TC_001(void);
void ShadersAndPrograms_CreateProgram_TC_002(void);
void ShadersAndPrograms_CreateProgram_TC_003(void);
void ShadersAndPrograms_CreateProgram_TC_004(void);

void ShadersAndPrograms_ProgramBinary_TC_001(void);
void ShadersAndPrograms_ProgramBinary_TC_002(void);
void ShadersAndPrograms_ProgramBinary_TC_003(void);
void ShadersAndPrograms_ProgramBinary_TC_004(void);

void ShadersAndPrograms_UseProgram_TC_001(void);
void ShadersAndPrograms_UseProgram_TC_002(void);
void ShadersAndPrograms_UseProgram_TC_003(void);
void ShadersAndPrograms_UseProgram_TC_004(void);

void ShadersAndPrograms_GetAttribLocation_TC_001(void);
void ShadersAndPrograms_GetAttribLocation_TC_002(void);
void ShadersAndPrograms_GetAttribLocation_TC_003(void);
void ShadersAndPrograms_GetAttribLocation_TC_004(void);

void ShadersAndPrograms_Uniform_TC_005(void);
void ShadersAndPrograms_Uniform_TC_006(void);

/* --------------- glGetProgramiv --------------- */
void ShaderQueries_GetProgram_TC_001(void);
void ShaderQueries_GetProgram_TC_002(void);
void ShaderQueries_GetProgram_TC_003(void);
void ShaderQueries_GetProgram_TC_004(void);

/* --------------- glGetUniformLocation --------------- */
void ShadersAndPrograms_GetUniformLocation_TC_001(void);
void ShadersAndPrograms_GetUniformLocation_TC_002(void);
void ShadersAndPrograms_GetUniformLocation_TC_003(void);
void ShadersAndPrograms_GetUniformLocation_TC_004(void);

/* --------------- glUniform{1234}{if} --------------- */
void ShadersAndPrograms_Uniform_TC_001(void);
void ShadersAndPrograms_Uniform_TC_002(void);

/* --------------- glUniform{1234}{if}v --------------- */
void ShadersAndPrograms_Uniform_TC_003(void);
void ShadersAndPrograms_Uniform_TC_004(void);

/* --------------- glUniformMatrix{234}fv --------------- */
void ShadersAndPrograms_UniformMatrix_TC_001(void);
void ShadersAndPrograms_UniformMatrix_TC_002(void);
void ShadersAndPrograms_UniformMatrix_TC_003(void);
void ShadersAndPrograms_UniformMatrix_TC_004(void);

/* --------------- glGetVertexAttribfv / iv / Pointerv --------------- */
void ShaderQueries_GetVertexAttrib_TC_001(void);
void ShaderQueries_GetVertexAttrib_TC_002(void);
void ShaderQueries_GetVertexAttrib_TC_003(void);
void ShaderQueries_GetVertexAttrib_TC_004(void);

void ShaderQueries_GetVertexAttribPointer_TC_001(void);
void ShaderQueries_GetVertexAttribPointer_TC_002(void);
void ShaderQueries_GetVertexAttribPointer_TC_003(void);
void ShaderQueries_GetVertexAttribPointer_TC_004(void);

/* --------------- glGetnUniformfv / iv (Robustness Extension) ---------------
 */
void ShaderQueries_GetnUniform_TC_001(void);
void ShaderQueries_GetnUniform_TC_002(void);
void ShaderQueries_GetnUniform_TC_003(void);
void ShaderQueries_GetnUniform_TC_004(void);

/* --------------- glVertexAttrib --------------- */
void Vertices_VertexAttrib_TC_001(void);
void Vertices_VertexAttrib_TC_002(void);
void Vertices_VertexAttrib_TC_003(void);
void Vertices_VertexAttrib_TC_004(void);

/* --------------- glVertexAttribPointer --------------- */
void Vertices_VertexAttribPointer_TC_001(void);
void Vertices_VertexAttribPointer_TC_002(void);
void Vertices_VertexAttribPointer_TC_003(void);
void Vertices_VertexAttribPointer_TC_004(void);
void Vertices_VertexAttribPointer_TC_005(void);

/* --------------- glEnable/DisableVertexAttribArray --------------- */
void Vertices_EnableVertexAttribArray_TC_001(void);
void Vertices_EnableVertexAttribArray_TC_002(void);
void Vertices_EnableVertexAttribArray_TC_003(void);
void Vertices_EnableVertexAttribArray_TC_004(void);
void Vertices_DisableVertexAttribArray_TC_001(void);
void Vertices_DisableVertexAttribArray_TC_002(void);
void Vertices_DisableVertexAttribArray_TC_003(void);
void Vertices_DisableVertexAttribArray_TC_004(void);

/* --------------- glDrawArrays --------------- */
void Vertices_DrawArrays_TC_001(void);
void Vertices_DrawArrays_TC_002(void);
void Vertices_DrawArrays_TC_003(void);
void Vertices_DrawArrays_TC_004(void);
void Vertices_DrawArrays_TC_005(void);
void Vertices_DrawArrays_TC_006(void);

/* --------------- glDrawElements / glDrawRangeElements --------------- */
void Vertices_DrawElements_TC_001(void);
void Vertices_DrawElements_TC_002(void);
void Vertices_DrawElements_TC_003(void);
void Vertices_DrawElements_TC_004(void);

void Vertices_DrawRangeElements_TC_001(void);
void Vertices_DrawRangeElements_TC_002(void);
void Vertices_DrawRangeElements_TC_003(void);
void Vertices_DrawRangeElements_TC_004(void);

/* --------------- Framebuffer Mask Operations --------------- */
void WholeFramebufferOperations_ColorMask_TC_001(void);
void WholeFramebufferOperations_StencilMaskSeparate_TC_001(void);

/* --------------- Framebuffer Clear Operations --------------- */
void WholeFramebufferOperations_Clear_TC_001(void);
void WholeFramebufferOperations_ClearColor_TC_001(void);
void WholeFramebufferOperations_ClearDepthf_TC_001(void);
void WholeFramebufferOperations_ClearStencil_TC_001(void);

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

/* --- Automatically added test declarations --- */
void BufferObjects_BindBuffer_TC_002(void);
void BufferObjects_BindBuffer_TC_003(void);
void BufferObjects_BindBuffer_TC_004(void);
void BufferObjects_BindBuffer_TC_005(void);
void BufferObjects_BindBuffer_TC_006(void);
void BufferObjects_BindBuffer_TC_007(void);
void BufferObjects_BindBuffer_TC_008(void);
void BufferObjects_BindBuffer_TC_009(void);
void BufferObjects_BindBuffer_TC_010(void);
void BufferObjects_BindBuffer_TC_011(void);
void BufferObjects_BufferData_TC_001(void);
void BufferObjects_BufferData_TC_002(void);
void BufferObjects_BufferData_TC_003(void);
void BufferObjects_BufferData_TC_004(void);
void BufferObjects_BufferData_TC_005(void);
void BufferObjects_BufferData_TC_006(void);
void BufferObjects_BufferData_TC_007(void);
void BufferObjects_BufferData_TC_008(void);
void BufferObjects_BufferData_TC_009(void);
void BufferObjects_BufferData_TC_010(void);
void BufferObjects_BufferData_TC_011(void);
void BufferObjects_BufferData_TC_012(void);
void BufferObjects_BufferData_TC_013(void);
void BufferObjects_BufferData_TC_014(void);
void BufferObjects_BufferSubData_TC_001(void);
void BufferObjects_BufferSubData_TC_002(void);
void BufferObjects_BufferSubData_TC_003(void);
void BufferObjects_BufferSubData_TC_004(void);
void BufferObjects_BufferSubData_TC_005(void);
void BufferObjects_BufferSubData_TC_006(void);
void BufferObjects_BufferSubData_TC_007(void);
void BufferObjects_BufferSubData_TC_008(void);
void BufferObjects_BufferSubData_TC_009(void);
void BufferObjects_BufferSubData_TC_010(void);
void BufferObjects_BufferSubData_TC_011(void);
void BufferObjects_BufferSubData_TC_012(void);
void BufferObjects_BufferSubData_TC_013(void);
void BufferObjects_BufferSubData_TC_014(void);
void BufferObjects_GenBuffers_TC_001(void);
void BufferObjects_GenBuffers_TC_002(void);
void BufferObjects_GenBuffers_TC_003(void);
void BufferObjects_GenBuffers_TC_004(void);
void BufferObjects_GenBuffers_TC_005(void);
void BufferObjects_GenBuffers_TC_006(void);
void BufferObjects_GenBuffers_TC_007(void);
void BufferObjects_GenBuffers_TC_008(void);
void BufferObjects_GenBuffers_TC_009(void);
void BufferObjects_GetBufferParameteriv_TC_001(void);
void BufferObjects_GetBufferParameteriv_TC_002(void);
void BufferObjects_GetBufferParameteriv_TC_003(void);
void BufferObjects_GetBufferParameteriv_TC_004(void);
void BufferObjects_GetBufferParameteriv_TC_005(void);
void BufferObjects_GetBufferParameteriv_TC_006(void);
void BufferObjects_GetBufferParameteriv_TC_007(void);
void BufferObjects_GetBufferParameteriv_TC_008(void);
void BufferObjects_GetBufferParameteriv_TC_009(void);
void BufferObjects_GetBufferParameteriv_TC_010(void);
void BufferObjects_GetBufferParameteriv_TC_011(void);
void BufferObjects_GetBufferParameteriv_TC_012(void);
void BufferObjects_GetBufferParameteriv_TC_013(void);
void BufferObjects_GetBufferParameteriv_TC_014(void);
void BufferObjects_GetBufferParameteriv_TC_015(void);
void BufferObjects_GetBufferParameteriv_TC_016(void);
void FramebufferObjects_BindFramebuffer_TC_001(void);
void FramebufferObjects_BindFramebuffer_TC_002(void);
void FramebufferObjects_BindFramebuffer_TC_003(void);
void FramebufferObjects_BindFramebuffer_TC_004(void);
void FramebufferObjects_BindFramebuffer_TC_005(void);
void FramebufferObjects_BindFramebuffer_TC_006(void);
void FramebufferObjects_BindFramebuffer_TC_007(void);
void FramebufferObjects_BindFramebuffer_TC_008(void);
void FramebufferObjects_BindFramebuffer_TC_009(void);
void FramebufferObjects_BindFramebuffer_TC_010(void);
void FramebufferObjects_BindFramebuffer_TC_011(void);
void FramebufferObjects_BindFramebuffer_TC_012(void);
void FramebufferObjects_GenFramebuffers_TC_001(void);
void FramebufferObjects_GenFramebuffers_TC_002(void);
void FramebufferObjects_GenFramebuffers_TC_003(void);
void FramebufferObjects_GenFramebuffers_TC_004(void);
void FramebufferObjects_GenFramebuffers_TC_005(void);
void FramebufferObjects_GenFramebuffers_TC_006(void);
void FramebufferObjects_GenFramebuffers_TC_007(void);
void FramebufferObjects_GenFramebuffers_TC_008(void);
void FramebufferObjects_GenFramebuffers_TC_009(void);
void FramebufferObjects_GenFramebuffers_TC_010(void);
void FramebufferObjects_GenFramebuffers_TC_011(void);
void FramebufferObjects_GenFramebuffers_TC_012(void);
void FramebufferObjects_GenFramebuffers_TC_013(void);
void FramebufferObjects_GenFramebuffers_TC_014(void);
void FramebufferObjects_GenFramebuffers_TC_015(void);
void PerFragmentOperations_Scissor_TC_001(void);
void PerFragmentOperations_Scissor_TC_002(void);
void PerFragmentOperations_Scissor_TC_003(void);
void PerFragmentOperations_Scissor_TC_004(void);
void PerFragmentOperations_Scissor_TC_005(void);
void PerFragmentOperations_Scissor_TC_006(void);
void PixelRectangles_PixelStorei_TC_001(void);
void PixelRectangles_PixelStorei_TC_002(void);
void PixelRectangles_PixelStorei_TC_003(void);
void PixelRectangles_PixelStorei_TC_004(void);
void PixelRectangles_PixelStorei_TC_005(void);
void PixelRectangles_PixelStorei_TC_006(void);
void Rasterizaton_CullFace_TC_001(void);
void Rasterizaton_CullFace_TC_002(void);
void Rasterizaton_CullFace_TC_003(void);
void Rasterizaton_CullFace_TC_004(void);
void Rasterizaton_CullFace_TC_005(void);
void Rasterizaton_CullFace_TC_006(void);
void Rasterizaton_CullFace_TC_007(void);
void Rasterizaton_CullFace_TC_008(void);
void Rasterizaton_CullFace_TC_009(void);
void Rasterizaton_FrontFace_TC_001(void);
void Rasterizaton_FrontFace_TC_002(void);
void Rasterizaton_FrontFace_TC_003(void);
void Rasterizaton_FrontFace_TC_004(void);
void Rasterizaton_FrontFace_TC_005(void);
void Rasterizaton_FrontFace_TC_006(void);
void Rasterizaton_FrontFace_TC_007(void);
void Rasterizaton_FrontFace_TC_008(void);
void Rasterizaton_LineWidth_TC_001(void);
void Rasterizaton_LineWidth_TC_002(void);
void Rasterizaton_LineWidth_TC_003(void);
void Rasterizaton_LineWidth_TC_004(void);
void Rasterizaton_LineWidth_TC_005(void);
void Rasterizaton_LineWidth_TC_006(void);
void Rasterizaton_LineWidth_TC_007(void);
void Rasterizaton_LineWidth_TC_008(void);
void Rasterizaton_PolygonOffset_TC_001(void);
void Rasterizaton_PolygonOffset_TC_002(void);
void Rasterizaton_PolygonOffset_TC_003(void);
void Rasterizaton_PolygonOffset_TC_004(void);
void Rasterizaton_PolygonOffset_TC_005(void);
void Rasterizaton_PolygonOffset_TC_006(void);
void Rasterizaton_PolygonOffset_TC_007(void);
void ReadingPixels_ReadnPixels_TC_001(void);
void ReadingPixels_ReadnPixels_TC_002(void);
void ReadingPixels_ReadnPixels_TC_003(void);
void ReadingPixels_ReadnPixels_TC_004(void);
void Run_glCullFace_Robustness(void);
void Run_glDepthRange_Robustness(void);
void Run_glFinish_Robustness(void);
void Run_glFlush_Robustness(void);
void Run_glFrontFace_Robustness(void);
void Run_glGetError_Robustness(void);
void Run_glLineWidth_Robustness(void);
void Run_glPixelStorei_Robustness(void);
void Run_glViewport_Robustness(void);
void SpecialFunctions_Finish_TC_001(void);
void SpecialFunctions_Finish_TC_002(void);
void SpecialFunctions_Finish_TC_003(void);
void SpecialFunctions_Finish_TC_004(void);
void SpecialFunctions_Finish_TC_005(void);
void SpecialFunctions_Finish_TC_006(void);
void SpecialFunctions_Finish_TC_007(void);
void Texturing_BindTexture_TC_001(void);
void Texturing_BindTexture_TC_002(void);
void Texturing_BindTexture_TC_003(void);
void Texturing_BindTexture_TC_004(void);
void Texturing_BindTexture_TC_005(void);
void Texturing_BindTexture_TC_006(void);
void Texturing_BindTexture_TC_007(void);
void Texturing_BindTexture_TC_008(void);
void Texturing_GenTextures_TC_001(void);
void Texturing_GenTextures_TC_002(void);
void Texturing_GenTextures_TC_003(void);
void Texturing_GenTextures_TC_004(void);
void Texturing_GenTextures_TC_005(void);
void Texturing_GenTextures_TC_006(void);
void Texturing_GenTextures_TC_007(void);
void Texturing_GenTextures_TC_008(void);
void Texturing_GenTextures_TC_009(void);
void Texturing_GenTextures_TC_010(void);
void Texturing_GenTextures_TC_011(void);
void Texturing_GenTextures_TC_012(void);
void Texturing_GenerateMipmap_TC_001(void);
void Texturing_GenerateMipmap_TC_002(void);
void Texturing_GenerateMipmap_TC_003(void);
void Texturing_GenerateMipmap_TC_004(void);
void Texturing_GenerateMipmap_TC_005(void);
void Texturing_GenerateMipmap_TC_006(void);
void Texturing_GenerateMipmap_TC_007(void);
void Texturing_GenerateMipmap_TC_008(void);
void Texturing_GenerateMipmap_TC_009(void);
void Texturing_GenerateMipmap_TC_010(void);
void Texturing_GenerateMipmap_TC_011(void);
void Texturing_GenerateMipmap_TC_012(void);
void Texturing_GenerateMipmap_TC_013(void);
void Texturing_GenerateMipmap_TC_014(void);
void Texturing_GenerateMipmap_TC_015(void);
void Texturing_TexStorage2D_TC_001(void);
void Texturing_TexStorage2D_TC_002(void);
void Texturing_TexStorage2D_TC_003(void);
void Texturing_TexStorage2D_TC_004(void);
void Texturing_TexStorage2D_TC_005(void);
void Texturing_TexSubImage2D_TC_001(void);
void Texturing_TexSubImage2D_TC_002(void);
void Texturing_TexSubImage2D_TC_003(void);
void Texturing_TexSubImage2D_TC_004(void);
void Texturing_TexSubImage2D_TC_005(void);
void ViewportandClipping_DepthRangef_TC_001(void);
void ViewportandClipping_DepthRangef_TC_002(void);
void ViewportandClipping_DepthRangef_TC_003(void);
void ViewportandClipping_DepthRangef_TC_004(void);
void ViewportandClipping_DepthRangef_TC_005(void);
void ViewportandClipping_DepthRangef_TC_006(void);
void ViewportandClipping_Viewport_TC_001(void);
void ViewportandClipping_Viewport_TC_002(void);
void ViewportandClipping_Viewport_TC_003(void);
void ViewportandClipping_Viewport_TC_004(void);
void ViewportandClipping_Viewport_TC_005(void);
void ViewportandClipping_Viewport_TC_006(void);
void WholeFramebufferOperations_ClearColor_TC_002(void);
void WholeFramebufferOperations_ClearColor_TC_003(void);
void WholeFramebufferOperations_ClearColor_TC_004(void);
void WholeFramebufferOperations_ClearColor_TC_005(void);
void WholeFramebufferOperations_ClearDepthf_TC_002(void);
void WholeFramebufferOperations_ClearDepthf_TC_003(void);
void WholeFramebufferOperations_ClearDepthf_TC_004(void);
void WholeFramebufferOperations_ClearDepthf_TC_005(void);
void WholeFramebufferOperations_ClearStencil_TC_002(void);
void WholeFramebufferOperations_ClearStencil_TC_003(void);
void WholeFramebufferOperations_ClearStencil_TC_004(void);
void WholeFramebufferOperations_ClearStencil_TC_005(void);
void WholeFramebufferOperations_Clear_TC_002(void);
void WholeFramebufferOperations_Clear_TC_003(void);
void WholeFramebufferOperations_Clear_TC_004(void);
void WholeFramebufferOperations_Clear_TC_005(void);
void WholeFramebufferOperations_ColorMask_TC_002(void);
void WholeFramebufferOperations_ColorMask_TC_003(void);
void WholeFramebufferOperations_ColorMask_TC_004(void);
void WholeFramebufferOperations_ColorMask_TC_005(void);
void WholeFramebufferOperations_DepthMask_TC_001(void);
void WholeFramebufferOperations_DepthMask_TC_002(void);
void WholeFramebufferOperations_DepthMask_TC_003(void);
void WholeFramebufferOperations_DepthMask_TC_004(void);
void WholeFramebufferOperations_StencilMaskSeparate_TC_002(void);
void WholeFramebufferOperations_StencilMaskSeparate_TC_003(void);
void WholeFramebufferOperations_StencilMaskSeparate_TC_004(void);
void WholeFramebufferOperations_StencilMaskSeparate_TC_005(void);
void WholeFramebufferOperations_StencilMask_TC_001(void);
void WholeFramebufferOperations_StencilMask_TC_002(void);
void WholeFramebufferOperations_StencilMask_TC_003(void);
void WholeFramebufferOperations_StencilMask_TC_004(void);
void SpecialFunctions_Flush_TC_001(void);
void SpecialFunctions_Flush_TC_002(void);
void SpecialFunctions_Flush_TC_003(void);
void SpecialFunctions_Flush_TC_004(void);
void SpecialFunctions_Flush_TC_005(void);
void SpecialFunctions_Flush_TC_006(void);
void SpecialFunctions_Flush_TC_007(void);
void ErrorsandStatusReset_GetError_TC_001(void);
void ErrorsandStatusReset_GetError_TC_002(void);
void ErrorsandStatusReset_GetError_TC_003(void);
void ErrorsandStatusReset_GetError_TC_004(void);
void ErrorsandStatusReset_GetError_TC_005(void);
void ErrorsandStatusReset_GetError_TC_006(void);
void ErrorsandStatusReset_GetError_TC_007(void);

#endif /* RTESTS_H */
