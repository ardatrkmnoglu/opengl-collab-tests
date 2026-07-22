#ifndef TESTS_H
#define TESTS_H


/* --------------- glGetBufferParameteriv --------------- */
void rTest_glGetBufferParameteriv_invalid_enum_target();
void rTest_glGetBufferParameteriv_invalid_enum_value();
void rTest_glGetBufferParameteriv_invalid_operation_zero_buffer_bound();
void rTest_glGetBufferParameteriv_invalid_target();
void rTest_glGetBufferParameteriv_invalid_value();
void rTest_glGetBufferParameteriv_reserved_name_zero_bound();
void rTest_glGetBufferParameteriv_element_array_zero_bound();
void rTest_glGetBufferParameteriv_null_data_pointer();
void rTest_glGetBufferParameteriv_dangling_data_pointer();
void rTest_glGetBufferParameteriv_size_consistency();
void rTest_glGetBufferParameteriv_usage_initial_and_updates();
void rTest_glGetBufferParameteriv_both_invalid();
void rTest_glGetBufferParameteriv_after_delete_binding_reverts();
void rTest_glGetBufferParameteriv_same_buffer_multiple_targets();
void rTest_glGetBufferParameteriv_unaligned_data_pointer();
void rTest_glGetBufferParameteriv_error_state_stress();

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

#endif