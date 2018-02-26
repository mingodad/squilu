
SQUIRREL=.
MAKE=make CC=g++ CC_EXTRA_FLAGS="-flto -DNDEBUG -DONLY_ASCII -DNO_ABSTRACT_METHOD -DCUSTOM_DELETE_OPERATOR -DSQ_USE_MKSTEMP  -DSQ_ENABLE_INCLUDES -DSQ_SUBLATIN -DNEED_SUBLATIN_C -DSQUSEDOUBLE" \
	LD_EXTRA_FLAGS="-flto"
#MAKE=make CC=g++ CC_EXTRA_FLAGS="-fsanitize=address -DONLY_ASCII -DSQ_USE_MKSTEMP -DSQ_ENABLE_INCLUDES -DSQ_SUBLATIN -DNEED_SUBLATIN_C -DSQUSEDOUBLE"
#MAKE=make

DIRS=bin lib
$(shell mkdir -p $(DIRS))

sq32:
	cd squirrel; $(MAKE) 
	cd sqstdlib; $(MAKE) 
	cd sq; $(MAKE) 

sqprof:
	cd squirrel; $(MAKE) sqprof
	cd sqstdlib; $(MAKE) sqprof
	cd sq; $(MAKE) sqprof

sq64:
	cd squirrel; $(MAKE) sq64
	cd sqstdlib; $(MAKE) sq64
	cd sq; $(MAKE) sq64
	
clean:
	rm lib/*
	rm bin/*
