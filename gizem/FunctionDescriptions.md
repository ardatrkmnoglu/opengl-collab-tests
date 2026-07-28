# OpenGL ES 2.0 — Buffer & Framebuffer Fonksiyonları

Bu doküman, OpenGL ES 2.0'da buffer nesneleri ve framebuffer nesneleriyle ilgili temel fonksiyonları özetler.

---

## Buffer Objects

### `glBindBuffer`
```c
void glBindBuffer(GLenum target, GLuint buffer);
```
- Bir buffer nesnesini belirli bir **target**'a bağlar.
- Bağlandıktan sonra o hedef üzerinde yapılan işlemler artık bu buffer üzerinde gerçekleştirilir.

### `glBufferData`
```c
void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
```
- `target` ile bind ettiğin buffer object için GPU üzerinde **yeni bir data store** oluşturur.
- Eski veriyi tamamen siler.
- İstersen verdiğin `data` pointer'ındaki veriyi bu yeni belleğe kopyalayarak başlatır.
- `usage` parametresiyle bu veriyi nasıl kullanacağını sürücüye ipucu olarak bildirirsin (performans optimizasyonu için).

### `glBufferSubData`
```c
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
```
- Daha önce oluşturulmuş bir buffer'ın içindeki **belirli bir kısmı** günceller.
- Yeni bellek ayırmaz; var olan `glBufferData` ile oluşturulmuş data store'un içini kısmen değiştirir.
- `target` ile belirtilen ve şu an `glBindBuffer` ile bağlanmış buffer'ı kullanır.
- Buffer'ın `offset` byte'tan başlayan kısmına, `size` byte uzunluğunda `data` pointer'ındaki veriyi kopyalar.

### `glGenBuffers`
```c
void glGenBuffers(GLsizei n, GLuint *buffers);
```
Yeni buffer nesneleri için ID üretir.

| Parametre | Açıklama |
|---|---|
| `n` | Kaç tane buffer ismi üretileceği |
| `buffers` | Üretilen buffer ID'lerinin yazılacağı dizi |

### `glGetBufferParameteriv`
```c
void glGetBufferParameteriv(GLenum target, GLenum value, GLint *data);
```
Halihazırda bağlı (bound) olan bir buffer nesnesinin bazı özelliklerini öğrenmek için kullanılır.

| Parametre | Açıklama |
|---|---|
| `target` | Hangi tür buffer'a bakacağını söyler |
| `value` | Hangi parametreyi istediğini söyler |
| `data` | Sonucun yazılacağı adres |

---

## Framebuffer Objects

### `glBindFramebuffer`
```c
void glBindFramebuffer(GLenum target, GLuint framebuffer);
```
OpenGL'de hangi framebuffer'a çizim yapılacağını ve hangi framebuffer'dan okunacağını seçmek için kullanılır.

- **target**: Modern çekirdekte genelde hep `GL_FRAMEBUFFER` kullanılır.
- **framebuffer = 0**: Varsayılan framebuffer.
- **framebuffer ≠ 0**: `glGenFramebuffers` ile oluşturduğun, "off-screen" (ekran dışında) bir framebuffer'a geçersin.

### `glGenFramebuffers`
```c
void glGenFramebuffers(GLsizei n, GLuint *framebuffers);
```
OpenGL'den senin için yeni framebuffer ID numaraları üretmesini ister.

| Parametre | Açıklama |
|---|---|
| `n` | Kaç tane framebuffer ID'si istiyorsun |
| `framebuffers` | Bu ID'lerin yazılacağı `GLuint` dizisinin adresi |

### `glBindRenderbuffer`
```c
void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
```
İçeride "hangi renderbuffer'ın aktif olduğunu" seçmek için kullanılır:

- `glGenRenderbuffers` → yeni renderbuffer isimleri üretir (ID verir)
- `glBindRenderbuffer(GL_RENDERBUFFER, id)` → o ID'li renderbuffer'ı aktif yapar

| Parametre | Açıklama |
|---|---|
| `target` | OpenGL'e "hangi binding point'i kullanacağım?" bilgisini verir (mutlaka `GL_RENDERBUFFER` olmalı) |
| `renderbuffer` | Bağlamak istediğin renderbuffer nesnesinin ID'si. Bir ID ise o renderbuffer aktif hale gelir; **0** ise unbind edilir |

### `glGenRenderbuffers`
```c
void glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
```
Renderbuffer object için yeni kimlik/ID üretmek içindir.

| Parametre | Açıklama |
|---|---|
| `n` | Kaç adet renderbuffer ID üretileceğini söyler |
| `renderbuffers` | Üretilen ID'lerin yazılacağı dizi (çoğunlukla tek bir `GLuint` değişken için `n=1` ve adresi verilir) |

### `glRenderbufferStorage`
```c
void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
```
Bir renderbuffer için bellekte data store oluşturur ve başlatır:

- Önceden o renderbuffer'a ait bir bellek alanı varsa silinir.
- Yeni bir `internalformat` seçilir.
- Renderbuffer'ın boyutları `width × height` olarak ayarlanır.
- Böylece renderbuffer, framebuffer'da (ör. renk / derinlik ekleri gibi) kullanılabilecek şekilde "hazır" hale gelir.
- Fonksiyonun sonunda renderbuffer'ın içeriği **tanımsızdır** (sıfırla/temizle yapmaz). İstersen render ile doldurursun.

| Parametre | Açıklama |
|---|---|
| `target` | Mutlaka `GL_RENDERBUFFER` olmalı |
| `internalformat` | Renderbuffer'ın türünü/formatını belirler (ör. renk için `GL_RGBA4`, depth için `GL_DEPTH_COMPONENT16`, stencil için `GL_STENCIL_INDEX8`) |
| `width, height` | Renderbuffer'ın piksel cinsinden boyutu |

### `glFramebufferRenderbuffer`
```c
void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
```
Mevcut (bağlı olan) bir Framebuffer (FBO) içine, bir Renderbuffer nesnesini (RBO) belirli bir "attachment" noktasına bağlar. Yani FBO'nun color / depth / stencil gibi mantıksal tamponlarına renderbuffer'ı takarsın.

| Parametre | Açıklama |
|---|---|
| `target` | `GL_FRAMEBUFFER` — bağlanacak framebuffer hedefi |
| `attachment` | Renderbuffer'ın FBO üzerinde takılacağı parça: `GL_COLOR_ATTACHMENT0` (renk), `GL_DEPTH_ATTACHMENT` (depth), `GL_STENCIL_ATTACHMENT` (stencil) |
| `renderbuffertarget` | `GL_RENDERBUFFER` — takılan şeyin türü |
| `renderbuffer` | Bağlanacak renderbuffer'ın ID'si. **0** verirsen ilgili attachment'ten çıkar/detach eder |

### `glCheckFramebufferStatus`
```c
GLenum glCheckFramebufferStatus(GLenum target);
```
- Şu an `glBindFramebuffer(GL_FRAMEBUFFER, ...)` ile bağlı olan framebuffer'ın (FBO) **"framebuffer complete"** olup olmadığını kontrol eder.
- Fonksiyon sana eksik/uygunsuzluk sebebini de söyler.
