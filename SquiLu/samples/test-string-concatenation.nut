local count = 100000;


local start = os.clock();
for(var k=0; k < count; ++k){
	var text = "Hello";
	text += " "
	text += "World!";
	text += "Hello";
	text += " "
	text += "World!";
	text += "Hello";
	text += " "
	text += "World!";
	text += "Hello";
	text += " "
	text += "World!";
	text += "Hello";
	text += " "
	text += "World!";
	text += "Hello";
	text += " "
	text += "World!";
	text += "Hello";
	text += " "
	text += "World!";
	text += "Hello";
	text += " "
	text += "World!";
}
print("String concatenation with + took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	var text = "Hello" + " " + "World!" + "Hello" + " " + "World!" + "Hello" + " " + "World!" + "Hello" +
		" " + "World!" + "Hello" + " " + "World!" + "Hello" + " " + "World!" + "Hello" + " " + "World!" +
		"Hello" + " " + "World!";
}
print("String concatenation with + took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	var buffer = [];
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");

	var text = buffer.concat();
}
print("String concatenation with array.concat1 took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	var buffer = array(24);
	buffer.clear();
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");
	buffer.push("Hello");
	buffer.push(" ");
	buffer.push("World!");

	var text = buffer.concat();
}
print("String concatenation with array.concat2 took:", os.clock()-start);


var buffer3 = array(24);
print(type(buffer3))
var push = buffer3.push.bindenv(buffer3);
print(type(push), type(push.getenv()), type(push.getenv().ref()))

{
	buffer3.clear();
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");

	print(buffer3.concat());

}

start = os.clock();
for(var k=0; k < count; ++k){
	buffer3.clear();
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");
	push("Hello");
	push(" ");
	push("World!");

	var text = buffer3.concat();
}
print("String concatenation with array.concat3 took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	var buffer = array(24), i = 0;
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";
	buffer[i++] = "Hello";
	buffer[i++] = " ";
	buffer[i++] = "World!";

	var text = buffer.concat();
}
print("String concatenation with array2.concat took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	var buffer = array(24), i = -1;
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";
	buffer[++i] = "Hello";
	buffer[++i] = " ";
	buffer[++i] = "World!";

	var text = buffer.concat();
}
print("String concatenation with array2.concat took:", os.clock()-start);
