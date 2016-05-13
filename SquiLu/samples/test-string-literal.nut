local txt;

txt = "A literal string";
print(txt);

txt = @"A literal string";
print(txt);

txt = [==[A literal string]==];
print(txt);

txt = {=={A literal string}==};
print(txt);

txt = (==(A literal string)==);
print(txt);

txt = R"dad(A literal string)dad";
print(txt);

txt = R"dad(
A literal string
)dad";
print(txt);

txt = R"(A literal string)";
print(txt);
