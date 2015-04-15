# New command line option "-s" to generate scriptable bytecode #

Here is an example of the output.


```
local bytecode = {
	source_name = "test-class-name-resolution.nut",
	function_name = "main"
	literals = [
		/*0*/ "A",
		/*1*/ "_value",
		/*2*/ "constructor",
		/*3*/ "print",
	],
	parameters = [
		/*0*/ "this",
		/*1*/ "vargv",
	],
	outervalues = [
		//[type, src, name],
	],
	localvarinfos = [
		//[pos, name, start_op, end_op, scope, type],
		/*0*/ [2, "a", 11, 13, 0, 1],
		/*1*/ [1, "vargv", 0, 14, 1, 1],
		/*2*/ [0, "this", 0, 14, 1, 1],
	],
	lineinfos = [
		//[op, line],
		/*0*/ [0, 1],
		/*1*/ [9, 10],
		/*2*/ [11, 11],
	],
	defaultparams = [],
	instructions = [
		//[op_str, op, arg0, arg1, arg2, arg3],
		/*0*/ ["_OP_LOAD", 1, 2, 0, 0, 0],		/* stk[2] <- literals[0] */
		/*1*/ ["_OP_NEWOBJ", 33, 3, -1, 255, 2],	/* stk[3], len(-1), NOT_CLASS(2) */
		/*2*/ ["_OP_LOAD", 1, 4, 1, 0, 0],		/* stk[4] <- literals[1] */
		/*3*/ ["_OP_LOADNULLS", 24, 5, 1, 0, 0],	/* stk_at_arg0[5 .. (arg0 + arg1(1))] = null */
		/*4*/ ["_OP_NEWSLOTA", 58, 0, 3, 4, 5],	/* flags(0), table_at_stk(3),  key_at_stk(4), val_at_stk(5) */
		/*5*/ ["_OP_LOAD", 1, 4, 2, 0, 0],		/* stk[4] <- literals[2] */
		/*6*/ ["_OP_CLOSURE", 48, 5, 0, 0, 0],	/* stk_at_arg0[5] <- functions[0] == ("constructor"), isLanbda(0) */
		/*7*/ ["_OP_NEWSLOTA", 58, 0, 3, 4, 5],	/* flags(0), table_at_stk(3),  key_at_stk(4), val_at_stk(5) */
		/*8*/ ["_OP_NEWSLOT", 11, 2, 0, 2, 3],	/* flags(2), table_at_stk(0),  key_at_stk(2), val_at_stk(3) */
		/*9*/ ["_OP_PREPCALLK", 8, 2, 0, 0, 3],	/* closure_at_stk[2], stk[0].get(literals[0]) -> stk[3] */
		/*10*/ ["_OP_CALL", 6, 2, 2, 3, 1],		/* target[2], closure_at_stk[2], stackbase(3), nargs(1) */
		/*11*/ ["_OP_PREPCALLK", 8, 3, 3, 0, 4],	/* closure_at_stk[3], stk[0].get(literals[3]) -> stk[4] */
		/*12*/ ["_OP_GETK", 9, 5, 1, 2, 0],		/* stk[5] <- literals[1] */
		/*13*/ ["_OP_CALL", 6, 255, 3, 4, 2],		/* target[255], closure_at_stk[3], stackbase(4), nargs(2) */
		/*14*/ ["_OP_RETURN", 23, 255, 0, 0, 0],	/* _arg0(0xFF) != 0xFF ? stk[0] : null */
	],
	functions = [
/*function 0*/{
	source_name = "test-class-name-resolution.nut",
	function_name = "constructor"
	literals = [
		/*0*/ "print",
		/*1*/ "dad",
		/*2*/ "last",
		/*3*/ "_value",
	],
	parameters = [
		/*0*/ "this",
	],
	outervalues = [
		//[type, src, name],
	],
	localvarinfos = [
		//[pos, name, start_op, end_op, scope, type],
		/*0*/ [1, "last", 6, 9, 1, 1],
		/*1*/ [0, "this", 0, 10, 1, 1],
	],
	lineinfos = [
		//[op, line],
		/*0*/ [0, 3],
		/*1*/ [0, 4],
		/*2*/ [4, 5],
		/*3*/ [6, 6],
		/*4*/ [10, 7],
	],
	defaultparams = [],
	instructions = [
		//[op_str, op, arg0, arg1, arg2, arg3],
		/*0*/ ["_OP_PREPCALLK", 8, 1, 0, 0, 2],	/* closure_at_stk[1], stk[0].get(literals[0]) -> stk[2] */
		/*1*/ ["_OP_GETK", 9, 3, 1, 0, 0],		/* stk[3] <- literals[1] */
		/*2*/ ["_OP_GETK", 9, 3, 2, 3, 0],		/* stk[3] <- literals[2] */
		/*3*/ ["_OP_CALL", 6, 255, 1, 2, 2],		/* target[255], closure_at_stk[1], stackbase(2), nargs(2) */
		/*4*/ ["_OP_GETK", 9, 1, 1, 0, 0],		/* stk[1] <- literals[1] */
		/*5*/ ["_OP_GETK", 9, 1, 2, 1, 0],		/* stk[1] <- literals[2] */
		/*6*/ ["_OP_DLOAD", 4, 2, 3, 3, 1],		/* stk[2] <- literals[3] */ /* stk[3] <- literals[1] */
		/*7*/ ["_OP_GET", 14, 3, 0, 3, 0],	/* stk_at_arg0[3] = stk_at_arg1[0].get(stk_at_arg2(3)) */
		/*8*/ ["_OP_GETK", 9, 3, 2, 3, 0],		/* stk[3] <- literals[2] */
		/*9*/ ["_OP_SET", 13, 255, 0, 2, 3],
		/*10*/ ["_OP_RETURN", 23, 255, 0, 0, 0],	/* _arg0(0xFF) != 0xFF ? stk[0] : null */
	],
	functions = [
],
	stacksize = 5,
	bgenerator = 0,
	varparams = 0,
},
],
	stacksize = 6,
	bgenerator = 0,
	varparams = 1,
}
```