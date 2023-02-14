#!/usr/bin/perl -w
#
# Script to fix Subversion dump files broken by svndumpfilter filtering content of an already empty revision.
#
$state = 0;
binmode STDIN;
binmode STDOUT;
while(<>) {
	if ($state == 0 and /^Prop-content-length: 10$/) {
		$state = 1;
	} elsif ($state == 1 and /^Content-length: 10$/) {
		$state = 2;
	} elsif ($state == 2 and /^$/) {
		$state = 3;
	} elsif ($state == 3 and /^$/) {
		print "PROPS-END\n";
		$state = 0;
	} else {
		$state = 0;
	}
	print;
}
