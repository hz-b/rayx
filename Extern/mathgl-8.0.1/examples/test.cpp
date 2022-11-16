// g++ test.cpp -lmgl
#include <mgl2/mgl.h>
int main()
{
	mglData dat(30,40);	// data to for plotting
	for(long i=0;i<30;i++)   for(long j=0;j<40;j++)
	   	dat.a[i+30*j] = 1/(1+(i-15)*(i-15)/9.+(j-20)*(j-20)/16.);
	mglGraph gr;		// class for plot drawing
	gr.SetRanges(0,2,0,2,0,1);	// ranges of coordinates
	gr.Rotate(50,60);	// rotate axis
	gr.Light(true);		// enable lighting
	gr.Surf(dat);		// plot surface
	gr.Cont(dat,"y");	// plot yellow contour lines
	gr.Axis();			// draw axis
	gr.Puts(mglPoint(1,1,1.2),"\\i f = \\dfrac{1}{1+(5x-5)^2+(5y-5)^2}");
	gr.WriteFrame("sample.png");	// save it
}
