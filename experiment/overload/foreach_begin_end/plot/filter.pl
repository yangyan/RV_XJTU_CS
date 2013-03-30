#!/usr/bin/perl -w

my $key_node = "总结点数:";
my $key_time = "推理时间:";
my $key_auto = "当前自动机为：";#最后为中文:

my @nta;
while(<STDIN>) {
	if(m/^($key_node|$key_time|$key_auto)/){
		push @nta, $_;
	}
}

my %auto_data;
my $index = 0;
my (@cell, $node, $auto, $time);
while($index <= $#nta) {
	if($nta[$index] =~ m/^$key_time(.*)ns$/) {
		$time = $1;
		if($nta[$index-1] =~ m/^$key_node(.*)$/) {
			$node = $1;
			if($nta[$index-2] =~ m/^$key_auto(.*)$/) {
				$auto = $1;
				@cell = [$node, $time];
				push @{$auto_data{$auto}}, @cell;
			}
		}
	}
	$index++;
}

#下面，对每个统计到的自动机，进行取平均操作
my $max_len = 0;
while((my $key, my $value) = each(%auto_data)) {
	my @tmp_val = @{$value};
	if($max_len < $#tmp_val+1){
		$max_len = $#tmp_val+1;
	}
}

my %avg_node_array;
my %avg_time_array;
while((my $key, my $value) = each(%auto_data)) {
	my $index = 0;
	for $i (@{$value}) {
		$index++;
		push @{$avg_time_array{$index}}, $i->[1]/1000000;
		push @{$avg_node_array{$index}}, $i->[0];
	}
}

my %avg_node;
my %avg_time;

while((my $key, my $value) = each(%avg_node_array)){
	my @array = @{$value};
	my $sum = 0;
	foreach(@array){
		$sum += $_;
	}
	$avg_node{$key} = $sum/($#array+1);
}

while((my $key, my $value) = each(%avg_time_array)){
	my @array = @{$value};
	my $sum = 0;
	foreach(@array){
		$sum += $_;
	}
	$avg_time{$key} = $sum/($#array+1);
}

my $total_node = 0;
my $total_time = 0;
for(my $i = 1; $i <= $max_len; $i++) {
	$total_node += $avg_node{$i};
	$total_time += $avg_time{$i};
	print $i." ".$total_node." ".$total_time."\n";
}
