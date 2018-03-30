#!/bin/env python

import os
import sys
import json
import argparse
        # sample, cross_section, gen_num
mclst =[
        [ "QCD_HT200to300",      1712000,   18722416],
        [ "QCD_HT300to500",      347700,    17035891],
        [ "QCD_HT500to700",      32100,     18929951],
        [ "QCD_HT700to1000",     6831,      15629253],
        [ "QCD_HT1000to1500",    1207,      4767100],
        [ "QCD_HT1500to2000",    119.9,     3970819],
        [ "QCD_HT2000toInf",     25.24,     1991645],
        [ "DYJet_HT-100to200",   147.4,     2751187],
        [ "DYJet_HT-1200to2500", 0.116,     596079],
        [ "DYJet_HT-200to400",   40.99,     962195],
        [ "DYJet_HT-2500toInf",  0.002592,  399492],
        [ "DYJet_HT-400to600",   5.678,     1070454],
        [ "DYJet_HT-600to800",   1.367,     8292957],
        [ "DYJet_HT-800to1200",  0.6759,    2668730],
        [ "SingleTop_s-ch",      3.36,      1000000],
        [ "SingleTop_t-ch",      136.02,    67240808],
        [ "SingleTop_t-ch_anti", 80.95,     38811017],
        [ "SingleTop_tW",        35.6,      6952830],
        [ "SingleTop_tW_anti",   35.6,      69933094],
        [ "TTbar",               831.76,    77081156],
        [ "VV_WW",               118.7,     994012],
        [ "VV_WZ",               47.13,     1000000],
        [ "VV_ZZ",               16.52,     990064],
        [ "WJet_Pt-100to250",    676.3,     10089661],
        [ "WJet_Pt-250to400",    23.94,     1001250],
        [ "WJet_Pt-400to600",    3.031,     2751187],
        [ "WJet_Pt-600toInf",    0.4524,    989482]
    ]

def main(args):

    parser = argparse.ArgumentParser(
            "Submit job for FullCut"
            )

    parser.add_argument(
            '-f', '--filename',
            help='input filename',
            type=str
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    sample_path = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/"

    content = {}
    datalst = []
    for mc in mclst :

        # mc = [ "sample", cross_section, gen_num ]
        content[ mc[0] ] = {
                "mupath" : [ sample_path + opt.filename + "_mu_" + mc[0] + ".root"] ,
                "elpath" : [ sample_path + opt.filename + "_el_" + mc[0] + ".root"] ,
                "cross_section" : mc[1],
                "gen_num" : mc[2]
                }

    content["Data"] = {
            "mupath" : [ sample_path + opt.filename + "_mu_Data.root"] ,
            "elpath" : [ sample_path + opt.filename + "_el_Data.root"] ,
            }

    #Event weight info
    content["lumi"] = 36811

    info_path = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/CompareDataMC/data/"
    #Scale factor weight info
    content["BtagWeight"] = info_path + "CSVv2_Moriond17_B_H.csv"

    content["elTrg"] = {
            "file" : info_path + "elTrgSF.root",
            "title": "scale_ele32"
            }
    content["elID"] = {
            "file" : info_path + "elTightIDSF.root",
            "title": "EGamma_SF2D"
            }
    content["elRECO"] = {
            "file" : info_path + "elRECOSF.root",
            "title": "EGamma_SF2D"
            }
    content["elweight"] = ["elTrg", "elID", "elRECO"]

    content["muTrg"] = {
            "file" : info_path + "muTrgSF.root",
            "title": "abseta_pt_ratio"
            }
    content["muID"] = {
            "file" : info_path + "muIDSF.root",
            "title": "abseta_pt_ratio"
            }
    content["muISO"] = {
            "file" : info_path + "muISOSF.root",
            "title": "abseta_pt_ratio"
            }
    content["muweight"] = ["muTrg", "muID", "muISO"]

    with open('settings/WeightInfo.json', 'w') as fp:
        json.dump(content, fp, indent=4, sort_keys=True)

if __name__ == '__main__':
    main(sys.argv)
