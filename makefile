target: mandelbrot mandelCalc mandelDisplay	
mandelbrot: mandelbrot_zli48.cpp
	g++ mandelbrot_zli48.cpp -o mandelbrot
mandelCalc: mandelCalc_zli48.cpp
	g++ mandelCalc_zli48.cpp -o mandelCalc
mandelDisplay: mandelDisplay_zli48.cpp
	g++ mandelDisplay_zli48.cpp -o mandelDisplay
clean:
	rm -rf *o mandelbrot mandelCalc mandelDisplay
