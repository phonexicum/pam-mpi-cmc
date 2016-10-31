#!/usr/bin/perl

#!/usr/bin/perl -I /etc/LoadL

###############################################################
# mpisubmit.bg -- script for submitting simple mpi jobs to LL #
#                via ' llsubmit - '                           #
###############################################################

use strict;
use Getopt::Long;

my $submit_file_name = "| /opt/ibmll/LoadL/full/bin/llsubmit - "; # file name or filter to stream commands out

my $user = `id -nu`;
chomp ($user);

# job file parameters
my %cmd =(
        'nodes' => '32', # nubmer of nodes
        'mode' => 'smp', # default mode
        'wtime' => '00:15:00', #wall time limit
        'class' => 'test_class',
                'env' => [],
        'top' => 'PREFER_TORUS', # topology
        'group' => '', # group

        'in' => '', # file stdin flows from
        'out' => '$(jobid).out', # file stdout flows to
        'err' => '$(jobid).err', # file stderr flows to

                'debug' => 0, # dump to stdout

        'efile' => '', #file to execute
        'args' => '', #its argument
    );              


# parse command line
sub printHelp;
my $was_help = 0;

GetOptions (
        'nproc=i' => \$cmd{"nodes"},
        'mode=s' => \$cmd{"mode"},
        'wtime=s' => \$cmd{"wtime"},
        'class=s' => \$cmd{"class"},
        'top=s' => \$cmd{"top"}, 
        'group=s' => \$cmd{"group"}, 

        'stdin=s' => \$cmd{"in"},
        'stdout=s' => \$cmd{"out"},
        'stderr=s' => \$cmd{"err"},
                'debug' => \$cmd{"debug"},
        'help' => sub {$was_help = 1;},
        'env=s', $cmd{"env"}, 
    );

$was_help && printHelp;

my @tags;
($cmd{"efile"}, @tags) = @ARGV;
$cmd{"args"} =  join ( ' ', @tags); 
 
( $cmd{"efile"} ) || ( warn ("error: you should specify executable\n") && printHelp);

# name stout and stderr files
my $efile_name = (split /\//, $cmd{"efile"} ) [-1];
$cmd{"out"} = "$efile_name." . $cmd{"out"}
    if ( $cmd{"out"} eq '$(jobid).out' );
$cmd{"err"} = "$efile_name." . $cmd{"err"}
    if ( $cmd{"err"} eq '$(jobid).err' );


# if everything is fine -- submitting...

my $env = '';
if ( @{ $cmd{'env'} } ) {
    my @envvals = @{ $cmd{'env'} };
    $env = join ' -env ', @envvals;
}

if ( $cmd{'debug'} ) {
    $submit_file_name = "| cat ";
}

open (SUBMIT_STREAM, $submit_file_name);
select SUBMIT_STREAM;

print "# this file was automaticly created by mpisubmit.bg script for $user #\n";
print "#@ job_type = bluegene\n";

my $class = 'test_class';

print "#@ bg_size = ", $cmd{"nodes"}, "\n";
print "#@ wall_clock_limit = ", $cmd{"wtime"}, "\n";
print "#@ class = ", $cmd{"class"}, "\n";
print "#@ bg_connection = ", $cmd{"top"}, "\n";
print "#@ group = ", $cmd{"group"}, "\n" if $cmd{"group"};
print "#@ bg_rotate = true\n";

print "#@ input = ", $cmd{"in"}, "\n" if $cmd{'in'};
print "#@ output = ", $cmd{"out"}, "\n";
print "#@ error = ", $cmd{"err"}, "\n";

print "#@ notification = never\n";
print "#@ queue\n";

print "/bgsys/drivers/ppcfloor/bin/mpirun";
print " -env CMC_SITE_SPECIFIC=" . $cmd{'class'};
print " -np " . $cmd{'nodes'};
print " -mode " . $cmd{'mode'};
print " -env " . $env if ( $env );
print " -exe " . $cmd{'efile'};
print " -args '" . $cmd{'args'} . "'" if $cmd{'args'};;
print "\n";

close (SUBMIT_STREAM);


# here's the usage information printing sub
sub printHelp
{
    warn ("usage: mpisubmit.bg {<option_value_pair>} <executable to submit> -- <args>\n");
    warn (" where <option_value_pair> could be:\n");
    warn ("\t( -n | --nproc ) <number of nodes to run at>,\n\t\tdefault is 32\n");
    warn ("\t( -m | --mode ) <mode to be used: smp | dual | vn>,\n\t\tdefault is smp\n");
    warn ("\t( -w | --wtime ) <wall clock limit>,\n\t\tdefault is 00:15:00\n");
    warn ("\t( -e | --env ) <environment to be passed >\n\t\t\"env=val env=val ...\"\n");
    warn ("\t( -t | --top ) <topology to be used: TORUS | MESH | PREFER_TORUS>,\n\t\tdefault is PREFER_TORUS\"\n");
    warn ("\t( -c | --class ) class to be used,\n\t\tdefault is \"short_class\"\n");

    warn ("\t --stdout <file to direct stdout to>,\n\t\tdefault is '<exec>.\$(jobid).out'\n");
    warn ("\t --stderr <file to direct sterr to>,\n\t\tdefault is '<exec>.\$(jobid).err'\n");
    warn ("\t --stdin <file to direct stdin from>,\n\t\tno default\n");

    warn ("\t( -d | --debug ) prints the jcf file to stdout instead of submiting to LoadLeveler\n");

    warn ("\t( -h | --help ) prints this message out\n");

    warn ("\ni.e. mpisubmit.bg -w 00:15:00 -n 32 -c\n");
    die ("or mpisubmit.bg -w 00:15:00 -n 32 -m smp -t MESH -e \"OMP_NUM_THREADS=4 A=123\" a.out -- 0.01\n");
}


