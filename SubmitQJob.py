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
cd {}/src && eval `scramv1 runtime -sh`
{}
"""

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
            '-Y', '--Year',
            help='Which year',
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
            nargs='+',
            type=str
            )
    
    parser.add_argument(
            '-I', '--InputOption',
            help='Input argument',
            type=str,
            default=""
            )

    parser.add_argument(
            '-E', '--extract',
            help='extract specific sample',
            type=str,
            default=""
            )

    parser.add_argument(
            '-R', '--maxRunJobs',
            help='max number of run jobs',
            type=int,
            default=6
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

    samplemod = importlib.import_module( subdir + opt.Command + opt.Year )

    cmd = opt.Command
    if opt.Trailer :
        opt.Command += " " + opt.Trailer

    samplelst = []
    for s in opt.Samplelst:
        samplelst += getattr( samplemod, s )
    samplelst = [ x for x in samplelst if opt.extract in x ]

    outputfilelst = []
    for sample in samplelst:
        command = "{} -y {} {}{}".format(opt.Command, opt.Year, opt.InputOption, sample)
        
        outputfilename = ".{}.sh".format( command.replace(" ", "") )
        outputfilelst.append( outputfilename )
        output = open( outputfilename, 'w' )
        output.write( qsub.format(os.environ['CMSSW_BASE'], command) )
        output.close()
        
        print ">> Preparing {}".format(sample)
        sys.stdout.flush()

    filelst = " ".join( outputfilelst )
    log = command.split("-s")[0].rstrip().replace(" ", "_")
    cmd = "nohup ./SentQJob.py -r {} -q {} -i {} > {}.out &".format( opt.maxRunJobs, opt.maxQueJobs, filelst, log )
    os.system( cmd )
    print "DONE"

if __name__ == '__main__':
    main(sys.argv)
