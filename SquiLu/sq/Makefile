SQUIRREL= ..


OUT= $(SQUIRREL)/bin/sq
INCZ= -I$(SQUIRREL)/include -I. -I$(SQUIRREL)/sqlibs -DNO_EXCEPTION_KEY_NOT_FOUND22=1
LIBZ= -L$(SQUIRREL)/lib 
LIB= -lsquirrel -lsqstdlib -lm $(LD_EXTRA_FLAGS)
DEFS= $(CC_EXTRA_FLAGS) $(LD_EXTRA_FLAGS)

OBJS= sq.o
	
SRCS= sq.c
	
	
sq32:
	g++ -O2 -s -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)

sq32i64:
	g++ -g -D_SQ64 -O2 -o $(OUT)32i64 $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)

sq32dbg:
	g++ -g -m32 -s -o $(OUT)dbg $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)

sqprof:
	#g++ -O2 -pg  -pie -gstabs -g3 -o $(OUT)-profile $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)
	g++ -O2 -pg -g -fno-exceptions -fno-rtti -gstabs -g3 -o $(OUT)-profile $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)
	
sq64:
	g++ -O3 -s -m64 -D_SQ64 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)
#	g++ -g -m64 -D_SQ64 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)

sq64p:
	g++ -fpack-struct -march=native -O3 -s -m64 -D_SQ64 -o $(OUT)-packed $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)
#	g++ -g -m64 -D_SQ64 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)

sq64dbg:
	g++ -g -o $(OUT)dbg $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)

