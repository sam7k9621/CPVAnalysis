#!/bin/env python

import os
import sys
import re

dataset = [
        "WJetsToLNu_HT-100To200",
        "WJetsToLNu_HT-200To400",
        "WJetsToLNu_HT-400To600",
        "WJetsToLNu_HT-600To800",
        "WJetsToLNu_HT-800To1200",
        "WJetsToLNu_HT-1200To2500",
        "WJetsToLNu_HT-2500ToInf",
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
        "VV_ZZ"
    ]

path = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/"

def main(args):

    with open( "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/data/Datacard.txt", "w") as outputfile:
        for d in dataset :
            version = len([name for name in os.listdir( path ) if re.search( d + "_[0-9]+.txt", name ) ])
            number = 0.0
            for i in range(1, version+1) :
                filename = path +  "Datacard_" + d + "_" + str(i) + ".txt"
                with open(filename) as f :
                    content = f.readlines()
                content = [ x.strip() for x in content ]
                number += float(content[4].split("=")[1])
                f.close()

            outputfile.write( d + "\n" )
            outputfile.write("Effective events after PU : {}\n ".format( number ))


    outputfile.close()

if __name__ == '__main__':
    main(sys.argv)
