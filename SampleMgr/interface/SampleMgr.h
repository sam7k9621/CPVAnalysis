#ifndef SAMPLEMGR_H
#define SAMPLEMGR_H

#include "CPVAnalysis/SampleMgr/interface/BranchInfo.h"
#include "TChain.h"
#include "TLorentzVector.h"

#include <string>
#include <vector>

namespace mgr{
    class SampleMgr {
        public:

            /*******************************************************************************
            *  Class initialization
            *******************************************************************************/
            SampleMgr( TChain* );
            virtual ~SampleMgr();
            void SetIndex( const int& i ){ _idx = i; }

            /*******************************************************************************
            *  Return file info
            *******************************************************************************/
            int Vsize() { return _vtx.Size; }
            int Lsize() { return _lep.Size; }
            int Jsize() { return _jet.Size; }
            int Gsize() { return _gen.Size; }
            int RunNo() { return _evt.RunNo; }
            int LumiNo(){ return _evt.LumiNo; }

            // weight
            int   nPU()      { return _evt.nPU[ 0 ]; }
            float GenWeight(){ return _gen.Weight; }

            /*******************************************************************************
            *   Basic RECO
            *******************************************************************************/
            double         RelIsoR04();
            double         Phi_mpi_pi( double );
            bool           IsIsoLepton( const int&, const int& );
            TLorentzVector GetMET( const TLorentzVector& );

            /*******************************************************************************
            *   Vertex & HLT info
            *******************************************************************************/
            bool  PassHLT( const std::vector<int>& );
            bool  IsFake();
            float Ndof();
            float AbsZ();
            float Rho();

            /*******************************************************************************
            *   Lep common info
            *******************************************************************************/
            float          LepPt();
            float          LepEta();
            float          LepAbsEta();
            float          GetLepCharge( const int& );
            TLorentzVector GetLepP4( const int& );
            int            Lep_Type(){ return _lep.LeptonType[ _idx ]; }

            /*******************************************************************************
            *   Muon info
            *******************************************************************************/
            float AbsMuInnerTrackDxy_PV();
            float AbsMuInnerTrackDz();
            int   MuNMuonhits();
            int   MuNMatchedStations();
            float MuGlobalNormalizedChi2();
            int   MuNTrackLayersWMeasurement();
            int   MuNPixelLayers();
            bool  IsPFMuon();
            bool  IsGlobalMuon();
            bool  IsTrackerMuon();

            /*******************************************************************************
            * Electron info
            *******************************************************************************/
            bool  ElIDLoose();
            bool  ElIDTight();
            float ElAbsTrackDz();
            float ElAbsTrackDxy_PV();
            float ElsigmaIetaIeta();
            float EldEtaInSeed();
            float EldPhiIn();
            float ElGsfEleHadronicOverEMCut();
            float GsfEleEInverseMinusPInverseCut();
            float ElNumberOfExpectedInnerHits();
            bool ElhasConv();
            /*******************************************************************************
            * Jet info
            *******************************************************************************/
            float          JERPt();
            float          JERScale();
            float          JetPt();
            float          JetEta();
            float          JetAbsEta();
            float          JetPhi();
            float          GenJetPt();
            float          GenJetEta();
            float          GenJetPhi();
            int            JetNConstituents();
            float          JetNEF();
            float          JetNHF();
            float          JetCHF();
            int            JetNCH();
            float          JetCEF();
            TLorentzVector GetJetP4( const int& );

            // Jet smeared
            // https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
            void SetJetPtEta( const float&, const float& );

            // b-tagging
            float JetCSV();

            /*******************************************************************************
            *   MC Truth
            *******************************************************************************/
            int            GetPartonID( const int& );
            int            MCTruthJet( const int& );
            int            MatchGenlevel( const float&, const float& );
            TLorentzVector GetGenJetP4( const int& );

        private:

            int _idx;
            VertexInfo _vtx;
            LeptonInfo _lep;
            EvtInfo _evt;
            JetInfo _jet;
            GenInfo _gen;
    };
}

#endif
