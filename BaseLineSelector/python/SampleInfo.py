import FWCore.ParameterSet.Config as cms
sample16 = cms.PSet()
setattr( sample16, "BtagWeight", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_2016LegacySF_V1.csv" ) ) )
setattr( sample16, "SR_eff_b", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt6321.root" ), title = cms.string( "eff_b" ) ) )
setattr( sample16, "SR_eff_c", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt6321.root" ), title = cms.string( "eff_c" ) ) )
setattr( sample16, "SR_eff_l", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt6321.root" ), title = cms.string( "eff_l" ) ) )
setattr( sample16, "CR_eff_b", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt2217.root" ), title = cms.string( "eff_b" ) ) )
setattr( sample16, "CR_eff_c", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt2217.root" ), title = cms.string( "eff_c" ) ) )
setattr( sample16, "CR_eff_l", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt2217.root" ), title = cms.string( "eff_l" ) ) )

sample16.Info = cms.PSet(
        mu_HLT = cms.vint32( 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 2412, 2413, 2414, 2415 ),
        el_HLT = cms.vint32( 917, 918, 919, 920, 921, 922, 923 ),
        lumimask = cms.string   ( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/Cert_271036-284044_13TeV_ReReco_07Aug2017_Collisions16_JSON.txt" ),
        puweight = cms.string   ( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_69200.csv" ),
        puweight_up = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_72400.csv" ),
        puweight_dn = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_16_66000.csv" )
        )
sample17 = cms.PSet()
setattr( sample17, "BtagWeight", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_94XSF_V4_B_F.csv" ) ) )
setattr( sample17, "SR_eff_b", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4941.root" ), title = cms.string( "eff_b" ) ) )
setattr( sample17, "SR_eff_c", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4941.root" ), title = cms.string( "eff_c" ) ) )
setattr( sample17, "SR_eff_l", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4941.root" ), title = cms.string( "eff_l" ) ) )
setattr( sample17, "CR_eff_b", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1522.root" ), title = cms.string( "eff_b" ) ) )
setattr( sample17, "CR_eff_c", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1522.root" ), title = cms.string( "eff_c" ) ) )
setattr( sample17, "CR_eff_l", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1522.root" ), title = cms.string( "eff_l" ) ) )

sample17.Info = cms.PSet(
        mu_HLT = cms.vint32( 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344 ),
        el_HLT = cms.vint32( 938, 939, 940, 941, 942, 943, 944 ),
        lumimask = cms.string   ( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt" ),
        puweight = cms.string   ( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_69200.csv" ),
        puweight_up = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_72400.csv" ),
        puweight_dn = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_17_66000.csv" )
        )
sample18 = cms.PSet()
setattr( sample18, "BtagWeight", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/DeepCSV_102XSF_V1.csv" ) ) )
setattr( sample18, "SR_eff_b", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4184.root" ), title = cms.string( "eff_b" ) ) )
setattr( sample18, "SR_eff_c", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4184.root" ), title = cms.string( "eff_c" ) ) )
setattr( sample18, "SR_eff_l", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_TTToSemiLeptonic_0pt4184.root" ), title = cms.string( "eff_l" ) ) )
setattr( sample18, "CR_eff_b", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1241.root" ), title = cms.string( "eff_b" ) ) )
setattr( sample18, "CR_eff_c", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1241.root" ), title = cms.string( "eff_c" ) ) )
setattr( sample18, "CR_eff_l", cms.PSet( path = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/beffPlot_WJets_0pt1241.root" ), title = cms.string( "eff_l" ) ) )

sample18.Info = cms.PSet(
        mu_HLT = cms.vint32( 200, 201, 202 ),
        el_HLT = cms.vint32( 116, 117, 118 ),
        lumimask = cms.string   ( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.txt" ),
        puweight = cms.string   ( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_69200.csv" ),
        puweight_up = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_72400.csv" ),
        puweight_dn = cms.string( "/afs/cern.ch/work/p/pusheng/CMSSW_10_2_14/src/CPVAnalysis/BaseLineSelector/data/pileupweights_18_66000.csv" )
        )
