#!/usr/bin/perl
#
# Message filter for git filter-branch to move revision number to start of commit message
#
$msg="";
while(<>) {
	if (/^git-svn-id: .*?@([0-9]+) [0-9a-f-]+$/) {
		$msg = "[r" . $1 . "] " . $msg if $msg !~ /^\[r[0-9]+\] /;
	} else {
		$msg.=$_;
	}
}
print $msg;
