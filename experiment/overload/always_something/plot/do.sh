#!/bin/bash 

./filter.pl 2>/dev/null <raw_data >plot.dat
gnuplot plot_command
evince pic.eps
