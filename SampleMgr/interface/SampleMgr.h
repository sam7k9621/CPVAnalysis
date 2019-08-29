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
            SampleMgr();
            virtual ~SampleMgr();
            void SetIndex( const int& i ){ _idx = i; }
            void Register( TChain* );

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
            int   LHESize()                             { return _gen.LHESize; }
            int   LHESystematicId( const int& idx )     { return _gen.LHESystematicId[ idx ]; }
            float LHESystematicWeights( const int& idx ){ return _gen.LHESystematicWeights[ idx ]; }

            int nPU()
            {
                for( int i = 0; i < _evt.nBX; i++ ){
                    if( _evt.BXPU[ i ] == 0 ){
                        return _evt.TrueIT[ i ];
                    }
                }

                return -1;
            }

            float GenWeight(){ return _gen.Weight; }
            float EvtRho()   { return _evt.Rho; }

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
            float VtxRho();

            /*******************************************************************************
            *   Lep common info
            *******************************************************************************/
            float          LepPt();
            float          LepEta();
            float          LepAbsEta();
            float          GetLepCharge( const int& );
            TLorentzVector GetLepP4( const int& );
            void           SetLepP4( const TLorentzVector& );
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
            bool  ElIDCRLoose();
            bool  ElIDCRTight();
            float ElAbsTrackDz();
            float ElAbsTrackDxy_PV();

            float ElsigmaIetaIeta();
            float EldEtaInSeed();
            float EldPhiIn();
            float ElGsfEleHadronicOverEMCut();
            float GsfEleEInverseMinusPInverseCut();
            float ElNumberOfExpectedInnerHits();
            float ElPFISO();
            float EnergySC();
            bool  ElhasConv();
            void  ElEnergyCorr();

            /*******************************************************************************
            * Jet info
            *******************************************************************************/
            float          JesUnc();
            float          JERPt();
            float          JERScale();
            float          JERScale_up();
            float          JERScale_dn();
            float          JetPt();
            float          JetEta();
            float          JetAbsEta();
            float          JetPhi();
            float          GenJetPt();
            float          GenJetEta();
            float          GenJetPhi();
            int            GenJetFlavor();
            int            JetNConstituents();
            float          JetNEF();
            float          JetNHF();
            float          JetCHF();
            int            JetNCH();
            float          JetCEF();
            TLorentzVector GetJetP4( const int& );

            // Jet smeared
            // https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
            void SetJetP4( const TLorentzVector& );

            // b-tagging
            float JetCSV();
            float JetDeepCSV();
            /*******************************************************************************
            *   MC Truth
            *******************************************************************************/
            int            GetPdgID( const int& );
            int            GetGenJetID( const int& );
            int            MCTruthJet( const int& );
            int            MCTruthLep( const int& );
            int            MatchGenlevel( const float&, const float& );
            TLorentzVector GetMCP4( const int& );

            int  FindJet( const int& );
            int  FindLepton();
            int  FindHardJet();
            void DumpEvtInfo();

            int AvoidDuplicate( const int& );
            int GetDirectMo1( const int& );
            int GetDirectMo2( const int& );

        protected:

            int _idx;
            TrgInfoBranches _trg;
            VertexInfoBranches _vtx;
            LepInfoBranches _lep;
            EvtInfoBranches _evt;
            JetInfoBranches _jet;
            GenInfoBranches _gen;
    };
}

#endif
