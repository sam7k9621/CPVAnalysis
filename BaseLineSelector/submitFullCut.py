#!/bin/env python

import os
import sys
import argparse

dataset = [
        "WJetsToLNu_HT-100To200",
        "WJetsToLNu_HT-200To400",
        "WJetsToLNu_HT-400To600",
        "WJetsToLNu_HT-600To800",
        "WJetsToLNu_HT-800To1200",
        "WJetsToLNu_HT-1200To2500",
        "WJetsToLNu_HT-2500ToInf"
        "QCD_HT100to200",
        "QCD_HT200to300",
        "QCD_HT300to500",
        "QCD_HT500to700",
        "QCD_HT700to1000",
        "QCD_HT1000to1500",
        "QCD_HT1500to2000",
        "QCD_HT2000toInf",
        "DYJet_HT-70to100",
        "DYJet_HT-100to200",
        "DYJet_HT-1200to2500",
        "DYJet_HT-200to400",
        "DYJet_HT-2500toInf",
        "DYJet_HT-400to600",
        "DYJet_HT-600to800",
        "DYJet_HT-800to1200",
        "SingleTop_s-ch",
        "SingleTop_t-ch",
        "SingleTop_t-ch_anti",
        "SingleTop_tW",
        "SingleTop_tW_anti",
        "TTbar",
        "VV_WW",
        "VV_WZ",
        "VV_ZZ",
        "Data"
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
"""
command = "FullCut -l {0} -s {1} "

def ResultName( opt, sample ) :
    filename = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/FullCut_" + opt.lepton + "_" + sample
    for arg in vars(opt):
        if ( getattr(opt, arg) and arg != "lepton" ) :
            try :
                filename += ( "_" + arg + "_" + getattr(opt, arg) )
            except :
                filename += ( "_" + arg )

    return filename + ".root"

def main(args):

    parser = argparse.ArgumentParser(
            "Submit jobs for FullCut"
            )

    parser.add_argument(
            '-l', '--lepton',
            help='lepton type',
            type=str,
            required=True
            )

    parser.add_argument(
            '-t', '--test',
            help='test for sending jobs',
            action='store_true'
            )

    parser.add_argument(
            '-r', '--region',
            help='CR region',
            type=str
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    for data in dataset :
        cmd = command.format(opt.lepton, data)

        if opt.test :
            print '>> Processing {}'.format(data)
            os.system( cmd + " -c -t")
            continue

        if os.path.isfile( ResultName(opt, data) ) :
            print "[Warning]" + data + " is already exist!!!"
            continue

        if opt.region :
            cmd += " -r " + opt.region

        output = open( ".sentJob.sh", 'w' )
        output.write( qsub )
        output.write( cmd )
        output.close()

        cmd = "qsub .sentJob.sh -N " + data
        print ">>Sending {}".format(data)
        os.system(cmd)
        os.system("rm .sentJob.sh")

if __name__ == '__main__':
    main(sys.argv)
