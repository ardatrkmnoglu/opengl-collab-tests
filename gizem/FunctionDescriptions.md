
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
















