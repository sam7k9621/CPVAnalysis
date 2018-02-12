#!/bin/env python

import os
import sys
import argparse

dataset = [
    "runB_1", "runB_2", "runB_3", "runB_4", "runB_5",
    "runC_1", "runC_2",
    "runD_1", "runD_2", "runD_3",
    "runE_1", "runE_2", "runE_3",
    "runF_1", "runF_2",
    "runG_1", "runG_2", "runG_3", "runG_4", "runG_5",
    "runH_1", "runH_2", "runH_3", "runH_4", "runH_5",
    "TTbar_1", "TTbar_2", "TTbar_3", "TTbar_4", "TTbar_5", "TTbar_6",
    "SingleTop_s-ch",
    "SingleTop_t-ch_1", "SingleTop_t-ch_2", "SingleTop_t-ch_3", "SingleTop_t-ch_4", "SingleTop_t-ch_5",
    "SingleTop_t-ch_anti_1", "SingleTop_t-ch_anti_2",
    "SingleTop_tW",
    "SingleTop_tW_anti",
    "DYJet_HT-100to200",
    "DYJet_HT-200to400",
    "DYJet_HT-400to600",
    "DYJet_HT-600to800",
    "DYJet_HT-800to1200",
    "DYJet_HT-1200to2500",
    "DYJet_HT-2500toInf",
    "VV_WW",
    "VV_WZ",
    "VV_ZZ",
    "WJet_Pt-100to250",
    "WJet_Pt-250to400",
    "WJet_Pt-400to600",
    "WJet_Pt-600toInf"
    ]

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
