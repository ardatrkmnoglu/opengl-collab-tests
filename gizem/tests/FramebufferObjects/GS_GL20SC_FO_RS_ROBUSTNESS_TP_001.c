//Gizem'de çalışması için
#include <limits.h>
#include <glad/gles2.h>
#include <stdio.h>
#include "../../../include/macro.h"

// Arda'da çalışması için
// #include "../../../include/rtests.h"
// #include "../../../include/helper.h"
// #include "../../../include/macro.h"


/*
GL20SC - FramebufferObjects - RenderbufferStorage - ROBUSTNESS
*/

static const char* test_procedure = "GS_GL20SC_FO_RS_ROBUSTNESS_TP_001";
static const char* test_case_1 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_001";
static const char* test_case_2 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_002";
static const char* test_case_3 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_003";
static const char* test_case_4 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_004";
static const char* test_case_5 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_005";
static const char* test_case_6 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_006";
static const char* test_case_7 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_007";
static const char* test_case_8 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_008";
static const char* test_case_9 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_009";
static const char* test_case_10 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_010";
static const char* test_case_11 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_011";
static const char* test_case_12 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_012";
static const char* test_case_13 = "GS_GL20SC_FO_RS_ROBUSTNESS_TC_013";

// Belirtilen hata: GL_INVALID_ENUM is generated if target is not GL_RENDERBUFFER.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_001()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage((GLenum)0xFFFFFFFF, GL_RGBA4, 16, 16);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_1, test_procedure, "Expected GL_INVALID_ENUM for invalid target, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_1, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_ENUM is generated if internalformat is not an accepted format.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_002()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)0xFFFFFFFF, 16, 16);

    GLenum err = glGetError();
    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_2, test_procedure, "Expected GL_INVALID_ENUM for invalid internalformat, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_2, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if width is less than zero.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_003()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, -1, 16);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_3, test_procedure, "Expected GL_INVALID_VALUE for negative width, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_3, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if height is less than zero.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_004()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, -1);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_4, test_procedure, "Expected GL_INVALID_VALUE for negative height, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_4, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_VALUE is generated if width or height is greater than GL_MAX_RENDERBUFFER_SIZE
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_005()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxSize);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, maxSize + 1, 16);

    GLenum err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_5, test_procedure, "Expected GL_INVALID_VALUE for width exceeding GL_MAX_RENDERBUFFER_SIZE, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_5, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Belirtilen hata: GL_INVALID_OPERATION is generated if the reserved renderbuffer object name 0 is bound.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_006()
{
    while (glGetError() != GL_NO_ERROR) {}

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 16, 16);

    GLenum err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case_6, test_procedure, "Expected GL_INVALID_OPERATION with reserved name 0 bound, but got 0x%x.", err);
        return;
    }
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
}

// Belirtilen hata: GL_OUT_OF_MEMORY is generated if the implementation 
// is unable to create a data store with the requested width and height.
// Bunu tetiklemek amacıyla, izin verilen maksimum boyutta 
// (GL_MAX_RENDERBUFFER_SIZE x GL_MAX_RENDERBUFFER_SIZE) bir tahsis denenir;
// bu boyut değer aralığı açısından geçerlidir ancak gerçek bellekte genellikle karşılanamaz.
/* It failed on Windows */
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_007()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxSize);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, maxSize, maxSize);

    GLenum err = glGetError();
    if (err != GL_OUT_OF_MEMORY) {
        TEST_LOG_FAIL(test_case_7, test_procedure, "Expected GL_OUT_OF_MEMORY when data store cannot be created at maximum size, but got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_7, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


// Belirtilmeyen hatalar ------------------------------

// Geçersiz bir çağrı (invalid enum) sonrasında renderbuffer'ın önceden var olan
// depolama durumunun (storage state) bozulmadığını doğrular.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_008()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 32, 32);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)0xFFFFFFFF, 64, 64); // gecersiz internalformat

    GLenum err = glGetError();

    GLint width = -1, height = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    if (err != GL_INVALID_ENUM) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Expected GL_INVALID_ENUM, got 0x%x.", err);
    }
    else if (width != 32 || height != 32) {
        TEST_LOG_FAIL(test_case_8, test_procedure, "Previous valid storage was altered by a failed call (width=%d, height=%d, expected 32x32).", width, height);
    }
    else {
        TEST_LOG_SUCCESS(test_case_8, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// width ve height değerlerinin her ikisi de 0 olduğunda (sıfır boyutlu geçerli bir istek)
// implementasyonun hata üretmeden bu isteği kabul ettiğini doğrular.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_009()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 0, 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_9, test_procedure, "Expected GL_NO_ERROR for zero-sized (0,0) storage request, got 0x%x.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_9, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Aynı renderbuffer üzerinde ardışık olarak farklı boyut ve formatlarla reallocation
// implementasyonunun çökertmediğini ve son çağrının durumunu doğru şekilde yansıttığını doğrular.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_010()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 64, 64);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, 128, 32);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 8, 8);

    GLenum err = glGetError();

    GLint width = -1, height = -1, format = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format);

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Unexpected error 0x%x during repeated reallocation sequence.", err);
    }
    else if (width != 8 || height != 8 || format != GL_DEPTH_COMPONENT16) {
        TEST_LOG_FAIL(test_case_10, test_procedure, "Final storage state does not match last call (width=%d, height=%d, format=0x%x).", width, height, format);
    }
    else {
        TEST_LOG_SUCCESS(test_case_10, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// GL_OUT_OF_MEMORY hatası oluştuktan sonra renderbuffer nesnesinin ve context'in
// hala kullanılabilir durumda olduğunu, yani implementasyonun bu hatadan sonra
// kalıcı olarak bozulmadığını (recoverable olduğunu) doğrular.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_011()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxSize);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    // Önce GL_OUT_OF_MEMORY tetiklenmeye calisilir.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, maxSize, maxSize);
    glGetError(); // hata kodu ne olursa olsun temizle, asil kontrol bir sonraki adimda

    // Ardindan makul, kucuk bir boyutla tekrar cagri yapilir.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, 8, 8);

    GLenum err = glGetError();

    GLint width = -1, height = -1;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Context did not recover after out-of-memory condition, got error 0x%x.", err);
    }
    else if (width != 8 || height != 8) {
        TEST_LOG_FAIL(test_case_11, test_procedure, "Storage state incorrect after recovery from out-of-memory (width=%d, height=%d).", width, height);
    }
    else {
        TEST_LOG_SUCCESS(test_case_11, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Çok büyük fakat GL_MAX_RENDERBUFFER_SIZE sınırı içinde kalan (ancak gerçekte bellekte
// karşılanamayacak) bir boyut istendiğinde implementasyonun çökmediğini, bunun yerine
// GL_OUT_OF_MEMORY hatasıyla veya güvenli bir şekilde sonuçlandığını doğrular.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_012()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxSize);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, maxSize, maxSize);

    GLenum err = glGetError();

    if (err != GL_NO_ERROR && err != GL_OUT_OF_MEMORY && err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case_12, test_procedure, "Unexpected/undefined error code 0x%x for maximum allowed renderbuffer size.", err);
    }
    else {
        TEST_LOG_SUCCESS(test_case_12, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Sınır/uç değer testi:
// width ve height parametreleri GLsizei (imzalı 32 bit tamsayı) olduğundan, bu parametrelerin
// alabileceği kritik sınır/uç değerler tek bir test içinde art arda denenir:
// INT_MIN -> mutlak alt sınır, kesinlikle geçersiz
// -1 -> en yaygın geçersiz negatif değer
// 0 -> geçerli, sıfır boyutlu sınır durumu
// 1 -> en küçük geçerli pozitif değer
// GL_MAX_RENDERBUFFER_SIZE -> izin verilen üst sınırın tam değeri, geçerli olmalı
// GL_MAX_RENDERBUFFER_SIZE + 1 -> izin verilen üst sınırın bir fazlası, geçersiz olmalı
// INT_MAX -> mutlak üst sınır, implementasyonun çökmeden bir hata
// üretmesi beklenir
// Her adımda implementasyonun çökmediği ve beklenen hata sınıflandırmasının (geçerli/
// geçersiz) doğru yapıldığı kontrol edilir; amaç sınır değerlerde kararlılığı sınamaktır.
void GS_GL20SC_FO_RS_ROBUSTNESS_TC_013()
{
    while (glGetError() != GL_NO_ERROR) {}

    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxSize);
    while (glGetError() != GL_NO_ERROR) {}

    GLuint rbo = 0;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    while (glGetError() != GL_NO_ERROR) {}

    const GLsizei boundaryValues[] = {INT_MIN, -1, 0, 1, maxSize, maxSize + 1, INT_MAX};
    const size_t boundaryCount = sizeof(boundaryValues) / sizeof(boundaryValues[0]);

    int failedIndex = -1;

    for (size_t i = 0; i < boundaryCount; ++i) {
        while (glGetError() != GL_NO_ERROR) {}

        // Her sınır değer hem width hem de height için aynı anda denenir.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, boundaryValues[i], boundaryValues[i]);
        GLenum err = glGetError();

        int shouldBeInvalid = (boundaryValues[i] < 0) || (boundaryValues[i] > maxSize);

        if (shouldBeInvalid && err != GL_INVALID_VALUE) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_13, test_procedure, "Expected GL_INVALID_VALUE for boundary value %d, got 0x%x.", boundaryValues[i], err);
            break;
        }
        else if (!shouldBeInvalid && err != GL_NO_ERROR && err != GL_OUT_OF_MEMORY) {
            failedIndex = (int)i;
            TEST_LOG_FAIL(test_case_13, test_procedure, "Unexpected error 0x%x for valid boundary value %d.", err, boundaryValues[i]);
            break;
        }
    }
    if (failedIndex == -1) {
        TEST_LOG_SUCCESS(test_case_13, test_procedure);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


/* Initialization */
void GS_GL20SC_FO_RS_ROBUSTNESS_TP_001_init(void) {
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_001();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_002();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_003();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_004();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_005();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_006();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_007();  /* It failed on Windows */
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_008();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_009();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_010();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_011();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_012();
    // CHECK_ERROR(test_procedure);
    GS_GL20SC_FO_RS_ROBUSTNESS_TC_013();
    // CHECK_ERROR(test_procedure);
}

void GS_GL20SC_FO_RS_ROBUSTNESS_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_FO_RS_ROBUSTNESS_TP_001_close(void) {
    // CHECK_ERROR(test_procedure);
}