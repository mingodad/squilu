local fname = "/home/mingo/dev/dadbiz++/third-party/sqlite3/sqlite3.c";

local count = 0;
local now = os.clock();
local fd = file(fname, "r");
while(fd.read_line()) ++count;
fd.close();
print("readline took ", os.clock()-now, " : ", count);

count = 0;
now = os.clock();
fd = file(fname, "r");
while(fd.read_line()) ++count;
fd.close();
print("readline2 took ", os.clock()-now, " : ", count);