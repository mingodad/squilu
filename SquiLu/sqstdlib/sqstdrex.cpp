/* see copyright notice in squirrel.h */
#include <squirrel.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <sqstdstring.h>

#ifdef _UINCODE
#define scisprint iswprint
#else
#define scisprint isprint
#endif

#ifdef _DEBUG
#include <stdio.h>

static const SQChar *g_nnames[] =
{
	_SC("NONE"),_SC("OP_GREEDY"),	_SC("OP_OR"),
	_SC("OP_EXPR"),_SC("OP_NOCAPEXPR"),_SC("OP_DOT"),	_SC("OP_CLASS"),
	_SC("OP_CCLASS"),_SC("OP_NCLASS"),_SC("OP_RANGE"),_SC("OP_CHAR"),
	_SC("OP_EOL"),_SC("OP_BOL"),_SC("OP_WB"),_SC("OP_MB")
};

#endif

#define OP_GREEDY		(MAX_CHAR+1) // * + ? {n}
#define OP_OR			(MAX_CHAR+2)
#define OP_EXPR			(MAX_CHAR+3) //parentesis ()
#define OP_NOCAPEXPR	(MAX_CHAR+4) //parentesis (?:)
#define OP_DOT			(MAX_CHAR+5)
#define OP_CLASS		(MAX_CHAR+6)
#define OP_CCLASS		(MAX_CHAR+7)
#define OP_NCLASS		(MAX_CHAR+8) //negates class the [^
#define OP_RANGE		(MAX_CHAR+9)
#define OP_CHAR			(MAX_CHAR+10)
#define OP_EOL			(MAX_CHAR+11)
#define OP_BOL			(MAX_CHAR+12)
#define OP_WB			(MAX_CHAR+13)
#define OP_MB           (MAX_CHAR+14) //match balanced
#define OP_EMPTY        (MAX_CHAR+15) //match position

#define SQREX_SYMBOL_ANY_CHAR (_SC('.'))
#define SQREX_SYMBOL_GREEDY_ONE_OR_MORE (_SC('+'))
#define SQREX_SYMBOL_GREEDY_ZERO_OR_MORE (_SC('*'))
#define SQREX_SYMBOL_GREEDY_ZERO_OR_ONE (_SC('?'))
#define SQREX_SYMBOL_BRANCH (_SC('|'))
#define SQREX_SYMBOL_END_OF_STRING (_SC('$'))
#define SQREX_SYMBOL_BEGINNING_OF_STRING (_SC('^'))
#define SQREX_SYMBOL_ESCAPE_CHAR (_SC('\\'))


typedef int SQRexNodeType;

typedef struct tagSQRexNode{
	SQRexNodeType type;
	SQInteger left;
	SQInteger right;
	SQInteger next;
}SQRexNode;

struct SQRex{
	const SQChar *_eol;
	const SQChar *_bol;
	const SQChar *_p;
	SQInteger _first;
	SQInteger _op;
	SQRexNode *_nodes;
	SQInteger _nallocated;
	SQInteger _nsize;
	SQInteger _nsubexpr;
	SQRexMatch *_matches;
	SQInteger _currsubexp;
	void *_jmpbuf;
	const SQChar **_error;
};

static SQInteger sqstd_rex_list(SQRex *exp);

static SQInteger sqstd_rex_newnode(SQRex *exp, SQRexNodeType type)
{
	SQRexNode n;
	n.type = type;
	n.next = n.right = n.left = -1;
	if((type == OP_EXPR) || (type == OP_EMPTY))
		n.right = exp->_nsubexpr++;
	if(exp->_nallocated < (exp->_nsize + 1)) {
		SQInteger oldsize = exp->_nallocated;
		exp->_nallocated *= 2;
		exp->_nodes = (SQRexNode *)sq_realloc(exp->_nodes, oldsize * sizeof(SQRexNode) ,exp->_nallocated * sizeof(SQRexNode));
	}
	exp->_nodes[exp->_nsize++] = n;
	SQInteger newid = exp->_nsize - 1;
	return (SQInteger)newid;
}

static void sqstd_rex_error(SQRex *exp,const SQChar *error)
{
	if(exp->_error) *exp->_error = error;
	longjmp(*((jmp_buf*)exp->_jmpbuf),-1);
}

static void sqstd_rex_expect(SQRex *exp, SQInteger n){
	if((*exp->_p) != n)
		sqstd_rex_error(exp, _SC("expected paren"));
	exp->_p++;
}

static SQChar sqstd_rex_escapechar(SQRex *exp)
{
	if(*exp->_p == SQREX_SYMBOL_ESCAPE_CHAR){
		exp->_p++;
		switch(*exp->_p) {
		case _SC('v'): exp->_p++; return _SC('\v');
		case _SC('n'): exp->_p++; return _SC('\n');
		case _SC('t'): exp->_p++; return _SC('\t');
		case _SC('r'): exp->_p++; return _SC('\r');
		case _SC('f'): exp->_p++; return _SC('\f');
		case _SC('z'): exp->_p++; return _SC('0');
		default: return (*exp->_p++);
		}
	}
#ifdef SQ_REXPATTERN_ONLY_PRINTABLE
	else if(!scisprint(*exp->_p)) sqstd_rex_error(exp,_SC("letter expected"));
#endif
	return (*exp->_p++);
}

static SQInteger sqstd_rex_charclass(SQRex *exp,SQInteger classid)
{
	SQInteger n = sqstd_rex_newnode(exp,OP_CCLASS);
	exp->_nodes[n].left = classid;
	return n;
}

static SQInteger sqstd_rex_charnode(SQRex *exp,SQBool isclass)
{
	SQChar t;
	if(*exp->_p == SQREX_SYMBOL_ESCAPE_CHAR) {
		exp->_p++;
		switch(*exp->_p) {
			case _SC('n'): exp->_p++; return sqstd_rex_newnode(exp,_SC('\n'));
			case _SC('t'): exp->_p++; return sqstd_rex_newnode(exp,_SC('\t'));
			case _SC('r'): exp->_p++; return sqstd_rex_newnode(exp,_SC('\r'));
			case _SC('f'): exp->_p++; return sqstd_rex_newnode(exp,_SC('\f'));
			case _SC('v'): exp->_p++; return sqstd_rex_newnode(exp,_SC('\v'));
			case _SC('a'): case _SC('A'): case _SC('w'): case _SC('W'): case _SC('s'): case _SC('S'):
			case _SC('d'): case _SC('D'): case _SC('x'): case _SC('X'): case _SC('c'): case _SC('C'):
			case _SC('p'): case _SC('P'): case _SC('l'): case _SC('u'):
				{
				t = *exp->_p; exp->_p++;
				return sqstd_rex_charclass(exp,t);
				}
            case _SC('m'):
                {
                     SQChar cb, ce; //cb = character begin match ce = character end match
                     cb = *++exp->_p; //skip _SC('m')
                     ce = *++exp->_p;
                     exp->_p++; //points to the next char to be parsed
                     if ((!cb) || (!ce)) sqstd_rex_error(exp,_SC("balanced chars expected"));
                     if ( cb == ce ) sqstd_rex_error(exp,_SC("open/close char can't be the same"));
                     SQInteger node =  sqstd_rex_newnode(exp,OP_MB);
                     exp->_nodes[node].left = cb;
                     exp->_nodes[node].right = ce;
                     return node;
                }
			case _SC('b'):
			case _SC('B'):
				if(!isclass) {
					SQInteger node = sqstd_rex_newnode(exp,OP_WB);
					exp->_nodes[node].left = *exp->_p;
					exp->_p++;
					return node;
				} //else default
			default:
				t = *exp->_p; exp->_p++;
				return sqstd_rex_newnode(exp,t);
		}
	}
	//else if(!scisprint(*exp->_p)) {
#ifdef SQ_REXPATTERN_ONLY_PRINTABLE
	else if(((SQUChar)*exp->_p) < _SC(' ')) {
		sqstd_rex_error(exp,_SC("letter expected"));
	}
#endif
	t = *exp->_p; exp->_p++;
	return sqstd_rex_newnode(exp,t);
}
static SQInteger sqstd_rex_class(SQRex *exp)
{
	SQInteger ret = -1;
	SQInteger first = -1,chain;
	if(*exp->_p == SQREX_SYMBOL_BEGINNING_OF_STRING){
		ret = sqstd_rex_newnode(exp,OP_NCLASS);
		exp->_p++;
	}else ret = sqstd_rex_newnode(exp,OP_CLASS);

	if(*exp->_p == _SC(']')) sqstd_rex_error(exp,_SC("empty class"));
	chain = ret;
	while(*exp->_p != _SC(']') && exp->_p != exp->_eol) {
		if(*exp->_p == _SC('-') && first != -1){
			SQInteger r;
			if(*exp->_p++ == _SC(']')) sqstd_rex_error(exp,_SC("unfinished range"));
			r = sqstd_rex_newnode(exp,OP_RANGE);
			if(exp->_nodes[first].type>*exp->_p) sqstd_rex_error(exp,_SC("invalid range"));
			if(exp->_nodes[first].type == OP_CCLASS) sqstd_rex_error(exp,_SC("cannot use character classes in ranges"));
			exp->_nodes[r].left = exp->_nodes[first].type;
			SQInteger t = sqstd_rex_escapechar(exp);
			exp->_nodes[r].right = t;
            exp->_nodes[chain].next = r;
			chain = r;
			first = -1;
		}
		else{
			if(first!=-1){
				SQInteger c = first;
				exp->_nodes[chain].next = c;
				chain = c;
				first = sqstd_rex_charnode(exp,SQTrue);
			}
			else{
				first = sqstd_rex_charnode(exp,SQTrue);
			}
		}
	}
	if(first!=-1){
		SQInteger c = first;
		exp->_nodes[chain].next = c;
	}
	/* hack? */
	exp->_nodes[ret].left = exp->_nodes[ret].next;
	exp->_nodes[ret].next = -1;
	return ret;
}

static SQInteger sqstd_rex_parsenumber(SQRex *exp)
{
	SQInteger ret = *exp->_p-_SC('0');
	SQInteger positions = 10;
	exp->_p++;
	while(isdigit(*exp->_p)) {
		ret = ret*10+(*exp->_p++-_SC('0'));
		if(positions==1000000000) sqstd_rex_error(exp,_SC("overflow in numeric constant"));
		positions *= 10;
	};
	return ret;
}

static SQInteger sqstd_rex_element(SQRex *exp)
{
	SQInteger ret = -1;
	switch(*exp->_p)
	{
	case _SC('('): {
		SQInteger expr;
		exp->_p++;


		if(*exp->_p ==_SC('?')) {
			exp->_p++;
			sqstd_rex_expect(exp,_SC(':'));
			expr = sqstd_rex_newnode(exp,OP_NOCAPEXPR);
		}
		else if(*exp->_p ==_SC(')'))
        {
            exp->_p++;
			expr = sqstd_rex_newnode(exp,OP_EMPTY);
			if(*exp->_p !=_SC('\0'))
            {
                SQInteger newn = sqstd_rex_list(exp);
                exp->_nodes[expr].next = newn;
            }
            ret = expr;
            break;
        }
		else
			expr = sqstd_rex_newnode(exp,OP_EXPR);
            SQInteger newn = sqstd_rex_list(exp);
            exp->_nodes[expr].left = newn;
            ret = expr;
            sqstd_rex_expect(exp,_SC(')'));
        }
        break;
	case _SC('['):
		exp->_p++;
		ret = sqstd_rex_class(exp);
		sqstd_rex_expect(exp,_SC(']'));
		break;
	case SQREX_SYMBOL_END_OF_STRING: exp->_p++; ret = sqstd_rex_newnode(exp,OP_EOL);break;
	case SQREX_SYMBOL_ANY_CHAR: exp->_p++; ret = sqstd_rex_newnode(exp,OP_DOT);break;
	default:
		ret = sqstd_rex_charnode(exp,SQFalse);
		break;
	}


	SQBool isgreedy = SQFalse;
	unsigned short p0 = 0, p1 = 0;
	switch(*exp->_p){
		case SQREX_SYMBOL_GREEDY_ZERO_OR_MORE: p0 = 0; p1 = 0xFFFF; exp->_p++; isgreedy = SQTrue; break;
		case SQREX_SYMBOL_GREEDY_ONE_OR_MORE: p0 = 1; p1 = 0xFFFF; exp->_p++; isgreedy = SQTrue; break;
		case SQREX_SYMBOL_GREEDY_ZERO_OR_ONE: p0 = 0; p1 = 1; exp->_p++; isgreedy = SQTrue; break;
		case _SC('{'):
			exp->_p++;
			if(!isdigit(*exp->_p)) sqstd_rex_error(exp,_SC("number expected"));
			p0 = (unsigned short)sqstd_rex_parsenumber(exp);
			/*******************************/
			switch(*exp->_p) {
                case _SC('}'):
                    p1 = p0; exp->_p++;
                    break;
                case _SC(','):
                    exp->_p++;
                    p1 = 0xFFFF;
                    if(isdigit(*exp->_p)){
                        p1 = (unsigned short)sqstd_rex_parsenumber(exp);
                    }
                    sqstd_rex_expect(exp,_SC('}'));
                    break;
                default:
                    sqstd_rex_error(exp,_SC(", or } expected"));
			}
			/*******************************/
			isgreedy = SQTrue;
			break;

	}
	if(isgreedy) {
		SQInteger nnode = sqstd_rex_newnode(exp,OP_GREEDY);
		exp->_nodes[nnode].left = ret;
		exp->_nodes[nnode].right = ((p0)<<16)|p1;
		ret = nnode;
	}

	if((*exp->_p != SQREX_SYMBOL_BRANCH) && (*exp->_p != _SC(')')) && (*exp->_p != SQREX_SYMBOL_GREEDY_ZERO_OR_MORE) && (*exp->_p != SQREX_SYMBOL_GREEDY_ONE_OR_MORE) && (*exp->_p != _SC('\0'))) {
		SQInteger nnode = sqstd_rex_element(exp);
		exp->_nodes[ret].next = nnode;
	}

	return ret;
}

static SQInteger sqstd_rex_list(SQRex *exp)
{
	SQInteger ret=-1,e;
	if(*exp->_p == SQREX_SYMBOL_BEGINNING_OF_STRING) {
		exp->_p++;
		ret = sqstd_rex_newnode(exp,OP_BOL);
	}
	e = sqstd_rex_element(exp);
	if(ret != -1) {
		exp->_nodes[ret].next = e;
	}
	else ret = e;

	if(*exp->_p == SQREX_SYMBOL_BRANCH) {
		SQInteger temp,tright;
		exp->_p++;
		temp = sqstd_rex_newnode(exp,OP_OR);
		exp->_nodes[temp].left = ret;
		tright = sqstd_rex_list(exp);
		exp->_nodes[temp].right = tright;
		ret = temp;
	}
	return ret;
}

static inline bool isChClassWord(int c)
{
    return (isalnum(c) || c == _SC('_'));
}

static SQBool sqstd_rex_matchcclass(SQInteger cclass,SQChar c)
{
	switch(cclass) {
	case _SC('a'): return isalpha(c)?SQTrue:SQFalse;
	case _SC('A'): return !isalpha(c)?SQTrue:SQFalse;
	case _SC('w'): return isChClassWord(c)?SQTrue:SQFalse;
	case _SC('W'): return !isChClassWord(c)?SQTrue:SQFalse;
	case _SC('s'): return isspace(c)?SQTrue:SQFalse;
	case _SC('S'): return !isspace(c)?SQTrue:SQFalse;
	case _SC('d'): return isdigit(c)?SQTrue:SQFalse;
	case _SC('D'): return !isdigit(c)?SQTrue:SQFalse;
	case _SC('x'): return isxdigit(c)?SQTrue:SQFalse;
	case _SC('X'): return !isxdigit(c)?SQTrue:SQFalse;
	case _SC('c'): return iscntrl(c)?SQTrue:SQFalse;
	case _SC('C'): return !iscntrl(c)?SQTrue:SQFalse;
	case _SC('p'): return ispunct(c)?SQTrue:SQFalse;
	case _SC('P'): return !ispunct(c)?SQTrue:SQFalse;
	case _SC('l'): return islower(c)?SQTrue:SQFalse;
	case _SC('u'): return isupper(c)?SQTrue:SQFalse;
	}
	return SQFalse; /*cannot happen*/
}

static SQBool sqstd_rex_matchclass(SQRex* exp,SQRexNode *node,SQChar c)
{
	do {
		switch(node->type) {
			case OP_RANGE:
				if(c >= node->left && c <= node->right) return SQTrue;
				break;
			case OP_CCLASS:
				if(sqstd_rex_matchcclass(node->left,c)) return SQTrue;
				break;
			default:
				if(c == node->type)return SQTrue;
		}
	} while((node->next != -1) && (node = &exp->_nodes[node->next]));
	return SQFalse;
}

static const SQChar *sqstd_rex_matchnode(SQRex* exp,SQRexNode *node,const SQChar *str,SQRexNode *next)
{

	SQRexNodeType type = node->type;
	switch(type) {
	case OP_GREEDY: {
		//SQRexNode *greedystop = (node->next != -1) ? &exp->_nodes[node->next] : NULL;
		SQRexNode *greedystop = NULL;
		SQInteger p0 = (node->right >> 16)&0x0000FFFF, p1 = node->right&0x0000FFFF, nmatches = 0;
		const SQChar *s=str, *good = str;

		if(node->next != -1) {
			greedystop = &exp->_nodes[node->next];
		}
		else {
			greedystop = next;
		}

		while((nmatches == 0xFFFF || nmatches < p1)) {

			const SQChar *stop, *last_match = s;
			SQInteger while_nmatches = 0;
			while((last_match = sqstd_rex_matchnode(exp,&exp->_nodes[node->left],s,greedystop)))
			{
			    while_nmatches++;
			    s = last_match;
			    if(while_nmatches == p1) break;
			}
			if(!while_nmatches) break;
			nmatches += while_nmatches;
			good=s;
			if(greedystop && !sqstd_rex_matchclass(exp,&exp->_nodes[node->left],*s)) {
				//checks that 0 matches satisfy the expression(if so skips)
				//if not would always stop(for instance if is a _SC('?'))
				if(greedystop->type != OP_GREEDY ||
				(greedystop->type == OP_GREEDY && ((greedystop->right >> 16)&0x0000FFFF) != 0))
				{
					SQRexNode *gnext = NULL;
					if(greedystop->next != -1) {
						gnext = &exp->_nodes[greedystop->next];
					}else if(next && next->next != -1){
						gnext = &exp->_nodes[next->next];
					}
					stop = sqstd_rex_matchnode(exp,greedystop,s,gnext);
					if(stop) {
						//if satisfied stop it
						if(p0 == p1 && p0 == nmatches) break;
						else if(nmatches >= p0 && p1 == 0xFFFF) break;
						else if(nmatches >= p0 && nmatches <= p1) break;
					}
				}
			}

			if(s >= exp->_eol)
				break;
		}
		if(p0 == p1 && p0 == nmatches) return good;
		else if(nmatches >= p0 && p1 == 0xFFFF) return good;
		else if(nmatches >= p0 && nmatches <= p1) return good;
		return NULL;
	}
	case OP_OR: {
			const SQChar *asd = str;
			SQRexNode *temp=&exp->_nodes[node->left];
			while( (asd = sqstd_rex_matchnode(exp,temp,asd,NULL)) ) {
				if(temp->next != -1)
					temp = &exp->_nodes[temp->next];
				else
					return asd;
			}
			asd = str;
			temp = &exp->_nodes[node->right];
			while( (asd = sqstd_rex_matchnode(exp,temp,asd,NULL)) ) {
				if(temp->next != -1)
					temp = &exp->_nodes[temp->next];
				else
					return asd;
			}
			return NULL;
	}
	case OP_EMPTY: //zero length capture
	case OP_EXPR:
	case OP_NOCAPEXPR:{
			SQRexNode *n = &exp->_nodes[node->left];
			const SQChar *cur = str;
			SQInteger capture = -1;
			if(node->type != OP_NOCAPEXPR && node->right == exp->_currsubexp) {
				capture = exp->_currsubexp;
				exp->_matches[capture].begin = cur;
				exp->_currsubexp++;
				if(type == OP_EMPTY)
                {
                    exp->_matches[capture].len = -1;
                    return cur;
                }
			}
			SQInteger tempcap = exp->_currsubexp;
			do {
				SQRexNode *subnext = NULL;
				if(n->next != -1) {
					subnext = &exp->_nodes[n->next];
				}else {
					subnext = next;
				}
				if(!(cur = sqstd_rex_matchnode(exp,n,cur,subnext))) {
					if(capture != -1){
						exp->_matches[capture].begin = 0;
						exp->_matches[capture].len = 0;
					}
					return NULL;
				}
			} while((n->next != -1) && (n = &exp->_nodes[n->next]));

			exp->_currsubexp = tempcap;
			if(capture != -1)
				exp->_matches[capture].len = cur - exp->_matches[capture].begin;
			return cur;
	}
	case OP_WB:
		if((str == exp->_bol && isChClassWord(*str))
		 || (str == exp->_eol && isChClassWord(*(str-1)))
		 || (isChClassWord(*str) != isChClassWord(*(str-1))) ) {
			return (node->left == _SC('b'))?str:NULL;
		}
		return (node->left == _SC('b'))?NULL:str;
	case OP_BOL:
		if(str == exp->_bol) return str;
		return NULL;
	case OP_EOL:
		if(str == exp->_eol) return str;
		return NULL;
	case OP_DOT:{
		if (str == exp->_eol) return NULL;
		str++;
				}
		return str;
	case OP_NCLASS:
	case OP_CLASS:
		if (str == exp->_eol) return NULL;
		if(sqstd_rex_matchclass(exp,&exp->_nodes[node->left],*str)?(type == OP_CLASS?SQTrue:SQFalse):(type == OP_NCLASS?SQTrue:SQFalse)) {
			str++;
			return str;
		}
		return NULL;
	case OP_CCLASS:
		if (str == exp->_eol) return NULL;
		if(sqstd_rex_matchcclass(node->left,*str)) {
			str++;
			return str;
		}
		return NULL;
    case OP_MB:
        {
            SQInteger cb = node->left; //char that opens a balanced expression
            if(*str != cb) return NULL; // string doesnt start with open char
            SQInteger ce = node->right; //char that closes a balanced expression
            SQInteger cont = 1;
            const SQChar *streol = exp->_eol;
            while (++str < streol) {
              if (*str == ce) {
                if (--cont == 0) {
                    return ++str;
                }
              }
              else if (*str == cb) cont++;
            }
        }
        return NULL; // string ends out of balance
	default: /* char */
		if (str == exp->_eol) return NULL;
		if(*str != node->type) return NULL;
		str++;
		return str;
	}
	return NULL;
}

/* public api */
SQRex *sqstd_rex_compile(const SQChar *pattern,const SQChar **error)
{
    SQInteger plen = (SQInteger)scstrlen(pattern) * sizeof(SQChar);
    if(plen == 0)
    {
        *error = _SC("empty pattern");
        return NULL;
    }
	SQRex * volatile exp = (SQRex *)sq_malloc(sizeof(SQRex)); // "volatile" is needed for setjmp()
	exp->_bol = exp->_p = pattern;
	exp->_eol = exp->_bol + plen;
	exp->_nallocated = plen;
	exp->_nodes = (SQRexNode *)sq_malloc(exp->_nallocated * sizeof(SQRexNode));
	exp->_nsize = 0;
	exp->_matches = 0;
	exp->_nsubexpr = 0;
	exp->_first = sqstd_rex_newnode(exp,OP_EXPR);
	exp->_error = error;
	exp->_jmpbuf = sq_malloc(sizeof(jmp_buf));
	if(setjmp(*((jmp_buf*)exp->_jmpbuf)) == 0) {
		SQInteger res = sqstd_rex_list(exp);
		exp->_nodes[exp->_first].left = res;
		if(*exp->_p!=_SC('\0'))
			sqstd_rex_error(exp,_SC("unexpected character"));
#ifdef _DEBUG
		{
			SQInteger nsize,i;
			SQRexNode *t;
			nsize = exp->_nsize;
			t = &exp->_nodes[0];
			scprintf(_SC("\n"));
			for(i = 0;i < nsize; i++) {
				if(exp->_nodes[i].type>MAX_CHAR)
					scprintf(_SC("[%02d] %10s "),(SQInt32)i,g_nnames[exp->_nodes[i].type-MAX_CHAR]);
				else
					scprintf(_SC("[%02d] %10c "),(SQInt32)i,exp->_nodes[i].type);
				scprintf(_SC("left %02d right %02d next %02d\n"), (SQInt32)exp->_nodes[i].left, (SQInt32)exp->_nodes[i].right, (SQInt32)exp->_nodes[i].next);
			}
			scprintf(_SC("\n"));
		}
#endif
		exp->_matches = (SQRexMatch *) sq_malloc(exp->_nsubexpr * sizeof(SQRexMatch));
		memset(exp->_matches,0,exp->_nsubexpr * sizeof(SQRexMatch));
	}
	else{
		sqstd_rex_free(exp);
		return NULL;
	}
	return exp;
}

void sqstd_rex_free(SQRex *exp)
{
	if(exp)	{
		if(exp->_nodes) sq_free(exp->_nodes,exp->_nallocated * sizeof(SQRexNode));
		if(exp->_jmpbuf) sq_free(exp->_jmpbuf,sizeof(jmp_buf));
		if(exp->_matches) sq_free(exp->_matches,exp->_nsubexpr * sizeof(SQRexMatch));
		sq_free(exp,sizeof(SQRex));
	}
}

SQBool sqstd_rex_match(SQRex* exp,const SQChar* text, SQInteger text_size)
{
	const SQChar* res = NULL;
	exp->_bol = text;
	exp->_eol = text + ((text_size < 0) ? scstrlen(text) : text_size);
	exp->_currsubexp = 0;
	res = sqstd_rex_matchnode(exp,exp->_nodes,text,NULL);
	if(res == NULL || res != exp->_eol)
		return SQFalse;
	return SQTrue;
}

SQBool sqstd_rex_searchrange(SQRex* exp,const SQChar* text_begin,const SQChar* text_end,const SQChar** out_begin, const SQChar** out_end)
{
	const SQChar *cur = NULL;
	SQInteger node = exp->_first;
	if(text_begin >= text_end) return SQFalse;
	exp->_bol = text_begin;
	exp->_eol = text_end;
	do {
		cur = text_begin;
		while(node != -1) {
			exp->_currsubexp = 0;
			cur = sqstd_rex_matchnode(exp,&exp->_nodes[node],cur,NULL);
			if(!cur)
				break;
			node = exp->_nodes[node].next;
		}
		text_begin++;
	} while(cur == NULL && text_begin != text_end);

	if(cur == NULL)
		return SQFalse;

	--text_begin;

	if(out_begin) *out_begin = text_begin;
	if(out_end) *out_end = cur;
	return SQTrue;
}

SQBool sqstd_rex_search(SQRex* exp,const SQChar* text, const SQChar** out_begin, const SQChar** out_end)
{
	return sqstd_rex_searchrange(exp,text,text + scstrlen(text),out_begin,out_end);
}

SQInteger sqstd_rex_getsubexpcount(SQRex* exp)
{
	return exp->_nsubexpr;
}

SQBool sqstd_rex_getsubexp(SQRex* exp, SQInteger n, SQRexMatch *subexp)
{
	if( n<0 || n >= exp->_nsubexpr) return SQFalse;
	*subexp = exp->_matches[n];
	return SQTrue;
}

