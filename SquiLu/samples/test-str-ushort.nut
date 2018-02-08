local str = "\x10\x10";
print(str[0], str[1], str.uchar(0), str.uchar(1), str.ushort(0), (str.uchar(0) * 256) + str.uchar(1));

auto max_loop = 64 * 1024;
str = str.rep(max_loop);

auto start_time = os.clock();
for(auto i=0; i < max_loop; ++i)
{
	auto ch = str[i];
}
print("Time spent", max_loop, os.clock() - start_time);

start_time = os.clock();
for(auto i=0; i < max_loop; ++i)
{
	auto ch = str.uchar(i);
}
print("Time spent", max_loop, os.clock() - start_time);

start_time = os.clock();
for(auto i=0, len = max_loop/2; i < len; ++i)
{
	auto ch = str.ushort(i);
}
print("Time spent", max_loop, os.clock() - start_time);

start_time = os.clock();
for(auto i=0, len = max_loop/2; i < len; ++i)
{
	auto ch = (str[i] * 256) + str[i+1];
}
print("Time spent", max_loop, os.clock() - start_time);

local buf = blob();
buf.write(str);
buf.seek(0);
start_time = os.clock();
for(auto i=0, len = max_loop/2; i < len; ++i)
{
	auto ch = buf.readn('w');
}
print("Time spent", max_loop, os.clock() - start_time);
