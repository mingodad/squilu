local f1 = 1.1;
local f2 = 2.2;
print(f1+f2);

f1 = 0.1;
f2 = 0.3;
print(f1+f1+f1-f2);

local d = Decimal();
print(d.isnan(), d.iszero(), d.isodd(), d.iseven());

local dec1 = Decimal("1.1");
local dec2 = Decimal("2.2");
print(dec1 + dec2)


dec1 = Decimal("0.1");
dec2 = Decimal("0.3");
print(dec1+dec1+dec1-dec2);

dec1 = Decimal("10.0");
dec2 = Decimal("3.33");

local result = dec1 + dec2;
print(result);

result = dec1 - dec2;
print(result);

result = dec1 * dec2;
print(result);

result = dec1 / dec2;
print(result);

dec2 = Decimal(0);
try {
	result = dec1 / dec2;
}
catch(e){
	print(e);
}
print(result);
print(Decimal.context);
print(result.context.prec());

dec1 = Decimal("10.0");
dec2 = Decimal("3.33");

local count = 1000;
f1 = 10.54;
f2 = 3.28;

local now = os.clock();

for(local i=0; i < count; ++i){
	result = dec1 + dec2;
	result = dec1 - dec2;
	result = dec1 * dec2;
	result = dec1 / dec2;
	//if(i%1000) collectgarbage();
	if(i%1000) dummy(); //dummy function call to trigger release hook garbage collection
}

print("Decimal took", os.clock()-now);

now = os.clock();
for(local i=0; i < count; ++i){
	result = f1 + f2;
	result = f1 - f2;
	result = f1 * f2;
	result = f1 / f2;
	if(i%1000) dummy(); // only to make this loop with the same extra load of a function call
}
print("Float took", os.clock()-now);

//stdin.readn('c');
