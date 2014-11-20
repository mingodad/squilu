function coroutine_test(a,b)
{
	local gca = dad_utils.gc_scope_alert(@(ud) print(ud), "<<Gone !>> : " + __LINE__);
	::print(a+" "+b+"\n");
	local ret = ::suspend("suspend 1");
	gca = dad_utils.gc_scope_alert(@(ud) print(ud), "<<Gone !>> : " + __LINE__);
	::print("the coroutine says "+ret+"\n");
	ret = ::suspend("suspend 2");
	gca = dad_utils.gc_scope_alert(@(ud) print(ud), "<<Gone !>> : " + __LINE__);
	::print("the coroutine says "+ret+"\n");
	ret = ::suspend("suspend 3");
	gca = dad_utils.gc_scope_alert(@(ud) print(ud), "<<Gone !>> : " + __LINE__);
	::print("the coroutine says "+ret+"\n");
	gca = dad_utils.gc_scope_alert(@(ud) print(ud), "<<Gone !>> : " + __LINE__);
	::print(getstacktop());
	return "I'm done"
}

local coro = ::newthread(coroutine_test);

local susparam = coro.call("test","coroutine"); //starts the coroutine

function doIt(str : string){
	print(str);
}

local a = 23;
print(a);

local i = 1;
do
{
	::print("suspend passed ["+susparam+"]\n")
	susparam = coro.wakeup("ciao "+i);
	++i;
}while(coro.getstatus()=="suspended")

::print("getstacktop", getstacktop());
::print("return passed ["+susparam+"]\n")
::print("getstacktop", getstacktop());
print(a);
