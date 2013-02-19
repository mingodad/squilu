/*
 * configuration for markdown, generated Mon Dec  5 22:48:15 GMT 2011
 * by mingo2@mingov
 */
#ifndef __AC_MARKDOWN_D
#define __AC_MARKDOWN_D 1


#define OS_MINGW32_NT 1
#define USE_DISCOUNT_DL 1
#define DWORD unsigned long
#define WORD unsigned short
#define BYTE unsigned char
#define HAVE_SRAND 1
#define INITRNG(x) srand((unsigned int)x)
#define HAVE_BZERO 1
#define HAVE_RAND 1
#define COINTOSS() (rand()&1)
#define HAVE_STRCASECMP 1
#define HAVE_STRNCASECMP 1
#define HAVE_GETCWD 1
#define TABSTOP 4
#define HAVE_MALLOC_H 1
#define PATH_FIND "/bin/find"
#define PATH_SED "/bin/sed"

#endif/* __AC_MARKDOWN_D */
