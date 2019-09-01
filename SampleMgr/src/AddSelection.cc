#include "CPVAnalysis/SampleMgr/interface/BaseLineMgr.h"
#include <climits>
#include <iostream>
#include <string>
using namespace std;
using namespace mgr;

/*******************************************************************************
*   Class initialization
*******************************************************************************/
void 
BaseLineMgr::ReadConfig()
{
    AddVal( "PV", "Ndof" );
    AddVal( "PV", "AbsZ" );
    AddVal( "PV", "VtxRho" );
    
    AddVal( "Jet", "NHF" );
    AddVal( "Jet", "NEF" );
    AddVal( "Jet", "NConstituents" );
    AddVal( "Jet", "CHF" );
    AddVal( "Jet", "NCH" );
    AddVal( "Jet", "CEF" );
    
    AddVal( "Jet", "AbsEta" );
    AddVal( "Jet", "Pt" );
    AddVal( "Jet", "Pre_AbsEta" );
    AddVal( "Jet", "Pre_Pt" );
    
    AddVal( "Jet", "CSVL" );
    AddVal( "Jet", "CSVM" );
    AddVal( "Jet", "DeepCSVL" );
    AddVal( "Jet", "DeepCSVM" );
    
    AddVal( "Mu", "GlobalNormalizedChi2" );
    AddVal( "Mu", "MuNMuonhits" );
    AddVal( "Mu", "MuNMatchedStations" );
    AddVal( "Mu", "AbsMuInnerTrackDxy_PV" );
    AddVal( "Mu", "AbsMuInnerTrackDz" );
    AddVal( "Mu", "MuNPixelLayers" );
    AddVal( "Mu", "MuNTrackLayersWMeasurement" );
    
    AddVal( "Mu", "LooseISO" );
    AddVal( "Mu", "LoosePt" );
    AddVal( "Mu", "LooseAbsEta" );
    AddVal( "Mu", "TightISO" );
    AddVal( "Mu", "TightPt" );
    AddVal( "Mu", "TightAbsEta" );
    
    AddVal( "El", "LoosePt" );
    AddVal( "El", "LooseAbsEta" );
    AddVal( "El", "TightPt" );
    AddVal( "El", "TightAbsEta" );

    AddVal( "El", "Tight_Bar_ISOc0" );
    AddVal( "El", "Tight_Bar_ISOcpt" );
    AddVal( "El", "Tight_End_ISOc0" );
    AddVal( "El", "Tight_End_ISOcpt" );
    
    AddVal( "El", "Loose_Bar_ElsigmaIetaIeta" );
    AddVal( "El", "Loose_Bar_EldEtaInSeed" );
    AddVal( "El", "Loose_Bar_EldPhiIn" );
    AddVal( "El", "Loose_Bar_HE_c0" );
    AddVal( "El", "Loose_Bar_HE_cE" );
    AddVal( "El", "Loose_Bar_HE_cR" );
    AddVal( "El", "Loose_Bar_GsfEleEInverseMinusPInverseCut" );
    AddVal( "El", "Loose_Bar_ElNumberOfExpectedInnerHits" );
   
    AddVal( "El", "Loose_End_ElsigmaIetaIeta" );
    AddVal( "El", "Loose_End_EldEtaInSeed" );
    AddVal( "El", "Loose_End_EldPhiIn" );
    AddVal( "El", "Loose_End_HE_c0" );
    AddVal( "El", "Loose_End_HE_cE" );
    AddVal( "El", "Loose_End_HE_cR" );
    AddVal( "El", "Loose_End_GsfEleEInverseMinusPInverseCut" );
    AddVal( "El", "Loose_End_ElNumberOfExpectedInnerHits" );
    
    AddVal( "El", "Tight_Bar_ElsigmaIetaIeta" );
    AddVal( "El", "Tight_Bar_EldEtaInSeed" );
    AddVal( "El", "Tight_Bar_EldPhiIn" );
    AddVal( "El", "Tight_Bar_HE_c0" );
    AddVal( "El", "Tight_Bar_HE_cE" );
    AddVal( "El", "Tight_Bar_HE_cR" );
    AddVal( "El", "Tight_Bar_GsfEleEInverseMinusPInverseCut" );
    AddVal( "El", "Tight_Bar_ElNumberOfExpectedInnerHits" );
   
    AddVal( "El", "Tight_End_ElsigmaIetaIeta" );
    AddVal( "El", "Tight_End_EldEtaInSeed" );
    AddVal( "El", "Tight_End_EldPhiIn" );
    AddVal( "El", "Tight_End_HE_c0" );
    AddVal( "El", "Tight_End_HE_cE" );
    AddVal( "El", "Tight_End_HE_cR" );
    AddVal( "El", "Tight_End_GsfEleEInverseMinusPInverseCut" );
    AddVal( "El", "Tight_End_ElNumberOfExpectedInnerHits" );
}
