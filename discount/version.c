#include "config.h"

#ifndef VERSION
#define VERSION "0.0"
#endif

char markdown_version[] = VERSION
#if 4 != 4
		" TAB=4"
#endif
#if USE_AMALLOC
		" DEBUG"
#endif
#if WITH_LATEX
		" LATEX"
#endif
		;
