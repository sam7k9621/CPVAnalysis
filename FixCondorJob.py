#!/bin/env python
import os
import sys
import datetime
import argparse
import subprocess

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

def main(args):
    parser = argparse.ArgumentParser("Fix")
    parser.add_argument( '-b', '--backup', action='store_true' )
    parser.add_argument( '-s', '--submit', action='store_true' )
    parser.add_argument( '-r', '--run', action='store_true' )
    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise

    path = "/afs/cern.ch/user/p/pusheng/condor"
    if not opt.backup:   
        s = subprocess.Popen( "ls -lh {}/error | awk '$5 != 0 {{print $9}}'".format( path ), shell=True, stdout=subprocess.PIPE )
        outputlst, err = s.communicate()
        outputlst = filter(None, outputlst.split('\n') )

        outputlst = [ x.replace( ".err", ".out" ) for x in outputlst ]
        with open( "{}/backup.txt".format( path ), 'w') as f:
            for output in outputlst:
                f.write( "{}\n".format( output ) )
    else:
        with open( "{}/backup.txt".format( path ) ) as f:
            outputlst = f.read().splitlines()

    print "Total job need to be submit: {}".format( len( outputlst ) )
    if not opt.run and not opt.submit:
        return 

    output_dat = open( "job.dat", 'w' )
    for idx, filename in enumerate( outputlst ):
        with open( "{}/output/{}".format( path, filename ) ) as f:
            content = ' '.join( f.readline().split(" ")[:-2] )
            
            if opt.run:
                print "[{}|{}] Processing {}".format( idx, len( outputlst ), content ) 
                os.system( content + " -c" )
   
            elif opt.submit:
                output_dat.write( "{}, {}\n".format( CMSSW_BASE, content ) )
    
            os.system( "rm {}/error/{}".format( path, filename.replace( ".out", ".err" ) ) )
    output_dat.close()

    if opt.submit:
        output_sub = open( "job.sub", 'w' )
        output_sub.write( sub.format( CMSSW_BASE, datetime.datetime.now().strftime("%m_%d_%H:%M") ) )
        output_sub.close()
  
        os.system( "condor_submit {}".format( "job.sub" ) )
        os.system( "rm job.sub" ) 
    os.system( "rm job.dat" ) 


if __name__ == '__main__':
    main(sys.argv)
