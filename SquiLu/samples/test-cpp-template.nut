typedef any T;
typedef int32 int;
typedef array charAry;
typedef double_t double;
typedef string_t string;

template<typename T>
T add2(T first, T second)
{
	return first + second;
}

function main(int argc, array_t argv)
{
	int ia = 4, ib = 6;
	print(add2(ia, ib));

	double da = 4.0, db = 6.0;
	print(add2(da, db));
	
	string sa = "4", sb = "6";
	print(add2(sa, sb));
	
	return 0;
}
