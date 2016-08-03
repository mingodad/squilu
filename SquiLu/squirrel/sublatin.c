#ifdef NEED_SUBLATIN_C
#include "sublatin.h"

//	AccentedChars 	= 		"ÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖØòóôõöøÙÚÛÜùúûüÝÿýÑñÇç"
//	deAccentedChars = 		"AAAAAAaaaaaaEEEEeeeeIIIIiiiiOOOOOOooooooUUUUuuuuYyyNnCc"
//	deAccentedLowerChars = 	"aaaaaaaaaaaaeeeeeeeeiiiiiiiioooooooooooouuuuuuuuyyynncc"
//	upperCaseAccentedChars = "ÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖØÙÚÛÜÝ"
//	lowerCaseAccentedChars = "àáâãäåèéêëìíîïòóôõöøùúûüý"
//	upperCaseChars = "ABCDEFGHIJKLMNOPQRSTUVWXZY"
//	lowerCaseChars = "abcdefghijklmnopqrstuvwxzy"


const unsigned char deAccentMapTable[] = { //0
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //16
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, //32
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, //48
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, //64
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, //80
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, //96
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, //112
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, //128
	65, 65, 65, 65, 65, 65, 134, 67, 69, 69, 69, 69, 73, 73, 73, 73, //144
	144, 78, 79, 79, 79, 79, 79, 151, 79, 85, 85, 85, 85, 89, 158, 159, //160
	97, 97, 97, 97, 97, 97, 166, 99, 101, 101, 101, 101, 105, 105, 105, 105, //176
	176, 110, 111, 111, 111, 111, 111, 183, 111, 117, 117, 117, 117, 121, 190, 121, //192
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, //208
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, //224
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, //240
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};


const unsigned char toLowerMapTable[] = { //0
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //16
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, //32
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, //48
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, //64
	64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, //80
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95, //96
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, //112
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, //128
	160, 161, 162, 163, 164, 165, 134, 167, 168, 169, 170, 171, 172, 173, 174, 175, //144
	144, 177, 178, 179, 180, 181, 182, 151, 184, 185, 186, 187, 188, 189, 158, 159, //160
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, //176
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, //192
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, //208
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, //224
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, //240
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};


const unsigned char toUpperMapTable[] = { //0
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, //16
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, //32
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, //48
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, //64
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, //80
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, //96
	96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, //112
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125, 126, 127, //128
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, //144
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, //160
	128, 129, 130, 131, 132, 133, 166, 135, 136, 137, 138, 139, 140, 141, 142, 143, //176
	176, 145, 146, 147, 148, 149, 150, 183, 152, 153, 154, 155, 156, 157, 190, 191, //192
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, //208
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, //224
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, //240
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};


const unsigned char toLowerDeaccentedMapTable[] = { //0
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,//16
16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,//32
32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,//48
48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,//64
64,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,//80
112,113,114,115,116,117,118,119,120,121,122,91,92,93,94,95,//96
96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,//112
112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,//128
97,97,97,97,97,97,134,99,101,101,101,101,105,105,105,105,//144
144,110,111,111,111,111,111,151,111,117,117,117,117,121,158,159,//160
97,97,97,97,97,97,166,99,101,101,101,101,105,105,105,105,//176
176,110,111,111,111,111,111,183,111,117,117,117,117,121,190,121,//192
192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,//208
208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,//224
224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,//240
240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};


const unsigned char isLowerMapTable[] = { //0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //16
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //32
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //48
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //64
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //80
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //96
	0, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, //112
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0, 0, 0, 0, 0, //128
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //144
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160
	160, 161, 162, 163, 164, 165, 0, 167, 168, 169, 170, 171, 172, 173, 174, 175, //176
	0, 177, 178, 179, 180, 181, 182, 0, 184, 185, 186, 187, 188, 189, 0, 0, //192
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //208
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //224
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


const unsigned char isUpperMapTable[] = { //0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //16
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //32
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //48
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //64
	0, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, //80
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 0, 0, 0, 0, 0, //96
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //112
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //128
	128, 129, 130, 131, 132, 133, 0, 135, 136, 137, 138, 139, 140, 141, 142, 143, //144
	0, 145, 146, 147, 148, 149, 150, 0, 152, 153, 154, 155, 156, 157, 0, 0, //160
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //176
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //192
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //208
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //224
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int nstrLenSubSetLatinUtf8( const char *str, int n )
{
	int result = 0;
	unsigned char *pstr = ( unsigned char * )str;

	while ( *pstr && (--n >= 0) ) {
		// an error when an utf8 char  is formed by 195#195 ???
		if ( *pstr != 195 ) ++result;

		++pstr;
	}

	return result;
}

int strLenSubSetLatinUtf8( const char *str ) {
	int result = 0;
	unsigned char *pstr = ( unsigned char * )str;

	while ( *pstr ) {
		// an error when an utf8 char  is formed by 195#195 ???
		if ( *pstr != 195 ) ++result;

		++pstr;
	}

	return result;
}

int isDeaccentableSubSetLatinUtf8( const char *str ) {
	unsigned int c1, c2;
	unsigned char *pstr = ( unsigned char * )str;

	if ( *pstr == 195 ) {
		c1 = *( pstr + 1 );
		c2 = deAccentMap( c1 );

		if (  c1 != c2 ) return c2;
	}

	return 0;
}

char *toLowerDeaccentSubSetLatinUtf8( char *str ) {

	unsigned char *ptrStr, *ptrStr2;

	if ( !str ) return str;

	ptrStr2 = ptrStr = ( unsigned char* )str;

	while ( *ptrStr ) {
		if ( *ptrStr == 195 ) ++ptrStr;

		*ptrStr2 = toLowerDeaccentedMap( *ptrStr );
		++ptrStr;
		++ptrStr2;
	}

	*ptrStr2 = 0;

	return str;
}

char *deAccentSubSetLatinUtf8( char *str ) {

	unsigned char *ptrStr, *ptrStr2;

	if ( !str ) return str;

	ptrStr2 = ptrStr = ( unsigned char* )str;

	while ( *ptrStr ) {
		if ( *ptrStr == 195 ) ++ptrStr;

		*ptrStr2 = deAccentMap( *ptrStr );
		++ptrStr;
		++ptrStr2;
	}

	*ptrStr2 = 0;

	return str;
}

char *toLowerSubSetLatinUtf8( char *str ) {
	unsigned char *ptrStr;

	if ( !str ) return str;

	ptrStr = ( unsigned char * )str;

	while ( *ptrStr ) {
		if ( *ptrStr == 195 ) ++ptrStr;

		*ptrStr = toLowerMap( *ptrStr );
		++ptrStr;
	}

	return str;
}

char *toUpperSubSetLatinUtf8( char *str ) {
	unsigned char *ptrStr;

	if ( !str ) return str;

	ptrStr = ( unsigned char * )str;

	while ( *ptrStr ) {
		if ( *ptrStr == 195 ) ++ptrStr;

		*ptrStr = toUpperMap( *ptrStr );
		++ptrStr;
	}

	return str;
}

#define checkToLowerSubSetLatinUtf8( a, b ) \
	if ( (*(unsigned char *)b) == 195 ) ++b;\
	a = toLowerMap(*b)

int strICmpSubSetLatinUtf8( const char *zLeft, const char *zRight ) {

	unsigned char *a, *b;
	unsigned char a2, b2;
	a2 = b2 = '\0';

	a = ( unsigned char* )zLeft;
	b = ( unsigned char* )zRight;

	while ( *a && *b) {
		checkToLowerSubSetLatinUtf8( a2, a );
		checkToLowerSubSetLatinUtf8( b2, b );

		if ( a2 != b2 ) break;

		a++;
		b++;
	}

	return a2 - b2;
}

#define  checkLowerDeAccentSubSetLatinUtf8( a, b ) \
	if ( (*(unsigned char *)b) == 195 ) b++;\
	a = toLowerDeaccentedMap(*b)

int strICmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight ) {
	unsigned char *a, *b;
	unsigned char a2, b2;
	a2 = b2 = '\0';

	a = ( unsigned char * )zLeft;
	b = ( unsigned char * )zRight;

	while ( *a && *b) {
		checkLowerDeAccentSubSetLatinUtf8( a2, a );
		checkLowerDeAccentSubSetLatinUtf8( b2, b );

		if ( a2 - b2 ) break;

		a++;
		b++;
	}

	return a2 - b2;
}

#define  checkDeAccentSubSetLatinUtf8( a, b ) \
	if ( (*(unsigned char *)b) == 195 ) b++;\
	a = deAccentMap(*b)

int strCmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight ) {
	unsigned char *a, *b;
	unsigned char a2, b2;
	a2 = b2 = '\0';

	a = ( unsigned char * )zLeft;
	b = ( unsigned char * )zRight;

	while ( *a && *b) {
		checkDeAccentSubSetLatinUtf8( a2, a );
		checkDeAccentSubSetLatinUtf8( b2, b );

		if ( a2 - b2 ) break;

		a++;
		b++;
	}

	return ( a2 - b2 );
}

// the N parameter is expected to be utf8 chars count
int strNICmpSubSetLatinUtf8( const char *zLeft, const char *zRight, int n ) {
	unsigned char *a, *b;
	unsigned char a2, b2;
	a2 = b2 = '\0';

	a = ( unsigned char * )zLeft;
	b = ( unsigned char * )zRight;

	while ( *a && *b ) {
		if ( --n < 0 ) break;

		checkToLowerSubSetLatinUtf8( a2, a );
		checkToLowerSubSetLatinUtf8( b2, b );

		if ( a2 != b2 ) break;

		a++;
		b++;
	}

	return ( a2 - b2 );
}

// the N parameter is expected to be utf8 chars count
int strNICmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight, int n ) {
	unsigned char *a, *b;
	unsigned char a2, b2;
	a2 = b2 = '\0';

	a = ( unsigned char * )zLeft;
	b = ( unsigned char * )zRight;

	while ( *a && *b) {
		if ( --n < 0 ) break;

		checkLowerDeAccentSubSetLatinUtf8( a2, a );
		checkLowerDeAccentSubSetLatinUtf8( b2, b );

		if ( a2 - b2 ) break;

		a++;
		b++;
	}

	return( a2 - b2 );
}

// the N parameter is expected to be utf8 chars count
int strNCmpSubSetLatinUtf8NoAccents( const char *zLeft, const char *zRight, int n ) {
	unsigned char *a, *b;
	unsigned char a2, b2;
	a2 = b2 = '\0';

	a = ( unsigned char * )zLeft;
	b = ( unsigned char * )zRight;

	while ( *a && *b) {
		if ( --n < 0 ) break;

		checkDeAccentSubSetLatinUtf8( a2, a );
		checkDeAccentSubSetLatinUtf8( b2, b );

		if ( a2 != b2 ) break;

		a++;
		b++;
	}

	return( a2 - b2 );
}

/*
** Compare two UTF-8 strings for equality where the first string is
** a "LIKE" expression. Return true (1) if they are the same and
** false (0) if they are different.
*/
static int subLatinLikeCompare0(
	const char *zPattern,   //* LIKE pattern */
	const char *zString,    //* The UTF-8 string to compare against */
	char cEsc,        //* The escape character */
	int deaccent	//if to compare deaccented
) {

	char MATCH_ONE, MATCH_ALL, cPattern, cString, c;
	int iPattern, iString;
	int prevEscape;

#define checkStringChar() \
	if( *(unsigned char*)(zString+iString) == 195 ) iString++;
#define nextStringChar() \
	iString++;\
	if( *(unsigned char*)(zString+iString) == 195 ) iString++;
#define checkPatternChar()\
	if( *(unsigned char*)(zPattern+iPattern) == 195 ) iPattern++;
#define nextPatternChar()\
	iPattern++;\
	if( *(unsigned char*)(zPattern+iPattern) == 195 ) iPattern++;

	MATCH_ONE = '_';
	MATCH_ALL = '%';

	iPattern = 0;       //* Current byte index in zPattern */
	iString = 0;        //* Current byte index in zString */

	prevEscape = 0;     //* True if the previous character was uEsc */

	//what if the pattern is a 195##195 ?????
	checkPatternChar();
	checkStringChar();

	while ( *( zPattern + iPattern ) ) {
		//* Read (and consume) the next character from the input pattern. */
		cPattern = *( zPattern + iPattern );
		nextPatternChar();
		//assert(uPattern!=0);

		/* There are now 4 possibilities:
		**
		**     1. uPattern is an unescaped match-all character "%",
		**     2. uPattern is an unescaped match-one character "_",
		**     3. uPattern is an unescaped escape character, or
		**     4. uPattern is to be handled as an ordinary character
		*/
		if ( ( ! prevEscape ) && ( cPattern == MATCH_ALL ) ) {
			///* Case 1. */

			/* Skip any MATCH_ALL or MATCH_ONE characters that follow a
			** MATCH_ALL. For each MATCH_ONE, skip one character in the
			** test string.
			*/
			c = *( zPattern + iPattern );

			while ( ( c == MATCH_ALL ) || ( c == MATCH_ONE ) ) {
				if ( c == MATCH_ONE ) {
					if ( !*( zString + iString ) ) return 0;

					//U8_FWD_1_UNSAFE(zString, iString);
					nextStringChar();
				}

				nextPatternChar();
				c = *( zPattern + iPattern );
			}

			if ( !*( zPattern + iPattern ) ) return 1;

			while ( *( zString + iString ) ) {
				if ( subLatinLikeCompare0( zPattern + iPattern, zString + iString, cEsc, deaccent ) ) return 1;

				//U8_FWD_1_UNSAFE(zString, iString);
				nextStringChar();
			}

			return 0;

		} else if ( ( ! prevEscape ) && ( cPattern == MATCH_ONE ) ) {
			//* Case 2. */
			if ( !*( zString + iString ) ) return 0;

			//U8_FWD_1_UNSAFE(zString, iString);
			nextStringChar();
		} else if ( ( ! prevEscape ) && ( cPattern == cEsc ) ) {
			//* Case 3. */
			prevEscape = 1;
		} else {
			//* Case 4. */
			//UChar32 uString;
			//U8_NEXT_UNSAFE(zString, iString, uString);
			cString = *( zString + iString );

			if ( deaccent ) {
				if ( toLowerDeaccentedMap( cString ) != toLowerDeaccentedMap( cPattern ) ) return 0;
			} else {
				if ( toLowerMap( cString ) != toLowerMap( cPattern ) ) return 0;
			}

			prevEscape = 0;
			nextStringChar();
		}
	}

	if ( !*( zString + iString ) ) return 1;
	else return 0;
}

int subLatinLikeCompare(
	const char *zPattern,   //* LIKE pattern */
	const char *zString,    //* The UTF-8 string to compare against */
	char cEsc        //* The escape character */
) {
	return subLatinLikeCompare0( zPattern, zString, cEsc, 0 );
}

int subLatinLikeCompareNoAccents(
	const char *zPattern,   //* LIKE pattern */
	const char *zString,    //* The UTF-8 string to compare against */
	char cEsc        //* The escape character */
) {
	return subLatinLikeCompare0( zPattern, zString, cEsc, 1 );
}
#endif //NEED_SUBLATIN_C
