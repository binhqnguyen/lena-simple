#!/bin/bash
SIM_TIME="100"
DISTANCE="500"
RESULT_DIR="results"
lte="lte-non-nsc"

if [ $# -lt 1 ]; then
		echo "Parameters: <simTime>"
		exit;
fi
SIM_TIME=$1
rm DlRxPhyStats.txt
rm DlTxPhyStats.txt
rm UlRxPhyStats.txt
rm DlTxPhyStats.txt

echo "Running simulation: time = $1, distance between UE and eNB = $2 ..."

echo "running UDP application..."
./waf --run "scratch/$lte --simTime=$SIM_TIME --isTcp=0" > $RESULT_DIR/UDP.log 2>&1 

echo "running TCP application..."
./waf --run "scratch/$lte --simTime=$SIM_TIME --isTcp=1" > $RESULT_DIR/TCP.log 2>&1 

#
#Parsing and plotting
#
cd $RESULT_DIR/parser
./parsing.sh



