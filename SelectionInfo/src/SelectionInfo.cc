#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
using namespace sel;
using namespace std;

/*******************************************************************************
*   Global function
*******************************************************************************/
extern SelectionMgr& SelMgr(const string& subdir){

    static SelectionMgr selmgr(subdir);
    return selmgr;
}

extern void process(int total, int progress){
    
    printf("[%d|%d]\r",total,progress);
    fflush(stdout);
}

/*******************************************************************************
*   SelectionInfo
*******************************************************************************/

/* HLT */

extern bool passHLT(const vector<int>& hlt){

    return SelMgr().passHLT(hlt);
}

/* Vertex */

extern bool passVertex(){

    for(int i=0;i<SelMgr().vsize();i++){
        SelMgr().SetIndex(i);

        if( passGoodPVtx() ){
            return true;
        }
    }

    return false;
}

extern bool passGoodPVtx(){

    return(
            !(SelMgr().IsFake()) *
            SelMgr().passNdof() *
            SelMgr().passAbsZ() *
            SelMgr().passRho()
          );
}

/* Jet */

extern bool preJet(){

    return (SelMgr().jsize() >= 4);
}

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

/* Muon */

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

/* Electron */

extern bool passElLoose(){

    return(
            //Kinematic
            SelMgr().passElPt(15) *
            SelMgr().passElEta(2.4) *
            //Loose ID
            SelMgr().passElIDLoose()
          );
}
