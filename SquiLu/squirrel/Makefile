SQUIRREL= ..


OUT= $(SQUIRREL)/lib/libsquirrel.a
INCZ= -I$(SQUIRREL)/include -I. -Iinclude -I$(SQUIRREL)/sqstdlib
DEFS= $(CC_EXTRA_FLAGS)
LIB=	

OBJS= \
	sqapi.o \
	sqbaselib.o \
	sqfuncstate.o \
	sqdebug.o \
	sqlexer.o \
	sqobject.o \
	sqcompiler.o \
	sqstate.o \
	sqtable.o \
	sqmem.o \
	sqvm.o \
	sqclass.o \
	lua-regex.o \
	sublatin.o
	
SRCS= \
	sqapi.cpp \
	sqbaselib.cpp \
	sqfuncstate.cpp \
	sqdebug.cpp \
	sqlexer.cpp \
	sqobject.cpp \
	sqcompiler.cpp \
	sqstate.cpp \
	sqtable.cpp \
	sqmem.cpp \
	sqvm.cpp \
	sqclass.cpp \
	lua-regex.c \
	sublatin.c

	
	
sq32:
	$(CC) -O2 -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing -c $(SRCS) $(INCZ) $(DEFS)
	ar rc $(OUT) *.o
	rm *.o

sqprof:
	$(CC) -O2 -pg -fno-exceptions -fno-rtti -pie -gstabs -g3 -Wall -fno-strict-aliasing -c $(SRCS) $(INCZ) $(DEFS)
	ar rc $(OUT) *.o
	rm *.o

sq64:
	$(CC) -O2 -m64 -D_SQ64 -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing -c $(SRCS) $(INCZ) $(DEFS)
#	$(CC) -g -m64 -D_SQ64 -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing -c $(SRCS) $(INCZ) $(DEFS)
	ar rc $(OUT) *.o
	rm *.o
