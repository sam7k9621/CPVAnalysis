#!/bin/env python

import os
import sys
import json
import argparse
        # sample, cross_section, gen_num
mclst =[
        [ "QCD_HT200to300",      1712000,   38857977],
        [ "QCD_HT300to500",      347700,    37502012],
        [ "QCD_HT500to700",      32100,     42190760],
        [ "QCD_HT700to1000",     6831,      15629253],
        [ "QCD_HT1000to1500",    1207,      4767100],
        [ "QCD_HT1500to2000",    119.9,     7855883],
        [ "QCD_HT2000toInf",     25.24,     3839969],
        [ "DYJet_HT-100to200",   1.23 * 147.4,     7145468],
        [ "DYJet_HT-1200to2500", 1.23 * 0.116,     596079],
        [ "DYJet_HT-200to400",   1.23 * 40.99,     5093947],
        [ "DYJet_HT-2500toInf",  1.23 * 0.002592,  399492],
        [ "DYJet_HT-400to600",   1.23 * 5.678,     1265157],
        [ "DYJet_HT-600to800",   1.23 * 1.367,     8292957],
        [ "DYJet_HT-800to1200",  1.23 * 0.6759,    2668730],
        [ "SingleTop_s-ch",      3.36,      622990],
        [ "SingleTop_t-ch",      136.02,    66437948],
        [ "SingleTop_t-ch_anti", 80.95,     38780433],
        [ "SingleTop_tW",        35.6,      6924540],
        [ "SingleTop_tW_anti",   35.6,      6710218],
        [ "TTbar",               831.76,    76357853],
        [ "VV_WW",               118.7,     994012],
        [ "VV_WZ",               47.13,     1000000],
        [ "VV_ZZ",               16.52,     843120],
        [ "WJetsToLNu_HT-200To400",   359.7,   13758694 * 2.838],
        [ "WJetsToLNu_HT-400To600",   48.91,   1963464 * 0.992],
        [ "WJetsToLNu_HT-600To800",   12.05,   12420040 * 0.937],
        [ "WJetsToLNu_HT-800To1200",  5.501,   6200954 * 3.037],
        [ "WJetsToLNu_HT-1200To2500", 1.329,   244532  * 3.054],
        [ "WJetsToLNu_HT-2500ToInf",  0.03216, 253561  * 3.012],
        [ "WJet_Pt-100to250",    676.3,     3644567],
        [ "WJet_Pt-250to400",    23.94,     369290],
        [ "WJet_Pt-400to600",    3.031,     376470],
        [ "WJet_Pt-600toInf",    0.4524,    396491]
    ]

def main(args):

    parser = argparse.ArgumentParser(
            "Submit job for FullCut"
            )

    parser.add_argument(
            '-p', '--prefix',
            help='prefix of input filename',
            type=str
            )

    parser.add_argument(
            '-c', '--cut',
            help='input cut region',
            type=str,
            default=""
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
                "mupath" : [ sample_path + opt.prefix + "_mu_" + mc[0] + opt.cut + ".root"] ,
                "elpath" : [ sample_path + opt.prefix + "_el_" + mc[0] + opt.cut + ".root"] ,
                "cross_section" : mc[1],
                "gen_num" : mc[2]
                }

    content["Data"] = {
            "mupath" : [ sample_path + opt.prefix + "_mu_Data" + opt.cut + ".root"] ,
            "elpath" : [ sample_path + opt.prefix + "_el_Data" + opt.cut + ".root"] ,
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
