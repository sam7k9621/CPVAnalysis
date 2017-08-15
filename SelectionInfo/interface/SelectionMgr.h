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
            void SetIndex(const int&);
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

            TLorentzVector getMET(const TLorentzVector);
            double  Phi_mpi_pi(double);
            bool isIsoLepton(const int&, const int&);
            TLorentzVector getLorentzVector(const std::string&, const int&);
            TLorentzVector getLorentzLep(const int&);
            std::vector<TLorentzVector> getLorentzJet(const std::vector<int>&);

/*******************************************************************************
*   MC Truth
*******************************************************************************/

            void    RvalueUP(const double&);
            void    getRvalue();
            int     matchGenlevel(const float&, const float&);
            int     getGenParticle();
            int     getGenParton();
            int     getDirectMother(int);
            int     getDirectDa1   (int);
            int     getDirectDa2   (int);
            int     getDirectMotherPdgID(const int&);
            bool    isXGenParticle(const int&, const int&);
            bool    isCommonMo(const int&, const int&, const int&);
            int     getPdgID(const int&);
            bool    checkPartonTopo();
            void    cleanHandle();
            void    showJetInfo(const int&);
            void    showGenInfo(const int&);

            int     bbarDeltaR(const int&, const int&);
            int     getMuonCharge(const int&);
            int     matchBhandle(const int&, const int&);
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
    
            bool passMuPt(const double&);
            bool passMuEta(const double&);
            bool passMuRelIsoR04(const double&);
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

            bool passElPt(const double&);
            bool passElEta(const double&);
            bool passElIDLoose();
            bool passElIDTight();

/*******************************************************************************
* Jet selection
*******************************************************************************/

            bool passJetPt(const double&);
            bool passJetEta(const double&);
            bool JetNConstituents();
            bool JetNEF();
            bool JetNHF();
            bool JetCHF();
            bool JetNCH();
            bool JetCEF();

            //b-tagging
            bool JetCSVM(const double&);

        private:
            int _idx;
            double _rvalue;
            VertexInfo vtx;
            LeptonInfo lep;
            EvtInfo    evt;
            JetInfo    jet;
            GenInfo    gen;

            std::vector<int> bhandle;
    };
}
#endif

