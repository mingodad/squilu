local s5 = "done5";

local function test10(x)
{
	print("goto start");
goto start;
change:
	x = 5;
	try
	{
		print("change");
	}
	catch(e)
	{
	}
//goto dad;
start:
	print("start");
	if(x > 10)
	{
		try
		{
			goto p2;
			print("change");
			p2:
			print("change p2");
		}
		catch(e)
		{
		}
		while(true)
		{
			try
			{
				try
				{
				print("x > 10");
				//return "x > 10";
				goto done2;
				goto done5;
				goto change;
				goto start;
				goto done4;
				goto done3;
				goto done;
				return true;
				}
				catch(e)
				{
				}
				goto done4;
			}
			catch(e)
			{
			}
			break;
		}
	}
	local z = 0;
	while(z < 3)
	{
		++z;
		try
		{
			print("while");
			try
			{
				print("while2");
				continue;
				goto done4;
				goto done3;
				goto done;
			}
			catch(e)
			{
			}
		}
		catch(e)
		{
		}
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
print("ok");