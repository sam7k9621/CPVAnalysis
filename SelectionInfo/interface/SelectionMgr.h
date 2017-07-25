#ifndef SELECTIONMGR_H
#define SELECTIONMGR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"

#include "CPVAnalysis/SelectionInfo/interface/BranchInfo.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include <vector>

namespace sel{

    class SelectionMgr : public dra::Pathmgr, public dra::Readmgr, public dra::Parsermgr{
        public :

/*******************************************************************************
*  Class initialization 
*******************************************************************************/
            SelectionMgr(const std::string&);
            ~SelectionMgr();
            void SetIndex(int);
            void SetRoot(TChain*);
            std::string GetFileName(const string&, const string&);
/*******************************************************************************
*   Common calculation
*******************************************************************************/
            bool   passHLT(std::vector<int>) ;
            double RelIsoR04();
            int    vsize();
            int    lsize();
            int    jsize();
            int    gsize();
            int    lep_type();
            int    runNO();
            int    lumiNO();

            std::vector<int> getSelJet(std::vector<int>, std::vector<int>);
            TLorentzVector getLorentzVector(std::string);
            TLorentzVector getMET(const TLorentzVector);
            double  Phi_mpi_pi(double);

/*******************************************************************************
*   MC Truth
*******************************************************************************/

            void    RvalueUP(const double&);
            int     matchGenlevel(const float&, const float&);
            int     getGenParticle(const int&);
            int     getGenParton(const int&);
            int     getDirectMother(int);
            int     getDirectMotherPdgID(const int&);
            bool    isXGenParticle(const int&, const int&);
            bool    isCommonMo(const int&, const int&, const int&);
            int     getPdgID(const int&);
            bool    checkPartonTopo();

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
            double Rvalue;
            VertexInfo vtx;
            LeptonInfo lep;
            EvtInfo    evt;
            JetInfo    jet;
            GenInfo    gen;

    };
}
#endif

