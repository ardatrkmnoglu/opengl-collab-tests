#ifndef FRAMEBUFFER_TESTS_H
#define FRAMEBUFFER_TESTS_H

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