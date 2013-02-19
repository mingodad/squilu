local a = 60;	/* 60 = 0011 1100 */  
local b = 13;	/* 13 = 0000 1101 */
local c = 0;           
c = a & b;                  /* 12 = 0000 1100 */ 
print(c);
c = a | b;                  /* 61 = 0011 1101 */ 
print(c);
c = a ^ b;                  /* 49 = 0011 0001 */
print(c);

local value=4;          /*   4 = 0000 0100 */  
value = ~ value;               /* 251 = 1111 1011 */ 
print(value);

value=4;
local shift=2;

value = value << shift; /* 16 = 0001 0000 */ 
printf("%d\n", value); /* Prints 16 */
value = value << shift; /* 64 = 0100 0000 */ 
printf("%d\n", value); /* Prints 64 */