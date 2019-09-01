#!/bin/env python

import os
import sys
import argparse
import importlib

# espresso	20min	
# microcentury	1h	
# longlunch	2h	
# workday	8h	
# tomorrow	1d	
# testmatch	3d	
# nextweek	1w	

CMSSW_BASE =  os.environ['CMSSW_BASE']

sub ="""
executable = {0}/src/CPVAnalysis/SentJob.sh

arguments  = $(cmd) $(opt) $(ClusterID) $(ProcId)
output     = /afs/cern.ch/user/p/pusheng/condor/output/{1}.$(ClusterID).$(ProcId).out 
error      = /afs/cern.ch/user/p/pusheng/condor/error/{1}.$(ClusterID).$(ProcId).err 
log        = /afs/cern.ch/user/p/pusheng/condor/log/{1}.$(ClusterID).$(ProcId).log 

requirements = (OpSysAndVer =?= "CentOS7")
+JobFlavour = "longlunch"

queue cmd,opt from job.dat
"""

def MakeName( cmd, trailer ):
    for t in trailer.split(' '):
        if "--" in t:
            cmd += "_" + t[2:]
        elif "-" in t:
            pass 
        else:
            cmd += "_" + t
    return cmd

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
            default=""
            )

    parser.add_argument(
            '-S', '--Samplelst',
            help='Input sample list',
            type=str
            )
   
    parser.add_argument(
            '-I', '--InputOption',
            help='Input argument',
            type=str,
            default=""
            )

    parser.add_argument(
            '--test',
            help='test',
            action='store_true'
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

    samplelst = importlib.import_module( subdir + opt.Command + opt.Year )

    if opt.test:
        for sample in getattr( samplelst, opt.Samplelst ):
            trailer = "{} -y {} -s {}".format( opt.Trailer, opt.Year, sample )
            os.system("{} {}".format( opt.Command, trailer ) )
        return
   
    # writing dat file
    output_dat = open( "job.dat", 'w' )
    for sample in getattr( samplelst, opt.Samplelst ):
        trailer = "{} {}{}".format( opt.Trailer, opt.InputOption, sample )
        output_dat.write( '{0}, {1}\n'.format( opt.Command, trailer ) )
    output_dat.close()

    output_sub = open( "job.sub", 'w' )
    output_sub.write( sub.format( CMSSW_BASE, MakeName( opt.Command, opt.Trailer ) ) )
    output_sub.close()
   
    os.system( "condor_submit {}".format( "job.sub" ) )
    os.system( "rm job.sub" ) 
    os.system( "rm job.dat" ) 
    
if __name__ == '__main__':
    main(sys.argv)
