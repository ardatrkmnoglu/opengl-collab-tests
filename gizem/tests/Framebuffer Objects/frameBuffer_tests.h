#ifndef FRAMEBUFFER_TESTS_H
#define FRAMEBUFFER_TESTS_H


void rTest_glGenFramebuffers_invalid_value();
void rTest_glGenFramebuffers_negative_n();
void rTest_glGenFramebuffers_extreme_negative_n();
void rTest_glGenFramebuffers_zero_n_null_pointer();
void rTest_glGenFramebuffers_null_pointer_nonzero_n();
void rTest_glGenFramebuffers_invalid_memory_pointer();
void rTest_glGenFramebuffers_buffer_overflow_canary();
void rTest_glGenFramebuffers_huge_n();
void rTest_glGenFramebuffers_stack_smash_small_array();
void rTest_glGenFramebuffers_exhaustion();
void rTest_glGenFramebuffers_unbound_name_lifecycle();
void rTest_glGenFramebuffers_name_reuse_after_delete();
void rTest_glGenFramebuffers_unaligned_pointer();
void rTest_glGenFramebuffers_varying_n_stability();
void rTest_glGenFramebuffers_namespace_isolation_from_renderbuffers();



/* --------------- glBindFramebuffer --------------- */

void rTest_glBindFramebuffer_invalid_enum();
void rTest_glBindFramebuffer_invalid_target();
void rTest_glBindFramebuffer_arbitrary_unused_name();
void rTest_glBindFramebuffer_initial_attachment_state();
void rTest_glBindFramebuffer_zero_bound_query_rejected();
void rTest_glBindFramebuffer_binding_reverts_after_delete();
void rTest_glBindFramebuffer_repeated_rebind_same_name();
void rTest_glBindFramebuffer_extreme_name_value();
void rTest_glBindFramebuffer_type_confusion_with_other_object();
void rTest_glBindFramebuffer_bind_unbind_stress();
void rTest_glBindFramebuffer_previous_binding_broken_on_switch();
void rTest_glBindFramebuffer_self_rebind_preserves_state();



#endif