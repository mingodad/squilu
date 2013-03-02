SET SRC_SCRIPT=%1
SET SQUILU=C:\D\squilu\squilu\SquiLu\bin\squilu
%SQUILU%.exe unify-code.nut %SRC_SCRIPT%.nut %SRC_SCRIPT%-unified.nut
%SQUILU%.exe squilufreeze.nut %SQUILU%-fltk-w.exe %SRC_SCRIPT%-unified.nut %SRC_SCRIPT%.exe

