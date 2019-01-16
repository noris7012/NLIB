del "*.java"
del "*.cxx"

swig -c++ -java -package com.awesomepiece.pl.swig NLIB.i

pause