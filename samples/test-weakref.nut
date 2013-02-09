class A {}
local instA = A();
print(instA);
local weakv = instA.weakref();
print(weakv);
local instBlob = blob();
print(instBlob);
weakv = instBlob.weakref();
print(weakv);
