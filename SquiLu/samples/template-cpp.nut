typedef string_t string;
typedef int_t int;
typedef any_t T;

template<typename T>
string doIt(T x)
{
	int z = 3;
	z = (int)x;
	return x.tostring();
}
	
class Klass
{
	template<typename T>
	string doIt(T x)
	{
		return x.tostring();
	}
}

Klass k = new Klass();
print(k.doIt(10));
