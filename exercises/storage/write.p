set terminal png
set output "write.png"
unset key
set xrange[-30:500]
set xtics 64
set boxwidth 10
plot 'write.dat' u 1:3:2:6:5 with candlesticks

