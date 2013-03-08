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

var buffer = array(24);

start = os.clock();
for(var k=0; k < count; ++k){
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


print(type(buffer))
var push = buffer.push.bindenv(buffer);
print(type(push), type(push.getenv()), type(push.getenv().ref()))

{
	buffer.clear();
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

	print(buffer.concat());

}

start = os.clock();
for(var k=0; k < count; ++k){
	buffer.clear();
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

	var text = buffer.concat();
}
print("String concatenation with array.concat3 took:", os.clock()-start);

buffer.resize(24);

start = os.clock();
for(var k=0; k < count; ++k){
	var i = 0;
	buffer.clear();
	buffer.resize(24);
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
	var i = -1;
	buffer.clear();
	buffer.resize(24);
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
print("String concatenation with array3.concat took:", os.clock()-start);

var ary_set = buffer.set.bindenv(buffer);

start = os.clock();
for(var k=0; k < count; ++k){
	var i = -1;
	buffer.clear();
	buffer.resize(24);
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");
	ary_set(++i, "Hello");
	ary_set(++i,  " ");
	ary_set(++i,  "World!");

	var text = buffer.concat();
}
print("String concatenation with array4.concat took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	var i = -1;
	buffer.clear();
	buffer.resize(24);
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");
	buffer.set(++i, "Hello");
	buffer.set(++i,  " ");
	buffer.set(++i,  "World!");

	var text = buffer.concat();
}
print("String concatenation with array5.concat took:", os.clock()-start);
