import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        "file:/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/CheckGenJet/test/0693E0E7-97BE-E611-B32F-0CC47A78A3D8.root"
        )
)

process.demo = cms.EDAnalyzer(
    'CheckGenJet',
    jetsrc = cms.InputTag("slimmedJets")
)


process.p = cms.Path(process.demo)
