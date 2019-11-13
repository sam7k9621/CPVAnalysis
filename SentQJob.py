#!/bin/env python

import os
import sys
import argparse
import importlib
import time
import subprocess
import random
import random

def isQsubOk( maxRunJobs, username='sam7k9621' ) :
    s = subprocess.Popen( 'qstat | grep -c ' + username, shell=True, stdout=subprocess.PIPE ) 
    sout, serr = s.communicate()
    sout = int(sout)
    if sout <= maxRunJobs :
        return True
    else :
        print "Queue is full, halt for 5 minutes..."
        sys.stdout.flush()
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
            type=str,
            nargs='+',
            required=True
            )
    
    parser.add_argument(
            '-r', '--maxRunJobs',
            help='max number of run jobs',
            type=int,
            required=True
            )

    parser.add_argument(
            '-q', '--maxQueJobs',
            help='max number of queue jobs',
            type=int,
            required=True
            )

    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise
    
    for file in opt.inputFile:
        while( not isQstatOk( opt.maxRunJobs, opt.maxQueJobs ) ):
            if isQsubOk( opt.maxRunJobs ) :
                break
            time.sleep(300)
        
        sample = file.split("-s")[-1].replace(".sh", "")
        cmd = "qsub {} -N {}".format( file, sample)
        print ">>Sending {}".format(sample)
        sys.stdout.flush()
        os.system(cmd)
        os.system( "rm {}".format( file ) )
    print "DONE"

if __name__ == '__main__':
    main(sys.argv)
