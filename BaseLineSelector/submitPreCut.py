#!/bin/env python

import os
import sys
import argparse

eldataset = [
    "el_runB_1", "el_runB_2", "el_runB_3", "el_runB_4", "el_runB_5",
    "el_runC_1", "el_runC_2",
    "el_runD_1", "el_runD_2", "el_runD_3",
    "el_runE_1", "el_runE_2", "el_runE_3",
    "el_runF_1", "el_runF_2",
    "el_runG_1", "el_runG_2", "el_runG_3", "el_runG_4", "el_runG_5",
    "el_runH_1", "el_runH_2", "el_runH_3", "el_runH_4", "el_runH_5"
    ]

mudataset = [
    "mu_runB_1", "mu_runB_2", "mu_runB_3", "mu_runB_4", "mu_runB_5",
    "mu_runC_1", "mu_runC_2",
    "mu_runD_1", "mu_runD_2", "mu_runD_3",
    "mu_runE_1", "mu_runE_2", "mu_runE_3",
    "mu_runF_1", "mu_runF_2",
    "mu_runG_1", "mu_runG_2", "mu_runG_3", "mu_runG_4", "mu_runG_5",
    "mu_runH_1", "mu_runH_2", "mu_runH_3", "mu_runH_4", "mu_runH_5"
    ]

mcdataset = [

    "WJetsToLNu_HT-200To400_1",
    "WJetsToLNu_HT-400To600_1",
    "WJetsToLNu_HT-600To800_1",
    "WJetsToLNu_HT-800To1200_1",
    "WJetsToLNu_HT-1200To2500_1",
    "WJetsToLNu_HT-2500ToInf_1"
  #   "TTbar_1", "TTbar_2", "TTbar_3", "TTbar_4", "TTbar_5", "TTbar_6",
    # "SingleTop_s-ch_1",
    # "SingleTop_t-ch_1", "SingleTop_t-ch_2", "SingleTop_t-ch_3", "SingleTop_t-ch_4", "SingleTop_t-ch_5",
    # "SingleTop_t-ch_anti_1", "SingleTop_t-ch_anti_2",
    # "SingleTop_tW_1",
    # "SingleTop_tW_anti_1",
    # "DYJet_HT-100to200_1",
    # "DYJet_HT-200to400_1",
    # "DYJet_HT-400to600_1",
    # "DYJet_HT-600to800_1",
    # "DYJet_HT-800to1200_1",
    # "DYJet_HT-1200to2500_1",
    # "DYJet_HT-2500toInf_1",
    # "VV_WW_1",
    # "VV_WZ_1",
    # "VV_ZZ_1",
    # "WJet_Pt-100to250_1",
    # "WJet_Pt-250to400_1",
    # "WJet_Pt-400to600_1",
    # "WJet_Pt-600toInf_1",
    # "QCD_HT200to300_1", "QCD_HT200to300_2",
    # "QCD_HT300to500_1", "QCD_HT300to500_2", "QCD_HT300to500_3",
    # "QCD_HT500to700_1", "QCD_HT500to700_2", "QCD_HT500to700_3",
    # "QCD_HT700to1000_1",
    # "QCD_HT1000to1500_1",
    # "QCD_HT1500to2000_1",
    # "QCD_HT2000toInf_1"
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
{0}
"""

def MakeFilename(data):
    filename = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/PreCut_"
    return filename + data + ".root"

def MakeCommand(opt, data):
    prefix = "PreCut "

    if opt.test:
        prefix += "-c -t "

    return"{0} -s {1}".format(prefix, data)

def GetDatalst(opt):
    if opt.lepton == "el" :
        return eldataset
    elif opt.lepton == "mu" :
        return mudataset
    else :
        return mcdataset

def main(args):

    parser = argparse.ArgumentParser(
            "Submit jobs for PreCut"
            )

    parser.add_argument(
            '-t', '--test',
            help='test for sending jobs',
            action='store_true'
            )

    parser.add_argument(
            '-l', '--lepton',
            help='lepton type',
            type=str
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    for data in GetDatalst(opt) :
        if opt.test :
           cmd = MakeCommand(opt, data)
           print '>> Processing {}'.format(data)
           os.system(cmd)

        else :
            if os.path.isfile( MakeFilename(data) ) :
                print "[Warning] " + data + " is already exist!!!"
            else :
                command = MakeCommand(opt, data)

                output = open( ".sentJob.sh", 'w' )
                output.write( qsub.format(command) )
                output.close()

                cmd = "qsub .sentJob.sh -N " + data
                print ">>Sending {}".format(data)
                os.system(cmd)
                os.system("rm .sentJob.sh")

if __name__ == '__main__':
    main(sys.argv)
