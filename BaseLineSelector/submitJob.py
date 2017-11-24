#!/bin/env python

import os
import sys
import argparse

# dataset = ["runB","runC","runD","runE","runF","runG","runH","TTbar","SingleTop_s-ch","SingleTop_t-ch","SingleTop_t-ch_anti","SingleTop_tW","SingleTop_tW_anti","DYJet_HT-100to200","DYJet_HT-200to400","DYJet_HT-400to600", "DYJet_HT-600to800", "DYJet_HT-800to1200","DYJet_HT-1200to2500","DYJet_HT-2500toInf", "VV_WW","VV_WZ","VV_ZZ","WJet_Pt-100to250","WJet_Pt-250to400", "WJet_Pt-400to600", "WJet_Pt-600toInf"]

dataset = ["TTbar"]

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
            help='test for sending jobs',
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
