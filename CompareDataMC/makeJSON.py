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
            default="FullCut2016"
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
   
    sample_path = "/eos/cms/store/user/youying/public/" + opt.dir + "/"
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
            "elpath_WJets"  : [ sample_path + opt.prefix + "_el_Run2016[A-Z]_[0-9]+_region_WJets.root"] ,
            "mupath_WJets"  : [ sample_path + opt.prefix + "_mu_Run2016[A-Z]_[0-9]+_region_WJets.root"] ,
            "elpath_QCD"    : [ sample_path + opt.prefix + "_el_Run2016[A-Z]_[0-9]+_region_QCD.root"] ,
            "mupath_QCD"    : [ sample_path + opt.prefix + "_mu_Run2016[A-Z]_[0-9]+_region_QCD.root"] ,
            "elpath"        : [ sample_path + opt.prefix + "_el_Run2016[A-Z]_[0-9]+.root"] ,
            "mupath"        : [ sample_path + opt.prefix + "_mu_Run2016[A-Z]_[0-9]+.root"] 
            }

    info_path = os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/CompareDataMC/data/"
    #Event weight info
    content["ellumi"] = 35922
    content["mulumi"] = 35922

    #Scale factor weight info

    # content["elTrg"] = {
            # "file" : info_path + "elTrgSF.root",
            # "title": "scale_ele32"
            # }
    content["elID"] = {
            "file" : info_path + "2016LegacyReReco_ElectronTight_Fall17V2.root",
            "title": "EGamma_SF2D"
            }
    content["elRECO"] = {
            "file" : info_path + "EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root",
            "title": "EGamma_SF2D"
            }
    # content["muTrg"] = {
            # "file" : info_path + "EfficienciesAndSF_RunBtoF_Nov17Nov2017.root",
            # "title": "IsoMu27_PtEtaBins"
            # }
    content["muID"] = {
            "file" : info_path + "muid_Run16All.root",
            "title": "muid"
            }
    content["muRECO"] = {
            "file" : info_path + "muiso_Run16All.root",
            "title": "muiso"
            }

    with open('settings/WeightInfo.json', 'w') as fp:
        json.dump(content, fp, indent=4)

if __name__ == '__main__':
    main(sys.argv)
