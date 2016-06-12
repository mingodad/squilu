local function test10(x)
{
dente:
	if(x > 10)
	{
		goto dente;
		goto done;
		return true;
	}
done:
	return false;
}

print(test10(23));