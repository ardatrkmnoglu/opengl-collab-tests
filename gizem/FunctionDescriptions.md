
BUFFER OBJECTS ---------

void glBindBuffer(GLenum target, GLuint buffer);
 Bir buffer nesnesini belirli bir target'a bağlar
 Bağlandıktan sonra o hedef üzerinde yapılan işlemler artık bu buffer üzerinde gerçekleştirilir


 void glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
 Target parametresiyle bind ettiğin buffer object için ekran kartı (GPU) üzerinde yeni bir data store oluşturur
 Eski veriyi tamamen siler
 İstersen verdiğin data pointer’ındaki veriyi bu yeni belleğe kopyalayarak başlatır
 usage parametresiyle de bu veriyi nasıl kullanacağını sürücüye ipucu olarak bildirirsin (performans optimizasyonu için)


 void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
 Daha önceden oluşturulmuş bir buffer’ın içindeki belirli bir kısmı günceller
 Yeni bellek ayırmaz, var olan glBufferData ile oluşturulmuş data store’un içini kısmen değiştirir
 target ile belirtilen ve şu an glBindBuffer ile bağlanmış buffer’ı kullanır
 Buffer’ın offset byte’tan başlayan kısmına, size byte uzunluğunda data pointer’ındaki veriyi kopyalar.


 glGenBuffers: yeni buffer nesneleri için ID üretir
 n: Kaç tane buffer ismi üretileceği.
 buffers: Üretilen buffer ID’lerinin yazılacağı dizi.


 void glGetBufferParameteriv(GLenum target, GLenum value, GLint * data);
 halihazırda bağlı (bound) olan bir buffer nesnesinin bazı özelliklerini öğrenmek için kullanılır.
 target: Hangi tür buffer’a bakacağını söyler
 value: Hangi parametreyi istediğini söyler
 data: Sonucun yazılacağı adres




FRAMEBUFFER OBJECTS ------------------

 void glBindFramebuffer(GLenum target, GLuint framebuffer);
 OpenGL’de hangi framebuffer’a çizim yapılacağını ve hangi framebuffer’dan okunacağını seçmek için kullanılır
 target: Modern çekirdekte genelde hep GL_FRAMEBUFFER kullanılır.
 framebuffer = 0: Varsayılan framebuffer
 framebuffer = 0 olmayan bir id dersen: Senin glGenFramebuffers ile oluşturduğun, “off-screen” (ekran dışında) bir framebuffer’a geçersin.


 void glGenFramebuffers(GLsizei n, GLuint * framebuffers);
 OpenGL’den senin için yeni framebuffer ID numaraları üretmesini ister.
 n: Kaç tane framebuffer ID’si istiyorsun.
 framebuffers: Bu ID’lerin yazılacağı GLuint dizisinin adresi


void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
içeride “hangi renderbuffer’ın aktif olduğunu” seçmek için kullanılır. Yani:
glGenRenderbuffers → yeni renderbuffer isimleri üretir (ID verir).
glBindRenderbuffer(GL_RENDERBUFFER, id) → o ID’li renderbuffer’ı aktif yapar.
target: OpenGL’e “hangi binding point’i kullanacağım?” bilgisini verir (mutlaka GL_RENDERBUFFER olmalıdır)
GLuint renderbuffer: Bağlamak istediğin renderbuffer nesnesinin adı/ID’si Bu değer:
- Bir renderbuffer ID’si ise → o renderbuffer aktif hale gelir.
- 0 ise → renderbuffer unbind edilir (şu an bağlı renderbuffer yok olur).


void glGenRenderbuffers(GLsizei n, GLuint * renderbuffers);
Renderbuffer object (renderbuffer) için yeni kimlik/ID üretmek içindir.
n: kaç adet renderbuffer ID üretileceğini söyler.
renderbuffers: üretilen ID’lerin yazılacağı dizi (çoğunlukla tek bir GLuint değişken için n=1 ve adresi verilir).


void glRenderbufferStorage(	GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
Bir renderbuffer (renderbuffer nesnesi) için bellekte data store oluşturur ve başlatır.
- Önceden o renderbuffer’a ait bir bellek alanı varsa silinir.
- Yeni bir internalformat seçilir.
- Renderbuffer’ın boyutları width × height olarak ayarlanır.
- Böylece renderbuffer, framebuffer’da (ör. renk / derinlik ekleri gibi) kullanılabilecek şekilde “hazır” hale gelir.
- Fonksiyonun sonunda renderbuffer’ın içeriği tanımsızdır (yani sıfırla/temizle yapmaz). İstersen render ile doldurursun.
Parametreler
target: mutlaka GL_RENDERBUFFER olmalı.
internalformat: renderbuffer’ın türünü/formatını belirler (ör. renk için GL_RGBA4, depth için GL_DEPTH_COMPONENT16, stencil için GL_STENCIL_INDEX8 gibi).
width, height: renderbuffer’ın piksel cinsinden boyutu.







