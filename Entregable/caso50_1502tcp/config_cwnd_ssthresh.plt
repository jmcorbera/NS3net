set terminal png
set output "Graficosolonodo2.png"
set xlabel "Time(seconds)"
set ylabel "Bytes"
set title "Congestion Window Plot"
set yrange [0000:16000]
set xrange [0:60]
plot "SOR2-TP2-Dumbbell-n2i0-cwnd.data" using 1:2 title "cwnd-n2i0"with lines, \
"SOR2-TP2-Dumbbell-n2i0-ssthresh.data" using 1:2 title "ssthresh-n2i0" with lines, \
