#include "CPVAnalysis/MassReco/interface/MassReco.h"

using namespace sel;

/*******************************************************************************
 * Jet 
*******************************************************************************/

extern bool passJet(){

    return(
            //Kinematic cut
            SelMgr().passJetPt(30) *
            SelMgr().passJetEta(2.4) *
            //Loose ID
            SelMgr().JetNConstituents() *
            SelMgr().JetNEF() *
            SelMgr().JetNHF() *
            SelMgr().JetCHF() *
            SelMgr().JetNCH() *
            SelMgr().JetCEF() 
          );
}

extern bool passBJet(){

    return(
            SelMgr().JetCSVM()
          );
}

/*******************************************************************************
 * Muon ID 
*******************************************************************************/

extern bool passMuLoose(){
    
    return(
            //Kinematic cut
            SelMgr().passMuPt(15) *
            SelMgr().passMuEta(2.4) *
            //Isolation cut
            SelMgr().passMuRelIsoR04(0.25) *
            //Loose ID
            (SelMgr().isGlobalMuon() || SelMgr().isTrackerMuon()) 
          );
}

extern bool passMuTight(){

    return(
            //Kinematic cut
            SelMgr().passMuPt(30) *
            SelMgr().passMuEta(2.1) *
            //Isolation cut
            SelMgr().passMuRelIsoR04(0.15) *
            //Tight ID
            SelMgr().MuInnerTrackDxy_PV() *
            SelMgr().MuInnerTrackDz() *
            SelMgr().MuNMuonhits() *
            SelMgr().MuNMatchedStations() *
            SelMgr().MuGlobalNormalizedChi2() *
            SelMgr().MuNTrackLayersWMeasurement() *
            SelMgr().MuNPixelLayers() *
            SelMgr().isGlobalMuon()
          );
}

/*******************************************************************************
 * Electron ID 
*******************************************************************************/

extern bool passElLoose(){
    
    return(
            //Kinematic
            SelMgr().passElPt(15) *
            SelMgr().passElEta(2.4) *
            //Loose ID
            SelMgr().passElIDLoose()
          );
}

/*******************************************************************************
 * Vertex 
*******************************************************************************/

extern bool passVertex(){

    return(
            !(SelMgr().IsFake()) * 
            SelMgr().passNdof() * 
            SelMgr().passAbsZ() *
            SelMgr().passRho() 
          );
}


