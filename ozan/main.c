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
extern void test_lineWidth_basicRobustness(void);
extern void test_lineWidth_stressSweep(void);
extern void test_lineWidth_specialFloats(void);
extern void test_lineWidth_errorQueue(void);
extern void test_lineWidth_limits(void);

/* glFrontFace */
extern void test_frontFace_errorQueue(void);
extern void test_frontFace_rapidToggle(void);
extern void test_frontFace_mixedValidity(void);
extern void test_frontFace_largeEnum(void);

/* glCullFace */
extern void test_cullFace_basicRobustness(void);
extern void test_cullFace_stressSweep(void);
extern void test_cullFace_errorQueue(void);
extern void test_cullFace_rapidToggle(void);
extern void test_cullFace_frontFaceCombo(void);

/* glEnable_glDisable */
extern void test_cullFaceEnable_basicRobustness(void);
extern void test_cullFaceEnable_rapidToggle(void);
extern void test_cullFaceEnable_invalidCaps(void);
extern void test_cullFaceEnable_capCombinations(void);

/* glPolygonOffset */
extern void test_polygonOffset_basicRobustness(void);
extern void test_polygonOffset_stressSweep(void);
extern void test_polygonOffset_errorQueue(void);
extern void test_polygonOffset_statePreservation(void);
extern void test_polygonOffset_specialFloats(void);

/* ============================================================
 * Test calistirma cercevesi
 * ============================================================ */

typedef void (*TestFunc)(void);

static struct {
    const char *name;
    TestFunc func;
} testList[] = {
    /* glLineWidth */
    {"glLineWidth - Basic Robustness",       test_lineWidth_basicRobustness},
    {"glLineWidth - Stress Sweep",           test_lineWidth_stressSweep},
    {"glLineWidth - Special Floats",         test_lineWidth_specialFloats},
    {"glLineWidth - Error Queue",            test_lineWidth_errorQueue},
    {"glLineWidth - Implementation Limits",  test_lineWidth_limits},

    /* glFrontFace */
    {"glFrontFace - Error Queue",            test_frontFace_errorQueue},
    {"glFrontFace - Rapid Toggle",           test_frontFace_rapidToggle},
    {"glFrontFace - Mixed Validity",         test_frontFace_mixedValidity},
    {"glFrontFace - Large Enum",             test_frontFace_largeEnum},

    /* glCullFace */
    {"glCullFace - Basic Robustness",        test_cullFace_basicRobustness},
    {"glCullFace - Stress Sweep",            test_cullFace_stressSweep},
    {"glCullFace - Error Queue",             test_cullFace_errorQueue},
    {"glCullFace - Rapid Toggle",            test_cullFace_rapidToggle},
    {"glCullFace - FrontFace Combinations",  test_cullFace_frontFaceCombo},

    /* glEnable/glDisable */
    {"glEnable/Disable - Basic Robustness",  test_cullFaceEnable_basicRobustness},
    {"glEnable/Disable - Rapid Toggle",      test_cullFaceEnable_rapidToggle},
    {"glEnable/Disable - Cap Combinations",  test_cullFaceEnable_capCombinations},
    {"glEnable/Disable - Invalid Caps",      test_cullFaceEnable_invalidCaps},

    /* glPolygonOffset */
    {"glPolygonOffset - Basic Robustness",   test_polygonOffset_basicRobustness},
    {"glPolygonOffset - Stress Sweep",       test_polygonOffset_stressSweep},
    {"glPolygonOffset - Error Queue",        test_polygonOffset_errorQueue},
    {"glPolygonOffset - State Preservation", test_polygonOffset_statePreservation},
    {"glPolygonOffset - Special Floats",     test_polygonOffset_specialFloats},
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
