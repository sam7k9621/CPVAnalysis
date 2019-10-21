#!/bin/env python

import os
import sys
import CPVAnalysis.CompareDataMC.Datacard as DC 
from collections import OrderedDict

text1 = 'setattr( sample{}, "{}", cms.PSet( evt = cms.double( {} ), xsec = cms.double( {} ), path = cms.string( "{}" ) ) )\n'
text2 = 'setattr( sample{}, "{}", cms.PSet( path = cms.string( "{}" ) ) )\n'
text3 = 'setattr( sample{}, "{}", cms.PSet( path = cms.string( "{}" ), title = cms.string( "{}" ) ) )\n'
text4 = 'sample{}.Info = cms.PSet( lumi = cms.double( {} ) )\n'

def MakeWeightInfo( outputfile, year ):
   
    ###########################################################################################################################
    outputfile.write("\n" + "#"*90 + "\n" )
    outputfile.write( "sample{} = cms.PSet()\n".format( year) )

    ###########################################################################################################################
    gennumlst = DC.GetGennumber( year )
    content = OrderedDict()
    sample_path = "/eos/cms/store/user/pusheng/public/FullCut/"

    ###########################################################################################################################
    for mc in DC.mclst :
        # mc = [ "tag", "sample", cross_section ]
        outputfile.write( text1.format( year, mc[0], gennumlst[ mc[1] ], mc[2], sample_path + "FullCut_%1%_%2%_{}%3%%4%.root".format( mc[1] ) ) )

    outputfile.write( text2.format( year, "Data", sample_path + "FullCut_%1%_%2%_Run20{}*%3%%4%.root".format( year ) ) )
    ###########################################################################################################################
    info_path = os.environ["CMSSW_BASE"] + "/src/CPVAnalysis/CompareDataMC/data/"
    if year == "16":
        #Event weight info
        outputfile.write( text4.format( year, "35922" ) )
    
        #Scale factor weight info
        outputfile.write( text3.format( year, "elID"  , info_path + "2016LegacyReReco_ElectronTight_Fall17V2.root", "EGamma_SF2D" ) )  
        outputfile.write( text3.format( year, "elRECO", info_path + "EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root",    "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "muID"  , info_path + "muid_Run16All.root",                           "muid" ) )
        outputfile.write( text3.format( year, "muRECO", info_path + "muiso_Run16All.root",                          "muiso" ) )

    elif year == "17":
        #Event weight info
        outputfile.write( text4.format( year, "41540" ) )

        #Scale factor weight info
        outputfile.write( text3.format( year, "elID"  , info_path + "2017_ElectronTight.root",                        "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "elRECO", info_path + "egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root", "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "muID"  , info_path + "RunBCDEF_SF_ID.root",                            "NUM_TightID_DEN_genTracks_pt_abseta" ) )
        outputfile.write( text3.format( year, "muRECO", info_path + "RunBCDEF_SF_ISO.root",                           "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta" ) )
    
    elif year == "18":
        #Event weight info
        outputfile.write( text4.format( year, "58880" ) )

        #Scale factor weight info
        outputfile.write( text3.format( year, "elID"  , info_path + "2018_ElectronTight.root",              "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "elRECO", info_path + "egammaEffi.txt_EGM2D_updatedAll.root", "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "muID"  , info_path + "RunABCD_SF_ID.root",                   "NUM_TightID_DEN_TrackerMuons_pt_abseta" ) )
        outputfile.write( text3.format( year, "muRECO", info_path + "RunABCD_SF_ISO.root",                  "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta" ) )
    
def main(args):

    outputfile = open( "python/WeightInfo.py", "w+")
    outputfile.write( "import FWCore.ParameterSet.Config as cms\n" )
    MakeWeightInfo( outputfile, "16" )
    MakeWeightInfo( outputfile, "17" )
    MakeWeightInfo( outputfile, "18" )
    outputfile.close()

if __name__ == '__main__':
    main(sys.argv)
