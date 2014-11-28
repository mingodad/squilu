SOURCES = attribute.c char_ref.c error.c parser.c string_buffer.c \
	string_piece.c tag.c tokenizer.c utf8.c util.c vector.c

libgumbo.a : $(SOURCES)
	gcc -O2 -std=c99 -c  $(SOURCES)
	ar -cvq libgumbo.a *.o

clean:
	rm *.o *.a