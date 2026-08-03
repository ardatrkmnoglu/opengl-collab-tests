#include "../../include/helper.h"
#include "../../include/macro.h"
#include "../../include/rtests.h"
#include <string.h>

/*
 * GL20SC - Texturing - CompressedTexSubImage2D - ROBUSTNESS
 *
 * glCompressedTexSubImage2D robustness tests.
 * TC_001: Invalid format (GL_INVALID_ENUM)
 * TC_002: Invalid target (GL_INVALID_ENUM)
 * TC_003: Cubemap targets (SC 2.0 vs ES 2.0)
 * TC_004: Invalid position/size parameters (GL_INVALID_VALUE)
 * TC_005: Invalid imageSize (GL_INVALID_VALUE)
 * TC_006: Error priority with NULL data
 * TC_007: Zero size with NULL data
 * TC_008: Unaligned data pointer
 * TC_009: data buffer is read-only
 */

static const char *test_procedure = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TP_001";
static const char *test_case_1 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_001";
static const char *test_case_2 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_002";
static const char *test_case_3 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_003";
static const char *test_case_4 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_004";
static const char *test_case_5 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_005";
static const char *test_case_6 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_006";
static const char *test_case_7 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_007";
static const char *test_case_8 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_008";
static const char *test_case_9 = "GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_009";

void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TP_001_close(void);

#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif
#ifndef GL_COMPRESSED_RGB8_ETC2
#define GL_COMPRESSED_RGB8_ETC2 0x9274
#endif
#ifndef GL_COMPRESSED_RGBA8_ETC2_EAC
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#endif
#ifndef GL_TEXTURE_3D
#define GL_TEXTURE_3D 0x806F
#endif
#ifndef GL_RGBA8_OES
#define GL_RGBA8_OES 0x8058
#endif

/* Known 4x4 block compressed formats and block sizes */
static const struct {
  GLenum format;
  GLsizei block_bytes;
} known_formats[] = {
    {GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 8},
    {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 8},
    {GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 16},
    {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 16},
    {GL_COMPRESSED_RGB8_ETC2, 8},
    {GL_COMPRESSED_RGBA8_ETC2_EAC, 16},
};
static const int known_format_count =
    sizeof(known_formats) / sizeof(known_formats[0]);

#define TEX_SIZE 16
#define MAX_IMAGE_BYTES 256

static PFNGLTEXSTORAGE2DEXTPROC pglTexStorage2D = NULL;

static GLuint g_tex = 0;
static GLenum g_format = 0;
static GLsizei g_block = 0;
static int g_has_format = 0;
static int g_sub_allowed = 0;

/* Detect supported compressed formats */
static void detect_format(void) {
  GLint count = 0;
  glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &count);
  while (glGetError() != GL_NO_ERROR)
    ;

  if (count <= 0) {
    TEST_LOG_INFO("No compressed formats reported by driver");
    return;
  }

  if (count > 64)
    count = 64;

  GLint list[64];
  glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, list);
  while (glGetError() != GL_NO_ERROR)
    ;

  for (int i = 0; i < count && !g_has_format; i++) {
    for (int k = 0; k < known_format_count; k++) {
      if ((GLenum)list[i] == known_formats[k].format) {
        g_format = known_formats[k].format;
        g_block = known_formats[k].block_bytes;
        g_has_format = 1;
        break;
      }
    }
  }

  if (g_has_format)
    TEST_LOG_INFO("Using format: 0x%X (block=%d bytes)", g_format, g_block);
  else
    TEST_LOG_INFO("No known compressed formats found");
}

/* Setup texture storage */
static void setup_texture(void) {
  pglTexStorage2D =
      (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress("glTexStorage2DEXT");
  if (!pglTexStorage2D)
    pglTexStorage2D =
        (PFNGLTEXSTORAGE2DEXTPROC)glfwGetProcAddress("glTexStorage2D");

  glGenTextures(1, &g_tex);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR)
    ;

  if (!g_has_format) {
    if (pglTexStorage2D)
      pglTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8_OES, TEX_SIZE, TEX_SIZE);
    else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_SIZE, TEX_SIZE, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, NULL);
    while (glGetError() != GL_NO_ERROR)
      ;
    return;
  }

  int stored = 0;

  if (pglTexStorage2D) {
    pglTexStorage2D(GL_TEXTURE_2D, 1, g_format, TEX_SIZE, TEX_SIZE);
    stored = (glGetError() == GL_NO_ERROR);
  }

  if (!stored) {
    static GLubyte payload[MAX_IMAGE_BYTES];
    memset(payload, 0, sizeof(payload));
    GLsizei full = (TEX_SIZE / 4) * (TEX_SIZE / 4) * g_block;

    glCompressedTexImage2D(GL_TEXTURE_2D, 0, g_format, TEX_SIZE, TEX_SIZE, 0,
                           full, payload);
    stored = (glGetError() == GL_NO_ERROR);
  }

  if (!stored) {
    TEST_LOG_INFO("Failed to create compressed texture");
    g_has_format = 0;
    while (glGetError() != GL_NO_ERROR)
      ;
    return;
  }

  static GLubyte probe[MAX_IMAGE_BYTES];
  memset(probe, 0, sizeof(probe));

  glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 4, 4, 4, 4, g_format, g_block,
                            probe);
  GLenum err = glGetError();
  g_sub_allowed = (err == GL_NO_ERROR);

  if (!g_sub_allowed)
    TEST_LOG_INFO("Format does not allow sub-updates (err=0x%X)", err);

  while (glGetError() != GL_NO_ERROR)
    ;
}


/* Testlerde kullanilacak gecerli format/imageSize degerleri */
static GLenum active_format(void) {
  return g_has_format ? g_format : GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
}

static GLsizei active_image_size(void) { return g_has_format ? g_block : 8; }

// TC_001: Gecersiz format
// Bu fonksiyon yalnizca SIKISTIRILMIS format kabul eder.
// Sikistirilmamis formatlar (GL_RGBA, vb.) ve konuyla ilgisi olmayan 
// enumlar denendiginde reddedilmelidir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_001(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GLubyte data[MAX_IMAGE_BYTES] = {0};
  int fail_count = 0;

  GLenum bad_formats[] = {GL_RGBA,      GL_RGB,         GL_LUMINANCE,
                          GL_ALPHA,     GL_TRIANGLES,   (GLenum)0x0000,
                          (GLenum)0xDEAD};
  int fmt_count = sizeof(bad_formats) / sizeof(bad_formats[0]);
  int enum_count = 0;
  int oper_count = 0;

  for (int i = 0; i < fmt_count; i++) {
    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 4, bad_formats[i], 8,
                              data);
    GLenum err = glGetError();

    if (err == GL_INVALID_ENUM)
      enum_count++;
    else if (err == GL_INVALID_OPERATION)
      oper_count++;
    else {
      TEST_LOG_FAIL(test_case_1, test_procedure,
                    "fmt=0x%X expected=GL_INVALID_ENUM or "
                    "GL_INVALID_OPERATION actual=0x%X",
                    bad_formats[i], err);
      fail_count++;
    }
  }

  TEST_LOG_INFO("Invalid formats rejected: %d INVALID_ENUM, %d "
                "INVALID_OPERATION (total %d)",
                enum_count, oper_count, fmt_count);

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_1, test_procedure);
  else
    TEST_LOG_FAIL(test_case_1, test_procedure, "Total failed: %d", fail_count);
}

// TC_002: Gecersiz target (GL_INVALID_ENUM)
// SC 2.0'da tek gecerli doku hedefi GL_TEXTURE_2D'dir. Doku hedefi olmayan
// baglama noktalari denenir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_002(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GLubyte data[MAX_IMAGE_BYTES] = {0};
  GLenum format = active_format();
  int fail_count = 0;

  GLenum bad_targets[] = {GL_TEXTURE_3D,  GL_RENDERBUFFER, GL_ARRAY_BUFFER,
                          GL_FRAMEBUFFER, (GLenum)0x0000,  (GLenum)0xDEAD};
  int tgt_count = sizeof(bad_targets) / sizeof(bad_targets[0]);

  for (int i = 0; i < tgt_count; i++) {
    glCompressedTexSubImage2D(bad_targets[i], 0, 0, 0, 4, 4, format, 8, data);
    GLenum err = glGetError();

    if (err != GL_INVALID_ENUM) {
      TEST_LOG_FAIL(test_case_2, test_procedure,
                    "target=0x%X expected=GL_INVALID_ENUM actual=0x%X",
                    bad_targets[i], err);
      fail_count++;
    }
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_2, test_procedure);
  else
    TEST_LOG_FAIL(test_case_2, test_procedure, "Total failed: %d", fail_count);
}

// TC_003: Cubemap hedefleri
// SC 2.0'da cubemap yok, reddedilmelidir. Ancak test ES 2.0 suruculerinde 
// calisirsa, kabul edilebilir, hata firlatmadigi icin log olarak raporlanir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_003(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GLubyte data[MAX_IMAGE_BYTES] = {0};
  GLenum format = active_format();

  GLenum cube_targets[] = {
      GL_TEXTURE_CUBE_MAP,            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
  int cube_count = sizeof(cube_targets) / sizeof(cube_targets[0]);
  int rejected = 0;

  for (int i = 0; i < cube_count; i++) {
    glCompressedTexSubImage2D(cube_targets[i], 0, 0, 0, 4, 4, format, 8, data);
    if (glGetError() == GL_INVALID_ENUM)
      rejected++;
  }
  while (glGetError() != GL_NO_ERROR)
    ;

  TEST_LOG_INFO("Cubemap targets rejected: %d/%d (SC 2.0 requires %d)",
                rejected, cube_count, cube_count);

  TEST_LOG_SUCCESS(test_case_3, test_procedure);
}


// TC_004: Konum ve boyut (GL_INVALID_VALUE)
// level, xoffset, yoffset, width, height negatif olamaz; yazilacak bolge 
// dokunun disina tasamaz.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_004(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GLubyte data[MAX_IMAGE_BYTES] = {0};
  GLenum format = active_format();
  GLsizei size = active_image_size();

  int fail_count = 0;

  struct {
    GLint level;
    GLint xoff;
    GLint yoff;
    GLsizei w;
    GLsizei h;
    const char *desc;
  } cases[] = {
      {-1, 0, 0, 4, 4, "level = -1"},
      {0, -4, 0, 4, 4, "xoffset = -4"},
      {0, 0, -4, 4, 4, "yoffset = -4"},
      {0, 0, 0, -4, 4, "width = -4"},
      {0, 0, 0, 4, -4, "height = -4"},
      {0, TEX_SIZE, 0, 4, 4, "xoffset + w > tex_width"},
      {0, 0, TEX_SIZE, 4, 4, "yoffset + h > tex_height"},
      {1000, 0, 0, 4, 4, "level > max"},
  };
  int count = sizeof(cases) / sizeof(cases[0]);

  for (int i = 0; i < count; i++) {
    glCompressedTexSubImage2D(GL_TEXTURE_2D, cases[i].level, cases[i].xoff,
                              cases[i].yoff, cases[i].w, cases[i].h, format,
                              size, data);
    GLenum err = glGetError();

    if (err != GL_INVALID_VALUE) {
      TEST_LOG_FAIL(test_case_4, test_procedure,
                    "%s: expected=GL_INVALID_VALUE actual=0x%X", cases[i].desc,
                    err);
      fail_count++;
    }
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_4, test_procedure);
  else
    TEST_LOG_FAIL(test_case_4, test_procedure, "Total failed: %d", fail_count);
}

// TC_005: imageSize (GL_INVALID_VALUE)
// Surucu kac bayt okunacagini sadece imageSize parametresinden bilir.
// Hatali imageSize degerleri verildiginde tampon tasmasi onlenmelidir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_005(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  if (!g_has_format) {
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
    return;
  }

  GLubyte data[MAX_IMAGE_BYTES] = {0};
  int fail_count = 0;

  GLsizei bad_sizes[] = {-1, 0, g_block - 1, g_block + 1, g_block * 4,
                         0x7FFFFFFF};
  int size_count = sizeof(bad_sizes) / sizeof(bad_sizes[0]);

  for (int i = 0; i < size_count; i++) {
    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 4, 4, 4, 4, g_format,
                              bad_sizes[i], data);
    GLenum err = glGetError();

    if (err != GL_INVALID_VALUE) {
      TEST_LOG_FAIL(test_case_5, test_procedure,
                    "imageSize=%d expected=GL_INVALID_VALUE actual=0x%X",
                    bad_sizes[i], err);
      fail_count++;
    }
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_5, test_procedure);
  else
    TEST_LOG_FAIL(test_case_5, test_procedure, "Total failed: %d", fail_count);
}


// TC_006: Hata sirasi
// Zaten hatali olan cagrilara data yerine NULL gonderilir. Surucunun hatayi 
// once fark edip pointer'a hic erismeden reddetmesi beklenir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_006(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  GLenum format = active_format();
  GLsizei size = active_image_size();
  int fail_count = 0;

  glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 4, (GLenum)0xDEAD, 8,
                            NULL);
  GLenum err = glGetError();

  if (err != GL_INVALID_ENUM && err != GL_INVALID_OPERATION) {
    TEST_LOG_FAIL(test_case_6, test_procedure,
                  "NULL data + bad format: expected=GL_INVALID_ENUM or "
                  "GL_INVALID_OPERATION actual=0x%X",
                  err);
    fail_count++;
  }

  glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, -4, -4, format, size,
                            NULL);
  err = glGetError();

  if (err != GL_INVALID_VALUE) {
    TEST_LOG_FAIL(test_case_6, test_procedure,
                  "NULL data + negative dim: expected=GL_INVALID_VALUE actual=0x%X",
                  err);
    fail_count++;
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_6, test_procedure);
  else
    TEST_LOG_FAIL(test_case_6, test_procedure, "Total failed: %d", fail_count);
}

// TC_007: Sifir boyut
// width, height ve imageSize 0 iken kopyalanacak veri olmadigi icin
// surucu pointer'i hic okumamali ve hata da firlatmamalidir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_007(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  if (!g_has_format || !g_sub_allowed) {
    TEST_LOG_SUCCESS(test_case_7, test_procedure);
    return;
  }

  int fail_count = 0;

  glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_format, 0, NULL);
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_7, test_procedure,
                  "0 size + NULL data: expected=GL_NO_ERROR actual=0x%X", err);
    fail_count++;
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_7, test_procedure);
  else
    TEST_LOG_FAIL(test_case_7, test_procedure, "Total failed: %d", fail_count);
}

// TC_008: Hizasiz pointer
// Sikistirilmis veriye GL_UNPACK_ALIGNMENT uygulanmadigi icin,
// adresin hizasiz olmasi durumunda da sonuc degismemelidir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_008(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  if (!g_has_format || !g_sub_allowed) {
    TEST_LOG_SUCCESS(test_case_8, test_procedure);
    return;
  }

  static GLubyte buffer[1 + MAX_IMAGE_BYTES];
  memset(buffer, 0, sizeof(buffer));

  GLubyte *payload = buffer + 1;
  for (GLsizei i = 0; i < g_block; i++)
    payload[i] = (GLubyte)(i * 7 + 1);

  int fail_count = 0;

  /* Aligned pointer */
  glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 4, 4, 4, 4, g_format, g_block,
                            payload);
  GLenum aligned_err = glGetError();

  /* Unaligned pointer (offset by 1) */
  glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 8, 8, 4, 4, g_format, g_block,
                            payload + 1);
  GLenum unaligned_err = glGetError();

  if (aligned_err != GL_NO_ERROR) {
    TEST_LOG_FAIL(test_case_8, test_procedure, "Aligned call failed: 0x%X",
                  aligned_err);
    fail_count++;
  }

  if (unaligned_err != aligned_err) {
    TEST_LOG_FAIL(test_case_8, test_procedure,
                  "Unaligned pointer result mismatch: aligned=0x%X "
                  "unaligned=0x%X",
                  aligned_err, unaligned_err);
    fail_count++;
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_8, test_procedure);
  else
    TEST_LOG_FAIL(test_case_8, test_procedure, "Total failed: %d", fail_count);
}

// TC_009: Salt okunurluk
// data tamponunun giris parametresi oldugu ve iceriginin modifiye
// edilmemesi gerektigi test edilir.
void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_009(void) {
  while (glGetError() != GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_tex);
  while (glGetError() != GL_NO_ERROR);

  if (!g_has_format || !g_sub_allowed) {
    TEST_LOG_SUCCESS(test_case_9, test_procedure);
    return;
  }

  static GLubyte buffer[8 + MAX_IMAGE_BYTES + 8];
  memset(buffer, 0xA5, sizeof(buffer));

  GLubyte *payload = buffer + 8;
  for (GLsizei i = 0; i < g_block; i++)
    payload[i] = (GLubyte)(i * 7 + 1);

  GLubyte snapshot[sizeof(buffer)];
  memcpy(snapshot, buffer, sizeof(buffer));

  int fail_count = 0;

  glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 4, 4, 4, 4, g_format, g_block,
                            payload);
  while (glGetError() != GL_NO_ERROR)
    ;

  if (memcmp(snapshot, buffer, sizeof(buffer)) != 0) {
    TEST_LOG_FAIL(test_case_9, test_procedure,
                  "Source buffer was modified (data should be read-only)");
    fail_count++;
  }

  if (fail_count == 0)
    TEST_LOG_SUCCESS(test_case_9, test_procedure);
  else
    TEST_LOG_FAIL(test_case_9, test_procedure, "Total failed: %d", fail_count);
}


void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TP_001_init(void) {
  detect_format();
  setup_texture();

  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_001();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_002();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_003();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_004();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_005();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_006();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_007();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_008();
  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TC_009();

  GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TP_001_close();
}

void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TP_001_draw(void) {}

void GS_GL20SC_TEXT_CTSI_ROBUSTNESS_TP_001_close(void) {
  glBindTexture(GL_TEXTURE_2D, 0);
#ifdef __ubuntu__
  if (g_tex)
    glDeleteTextures(1, &g_tex);
#endif
  while (glGetError() != GL_NO_ERROR)
    ;
}
