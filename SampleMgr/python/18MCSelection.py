import FWCore.ParameterSet.Config as cms
import sys

process = cms.PSet()

process.PV = cms.PSet(
        Ndof = cms.double( 4 ),
        AbsZ = cms.double( 24),
        VtxRho = cms.double( 2 )
        )

process.Jet = cms.PSet(
        JerUpdate  = cms.bool( True ),
        Jesfile    = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V7b_MC_PtResolution_AK4PFchs.txt" ),
        Jerfile    = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V7b_MC_SF_AK4PFchs.txt" ),

        JecUpdate  = cms.bool( True ),
        Resfile    = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V19_MC_L2L3Residual_AK4PFchs.txt" ),     
        L1file     = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V19_MC_L1FastJet_AK4PFchs.txt" ),
        L2file     = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V19_MC_L2Relative_AK4PFchs.txt" ),
        L3file     = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V19_MC_L3Absolute_AK4PFchs.txt" ),
        JecUncfile = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V19_MC_Uncertainty_AK4PFchs.txt" ),
        
        JecSrcUpdate  = cms.bool( True ),
        JecSrcfile = cms.string( "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/data/Autumn18_V19_MC_UncertaintySources_AK4PFchs.txt" ),
        
        jetid      = cms.string( "tight" ),
        jetid_test = cms.string( "loose" ), 

        loose_NHF = cms.double( 0.99 ),
        loose_NEF = cms.double( 0.99 ),
        loose_NConstituents = cms.double( 1 ),
        loose_CHF = cms.double( 0 ),
        loose_NCH = cms.double( 0 ),
        loose_CEF = cms.double( 0.99 ),
        
        tight_NHF = cms.double( 0.9 ),
        tight_NEF = cms.double( 0.9 ),
        tight_NConstituents = cms.double( 1 ),
        tight_CHF = cms.double( 0 ),
        tight_NCH = cms.double( 0 ),
        tight_CEF = cms.double( sys.maxint ),
        
        AbsEta = cms.double( 2.4 ),
        Pt = cms.double( 30 ),
        Pt_CRDYJets = cms.double( 30 ),
        Pre_AbsEta = cms.double( 2.4  ),
        Pre_Pt = cms.double( 20 ),
        
        CSVL = cms.double( 0.5803 ),
        CSVM = cms.double( 0.8838 ),
        DeepCSVL = cms.double( 0.1241 ),
        DeepCSVM = cms.double( 0.4184 )
        )
    
process.Mu = cms.PSet(
        GlobalNormalizedChi2 = cms.double( 10 ),
        MuNMuonhits = cms.double( 0 ),
        MuNMatchedStations = cms.double( 1 ),
        AbsMuInnerTrackDxy_PV = cms.double( 0.2 ),
        AbsMuInnerTrackDz = cms.double( 0.5 ),
        MuNPixelLayers = cms.double( 0 ),
        MuNTrackLayersWMeasurement = cms.double( 5 ),
        
        LooseISO = cms.double( 0.25 ),
        LoosePt = cms.double( 15 ),
        LooseAbsEta = cms.double( 2.4 ),
        TightISO = cms.double( 0.15 ),
        TightPt = cms.double( 30 ),
        TightAbsEta = cms.double( 2.4 )
        )
    
process.El = cms.PSet(
        LoosePt = cms.double( 15 ),
        LooseAbsEta = cms.double( 2.4 ),
        TightPt = cms.double( 38 ),
        TightAbsEta = cms.double( 2.4 ),
        
        Tight_Bar_ISOc0 = cms.double( 0.0287 ),
        Tight_Bar_ISOcpt = cms.double( 0.506 ),
        Tight_End_ISOc0 = cms.double( 0.0445 ),
        Tight_End_ISOcpt = cms.double( 0.963 ),
        
        Loose_Bar_ElsigmaIetaIeta = cms.double( 0.0112 ),
        Loose_Bar_EldEtaInSeed = cms.double( 0.00377 ),
        Loose_Bar_EldPhiIn = cms.double( 0.0884 ),
        Loose_Bar_HE_c0 = cms.double( 0.05 ),
        Loose_Bar_HE_cE = cms.double( 1.16 ),
        Loose_Bar_HE_cR = cms.double( 0.0324 ),
        Loose_Bar_GsfEleEInverseMinusPInverseCut = cms.double( 0.193 ),
        Loose_Bar_ElNumberOfExpectedInnerHits = cms.double( 1 ),
        
        Loose_End_ElsigmaIetaIeta = cms.double( 0.0425 ),
        Loose_End_EldEtaInSeed = cms.double( 0.00674 ),
        Loose_End_EldPhiIn = cms.double( 0.169 ),
        Loose_End_HE_c0 = cms.double( 0.0441 ),
        Loose_End_HE_cE = cms.double( 2.54 ),
        Loose_End_HE_cR = cms.double( 0.183 ),
        Loose_End_GsfEleEInverseMinusPInverseCut = cms.double( 0.111 ),
        Loose_End_ElNumberOfExpectedInnerHits = cms.double( 1 ),
        
        Tight_Bar_ElsigmaIetaIeta =cms.double( 0.0104 ),
        Tight_Bar_EldEtaInSeed = cms.double( 0.00255 ),
        Tight_Bar_EldPhiIn = cms.double( 0.022 ),
        Tight_Bar_HE_c0 = cms.double( 0.026 ),
        Tight_Bar_HE_cE = cms.double( 1.15 ),
        Tight_Bar_HE_cR = cms.double( 0.0324 ),
        Tight_Bar_GsfEleEInverseMinusPInverseCut = cms.double( 0.159 ),
        Tight_Bar_ElNumberOfExpectedInnerHits = cms.double( 1 ),
        
        Tight_End_ElsigmaIetaIeta = cms.double( 0.0353 ),
        Tight_End_EldEtaInSeed = cms.double( 0.00501 ),
        Tight_End_EldPhiIn = cms.double( 0.0236 ),
        Tight_End_HE_c0 = cms.double( 0.0188 ),
        Tight_End_HE_cE = cms.double( 2.06 ),
        Tight_End_HE_cR = cms.double( 0.183 ),
        Tight_End_GsfEleEInverseMinusPInverseCut =cms.double( 0.0197 ),
        Tight_End_ElNumberOfExpectedInnerHits = cms.double( 1 )
    )