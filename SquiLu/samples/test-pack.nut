auto bpack=sqpack.pack
auto bunpack=sqpack.unpack

local function hex(s)
{
	s=s.gsub("(.)",function (x) {return format("%02X",x[0]);});
	return s;
}

/*
auto a=bpack("Ab8","\027Lua",5*16+1,0,1,4,4,4,8,0);
print(hex(a),a.len());

auto b=string.dump(hex);
b=b.slice(0,a.len());
print(a==b,b.len());
print(bunpack(b,"bA3b8"));
*/

auto i=314159265;
auto f="<I>I=I";
auto a=bpack(f,i,i,i);
print(hex(a));
auto up = bunpack(a,f);
print("up", type(up), up.len());
foreach(elm in up) print(elm);

i=3.14159265;
print(i);
f="<d>d=d";
a=bpack(f,i,i,i);
print(hex(a));
up = bunpack(a,f);
print("up", type(up), up.len());
foreach(elm in up) print(format("%.8f", elm));

i=3.14159265;
print(i);
f="<f>f=f";
a=bpack(f,i,i,i);
print(hex(a));
up = bunpack(a,f);
print("up", type(up), up.len());
foreach(elm in up) print(format("%.8f", elm));
