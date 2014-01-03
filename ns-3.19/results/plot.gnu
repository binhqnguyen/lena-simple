
##======CONST=======
TCP="NewReno"
x1 = 0
x2 = 100
SEGSIZE=1448
PKTSIZE=1500


#======== rate ========
reset
set title "LTE Rate"
set key inside bottom right box
set xlabel "Time (s)"
set xtic 10
set xrange [x1:x2]
set x2range [x1:x2]
set ylabel "Rate (kbps)"
set output "rate.svg"
set terminal svg


plot "data/tcp-send.dat" using 1:3 title "TCP throughput (Rx rate)" with lines

#===========================================
reset
set title TCP." Congestion window and end-to-end delay"
set key inside top right box
set xlabel "Time (s)"
set ylabel "value (packets)"
set y2label "delay (ms)"
set output "cwnd-delay.svg"
set xrange [x1:x2]
set x2range [x1:x2]
set y2tics nomirror tc lt 2
set terminal svg


plot "data/cwnd.dat" using 3:12 title "cwnd" with lines,\
"data/tcp-send.dat" using 1:12 title "TCP delay" with lines axis x2y2




