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
    
    parser.add_argument(
            '-d', '--dir',
            help='input directory',
            type=str,
            default="2017results"
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
   
    sample_path = "root://cms-xrd-global.cern.ch//eos/cms/store/user/pusheng/" + opt.dir + "/"
    for mc in DC.mclst :

        # mc = [ "tag", "sample", cross_section ]
        content[ mc[0] ] = {
                "elpath_WJets"  : [ sample_path + opt.prefix + "_el_" + mc[1] + "_[0-9]+_region_WJets.root"] ,
                "mupath_WJets"  : [ sample_path + opt.prefix + "_mu_" + mc[1] + "_[0-9]+_region_WJets.root"] ,
                "elpath_QCD"    : [ sample_path + opt.prefix + "_el_" + mc[1] + "_[0-9]+_region_QCD.root"] ,
                "mupath_QCD"    : [ sample_path + opt.prefix + "_mu_" + mc[1] + "_[0-9]+_region_QCD.root"] ,
                "elpath"        : [ sample_path + opt.prefix + "_el_" + mc[1] + "_[0-9]+.root"] ,
                "mupath"        : [ sample_path + opt.prefix + "_mu_" + mc[1] + "_[0-9]+.root"] ,
                "cross_section" : mc[2],
                "gen_num"       : gennumlst[ mc[1] ]
                }
    
    content["Data"] = {
            "elpath_WJets"  : [ sample_path + opt.prefix + "_el_Run2017[A-Z]_[0-9]+_region_WJets.root"] ,
            "mupath_WJets"  : [ sample_path + opt.prefix + "_mu_Run2017[A-Z]_[0-9]+_region_WJets.root"] ,
            "elpath_QCD"    : [ sample_path + opt.prefix + "_el_Run2017[A-Z]_[0-9]+_region_QCD.root"] ,
            "mupath_QCD"    : [ sample_path + opt.prefix + "_mu_Run2017[A-Z]_[0-9]+_region_QCD.root"] ,
            "elpath"        : [ sample_path + opt.prefix + "_el_Run2017[A-Z]_[0-9]+.root"] ,
            "mupath"        : [ sample_path + opt.prefix + "_mu_Run2017[A-Z]_[0-9]+.root"] 
            }

    info_path = os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/CompareDataMC/data/"
    #Event weight info
    content["ellumi"] = 41540
    content["mulumi"] = 41540

    #Scale factor weight info
    content["BtagWeight"] = info_path + "CSVv2_94XSF_V2_B_F.csv"

    # content["elTrg"] = {
            # "file" : info_path + "elTrgSF.root",
            # "title": "scale_ele32"
            # }
    content["elID"] = {
            "file" : info_path + "2017_ElectronTight.root",
            "title": "EGamma_SF2D"
            }
    content["elRECO"] = {
            "file" : info_path + "egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root",
            "title": "EGamma_SF2D"
            }
    content["muTrg"] = {
            "file" : info_path + "EfficienciesAndSF_RunBtoF_Nov17Nov2017.root",
            "title": "IsoMu27_PtEtaBins"
            }
    content["muID"] = {
            "file" : info_path + "RunBCDEF_SF_ID.root",
            "title": "NUM_TightID_DEN_genTracks_pt_abseta"
            }
    content["muRECO"] = {
            "file" : info_path + "RunBCDEF_SF_ISO.root",
            "title": "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta"
            }
    content[ "mu_HLT" ] = [1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344]
    content[ "el_HLT" ] = [938, 939, 940, 941, 942, 943, 944]

    with open('settings/WeightInfo.json', 'w') as fp:
        json.dump(content, fp, indent=4)

if __name__ == '__main__':
    main(sys.argv)
