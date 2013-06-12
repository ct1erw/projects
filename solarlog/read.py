#! /usr/bin/python

import serial
import sys
import time

ser = serial.Serial('/dev/ttyACM0', 9600)

# calibration factors
calv = 21.229
cali = 24409.069

while 1:
	# read and split the values
	line = ser.readline()
	list = line.split(',')
	t = time.time()

	# calculate power
	count = list[0]
	v = float(list[1]) / calv
	i = float(list[2]) / cali
	w = v * i

	# display
	print t, v, i, w
	sys.stdout.flush()
