del "*.java"
del "*.cxx"

swig -c++ -java -package com.awesomepiece.pl.swig NLIB.i

del "E:\GitRepository\popoland\Server\src\main\java\com\awesomepiece\pl\swig\*.java"
move "*.java" "E:\GitRepository\popoland\Server\src\main\java\com\awesomepiece\pl\swig\"

pause