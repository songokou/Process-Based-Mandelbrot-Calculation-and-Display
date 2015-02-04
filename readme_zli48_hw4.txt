This is the readme file for homework 4, CS 361, the mandelbrot programs.

This homework is done by Ze Li, zli48, 676755673.

In order to compile, you just need to type

$ make

from your command line.

Then you will be able to run the program by 
$ ./mandelbrot 

Then the program will be straightforward. You first need to type in a filename where 
child2 will be using for writing the file.

After that, you will have to give values for xMin, xMax, yMin, yMax, nRows, nCols and maxIters. An error checking on the value of xMin,xMax is [-2.0,2.0] and xMin is no bigger
 than xMax. yMin, yMax are within [-1.5,1.5] and yMin is no bigger than yMax. nRows, nCols and maxIters are positive values. 
If undesired values are entered, the program will keep looping, asking for the desired value till it’s entered.

No optional enhancement is done for this program.