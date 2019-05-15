#!/bin/env python

import os
import sys
import argparse
import importlib
import time
import subprocess
import random
import random

qsub ="""
#!/usr/bin/env sh
#PBS -V
#PBS -j oe
#PBS -q cms
#PBS -d /wk_cms/sam7k9621/qsub/dMESSAGE
#PBS -o /wk_cms/sam7k9621/qsub/oMESSAGE
#PBS -e /wk_cms/sam7k9621/qsub/eMESSAGE
cd /wk_cms2/sam7k9621/CMSSW_8_0_19/src && eval `scramv1 runtime -sh`
{}
"""

def isQsubOk( maxRunJobs, username='sam7k9621' ) :
    s = subprocess.Popen( 'qstat | grep -c ' + username, shell=True, stdout=subprocess.PIPE ) 
    sout, serr = s.communicate()
    sout = int(sout)
    if sout <= maxRunJobs :
        return True
    else :
        print "Queue is full, halt for one minute..."
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
            '-C', '--Command',
            help='Main command',
            type=str,
            required=True
            )

    parser.add_argument(
            '-T', '--Trailer',
            help='Trailer command line',
            type=str,
            )

    parser.add_argument(
            '-S', '--Samplelst',
            help='Input sample list',
            type=str
            )
    
    parser.add_argument(
            '-R', '--maxRunJobs',
            help='max number of run jobs',
            type=int,
            default=10
            )

    parser.add_argument(
            '-Q', '--maxQueJobs',
            help='max number of queue jobs',
            type=int,
            default=5
            )

    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise

    if opt.Command in ("PreCut", "FullCut") :
        subdir = "CPVAnalysis.BaseLineSelector."
    elif opt.Command in ("MakeHist") :
        subdir = "CPVAnalysis.CompareDataMC."

    samplelst = importlib.import_module( subdir + opt.Command )
    random.seed()

    # cmd = opt.Command
    if opt.Trailer :
        opt.Command += " " + opt.Trailer

    for sample in getattr( samplelst, opt.Samplelst ) :
        while( not isQstatOk( 40, opt.maxQueJobs ) ):
            if isQsubOk( opt.maxRunJobs ) :
                break
            time.sleep(60)
       
        command = "{} -s {}".format(opt.Command, sample)
        idx     = random.randint(1, 10000)
        output = open( ".sentJob{}.sh".format(idx), 'w' )
        output.write( qsub.format(command) )
        output.close()
        
        cmd = "qsub .sentJob{}.sh -N {} -l host='!node13.cluster'".format(idx, sample)
        print cmd
        print ">>Sending {}".format(sample)
        sys.stdout.flush()
        os.system(cmd)
        os.system( "rm .sentJob{}.sh".format(idx)) 

    print "DONE"

if __name__ == '__main__':
    main(sys.argv)
