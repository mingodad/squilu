Java.open();
print(Java.GetVersion());
local System = Java.FindClass("java/lang/System");
print(System);
print(System.currentTimeMillis());
Java.close();
