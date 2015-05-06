Fork of OGDF library, version ogdf.v2007.11 (Bubinga).  
To find original sources see: http://www.ogdf.net/doku.php/tech:versions#v_200711_bubinga  
Using this fork you can compile debug library on MinGW (tested on GCC 4.8.1 + Msys).  
Library is compilable, but some functions may be unavailable.  
Compile command:  
  
make debug  
  
Usage:  
copy libOGDF.a to Your lib directory, next copy ogdf directory (it is located in OGDF) to compiler headers dir.  
Complile Your programs using:  
-lOGDF 
