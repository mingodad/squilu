#ifdef __cplusplus
extern "C" {
#endif

int delta_analyze(const char *zDelta,int lenDelta,int *pnCopy,int *pnInsert);
int delta_apply(const char *zSrc,int lenSrc,const char *zDelta,int lenDelta,char *zOut);
int delta_output_size(const char *zDelta,int lenDelta);
int delta_create(const char *zSrc,unsigned int lenSrc,const char *zOut,unsigned int lenOut,char *zDelta);

#define DELTA_OVERFLOW 60

#define fossil_free(p) free(p)
#define fossil_malloc(n) malloc(n)

#ifdef __cplusplus
}
#endif
