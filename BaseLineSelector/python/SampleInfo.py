import os
import FWCore.ParameterSet.Config as cms
from CPVAnalysis.BaseLineSelector.makeInput import MakeInput
import CPVAnalysis.BaseLineSelector.Samplelst16 as s16
import CPVAnalysis.BaseLineSelector.Samplelst17 as s17
import CPVAnalysis.BaseLineSelector.Samplelst18 as s18

##############################################################################################################################################################
CMSSW_BASE =  os.environ['CMSSW_BASE']
sample16 = cms.PSet()
sample17 = cms.PSet()
sample18 = cms.PSet()

##############################################################################################################################################################
MakeInput( 
        sample16, "16",
        "/wk_cms2/youying/public/bprimekit_2016_Legacy/",
        s16.mclst, s16.datalst 
        )

sample16.Info = cms.PSet(
        mu_HLT = cms.vint32( 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324 ),
        el_HLT = cms.vint32( 917, 918, 919, 920, 921, 922, 923 ),
        lumimask = cms.string   ( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data//src/CPVAnalysis/BaseLineSelector/data/Cert_271036-284044_13TeV_ReReco_07Aug2017_Collisions16_JSON.txt"),
        puweight = cms.string   ( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_69200.csv"),
        puweight_up = cms.string( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_72400.csv"),
        puweight_dn = cms.string( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_66000.csv")
        )

##############################################################################################################################################################
MakeInput( 
        sample17, "17",
        "/wk_cms2/youying/public/bprimekit_2017_ReMiniAODv2/",
        s17.mclst, s17.datalst 
        )

sample17.Info = cms.PSet(
        mu_HLT = cms.vint32( 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344),
        el_HLT = cms.vint32( 938, 939, 940, 941, 942, 943, 944 ),
        lumimask = cms.string   ( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt"),
        puweight = cms.string   ( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_69200.csv"),
        puweight_up = cms.string( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_72400.csv"),
        puweight_dn = cms.string( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_66000.csv")
        )

##############################################################################################################################################################
MakeInput(
        sample18, "18",
        "/wk_cms2/youying/public/bprimekit_2018_ReReco/",
        s18.mclst, s18.datalst 
        )

sample18.Info = cms.PSet(
        mu_HLT = cms.vint32( 200, 201, 202 ),
        el_HLT = cms.vint32( 116, 117, 118 ),
        lumimask = cms.string   ( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.txt"),
        puweight = cms.string   ( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_69200.csv"),
        puweight_up = cms.string( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_72400.csv"),
        puweight_dn = cms.string( CMSSW_BASE + "/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_66000.csv")
        )
