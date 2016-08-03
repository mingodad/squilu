#ifndef _SUBLATIN_H_
#define  _SUBLATIN_H_

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned char deAccentMapTable[];
extern const unsigned char toLowerMapTable[];
extern const unsigned char toUpperMapTable[];
extern const unsigned char toLowerDeaccentedMapTable[];
extern const unsigned char isLowerMapTable[];
extern const unsigned char isUpperMapTable[];

#define deAccentMap(x) deAccentMapTable[((unsigned char)x)]
#define toLowerMap(x) toLowerMapTable[((unsigned char)x)]
#define toUpperMap(x) toUpperMapTable[((unsigned char)x)]
#define toLowerDeaccentedMap(x) toLowerDeaccentedMapTable[((unsigned char)x)]
#define isLowerMap(x) isLowerMapTable[((unsigned char)x)]
#define isUpperMap(x) isUpperMapTable[((unsigned char)x)]

//The functions bellow convert in place, overwiten passed paramters
int strLenSubSetLatinUtf8( const char *str );
int nstrLenSubSetLatinUtf8( const char *str, int n );
int isDeaccentableSubSetLatinUtf8( const char *str );
char *deAccentSubSetLatinUtf8( char *str );
char *toLowerDeaccentSubSetLatinUtf8( char *str );
char *toLowerSubSetLatinUtf8( char *str );
char *toUpperSubSetLatinUtf8( char *str );
int strICmpSubSetLatinUtf8( const char *zLeft, const char *zRight );
int strICmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight );
int strCmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight );

// the N parameter is expected to be utf8 chars count
int strNICmpSubSetLatinUtf8( const char *zLeft, const char *zRight, int n );
int strNICmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight, int n );
int strNCmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight, int n );

int subLatinLikeCompare(
	const char *zPattern,   //* LIKE pattern */
	const char *zString,    //* The UTF-8 string to compare against */
	char cEsc        //* The escape character */
);

int subLatinLikeCompareNoAccents(
	const char *zPattern,   //* LIKE pattern */
	const char *zString,    //* The UTF-8 string to compare against */
	char cEsc        //* The escape character */
);

#ifdef __cplusplus
}
#endif

#endif //_SUBLATIN_H_
