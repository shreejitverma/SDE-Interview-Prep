import os
import numpy as np
import matplotlib.pyplot as plt

class Plot(object):
	def __init__(self, file_name):
		self.file_name = file_name
		self.lines = []

	def read_file(self):
		try:
			if self.file_exists():
				with open(self.file_name) as file:
					self.lines = file.readlines()
		except Exception as e:
			print e

	def file_exists(self):
		if os.path.exists(self.file_name):
			return True
		return False

	def clean_lines(self):
		self.lines = self.lines[2:-4]
		
	def plot(self):
		x = [-10]
		y = [-10]
		# Plot an n^2 line to contrast it with nlogn line

		# Plot the data
		for line in self.lines:
			# size
			x.append(int(line.strip().split(',')[0]))
			# time in ms
			y.append(float(line.strip().split(',')[1]))

		# evenly sampled time at 200ms intervals
		t = np.arange(500., 80000000., 10000)
		q = np.arange(500., 80000000., 10000)
		s = np.arange(500., 80000000., 10000)
		# red dashes, blue squares and green triangles
		plt.plot(x, y, 'bs', t, t**2, 'r--', t, t, 'r--')
		#plt.plot(x, y, "ro")
		#plt.axis([-2, 90000000, -2, 60000])
		plt.show()

if __name__ == '__main__':
	print "------------------------------------"
	file_name = raw_input("Enter the file name: ")
	print "------------------------------------"
	print ""
	plot_now = Plot(file_name)
	plot_now.read_file()
	plot_now.clean_lines()
	print "------------------------------------"
	print
	plot_now.plot()
	print
	print "------------------------------------"