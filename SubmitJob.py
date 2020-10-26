#!/bin/env python
import re
import os
import json
import collections
import argparse
import importlib
import datetime
from termcolor import colored

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
output     = /afs/cern.ch/user/p/pusheng/condor/output/{1}_$(ClusterID)_$(ProcId).out 
error      = /afs/cern.ch/user/p/pusheng/condor/error/{1}_$(ClusterID)_$(ProcId).err 
log        = /afs/cern.ch/user/p/pusheng/condor/log/{1}_$(ClusterID)_$(ProcId).log

requirements = (OpSysAndVer =?= "CentOS7")
+JobFlavour = "microcentury"

queue base,cmd from job.dat
"""

def CondorJob( job_detail ):
    output_sub = open( "job.sub", 'w' )
    output_sub.write( sub.format( CMSSW_BASE, datetime.datetime.now().strftime("%m_%d_%H:%M") ) )
    output_sub.close()
    cmd = "nohup ./SentQJob.py -s {} > {}.out &".format( job_detail.split("_")[0], command.replace( " ", "_" ) ) 

def QJob( job_detail ):
    cmd = "nohup ./SentQJob.py -s %s -r %s -q %s -i {0}.dat > {0}.out &".format( job_detail ) % tuple( job_detail.split( "_" ) )
    os.system( cmd )

def main():
    parser = argparse.ArgumentParser( "Submit jobs for commands" )
    #-------------------------------------------------------------------------------------
    parser.add_argument( '-F', '--InputFile',   type=str )
    parser.add_argument( '-T', '--Test',        action='store_true' )
    #-------------------------------------------------------------------------------------
    parser.add_argument( '-C', '--Command',     type=str )
    parser.add_argument( '-M', '--Module',      type=str )
    parser.add_argument( '-I', '--InputOption', type=str,   default="sample" )
    parser.add_argument( '-E', '--Extract',     type=str,   nargs='+' )
    parser.add_argument( '-X', '--Exclude',     type=str,   nargs='+' )
    parser.add_argument( '-S', '--Samplelst',   type=str,   nargs='+' )
    #-------------------------------------------------------------------------------------

    try:
        par = parser.parse_args()
    except:
        parser.print_help()
        raise

    with open( par.InputFile, 'r' ) as inputfile:
        data = json.loads( re.sub("//.*","", inputfile.read() ,flags=re.MULTILINE) ) 

    for job_detail, arglst in data.iteritems(): 
        commandlst = []
        for arg in arglst: 
            try:
                opt = parser.parse_args( [ x.rstrip(" ").lstrip(" ") for x in arg.split(" @ ") ] ) 
            except:
                parser.print_help()
                raise
            
            command = opt.Command.split(" ", 1)
            module = opt.Module if opt.Module else command[0] + command[1].split(" ")[ command[1].split(" ").index("-y") + 1 ] 
            if any( x in module for x in [ "PreCut", "FullCut" ] ):
                subdir = "CPVAnalysis.BaseLineSelector."
            else:
                subdir = "CPVAnalysis.CompareDataMC."
       
            samplelst = []
            samplemod = importlib.import_module( subdir + module )
            for s in opt.Samplelst:
                samplelst += getattr( samplemod, s )
   
            if opt.Extract:
                samplelst = [ x for x in samplelst if any( y in x for y in opt.Extract )]
            if opt.Exclude:
                samplelst = [ x for x in samplelst if not any( y in x for y in opt.Exclude )]

           
            for sample in samplelst:
                commandlst.append( "{} --{} {}".format(opt.Command, opt.InputOption, sample) )
       
        if par.Test:
            print "\n".join( commandlst )
            print "[ Total jobs: %s | Wait time: %s | Max jobs: %s | Max queue jobs: %s ] " % tuple(  [colored( len(commandlst), 'red')] + job_detail.split("_" ) )
            continue 
        
        print "[ Total jobs: %s | Wait time: %s | Max jobs: %s | Max queue jobs: %s ] " % tuple(  [colored( len(commandlst), 'red')] + job_detail.split("_" ) )

        # writing dat file
        if commandlst:
            with open( "{}.dat".format( job_detail ), "w" ) as output_dat:
                for command in commandlst:
                    command = ' '.join( command.split() )
                    output_dat.write(  '{0}, {1}\n'.format( CMSSW_BASE, command ) )

            if "lxplus" in HOSTNAME:
                CondorJob( job_detail )

            elif "ntugrid5" in HOSTNAME:
                QJob( job_detail )
    
if __name__ == '__main__':
    main()
