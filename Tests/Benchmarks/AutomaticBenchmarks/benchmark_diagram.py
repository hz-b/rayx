#!/usr/bin/python3 -B

import matplotlib
import matplotlib.pyplot as plt
import numpy as np


def plot(rayx_times, rayui_times, beamlines):
	"""
	Plot the Beamline time; x, for RAY-X. ui, for RAY-UI
	beamlines is also the chaning variable throughout the beamline
	"""
	assert(len(rayx_times) == len(rayui_times))

	N = len(rayx_times)

	ind = np.arange(N)

	rects_x = plt.bar(ind, rayx_times, 0.15, color='#6cabe9', label='RAY-X')
	rects_ui = plt.bar(ind + 0.15, rayui_times, 0.15, color='#d15e57', label='RAY-UI')

	plt.xlabel('Beamlines')
	plt.xticks([r + 0.15/2 for r in range(N)], beamlines)
	
	#Grid display
	plt.grid(axis='y', linewidth=.25)

	plt.ylabel('time (ms)')
	plt.title("Exectuion time in ms")
	plt.suptitle("RAY-X vs RAY-UI")
	plt.legend()
	plt.show()


if __name__ == "__main__":
	# Change accordingly 
	_rayx_times = [1, 3, 5, 3, 2]
	_rayui_times = [2, 4, 6, 3, 3]
	_beamlines = ["A", "B", "C", "D", "E"]

	plot(_rayx_times,_rayui_times,_beamlines)