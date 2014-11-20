class A {}
local instA = A();
print(instA);
local weakv = instA.weakref();
print(weakv);
local instBlob = blob();
print(instBlob);
weakv = instBlob.weakref();
print(weakv);

local t = {}
local a = ["first","second","third"]
//creates a weakref to the array and assigns it to a table slot
t.thearray <- a.weakref();
print(t.thearray[0]);
a = 123;
::print(typeof(t.thearray))

