CFLAGS = -O3 -Wall -DCONFIG_32=1 -DPPRO=1 -DASM=1

SOURCES= basearith.c constants.c context.c convolute.c crt.c difradix2.c fnt.c \
	fourstep.c io.c memory.c mpdecimal.c mpsignal.c \
	numbertheory.c sixstep.c transpose.c 

OBJECTS = $(SOURCES:.c=.o)

libmpdecimal.a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)