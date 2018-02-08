local a = "aaa aa a aaa a".gsub("%f[%w]a", "x");
if(a == "xaa xa x xaa x") a = 1;
else a = 0;
