#ifndef TEST_UTILITY_H
#define TEST_UTILITY_H

#include "include/helper.h"
#include "include/macro.h"
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

extern int retcode;

#define CHECK_ERROR(tp)                                                        \
	do {                                                                   \
		GLenum err = glGetError();                                     \
		if (err != GL_NO_ERROR) {                                      \
			printf("\\x1b[31m[ERROR]\\x1b[0m OpenGL error 0x%04X " \
			       "in %s close\\n",                               \
			       err, tp);                                       \
		}                                                              \
	} while (0)

extern void GS_GL20SC_FOP_CC_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_CC_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_CC_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FOP_CD_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_CD_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_CD_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FOP_CM_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_CM_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_CM_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_CS_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FOP_C_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_C_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_C_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_DM_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_SMS_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_FOP_SM_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_FOP_SM_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_FOP_SM_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_CP_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_CP_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_CP_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_GAL_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_GPIV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_GUFV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_GUFV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_GUFV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_GUIV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_GUL_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_GUL_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_GUL_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_GVAFV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_GVAFV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_GVAFV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_GVAIV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_PB_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_PB_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_PB_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U1FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U1FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U1FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U1F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U1F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U1F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U1IV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U1IV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U1IV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U1I_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U1I_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U1I_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U2FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U2F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U2F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U2F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U2IV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U2IV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U2IV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U2I_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U2I_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U2I_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U3FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U3FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U3FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U3F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U3IV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U3IV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U3IV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U3I_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U4FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U4FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U4FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U4F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U4F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U4F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U4IV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U4IV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U4IV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_U4I_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_U4I_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_U4I_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_UM2FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_UM3FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_UM3FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_UM3FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_UM4FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_UM4FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_UM4FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_UP_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_UP_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_UP_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_SP_VAPV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_SP_VAPV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_SP_VAPV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_DA_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_DA_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_DA_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_DE_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_DE_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_DE_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_DRE_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_DRE_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_DRE_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_DVAA_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_DVAA_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_DVAA_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_EVAA_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_EVAA_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_EVAA_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA1FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA1F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA1F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA1F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA2FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA2FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA2FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA2F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA2F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA2F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA3FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA3FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA3FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA3F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA3F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA3F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA4FV_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA4FV_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA4FV_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VA4F_ROBUSTNESS_TP_001_close(void);

extern void GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001_init(void);
extern void GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001_draw(void);
extern void GS_GL20SC_VERT_VAP_ROBUSTNESS_TP_001_close(void);

extern void init(void);
extern void draw(void);
extern void cleanup(void);
#endif /* TEST_UTILITY_H */
