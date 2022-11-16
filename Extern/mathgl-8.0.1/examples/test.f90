! gfortran test.f90 -lmgl -fdefault-real-8
program test
integer(8) :: dat, gr, mgl_create_data_size, mgl_create_graph
real(8) :: v
dat = mgl_create_data_size(30,40,1) ! data to for plotting
do i=0,29
  do j=0, 39
    v = 1/(1+(i-15)*(i-15)/9.+(j-20)*(j-20)/16.)
    call mgl_data_set_value(dat, v,i,j,0)
  end do
end do
gr = mgl_create_graph(600, 400)      ! class for plot drawing
call mgl_set_ranges(gr,0.,2.,0.,2.,0.,1.)  ! ranges of coordinates
call mgl_rotate(gr,50.,60.,0.)          ! rotate axis
call mgl_set_light(gr,1)             ! enable lighting
call mgl_surf(gr,dat,"","")          ! plot surface
call mgl_cont(gr,dat,"y","")         ! plot yellow contour lines
call mgl_axis(gr,"xyzt","","")       ! draw axis
call mgl_puts(gr,1.,1.,1.2,"\i f = \dfrac{1}{1+(5x-5)^2+(5y-5)^2}","",-1.)
call mgl_write_frame(gr,"sample.png","") ! save it
call mgl_delete_data(dat)            ! free used memory
call mgl_delete_graph(gr) 
end program

