/* main.c
 * Rasterization Robustness Test Suite
 *
 * glLineWidth, glFrontFace, glCullFace, glEnable/glDisable,
 * glPolygonOffset fonksiyonlarinin robustness testlerini
 * calistiran ana program.
 */

#include <stdio.h>

/* ============================================================
 * Test fonksiyon bildirimleri
 * ============================================================ */

/* glLineWidth */
extern void Rasterizaton_LineWidth_TC_001(void);
extern void Rasterizaton_LineWidth_TC_002(void);
extern void Rasterizaton_LineWidth_TC_003(void);
extern void Rasterizaton_LineWidth_TC_004(void);
extern void Rasterizaton_LineWidth_TC_005(void);

/* glFrontFace */
extern void Rasterizaton_FrontFace_TC_001(void);
extern void Rasterizaton_FrontFace_TC_002(void);
extern void Rasterizaton_FrontFace_TC_003(void);
extern void Rasterizaton_FrontFace_TC_006(void);

/* glCullFace */
extern void Rasterizaton_CullFace_TC_001(void);
extern void Rasterizaton_CullFace_TC_002(void);
extern void Rasterizaton_CullFace_TC_003(void);
extern void Rasterizaton_CullFace_TC_004(void);
extern void test_cullFace_frontFaceCombo(void);

/* glEnable_glDisable */
extern void test_cullFaceEnable_basicRobustness(void);
extern void test_cullFaceEnable_rapidToggle(void);
extern void test_cullFaceEnable_invalidCaps(void);
extern void test_cullFaceEnable_capCombinations(void);

/* glPolygonOffset */
extern void Rasterizaton_PolygonOffset_TC_001(void);
extern void Rasterizaton_PolygonOffset_TC_002(void);
extern void Rasterizaton_PolygonOffset_TC_003(void);
extern void Rasterizaton_PolygonOffset_TC_004(void);
extern void Rasterizaton_PolygonOffset_TC_005(void);

/* ============================================================
 * Test calistirma cercevesi
 * ============================================================ */

typedef void (*TestFunc)(void);

static struct {
    const char *name;
    TestFunc func;
} testList[] = {
    /* glLineWidth */
    {"glLineWidth - Basic Robustness",       Rasterizaton_LineWidth_TC_001},
    {"glLineWidth - Stress Sweep",           Rasterizaton_LineWidth_TC_002},
    {"glLineWidth - Special Floats",         Rasterizaton_LineWidth_TC_003},
    {"glLineWidth - Error Queue",            Rasterizaton_LineWidth_TC_004},
    {"glLineWidth - Implementation Limits",  Rasterizaton_LineWidth_TC_005},

    /* glFrontFace */
    {"glFrontFace - Error Queue",            Rasterizaton_FrontFace_TC_001},
    {"glFrontFace - Rapid Toggle",           Rasterizaton_FrontFace_TC_002},
    {"glFrontFace - Mixed Validity",         Rasterizaton_FrontFace_TC_003},
    {"glFrontFace - Large Enum",             Rasterizaton_FrontFace_TC_006},

    /* glCullFace */
    {"glCullFace - Basic Robustness",        Rasterizaton_CullFace_TC_001},
    {"glCullFace - Stress Sweep",            Rasterizaton_CullFace_TC_002},
    {"glCullFace - Error Queue",             Rasterizaton_CullFace_TC_003},
    {"glCullFace - Rapid Toggle",            Rasterizaton_CullFace_TC_004},
    {"glCullFace - FrontFace Combinations",  test_cullFace_frontFaceCombo},

    /* glEnable/glDisable */
    {"glEnable/Disable - Basic Robustness",  test_cullFaceEnable_basicRobustness},
    {"glEnable/Disable - Rapid Toggle",      test_cullFaceEnable_rapidToggle},
    {"glEnable/Disable - Cap Combinations",  test_cullFaceEnable_capCombinations},
    {"glEnable/Disable - Invalid Caps",      test_cullFaceEnable_invalidCaps},

    /* glPolygonOffset */
    {"glPolygonOffset - Basic Robustness",   Rasterizaton_PolygonOffset_TC_001},
    {"glPolygonOffset - Stress Sweep",       Rasterizaton_PolygonOffset_TC_002},
    {"glPolygonOffset - Error Queue",        Rasterizaton_PolygonOffset_TC_003},
    {"glPolygonOffset - State Preservation", Rasterizaton_PolygonOffset_TC_004},
    {"glPolygonOffset - Special Floats",     Rasterizaton_PolygonOffset_TC_005},
};

int main(int argc, char **argv) {
    int i;
    int testCount = sizeof(testList) / sizeof(testList[0]);

    printf("========================================\n");
    printf("Rasterization Robustness Test Suite\n");
    printf("Toplam test: %d\n", testCount);
    printf("Fonksiyonlar: glLineWidth, glFrontFace,\n");
    printf("              glCullFace, glEnable/glDisable,\n");
    printf("              glPolygonOffset\n");
    printf("========================================\n\n");

    for (i = 0; i < testCount; i++) {
        printf("----------------------------------------\n");
        printf("[%d/%d] %s\n", i + 1, testCount, testList[i].name);
        printf("----------------------------------------\n");
        testList[i].func();
    }

    printf("========================================\n");
    printf("TUM TESTLER BASARIYLA TAMAMLANDI\n");
    printf("========================================\n");

    return 0;
}
