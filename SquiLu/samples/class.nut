//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class BaseVector {
	constructor(...)
	{
		if(vargv.len() >= 3) {
			x = vargv[0];
			y = vargv[1];
			z = vargv[2];
		}
	}
	
	
	x = 0;
	y = 0;
	z = 0;
}

class Vector3 extends BaseVector {
	function _add(other)
	{
		if(other instanceof this.getclass())
			return ::Vector3(x+other.x,y+other.y,z+other.z);
		else
			throw "wrong parameter";
	}
	function Print()
	{
		::print(x+","+y+","+z+"\n");
	}
}

local v0 = Vector3(1,2,3)
local v1 = Vector3(11,12,13)
local v2 = v0 + v1;
v2.Print();

FakeNamespace <- {
	Utils = {}
}

class FakeNamespace.Utils.SuperClass {
	constructor()
	{
		::print("FakeNamespace.Utils.SuperClass")
	}
}

class FakeNamespace::Utils::SuperClass2 {
	constructor()
	{
		::print("FakeNamespace::Utils::SuperClass2")
	}
}

local testy = FakeNamespace.Utils.SuperClass();

local testy2 = FakeNamespace::Utils::SuperClass2();

class Base_Window /*extends Fl_Window*/ {
}

local mw = new Base_Window();

class MainWindow extends Base_Window {
}

mw = new MainWindow();

class MyMainWindow extends MainWindow {
}

mw = new MyMainWindow();