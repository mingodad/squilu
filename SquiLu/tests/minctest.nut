//
// MINCTEST - Minimal SquiLu Test Library
// based on MINCTEST - Minimal Lua Test Library - 0.1.1
// This is based on minctest.h (https://codeplea.com/minctest)
//
// Copyright (c) 2014, 2015, 2016 Lewis Van Winkle
// Copyright (c) 2017 Domingo Alvarez Duarte
//
// http://CodePlea.com
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.


// MINCTEST - Minimal testing library for C
//
//
// Example:
//
//
// #include "minctest.nut"
// local l = minctest();
//
// l.run("test1", function(){
//    l.ok('a' == 'a');          //assert true
// });
//
// l.run("test2", function(){
//    l.equal(5, 6);             //compare integers
//    l.fequal(5.5, 5.6);        //compare floats
// });
//
// return l.results();           //show results
//
//
// Hints:
//      All functions/variables start with the letter 'l'.
//
//

function minctest()
{
	local self = {};
	const LTEST_FLOAT_TOLERANCE = 1e-12; //1e-6; 0.001;


	local ltests = 0;
	local lfails = 0;
	local start_clock = os.clock();


	self.results <- function()
	{
	    local total_time = math.floor((os.clock() - start_clock) * 1000) + "ms";
	    if (lfails == 0)
		print("ALL TESTS PASSED (" + ltests + "/" + ltests + ") " + total_time);
	    else
		print("SOME TESTS FAILED (" + (ltests-lfails) + "/" + ltests + ") " + total_time);

	    return lfails != 0;
	}


	self.run <- function(name, testfunc)
	{
	    local ts = ltests;
	    local fs = lfails;
	    local lclock = os.clock();
	    local msg = format("\t%-24s", name);
	    stdout.write(msg);
	    testfunc();
	    msg = format("pass: %2d   fail: %2d   %4dms\n",
		(ltests-ts)-(lfails-fs), lfails-fs,
		math.floor((os.clock() - lclock) * 1000));
            if(lfails) stdout.write("\n");
	    stdout.write(msg);
	}

	self.ok <- function(test)
	{
	    ++ltests;
	    if ( ! test )
	    {
		++lfails;
		local stack_info = getstackinfos(2);
		stdout.write(format("\n%s:%d:0 error",
		    stack_info.src,
		    stack_info.line));
	    }
	}

	self.equal <- function(a, b)
	{
	    ++ltests;
	    if (a != b)
	    {
		++lfails;
		local stack_info = getstackinfos(2);
		stdout.write(format("%s:%d (%d != %d)\n",
		    stack_info.src,
		    stack_info.line,
		    a, b));
	    }
	}

	self.fequal <- function(a, b)
	{
	    ++ltests;
	    if (math.fabs(a - b) > LTEST_FLOAT_TOLERANCE)
	    {
		++lfails;
		local stack_info = getstackinfos(2);
		stdout.write(format("%s:%d (%f != %f)\n",
		    stack_info.src,
		    stack_info.line,
		    a, b));
	    }
	}

	self.fails <- function() {return lfails;}

	return self;
}

