#define GLFW_INCLUDE_ES2
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* test_procedure = "Texturing_GenTextures_TP_001";

/*
 * glGenTextures Robustness Test Suite
 * Hasan - OpenGL ES 2.0
 *
 * Bu test paketi, glGenTextures fonksiyonunun texture'a ozgu
 * sinir durumlarini ve davranislarini test eder.
 */

// ---------------------------------------------------------------
// TEST 1: glIsTexture Durum Zinciri
// Bir texture ID'sinin yasam dongusundeki her asamada
// glIsTexture'in dogru sonuc dondurup dondurmedigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_001(void) {
  static const char* test_case = "Texturing_GenTextures_TC_001";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_isTexture_state_chain\n", test_procedure, test_case);

  GLuint tex;
  glGenTextures(1, &tex);

  // Asama 1: Uretildi ama bind edilmedi -> GL_FALSE olmali
  GLboolean after_gen = glIsTexture(tex);

  // Asama 2: Bind edildi -> GL_TRUE olmali
  glBindTexture(GL_TEXTURE_2D, tex);
  GLboolean after_bind = glIsTexture(tex);

  // Asama 3: Silindi -> GL_FALSE olmali
  glDeleteTextures(1, &tex);
  GLboolean after_delete = glIsTexture(tex);

  GLenum err = glGetError();

  printf("  gen sonrasi:    glIsTexture = %s (beklenen: GL_FALSE)\n",
         after_gen ? "GL_TRUE" : "GL_FALSE");
  printf("  bind sonrasi:   glIsTexture = %s (beklenen: GL_TRUE)\n",
         after_bind ? "GL_TRUE" : "GL_FALSE");
  printf("  delete sonrasi: glIsTexture = %s (beklenen: GL_FALSE)\n",
         after_delete ? "GL_TRUE" : "GL_FALSE");

  if (!after_gen && after_bind && !after_delete && err == GL_NO_ERROR)
    printf("  -> PASSED\n\n");
  else
    printf("  -> FAILED (hata: 0x%X)\n\n", err);
}

// ---------------------------------------------------------------
// TEST 2: Aktif Bind Sirasinda Uretim
// Bir texture aktif olarak bind edilmis iken yeni texture'lar
// uretmenin mevcut baglantiyı bozup bozmadigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_002(void) {
  static const char* test_case = "Texturing_GenTextures_TC_002";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_gen_while_texture_bound\n", test_procedure, test_case);

  GLuint first;
  glGenTextures(1, &first);
  glBindTexture(GL_TEXTURE_2D, first);

  // Binding aktifken yeni texture'lar uret
  GLuint batch[10];
  glGenTextures(10, batch);

  // Mevcut binding bozulmamis olmali
  GLint current_binding = -1;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_binding);

  GLenum err = glGetError();

  if ((GLuint)current_binding == first && err == GL_NO_ERROR)
    printf("  -> PASSED: Binding korundu (aktif=%u, beklenen=%u)\n\n",
           current_binding, first);
  else
    printf(
        "  -> FAILED: Binding bozuldu! (aktif=%d, beklenen=%u, hata=0x%X)\n\n",
        current_binding, first, err);

  glDeleteTextures(10, batch);
  glDeleteTextures(1, &first);
}

// ---------------------------------------------------------------
// TEST 3: Ayni Diziye Ust Uste Yazma
// Ayni buffer'a art arda glGenTextures cagrildiginda eski
// isimlerin gecersiz kalip kalmadigini gozlemler.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_003(void) {
  static const char* test_case = "Texturing_GenTextures_TC_003";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_overwrite_same_array\n", test_procedure, test_case);

  GLuint names[5];

  // Ilk uretim
  glGenTextures(5, names);
  GLuint first_set[5];
  memcpy(first_set, names, sizeof(names));

  // Ayni array'e ikinci uretim (eski isimler kaybolur)
  glGenTextures(5, names);

  // Ilk uretilen isimler artik sahipsiz: bind edilmemis ve kimsenin
  // referansi yok. Spec'e gore leak olur ama crash olmamali.
  GLenum err = glGetError();

  printf("  Ilk set:  [%u, %u, %u, %u, %u]\n", first_set[0], first_set[1],
         first_set[2], first_set[3], first_set[4]);
  printf("  Ikinci set: [%u, %u, %u, %u, %u]\n", names[0], names[1], names[2],
         names[3], names[4]);

  if (err == GL_NO_ERROR)
    printf("  -> PASSED: Ust uste yazma sorunsuz (hata: 0x0)\n\n");
  else
    printf("  -> FAILED: Hata: 0x%X\n\n", err);

  glDeleteTextures(5, names);
}

// ---------------------------------------------------------------
// TEST 4: TEXTURE_2D ve CUBE_MAP Karisik Uretim-Bind Senaryosu
// Ayni batch'te uretilen ID'lerden bazilari 2D, bazilari CUBE_MAP
// olarak bind edildiginde birbirlerini etkileyip etkilemedigini test eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_004(void) {
  static const char* test_case = "Texturing_GenTextures_TC_004";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_mixed_target_batch\n", test_procedure, test_case);

  GLuint tex_ids[4];
  glGenTextures(4, tex_ids);

  // 0 ve 1 -> TEXTURE_2D
  glBindTexture(GL_TEXTURE_2D, tex_ids[0]);
  glBindTexture(GL_TEXTURE_2D, tex_ids[1]);

  // 2 ve 3 -> TEXTURE_CUBE_MAP
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_ids[2]);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_ids[3]);

  GLenum err = glGetError();

  // Simdi 0'i CUBE_MAP'e bind etmeye calis -> GL_INVALID_OPERATION
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_ids[0]);
  GLenum cross_err = glGetError();

  printf("  Karisik bind hatasi: 0x%X (beklenen: 0x0)\n", err);
  printf("  Capraz hedef hatasi: 0x%X (beklenen: GL_INVALID_OPERATION 0x502)\n",
         cross_err);

  if (err == GL_NO_ERROR && cross_err == GL_INVALID_OPERATION)
    printf("  -> PASSED\n\n");
  else
    printf("  -> FAILED\n\n");

  glDeleteTextures(4, tex_ids);
}

// ---------------------------------------------------------------
// TEST 5: Hata Bayraginin Korunmasi
// Onceden var olan bir GL hata bayragi uzerinde glGenTextures
// cagrisinin hata durumunu temizleyip temizledigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_005(void) {
  static const char* test_case = "Texturing_GenTextures_TC_005";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_error_flag_preservation\n", test_procedure, test_case);

  // Kasitli olarak bir hata uret
  glBindTexture(0xDEAD, 0); // GL_INVALID_ENUM uretir
  GLenum planted_err = glGetError();

  // Simdi yeni bir hata dikmeden temiz cagri yap
  while (glGetError() != GL_NO_ERROR)
    ;                       // onceki hatayi temizle
  glBindTexture(0xBEEF, 0); // yeni bir hata dik
  // Bu hatayi OKUMADAN glGenTextures cagir
  GLuint tex;
  glGenTextures(1, &tex);

  // Dikilen hata hala mevcut olmali
  GLenum surviving_err = glGetError();

  printf("  Dikilen hata:  0x%X (GL_INVALID_ENUM beklenir)\n", planted_err);
  printf("  Gen sonrasi:   0x%X (GL_INVALID_ENUM hala durmali)\n",
         surviving_err);

  if (planted_err == GL_INVALID_ENUM && surviving_err == GL_INVALID_ENUM)
    printf("  -> PASSED: glGenTextures onceki hatayi silmedi\n\n");
  else
    printf("  -> FAILED\n\n");

  glDeleteTextures(1, &tex);
}

// ---------------------------------------------------------------
// TEST 6: Tekli vs Toplu Uretim Tutarliligi
// 5 adet tek tek uretilmis ve 5 adet toplu uretilmis ID'nin
// birbirleriyle veya 0 ile cakismadigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_006(void) {
  static const char* test_case = "Texturing_GenTextures_TC_006";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_single_vs_batch_consistency\n", test_procedure, test_case);

  GLuint singles[5];
  for (int i = 0; i < 5; i++) {
    glGenTextures(1, &singles[i]);
  }

  GLuint batch[5];
  glGenTextures(5, batch);

  GLuint all[10];
  memcpy(all, singles, 5 * sizeof(GLuint));
  memcpy(all + 5, batch, 5 * sizeof(GLuint));

  int ok = 1;
  for (int i = 0; i < 10; i++) {
    if (all[i] == 0) {
      ok = 0;
      break;
    }
    for (int j = i + 1; j < 10; j++) {
      if (all[i] == all[j]) {
        ok = 0;
        break;
      }
    }
    if (!ok)
      break;
  }

  GLenum err = glGetError();

  if (ok && err == GL_NO_ERROR)
    printf(
        "  -> PASSED: Tekli ve toplu uretim tutarli, tum ID'ler benzersiz\n\n");
  else
    printf("  -> FAILED: Tutarsizlik veya sifir ID saptandi (hata: 0x%X)\n\n",
           err);

  glDeleteTextures(5, singles);
  glDeleteTextures(5, batch);
}

// ---------------------------------------------------------------
// TEST 7: TexImage2D ile Uretim Arasi Etkilesim
// Aktif bir texture'a glTexImage2D ile veri yuklenirken araya
// glGenTextures girmesinin VRAM/state durumunu bozmadigini test eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_007(void) {
  static const char* test_case = "Texturing_GenTextures_TC_007";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_gen_interleaved_with_teximage\n", test_procedure, test_case);

  GLuint tex_a;
  glGenTextures(1, &tex_a);
  glBindTexture(GL_TEXTURE_2D, tex_a);

  GLubyte red_pixels[16] = {255, 0, 0, 255, 255, 0, 0, 255,
                            255, 0, 0, 255, 255, 0, 0, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               red_pixels);
  GLenum err1 = glGetError();

  // Veri yuklendi, simdi araya gen giriyor
  GLuint tex_b;
  glGenTextures(1, &tex_b);

  // tex_a hala aktif bind olmali
  GLint bound_now = -1;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_now);

  GLenum err2 = glGetError();

  printf("  TexImage2D hatasi: 0x%X\n", err1);
  printf("  Araya gen sonrasi aktif texture: %d (beklenen: %u)\n", bound_now,
         tex_a);

  if (err1 == GL_NO_ERROR && err2 == GL_NO_ERROR && (GLuint)bound_now == tex_a)
    printf("  -> PASSED: Veri yukleme ve uretim birbirini etkilemedi\n\n");
  else
    printf("  -> FAILED\n\n");

  glDeleteTextures(1, &tex_a);
  glDeleteTextures(1, &tex_b);
}

// ---------------------------------------------------------------
// TEST 8: Negatif n Degeri (Spec Tanimi: GL_INVALID_VALUE)
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_008(void) {
  static const char* test_case = "Texturing_GenTextures_TC_008";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_negative_n_spec_error\n", test_procedure, test_case);

  GLuint dummy = 0;
  glGenTextures(-1, &dummy);
  GLenum err = glGetError();

  if (err == GL_INVALID_VALUE)
    printf("  -> PASSED: n=-1 icin GL_INVALID_VALUE (0x501) alindi\n\n");
  else
    printf("  -> FAILED: Beklenen 0x501, alinan 0x%X\n\n", err);
}

// ---------------------------------------------------------------
// TEST 9: Texture Parametre Durumu (Varsayilan Degerler)
// Yeni uretilip bind edilen bir texture'in varsayilan filtre
// parametrelerinin dogru olup olmadigini kontrol eder.
// ---------------------------------------------------------------
void Texturing_GenTextures_TC_009(void) {
  static const char* test_case = "Texturing_GenTextures_TC_009";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] test_default_texture_parameters\n", test_procedure, test_case);

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  GLint min_filter = 0, mag_filter = 0, wrap_s = 0, wrap_t = 0;
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &mag_filter);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap_s);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrap_t);

  GLenum err = glGetError();

  printf("  MIN_FILTER = 0x%X (beklenen: 0x2702)\n", min_filter);
  printf("  MAG_FILTER = 0x%X (beklenen: 0x2601)\n", mag_filter);
  printf("  WRAP_S     = 0x%X (beklenen: 0x2901)\n", wrap_s);
  printf("  WRAP_T     = 0x%X (beklenen: 0x2901)\n", wrap_t);

  if (min_filter == GL_NEAREST_MIPMAP_LINEAR && mag_filter == GL_LINEAR &&
      wrap_s == GL_REPEAT && wrap_t == GL_REPEAT && err == GL_NO_ERROR)
    printf("  -> PASSED: Varsayilan texture parametreleri dogru\n\n");
  else
    printf("  -> FAILED (hata: 0x%X)\n\n", err);

  glDeleteTextures(1, &tex);
}

// TEST 10 : n<0 ise Beklenen hata: GL_INVALID_VALUE
void Texturing_GenTextures_TC_010(void) {
  static const char* test_case = "Texturing_GenTextures_TC_010";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] Texturing_GenTextures_TC_010()\n", test_procedure, test_case);

  GLuint texture = 0;
  glGenTextures(-1, &texture);
  GLenum err = glGetError();
  if (err != GL_INVALID_VALUE) {
    printf("[FAIL] Expected GL_INVALID_VALUE, but got 0x%X\n", err);
  } else {
    printf("[PASS] Texturing_GenTextures_TC_010()\n");
  }
}

// TEST 11 : textures = NULL, n > 0 (negative robustness)
void Texturing_GenTextures_TC_011(void) {
  static const char* test_case = "Texturing_GenTextures_TC_011";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] Texturing_GenTextures_TC_011()\n", test_procedure, test_case);

  glGenTextures(5, NULL);
  GLenum err = glGetError();
  printf("[INFO] glGenTextures(textures=nullptr, n=5): error=0x%X\n", err);
}

// TEST 12 : Unique Names Test
void Texturing_GenTextures_TC_012(void) {
  static const char* test_case = "Texturing_GenTextures_TC_012";
  while (glGetError() != GL_NO_ERROR)
    ;
  printf("[TEST][%s][%s] Texturing_GenTextures_TC_012()\n", test_procedure, test_case);

  const GLsizei COUNT = 1000;
  GLuint textures[1000];
  glGenTextures(COUNT, textures);

  for (int i = 0; i < COUNT; i++) {
    if (textures[i] == 0) {
      printf("[FAIL] glGenTextures returned reserved name 0.\n");
      glDeleteTextures(COUNT, textures);
      return;
    }
  }

  for (int i = 0; i < COUNT; i++) {
    for (int j = i + 1; j < COUNT; j++) {
      if (textures[i] == textures[j]) {
        printf("[FAIL] Duplicate texture name %u found.\n", textures[i]);
        glDeleteTextures(COUNT, textures);
        return;
      }
    }
  }

  GLenum err = glGetError();
  if (err == GL_NO_ERROR)
    printf("[PASS] All generated texture names are unique and non-zero.\n");
  else
    printf("[FAIL] glGetError() = 0x%X\n", err);

  glDeleteTextures(COUNT, textures);
}

static GLFWwindow *window = NULL;
static int width = 640, height = 480;
static const char *windowTitle = "glGenTextures Robustness Tests - Hasan";

void init(void);
void draw(void);
void clean(void);

void init(void) {
  printf("=====================================================\n");
  printf("  %s ROBUSTNESS SUITE - HASAN\n", test_procedure);
  printf("=====================================================\n\n");

  Texturing_GenTextures_TC_001();
  Texturing_GenTextures_TC_002();
  Texturing_GenTextures_TC_003();
  Texturing_GenTextures_TC_004();
  Texturing_GenTextures_TC_005();
  Texturing_GenTextures_TC_006();
  Texturing_GenTextures_TC_007();
  Texturing_GenTextures_TC_008();
  Texturing_GenTextures_TC_009();
  Texturing_GenTextures_TC_010();
  Texturing_GenTextures_TC_011();
  Texturing_GenTextures_TC_012();

  printf("=====================================================\n");
  printf("  TUM TESTLER TAMAMLANDI\n");
  printf("=====================================================\n");
}

void draw(void) {
  glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void clean(void) {
}

int main(void) {
  setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
  setenv("GALLIUM_DRIVER", "llvmpipe", 1);
  unsetenv("WAYLAND_DISPLAY");

  if (!glfwInit())
    return -1;

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
