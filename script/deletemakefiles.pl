#/usr/bin/perl -w
use warnings;
use File::Find;
use strict;
use File::Basename;
use FindBin qw($Bin);

my $cwd = $Bin;
my $path = $cwd.'/../';
print "find path: $path\n";

sub wanted_print {
	if ( -d $File::Find::dir ){
		#print "Find Dir: $File::Find::dir\n";
	}
    if ( -f $File::Find::name ) {
		#print "Find File: $File::Find::name\n";
    }
}

sub wanted {
	if ( -d $File::Find::dir ){
		if ( $File::Find::dir =~ /\.git/ ){
			#print "Delete Dir: $File::Find::dir\n";
			return;
		}
	}
    if ( -f $File::Find::name ) {
        if ( $File::Find::name =~ /Makefile/ ) {
            print "Delete File: $File::Find::name\n";
            unlink($File::Find::name) or warn "failed on $File::Find::name";
        }
    }
}

find( \&wanted, $path );
