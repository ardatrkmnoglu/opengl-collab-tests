#ifndef TESTS_H
#define TESTS_H







// --------------- glBindBuffer ---------------
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

// --------------- glGenBuffers ---------------
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