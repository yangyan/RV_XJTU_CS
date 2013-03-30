#!/bin/bash 

ln -s ../always_something/plot/plot.dat data_as
ln -s ../always_finally/plot/plot.dat data_af
ln -s ../foreach_begin_end/plot/plot.dat data_fbe

./concat.pl

gnuplot plot_command_nodes

gnuplot plot_command_times

evince nodes.eps &
evince times.eps &

convert -density 300 nodes.{eps,png}
convert -density 300 times.{eps,png}

