#!/bin/env python

import os
import sys
import re

dataset = [
        # "WJets_HT-70To100",
        "WJets_HT-100To200",
        "WJets_HT-200To400",
        "WJets_HT-400To600",
        "WJets_HT-600To800",
        "WJets_HT-800To1200",
        "WJets_HT-1200To2500",
        "WJets_HT-2500ToInf",
        # "QCD_HT50to200",
        "QCD_HT100to200",
        "QCD_HT200to300",
        "QCD_HT300to500",
        "QCD_HT500to700",
        "QCD_HT700to1000",
        "QCD_HT1000to1500",
        "QCD_HT1500to2000",
        "QCD_HT2000toInf",
        "DYJets_HT-70to100",
        "DYJets_HT-100to200",
        "DYJets_HT-1200to2500",
        "DYJets_HT-200to400",
        "DYJets_HT-2500toInf",
        "DYJets_HT-400to600",
        "DYJets_HT-600to800",
        "DYJets_HT-800to1200",
        "ST_s-channel",
        "ST_t-channel_antitop",
        "ST_t-channel_top",
        "ST_tW_antitop",
        "ST_tW_top",
        "TTToSemiLeptonic",
        "WW",
        "WZ",
        "ZZ"
    ]

path = os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/BaseLineSelector/results/" if "ntugrid5" in os.environ["HOSTNAME"] else "/eos/cms/store/user/pusheng/public/PreCut/"

def main(args):

    for i in [ "16", "17", "18" ]:
        with open( os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/BaseLineSelector/data/{}_Datacard.txt".format( i ), "w") as outputfile:
            for d in dataset :
                version = [name for name in os.listdir( path ) if re.search( "{}_{}_[0-9]+.txt".format( i, d ), name ) ]
                number = 0.0
                
                for ver in version :
                    filename = path + ver 
                    with open(filename) as f :
                        content = f.readlines()
                    content = [ x.strip() for x in content ]
                    number += float(content[4].split("=")[1])
                    print ver, float(content[4].split("=")[1])
                    f.close()

                outputfile.write( d + "\n" )
                outputfile.write("Effective events : {}\n ".format( number ))


        outputfile.close()

if __name__ == '__main__':
    main(sys.argv)
