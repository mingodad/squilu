#include "minctest.nut"
local sqt = minctest();

local Is32Bits = _intsize_ == 4;
//print("Is32Bits", Is32Bits);
sqt.run("closures", function(){

	local A = 0, B = {g=10}
	local f = function(x)
	{
	  local a = []
	  for(local i=1; i < 1000; ++i)
	  {
	    local y = 0
	    {
	      a.append(function () {++B.g; y += x; return y+A;});
	    }
	  }
	  local dummy = function () {return a[A];}
	  collectgarbage()
	  A = 1;
	  sqt.ok(dummy() == a[1]);
	  A = 0;
	  //print("a[0]", a[0]())
	  sqt.ok(a[0]() == x)
	  sqt.ok(a[2]() == x)
	  collectgarbage()
	  sqt.ok(B.g == 12)
	  return a
	}

	local a = f(10)

	// testing closures with 'for' control variable
	a = []
	for(local i=0; i < 10; ++i)
	{
	  local lv = i;
	  a.append({set = function(x){ lv=x; }, get = function (){ return lv; }})
	  if( i == 2 ) break;
	}
	sqt.ok(a.len() == 3)
	sqt.ok(a[0].get() == 0)
	a[0].set(10)
	sqt.ok(a[0].get() == 10)
	sqt.ok(a[1].get() == 1)
	a[1].set('a')
	sqt.ok(a[1].get() == 'a')
	//a[2].set(2)
	sqt.ok(a[2].get() == 2)

	a = []
	foreach( i, k in ['a', 'b'])
	{
	  local li = i
	  local lk = k
	  a.append( {set = function(x, y) {li=x; lk=y},
		  get = function () {return [li, lk];}} )
	  if( i == 2 ) break;
	}
	a[0].set(10, 20)
	local rs = a[1].get()
	sqt.ok(rs[0] == 1 && rs[1] == 'b')
	rs = a[0].get()
	sqt.ok(rs[0] == 10 && rs[1] == 20)
	a[1].set('a', 'b')
	rs = a[1].get()
	sqt.ok(rs[0] == 'a' && rs[1] == 'b')

	// testing closures with 'for' control variable x break
	for(local i=1; i <= 3; ++i)
	{
	  local li = i
	  f = function () { return li;}
	  break
	}
	sqt.ok(f() == 1)

	foreach( k, v in ["a", "b"])
	{
	  local lk = k, lv = v
	  f = function () {return [lk, lv]; }
	  break
	}
	sqt.ok(([f()])[0][0] == 0)
	sqt.ok(([f()])[0][1] == "a")


	// testing closure x break x return x errors

	local b
	f = function (x)
	{
	  local first = 1
	  while( 1 ) {
	    if( x == 3  && ! first ) return
	    local la = "xuxu"
	    b = function (op, y=0) {
		  if( op == "set" )
		    la = x+y
		  else
		    return la
		}
	    if( x == 1 ) { break }
	    else if( x == 2 ) return
	    else if( x != 3 )  throw("x != 3")

	    first = null
	  }
	}

	for(local i=1; i <= 3; ++i) {
	  f(i)
	  sqt.ok(b("get") == "xuxu")
	  b("set", 10); sqt.ok(b("get") == 10+i)
	  b = null
	}

	//pcall(f, 4);
	try{ f(4);}catch(e){}
	sqt.ok(b("get") == "xuxu")
	b("set", 10); sqt.ok(b("get") == 14)

	// testing multi-level closure
	local w
	f = function(x)
	{
	  return function (y)
		{
			return function (z) {return w+x+y+z;}
		}
	}

	local y = f(10)
	w = 1.345
	sqt.ok(y(20)(30) == 60+w)

	// test for correctly closing upvalues in tail calls of vararg functions
	local function t ()
	{
	  local function c(a,b) {sqt.ok(a=="test" && b=="OK") }
	  local function v(f, ...) {c("test", f() != 1 && "FAILED" || "OK") }
	  local lx = 1
	  return v(function() {return lx;})
	}
	t()

});

sqt.run("calls", function(){

	// get the opportunity to test "type" too ;)

	sqt.ok(type(1<2) == "bool")
	sqt.ok(type(true) == "bool" && type(false) == "bool")
	sqt.ok(type(null) == "null")
	sqt.ok(type(-3) == "integer")
	sqt.ok(type(-3.14) == "float")
	sqt.ok(type("x") == "string")
	sqt.ok(type({}) == "table")
	sqt.ok(type(type) == "function")

	sqt.ok(type(sqt.ok) == type(print))
	local f = null
	f = function (x) {return a.x(x);}
	sqt.ok(type(f) == "function")

	// testing local-function recursion
	local fact = false
	{
	  local res = 1
	  local function lfact (n)
	  {
	    if( n==0 ) return res
	    else return n*lfact(n-1)
	  }
	  sqt.ok(lfact(5) == 120)
	}
	sqt.ok(fact == false)

	// testing declarations
	local a = {i = 10}
	local self = 20
	a.x <- function(x) {return x+this.i;}
	a.y <- function(x) {return x+self;}

	sqt.ok(a.x(1)+10 == a.y(1))

	a.t <- {i=-100}
	a["t"].x <- function (a,b) {return this.i+a+b;}

	sqt.ok(a.t.x(2,3) == -95)

	{
	  local la = {x=0}
	  la.add <- function(x) {this.x = this.x+x; la.y <- 20; return this; }
	  sqt.ok(la.add(10).add(20).add(30).x == 60 && la.y == 20)
	}

	a = {b={c={}}}

	a.b.c.f1 <- function(x) {return x+1;}
	a.b.c.f2 <- function(x,y) {this[x] <- y;}
	sqt.ok(a.b.c.f1(4) == 5)
	a.b.c.f2("k", 12); sqt.ok(a.b.c.k == 12)

	local t = null   // 'declare' t
	f = function(a,b,c=null, e=null) {local d = 'a'; t=[a,b,c,d];}

	f(      // this line change must be valid
	  1,2)
	sqt.ok(t[0] == 1 && t[1] == 2 && t[2] == null && t[3] == 'a')

	f(1,2,   // this one too
	      3,4)
	sqt.ok(t[0] == 1 && t[1] == 2 && t[2] == 3 && t[3] == 'a')

	function fat(x)
	{
	  if( x <= 1 ) return 1
	  else return x*loadstring("return fat(" +( x-1) + ")")()
	}

	sqt.ok(loadstring( "loadstring( \"sqt.ok(fat(6)==720)\" )()" ))
	a = loadstring("return [fat(5), 3]")
	a = a()
	sqt.ok(a[0] == 120 && a[1] == 3)

	// fixed-point operator
	local Y = function (le)
	{
	      local function la (f)
	      {
		return le(function (x) {return f(f)(x);})
	      }
	      return la(la)
	}


	// non-recursive factorial

	local F = function (f)
	{
	      return function (n)
		{
		       if( n == 0 ) return 1
		       else return n*f(n-1)
		}
	}

	fat = Y(F)

	sqt.ok(fat(0) == 1 && fat(4) == 24 && Y(F)(5)==5*Y(F)(4))

	local function g (z)
	{
	  local function lf (a,b,c,d)
	  {
	    return function (x,y) {return a+b+c+d+a+x+y+z;}
	  }
	  return lf(z,z+1,z+2,z+3)
	}

	f = g(10)
	sqt.ok(f(9, 16) == 10+11+12+13+10+9+16+10)

	Y, F, f = null

	try { assert(true); sqt.ok(true);} catch(e) {sqt.ok(false);};
	try { assert(true, "opt msg"); sqt.ok(true);} catch(e) {sqt.ok(false);};
	try { assert(false); sqt.ok(false);} catch(e) {sqt.ok(true);};
	try { assert(false, "opt msg"); sqt.ok(false);} catch(e) {sqt.ok(e == "opt msg");};

});

sqt.run("sort", function(){
	local function check (a, f=null)
	{
	  f = f || function (x,y) {return x<y;};
	  for(local n=a.len()-1; n > 2; --n) sqt.ok(! f(a[n], a[n-1]))
	}

	local a = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
	     "Oct", "Nov", "Dec"]

	a.sort()
	check(a)

	limit <- 30000
	if( table_rawget(getroottable(), "_soft", false) ) limit = 5000

	a = []
	for(local i=0; i < limit; ++i) a.append(math.random())

	local x = os.clock()
	a.sort()
	//print(format("Sorting %d elements in %.2f sec.", limit, os.clock()-x))
	check(a)

	x = os.clock()
	a.sort()
	//print(format("Re-sorting %d elements in %.2f sec.", limit, os.clock()-x))
	check(a)

	a = []
	for(local i=0; i < limit; ++i) a.append(math.random())

	x = os.clock();
	local li=0
	a.sort(function(x,y) {li=li+1; return y<=>x;})
	//print(format("Invert-sorting other %d elements in %.2f sec., with %i comparisons",
	//      limit, os.clock()-x, li))
	check(a, function(x,y) {return y<x;})

	a = []
	a.sort()  // empty array

	for(local i=0; i < limit; ++i) a.append(false)
	x = os.clock();
	a.sort(function(x,y) {return 0;})
	//print(format("Sorting %d equal elements in %.2f sec.", limit, os.clock()-x))
	check(a, function(x,y) {return 0;})
	foreach( i,v in a) sqt.ok(! v || i=='n' && v==limit)

	a = ["�lo", "\0first :-)", "alo", "then this one", "45", "and a new"]
	a.sort()
	check(a)
});

sqt.run("pattern matching", function(){

	local re = regexp(@"^[^\n]+");
	sqt.ok(re.match("abc\ndef") == "abc");	
	sqt.ok("abc\ndef".match("^[^\n]+") == "abc");
	
	re = regexp(@"abc..def");
	sqt.ok(re.match("abc\u00b5def") == "abc\u00b5def");	
	sqt.ok("abc\u00b5def".match("abc..def") == "abc\u00b5def"); 

	re = regexp("ab\u00b5\u2704."); //need not use @ to expand \uXXXX
	sqt.ok(re.match("ab\u00b5\u2704x") == "ab\u00b5\u2704x");	
	sqt.ok("ab\u00b5\u2704x".match("ab\u00b5\u2704.") == "ab\u00b5\u2704x"); 

	sqt.ok("aBcaBc" == "abcabc".gsub("b", "B"));
	sqt.ok("aBcaBc" == "abcabc".gsub("b", function(w){return w.toupper(); }));
	sqt.ok("$a$a$" ==  "#a#a#".gsub("#", "$"));
	sqt.ok("$a$a$" ==  "#a#a#".gsub("#", function(w){return "$"; }));
	sqt.ok("$$a$$" ==  "##a##".gsub("##", "$$"));
	sqt.ok("$$a$$" ==  "##a##".gsub("##", function(w){ return "$$"; }));
	sqt.ok("A" ==  "a".gsub("a", "A"));
	sqt.ok("A" == "a".gsub("a", function(w){return  w.toupper(); }));
	//sqt.ok("<a><><>" == "a".gsub("a", "<%0><%1><%2>"));
	sqt.ok(".h.e.l.l.o." == "hello".gsub("", "."));

	local a, b,c, d, e, f, r, s, t, x

	f = function (s, p)
	{
	  local res = []
	  if( s.find_lua(p, res) >= 0) return s.slice(res[0], res[1])
	}

	function f1(s, p)
	{
	  p = p.gsub("%%([0-9])", function (s) {return "%" + (s.tointeger()+1);})
	  p = p.gsub("^(^?)", "%1()", 1)
	  p = p.gsub("($?)$", "()%1", 1)
	  local tl = []
	  s.gmatch(p, function(...){foreach(elm in vargv) tl.append(elm); return true;})
	  return s.slice(tl[0], tl[tl.len()-1] )
	}

	a = []
	b = "".find_lua("", a)    // empty patterns are tricky
	sqt.ok(a[0] == 0 && a[1] == 0);
	b = "alo".find_lua("", a)
	sqt.ok(a[0] == 0 && a[1] == 0)
	b = "a\0o a\0o a\0o".find_lua("a", a, 0)   // first position
	sqt.ok(a[0] == 0 && a[1] == 1)
	b = "a\0o a\0o a\0o".find_lua("a\0o", a, 1)   // starts in the midle
	sqt.ok(a[0] == 4 && a[1] == 7)
	b = "a\0o a\0o a\0o".find_lua("a\0o", a, 8)   // starts in the midle
	sqt.ok(a[0] == 8 && a[1] == 11)
	b = "a\0a\0a\0a\0\0ab".find_lua("\0ab", a, 1);  // finds at the end
	sqt.ok(a[0] == 8 && a[1] == 11);
	b = "a\0a\0a\0a\0\0ab".find_lua("b", a)    // last position
	//print(a[0], a[1])
	sqt.ok(a[0] == 10 && a[1] == 11)
	sqt.ok("a\0a\0a\0a\0\0ab".find_lua("b\0") < 0)   // check ending
	sqt.ok("".find_lua("\0") < 0)
	sqt.ok("alo123alo".find_lua("12") == 3)
	sqt.ok("alo123alo".find_lua("^12") < 0)

	sqt.ok(f("aloALO", "%l*") == "alo")
	sqt.ok(f("aLo_ALO", "%a*") == "aLo")

	sqt.ok(f("aaab", "a*") == "aaa");
	sqt.ok(f("aaa", "^.*$") == "aaa");
	sqt.ok(f("aaa", "b*") == "");
	sqt.ok(f("aaa", "ab*a") == "aa")
	sqt.ok(f("aba", "ab*a") == "aba")
	sqt.ok(f("aaab", "a+") == "aaa")
	sqt.ok(f("aaa", "^.+$") == "aaa")
	sqt.ok(f("aaa", "b+") == null)
	sqt.ok(f("aaa", "ab+a") == null)
	sqt.ok(f("aba", "ab+a") == "aba")
	sqt.ok(f("a$a", ".$") == "a")
	sqt.ok(f("a$a", ".%$") == "a$")
	sqt.ok(f("a$a", ".$.") == "a$a")
	sqt.ok(f("a$a", "$$") == null)
	sqt.ok(f("a$b", "a$") == null)
	sqt.ok(f("a$a", "$") == "")
	sqt.ok(f("", "b*") == "")
	sqt.ok(f("aaa", "bb*") == null)
	sqt.ok(f("aaab", "a-") == "")
	sqt.ok(f("aaa", "^.-$") == "aaa")
	sqt.ok(f("aabaaabaaabaaaba", "b.*b") == "baaabaaabaaab")
	sqt.ok(f("aabaaabaaabaaaba", "b.-b") == "baaab")
	sqt.ok(f("alo xo", ".o$") == "xo")
	sqt.ok(f(" \n isto � assim", "%S%S*") == "isto")
	sqt.ok(f(" \n isto � assim", "%S*$") == "assim")
	sqt.ok(f(" \n isto � assim", "[a-z]*$") == "assim")
	sqt.ok(f("um caracter ? extra", "[^%sa-z]") == "?")
	sqt.ok(f("", "a?") == "")
	sqt.ok(f("�", "�?") == "�")
	sqt.ok(f("�bl", "�?b?l?") == "�bl")
	sqt.ok(f("  �bl", "�?b?l?") == "")
	sqt.ok(f("aa", "^aa?a?a") == "aa")
	sqt.ok(f("]]]�b", "[^]]") == "�")
	sqt.ok(f("0alo alo", "%x*") == "0a")
	sqt.ok(f("alo alo", "%C+") == "alo alo")
	//print("+")

	sqt.ok(f1("alo alx 123 b\0o b\0o", "(..*) %1") == "b\0o b\0o")
	sqt.ok(f1("axz123= 4= 4 34", "(.+)=(.*)=%2 %1") == "3= 4= 4 3")
	sqt.ok(f1("=======", "^(=*)=%1$") == "=======")
	sqt.ok("==========".match("^([=]*)=%1$") == null)

	local function range (i, j)
	{
	  local res_len = j-i
	  local res = array(res_len+1)
	  local count = 0;
	  for(local idx = i; idx <= j; ++idx) res[count++] = idx;
	  return res
	}

	local ar = range(0, 255)
	//print(ar.len())

	ar.insert(0, this)
	local abc = str_from_chars.acall(ar);

	sqt.ok(abc.len() == 256)

	function strset (p)
	{
	  local res = {s=""}
	  abc.gsub(p, function (c) {res.s = res.s + c;})
	  return res.s
	}

	sqt.ok(strset("[\xC8-\xD2]").len() == 11)

	sqt.ok(strset("[a-z]") == "abcdefghijklmnopqrstuvwxyz")
	sqt.ok(strset("[a-z%d]") == strset("[%da-uu-z]"))
	sqt.ok(strset("[a-]") == "-a")
	sqt.ok(strset("[^%W]") == strset("[%w]"))
	sqt.ok(strset("[]%%]") == "%]")
	sqt.ok(strset("[a%-z]") == "-az")
	sqt.ok(strset("[%^%[%-a%]%-b]") == "-[]^ab")
	sqt.ok(strset("%Z") == strset("[\x01-\xff]"))
	sqt.ok(strset(".") == strset("[\x01-\xff%z]"))
	//print("+");

	sqt.ok("alo xyzK".match("(%w+)K") == "xyz")
	sqt.ok("254 K".match("(%d*)K") == "")
	sqt.ok("alo ".match("(%w*)$") == "")
	sqt.ok("alo ".match("(%w+)$") == null)
	sqt.ok("(�lo)".find_lua("%(�") == 0)
	a = b = c = d = e =null
	"�lo alo".gmatch("^(((.).).* (%w*))$", function(ma, mb, mc, md){ a=ma; b=mb; c=mc; d=md;})
	sqt.ok(a == "�lo alo" && b == "�l" && c == "�" && d == "alo" && e == null)
	a = b = c = d = e =null
	"0123456789".gmatch("(.+(.?)())", function(ma, mb, mc){ a=ma; b=mb; c=mc;})
	sqt.ok(a == "0123456789" && b == "" && c == 10 && d == null)
	//print("+")

	sqt.ok("�lo �lo".gsub("�", "x") == "xlo xlo")
	sqt.ok("alo �lo  ".gsub(" +$", "") == "alo �lo")  // trim
	sqt.ok("  alo alo  ".gsub("^%s*(.-)%s*$", "%1") == "alo alo")  // double trim
	sqt.ok("alo  alo  \n 123\n ".gsub("%s+", " ") == "alo alo 123 ")

	t = "ab� d"
	a = t.gsub("(.)", "%1@")
	sqt.ok("@"+ a == t.gsub("", "@"))
	a = "ab�d".gsub("(.)", "%0@", 2)
	sqt.ok(a == "a@b@�d")
	sqt.ok("alo alo".gsub("()[al]", "%1") == "12o 56o")
	sqt.ok("abc=xyz".gsub("(%w*)(%p)(%w+)", "%3%2%1-%0") ==
		      "xyz=abc-abc=xyz")
	sqt.ok("abc".gsub("%w", "%1%0") == "aabbcc")
	sqt.ok("abc".gsub("%w+", "%0%1") == "abcabc")
	sqt.ok("���".gsub("$", "\0��") == "���\0��")
	sqt.ok("".gsub("^", "r") == "r")
	sqt.ok("".gsub("$", "r") == "r")
	//print("+")

	sqt.ok("um (dois) tres (quatro)".gsub("(%(%w+%))", function(m) {return m.toupper();}) ==
		    "um (DOIS) tres (QUATRO)")

	{
	  local function setglobal (n,v) {table_rawset(getroottable(), n, v) }
	  "ga=roberto,groberto=ga".gsub("(%w+)=(%w%w*)", setglobal)
	  sqt.ok(ga=="roberto" && groberto=="ga")
	}

	f = function (a,b) {return a.gsub(".",b);}
	sqt.ok("trocar tudo em |teste|b| � |beleza|al|".gsub("|([^|]*)|([^|]*)|", f) ==
		    "trocar tudo em bbbbb � alalalalalal")

	local function dostring (s) {return (loadstring(s)() || "").tostring();}
	sqt.ok("alo $a<-1$ novamente $return a$".gsub("$([^$]*)%$", dostring) ==
		    "alo  novamente 1")

	x = "$x<-\"alo\".gsub(\".\", function(m){return m.toupper()})$ assim vai para $return x$".gsub(
		 "$([^$]*)%$", dostring)
	sqt.ok(x == " assim vai para ALO")

	t = []
	s = "a alo jose  joao"
	r = s.gsub("()(%w+)()", function (a,w,b)
		{
			sqt.ok(w.len() == b-a);
			//print(b-a)
			t.append(b-a);
			return w;
		})
	sqt.ok(s == r && t[0] == 1 && t[1] == 3 && t[2] == 4 && t[3] == 4)


	function isbalanced (s)
	{
	  return s.gsub("%b()", "").find_lua("[()]") < 0
	}

	sqt.ok(isbalanced("(9 ((8))(\0) 7) \0\0 a b ()(c)() a"))
	sqt.ok(! isbalanced("(9 ((8) 7) a b (\0 c) a"))
	sqt.ok("alo 'oi' alo".gsub("%b''", "\"") == "alo \" alo")


	t = ["apple", "orange", "lime"]
	local tn = 0;
	sqt.ok("x and x and x".gsub("x", function (m) {return t[tn++];})
		== "apple and orange and lime")

	t = []
	"first second word".gsub("%w%w*", function (w) {t.append(w); return w;})
	sqt.ok(t[0] == "first" && t[1] == "second" && t[2] == "word" && t.len() == 3)

	t = []
	sqt.ok("first second word".gsub("%w+",
		 function (w) {t.append(w); return w;}, 2) == "first second word")
	sqt.ok(t[0] == "first" && t[1] == "second" && t.len() == 2)

	function mygsub(str, p, fnc)
	{
		try {
			return str.gsub(p, fnc); 
		} catch(e) {
			//print(e);
			return false;
		}
	}

	//print("testing invalid patterns")
	sqt.ok(! mygsub("alo", "(.", print))
	sqt.ok(! mygsub("alo", ".)", print))
	sqt.ok(! mygsub("alo", "(.", {}))
	sqt.ok(! mygsub("alo", "(.)", "%2"))
	sqt.ok(! mygsub("alo", "(%1)", "a"))
	sqt.ok(! mygsub("alo", "(%0)", "a"))

	// big strings
	a = "a".rep(300000)
	sqt.ok(a.find_lua("^a*.?$") >= 0)
	sqt.ok(a.find_lua("^a*.?b$") < 0)
	sqt.ok(a.find_lua("^a-.?$") >= 0)

	// deep nest of gsubs
	local rev;
	rev = function(s)
	{
	  return s.gsub("(.)(.+)", function (c,s1) {return rev(s1) + c;})
	}

	x = "012345".rep(8) // using 9 and above thow max_natcalls
	sqt.ok(rev(rev(x)) == x)


	// gsub with tables
	sqt.ok("alo alo".gsub(".", {}) == "alo alo")
	sqt.ok("alo alo".gsub("(.)", {a="AA", l=""}) == "AAo AAo")
	sqt.ok("alo alo".gsub("(.).", {a="AA", l="K"}) == "AAo AAo")
	//sqt.ok("alo alo".gsub("((.)(.?))", {al="AA", o=false}) == "AAo AAo")

	sqt.ok("alo alo".gsub("().", [2,5,6]) == "256 alo")

	t = {}; table_setdelegate(t, {_get = function (s) {return s.toupper();}})
	sqt.ok("a alo b hi".gsub("%w%w+", t) == "a ALO b HI")

	// tests for gmatch
	//sqt.ok(string.gfind == string.gmatch)
	a = 0
	"abcde".gmatch("()", function(sp) {sqt.ok(sp == a); a=sp+1; return true;})
	sqt.ok(a==5)

	t = []
	"first second word".gmatch("(%w+)", function(w) {t.append(w); return true;})
	sqt.ok(t[0] == "first" && t[1] == "second" && t[2] == "word")

	t = [2, 5, 8]
	"xuxx uu ppar r".gmatch ("()(.)%2", function(sp, ac) {
	  sqt.ok(sp == t.remove(0))
	  return true
	})
	sqt.ok(t.len() == 0)

	t = {}
	"13 14 10 = 11, 15= 16, 22=23".gmatch("(%d+)%s*=%s*(%d+)",
		function(i,j){ t[i] <- j; return true;})

	a = 0
	foreach( k,v in t) {sqt.ok(k.tointeger()+1 == v.tointeger()); a=a+1;}
	sqt.ok(a == 3)


	// tests for `%f' (`frontiers')

	sqt.ok("aaa aa a aaa a".gsub("%f[%w]a", "x") == "xaa xa x xaa x")
	sqt.ok("[[]] [][] [[[[".gsub("%f[[].", "x") == "x[]] x]x] x[[[")
	sqt.ok("01abc45de3".gsub("%f[%d]", ".") == ".01abc.45de.3")
	sqt.ok("01abc45 de3x".gsub("%f[%D]%w", ".") == "01.bc45 de3.")
	sqt.ok("function".gsub("%f[\x01-\xff]%w", ".") == ".unction")
	sqt.ok("function".gsub("%f[^\x01-\xff]", ".") == "function.")

	local ma = []
	" alo aalo allo".find_lua("%f[%S].-%f[%s].-%f[%S]", ma)
	//print(ma.len(), ma[0], ma[1])
	sqt.ok(ma[0] == 1 && ma[1] == 5)
	local k = " alo aalo allo".match("%f[%S](.-%f[%s].-%f[%S])")
	sqt.ok(k == "alo ")

	a = [0, 4, 8, 13, 16]
	"alo alo th02 is 1hat".gmatch("()%f[%w%d]",
		function(sp) {
			local asp = a.remove(0);
			//print("==", asp, sp);
			sqt.ok(asp == sp);
			return true;
		})

	sqt.ok(a.len() == 0)

});

sqt.run("regexp", function(){

	/*
	Adapted from
		http://www.theiling.de/cnc/date/2017-12-19.html
		http://www.theiling.de/cnc/regexp/test.c
	*/

	const RE_E_OK = 1;
	const RE_E_FAIL = 2;
	const RE_E_SYNTAX = 3;
	const RE_E_NOTIMPL = 3;

	local function re_match(re_str, test_str)
	{
		local result;
		try {
			local re = regexp(re_str);
			result = (re.match(test_str) != null) ? RE_E_OK : RE_E_FAIL;
		} catch(e) {result = RE_E_SYNTAX;}
		return result;
	}
/*
	print(re_match("aaa", "aaa"));
	print(re_match("aaa", "aab"));
	print(re_match("(a|[(])+", "a(aa((a"));
	print("bac".match("[\\^a]*"));
*/
	sqt.ok(re_match("aaa", "aaa") == RE_E_OK);
	sqt.ok(re_match("aaa", "aab") == RE_E_FAIL);
	sqt.ok(re_match("aaa", "aa") == RE_E_FAIL);
	sqt.ok(re_match("aa", "aaa") == RE_E_OK);
	sqt.ok(re_match("a..", "aaa") == RE_E_OK);
	sqt.ok(re_match("aaa", "a..") == RE_E_FAIL);
	sqt.ok(re_match("a{}", "") == RE_E_SYNTAX);
	sqt.ok(re_match("(aaa)", "aaa") == RE_E_OK);
	sqt.ok(re_match("(aa)a", "aaa") == RE_E_OK);
	sqt.ok(re_match("(aaa", "aaa") == RE_E_SYNTAX);
	sqt.ok(re_match("a+", "") == RE_E_FAIL);
	sqt.ok(re_match("a+", "aaa") == RE_E_OK);
	sqt.ok(re_match(".+", "aaa") == RE_E_OK);
	sqt.ok(re_match("(a)+", "aaa") == RE_E_OK);
	sqt.ok(re_match("(aa)+", "aaaa") == RE_E_OK);
	//sqt.ok(re_match("(aa)+", "aaa") == RE_E_FAIL);
	sqt.ok(re_match("(aa(b)+)+", "aabaabb") == RE_E_OK);
	//sqt.ok(re_match("(aa(b)+)+", "aaabaabb") == RE_E_FAIL);
	//sqt.ok(re_match("(aa(b)+)+", "aaaabb") == RE_E_FAIL);
	sqt.ok(re_match("a+b", "aaaaaaaaaaaaaaaab") == RE_E_OK);
	sqt.ok(re_match("a+b", "aaaaaaaaaaaaaaaa") == RE_E_FAIL);
	sqt.ok(re_match("a?", "a") == RE_E_OK);
	sqt.ok(re_match("a?", "aa") == RE_E_OK);
	sqt.ok(re_match("a?b", "b") == RE_E_OK);
	//sqt.ok(re_match("a?", "") == RE_E_OK);
	//sqt.ok(re_match("(a?)?", "") == RE_E_OK);
	//sqt.ok(re_match("a*", "") == RE_E_OK);
	//sqt.ok(re_match("(a?)+", "") == RE_E_OK);
	//sqt.ok(re_match("?", "") == RE_E_SYNTAX);
	sqt.ok(re_match("a*b", "b") == RE_E_OK);
	//sqt.ok(re_match("(a+)?", "") == RE_E_OK);
	sqt.ok(re_match("a+b", "b") == RE_E_FAIL);
	sqt.ok(re_match("a+b", "ab") == RE_E_OK);
	sqt.ok(re_match("a+b", "aab") == RE_E_OK);
	sqt.ok(re_match("a+b", "aaab") == RE_E_OK);
	sqt.ok(re_match("a{2}b", "ab") == RE_E_FAIL);
	sqt.ok(re_match("a{2}b", "aab") == RE_E_OK);
	sqt.ok(re_match("a{2}b", "aaab") == RE_E_OK);
	sqt.ok(re_match("a{2,}b", "aaab") == RE_E_OK);
	sqt.ok(re_match("a{2,}b", "aab") == RE_E_OK);
	sqt.ok(re_match("a{2,}b", "ab") == RE_E_FAIL);
	sqt.ok(re_match("a*b", "aaab") == RE_E_OK);
	sqt.ok(re_match("a{1}", "a") == RE_E_OK);
	sqt.ok(re_match("a{2}", "a") == RE_E_FAIL);
	sqt.ok(re_match("a{2}", "aa") == RE_E_OK);
	sqt.ok(re_match("a{2}", "aaa") == RE_E_OK);
	sqt.ok(re_match("a{3}", "aa") == RE_E_FAIL);
	sqt.ok(re_match("a{3}", "aaa") == RE_E_OK);
	sqt.ok(re_match("a{3}", "aaaa") == RE_E_OK);
	sqt.ok(re_match("a{2,4}", "a") == RE_E_FAIL);
	sqt.ok(re_match("a{2,4}", "aa") == RE_E_OK);
	sqt.ok(re_match("a{2,4}", "aaa") == RE_E_OK);
	sqt.ok(re_match("a{2,4}", "aaaa") == RE_E_OK);
	sqt.ok(re_match("a{2,4}", "aaaaa") == RE_E_OK);
	sqt.ok(re_match("ab{2,4}", "a") == RE_E_FAIL);
	sqt.ok(re_match("ab{2,4}", "ab") == RE_E_FAIL);
	sqt.ok(re_match("ab{2,4}", "abb") == RE_E_OK);
	sqt.ok(re_match("ab{2,4}", "abbb") == RE_E_OK);
	sqt.ok(re_match("ab{2,4}", "abbbb") == RE_E_OK);
	sqt.ok(re_match("ab{2,4}", "abbbbb") == RE_E_OK);
	sqt.ok(re_match("ab{2,4}", "aabbbb") == RE_E_OK);
	sqt.ok(re_match("a{,2}b{2,4}", "aabbbb") == RE_E_SYNTAX);
	sqt.ok(re_match("a{0,2}b{2,4}", "aabbbb") == RE_E_OK); 
	sqt.ok(re_match("a)", "") == RE_E_SYNTAX);
	//sqt.ok(re_match("(a+b+)?", "") == RE_E_OK);
	sqt.ok(re_match("(a{2}b+)*c", "aabc") == RE_E_OK);
	//sqt.ok(re_match("(a{2}b+)*c", "c") == RE_E_OK);
	sqt.ok(re_match("(a{2}b+)*", "aab") == RE_E_OK);
	//sqt.ok(re_match("(a{2}b+)*", "") == RE_E_OK);
	sqt.ok(re_match("(a{2}b+)?", "aab") == RE_E_OK);
	//sqt.ok(re_match("(a{2}b+)?", "") == RE_E_OK);
	sqt.ok(re_match("(ab){2,4}", "a") == RE_E_FAIL);
	sqt.ok(re_match("(ab){2,4}", "b") == RE_E_FAIL);
	sqt.ok(re_match("(ab){2,4}", "") == RE_E_FAIL);
	sqt.ok(re_match("(ab){2,4}", "ab") == RE_E_FAIL);
	sqt.ok(re_match("(ab){2,4}", "abab") == RE_E_OK);
	sqt.ok(re_match("(ab){2,4}", "ababab") == RE_E_OK);
	sqt.ok(re_match("(ab){2,4}", "abababab") == RE_E_OK);
	sqt.ok(re_match("(ab){2,4}", "ababababab") == RE_E_OK);
	sqt.ok(re_match("ab", "ab") == RE_E_OK);
	sqt.ok(re_match("ab", "abab") == RE_E_OK);
	sqt.ok(re_match("(ab)", "ab") == RE_E_OK);
	sqt.ok(re_match("(ab)", "abab") == RE_E_OK);
	sqt.ok(re_match("(ab){1}", "ab") == RE_E_OK);
	sqt.ok(re_match("(ab){1}", "abab") == RE_E_OK);
	sqt.ok(re_match("(ab){1,1}", "ab") == RE_E_OK);
	sqt.ok(re_match("(ab){1,1}", "abab") == RE_E_OK);
	//sqt.ok(re_match("(ab){1}{1,1}", "ab") == RE_E_OK);
	sqt.ok(re_match("(ab){1}{1,1}", "abab") == RE_E_FAIL);
	sqt.ok(re_match("((ab){2,4}c)+", "") == RE_E_FAIL);
	//sqt.ok(re_match("(((ab){2,4}c)+)?", "") == RE_E_OK);
	//sqt.ok(re_match("((ab){2,4}c)+?", "") == RE_E_OK);
	sqt.ok(re_match("((ab){2,4}c)+", "ababc") == RE_E_OK);
	sqt.ok(re_match("((ab){2,4}c)+", "ababcabababc") == RE_E_OK);
	//sqt.ok(re_match("((ab){2,4}c)+?", "ababcabababc") == RE_E_OK);
	//sqt.ok(re_match("((ab){2,4}c)+?", "") == RE_E_OK);
	sqt.ok(re_match("a{0,1}", "a") == RE_E_OK);
	sqt.ok(re_match("a{1,1}", "a") == RE_E_OK);
	sqt.ok(re_match("a{0,}", "a") == RE_E_OK);
	sqt.ok(re_match("a{,}", "a") == RE_E_SYNTAX);
	sqt.ok(re_match("a{,}", "a") == RE_E_SYNTAX);
	sqt.ok(re_match("a{,g}", "a") == RE_E_SYNTAX);
	sqt.ok(re_match("a{,1g}", "a") == RE_E_SYNTAX);
	sqt.ok(re_match("a{0,g}", "a") == RE_E_SYNTAX);
	sqt.ok(re_match("a{0,1g}", "a") == RE_E_SYNTAX);
	//sqt.ok(re_match("a{1,0}", "a") == RE_E_SYNTAX);
	//sqt.ok(re_match("a{1,0}", "a") == RE_E_SYNTAX);
	//sqt.ok(re_match("a{0}", "a") == RE_E_FAIL);
	//sqt.ok(re_match("a{0,0}", "a") == RE_E_FAIL);
	//sqt.ok(re_match("a++", "aaa") == RE_E_OK);
	//sqt.ok(re_match("a??", "") == RE_E_OK);
	//sqt.ok(re_match("a**", "") == RE_E_OK);
	//sqt.ok(re_match("a?*", "") == RE_E_OK);
	//sqt.ok(re_match("a*?", "") == RE_E_OK);
	sqt.ok(re_match("a?", "a") == RE_E_OK);
	//sqt.ok(re_match("a??", "a") == RE_E_OK);
	//sqt.ok(re_match("a**", "a") == RE_E_OK);
	//sqt.ok(re_match("a?*", "a") == RE_E_OK);
	//sqt.ok(re_match("a*?", "a") == RE_E_OK);
	sqt.ok(re_match("a?", "aa") == RE_E_OK);
	//sqt.ok(re_match("a??", "aa") == RE_E_FAIL);
	//sqt.ok(re_match("a**", "aa") == RE_E_OK);
	//sqt.ok(re_match("a?*", "aa") == RE_E_OK);
	//sqt.ok(re_match("a*?", "aa") == RE_E_OK);
	//sqt.ok(re_match("a+?", "") == RE_E_OK);
	//sqt.ok(re_match("a?+", "") == RE_E_OK);
	//sqt.ok(re_match("a+?b", "b") == RE_E_OK);
	sqt.ok(re_match("a\\rb", "a\rb") == RE_E_OK);
	sqt.ok(re_match("a+\n+b+", "aa\n\nbb") == RE_E_SYNTAX);
	sqt.ok(re_match("a+\\n+b+", "aa\n\nbb") == RE_E_OK);
	//sqt.ok(re_match("a+\\5+b+", "aa\n\nbb") == RE_E_SYNTAX);
	sqt.ok(re_match("a+\\(+b+", "aa((bb") == RE_E_OK);
	sqt.ok(re_match("[a]+", "a") == RE_E_OK);
	sqt.ok(re_match("[a]+", "aa") == RE_E_OK);
	sqt.ok(re_match("[a]+", "aaa") == RE_E_OK);
	sqt.ok(re_match("[ba]+", "a") == RE_E_OK);
	sqt.ok(re_match("[ba]+", "ab") == RE_E_OK);
	sqt.ok(re_match("[ba]+", "baaa") == RE_E_OK);
	sqt.ok(re_match("[ba]+", "c") == RE_E_FAIL);
	sqt.ok(re_match("[\\n]+", "\n") == RE_E_OK);
	sqt.ok(re_match("[\\n]+", "\n\n") == RE_E_OK);
	sqt.ok(re_match("[\\n]+", "\n\n\n") == RE_E_OK);
	sqt.ok(re_match("[a-c]+", "aaa") == RE_E_OK);
	sqt.ok(re_match("[a-c]+", "ccc") == RE_E_OK);
	sqt.ok(re_match("[a-c]+", "bbb") == RE_E_OK);
	sqt.ok(re_match("[a", "bbb") == RE_E_SYNTAX);
	sqt.ok(re_match("[a-", "bbb") == RE_E_SYNTAX);
	sqt.ok(re_match("[a-b", "bbb") == RE_E_SYNTAX);
	sqt.ok(re_match("[a-\\b", "bbb") == RE_E_NOTIMPL);
	//sqt.ok(re_match("a\\bb", "bbb") == RE_E_NOTIMPL);
	sqt.ok(re_match("[a-\\n", "bbb") == RE_E_NOTIMPL);
	sqt.ok(re_match("[\\n-a", "bbb") == RE_E_NOTIMPL);
	//sqt.ok(re_match("[-]", "bbb") == RE_E_NOTIMPL);
	//sqt.ok(re_match("[a-b-c]", "bbb") == RE_E_NOTIMPL);
	sqt.ok(re_match("[a--]", "bbb") == RE_E_NOTIMPL);
	//sqt.ok(re_match("[a-z-[bc]]", "bbb") == RE_E_NOTIMPL);
	sqt.ok(re_match("0[xX][\\da-fA-F]{1,4}", "0x12a6") == RE_E_OK);
	//sqt.ok(re_match("0[xX][\\da-fA-F]{1,4}", "0x12ag") == RE_E_FAIL);
	sqt.ok(re_match("(a|c)+", "aa") == RE_E_OK);
	sqt.ok(re_match("(a|c)+", "cc") == RE_E_OK);
	sqt.ok(re_match("(a|c)+", "cccc") == RE_E_OK);
	sqt.ok(re_match("(a|c)+", "aaaa") == RE_E_OK);
	sqt.ok(re_match("(a|c)+", "ca") == RE_E_OK);
	sqt.ok(re_match("(a|c)+", "caca") == RE_E_OK);
	sqt.ok(re_match("(a|b|c|d)+", "aa") == RE_E_OK);
	sqt.ok(re_match("(a|b|c|d)+", "bb") == RE_E_OK);
	sqt.ok(re_match("(a|b|c|d)+", "cc") == RE_E_OK);
	sqt.ok(re_match("(a|b|c|d)+", "dd") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "aa") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "bb") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "cc") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "abc") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "cba") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "cccc") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "aa") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "aaaa") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "ca") == RE_E_OK);
	sqt.ok(re_match("(a|b|c)+", "caca") == RE_E_OK);
	sqt.ok(re_match("(ab|cd|ed)+", "abcded") == RE_E_OK);
	//sqt.ok(re_match("(?:ab)", "a") == RE_E_SYNTAX);
	//sqt.ok(re_match("[^a]*", "") == RE_E_OK);
	sqt.ok(re_match("[^a]+", "") == RE_E_FAIL);
	sqt.ok(re_match("[^a]*", "b") == RE_E_OK);
	sqt.ok(re_match("[^a]*", "bcbd") == RE_E_OK);
	//sqt.ok(re_match("[^a]*", "a") == RE_E_FAIL);
	sqt.ok(re_match("[^a]*", "bac") == RE_E_OK);
	sqt.ok(re_match("[^ab]*", "bac") == RE_E_OK);
	sqt.ok(re_match("[^ab]*", "cded") == RE_E_OK);
	sqt.ok(re_match("[\\^a]*", "bac") == RE_E_OK);
	sqt.ok(re_match("[\\^a]*", "a") == RE_E_OK);
	sqt.ok(re_match("[\\^a]*", "a^") == RE_E_OK);
	sqt.ok(re_match("(a|[\\(])+", "a(aa((a") == RE_E_OK);
	sqt.ok(re_match("(a|[\\[])+", "a[aa[[a") == RE_E_OK);
	sqt.ok(re_match("(a|[(])+", "a(aa((a") == RE_E_OK);
	sqt.ok(re_match("(a|[[])+", "a(aa((a") == RE_E_OK);
	sqt.ok(re_match("\\D*", "bac") == RE_E_OK);
	sqt.ok(re_match("\\D*", "b0c") == RE_E_OK);
	sqt.ok(re_match("\\S*", "bac") == RE_E_OK);
	sqt.ok(re_match("\\S*", "b c") == RE_E_OK);
	sqt.ok(re_match(".", "") == RE_E_FAIL);
	sqt.ok(re_match("a.b", "acb") == RE_E_OK);
	sqt.ok(re_match("a.b", "a\nb") == RE_E_OK);
});

sqt.run("table", function(){
	
	local t = table_new(10);
	local t2 = obj_clone(t);
	sqt.ok((type(t2) == "table") && (t != t2));
	sqt.ok(table_len(t) == 0);
	table_rawset(t, "one", 1);
	sqt.ok(table_len(t) == 1 && (table_get(t, "one") == 1));
	table_set(t, "one", 10);
	sqt.ok((table_get(t, "one") == 10));
	table_rawset(t, "two", 2);
	sqt.ok(table_len(t) == 2 && (table_get(t, "two") == 2));
	table_rawdelete(t, "one");
	sqt.ok(table_len(t) == 1 && (table_get(t, "two") == 2));
	sqt.ok(table_rawin(t, "two"));
	table_clear(t);
	sqt.ok(table_len(t) == 0);

	sqt.ok({ abc = "abc" } != { abc = "abc" });

});

sqt.run("array", function(){
	
	local t = array(0);
	local t2 = obj_clone(t);
	sqt.ok((type(t2) == "array") && (t != t2));
	sqt.ok(t.len() == 0);
	t.append(1);
	sqt.ok(t.len() == 1 && (t.get(0) == 1));
	t.clear();
	sqt.ok(t.len() == 0);

	sqt.ok([ 0, 1, 2 ] != [ 0, 1, 2 ]);

	t = ["one"];
	sqt.ok(t.concat(",") == "one");

	t = ["one", "two"];
	sqt.ok(t.concat(", ") == "one, two");
	sqt.ok(t.len() == 2);
	sqt.ok(t.capacity() == 2);
	t.reserve(100);
	sqt.ok(t.len() == 2);
	sqt.ok(t.capacity() == 100);
	t.clear();
	sqt.ok(t.len() == 0);
	sqt.ok(t.capacity() == 100);
	t.extend([1,2,3,4,5,6,7,8,9,10]);
	sqt.ok(t.len() == 10);
	sqt.ok(t.capacity() == 100);
	t.minsize(5);
	sqt.ok(t.len() == 10);
	sqt.ok(t.capacity() == 100);
	t.resize(5);
	sqt.ok(t.len() == 5);
	sqt.ok(t.capacity() == t.len());
	
});

//	core/string/index_of_start.wren
sqt.run("string", function(){

	sqt.ok( ("a".toupper() + "A".tolower()) == "Aa");
	sqt.ok( ("à".sl_upper() + "À".sl_lower()) == "Àà");
	
	sqt.ok(type("s") == "string");
	sqt.ok("a" + "b" == "ab");
	
	local str = "abcde";
	
	sqt.ok(str.slice(0, 0) == "");
	sqt.ok(str.slice(1, 1) == "");
	sqt.ok(str.slice(1, 3) == "bc");
	sqt.ok(str.slice(1, 2) == "b");
	sqt.ok(str.slice(2) == "cde");
	sqt.ok(str.slice(2, 5) == "cde");
	//sqt.ok(str.slice(3, 1) == "dcb");
	//sqt.ok(str.slice(3, 1) == "dc");
	sqt.ok(str.slice(3, 3) == "");
	sqt.ok(str.slice(-5, -2) == "abc");
	//sqt.ok(str.slice(-3, -5) == "cba");
	//sqt.ok(str.slice(-3, -6) == "cba");
	sqt.ok(str.slice(-5, 3) == "abc");
	sqt.ok(str.slice(-3, 5) == "cde");
	//sqt.ok(str.slice(-2, 1) == "dcb");
	//sqt.ok(str.slice(-2, 0) == "dcb");
	sqt.ok(str.slice(1, -2) == "bc");
	sqt.ok(str.slice(2, -1) == "cd");
	//sqt.ok(str.slice(4, -5) == "edcba");
	//sqt.ok(str.slice(3, -6) == "dcba");
	sqt.ok("".slice(0, 0) == "");
	//sqt.ok("".slice(0, -1) == "");
	sqt.ok("abc".slice(3, 3) == "");
	//sqt.ok("abc".slice(3, -1) == "");
	sqt.ok("søméஃthîng".slice(0, 3) == "sø");
	sqt.ok("søméஃthîng".slice(3, 10) == "méஃt");
	sqt.ok("søméஃthîng".slice(3, 9) == "méஃ");
	sqt.ok("søméஃthîng".slice(3, 6) == "mé");
	
	sqt.ok("".tostring() == "");
	sqt.ok("blah".tostring() == "blah");
	sqt.ok("a\0b\0c".tostring() == "a\0b\0c");
	sqt.ok("a\0b\0c".tostring() != "a");

	sqt.ok("abcd"[0] == 'a');
	sqt.ok("abcd"[1] == 'b');
	sqt.ok("abcd"[2] == 'c');
	sqt.ok("abcd"[3] == 'd');
	sqt.ok("abcd"[-4] == 'a');
	sqt.ok("abcd"[-3] == 'b');
	sqt.ok("abcd"[-2] == 'c');
	sqt.ok("abcd"[-1] == 'd');
	sqt.ok("abcd"[1] == 'b');
	sqt.ok("søméஃthîng"[0] == 's');
	//sqt.ok("søméஃthîng"[1] == 'ø');
	sqt.ok("søméஃthîng"[3] == 'm');
	//sqt.ok("søméஃthîng"[6] == 'ஃ');
	sqt.ok("søméஃthîng"[10] == 'h');
	sqt.ok("søméஃthîng"[-1] == 'g');
	sqt.ok("søméஃthîng"[-2] == 'n');
	//sqt.ok("søméஃthîng"[-4] == 'î');
	sqt.ok("søméஃthîng"[2] == '\xb8');
	sqt.ok("søméஃthîng"[7] == '\xae');
	sqt.ok("søméஃthîng"[8] == '\x83');
	sqt.ok("søméஃ"[-1] == '\x83');
	sqt.ok("søméஃ"[-2] == '\xae');
	sqt.ok("a\0b\0c"[0] == 'a');
	sqt.ok("a\0b\0c"[1] == '\0');
	sqt.ok("a\0b\0c"[2] == 'b');
	sqt.ok("a\0b\0c"[3] == '\0');
	sqt.ok("a\0b\0c"[4] == 'c');
	sqt.ok("\xef\xf7"[0] == '\xef');
	sqt.ok("\xef\xf7"[1] == '\xf7');

	//sqt.ok("something".split("meth") == ["so", "ing"]);
	//sqt.ok("something".split("some") == ["thing"]);
	//sqt.ok("something".split("math") == ["something"]);
	//sqt.ok("somethingsomething".split("meth") == ["so", "ingso", "ing"]);
	//sqt.ok("abc abc abc".split(" ") == ["abc", "abc", "abc"]);
	//sqt.ok("abcabcabc".split("abc") == [, , , ]);
	//sqt.ok("søméthîng".split("meth") == ["søméthîng"]);
	//sqt.ok("a\0b\0c".split("\0") == ["a", "b", "c"]);

	sqt.ok("something".replace("some", "no") == "nothing");
	sqt.ok("something".replace("thing", "one") == "someone");
	sqt.ok("something".replace("math", "ton") == "something");
	sqt.ok("somethingsomething".replace("some", "no") == "nothingnothing");
	sqt.ok("abc abc abc".replace(" ", "") == "abcabcabc");
	sqt.ok("søméthîng".replace("meth", "ton") == "søméthîng");
	sqt.ok("a\0b\0c".replace("\0", "") == "abc");
	sqt.ok("a\0b\0c".replace("b", "") == "a\0\0c");

	sqt.ok(!"s" == false);
	sqt.ok(!"" == false);

	sqt.ok("a string".len() == 8);
	//sqt.ok("søméஃthîng".len() == 10);
	sqt.ok("\xefok\xf7".len() == 4);

	sqt.ok("abcd".startswith("cd") == false);
	sqt.ok("abcd".startswith("a") == true);
	sqt.ok("abcd".startswith("abcd") == true);
	sqt.ok("abcd".startswith("abcde") == false);
	sqt.ok("abcd".startswith("") == true);
	sqt.ok("søméthîng".startswith("some") == false);

	sqt.ok("abcd".endswith("cd") == true);
	sqt.ok("abcd".endswith("abcde") == false);
	sqt.ok("abcd".endswith("abcd") == true);
	sqt.ok("abcd".endswith("f") == false);
	sqt.ok("abcd".endswith("") == true);
	sqt.ok("søméthîng".endswith("thing") == false);

	sqt.ok("vålue" != "value");
	sqt.ok("vålue" == "vålue");
	sqt.ok("a\0b\0c" != "a");
	sqt.ok("a\0b\0c" != "abc");
	sqt.ok("a\0b\0c" == "a\0b\0c");

	sqt.ok("abcd".indexOf("") == 0);
	sqt.ok("abcd".indexOf("cd") == 2);
	sqt.ok("abcd".indexOf("a") == 0);
	sqt.ok("abcd".indexOf("abcd") == 0);
	sqt.ok("abcd".indexOf("abcde") == -1);
	sqt.ok("abab".indexOf("ab") == 0);
	sqt.ok("abcdefabcdefg".indexOf("defg") == 9);
	sqt.ok("abcdabcdabcd".indexOf("dab") == 3);
	sqt.ok("abcdabcdabcdabcd".indexOf("dabcdabc") == 3);
	sqt.ok("abcdefg".indexOf("abcdef!") == -1);
	sqt.ok("søméஃthîng".indexOf("e") == -1);
	sqt.ok("søméஃthîng".indexOf("m") == 3);
	sqt.ok("søméஃthîng".indexOf("thî") == 9);
	//sqt.ok("a\0b\0c".indexOf("\0") == 1);
	sqt.ok("a\0b\0c".indexOf("a") == 0);
	//sqt.ok("a\0b\0c".indexOf("b\0c") == 2);
	//sqt.ok("a\0b\0c".indexOf("a\0b\0c\0d") == -1);
	sqt.ok("a\0b\0a\0b".indexOf("a\0b") == 0);

	sqt.ok("abcd".indexOf("") == 0);
	sqt.ok("aaaaa".indexOf("aaaa") == 0);
	sqt.ok("abcd".indexOf("cd") == 2);
	//sqt.ok("here as well as here".indexOf("here") == 16);
	//sqt.ok("a\0b\0c".indexOf("\0") == 1);
	sqt.ok("a\0b\0c".indexOf("a") == 0);
	//sqt.ok("a\0b\0c".indexOf("a\0b\0c\0d") == -1);
	sqt.ok("a\0b\0a\0b".indexOf("a\0b") == 0);
	
	local base_str = "one two"
	str = "one \
two";
	sqt.ok(base_str == str);
	str = "one "
		"two";
	sqt.ok(base_str == str);

	sqt.ok(@"foo
bar" == "foo\nbar");
	sqt.ok("foo \
bar" == "foo bar");
	sqt.ok("foo "
"bar" == "foo bar");
	sqt.ok(R"(foo
bar)" == "foo\nbar");
	sqt.ok([==[foo
bar]==] == "foo\nbar");

	sqt.ok(("(" + 0x7fffffff + ")") == "(2147483647)");
	sqt.ok(("(" + -0x80000000 + ")") ==  "(-2147483648)");

});

sqt.run("number", function(){

	sqt.ok(0.0f == 0.0); //C/C++ notation
//adapetd from pike https://github.com/pikelang/Pike
	sqt.ok(1e1 == 10.0);
	sqt.ok(1E1 == 10.0);
	sqt.ok(1e+1 == 10.0);
	sqt.ok(1.1e1 == 11.0);
	sqt.ok(1e-1 == 0.1);
	sqt.ok('\x20' == 32);
	sqt.ok("\x20" == "\040");
	//sqt.ok("\d32" == "\x20");

	sqt.ok('�' == "�"[0]);
	//sqt.ok('\7777' == "\7777"[0]);
	//sqt.ok('\77777777' == "\77777777"[0]);
	sqt.ok("\x10000" == "\x10000");
	sqt.ok(0x80000000-0x80000000 ==  0);
	//if(!Is32Bits) sqt.ok(0xf0000000-0xf0000000 ==  0);
	sqt.ok(0x80000001-0x80000000 ==  1);
	sqt.ok(0x80000000-0x80000001 == -1);
	sqt.ok(-2147483648*-1 ==  -2147483648/-1);
	//if(!Is32Bits) sqt.ok(0x8000000000000000-0x8000000000000000 ==  0);
	//sqt.ok(0xf000000000000000-0xf000000000000000 ==  0);
	//if(!Is32Bits) sqt.ok(0x8000000000000001-0x8000000000000000 ==  1);
	//if(!Is32Bits) sqt.ok(0x8000000000000000-0x8000000000000001 == -1);
	//sqt.ok(-9223372036854775808*-1 ==  -9223372036854775808/-1);
// https://github.com/pikelang/Pike

	sqt.ok(15 == "0x0F".tointeger(16));
	sqt.ok(15 == "F".tointeger(16));
	sqt.ok(15 == "017".tointeger(8));
	sqt.ok(15 == "17".tointeger(8));
	sqt.ok(15 == "1111".tointeger(2));
	sqt.ok(15 == "00001111".tointeger(2));

	sqt.ok("0.5".tofloat() == 0.5);
	sqt.ok("0.5" == (0.5).tostring());
	sqt.ok("9.99".tofloat() == 9.99);
	sqt.ok("9.99" == (9.99).tostring());
	sqt.ok("12".tointeger() == 12);
	sqt.ok("12" == (12).tostring());
	sqt.ok("12.0".tofloat() == 12.0);
	//sqt.ok("12.0" == (12.0).tostring());
	sqt.ok("12.0e+10".tofloat() == 12.0e+10);
	sqt.ok("-122131242342.0e-10".tofloat() == -122131242342.0e-10);
	//sqt.ok("-122131242342.0e-10" == (-122131242342.0e-10).tostring());
	//sqt.ok(".5".tofloat() == .5);
	sqt.ok("122138760e-2".tofloat() == 122138760e-2);
	//sqt.ok("122138760e-2" == (122138760e-2).tostring());
	
	// signed and unsigned integer
	sqt.ok( 1 == 1 );
	sqt.ok(-1 == -1);
	//sqt.ok(+1 == +1);
	// signed and unsigned float
	sqt.ok( 1.0 == 1.0 );
	sqt.ok(-1.0 == -1.0);
	// binary
	//sqt.ok( 128 == 0b10000000 );
	//sqt.ok( 128 == 0B10000000 );
	// octal
	//sqt.ok( 8 == 0o10 );
	//sqt.ok( 8 == 0O10 );
	//sqt.ok( 8 == 0_10 );
	// hex
	sqt.ok( 255 == 0xff );
	sqt.ok( 255 == 0Xff );
	// decimal
	//sqt.ok( 999 == 0d999 );
	//sqt.ok( 999 == 0D999 );
	// decimal seperator
	//sqt.ok( 10000000 == 10_000_000 );
	//sqt.ok( 10 == 1_0 );
	// integer with exponent
	sqt.ok( 10.0 == 1e1 );
	sqt.ok(0.1 == 1e-1);
	sqt.ok( 10.0 == 1e+1 );
	// float with exponent
	sqt.ok( 10.0 == 1.0e1 );
	sqt.ok(0.1 == 1.0e-1);
	sqt.ok( 10.0 == 1.0e+1 );

	sqt.ok(format("%7.1e",  68.514) == "6.9e+01");
	sqt.ok(format("%4d %4d %4d %4d %d %#x %#X", 6, 34, 16923, -12, -1, 14, 12) == "   6   34 16923  -12 -1 0xe 0XC");

	sqt.ok((3.123456788 + 0.000000001) == 3.123456789);
	sqt.fequal((3.123456789 + 1), 4.123456789);

	sqt.ok((3.123456790 - 0.000000001) == 3.123456789);
	sqt.ok((5.123456789 - 1) == 4.123456789);

	sqt.ok((3.125 * 3.125) == 9.765625);
	sqt.ok((3.125 * 1) == 3.125);

	sqt.ok((3.123456789 / 3.123456789) == 1.0);
	sqt.ok((3.123456789 / 1) == 3.123456789);

	sqt.ok(math.abs(123) == 123);
	sqt.ok(math.abs(-123) == 123);
	sqt.ok(math.abs(0) == 0);
	sqt.ok(math.abs(-0) == 0);
	sqt.ok(math.fabs(-0.12) == 0.12);
	sqt.ok(math.fabs(12.34) == 12.34);

	//print("math.acos(0)", math.acos(0))
	sqt.fequal(math.acos(0), 1.5707963267949);
	sqt.ok(math.acos(1) == 0);
	sqt.fequal(math.acos(-1), 3.1415926535898);

	sqt.ok(math.asin(0) == 0);
	sqt.fequal(math.asin(1), 1.5707963267949);
	sqt.fequal(math.asin(-1), -1.5707963267949);

	sqt.ok(math.atan(0) == 0);
	sqt.fequal(math.atan(1), 0.78539816339745);

	sqt.ok(math.atan2(0, 0) == 0);
	sqt.ok(math.atan2(0, 1) == 0);
	sqt.fequal(math.atan2(1, 0), 1.5707963267949);

	sqt.ok(math.ceil(123) == 123);
	sqt.ok(math.ceil(-123) == -123);
	sqt.ok(math.ceil(0) == 0);
	sqt.ok(math.ceil(-0) == -0);
	sqt.ok(math.ceil(0.123) == 1);
	sqt.ok(math.ceil(12.3) == 13);
	sqt.ok(math.ceil(-0.123) == -0);
	sqt.ok(math.ceil(-12.3) == -12);

	sqt.ok(math.floor(123) == 123);
	sqt.ok(math.floor(-123) == -123);
	sqt.ok(math.floor(0) == 0);
	sqt.ok(math.floor(-0) == -0);
	sqt.ok(math.floor(0.123) == 0);
	sqt.ok(math.floor(12.3) == 12);
	sqt.ok(math.floor(-0.123) == -1);
	sqt.ok(math.floor(-12.3) == -13);

	sqt.ok(math.tan(0) == 0);
	sqt.fequal(math.tan(math.PI / 4), 1.0);
	sqt.fequal(math.tan(-math.PI / 4), -1.0);

	sqt.ok(math.cos(0) == 1);
	sqt.ok(math.cos(math.PI) == -1);
	sqt.ok(math.cos(2 * math.PI) == 1);
	sqt.ok(math.abs(math.cos(math.PI / 2)) < 1.0e-16);

	sqt.ok(math.sin(0) == 0);
	sqt.ok(math.sin(math.PI / 2) == 1);
	sqt.ok(math.abs(math.sin(math.PI)) < 1.0e-15);
	sqt.ok(math.abs(math.sin(2 * math.PI)) < 1.0e-15);

	sqt.fequal(math.log(3), 1.0986122886681);
	sqt.fequal(math.log(100), 4.6051701859881);
	sqt.ok(math.isnan(math.log(-1)));

	sqt.ok(math.pow(2,4) == 16);
	sqt.ok(math.pow(2, 10) == 1024);
	sqt.ok(math.pow(1, 0) == 1);
	
	sqt.ok(math.sqrt(4) == 2);
	sqt.ok(math.sqrt(1000000) == 1000);
	sqt.ok(math.sqrt(1) == 1);
	sqt.ok(math.sqrt(-0) == -0);
	sqt.ok(math.sqrt(0) == 0);
	sqt.fequal(math.sqrt(2), 1.4142135623731);
	sqt.ok(math.isnan(math.sqrt(-4)) == true);

/*
	sqt.ok(123.truncate == 123);
	sqt.ok((-123).truncate == -123);
	sqt.ok(0.truncate == 0);
	sqt.ok((-0).truncate == -0);
	sqt.ok(0.123.truncate == 0);
	sqt.ok(12.3.truncate == 12);
	sqt.ok((-0.123).truncate == -0);
	sqt.ok((-12.3).truncate == -12);
	sqt.ok((12345678901234.5).truncate == 12345678901234);
	sqt.ok((-12345678901234.5).truncate == -12345678901234);
*/
/*
	sqt.ok(123.sign == 1);
	sqt.ok((-123).sign == -1);
	sqt.ok(0.sign == 0);
	sqt.ok((-0).sign == 0);
	sqt.ok(0.123.sign == 1);
	sqt.ok((-0.123).sign == -1);
*/
/*
	sqt.ok(123.round == 123);
	sqt.ok((-123).round == -123);
	sqt.ok(0.round == 0);
	sqt.ok((-0).round == -0);
	sqt.ok(0.123.round == 0);
	sqt.ok(12.3.round == 12);
	sqt.ok((-0.123).round == -0);
	sqt.ok((-12.3).round == -12);
*/
	sqt.ok(123 % 1 == 0);
	sqt.ok((-123) % 1 == -0);
	sqt.ok(0 % 1 == 0);
	sqt.ok((-0) % 1 == -0);
	sqt.ok((0.123) % 1.0 == 0.123);
	sqt.ok((-0.123) % 1.0 == -0.123);
	sqt.fequal((12.3) % 1.0, 0.3);
	sqt.fequal((-12.3) % 1.0, -0.3);
	sqt.fequal((1.23456789012345) % 1.0, 0.23456789012345);
	sqt.fequal((-1.23456789012345) % 1.0, -0.23456789012345);
	sqt.ok((0.000000000000000000000000000000000000000001) % 1.0 == 1e-42);
	sqt.ok((-0.000000000000000000000000000000000000000001) % 1.0 == -1e-42);
	sqt.ok((1.000000000000000000000000000000000000000001) % 1.0 == 0);
	sqt.ok((-1.000000000000000000000000000000000000000001) % 1.0 == -0);

	sqt.ok("123".tointeger() == 123);
	sqt.ok("-123".tointeger() == -123);
	sqt.ok("-0".tointeger() == -0);
	sqt.ok("12.34".tofloat() == 12.34);
	sqt.ok("-0.0001".tofloat() == -0.0001 == true);
	sqt.ok(" 12 ".tointeger() == 12);
	try{"test1".tointeger(), sqt.ok(0);}catch(e){sqt.ok(1);};
	try{"test1".tofloat(), sqt.ok(0);}catch(e){sqt.ok(1);};
	//sqt.ok("".tointeger() == null);
	//sqt.ok("prefix1.2".tofloat() == null);
	//sqt.ok("1.2suffix".tofloat() == null);
	sqt.ok("1.2suffix".tofloat() == 1.2); // ??????
	sqt.ok("1.2suffix".tointeger() == 1); // ?????
	
	sqt.ok((123).tostring() == "123");
	sqt.ok((-123).tostring() == "-123");
	//sqt.ok((-0).tostring() == "-0");
	sqt.ok((12.34).tostring() == "12.34");
	sqt.ok((-0.0001).tostring() == "-0.0001");

	sqt.ok(type(123) == "integer");
	sqt.ok(type(123.0) != "integer");
	sqt.ok(type(0) == "integer");
	sqt.ok(type(1.0000001) != "integer");
	sqt.ok(type(-2.3) != "integer");
	//sqt.ok(type(0/0).isInteger == false);
	//sqt.ok(type(1/0).isInteger == false);

	//sqt.fequal(math.HUGE, 1.7976931348623e+308);
	//sqt.ok(Num.smallest == 2.2250738585072e-308);

	sqt.ok(1 < 2);
	sqt.ok(!(2 < 2));
	sqt.ok(!(2 < 1));
	sqt.ok(1 <= 2);
	sqt.ok(2 <= 2);
	sqt.ok(!(2 <= 1));
	sqt.ok(!(1 > 2));
	sqt.ok(!(2 > 2));
	sqt.ok(2 > 1);
	sqt.ok(!(1 >= 2));
	sqt.ok(2 >= 2);
	sqt.ok(2 >= 1);
	sqt.ok(!(0 < -0));
	sqt.ok(!(-0 < 0));
	sqt.ok(!(0 > -0));
	sqt.ok(!(-0 > 0));
	sqt.ok(0 <= -0);
	sqt.ok(-0 <= 0);
	sqt.ok(0 >= -0);
	sqt.ok(-0 >= 0);

	sqt.ok((8 / 2) == 4);
	sqt.fequal((12.34 / -0.4), -30.85);
	//sqt.ok(!math.isfinite(3 / 0)); // == infinity);
	//sqt.ok(!math.isfinite(-3 / 0)); // == -infinity);
	//sqt.ok(math.isnan(0 / 0));
	//sqt.ok(math.isnan(-0 / 0));
	//sqt.ok(!math.isfinite(3 / -0)); // == -infinity);
	//sqt.ok(!math.isfinite(-3 / -0)); // == infinity);
	//sqt.ok(math.isnan(0 / -0));
	//sqt.ok(math.isnan(-0 / -0));

	sqt.ok(123 == 123);
	sqt.ok(123 != 124);
	sqt.ok(-3 != 3);
	sqt.ok(0 == -0);
	sqt.ok(123 != "123");
	sqt.ok(1 == true);
	sqt.ok(0 == false);
	sqt.ok(1 !== true);
	sqt.ok(0 !== false);

	sqt.ok((0 & 0) == 0);
	sqt.ok((0xaaaaaaaa & 0x55555555) == 0);
	//sqt.ok((0xf0f0f0f0 & 0x3c3c3c3c) == 808464432);
	//sqt.ok((0xffffffff & 0xffffffff) == 4294967295);
	
	try{1 & false, sqt.ok(0)} catch(e) {sqt.ok(1);} // expect runtime error: Right operand must be a number.

	sqt.ok((0 << 0) == 0);
	sqt.ok((1 << 0) == 1);
	sqt.ok((0 << 1) == 0);
	sqt.ok((1 << 1) == 2);
	//sqt.ok((0xffffffff << 0) == 4294967295);

	sqt.ok((0 >> 0) == 0);
	sqt.ok((1 >> 0) == 1);
	sqt.ok((0 >> 1) == 0);
	sqt.ok((1 >> 1) == 0);
/*
	if(_intsize_ == 8)
	{
		sqt.ok((0xaaaaaaaa << 1) == 5726623060);
		sqt.ok((0xf0f0f0f0 << 1) == 8084644320);
		sqt.ok((0xaaaaaaaa >> 1) == 1431655765);
		sqt.ok((0xf0f0f0f0 >> 1) == 2021161080);
		sqt.ok((0xffffffff >> 1) == 2147483647);
	}
	else
	{
		sqt.ok((0xaaaaaaaa << 1) == 1431655764);
		sqt.ok((0xf0f0f0f0 << 1) == -505290272);
		sqt.ok((0xaaaaaaaa >> 1) == -715827883);
		sqt.ok((0xf0f0f0f0 >> 1) == -126322568);
		sqt.ok((0xffffffff >> 1) == -1);
	}

*/
	sqt.ok((0 ^ 0) == 0);
	sqt.ok((1 ^ 1) == 0);
	sqt.ok((0 ^ 1) == 1);
	sqt.ok((1 ^ 0) == 1);
	//sqt.ok((0xaaaaaaaa ^ 0x55555555) == 4294967295);
	//sqt.ok((0xf0f0f0f0 ^ 0x3c3c3c3c) == 3435973836);
	//sqt.ok((0xffffffff ^ 0xffffffff) == 0);

	//sqt.ok((~0) == 4294967295);
	//sqt.ok((~1) == 4294967294);
	//sqt.ok((~23) == 4294967272);
	//sqt.ok((~0xffffffff) == 0);
	//sqt.ok((~1.23) == 4294967294);
	//sqt.ok((~0.00123) == 4294967295);
	//sqt.ok((~345.67) == 4294966950);

	sqt.ok((0 | 0) == 0);
	//sqt.ok((0xaaaaaaaa | 0x55555555) == 4294967295);
	//sqt.ok((0xcccccccc | 0x66666666) == 4008636142);
	//sqt.ok((0xffffffff | 0xffffffff) == 4294967295);

	local a = 3;
	sqt.ok((5 - 3) == 2);
	sqt.fequal((3.1 - 0.24),  2.86);
	sqt.ok((3 - 2 - 1) == 0);
	sqt.ok(-a == -3);

	sqt.ok((1 + 2) == 3);
	sqt.ok((12.34 + 0.13) == 12.47);
	sqt.ok((3 + 5 + 2) == 10);

	sqt.ok((5 % 3) == 2);
	sqt.ok((10 % 5) == 0);
	sqt.ok((-4 % 3) == -1);
	sqt.ok((4 % -3) == 1);
	sqt.ok((-4 % -3) == -1);
	sqt.ok((-4.2 % 3.1) == -1.1);
	sqt.ok((4.2 % -3.1) == 1.1);
	sqt.ok((-4.2 % -3.1) == -1.1);
	sqt.ok((13 % 7 % 4) == 2);
	sqt.ok((13 + 1 % 7) == 14);

	sqt.ok((5 * 3) == 15);
	sqt.ok((12.34 * 0.3) == 3.702);

});

sqt.run("enum", function(){
	enum e1 {one=1, two};
	sqt.ok(e1.one == 1);
	sqt.ok(e1.two == 2);

	enum e2 {one=-1, two, three};
	sqt.ok(e2.one == -1);
	sqt.ok(e2.two == 0);
	sqt.ok(e2.three == 1);

	enum e3 {one=-1, two, three, nine=9, ten};
	sqt.ok(e3.one == -1);
	sqt.ok(e3.two == 0);
	sqt.ok(e3.three == 1);
	sqt.ok(e3.nine == 9);
	sqt.ok(e3.ten == 10);
});

return sqt.results();           //show results