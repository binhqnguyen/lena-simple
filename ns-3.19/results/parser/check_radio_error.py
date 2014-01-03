#!/usr/bin/python

import os
import sys
import FileHandle

#
#Get radio frame error using Phy layer log file
#Result is written into radio_error_rx.dat
#
if __name__ == "__main__":
	CUMMULATIVE_INTERVAL = 0.01
	##########Error received packets (from DlRxPhyStats.txt) #############
	INPUT_FILE =  "../../DlRxPhyStats.txt" 
	OUTPUT_FILE = "../data/radio_error_rx.dat"

	file = open (INPUT_FILE)
	line = file.readline()
	tokens = {}

	if (os.path.isfile(OUTPUT_FILE)):      ##if output file not exist
	    open(OUTPUT_FILE,'w').close()
	outfile = open (OUTPUT_FILE,'w+')

	total_frames = 0
	error_frames = 0.0

	while (line):
		tokens = line.split()
		total_frames += 1
		if (tokens[10] == '0'):
			outfile.write(str ( (float (tokens[0]))/1000 )+"\n")
			error_frames += 1
		line = file.readline()
	outfile.write("#frame error rate: " + str(error_frames*100/total_frames)+"\n")



