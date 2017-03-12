set terminal png
set output "write_no_o_dsync.png"
unset key
set xrange[-30:500]
set xtics 64
set boxwidth 10
plot 'write_no_o_dsync.dat' u 1:3:2:6:5 with candlesticks

