#!/bin/env python
import os
import sys
import argparse
import time
import subprocess

qsub ="""
#!/usr/bin/env sh
#PBS -V
#PBS -j oe
#PBS -q cms
#PBS -d /wk_cms/sam7k9621/qsub/dMESSAGE
#PBS -o /wk_cms/sam7k9621/qsub/oMESSAGE
#PBS -e /wk_cms/sam7k9621/qsub/eMESSAGE
cd {}/src && eval `scramv1 runtime -sh`
{}
"""

def isQsubOk( maxRunJobs, username='sam7k9621' ) :
    s = subprocess.Popen( 'qstat | grep -c ' + username, shell=True, stdout=subprocess.PIPE ) 
    sout, serr = s.communicate()
    sout = int(sout)
    return sout <= maxRunJobs
    
    if sout <= maxRunJobs :
        return True
    else :
        return False

def isQstatOk( maxRunJobs, maxQueJobs, queue='cms'):
    # check number of running jobs instead of on queue.
    s = subprocess.Popen('qstat -q | grep '+queue+' | awk \'{printf("%d,%d",$6,$7)}\'', shell=True, stdout=subprocess.PIPE)
    sout, serr = s.communicate()
    sout = sout.split(',')
    
    try :
        if int(sout[0]) <= maxRunJobs and int(sout[1]) <= maxQueJobs:
            return True
        else:
            return False
    except ValueError:
        return False

def main(args):

    parser = argparse.ArgumentParser(
            "Submit jobs for commands"
            )

    parser.add_argument(
            '-i', '--inputFile',
            type=str
            )
    
    parser.add_argument(
            '-s', '--sleepTime',
            type=int,
            required=True
            )
    parser.add_argument(
            '-r', '--maxRunJobs',
            help='max number of run jobs',
            type=int
            )

    parser.add_argument(
            '-q', '--maxQueJobs',
            help='max number of queue jobs',
            type=int
            )

    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise
   
    print "Hostname:     {}".format( os.environ['HOSTNAME'] )
    print "Current time: {}".format( time.ctime() )
    print "Start time:   {}".format( time.ctime( time.time() + opt.sleepTime ) )
    sys.stdout.flush()
    time.sleep( opt.sleepTime ) 

    
    print "*"*80
    if opt.inputFile:
        # ntugrid5 Qjob
        with open( opt.inputFile ) as inputfile:
            lines = inputfile.readlines()
            for i, line in enumerate( lines ):
                pos, cmd = tuple( line.rstrip("\n").split(", ") )
                file =  ".{}.sh".format( cmd.replace( " ", "_" ).replace("/", "_") )
                with open( file, "w" ) as outputfile:
                    outputfile.write( qsub.format( pos, cmd ) )
                
                while( not isQstatOk( opt.maxRunJobs, opt.maxQueJobs ) ):
                    if isQsubOk( opt.maxRunJobs ) :
                        break
                    print "[{}|{}] Queue is full, resubmit in 1 minute...".format( i+1 ,len( lines ) )
                    sys.stdout.flush()
                    time.sleep(60)
                
                print "[{}|{}] Sending script {}".format( i+1, len(lines), file )
                sys.stdout.flush()
                cmd = "qsub {} -N {}".format( file, file.split(".")[1] )
                os.system(cmd)
                os.system( "rm {}".format( file ) )
    else:
        # lxplus condor job
        os.system( "condor_submit job.sub" )
        os.system( "rm job.sub" )
        os.system( "rm job.dat" )
    print "*"*80
    
    print "Finish time:  {}".format( time.ctime() )
    os.system( "rm {}".format( opt.inputFile ) )

if __name__ == '__main__':
    main(sys.argv)
