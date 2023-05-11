#!/usr/bin/python3 -B

import matplotlib
import matplotlib.pyplot as plt
import numpy as np


def plot(rayx_times, rayui_times, beamlines):
	"""
	Plot the Beamline time; x, for rayx. ui, for RAY-UI
	beamlines is also the chaning variable throughout the beamline
	"""
	assert(len(rayx_times) == len(rayui_times))

	N = len(rayx_times)

	ind = np.arange(N)

	#HZB Color Scheme
	rects_x = plt.bar(ind, rayx_times, 0.15, color='#C6D970', label='rayx')
	rects_ui = plt.bar(ind + 0.15, rayui_times, 0.15, color='#9AC6F3', label='RAY-UI')

	plt.xlabel('Beamlines')
	plt.xticks([r + 0.15/2 for r in range(N)], beamlines)
	
	#Grid display
	plt.grid(axis='y', linewidth=.25)

	plt.ylabel('time (ms) (logscale)')
	plt.title("Exectuion time in ms")
	plt.yscale('log')
	plt.suptitle("rayx vs RAY-UI")
	plt.legend()
	plt.show()


if __name__ == "__main__":
	# Change accordingly 
	_rayx_times = [1676, 3329, 6477, 12467, 15941, 18653]
	_rayui_times = [46864, 86773, 171588, 323714 , 485854, 807656]
	_beamlines = ["250T","500T","1M", "2M", "3M", "5M"]


	plot(_rayx_times,_rayui_times,_beamlines)