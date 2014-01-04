#!/bin/bash

#
#Constants
#
LOG_FILE="TCP.log"

#jump to results directory
cd ..

#remove old files
rm *.dat
rm fig/*.svg

#extract cwnd data from the log file.
grep "updated to cwnd" $LOG_FILE > data/cwnd.dat
grep "7.0.0.2" data/tcp-put.dat > data/tcp-send.dat
grep "7.0.0.2" data/udp-put.dat > data/udp-send.dat
#process MAC layer log file to extract radio frame losses
cd parser
./check_radio_error.py

######ploting######
cd ..
gnuplot plot.gnu
mv *.svg fig/



