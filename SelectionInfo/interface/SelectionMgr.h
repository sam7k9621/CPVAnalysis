#ifndef SELECTIONMGR_H
#define SELECTIONMGR_H

#include "Selection/SelectionInfo/interface/BranchInfo.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include <vector>

namespace sel{

    class SelectionMgr {
        public :

/*******************************************************************************
*  Class initialization 
*******************************************************************************/
            
            SelectionMgr();
            ~SelectionMgr();
            void SetIndex(int);
            void SetRoot(TChain*); 
/*******************************************************************************
*   Common calculation
*******************************************************************************/
            bool   passHLT(std::vector<int>) ;
            double RelIsoR04();
            int    vsize();
            int    lsize();
            int    jsize();
            int    lep_type();
            std::vector<int> getSelJet(std::vector<int>, std::vector<int>);
            TLorentzVector getLorentzVector(std::string);

/*******************************************************************************
*   Vertex selection
*******************************************************************************/
    
            bool IsFake();
            bool IsOfflinePV();
            bool passNdof();
            bool passAbsZ();
            bool passRho();

/*******************************************************************************
*   Muon selection
*******************************************************************************/
    
            bool passMuPt(double);
            bool passMuEta(double);
            bool passMuRelIsoR04(double);
            bool MuInnerTrackDxy_PV();
            bool MuInnerTrackDz();
            bool MuNMuonhits();
            bool MuNMatchedStations();
            bool MuGlobalNormalizedChi2();
            bool MuNTrackLayersWMeasurement();
            bool MuNPixelLayers();
            bool isGlobalMuon();
            bool isTrackerMuon();
            
/*******************************************************************************
* Electron selection
*******************************************************************************/

            bool passElPt(double);
            bool passElEta(double);
            bool passElLoose();
            bool passElTight();

/*******************************************************************************
* Jet selection
*******************************************************************************/

            bool passJetPt(double);
            bool passJetEta(double);
            bool JetNConstituents();
            bool JetNEF();
            bool JetNHF();
            bool JetCHF();
            bool JetNCH();
            bool JetCEF();

            //b-tagging
            bool JetCSVM();

        private:
            int idx;
            VertexInfo vtx;
            LeptonInfo lep;
            EvtInfo    evt;
            JetInfo    jet;

    };
}
#endif

