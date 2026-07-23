#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

static const char* test_procedure = "Texturing_GenerateMipmap_TP_001";

/*
 * ============================================================================
 * glGenerateMipmap Robustness Test Suite
 * Hasan - OpenGL ES 2.0
 *
 * dEQP, Piglit ve WebGL CTS test paketlerindeki yaklasimlardan
 * esinlenerek yazilmis kapsamli saglamlik (robustness) test paketi.
 *
 * Test edilen parametreler:
 *   - Uc boyutlar (1x1, NPOT, MAX_TEXTURE_SIZE, MAX+1)
 *   - Yasak/desteklenmeyen formatlar
 *   - Bellek sinirlari (VRAM tasmasi)
 *   - Eksik seviyeler / bos level 0
 *   - Piksel bazli gorsel dogrulama (glReadPixels)
 *   - Cokme guvenligi (crash resilience)
 * ============================================================================
 */

// Yardimci: basit bir shader programi olusturur (piksel dogrulama icin)
static GLuint create_simple_program(void)
{
    const char* vs_src =
        "attribute vec4 a_position;\n"
        "attribute vec2 a_texcoord;\n"
        "varying vec2 v_texcoord;\n"
        "void main() {\n"
        "    gl_Position = a_position;\n"
        "    v_texcoord = a_texcoord;\n"
        "}\n";

    const char* fs_src =
        "precision mediump float;\n"
        "varying vec2 v_texcoord;\n"
        "uniform sampler2D u_texture;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(u_texture, v_texcoord);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(fs);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glBindAttribLocation(prog, 0, "a_position");
    glBindAttribLocation(prog, 1, "a_texcoord");
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// Yardimci: tek renkli piksel datasi olusturur (RGBA)
static GLubyte* create_solid_color_data(GLsizei w, GLsizei h, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    GLubyte* data = (GLubyte*)malloc(w * h * 4);
    if (!data) return NULL;
    for (GLsizei i = 0; i < w * h; i++) {
        data[i * 4 + 0] = r;
        data[i * 4 + 1] = g;
        data[i * 4 + 2] = b;
        data[i * 4 + 3] = a;
    }
    return data;
}

// ---------------------------------------------------------------
// TEST 1: Standart 2'nin Kuvveti Boyut (Power-of-Two)
// 256x256 RGBA texture uzerinde mipmap uretimi.
// Baseline test: her sey dogru ise hata olmamali.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_001(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_001";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_pot_baseline (256x256)\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLubyte* data = create_solid_color_data(256, 256, 255, 0, 0, 255);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);

    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum err = glGetError();

    if (err == GL_NO_ERROR)
        printf("  -> PASSED: 256x256 POT mipmap uretimi basarili (0x0)\n\n");
    else
        printf("  -> FAILED: Beklenen 0x0, alinan 0x%X\n\n", err);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 2: 1x1 Minimum Boyut
// En kucuk gecerli texture boyutunda mipmap uretimi.
// Mipmap zinciri sadece tek seviyeden olusmali (level 0).
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_002(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_002";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_1x1_minimum\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLubyte pixel[4] = {0, 255, 0, 255}; // yesil
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum err = glGetError();

    if (err == GL_NO_ERROR)
        printf("  -> PASSED: 1x1 mipmap uretimi sorunsuz (tek seviye)\n\n");
    else
        printf("  -> FAILED: Hata: 0x%X\n\n", err);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 3: NPOT (Non-Power-of-Two) Asimetrik Boyutlar
// ES 2.0'da NPOT texture'larda mipmap destegi sinirlidir.
// OES_texture_npot extension'i yoksa GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_003(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_003";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_npot_dimensions\n", test_procedure, test_case);

    typedef struct { GLsizei w, h; } Dim;
    Dim npot_sizes[] = { {3, 7}, {100, 100}, {13, 1}, {1, 37}, {5, 5} };
    int count = sizeof(npot_sizes) / sizeof(npot_sizes[0]);

    for (int i = 0; i < count; i++) {
        while (glGetError() != GL_NO_ERROR);

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        GLubyte* data = create_solid_color_data(npot_sizes[i].w, npot_sizes[i].h, 0, 0, 255, 255);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     npot_sizes[i].w, npot_sizes[i].h,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        free(data);

        glGenerateMipmap(GL_TEXTURE_2D);
        GLenum err = glGetError();

        printf("  %dx%d -> hata: 0x%X %s\n",
               npot_sizes[i].w, npot_sizes[i].h, err,
               (err == GL_INVALID_OPERATION) ? "(GL_INVALID_OPERATION - NPOT mipmap yasak)" :
               (err == GL_NO_ERROR) ? "(Basarili - NPOT extension aktif)" :
               "(Beklenmeyen hata!)");

        glDeleteTextures(1, &tex);
    }
    printf("\n");
}

// ---------------------------------------------------------------
// TEST 4: Maksimum Texture Boyutunun Zorlanmasi
// GL_MAX_TEXTURE_SIZE degerini sorgulayarak sinir ve sinir+1
// boyutlarinda mipmap uretimi denenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_004(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_004";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_max_texture_size\n", test_procedure, test_case);

    GLint max_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
    printf("  GL_MAX_TEXTURE_SIZE = %d\n", max_size);

    // Sinir boyutunda (max x max) - cok buyuk olabilir, kucuk deneyelim
    GLsizei test_size = (max_size > 4096) ? 4096 : max_size;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, test_size, test_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GLenum alloc_err = glGetError();

    if (alloc_err == GL_NO_ERROR) {
        glGenerateMipmap(GL_TEXTURE_2D);
        GLenum mip_err = glGetError();
        printf("  %dx%d tahsis: basarili, mipmap: 0x%X\n", test_size, test_size, mip_err);
    } else {
        printf("  %dx%d tahsis: BASARISIZ (0x%X) - bellek yetersiz olabilir\n", test_size, test_size, alloc_err);
    }

    glDeleteTextures(1, &tex);

    // Sinir + 1 deneriz (ekran karti desteklememeli)
    while (glGetError() != GL_NO_ERROR);
    GLuint tex2;
    glGenTextures(1, &tex2);
    glBindTexture(GL_TEXTURE_2D, tex2);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max_size + 1, max_size + 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GLenum over_err = glGetError();
    printf("  %dx%d (MAX+1) tahsis hatasi: 0x%X (GL_INVALID_VALUE beklenir)\n\n",
           max_size + 1, max_size + 1, over_err);

    glDeleteTextures(1, &tex2);
}

// ---------------------------------------------------------------
// TEST 5: Gecersiz Target Enum
// GL_TEXTURE_2D veya GL_TEXTURE_CUBE_MAP disinda bir target
// verildiğinde GL_INVALID_ENUM beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_005(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_005";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_invalid_target\n", test_procedure, test_case);

    GLenum bad_targets[] = { 0x0000, 0xFFFF, 0xDEAD, GL_ARRAY_BUFFER, GL_FRAMEBUFFER };
    int count = sizeof(bad_targets) / sizeof(bad_targets[0]);

    for (int i = 0; i < count; i++) {
        while (glGetError() != GL_NO_ERROR);
        glGenerateMipmap(bad_targets[i]);
        GLenum err = glGetError();

        printf("  target=0x%04X -> hata: 0x%X %s\n",
               bad_targets[i], err,
               (err == GL_INVALID_ENUM) ? "(GL_INVALID_ENUM - Beklenen)" : "(Beklenmeyen!)");
    }
    printf("\n");
}

// ---------------------------------------------------------------
// TEST 6: Level 0 Bos / Tanimsiz (Incomplete Texture)
// glTexImage2D cagrilmadan glGenerateMipmap cagrilir.
// Spec: Level 0 tanimsiz ise GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_006(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_006";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_empty_level_zero\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Level 0 verisi hic yuklenmedi!
    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum err = glGetError();

    printf("  Bos texture uzerinde mipmap: hata=0x%X %s\n\n",
           err,
           (err == GL_INVALID_OPERATION) ? "(GL_INVALID_OPERATION - Beklenen)" :
           (err == GL_NO_ERROR) ? "(Hata yok - surucu toleransli)" :
           "(Farkli hata)");

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 7: Boyutu 0x0 Olan Texture
// Width=0 veya Height=0 olan bir texture uzerinde mipmap denenir.
// Surucu cokmemeli, tanimli hatayi vermelidir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_007(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_007";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_zero_dimensions\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GLenum tex_err = glGetError();

    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum mip_err = glGetError();

    printf("  0x0 TexImage2D hatasi: 0x%X\n", tex_err);
    printf("  0x0 GenerateMipmap hatasi: 0x%X\n", mip_err);
    printf("  Surucu cokmedi -> kararli\n\n");

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 8: Luminance ve Alpha Formatlari
// Sadece RGBA degil, GL_LUMINANCE, GL_ALPHA vb. farkli internal
// formatlarda mipmap uretimi test edilir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_008(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_008";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_special_formats\n", test_procedure, test_case);

    typedef struct { GLenum format; const char* name; } FmtEntry;
    FmtEntry formats[] = {
        { GL_RGBA,            "GL_RGBA" },
        { GL_RGB,             "GL_RGB" },
        { GL_LUMINANCE,       "GL_LUMINANCE" },
        { GL_ALPHA,           "GL_ALPHA" },
        { GL_LUMINANCE_ALPHA, "GL_LUMINANCE_ALPHA" }
    };
    int count = sizeof(formats) / sizeof(formats[0]);

    for (int i = 0; i < count; i++) {
        while (glGetError() != GL_NO_ERROR);

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        int components = 4;
        if (formats[i].format == GL_RGB) components = 3;
        else if (formats[i].format == GL_LUMINANCE || formats[i].format == GL_ALPHA) components = 1;
        else if (formats[i].format == GL_LUMINANCE_ALPHA) components = 2;

        GLubyte* data = (GLubyte*)calloc(64 * 64 * components, 1);
        memset(data, 128, 64 * 64 * components);

        glTexImage2D(GL_TEXTURE_2D, 0, formats[i].format, 64, 64, 0,
                     formats[i].format, GL_UNSIGNED_BYTE, data);
        free(data);

        GLenum tex_err = glGetError();
        if (tex_err != GL_NO_ERROR) {
            printf("  %s: TexImage2D hatasi 0x%X (atlanıyor)\n", formats[i].name, tex_err);
            glDeleteTextures(1, &tex);
            continue;
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        GLenum mip_err = glGetError();

        printf("  %s: mipmap hatasi=0x%X %s\n",
               formats[i].name, mip_err,
               (mip_err == GL_NO_ERROR) ? "(Basarili)" : "(Reddedildi)");

        glDeleteTextures(1, &tex);
    }
    printf("\n");
}

// ---------------------------------------------------------------
// TEST 9: Ardisik Buyuk Texture Tahsis Stresi (VRAM Tasmasi)
// Cok sayida buyuk (2048x2048) texture uretilip mipmap'lenir.
// VRAM doldugunda GL_OUT_OF_MEMORY vermeli, CHIP/DRIVER COKMEMELI.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_009(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_009";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_vram_exhaustion\n", test_procedure, test_case);

    const int MAX_TEXTURES = 200;
    GLuint textures[200];
    int allocated = 0;

    for (int i = 0; i < MAX_TEXTURES; i++) {
        while (glGetError() != GL_NO_ERROR);

        glGenTextures(1, &textures[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        GLenum alloc_err = glGetError();

        if (alloc_err == GL_OUT_OF_MEMORY) {
            printf("  %d. texture'da GL_OUT_OF_MEMORY alindi (beklenen davranis)\n", i + 1);
            glDeleteTextures(1, &textures[i]);
            break;
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        GLenum mip_err = glGetError();

        if (mip_err == GL_OUT_OF_MEMORY) {
            printf("  %d. mipmap'te GL_OUT_OF_MEMORY alindi\n", i + 1);
            allocated = i + 1;
            break;
        }

        if (alloc_err != GL_NO_ERROR || mip_err != GL_NO_ERROR) {
            printf("  %d. texture'da beklenmeyen hata: alloc=0x%X, mip=0x%X\n",
                   i + 1, alloc_err, mip_err);
            allocated = i + 1;
            break;
        }

        allocated = i + 1;
    }

    printf("  Toplam basariyla tahsis edilen texture: %d / %d\n", allocated, MAX_TEXTURES);
    printf("  Surucu VRAM stresini cokme olmaksizin yonetti\n\n");

    glDeleteTextures(allocated, textures);
}

// ---------------------------------------------------------------
// TEST 10: Bind Edilmemis Texture Uzerinde GenerateMipmap
// Active texture target'ina texture 0 (varsayilan) bind iken
// glGenerateMipmap cagrilir. Spec: GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_010(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_010";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_no_texture_bound\n", test_procedure, test_case);

    glBindTexture(GL_TEXTURE_2D, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum err = glGetError();

    printf("  Bind edilmemis texture uzerinde mipmap: hata=0x%X %s\n\n",
           err,
           (err == GL_INVALID_OPERATION) ? "(GL_INVALID_OPERATION - Beklenen)" :
           (err == GL_NO_ERROR) ? "(Hata yok - varsayilan texture'a etki)" :
           "(Farkli hata)");
}

// ---------------------------------------------------------------
// TEST 11: Ardisik Coklu Mipmap Uretimi (Rapid Regeneration)
// Ayni texture uzerinde 100 kez ardisik glGenerateMipmap cagrilir.
// Sürücü kaynak sızıntısı yapmamalı veya kilitlenmemelidir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_011(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_011";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_rapid_regeneration\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLubyte* data = create_solid_color_data(128, 128, 255, 255, 0, 255);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);

    int fail_count = 0;
    for (int i = 0; i < 100; i++) {
        glGenerateMipmap(GL_TEXTURE_2D);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("  %d. iterasyonda hata: 0x%X\n", i + 1, err);
            fail_count++;
            break;
        }
    }

    if (fail_count == 0)
        printf("  -> PASSED: 100 kez ardisik mipmap uretimi hatasiz tamamlandi\n\n");
    else
        printf("  -> FAILED\n\n");

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 12: Piksel Bazli Mipmap Dogrulama (Visual Verification)
// Kirmizi renkli bir texture üretilir, mipmap oluşturulur.
// Render yapılıp glReadPixels ile mipmap level 1 seviyesindeki
// piksel renginin kirmizi kaldigi (dogru harmanlandigi) teyit edilir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_012(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_012";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_pixel_verification\n", test_procedure, test_case);

    GLuint prog = create_simple_program();

    // Render edilecek FBO hazirla
    GLuint fbo, render_tex;
    glGenTextures(1, &render_tex);
    glBindTexture(GL_TEXTURE_2D, render_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_tex, 0);

    GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        printf("  FBO tamamlanmadi (0x%X), test atlaniyor\n\n", fbo_status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &render_tex);
        glDeleteProgram(prog);
        return;
    }

    // 4x4 Kirmizi Texture
    GLuint src_tex;
    glGenTextures(1, &src_tex);
    glBindTexture(GL_TEXTURE_2D, src_tex);

    GLubyte* red_data = create_solid_color_data(4, 4, 255, 0, 0, 255);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, red_data);
    free(red_data);

    // Mipmap uret (Level 0: 4x4, Level 1: 2x2, Level 2: 1x1 olmali)
    glGenerateMipmap(GL_TEXTURE_2D);

    // Minification filtresini MIPMAP secelim ki mipmap seviyesinden okusun
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glViewport(0, 0, 2, 2);
    glUseProgram(prog);

    GLfloat verts[] = { -1,-1, 1,-1, -1,1, 1,1 };
    GLfloat texcoords[] = { 0,0, 1,0, 0,1, 1,1 };

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 2x2 render sonucunu oku
    GLubyte pixels[4 * 4];
    glReadPixels(0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    GLenum err = glGetError();

    int r = pixels[0], g = pixels[1], b = pixels[2];
    printf("  Okunan piksel (0,0): R=%d G=%d B=%d\n", r, g, b);

    if (err == GL_NO_ERROR && r > 200 && g < 50 && b < 50)
        printf("  -> PASSED: Mipmap level 1 piksel dogrulamasi basarili (kirmizi)\n\n");
    else
        printf("  -> FAILED: Piksel beklenen renkte degil veya hata var (0x%X)\n\n", err);

    // Temizlik
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &render_tex);
    glDeleteTextures(1, &src_tex);
    glDeleteProgram(prog);
}

// ---------------------------------------------------------------
// TEST 13: CubeMap Uzerinde GenerateMipmap
// GL_TEXTURE_CUBE_MAP target'i ve 6 yuzeyinin tamami uretildikten
// sonra glGenerateMipmap cagirilarak cubemap destegi test edilir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_013(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_013";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_cubemap\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    GLenum faces[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    GLubyte* data = create_solid_color_data(64, 64, 0, 128, 255, 255);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(faces[i], 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    free(data);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    GLenum err = glGetError();

    if (err == GL_NO_ERROR)
        printf("  -> PASSED: CubeMap (6 yuz, 64x64) mipmap uretimi basarili\n\n");
    else
        printf("  -> FAILED: Hata: 0x%X\n\n", err);

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 14: Eksik CubeMap Yuzleri (Incomplete Cubemap)
// 6 yuzeyden sadece 3'u doldurulup glGenerateMipmap cagrilir.
// Spec: CubeMap incomplete ise GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_014(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_014";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_incomplete_cubemap\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    GLubyte* data = create_solid_color_data(32, 32, 255, 128, 0, 255);
    // Sadece 3 yuzeyi dolduruyoruz (eksik!)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    GLenum err = glGetError();

    printf("  Eksik cubemap uzerinde mipmap: hata=0x%X %s\n\n",
           err,
           (err == GL_INVALID_OPERATION) ? "(GL_INVALID_OPERATION - Beklenen)" :
           (err == GL_NO_ERROR) ? "(Surucu toleransli davranmis)" :
           "(Farkli hata)");

    glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 15: Level 0 Verisi Degistikten Sonra Mipmap Yenileme
// Doku uretilip mipmap'lendikten sonra Level 0 yeni veriyle güncellenip
// tekrar glGenerateMipmap çağrıldığında yeni mipmap'lerin oluşumu test edilir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_015(void)
{
    static const char* test_case = "Texturing_GenerateMipmap_TC_015";
    while (glGetError() != GL_NO_ERROR);
    printf("[TEST][%s][%s] test_mipmap_update_and_regenerate\n", test_procedure, test_case);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Ilk veri (kirmizi)
    GLubyte* red = create_solid_color_data(64, 64, 255, 0, 0, 255);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, red);
    free(red);
    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum err1 = glGetError();

    // Ikinci veri (yesil) - Level 0 guncelleniyor
    GLubyte* green = create_solid_color_data(64, 64, 0, 255, 0, 255);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, green);
    free(green);
    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum err2 = glGetError();

    if (err1 == GL_NO_ERROR && err2 == GL_NO_ERROR)
        printf("  -> PASSED: Level 0 guncelleme ve mipmap yenileme hatasiz\n\n");
    else
        printf("  -> FAILED: ilk=0x%X, ikinci=0x%X\n\n", err1, err2);

    glDeleteTextures(1, &tex);
}

// =========================================================
// ANA YAPI: init, draw, clean
// =========================================================

static GLFWwindow* window = NULL;
static int width = 640, height = 480;
static const char* windowTitle = "glGenerateMipmap Robustness Suite - Hasan";

void init(void);
void draw(void);
void clean(void);

void init(void)
{
    printf("=====================================================\n");
    printf("  %s ROBUSTNESS SUITE - HASAN\n", test_procedure);
    printf("=====================================================\n\n");

    Texturing_GenerateMipmap_TC_001();
    Texturing_GenerateMipmap_TC_002();
    Texturing_GenerateMipmap_TC_003();
    Texturing_GenerateMipmap_TC_004();
    Texturing_GenerateMipmap_TC_005();
    Texturing_GenerateMipmap_TC_006();
    Texturing_GenerateMipmap_TC_007();
    Texturing_GenerateMipmap_TC_008();
    Texturing_GenerateMipmap_TC_009();
    Texturing_GenerateMipmap_TC_010();
    Texturing_GenerateMipmap_TC_011();
    Texturing_GenerateMipmap_TC_012();
    Texturing_GenerateMipmap_TC_013();
    Texturing_GenerateMipmap_TC_014();
    Texturing_GenerateMipmap_TC_015();

    printf("=====================================================\n");
    printf("  TUM TESTLER TAMAMLANDI\n");
    printf("=====================================================\n");
}

void draw(void)
{
    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void clean(void)
{
}

int main(void)
{
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    setenv("GALLIUM_DRIVER", "llvmpipe", 1);
    unsetenv("WAYLAND_DISPLAY");

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    init();

    while (!glfwWindowShouldClose(window)) {
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    clean();
    glfwTerminate();
    return 0;
}
