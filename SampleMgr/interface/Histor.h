#ifndef HISTOR_H
#define HISTOR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"

#include "CPVAnalysis/SampleMgr/interface/BaseLineMgr.h"

#include "TH1D.h"

class Histor : public mgr::Pathmgr,
               public mgr::Readmgr,
               public mgr::Parsermgr {
    public:

        /*******************************************************************************
        *   Initialization
        *******************************************************************************/
        Histor( const std::string&, const std::string& );
        ~Histor();

        Histor( const Histor& )            = delete;
        Histor& operator=( const Histor& ) = delete;

        void AddSample( TChain* ch, const bool&, const std::string& );
        void AddSample( const std::string& );

        /*******************************************************************************
        *   Common
        *******************************************************************************/
        void                 InitBtagWeight( const std::string&, const std::string& );
        BTagEntry::JetFlavor GetBtagFlavor( const int& );
        double               GetBtagSF( const int& );
        bool                 PassHLT( const std::vector<int>& );
        bool                 PassISOLepton( const int&, const int& );
        bool                 PassFullLepton_CRWJets( const string& );
        std::string          GetResultsName( const std::string&, const std::string& );
        std::string          Discript( TH1* );
        void                 Scale( const double x )            { _sample->Scale( x ); }
        void                 Scale2D( const double x )            { _sample->Scale2D( x ); }
        TH1D*                Hist( const std::string& h )       { return _sample->Hist( h ); }
        const TH1D*          Hist( const std::string& h ) const { return _sample->Hist( h ); }
        void                 AddHist(
            const std::string&,
            const std::string&,
            const std::string&,
            const int,
            const double,
            const double
            );

        TH2D*       Hist2D( const std::string& h )       { return _sample->Hist2D( h ); }
        const TH2D* Hist2D( const std::string& h ) const { return _sample->Hist2D( h ); }
        void        AddHist2D(
            const std::string&,
            const std::string&,
            const std::string&,
            const unsigned,
            const double,
            const double,
            const unsigned,
            const double,
            const double
            );

        float GetLepPt( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->LepPt();
        }

        float GetLepEta( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->LepEta();
        }

        float GetJetEta( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->JetEta();
        }

        float GetJetPt( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->JetPt();
        }

        float GetJetCSV( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->JetDeepCSV();
        }

        float GetJetFlavor( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->GenJetFlavor();
        }

        float GetMuISO( const int& );
        float GetElISO( const int& );

        /*******************************************************************************
        *   Looping event
        *******************************************************************************/
        void         process( const int&, const int& );
        BaseLineMgr* GetSample(){ return _sample; }

        /*******************************************************************************
        *   Weight
        *******************************************************************************/
        void   RegisterWeight( TChain* ch ){ _sample->RegisterWeight( ch ); }
        float  GetPUWeight()               { return _sample->GetPUWeight(); }
        float  GetPUWeightUp()             { return _sample->GetPUWeightUp(); }
        float  GetPUWeightDn()             { return _sample->GetPUWeightDn(); }
        float  GenWeight()                 { return _sample->GenWeight(); }
        float  GetISRup()                  { return _sample->GetISRup(); }
        float  GetISRdn()                  { return _sample->GetISRdn(); }
        float  GetFSRup()                  { return _sample->GetFSRup(); }
        float  GetFSRdn()                  { return _sample->GetFSRdn(); }
        double GetLepSF( TH2D*, const int& );
        double GetLepSFUp( TH2D*, const int& );
        double GetLepSFDn( TH2D*, const int& );
        TH2D*  GetSFHist( const std::string& );
        double TopPtWeight(){ return _sample->TopPtWeight(); }
        void   WeightMC( const std::string& );
        float  PDFWeight( const int& );
        float  muFmuRWeight( const int& );

        /*******************************************************************************
        *   Basic RECO
        *******************************************************************************/
        TLorentzVector         GetLepP4( const int& i )                                    { return _sample->GetLepP4( i ); }
        TLorentzVector         GetJetP4( const int& i )                                    { return _sample->GetJetP4( i ); }
        TLorentzVector         GetGenP4( const int& i )                                    { return _sample->GetMCP4( i ); }
        int                    GetPdgID( const int& i )                                    { return _sample->GetPdgID( i ); }
        int                    GetGenJetIdx( const int& i )                                { return _sample->MCTruthJet( i ); }
        int                    GetGenLepIdx( const int& i )                                { return _sample->MCTruthLep( i ); }
        int                    nVtx()                                                      { return _sample->nVtx(); }
        float                  Rho()                                                       { return _sample->Rho(); }
        float                  GetIsoLepCharge( const int& i )                             { return _sample->GetLepCharge( i ); }
        BaseLineMgr::MatchType bbSeparation( const int& hb, const int& lb, const int& lep ){ return _sample->bbSeparation( hb, lb, lep ); }
        void                   DumpEvtInfo()                                               { return _sample->DumpEvtInfo(); }
        int                    GetGenJet( const int& i )                                   { return _sample->FindJet( i ); }
        int                    GetGenLepton()                                              { return _sample->FindLepton(); }
        int                    GetGenHardJet()                                             { return _sample->FindHardJet(); }
        int                    GetLepMo1ID( const int& i )                                 { return _sample->GetLepMo1ID( i ); }

    private:
        BaseLineMgr* _sample;
};

#endif
