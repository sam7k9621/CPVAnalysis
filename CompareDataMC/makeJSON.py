#!/bin/env python

import os
import sys
import json
import argparse
import CPVAnalysis.CompareDataMC.Datacard as DC 

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

    gennumlst = DC.GetGennumber()

    from collections import OrderedDict
    content = OrderedDict()
    datalst = []
    
    for mc in DC.mclst :

        # mc = [ "sample", cross_section, gen_num ]
        content[ mc[0] ] = {
                "elpath" : [ DC.sample_path + opt.prefix + "_el_" + mc[0] + opt.cut + ".root"] ,
                "mupath" : [ DC.sample_path + opt.prefix + "_mu_" + mc[0] + opt.cut + ".root"] ,
                "cross_section" : mc[1],
                "gen_num" : gennumlst[ mc[0] ]
                }
    content["Data"] = {
            "elpath" : [ DC.sample_path + opt.prefix + "_el_Data" + opt.cut + ".root"] ,
            "mupath" : [ DC.sample_path + opt.prefix + "_mu_Data" + opt.cut + ".root"] ,
            }

    info_path = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/CompareDataMC/data/"
    #Event weight info
    content["ellumi"] = 35700
    content["mulumi"] = 35900

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
    content["muTrg"] = {
            "file" : info_path + "muTrgSF.root",
            "title": "abseta_pt_ratio"
            }
    content["muID"] = {
            "file" : info_path + "muIDSF.root",
            "title": "abseta_pt_ratio"
            }
    content["muRECO"] = {
            "file" : info_path + "muISOSF.root",
            "title": "abseta_pt_ratio"
            }

    with open('settings/WeightInfo.json', 'w') as fp:
        json.dump(content, fp, indent=4)

if __name__ == '__main__':
    main(sys.argv)
