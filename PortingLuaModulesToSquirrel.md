When porting code from Lua to Squirrel there is some differences that can make having the same functionality a bit trick.

# Error #
In lua calling "lua\_error" will jump out of the C code but in Squirrel we must explicitly {{{return sq\_throwerror()}}.
Lua uses return value checks, Squirrel throw exceptions.

# Foreach #
Squirrel foreach is done in two steps, first a call to "`_nexti`" and with the return value of it a call to "`_get`" to retrieve the data value.