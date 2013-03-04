//multiline strings can use [=*[ content ]=*] or  {=*{ content }=*} or (=*( content )=*)
local str = [==[
local str2 = {=={
Hello !
I'm a multiline string !	
}==};
]==];

print(str);

str = {=={
local str2 = [==[
Hello !
I'm a multiline string !	
]==];
}==};

print(str);

str = (==(
local str2 = [==[
Hello !
I'm a multiline string !	
]==];
)==);

print(str);