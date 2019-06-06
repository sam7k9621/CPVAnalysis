#!/bin/env python

import subprocess
import os
import sys
import json
import argparse

mclst = [
        ( 5, "DYJets_HT-100to200"),
        ( 5, "DYJets_HT-1200to2500"),
        ( 5, "DYJets_HT-200to400"),
        ( 5, "DYJets_HT-2500toInf"),
        ( 5, "DYJets_HT-400to600"),
        ( 5, "DYJets_HT-600to800"),
        ( 5, "DYJets_HT-70to100"),
        ( 5, "DYJets_HT-800to1200"),
        ( 1, "QCD_HT1000to1500"),
        ( 1, "QCD_HT100to200"),
        ( 1, "QCD_HT1500to2000"),
        ( 1, "QCD_HT200to300"),
        ( 1, "QCD_HT300to500"),
        ( 1, "QCD_HT500to700"),
        ( 1, "QCD_HT700to1000"),
        ( 1, "QCD_HT2000toInf_1"),
        ( 1, "ST_s-channel"),
        ( 1, "ST_t-channel_antitop"),
        ( 1, "ST_t-channel_top"),
        ( 1, "ST_tW_antitop"),
        ( 1, "ST_tW_top"),
        ( 10, "TTToSemiLeptonic"),
        ( 5, "WJets_HT-100To200"),
        ( 5, "WJets_HT-1200To2500"),
        ( 5, "WJets_HT-200To400"),
        ( 10, "WJets_HT-2500ToInf"),
        ( 5, "WJets_HT-400To600"),
        ( 5, "WJets_HT-600To800"),
        ( 5, "WJets_HT-70To100"),
        ( 5, "WJets_HT-800To1200"),
        ( 1, "WW"),
        ( 1, "WZ"),
        ( 1, "ZZ")
        ]

datalst = [
        (5, "el_Run2017B"),
        (5, "el_Run2017C"),
        (5, "el_Run2017D"),
        (5, "el_Run2017E"),
        (5, "el_Run2017F"),
        (5, "mu_Run2017B"),
        (5, "mu_Run2017C"),
        (5, "mu_Run2017D"),
        (5, "mu_Run2017E"),
        (10, "mu_Run2017F"),
        (1, "mu_Run2017F_ext1"),
        (1, "mu_Run2017F_ext2")
        ]
    
context = """
data = [
    {}
    ],

mc = [
    {}
    ]
"""

def main(args):

    parser = argparse.ArgumentParser(
            "Submit job for FullCut"
            )

    parser.add_argument(
            '-d', '--dir',
            help='input directory',
            type=str,
            default="/wk_cms2/youying/public/bprimekit_2017_ReMiniAODv2/"
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    s = subprocess.Popen( 'ls ' + opt.dir, shell=True, stdout=subprocess.PIPE )
    outputlst, err = s.communicate()
    outputlst = filter(None, outputlst.split('\n') )
   
    samplelst = []
    for mc in mclst:
        for output in outputlst:
            if all( sub in output for sub in mc[1].split("_") ):
                samplelst.append( ( output, mc ) )
                outputlst.remove( output )
                break

    for data in datalst:
        for output in outputlst:
            temp = data[1].split("_")
            if temp[0] in output.lower():
                samplelst.append( ( output + "/" + '_'.join( temp[1:] ), data ) )
                break

    from collections import OrderedDict
    content = OrderedDict()

    for sample, tag in samplelst:
        if tag[0] == 1:
            content[ tag[1] ] = { "path" : opt.dir + sample + "/*.root" }
        elif tag[0] == 2:
            content[ tag[1] + "_0" ] = { "path" : opt.dir + sample + "/*[0-5].root" }
            content[ tag[1] + "_1" ] = { "path" : opt.dir + sample + "/*[6-9].root" }
        elif tag[0] == 5:
            content[ tag[1] + "_0" ] = { "path" : opt.dir + sample + "/*[0-1].root" }
            content[ tag[1] + "_1" ] = { "path" : opt.dir + sample + "/*[2-3].root" }
            content[ tag[1] + "_2" ] = { "path" : opt.dir + sample + "/*[4-5].root" }
            content[ tag[1] + "_3" ] = { "path" : opt.dir + sample + "/*[6-7].root" }
            content[ tag[1] + "_4" ] = { "path" : opt.dir + sample + "/*[8-9].root" }
        else:
            content[ tag[1] + "_0" ] = { "path" : opt.dir + sample + "/*0.root" }
            content[ tag[1] + "_1" ] = { "path" : opt.dir + sample + "/*1.root" }
            content[ tag[1] + "_2" ] = { "path" : opt.dir + sample + "/*2.root" }
            content[ tag[1] + "_3" ] = { "path" : opt.dir + sample + "/*3.root" }
            content[ tag[1] + "_4" ] = { "path" : opt.dir + sample + "/*4.root" }
            content[ tag[1] + "_5" ] = { "path" : opt.dir + sample + "/*5.root" }
            content[ tag[1] + "_6" ] = { "path" : opt.dir + sample + "/*6.root" }
            content[ tag[1] + "_7" ] = { "path" : opt.dir + sample + "/*7.root" }
            content[ tag[1] + "_8" ] = { "path" : opt.dir + sample + "/*8.root" }
            content[ tag[1] + "_9" ] = { "path" : opt.dir + sample + "/*9.root" }
    
    data, mc = [], []
    for c in content:
        if "Run" in c:
            data.append( c )
        else:
            mc.append( c )

    data = ["'" + x + "'" for x in data ]
    mc   = ["'" + x + "'" for x in mc ]
    with open('python/PreCut.py', 'w' ) as fp:
        fp.write( context.format( ', '.join( data ), ', '.join( mc ) ) )

    ###
    content[ "lumimask" ] = "/wk_cms2/sam7k9621/CMSSW_9_4_13/src/CPVAnalysis/BaseLineSelector/data/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt"
    content[ "puweight" ] = "/wk_cms2/sam7k9621/CMSSW_9_4_13/src/CPVAnalysis/BaseLineSelector/data/pileupweights_69200.csv"
    content[ "mu_HLT" ] = [1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344]
    content[ "el_HLT" ] = [938, 939, 940, 941, 942, 943, 944]

    with open('settings/SampleInfo.json', 'w') as fp:
        json.dump(content, fp, indent=4)



if __name__ == '__main__':
    main(sys.argv)
