local nbits = 31;
local one = 1;
local expected = 0;
local zero = 0;
for(local i=0; i < nbits; ++i)
{
	local bit = i+1;
	local value = zero;
	value |= bit;
	local value1 = zero | bit;
	local value2 = value1 & (~value1)
	//print(i, value, value1, value2);
	//if(value != value1) throw("Unexpected result");
}