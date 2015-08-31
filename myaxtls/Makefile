CFLAGS = -O3 -DSSL_STATIC_LIBRARY=1 -DAXTLS_LIBRARY=1 -DAS_STATIC_LIB=1 -DCONFIG_OPENSSL_COMPATIBLE=1

SOURCES= aes.c asn1.c bigint.c crypto_misc.c gen_cert.c hmac.c \
    loader.c sha256.c md5.c openssl.c os_port.c p12.c rc4.c \
    rsa.c sha1.c tls1.c tls1_clnt.c tls1_svr.c x509.c

OBJECTS = $(SOURCES:.c=.o)

libaxtls.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)