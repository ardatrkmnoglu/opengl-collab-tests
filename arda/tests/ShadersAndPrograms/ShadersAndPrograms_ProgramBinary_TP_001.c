#include "../../../include/macro.h"
#include "../../../include/rtests.h"

/* ============================================================
 * TEST GRUBU: glProgramBinary
 * ============================================================ */

/* ============================================================
 * glProgramBinary — Hizasız Pointer
 * ============================================================
 *
 * glProgramBinary fonksiyonuna kasıtlı olarak hizası bozulmuş
 * (unaligned) bir bellek adresi verilerek sürücünün geçersiz
 * bellek erişiminde çökmeden hata üretip üretmediği doğrulanır.
 * ============================================================ */
void ShadersAndPrograms_ProgramBinary_TC_001(void) {
	GLuint prog = glCreateProgram();

	void *valid_memblock = malloc(1024);
	if (!valid_memblock) {
		TEST_LOG_INFO(
		    "ShadersAndPrograms_ProgramBinary_TC_001: malloc başarısız.");
		glDeleteProgram(prog);
		return;
	}
	const void *unaligned_ptr = (const void *)((char *)valid_memblock + 1);

	glProgramBinary(prog, 0x1234, unaligned_ptr, 100);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM || err == GL_INVALID_VALUE ||
	      err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("ProgramBinary",
			      "ShadersAndPrograms_ProgramBinary_TC_001",
			      "Hizasız pointer ile çağrı kabul edildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("ProgramBinary",
				 "ShadersAndPrograms_ProgramBinary_TC_001");
	}

	free(valid_memblock);
	glDeleteProgram(prog);
}

/* ============================================================
 * glProgramBinary — Erişim Hakkı Kaldırılmış Bellek
 * ============================================================
 *
 * mmap ile ayrılan bir bellek bölgesinin erişim hakları
 * mprotect(PROT_NONE) ile kaldırılır ve bu adres
 * glProgramBinary'ye verilir. Sürücünün bellek koruma
 * ihlaline karşı dayanıklılığını test eder.
 * ============================================================ */
void ShadersAndPrograms_ProgramBinary_TC_002(void) {
	GLuint prog = glCreateProgram();

	size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
	void *mapped_memory = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
				   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mapped_memory == MAP_FAILED) {
		TEST_LOG_INFO("ShadersAndPrograms_ProgramBinary_TC_002: mmap başarısız.");
		glDeleteProgram(prog);
		return;
	}
	mprotect(mapped_memory, page_size, PROT_NONE);

	glProgramBinary(prog, 0x1234, mapped_memory, 1024);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_ENUM || err == GL_INVALID_VALUE ||
	      err == GL_INVALID_OPERATION)) {
		TEST_LOG_FAIL("ProgramBinary", "ShadersAndPrograms_ProgramBinary_TC_002",
			      "PROT_NONE bellekle çağrı kabul edildi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("ProgramBinary",
				 "ShadersAndPrograms_ProgramBinary_TC_002");
	}

	munmap(mapped_memory, page_size);
	glDeleteProgram(prog);
}

/* ============================================================
 * glProgramBinary — Aşırı Yükleme (Overload)
 * ============================================================
 *
 * Çok büyük bir uzunluk değeri (2 147 483 631) ve çöp veri
 * ile glProgramBinary çağrılarak sürücünün bu geçersiz
 * binary'yi kabul edip etmediği doğrulanır. Link durumunun
 * GL_FALSE olması beklenir.
 * ============================================================ */
void ShadersAndPrograms_ProgramBinary_TC_003(void) {
	GLuint prog = glCreateProgram();

	GLint num_formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &num_formats);

	GLenum valid_format = 0x1234;
	if (num_formats > 0)
		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS,
			      (GLint *)&valid_format);

	char buf[4] = {(char)0xDE, (char)0xAD, (char)0xBE, (char)0xEF};
	GLsizei overload_len = 2147483631;

	glProgramBinary(prog, valid_format, buf, overload_len);
	GLenum err = glGetError();

	if (err != GL_NO_ERROR) {
		if (!(err == GL_INVALID_ENUM || err == GL_INVALID_VALUE ||
		      err == GL_INVALID_OPERATION)) {
			TEST_LOG_FAIL("ProgramBinary",
				      "ShadersAndPrograms_ProgramBinary_TC_003",
				      "Aşırı yükleme beklenmeyen hata üretti."
				      " Actual: 0x%04X",
				      err);
			glDeleteProgram(prog);
			return;
		}
	}

	GLint link_status = GL_TRUE;
	glGetProgramiv(prog, GL_LINK_STATUS, &link_status);

	if (!(link_status == GL_FALSE)) {
		TEST_LOG_FAIL(
		    "ProgramBinary", "ShadersAndPrograms_ProgramBinary_TC_003",
		    "CRITICAL: Sürücü çöp veriyi geçerli Program Binary "
		    "olarak kabul etti! link_status=%d",
		    link_status);
	} else {
		TEST_LOG_SUCCESS("ProgramBinary",
				 "ShadersAndPrograms_ProgramBinary_TC_003");
	}

	glDeleteProgram(prog);
}

/* ============================================================
 * glProgramBinary — NULL Pointer
 * ============================================================
 *
 * binary parametresi olarak NULL pointer verildiğinde
 * sürücünün çökmeden hata ürettiği doğrulanır.
 * GL_INVALID_VALUE veya GL_INVALID_OPERATION beklenir.
 * ============================================================ */
void ShadersAndPrograms_ProgramBinary_TC_004(void) {
	GLuint prog = glCreateProgram();
	while (glGetError() != GL_NO_ERROR) { /* temizle */
	}

	/* NULL binary pointer: sürücü çökmemeli */
	glProgramBinary(prog, 0x1234, NULL, 64);
	GLenum err = glGetError();

	if (!(err == GL_INVALID_VALUE || err == GL_INVALID_OPERATION ||
	      err == GL_INVALID_ENUM)) {
		TEST_LOG_FAIL("ProgramBinary",
			      "ShadersAndPrograms_ProgramBinary_TC_004",
			      "NULL binary pointer ile çağrı reddedilmedi."
			      " Actual: 0x%04X",
			      err);
	} else {
		TEST_LOG_SUCCESS("ProgramBinary",
				 "ShadersAndPrograms_ProgramBinary_TC_004");
	}

	glDeleteProgram(prog);
}
