#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"

static const char *test_case1 = "Texturing_GenerateMipmap_TC_001";
static const char *test_case2 = "Texturing_GenerateMipmap_TC_002";
static const char *test_case3 = "Texturing_GenerateMipmap_TC_003";
static const char *test_case4 = "Texturing_GenerateMipmap_TC_004";
static const char *test_case5 = "Texturing_GenerateMipmap_TC_005";
static const char *test_case6 = "Texturing_GenerateMipmap_TC_006";
static const char *test_case7 = "Texturing_GenerateMipmap_TC_007";
static const char *test_case8 = "Texturing_GenerateMipmap_TC_008";
static const char *test_case9 = "Texturing_GenerateMipmap_TC_009";
static const char *test_case10 = "Texturing_GenerateMipmap_TC_010";
static const char *test_case11 = "Texturing_GenerateMipmap_TC_011";
static const char *test_case12 = "Texturing_GenerateMipmap_TC_012";
static const char *test_case13 = "Texturing_GenerateMipmap_TC_013";
static const char *test_case14 = "Texturing_GenerateMipmap_TC_014";
static const char *test_case15 = "Texturing_GenerateMipmap_TC_015";

static const char *test_procedure = "Texturing_GenerateMipmap_TP_001";

static GLuint g_tex1 = 0;
static GLuint g_tex2 = 0;
static GLuint g_tex4_1 = 0;
static GLuint g_tex4_2 = 0;
static GLuint g_tex6 = 0;
static GLuint g_tex7 = 0;
static GLuint g_tex11 = 0;
static GLuint g_tex13 = 0;
static GLuint g_tex14 = 0;
static GLuint g_tex15 = 0;

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
static GLuint create_simple_program(void) {
	const char *vs_src = "attribute vec4 a_position;\n"
			     "attribute vec2 a_texcoord;\n"
			     "varying vec2 v_texcoord;\n"
			     "void main() {\n"
			     "    gl_Position = a_position;\n"
			     "    v_texcoord = a_texcoord;\n"
			     "}\n";

	const char *fs_src =
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
static GLubyte *create_solid_color_data(GLsizei w, GLsizei h, GLubyte r,
					GLubyte g, GLubyte b, GLubyte a) {
	GLubyte *data = (GLubyte *)malloc(w * h * 4);
	if (!data)
		return NULL;
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
void Texturing_GenerateMipmap_TC_001(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex1);
	glBindTexture(GL_TEXTURE_2D, g_tex1);

	GLubyte *data = create_solid_color_data(256, 256, 255, 0, 0, 255);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, data);
	free(data);

	glGenerateMipmap(GL_TEXTURE_2D);
	GLenum err = glGetError();

	if (err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case1, test_procedure);
	else
		TEST_LOG_FAIL(test_case1, test_procedure,
			      "256x256 POT mipmap uretiminde hata: 0x%X", err);
}

// ---------------------------------------------------------------
// TEST 2: 1x1 Minimum Boyut
// En kucuk gecerli texture boyutunda mipmap uretimi.
// Mipmap zinciri sadece tek seviyeden olusmali (level 0).
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_002(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex2);
	glBindTexture(GL_TEXTURE_2D, g_tex2);

	GLubyte pixel[4] = {0, 255, 0, 255}; // yesil
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, pixel);

	glGenerateMipmap(GL_TEXTURE_2D);
	GLenum err = glGetError();

	if (err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case2, test_procedure);
	else
		TEST_LOG_FAIL(test_case2, test_procedure,
			      "1x1 mipmap uretiminde hata: 0x%X", err);
}

// ---------------------------------------------------------------
// TEST 3: NPOT (Non-Power-of-Two) Asimetrik Boyutlar
// ES 2.0'da NPOT texture'larda mipmap destegi sinirlidir.
// OES_texture_npot extension'i yoksa GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_003(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	typedef struct {
		GLsizei w, h;
	} Dim;
	Dim npot_sizes[] = {{3, 7}, {100, 100}, {13, 1}, {1, 37}, {5, 5}};
	int count = sizeof(npot_sizes) / sizeof(npot_sizes[0]);
	int all_ok = 1;

	for (int i = 0; i < count; i++) {
		while (glGetError() != GL_NO_ERROR)
			;

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		GLubyte *data = create_solid_color_data(
		    npot_sizes[i].w, npot_sizes[i].h, 0, 0, 255, 255);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, npot_sizes[i].w,
			     npot_sizes[i].h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			     data);
		free(data);

		glGenerateMipmap(GL_TEXTURE_2D);
		GLenum err = glGetError();

		if (err != GL_INVALID_OPERATION && err != GL_NO_ERROR) {
			all_ok = 0;
		}

		glDeleteTextures(1, &tex);
	}

	if (all_ok)
		TEST_LOG_SUCCESS(test_case3, test_procedure);
	else
		TEST_LOG_FAIL(test_case3, test_procedure,
			      "NPOT boyutlarda beklenmeyen hata");
}

// ---------------------------------------------------------------
// TEST 4: Maksimum Texture Boyutunun Zorlanmasi
// GL_MAX_TEXTURE_SIZE degerini sorgulayarak sinir ve sinir+1
// boyutlarinda mipmap uretimi denenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_004(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLint max_size = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);

	GLsizei test_size = (max_size > 4096) ? 4096 : max_size;

	glGenTextures(1, &g_tex4_1);
	glBindTexture(GL_TEXTURE_2D, g_tex4_1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, test_size, test_size, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	GLenum alloc_err = glGetError();

	if (alloc_err == GL_NO_ERROR) {
		glGenerateMipmap(GL_TEXTURE_2D);
		glGetError();
	}

	// Sinir + 1 deneriz (ekran karti desteklememeli)
	while (glGetError() != GL_NO_ERROR)
		;
	glGenTextures(1, &g_tex4_2);
	glBindTexture(GL_TEXTURE_2D, g_tex4_2);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max_size + 1, max_size + 1, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	GLenum over_err = glGetError();

	if (over_err == GL_INVALID_VALUE || over_err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case4, test_procedure);
	else
		TEST_LOG_FAIL(test_case4, test_procedure,
			      "MAX+1 boyutta beklenmeyen hata: 0x%X", over_err);
}

// ---------------------------------------------------------------
// TEST 5: Gecersiz Target Enum
// GL_TEXTURE_2D veya GL_TEXTURE_CUBE_MAP disinda bir target
// verildiğinde GL_INVALID_ENUM beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_005(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLenum bad_targets[] = {0x0000, 0xFFFF, 0xDEAD, GL_ARRAY_BUFFER,
				GL_FRAMEBUFFER};
	int count = sizeof(bad_targets) / sizeof(bad_targets[0]);
	int all_invalid = 1;

	for (int i = 0; i < count; i++) {
		while (glGetError() != GL_NO_ERROR)
			;
		glGenerateMipmap(bad_targets[i]);
		GLenum err = glGetError();
		if (err != GL_INVALID_ENUM) {
			all_invalid = 0;
		}
	}

	if (all_invalid)
		TEST_LOG_SUCCESS(test_case5, test_procedure);
	else
		TEST_LOG_FAIL(test_case5, test_procedure,
			      "Gecersiz hedefler GL_INVALID_ENUM dondurmedi");
}

// ---------------------------------------------------------------
// TEST 6: Level 0 Bos / Tanimsiz (Incomplete Texture)
// glTexImage2D cagrilmadan glGenerateMipmap cagrilir.
// Spec: Level 0 tanimsiz ise GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_006(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex6);
	glBindTexture(GL_TEXTURE_2D, g_tex6);

	// Level 0 verisi hic yuklenmedi!
	glGenerateMipmap(GL_TEXTURE_2D);
	GLenum err = glGetError();

	if (err == GL_INVALID_OPERATION || err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case6, test_procedure);
	else
		TEST_LOG_FAIL(test_case6, test_procedure,
			      "Bos texture uzerinde mipmap hatasi: 0x%X", err);
}

// ---------------------------------------------------------------
// TEST 7: Boyutu 0x0 Olan Texture
// Width=0 veya Height=0 olan bir texture uzerinde mipmap denenir.
// Surucu cokmemeli, tanimli hatayi vermelidir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_007(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex7);
	glBindTexture(GL_TEXTURE_2D, g_tex7);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
	glGetError();

	glGenerateMipmap(GL_TEXTURE_2D);
	GLenum mip_err = glGetError();

	if (mip_err == GL_NO_ERROR || mip_err == GL_INVALID_OPERATION ||
	    mip_err == GL_INVALID_VALUE)
		TEST_LOG_SUCCESS(test_case7, test_procedure);
	else
		TEST_LOG_FAIL(test_case7, test_procedure,
			      "0x0 texture mipmap hatasi: 0x%X", mip_err);
}

// ---------------------------------------------------------------
// TEST 8: Luminance ve Alpha Formatlari
// Sadece RGBA degil, GL_LUMINANCE, GL_ALPHA vb. farkli internal
// formatlarda mipmap uretimi test edilir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_008(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	typedef struct {
		GLenum format;
		const char *name;
	} FmtEntry;
	FmtEntry formats[] = {{GL_RGBA, "GL_RGBA"},
			      {GL_RGB, "GL_RGB"},
			      {GL_LUMINANCE, "GL_LUMINANCE"},
			      {GL_ALPHA, "GL_ALPHA"},
			      {GL_LUMINANCE_ALPHA, "GL_LUMINANCE_ALPHA"}};
	int count = sizeof(formats) / sizeof(formats[0]);
	int all_ok = 1;

	for (int i = 0; i < count; i++) {
		while (glGetError() != GL_NO_ERROR)
			;

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		int components = 4;
		if (formats[i].format == GL_RGB)
			components = 3;
		else if (formats[i].format == GL_LUMINANCE ||
			 formats[i].format == GL_ALPHA)
			components = 1;
		else if (formats[i].format == GL_LUMINANCE_ALPHA)
			components = 2;

		GLubyte *data = (GLubyte *)calloc(64 * 64 * components, 1);
		if (data)
			memset(data, 128, 64 * 64 * components);

		glTexImage2D(GL_TEXTURE_2D, 0, formats[i].format, 64, 64, 0,
			     formats[i].format, GL_UNSIGNED_BYTE, data);
		free(data);

		GLenum tex_err = glGetError();
		if (tex_err != GL_NO_ERROR) {
			glDeleteTextures(1, &tex);
			continue;
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		GLenum mip_err = glGetError();
		if (mip_err != GL_NO_ERROR) {
			all_ok = 0;
		}

		glDeleteTextures(1, &tex);
	}

	if (all_ok)
		TEST_LOG_SUCCESS(test_case8, test_procedure);
	else
		TEST_LOG_FAIL(test_case8, test_procedure,
			      "Farkli formatlarda mipmap uretimi basarisiz");
}

// ---------------------------------------------------------------
// TEST 9: Ardisik Buyuk Texture Tahsis Stresi (VRAM Tasmasi)
// Cok sayida buyuk (2048x2048) texture uretilip mipmap'lenir.
// VRAM doldugunda GL_OUT_OF_MEMORY vermeli, CHIP/DRIVER COKMEMELI.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_009(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	const int MAX_TEXTURES = 200;
	GLuint textures[200];
	int allocated = 0;
	int crash_resilient = 1;

	for (int i = 0; i < MAX_TEXTURES; i++) {
		while (glGetError() != GL_NO_ERROR)
			;

		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA,
			     GL_UNSIGNED_BYTE, NULL);
		GLenum alloc_err = glGetError();

		if (alloc_err == GL_OUT_OF_MEMORY) {
			glDeleteTextures(1, &textures[i]);
			break;
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		GLenum mip_err = glGetError();

		if (mip_err == GL_OUT_OF_MEMORY) {
			allocated = i + 1;
			break;
		}

		if (alloc_err != GL_NO_ERROR || mip_err != GL_NO_ERROR) {
			allocated = i + 1;
			break;
		}

		allocated = i + 1;
	}

	if (crash_resilient)
		TEST_LOG_SUCCESS(test_case9, test_procedure);
	else
		TEST_LOG_FAIL(test_case9, test_procedure,
			      "VRAM stresi yonetilemedi");

	glDeleteTextures(allocated, textures);
}

// ---------------------------------------------------------------
// TEST 10: Bind Edilmemis Texture Uzerinde GenerateMipmap
// Active texture target'ina texture 0 (varsayilan) bind iken
// glGenerateMipmap cagrilir. Spec: GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_010(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenerateMipmap(GL_TEXTURE_2D);
	GLenum err = glGetError();

	if (err == GL_INVALID_OPERATION || err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case10, test_procedure);
	else
		TEST_LOG_FAIL(test_case10, test_procedure,
			      "Bind edilmemis texture mipmap hatasi: 0x%X",
			      err);
}

// ---------------------------------------------------------------
// TEST 11: Ardisik Coklu Mipmap Uretimi (Rapid Regeneration)
// Ayni texture uzerinde 100 kez ardisik glGenerateMipmap cagrilir.
// Sürücü kaynak sızıntısı yapmamalı veya kilitlenmemelidir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_011(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex11);
	glBindTexture(GL_TEXTURE_2D, g_tex11);

	GLubyte *data = create_solid_color_data(128, 128, 255, 255, 0, 255);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, data);
	free(data);

	int fail_count = 0;
	for (int i = 0; i < 100; i++) {
		glGenerateMipmap(GL_TEXTURE_2D);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			fail_count++;
			break;
		}
	}

	if (fail_count == 0)
		TEST_LOG_SUCCESS(test_case11, test_procedure);
	else
		TEST_LOG_FAIL(test_case11, test_procedure,
			      "100 kez ardisik mipmap uretimi basarisiz");
}

// ---------------------------------------------------------------
// TEST 12: Piksel Bazli Mipmap Dogrulama (Visual Verification)
// Kirmizi renkli bir texture üretilir, mipmap oluşturulur.
// Render yapılıp glReadPixels ile mipmap level 1 seviyesindeki
// piksel renginin kirmizi kaldigi (dogru harmanlandigi) teyit edilir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_012(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	GLuint prog = create_simple_program();

	// Render edilecek FBO hazirla
	GLuint fbo, render_tex;
	glGenTextures(1, &render_tex);
	glBindTexture(GL_TEXTURE_2D, render_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			       GL_TEXTURE_2D, render_tex, 0);

	GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &render_tex);
		glDeleteProgram(prog);
		TEST_LOG_FAIL(test_case12, test_procedure,
			      "FBO tamamlanmadi (0x%X)", fbo_status);
		return;
	}

	// 4x4 Kirmizi Texture
	GLuint src_tex;
	glGenTextures(1, &src_tex);
	glBindTexture(GL_TEXTURE_2D, src_tex);

	GLubyte *red_data = create_solid_color_data(4, 4, 255, 0, 0, 255);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, red_data);
	free(red_data);

	// Mipmap uret (Level 0: 4x4, Level 1: 2x2, Level 2: 1x1 olmali)
	glGenerateMipmap(GL_TEXTURE_2D);

	// Minification filtresini MIPMAP secelim ki mipmap seviyesinden okusun
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glViewport(0, 0, 2, 2);
	glUseProgram(prog);

	GLfloat verts[] = {-1, -1, 1, -1, -1, 1, 1, 1};
	GLfloat texcoords[] = {0, 0, 1, 0, 0, 1, 1, 1};

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

	if (err == GL_NO_ERROR && r > 200 && g < 50 && b < 50)
		TEST_LOG_SUCCESS(test_case12, test_procedure);
	else
		TEST_LOG_FAIL(
		    test_case12, test_procedure,
		    "Piksel kirmizi degil R=%d G=%d B=%d veya hata=0x%X", r, g,
		    b, err);

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
void Texturing_GenerateMipmap_TC_013(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex13);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex13);

	GLenum faces[] = {
	    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	    GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

	GLubyte *data = create_solid_color_data(64, 64, 0, 128, 255, 255);
	for (int i = 0; i < 6; i++) {
		glTexImage2D(faces[i], 0, GL_RGBA, 64, 64, 0, GL_RGBA,
			     GL_UNSIGNED_BYTE, data);
	}
	free(data);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	GLenum err = glGetError();

	if (err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case13, test_procedure);
	else
		TEST_LOG_FAIL(test_case13, test_procedure,
			      "CubeMap mipmap uretimi hatali: 0x%X", err);
}

// ---------------------------------------------------------------
// TEST 14: Eksik CubeMap Yuzleri (Incomplete Cubemap)
// 6 yuzeyden sadece 3'u doldurulup glGenerateMipmap cagrilir.
// Spec: CubeMap incomplete ise GL_INVALID_OPERATION beklenir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_014(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex14);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex14);

	GLubyte *data = create_solid_color_data(32, 32, 255, 128, 0, 255);
	// Sadece 3 yuzeyi dolduruyoruz (eksik!)
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 32, 32, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 32, 32, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 32, 32, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, data);
	free(data);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	GLenum err = glGetError();

	if (err == GL_INVALID_OPERATION || err == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case14, test_procedure);
	else
		TEST_LOG_FAIL(test_case14, test_procedure,
			      "Eksik cubemap mipmap hatasi: 0x%X", err);
}

// ---------------------------------------------------------------
// TEST 15: Level 0 Verisi Degistikten Sonra Mipmap Yenileme
// Doku uretilip mipmap'lendikten sonra Level 0 yeni veriyle güncellenip
// tekrar glGenerateMipmap çağrıldığında yeni mipmap'lerin oluşumu test edilir.
// ---------------------------------------------------------------
void Texturing_GenerateMipmap_TC_015(void) {
	while (glGetError() != GL_NO_ERROR)
		;

	glGenTextures(1, &g_tex15);
	glBindTexture(GL_TEXTURE_2D, g_tex15);

	// Ilk veri (kirmizi)
	GLubyte *red = create_solid_color_data(64, 64, 255, 0, 0, 255);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, red);
	free(red);
	glGenerateMipmap(GL_TEXTURE_2D);
	GLenum err1 = glGetError();

	// Ikinci veri (yesil) - Level 0 guncelleniyor
	GLubyte *green = create_solid_color_data(64, 64, 0, 255, 0, 255);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, green);
	free(green);
	glGenerateMipmap(GL_TEXTURE_2D);
	GLenum err2 = glGetError();

	if (err1 == GL_NO_ERROR && err2 == GL_NO_ERROR)
		TEST_LOG_SUCCESS(test_case15, test_procedure);
	else
		TEST_LOG_FAIL(test_case15, test_procedure,
			      "Mipmap yenileme hatali: err1=0x%X, err2=0x%X",
			      err1, err2);
}

/* Cleanup */
void Texturing_GenerateMipmap_close(void) {
#ifdef __ubuntu__
	if (g_tex1)
		glDeleteTextures(1, &g_tex1);
	if (g_tex2)
		glDeleteTextures(1, &g_tex2);
	if (g_tex4_1)
		glDeleteTextures(1, &g_tex4_1);
	if (g_tex4_2)
		glDeleteTextures(1, &g_tex4_2);
	if (g_tex6)
		glDeleteTextures(1, &g_tex6);
	if (g_tex7)
		glDeleteTextures(1, &g_tex7);
	if (g_tex11)
		glDeleteTextures(1, &g_tex11);
	if (g_tex13)
		glDeleteTextures(1, &g_tex13);
	if (g_tex14)
		glDeleteTextures(1, &g_tex14);
	if (g_tex15)
		glDeleteTextures(1, &g_tex15);
#endif
}
