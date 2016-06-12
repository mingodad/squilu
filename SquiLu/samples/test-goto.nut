local function test10(x)
{
	print("goto start");
goto start;
change:
	x = 5;
	print("change");
start:
	print("start");
	if(x > 10)
	{
		print("x > 10");
		goto done4;
		goto change;
		goto start;
		return true;
	}
done:
	return false;
done2:
	return "done2";
done3:
	return "done3";
done4:
	return "done4";
done5:
	return "done5";
}


print(test10(23));