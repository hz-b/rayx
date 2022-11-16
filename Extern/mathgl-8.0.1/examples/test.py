# python test.py
from mathgl import *
dat = mglData()	# data to for plotting
dat.Create(30,40)
for i in range(0,30):
  for j in range(0,40):
    dat.SetVal(1/(1+(i-15)*(i-15)/9.+(j-20)*(j-20)/16.),i,j)
gr = mglGraph()     # class for plot drawing
gr.SetRanges(0,2,0,2,0,1)	# ranges of coordinates
gr.Rotate(50,60)	# rotate axis
gr.Light(True)		# enable lighting
gr.Surf(dat)		# plot surface
gr.Cont(dat,"y")	# plot yellow contour lines
gr.Axis()			# draw axis
gr.Puts(mglPoint(1,1,1.2),"\i f = \dfrac{1}{1+(5x-5)^2+(5y-5)^2}")
gr.WriteFrame("sample.png")	# save it
