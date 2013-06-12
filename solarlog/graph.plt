# Examples
# http://lowrank.net/gnuplot/datafile2-e.html

set datafile separator ' '

set xrange [500:7000]
set yrange [0:0.3]
set y2range [0:15]
set ytics nomirror
set y2tics 0,5

set title "Solar power"
#set xdata time
#set timefmt "%s"
#set format x "%M"

set ylabel 'Power (W)'
set y2label 'Voltage (V)'
set xlabel 'Time (s)'
#set label 1 strftime("%T", time(0)) at screen 0.1,0.9
set grid


plot 'data.txt' using 0:4 with lines lw 1 title 'Power' axis x1y1, \
     'data.txt' using 0:2 with lines lw 1 title 'Voltage' axis x1y2

pause 60
reread
