#!/usr/bin/perl

while(<>) {
	chomp;

	foreach $c (split //, $_) {
		printf "%s: %d\n", $c, ord($c);
	}
	print "\n";

	print "// \"$_\" =>";
	foreach $c (split //, $_) {
		printf " %d", ord($c);
	}
	print "\n\n";
}

