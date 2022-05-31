set terminal png
set output "Dumbbell-2TCP-cwnd-ssthresh.png"
set xlabel "Time(seconds)"
set ylabel "Ventana de Congestion (cwnd)"
set yrange [0:12]
set mytics 2
set xrange [0:60]
set xtics 0,5
set mxtics 2
set title "Congestion Window Plot"

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 1 \
    pointtype 7 pointsize 1

set style line 2 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 1 \
    pointtype 3 pointsize 1.5

set style line 3 \
    linecolor rgb '#dd181f' \
    linetype 1 linewidth 1 \
    pointtype 7 pointsize 1

set style line 4 \
    linecolor rgb '#dd181f' \
    linetype 1 linewidth 1 \
    pointtype 3 pointsize 1.5


plot "SOR2-TP2-Dumbbell-n2i0-cwnd.data" using 1:2 title "cwnd-n2i0" with linespoints linestyle 1, \
"SOR2-TP2-Dumbbell-n2i0-ssthresh.data" using 1:2 title "ssthresh-n2i0" with points linestyle 2, \
"SOR2-TP2-Dumbbell-n3i0-cwnd.data" using 1:2 title "cwnd-n3i0" with linespoints linestyle 3, \
"SOR2-TP2-Dumbbell-n3i0-ssthresh.data" using 1:2 title "ssthresh-n3i0" with points linestyle 4