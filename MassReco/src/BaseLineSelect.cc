#include "CPVAnalysis/MassReco/interface/MassReco.h"

using namespace sel;

/*******************************************************************************
 * Jet 
*******************************************************************************/

extern bool passJet(){

    return(
            //Kinematic cut
            smgr.passJetPt(30) *
            smgr.passJetEta(2.4) *
            //Loose ID
            smgr.JetNConstituents() *
            smgr.JetNEF() *
            smgr.JetNHF() *
            smgr.JetCHF() *
            smgr.JetNCH() *
            smgr.JetCEF() 
          );
}

extern bool passBJet(){

    return(
            smgr.JetCSVM()
          );
}

/*******************************************************************************
 * Muon ID 
*******************************************************************************/

extern bool passMuLoose(){
    
    return(
            //Kinematic cut
            smgr.passMuPt(10) *
            smgr.passMuEta(2.5) *
            //Isolation cut
            smgr.passMuRelIsoR04(0.2) *
            //Loose ID
            (smgr.isGlobalMuon() || smgr.isTrackerMuon()) 
          );
}

extern bool passMuTight(){

    return(
            //Kinematic cut
            smgr.passMuPt(26) *
            smgr.passMuEta(2.1) *
            //Isolation cut
            smgr.passMuRelIsoR04(0.12) *
            //Tight ID
            smgr.MuInnerTrackDxy_PV() *
            smgr.MuInnerTrackDz() *
            smgr.MuNMuonhits() *
            smgr.MuNMatchedStations() *
            smgr.MuGlobalNormalizedChi2() *
            smgr.MuNTrackLayersWMeasurement() *
            smgr.MuNPixelLayers() *
            smgr.isGlobalMuon()
          );
}

/*******************************************************************************
 * Electron ID 
*******************************************************************************/

extern bool passElLoose(){
    
    return(
            //Kinematic
            smgr.passElPt(20) *
            smgr.passElEta(2.5) *
            //Loose ID
            smgr.passElIDLoose()
          );
}

/*******************************************************************************
 * Vertex 
*******************************************************************************/

extern bool passVertex(){

    return(
            !(smgr.IsFake()) * 
            smgr.passNdof() * 
            smgr.passAbsZ() *
            smgr.passRho() 
          );
}


