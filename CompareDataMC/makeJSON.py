#!/bin/env python

import os
import sys
import json
import argparse
        # sample, cross_section, gen_num
mclst =[
        [ "QCD_HT100to200",             27990000,           77777089.35],
        [ "QCD_HT200to300",             1712000,            37608094.69],
        [ "QCD_HT300to500",             347700,             36293911.87],
        [ "QCD_HT500to700",             32100,              40834538.24],
        [ "QCD_HT700to1000",            6831,               15126222.53],
        [ "QCD_HT1000to1500",           1207,               4612038.58],
        [ "QCD_HT1500to2000",           119.9,              7602054.23],
        [ "QCD_HT2000toInf",            25.24,              3716195.29],
        [ "DYJet_HT-70to100",           1.23 * 169.9,       9305773.2],
        [ "DYJet_HT-100to200",          1.23 * 147.4,       6916793.06],
        [ "DYJet_HT-1200to2500",        1.23 * 0.1514,      576786.67],
        [ "DYJet_HT-200to400",          1.23 * 40.99,       4929696.96],
        [ "DYJet_HT-2500toInf",         1.23 * 0.003565,    386734.55],
        [ "DYJet_HT-400to600",          1.23 * 5.678,       1224284.65],
        [ "DYJet_HT-600to800",          1.23 * 1.367,       8026407.62],
        [ "DYJet_HT-800to1200",         1.23 * 0.6304,      2582923.5],
        [ "SingleTop_s-ch",             3.36,               602629.59],
        [ "SingleTop_t-ch",             136.02,             64296935.48],
        [ "SingleTop_t-ch_anti",        80.95,              37536283.66],
        [ "SingleTop_tW",               35.6,               6701500.06],
        [ "SingleTop_tW_anti",          35.6,               6493496.53],
        [ "TTbar",                      831.76,             73900373.35],
        [ "VV_WW",                      118.7,              961891.78],
        [ "VV_WZ",                      47.13,              968023.57],
        [ "VV_ZZ",                      16.52,              815915.99],
        [ "WJetsToLNu_HT-100To200",     1.21 * 1345.7,      9882645.35],
        [ "WJetsToLNu_HT-200To400",     1.21 * 359.7,       13358363.35],
        [ "WJetsToLNu_HT-400To600",     1.21 * 48.91,       1900583.12],
        [ "WJetsToLNu_HT-600To800",     1.21 * 12.05,       12020199.53],
        [ "WJetsToLNu_HT-800To1200",    1.21 * 5.501,       6001577.17],
        [ "WJetsToLNu_HT-1200To2500",   1.21 * 1.329,       236702.46],
        [ "WJetsToLNu_HT-2500ToInf",    1.21 * 0.03216,     245295.36]
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
