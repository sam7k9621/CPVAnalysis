#!/bin/env python

import os
import sys
import argparse
import importlib
import datetime

CMSSW_BASE =  os.environ['CMSSW_BASE']
HOSTNAME   =  os.environ['HOSTNAME']

# espresso	20min	
# microcentury	1h	
# longlunch	2h	
# workday	8h	
# tomorrow	1d	
# testmatch	3d	
# nextweek	1w	

sub ="""
executable = {0}/src/CPVAnalysis/SentJob.sh

arguments  = $(base) $(cmd) $(ClusterID) $(ProcId)
output     = /afs/cern.ch/user/p/pusheng/condor/output/{1}-{2}.$(ClusterID).$(ProcId).out 
error      = /afs/cern.ch/user/p/pusheng/condor/error/{1}-{2}.$(ClusterID).$(ProcId).err 
log        = /afs/cern.ch/user/p/pusheng/condor/log/{1}-{2}.$(ClusterID).$(ProcId).log 

requirements = (OpSysAndVer =?= "CentOS7")
+JobFlavour = "longlunch"

queue base,cmd from job.dat
"""

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

def CondorJob( opt, samplelst ):
    # writing dat file
    output_dat = open( "job.dat", 'w' )
    for sample in samplelst:
        command = "{} --{} {}".format( opt.Command, opt.InputOption, sample )
        output_dat.write( '{0}, {1}\n'.format( CMSSW_BASE, command ) )
    output_dat.close()

    output_sub = open( "job.sub", 'w' )
    output_sub.write( sub.format( CMSSW_BASE, datetime.datetime.now().strftime("%m-%d-%H:%M"), MakeName( opt.Command.split(" ", 1 ) ) ) )
    output_sub.close()
   
    os.system( "condor_submit {}".format( "job.sub" ) )
    os.system( "rm job.sub" ) 
    os.system( "rm job.dat" ) 

def QJob( opt, samplelst ):
    outputfilelst = []
    for sample in samplelst:
        command = "{} --{} {}".format(opt.Command, opt.InputOption, sample)
        
        outputfilename = ".{}.sh".format( command.replace(" ", "") )
        outputfilelst.append( outputfilename )
        output = open( outputfilename, 'w' )
        output.write( qsub.format( CMSSW_BASE, command ) )
        output.close()
        
        print ">> Preparing {}".format(sample)
        sys.stdout.flush()

    filelst = " ".join( outputfilelst )
    cmd = "nohup ./SentQJob.py -r {} -q {} -i {} > {}.out &".format( opt.maxRunJobs, opt.maxQueJobs, filelst, MakeName( opt.Command.split(" ", 1 ) ))
    os.system( cmd )
    print "DONE"

def MakeName( command ):
    cmd     = command[0]
    trailer = command[1]
    for t in trailer.split(' '):
        if "--" in t:
            cmd += "_" + t[2:]
        elif "-" in t:
            pass 
        else:
            cmd += "_" + t
    return cmd

def main(args):
    parser = argparse.ArgumentParser( "Submit jobs for commands" )
    parser.add_argument( '-C', '--Command',     type=str,   required=True )
    parser.add_argument( '-I', '--InputOption', type=str,   default="sample" )
    parser.add_argument( '-E', '--Extract',     type=str,   nargs='+' )
    parser.add_argument( '-S', '--Samplelst',   type=str,   nargs='+' )
    parser.add_argument( '-R', '--MaxRunJobs',  type=int,   default=6 )
    parser.add_argument( '-Q', '--MaxQueJobs',  type=int,   default=5 )
    parser.add_argument( '-T', '--Test',        action='store_true' )
    
    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise
   
    command = opt.Command.split(" ", 1)
    year    = command[1].split(" ")[ command[1].split(" ").index("-y") + 1 ]

    if command[0] in ("PreCut", "FullCut") :
        subdir = "CPVAnalysis.BaseLineSelector."
    elif command[0] in ("MakeHist") :
        subdir = "CPVAnalysis.CompareDataMC."

    samplemod = importlib.import_module( subdir + command[0] + year )
    samplelst = []
    for s in opt.Samplelst:
        samplelst += getattr( samplemod, s )
    samplelst = [ x for x in samplelst if any( y in x for y in opt.Extract )]

    if opt.Test:
        print samplelst 
        return 

    if "lxplus" in HOSTNAME:
        CondorJob( opt, samplelst )

    elif "ntugrid5" in HOSTNAME:
        QJob( opt, samplelst )
    
if __name__ == '__main__':
    main(sys.argv)
