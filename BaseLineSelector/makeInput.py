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
        ( 1, "QCD_HT2000toInf"),
        ( 1, "ST_s-channel"),
        ( 1, "ST_t-channel_antitop"),
        ( 1, "ST_t-channel_top"),
        ( 1, "ST_tW_antitop"),
        ( 1, "ST_tW_top"),
        ( 15, "TTToSemiLeptonic"),
        ( 5, "WJets_HT-100To200"),
        ( 10, "WJets_HT-1200To2500"),
        ( 5, "WJets_HT-200To400"),
        ( 10, "WJets_HT-2500ToInf"),
        ( 5, "WJets_HT-400To600"),
        ( 10, "WJets_HT-600To800"),
        ( 5, "WJets_HT-70To100"),
        ( 10, "WJets_HT-800To1200"),
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
        (10, "mu_Run2017E"),
        (15, "mu_Run2017F"),
        (1, "mu_Run2017F_ext1"),
        (1, "mu_Run2017F_ext2")
        ]
    
context = """
data = [
    {}
    ]

mc = [
    {}
    ]
"""

def SplitFile( num ):
    if num == 2:
        return [ "[0-5]", "[6-9]"]
    elif num == 3:
        return [ "[0-3]", "[4-6]", "[7-9]"]
    elif num == 5:
        return [ "[0-1]", "[2-3]", "[4-5]", "[6-7]", "[8-9]" ]

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
            content[ tag[1] + "_0" ] = { "path" : opt.dir + sample + "/*.root" }
        elif tag[0] < 10:
            for i in range( tag[0] ):
                content[ tag[1] + "_{}".format( str( i ) ) ] = { "path" : opt.dir + sample + "/*{}.root".format( SplitFile( tag[0] )[i] ) }
        else:
            if tag[0] == 10:
                filelst = [x+y for x in SplitFile(2) for y in SplitFile(5) ] 
            else:
                filelst = [x+y for x in SplitFile(3) for y in SplitFile(5) ] 
            
            for i in range( tag[0] ):
                content[ tag[1] + "_{}".format( str( i ) ) ] = { "path" : opt.dir + sample + "/*{}.root".format( filelst[i] ) }
            # include one-digit file
            content[ tag[1] + "_{}".format( str( tag[0] ) ) ] = { "path" : opt.dir + sample + "/*_[0-9].root"}


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
    info_path = os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/BaseLineSelector/data/"
    content[ "mu_HLT" ] = [1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324] #HLT_IsoMu24_v* 
    content[ "el_HLT" ] = [917, 918, 919, 920, 921, 922, 923]                          #HLT_Ele32_eta2p1_WPTight
    content[ "BtagWeight" ] = info_path + "DeepCSV_2016LegacySF_V1.csv"
    
    content[ "SR_b_eff" ] = {
            "file": info_path + "beffPlot_TTToSemiLeptonic_0pt6321.root",
            "title": "eff_b"
            }
    content[ "SR_c_eff" ] = {
            "file": info_path + "beffPlot_TTToSemiLeptonic_0pt6321.root",
            "title": "eff_c"
            }
    content[ "SR_l_eff" ] = {
            "file": info_path + "beffPlot_TTToSemiLeptonic_0pt6321.root",
            "title": "eff_l"
            }

    content[ "CR_b_eff" ] = {
            "file": info_path + "beffPlot_WJets_0pt2217.root",
            "title": "eff_b"
            }
    content[ "CR_c_eff" ] = {
            "file": info_path + "beffPlot_WJets_0pt2217.root",
            "title": "eff_c"
            }
    content[ "CR_l_eff" ] = {
            "file": info_path + "beffPlot_WJets_0pt2217.root",
            "title": "eff_l"
            }

    with open('settings/SampleInfo.json', 'w') as fp:
        json.dump(content, fp, indent=4)



if __name__ == '__main__':
    main(sys.argv)
