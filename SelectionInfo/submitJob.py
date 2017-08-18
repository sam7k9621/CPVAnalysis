#!/bin/env python

import os
import sys
import argparse

dataset = ["runB","runC","runD","runE","runF","runG","runH","signal_mc","SingleTop","DYJet","VV","WJet"]
qsub ="""
#!/usr/bin/env sh

#PBS -V
#PBS -j oe
#PBS -q cms
#PBS -d /wk_cms/sam7k9621/qsub/dMESSAGE
#PBS -o /wk_cms/sam7k9621/qsub/oMESSAGE
#PBS -e /wk_cms/sam7k9621/qsub/eMESSAGE

cd /wk_cms2/sam7k9621/CMSSW_8_0_19/src && eval `scramv1 runtime -sh`
precut -l muon -s {0}
"""

def main(args):

    parser = argparse.ArgumentParser(
            "Options for updating a HLTList file"
            )

    parser.add_argument(
            '-t', '--test',
            help='Which HLT list file to update/write to',
            action='store_true'
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    for data in dataset :

        if opt.test :
           cmd = "precut -c -t -l muon -s {0}".format(data)
           print '>> Processing {}'.format(data)
           os.system(cmd)

        else:
            output = open( ".sentJob.sh", 'w' )
            output.write( qsub.format(data) )
            output.close()

            cmd = "qsub .sentJob.sh -N " + data
            print ">>Sending {}".format(data)
            os.system(cmd)
            os.system("rm .sentJob.sh")

if __name__ == '__main__':
    main(sys.argv)
