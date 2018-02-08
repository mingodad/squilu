/*
*
* Original Javascript version by David Hedbor(http://www.bagley.org/~doug/shootout/)
*
*/

int32_t fib(int32_t n)
{
    if (n < 2) return 1;
    return fib(n-2) + fib(n-1);
}

int32_t n = vargv.len()>1?vargv[1].tointeger():10;

print(n, fib(n));
