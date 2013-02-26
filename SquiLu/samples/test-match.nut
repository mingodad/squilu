local str = "Fl_Input";
print(str.match("Fl_"));
print(str.match("Fl_Input"));

print(str.substr(0,1), str.substr(2,3))
print(str.slice(0,1), str.slice(2,3))

local str2 = "Another day in paradize or another day in heaven?";
print(str2.slice(0,-1), str2.slice(0,-2))

local rc = str2.match("(day).-(paradize)")
print(type(rc));

local line = "HTTP/1.1 301 Moved Permanently";
rc = line.match("%s*(%S+)%s+(%d+)%s+(.+)");
print(rc);

i <- 2;
//print(i/0);
print(i%0);
//i <- -2147483648;
i <- -214748364;
print("i/-1", i, i/-1);
print("i%-1", i, i%-1);

local ar = ["\"ourbiz.dadbiz.es\"", "\"auth\"", "\"7f1de29e6da19d22b51c68001e7e0e54\""];
foreach(v in ar) {
	print(v);
	print(v.slice(1, -1));
}

str = [==[Digest qop="auth", realm="ourbiz.dadbiz.es", nonce="7f1de29e6da19d22b51c68001e7e0e54"]==];
		local function checkKV(k,v){
print(__LINE__, __FUNCTION__, k,v);
			if(v[0] == '"') v = v.slice(1, -1);
print(__LINE__, k,v);
			return true;
		}
		str.gmatch("(%w+)%s*=%s*([^%s,]+)", checkKV);
