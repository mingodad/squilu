/**
LuaXML License

LuaXml is licensed under the terms of the MIT license reproduced below,
the same as Lua itself. This means that LuaXml is free software and can be
used for both academic and commercial purposes at absolutely no cost.

Copyright (C) 2007-2010 Gerald Franz, www.viremo.de

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Ported to Squirrel/SquiLu by Domingo Alvarez Duarte 22/01/2012

*/

#include "squirrel.h"
#include "sqstdblobimpl.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

SQ_OPT_STRING_STRLEN();

static const SQChar ESC=27;
static const SQChar OPN=28;
static const SQChar CLS=29;

//--- auxliary functions -------------------------------------------

static const SQChar* char2code(USQChar ch, SQChar buf[8]) {
	USQChar i=0;
	buf[i++]=_SC('&');
	buf[i++]=_SC('#');
	if(ch>99) buf[i++]=ch/100+48;
	if(ch>9) buf[i++]=(ch%100)/10+48;
	buf[i++]=ch%10+48;
	buf[i++]=_SC(';');
	buf[i]=0;
	return buf;
}

static size_t find(const SQChar* s, const SQChar* pattern, size_t start) {
	const SQChar* found =scstrstr(s+start, pattern);
	return found ? found-s : scstrlen(s);
}

//--- internal tokenizer -------------------------------------------

typedef struct Tokenizer_s  {
	/// stores string to be tokenized
	const SQChar* s;
	/// stores size of string to be tokenized
	size_t s_size;
	/// stores current read position
	size_t i;
	/// stores current read context
	int tagMode;
	/// stores next token, if already determined
	const SQChar* m_next;
	/// size of next token
	size_t m_next_size;
	/// pointer to current token
	SQChar* m_token;
	/// size of current token
	size_t m_token_size;
	/// capacity of current token
	size_t m_token_capacity;
} Tokenizer;

Tokenizer* Tokenizer_new(const SQChar* str, size_t str_size) {
	Tokenizer *tok = (Tokenizer*)sq_malloc(sizeof(Tokenizer));
	memset(tok, 0, sizeof(Tokenizer));
	tok->s_size = str_size;
	tok->s = str;
	return tok;
}

void Tokenizer_delete(Tokenizer* tok) {
	sq_free(tok->m_token);
	sq_free(tok);
}

//void Tokenizer_print(Tokenizer* tok) { printf("  @%u %s\n", tok->i, !tok->m_token ? "(null)" : (tok->m_token[0]==ESC)?"(esc)" : (tok->m_token[0]==OPN)?"(open)": (tok->m_token[0]==CLS)?"(close)" : tok->m_token); fflush(stdout); }

static const SQChar* Tokenizer_set(Tokenizer* tok, const SQChar* s, size_t size) {
	if(!size||!s) return 0;
	free(tok->m_token);
	tok->m_token = (SQChar*)sq_malloc(size+1);
	scstrncpy(tok->m_token,s, size);
	tok->m_token[size] = 0;
	tok->m_token_size = tok->m_token_capacity = size;
	//Tokenizer_print(tok);
	return tok->m_token;
}

static void Tokenizer_append(Tokenizer* tok, SQChar ch) {
	if(tok->m_token_size+1>=tok->m_token_capacity) {
		tok->m_token_capacity = (tok->m_token_capacity==0) ? 16 : tok->m_token_capacity*2;
		tok->m_token = (SQChar*)sq_realloc(tok->m_token, tok->m_token_capacity);
	}
	tok->m_token[tok->m_token_size]=ch;
	tok->m_token[++tok->m_token_size]=0;
}

const SQChar* Tokenizer_next(Tokenizer* tok) {
	const SQChar* ESC_str = _SC("\033");
	const SQChar* OPEN_str = _SC("\034");
	const SQChar* CLOSE_str = _SC("\035");


	if(tok->m_token) {
		sq_free(tok->m_token);
		tok->m_token = 0;
		tok->m_token_size=tok->m_token_capacity = 0;
	}

	int quotMode=0;
	int tokenComplete = 0;
	while(tok->m_next_size || (tok->i < tok->s_size)) {

		if(tok->m_next_size) {
			Tokenizer_set(tok, tok->m_next, tok->m_next_size);
			tok->m_next=0;
			tok->m_next_size=0;
			return tok->m_token;
		}

		switch(tok->s[tok->i]) {
		    case _SC('"'):
		    case _SC('\''):
			if(tok->tagMode) {
				if(!quotMode) quotMode=tok->s[tok->i];
				else if(quotMode==tok->s[tok->i]) quotMode=0;
			}
			Tokenizer_append(tok, tok->s[tok->i]);
			break;
		    case _SC('<'):
			if(!quotMode&&(tok->i+4<tok->s_size)&&(scstrncmp(tok->s+tok->i,_SC("<!--"),4)==0)) // strip comments
			    tok->i=find(tok->s, _SC("-->"), tok->i+4)+2;
			else if(!quotMode&&(tok->i+9<tok->s_size)&&(scstrncmp(tok->s+tok->i,_SC("<![CDATA["),9)==0)) { // interpret CDATA
			    size_t b=tok->i+9;
			    tok->i=find(tok->s, _SC("]]>"),b)+3;
			    if(!tok->m_token_size) return Tokenizer_set(tok, tok->s+b, tok->i-b-3);
			    tokenComplete = 1;
			    tok->m_next = tok->s+b;
			    tok->m_next_size = tok->i-b-3;
			    --tok->i;
			}
			else if(!quotMode&&(tok->i+1<tok->s_size)&&((tok->s[tok->i+1]==_SC('?'))||(tok->s[tok->i+1]==_SC('!')))) // strip meta information
			    tok->i=find(tok->s, _SC(">"), tok->i+2);
			else if(!quotMode&&!tok->tagMode) {
				if((tok->i+1<tok->s_size)&&(tok->s[tok->i+1]==_SC('/'))) {
					tok->m_next=ESC_str;
					tok->m_next_size = 1;
					tok->i=find(tok->s, _SC(">"), tok->i+2);
				}
				else {
					tok->m_next = OPEN_str;
					tok->m_next_size = 1;
					tok->tagMode=1;
				}
				tokenComplete = 1;
			}
			else Tokenizer_append(tok, tok->s[tok->i]);
			break;
		    case _SC('/'):
			if(tok->tagMode&&!quotMode) {
				tokenComplete = 1;
				if((tok->i+1 < tok->s_size) && (tok->s[tok->i+1]==_SC('>'))) {
					tok->tagMode=0;
					tok->m_next=ESC_str;
					tok->m_next_size = 1;
					++tok->i;
				}
				else Tokenizer_append(tok, tok->s[tok->i]);
			}
			else Tokenizer_append(tok, tok->s[tok->i]);
			break;
		    case _SC('>'):
			if(!quotMode&&tok->tagMode) {
				tok->tagMode=0;
				tokenComplete = 1;
				tok->m_next = CLOSE_str;
				tok->m_next_size = 1;
			}
			else Tokenizer_append(tok, tok->s[tok->i]);
			break;
		    case _SC(' '):
		    case _SC('\r'):
		    case _SC('\n'):
		    case _SC('\t'):
			if(tok->tagMode&&!quotMode) {
			    if(tok->m_token_size) tokenComplete=1;
			}
			else if(tok->m_token_size) Tokenizer_append(tok, tok->s[tok->i]);
			break;
		    default: Tokenizer_append(tok, tok->s[tok->i]);
		}
		++tok->i;
		if((tok->i>=tok->s_size)||(tokenComplete&&tok->m_token_size)) {
			tokenComplete=0;
			while(tok->m_token_size&&isspace(tok->m_token[tok->m_token_size-1])) // trim whitespace
				tok->m_token[--tok->m_token_size]=0;
			if(tok->m_token_size) break;
		}
	}
	//Tokenizer_print(tok);
	return tok->m_token;
}

//--- local variables ----------------------------------------------

/// stores number of special character codes
static size_t sv_code_size=0;
/// stores currently allocated capacity for special character codes
static size_t sv_code_capacity=16;
/// stores code table for special characters
static SQChar** sv_code=0;


static const SQChar *sq_str_replace (HSQUIRRELVM v, const SQChar *s, const SQChar *p, const SQChar *r) {
  const SQChar *wild;
  size_t l = scstrlen(p);
  SQBlob b(0, BLOB_BUFSIZE);
  while ((wild = scstrstr(s, p)) != NULL) {
    b.Write(s, wild - s);  /* push prefix */
    b.WriteZstr(r);  /* push replacement in place of pattern */
    s = wild + l;  /* continue after `p' */
  }
  b.WriteZstr(s);  /* push last suffix */
  sq_pushstring(v, (const SQChar*)b.GetBuf(), b.Len());
  return sq_tostring(v, -1);
}

//--- public methods -----------------------------------------------

static void Xml_pushDecode(HSQUIRRELVM v, const char* s, size_t s_size) {
    SQ_FUNC_VARS_NO_TOP(v);
	size_t start=0, pos;
	if(!s_size) s_size=strlen(s);
	SQBlob b(0, BLOB_BUFSIZE);
	const char* found = scstrstr(s, _SC("&#"));
	if(!found) pos = s_size;
	else pos = found-s;
	while(found&&(pos+5<s_size)&&(*(found+5)==_SC(';'))&&scisdigit(*(found+2))&&scisdigit(*(found+3))&&scisdigit(*(found+4)) ) {
		if(pos>start) b.Write(s+start, pos-start);
		b.WriteChar(100*(s[pos+2]-48)+10*(s[pos+3]-48)+(s[pos+4]-48));
		start=pos+6;
		found = scstrstr(found+6, _SC("&#"));
		if(!found) pos = s_size;
		else pos = found-s;
	}
	if(pos>start) b.Write(s+start, pos-start);
	sq_pushstring(v, (const SQChar*)b.GetBuf(), b.Len());
	size_t i;
	for(i=sv_code_size-1; i<sv_code_size; i-=2) {
	    SQ_GET_STRING(v, -1, str);
		sq_str_replace(v, str, sv_code[i], sv_code[i-1]);
		sq_remove(v,-2);
	}
}

int Xml_eval(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
	SQChar* str = 0;
	size_t str_size=0;
	if(sq_gettype(v,2) == OT_USERPOINTER) sq_getuserpointer(v, 2, &str);
	else {
	    SQ_GET_STRING(v, 2, sTmp);
		str = (SQChar*)sq_malloc(sTmp_size+(sizeof(SQChar)));
		memcpy(str, sTmp, sTmp_size);
		str[sTmp_size]=0;
		str_size = sTmp_size;
	}
	Tokenizer* tok = Tokenizer_new(str, str_size ? str_size : scstrlen(str));
	sq_settop(v,0);
	const SQChar* token=0;
	int firstStatement = 1;
	while((token=Tokenizer_next(tok))!=0) if(token[0]==OPN) { // new tag found
		if(sq_gettop(v)) {
			int newIndex=sq_size(v,-1)+1;
			sq_pushinteger(v,newIndex);
			sq_newtable(v);
			sq_set(v, -3);
			sq_pushinteger(v,newIndex);
			sq_get(v,-2);
		}
		else {
			if (firstStatement) {
				sq_newtable(v);
				firstStatement = 0;
			}
			else return lua_gettop(L);
		}
		// set metatable:
		sq_newtable(v);
		sq_pushliteral(v, _SC("__index"));
		sq_getglobal(v, "xml");
		lua_settable(v, -3);

		sq_pushliteral(v, _SC("__tostring")); // set __tostring metamethod
		lua_getglobal(L, "xml");
		lua_pushliteral(L,"str");
		lua_gettable(v, -2);
		sq_remove(v, -2);
		sq_set(v, -3);
		lua_setmetatable(L, -2);

		// parse tag and content:
		sq_pushinteger(v,0); // use index 0 for storing the tag
		sq_pushstring(v, Tokenizer_next(tok), -1);
		sq_set(v, -3);

		while(((token = Tokenizer_next(tok))!=0)&&(token[0]!=CLS)&&(token[0]!=ESC)) { // parse tag header
			size_t sepPos=find(token, "=", 0);
			if(token[sepPos]) { // regular attribute
				const SQChar* aVal =token+sepPos+2;
				sq_pushstring(v, token, sepPos);
				size_t lenVal = strlen(aVal)-1;
				if(!lenVal) Xml_pushDecode(v, _SC(""), 0);
				else Xml_pushDecode(v, aVal, lenVal);
				sq_set(v, -3);
			}
		}
		if(!token||(token[0]==ESC)) {
			if(sq_gettop(v)>1) sq_settop(v,-2); // this tag has no content, only attributes
			else break;
		}
	}
	else if(token[0]==ESC) { // previous tag is over
		if(sq_gettop(v)>1) sq_settop(v,-2); // pop current table
		else break;
	}
	else { // read elements
		sq_pushinteger(v,sq_size(v,-1)+1);
		Xml_pushDecode(v, token, 0);
		sq_rawset(v, -3);
	}
	Tokenizer_delete(tok);
	sq_free(str);
	return sq_gettop(v);
}

int Xml_load (HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, filename);
	FILE * file=fopen(filename,"r");
	if(!file) return sq_throwerror(v, _SC("SQXml ERROR: \"%s\" file error or file not found!"),filename);

	fseek (file , 0 , SEEK_END);
	size_t sz = ftell (file);
	rewind (file);
	char* buffer = (char*)sq_malloc(sz+1);
	sz = fread (buffer,1,sz,file);
	fclose(file);
	buffer[sz]=0;
	sq_pushuserpointer(v,buffer);
	sq_replace(v,2);
	return Xml_eval(v);
};

int Xml_registerCode(HSQUIRRELVM v) {
    SQ_FUNC_VARS_NO_TOP(v);
    SQ_GET_STRING(v, 2, decoded);
    SQ_GET_STRING(v, 3, encoded);

    size_t i;
    for(i=0; i<sv_code_size; i+=2)
        if(scstrcmp(sv_code[i],decoded)==0)
            return sq_throwerror(v,_SC"SQXml ERROR: code already exists."));
	if(sv_code_size+2>sv_code_capacity) {
	    size_t old_capacity = sv_code_capacity;
		sv_code_capacity*=2;
		sv_code = (SQChar**)sq_realloc(sv_code, old_capacity*sizeof(SQChar*), sv_code_capacity*sizeof(SQChar*));
	}
	sv_code[sv_code_size]=(SQChar*)sq_malloc((scstrlen(decoded)+1)*sizeof(SQChar*));
	strcpy(sv_code[sv_code_size++], decoded);
	sv_code[sv_code_size]=(SQChar*)sq_malloc((scstrlen(decoded)+1)*sizeof(SQChar*));
	strcpy(sv_code[sv_code_size++],encoded);
    return 0;
}

int Xml_encode(HSQUIRRELVM v) {
    SQ_FUNC_VARS(v);
	SQ_GET_STRING(v, 2, s);
	size_t i;
	for(i=0; i<sv_code_size; i+=2) {
		sq_str_replace(v, s, sv_code[i], sv_code[i+1]);
		sq_remove(v,-2);
	}
	SQChar buf[8];
	SQ_GET_STRING(v, 2, s);
	size_t start, pos;
	SQBlob b(0, BLOB_BUFSIZE);
	for(start=pos=0; s[pos]!=0; ++pos) if(s[pos]<0) {
		if(pos>start) b.Write(s+start, pos-start);
		b.WriteZstr(char2code(uchar(s[pos]),buf));
		start=pos+1;
	}
	if(pos>start) b.Write(s+start, pos-start);
	sq_pushstring(v, (const SQChar*)b.GetBuf(), b.Len());
    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),base64_##name,nparams,tycheck}
static SQRegFunction xml_methods[] =
{
    _DECL_FUNC(load,2,_SC(".s")),
    _DECL_FUNC(eval,2,_SC(".s")),
    _DECL_FUNC(encode,2,_SC(".s")),
    _DECL_FUNC(registerCode,2,_SC(".s")),
    {0,0}
};

#ifdef __cplusplus
extern "C" {
#endif

SQRESULT sqext_register_sqxml(HSQUIRRELVM v)
{
    sq_pushstring(v,_SC("SQXml"),-1);
    sq_newclass(v,SQFalse);
    sq_insert_reg_funcs(v, xml_methods);
    sq_newslot(v,-3,SQTrue);

	// register default codes:
	if(!sv_code) {
		sv_code=(SQChar**)sq_malloc(sv_code_capacity*sizeof(SQChar*));
		sv_code[sv_code_size++]=_SC("&");
		sv_code[sv_code_size++]=_SC("&amp;");
		sv_code[sv_code_size++]=_SC("<");
		sv_code[sv_code_size++]=_SC("&lt;");
		sv_code[sv_code_size++]=_SC(">");
		sv_code[sv_code_size++]=_SC("&gt;");
		sv_code[sv_code_size++]=_SC("\"");
		sv_code[sv_code_size++]=_SC("&quot;");
		sv_code[sv_code_size++]=_SC("'");
		sv_code[sv_code_size++]=_SC("&apos;");
	}
    return 1;
}

#ifdef __cplusplus
}
#endif
