#!/bin/env python

import os
import sys
import CPVAnalysis.CompareDataMC.Datacard as DC 
from collections import OrderedDict

CMSSW_BASE = os.environ[ "CMSSW_BASE" ]
text1 = 'setattr( sample{}, "{}", cms.PSet( evt = cms.double( {} ), xsec = cms.double( {} ), path = cms.string( "{}" ) ) )\n'
text2 = 'setattr( sample{}, "{}", cms.PSet( path = cms.string( "{}" ) ) )\n'
text3 = 'setattr( sample{}, "{}", cms.PSet( path = cms.string( "{}" ), title = cms.string( "{}" ) ) )\n'
text4 = 'sample{}.Info = cms.PSet( lumi = cms.double( {} ) )\n'

context="""
samplelst = [
    {}
    ]

uncertainty = [ 
    {}
    ]

pseudo = [
    {}
    ]

shuffle = [
    {}
    ]

acp = [
    {}
    ]
"""

dtglst="""
dtglst = [
    'ttbar_dtg0',
    'ttbar_dtg1',
    'ttbar_dtg2',
    'ttbar_dtg3',
    'ttbar_dtg3',
    'ttbar_dtg5'
    ]
"""

def MakeWeightInfo( outputfile, year ):
   
    ###########################################################################################################################
    outputfile.write("\n" + "#"*90 + "\n" )
    outputfile.write( "sample{} = cms.PSet()\n".format( year) )

    ###########################################################################################################################
    gennumlst = DC.GetGennumber( year )
    content = OrderedDict()
    sample_path = CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/results/" if "lxplus" not in os.environ["HOSTNAME"] else "/eos/cms/store/user/pusheng/public/FullCut/"
    filename    = "FullCut_%1%_%2%_{}%3%%4%.root"
    
    ###########################################################################################################################
    samplelst = []
    for mc in DC.mclst :
        # mc = [ "tag", "sample", cross_section ]
        if gennumlst[ mc[1] ] <= 0:
            print "Didn't find sample {} in {} ".format( mc[1], year )
            continue

        outputfile.write( text1.format( year, mc[0], gennumlst[ mc[1] ], mc[2], sample_path + filename.format( mc[1] ) ) )
        samplelst.append( mc[0] )
    outputfile.write( text2.format( year, "Data", sample_path + filename.format( "Run*" ) ) )
   
    samplelst.append( "Data" )
    samplelst = ["'" + x  + "'" for x in samplelst if "CP5" not in x and "hdamp" not in x and "mtop" not in x and "dtg" not in x ]
    
    uncertainty = [ 
            # "pileup_up", "pileup_dn", "bctag_up", "bctag_dn", "lighttag_up", "lighttag_dn", "lepton_up", "lepton_dn", 
            # "JER_up", "JER_dn", "JEC_up", "JEC_dn",
            # "PDF_up", "PDF_dn", "muFmuR_up", "muFmuR_dn", "FR_up", "FR_dn" ]
            
            "pileup_up", "pileup_dn", "bctag_up", "bctag_dn", "lighttag_up", "lighttag_dn", "lepton_up", "lepton_dn", 
            "JER_up", "JER_dn", "JEC_up", "JEC_dn",
            "CP5CR1", "CP5CR2", "CP5down", "CP5up", "hdampDOWN", "hdampUP", "ISR_up", "ISR_dn", "FSR_up", "FSR_dn",
            "PDF_up", "PDF_dn", "muFmuR_up", "muFmuR_dn", "FR_up", "FR_dn",
            "mtop1695", "mtop1755", "resolution_10", "resolution_-10" ,"woTopPT"
            ]
    uncertainty = ["'" + x  + "'" for x in uncertainty]
    
    with open( "python/MakeHist{}.py".format( year ), "w+" ) as fp:
        fp.write( context.format( 
            ',\n    '.join( samplelst ), 
            ',\n    '.join( uncertainty ), 
            ", ".join( [ str(x) for x in range(1,21) ] ), 
            ", ".join( [ str(x) for x in range(1,1001) ] ),
            ", ".join( [ str(x) for x in [ -15, -10, -7, -5, -3, 0, 3, 5, 7, 10, 15 ] ] )
            ) )

        if year == "17":
            fp.write( dtglst )
    
    ###########################################################################################################################
    info_path = CMSSW_BASE + "/src/CPVAnalysis/CompareDataMC/data/"
    if year == "16":
        #Event weight info
        outputfile.write( text4.format( year, "35922" ) )
        outputfile.write( text2.format( year, "BtagWeight", CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_2016LegacySF_V1.csv" ) ) 
        #Scale factor weight info
        outputfile.write( text3.format( year, "elHLT" , info_path + "HLT_SF_16.root",                               "abseta_pt_ratio" ) )  
        outputfile.write( text3.format( year, "elID"  , info_path + "2016LegacyReReco_ElectronTight_Fall17V2.root", "EGamma_SF2D" ) )  
        outputfile.write( text3.format( year, "elRECO", info_path + "EGM2D_BtoH_GT20GeV_RecoSF_Legacy2016.root",    "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "muHLT" , info_path + "HLT16_mu_SF.root",                             "abseta_pt_ratio" ) )
        outputfile.write( text3.format( year, "muID"  , info_path + "muid_Run16All.root",                           "muid" ) )
        outputfile.write( text3.format( year, "muRECO", info_path + "muiso_Run16All.root",                          "muiso" ) )

    elif year == "17":
        #Event weight info
        outputfile.write( text4.format( year, "41540" ) )
        outputfile.write( text2.format( year, "BtagWeight", CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_94XSF_V4_B_F.csv" ) )

        #Scale factor weight info
        outputfile.write( text3.format( year, "elHLT" , info_path + "HLT_SF_17.root",                                   "abseta_pt_ratio" ) )  
        outputfile.write( text3.format( year, "elID"  , info_path + "2017_ElectronTight.root",                          "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "elRECO", info_path + "egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root",   "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "muHLT" , info_path + "HLT17_mu_SF.root",                                 "abseta_pt_ratio" ) )
        outputfile.write( text3.format( year, "muID"  , info_path + "RunBCDEF_SF_ID.root",                              "NUM_TightID_DEN_genTracks_pt_abseta" ) )
        outputfile.write( text3.format( year, "muRECO", info_path + "RunBCDEF_SF_ISO.root",                             "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta" ) )
    
    elif year == "18":
        #Event weight info
        outputfile.write( text4.format( year, "59690" ) )
        outputfile.write( text2.format( year, "BtagWeight", CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_102XSF_V1.csv" ) )

        #Scale factor weight info
        outputfile.write( text3.format( year, "elHLT" , info_path + "HLT_SF_18.root",                           "abseta_pt_ratio" ) )  
        outputfile.write( text3.format( year, "elID"  , info_path + "2018_ElectronTight.root",                  "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "elRECO", info_path + "egammaEffi.txt_EGM2D_updatedAll.root",     "EGamma_SF2D" ) )
        outputfile.write( text3.format( year, "muHLT" , info_path + "HLT18_mu_SF.root",                         "abseta_pt_ratio" ) )
        outputfile.write( text3.format( year, "muID"  , info_path + "RunABCD_SF_ID.root",                       "NUM_TightID_DEN_TrackerMuons_pt_abseta" ) )
        outputfile.write( text3.format( year, "muRECO", info_path + "RunABCD_SF_ISO.root",                      "NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta" ) )
    
def main(args):

    outputfile = open( "python/WeightInfo.py", "w+")
    outputfile.write( "import FWCore.ParameterSet.Config as cms\n" )
    MakeWeightInfo( outputfile, "16" )
    MakeWeightInfo( outputfile, "17" )
    MakeWeightInfo( outputfile, "18" )
    outputfile.close()

if __name__ == '__main__':
    main(sys.argv)
