set terminal png
set output "GraficoZOOM1820sec.png"
set xlabel "Time(seconds)"
set ylabel "Bytes"
set title "Congestion Window Plot"
set yrange [2500:3000]
set xrange [18:20]
plot "SOR2-TP2-Dumbbell-n2i0-cwnd.data" using 1:2 title "cwnd-n2i0"with lines, \
"SOR2-TP2-Dumbbell-n2i0-ssthresh.data" using 1:2 title "ssthresh-n2i0" with lines, \
