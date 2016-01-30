local input = [==[
    <TITlE>Test Document</TITLe>
    <h1>Test Heading</h1>
    <p><a href=foobar.html>Quux</a></p>
    <iNValID foo="bar">abc</invalid>
    <p class=empty></p>
]==];

//local document = assert(gumbo.parse(input))
local document = gumbo.parse(input);
foreach(k,v in document) print(k,v);
//local root = assert(document.root)
print(document.root.len());
local root = document.root
local head = root.children[0];
local body = root.children[1];

assert(root.tag == "html")
assert(head.tag == "head")
assert(body.tag == "body")
print("head.children[0].tag", head.children[0].tag);
assert(head.children[0].tag == "title")
assert(head.children[0].children[0] == "Test Document")
assert(body.children[0].tag == "h1")
assert(body.children[0].children[0] == "Test Heading")
assert(body.children[1].tag == "p")
assert(body.children[1].children[0].attr.href == "foobar.html")
assert(body.children[2].tag == "iNValID")
assert(body.children[2].attr.foo == "bar")
assert(body.children[2].children[0] == "abc")
assert(body.children[3].children.len() == 0)
assert(body.children[3].attr["class"] == "empty")

assert(gumbo.parse("<h1>Hello</h1>").root.children[1].children[0].children[0] == "Hello")
//assert(! gumbo.parse_file( "non-existent-file"))

document = gumbo.parse_file( "/home/mingo/Downloads/Components · Bootstrap-3.0.html");
foreach(k,v in document) print(k,v);
root = document.root
head = root.children[0];
foreach(k,v in head.children) print(k,v);
foreach(k,v in head) print(k,v);
body = root.children[1];
foreach(k,v in body) print(k,v);
foreach(k,v in body.children) print(k,v, v ? v.rawget("tag", "?") : "??");
