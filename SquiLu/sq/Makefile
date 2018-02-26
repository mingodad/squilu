SQUIRREL= ..


OUT= $(SQUIRREL)/bin/sq
INCZ= -I$(SQUIRREL)/include -I. -I$(SQUIRREL)/sqlibs -DNO_EXCEPTION_KEY_NOT_FOUND22=1
LIBZ= -L$(SQUIRREL)/lib 
LIB= -lsquirrel -lsqstdlib -lm $(LD_EXTRA_FLAGS)

OBJS= sq.o
	
SRCS= sq.c
	
	
sq32:
	g++ -O2 -s -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB)

sqprof:
	g++ -O2 -pg  -pie -gstabs -g3 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB)
	
sq64:
	g++ -O2 -s -m64 -D_SQ64 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB)
#	g++ -g -m64 -D_SQ64 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB)
