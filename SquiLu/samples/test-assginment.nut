local count = 100000000;
local bval = true;

local start = os.clock();
for(local i=0; i< count; ++i) bval = !bval;
print("Time spent :", os.clock() -start, count);