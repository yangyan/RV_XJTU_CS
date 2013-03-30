#!/usr/bin/perl -w
#
open AS, "data_as";
open AF, "data_af";
open FBE, "data_fbe";

my (@as_node, @as_time);
my (@af_node, @af_time);
my (@fbe_node, @fbe_time);

while(<AS>) {
	m/^(.*) (.*) (.*)$/;
	push @as_node, $2;
	push @as_time, $3;
}

while(<AF>) {
	m/^(.*) (.*) (.*)$/;
	push @af_node, $2;
	push @af_time, $3;
}

while(<FBE>) {
	m/^(.*) (.*) (.*)$/;
	push @fbe_node, $2;
	push @fbe_time, $3;
}

my $line = @as_node < @af_node ? @as_node: @af_node;
$line = $line < @fbe_node ? $line: @fbe_node;

open NODES, ">nodes";
open TIMES, ">times";

for(0..$line-1) {
	print NODES ($_ + 1)." ".$as_node[$_]." ".$af_node[$_]." ".$fbe_node[$_]."\n";
}

for(0..$line-1) {
	print TIMES ($_ + 1)." ".$as_time[$_]." ".$af_time[$_]." ".$fbe_time[$_]."\n";
}
