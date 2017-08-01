local ex = regexp("[a-zA-Z]+");
local string = "123 Test; strlen(str);";
local res = ex.search(string);
print(string.slice(res.begin,res.end)); //prints "Test"
print("\n");
ex = regexp(@"\m()");
string = "123 Test; strlen(str);";
res = ex.search(string);
print(string.slice(res.begin,res.end)); //prints "(str)"

local re = regexp(@"(style|layout)\.width");

print(re.gsub(txt, "$1.dimensions[DIMENSION_WIDTH]"));