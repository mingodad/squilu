auto bv_size = 800000;
auto step = 1;

auto sqBitVect = blob((bv_size / 8) + 512);
//sqBitVect.memset(0, 0, sqBitVect.len()-1);

auto bitGet(bpos)
{
	auto cpos = bpos/8;
	auto b8 = 1 << (bpos%8);
	return sqBitVect[cpos] & b8;
}

auto bitSet(bpos)
{
	auto cpos = bpos/8;
	auto b8 = 1 << (bpos%8);
	sqBitVect[cpos] |= b8;
}
 
auto bitClear(bpos)
{
	auto cpos = bpos/8;
	auto b8 = 1 << (bpos%8);
	sqBitVect[cpos] &= (~b8);
}
 
auto bitTogle(bpos)
{
	auto cpos = bpos/8;
	auto b8 = 1 << (bpos%8);
	sqBitVect[cpos] ^= b8;
}

auto bit_pos = 3;

auto cpos = bit_pos/8;
auto m8 = (bit_pos%8);
auto b8 = 1 << m8;

print(bit_pos, cpos, m8, b8);

print(bitGet(bit_pos));
bitSet(bit_pos);
print(bitGet(bit_pos));
bitClear(bit_pos);
print(bitGet(bit_pos));
bitTogle(bit_pos);
print(bitGet(bit_pos));

print("===");
++bit_pos;
print(sqBitVect.bitGet(bit_pos));
sqBitVect.bitSet(bit_pos);
print(sqBitVect.bitGet(bit_pos));
sqBitVect.bitClear(bit_pos);
print(sqBitVect.bitGet(bit_pos));
sqBitVect.bitTogle(bit_pos);
print(sqBitVect.bitGet(bit_pos));

//return;
 
 //BitVector is 1 based
auto bv = BitVector(bv_size);
print(bv);

auto start_milli = os.getmillicount();
for(auto i=1; i <= bv_size; i+=step) bv.set(i);
print("Time spent set bv a", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=1; i <= bv_size; i+=step) bv[i] = false;
print("Time spent set bv b", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=1; i <= bv_size; i+=step) bv[i] = true;
print("Time spent set bv c", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=1; i <= bv_size; i+=step) bv.test(i);
print("Time spent get bv a", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=1; i <= bv_size; i+=step) bv[i];
print("Time spent get bv b", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) bitSet(i);
print("Time spent set sq", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) bitGet(i);
print("Time spent get sq", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) sqBitVect.bitSet(i);
print("Time spent set blob", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) sqBitVect.bitGet(i);
print("Time spent get blob", os.getmillicount() - start_milli);

auto cbitSet = sqBitVect.bitSet;
auto cbitGet = sqBitVect.bitGet;

start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) rawcall(cbitSet, sqBitVect, i);
print("Time spent set rawcall blob", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) rawcall(cbitGet, sqBitVect, i);
print("Time spent get rawcall blob", os.getmillicount() - start_milli);

cbitSet = BitVector.set;
cbitGet = BitVector.test;
print(cbitSet, cbitGet);
//bv = BitVector(bv_size);

start_milli = os.getmillicount();
for(auto i=1; i <= bv_size; i+=step) rawcall(cbitSet, bv, i);
print("Time spent set rawcall bv", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=1; i <= bv_size; i+=step) rawcall(cbitGet, bv, i);
print("Time spent get rawcall bv", os.getmillicount() - start_milli);

bv = array(bv_size);
start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) bv[i] = true;
print("Time spent set array", os.getmillicount() - start_milli);

start_milli = os.getmillicount();
for(auto i=0; i < bv_size; i+=step) bv[i];
print("Time spent get array", os.getmillicount() - start_milli);
