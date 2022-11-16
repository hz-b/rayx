// gcc test.c -lmgl
#include <mgl2/mgl_cf.h>
int main()
{
	HMDT dat = mgl_create_data_size(30,40,1);	// data to for plotting
	for(long i=0;i<30;i++)   for(long j=0;j<40;j++)
	   	mgl_data_set_value(dat, 1/(1+(i-15)*(i-15)/9.+(j-20)*(j-20)/16.),i,j,0);
	HMGL gr = mgl_create_graph(600, 400);	// class for plot drawing
	mgl_set_ranges(gr,0,2,0,2,0,1);			// ranges of coordinates
	mgl_rotate(gr,50,60,0);					// rotate axis
	mgl_set_light(gr,1);					// enable lighting
	mgl_surf(gr,dat,"","");					// plot surface
	mgl_cont(gr,dat,"y","");				// plot yellow contour lines
	mgl_axis(gr,"xyzt","","");				// draw axis
	mgl_puts(gr,1,1,1.2,"\\i f = \\dfrac{1}{1+(5x-5)^2+(5y-5)^2}","",-1);
	mgl_write_frame(gr,"sample.png","");	// save it
	mgl_delete_data(dat);					// free used memory
	mgl_delete_graph(gr);
}
