Fork of OGDF library, ogdf.v2007.11 (Bubinga) see: http://www.ogdf.net/doku.php/tech:versions#v_200711_bubinga

Using this code you can compile debug library on Windows (MinGW, GCC 4.8.1 + Msys).

Library is compilable, but no functionality is guarantee.

Compile command: 

make debug

Usage: 

copy libOGDF.a to yours lib directory, then

gcc ... -I/header_path -lOGDF 
