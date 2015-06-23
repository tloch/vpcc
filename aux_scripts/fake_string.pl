#!/usr/bin/perl

use warnings;
use strict;
use POSIX qw(isprint);

my $varname = shift;
my $string = shift;

die("usage: $0 <varname> <string>\n") unless defined $string;


print "int *$varname;\n";
print "int *c_$varname;\n";

print "\n";
print "$varname = malloc(" . (length($string) + 1) . " * 4);\n";
print "c_$varname = $varname;\n";

print "\n";
print "// string: \"$string\"\n";
$string = eval " \"$string\" ";
foreach(split //, $string) {
	print "*c_$varname  = " . ord($_) . ";" . (
		isprint($_) ? " // '$_'" : ord($_) == 10 ? " // '\\n'" : "" )  .
		"\n";
	print "c_$varname = c_$varname + 1;\n";
}

print "*c_$varname  = 0; // end of string\n";

