local l = ffi.load("libc.so.6")

local strlen = l.bind_func("i", "strlen", ["s"]);
local str = "Hello World !";

print(str, str.len(), strlen(str));

local function mystrlen(str)
{
    return str.len();
} 

local function mystrlen2(str)
{
    local result = 0;
    for(local len=str.len(); result < len; ++result); 
    return result;
} 

const MAX_COUNT = 5000000;

local str_len = 0;
local start_time = os.clock();

for(local i=0; i < MAX_COUNT; ++i)
{
    str_len = str.len();
}

print(MAX_COUNT, str_len, os.clock() - start_time);

start_time = os.clock();

for(local i=0; i < MAX_COUNT; ++i)
{
    str_len = strlen(str);
}

print(MAX_COUNT, str_len, os.clock() - start_time);

start_time = os.clock();

for(local i=0; i < MAX_COUNT; ++i)
{
    str_len = mystrlen(str);
}

print(MAX_COUNT, str_len, os.clock() - start_time);

start_time = os.clock();

for(local i=0; i < MAX_COUNT; ++i)
{
    str_len = mystrlen2(str);
}

print(MAX_COUNT, str_len, os.clock() - start_time);

