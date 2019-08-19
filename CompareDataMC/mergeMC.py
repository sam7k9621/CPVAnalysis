#!/bin/env python
import os
import sys
import json
import argparse
import CPVAnalysis.CompareDataMC.MakeHist as MakeHist
def main(args):

    parser = argparse.ArgumentParser(
            "Submit job for FullCut"
            )

    parser.add_argument(
            '-l', '--lepton',
            help='lepton type',
            type=str,
            required=True
            )

    parser.add_argument(
            '-r', '--region',
            help='which sample',
            type=str,
            default=""
            )

    parser.add_argument(
            '-e', '--uncertainty',
            help='systematic uncertainty',
            type=str,
            default=""
            )

    parser.add_argument(
            '-c', '--chi2',
            help='chi2 cut',
            type=str
            )

    parser.add_argument(
            '-o', '--Opt',
            help='lep_tmass optimization cut',
            type=str
            )

    parser.add_argument(
            '-A', '--Acp',
            help='intrinsci Acp(%)',
            type=str
            )

    parser.add_argument(
            '-x', '--mixed',
            help='randomly choose events',
            action='store_true'
            )
    parser.add_argument(
            '-p', '--pileup',
            help='randomly choose events',
            action='store_true'
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    sample_path = os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/CompareDataMC/results/"
    samplelst       = [ "Data", "QCD", "DYJets", "SingleTop", "VV", "WJets", "ttbar" ] #sr all
    cmd         = "hadd -f "

    for s in samplelst :
        output = sample_path + "HistMerge_" + opt.lepton + "_" + s

        lep = opt.lepton if opt.lepton != "co" else "*"
        input  = sample_path + "Hist_" + lep + "_{}"

        if opt.chi2 :
            input  += "_chi2_" + opt.chi2
            output += "_chi2_" + opt.chi2

        if opt.Acp :
            input  += "_Acp_" + opt.Acp
            output += "_Acp_" + opt.Acp

        if opt.Opt :
            input  += "_Opt_" + opt.Opt
            output += "_Opt_" + opt.Opt

        if opt.uncertainty :
            input  += "_uncertainty_" + opt.uncertainty
            output += "_uncertainty_" + opt.uncertainty

        if opt.mixed :
            input  += "_mixed"
            output += "_mixed"
        
        if opt.pileup :
            output += "_pileup"

        
        if not opt.region :
           output += "_" + "SR"

        elif opt.region == "0bjet":
            input  += "_region_" + opt.region
            output += "_" + "CS"
        
        else :
            input  += "_region_" + opt.region
            output += "_" + "CS2"
        
        if opt.pileup :
            input += "_pileup"


        input  += ".root"
        output += ".root "

        inputlst = [ input.format( x ) for x in MakeHist.samplelst if s in x ]
        os.system( cmd + output + " ".join( inputlst ) )


if __name__ == '__main__':
    main(sys.argv)
