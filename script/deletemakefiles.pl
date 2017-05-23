#/usr/bin/perl -w
use warnings;
use File::Find;
use strict;
use Cwd;

my $cwd = getcwd;
my $path = $cwd.'/../';

sub wanted {
    if ( -f $File::Find::name ) {
        if ( $File::Find::name =~ /\Makefile$/ ) {
            print "Delete File: $File::Find::name\n";
            unlink($File::Find::name) or warn "failed on $File::Find::name";
        }
    }
}

find( \&wanted, $path );
