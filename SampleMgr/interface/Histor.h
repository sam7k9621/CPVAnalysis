#ifndef SELECTOR_H
#define SELECTOR_H

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

        void AddSample( const string&, TChain* );

        /*******************************************************************************
        *   Common
        *******************************************************************************/
        std::string GetResultsName( const std::string&, const std::string& );
        std::string Discript( TH1* );
        void        Scale( const double x )            { _sample->Scale( x ); };
        TH1D*       Hist( const std::string& h )       { return _sample->Hist( h ); }
        const TH1D* Hist( const std::string& h ) const { return _sample->Hist( h ); }
        void        AddHist(
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


        float GetLepPt( int i )
        { 
            _sample->SetIndex( i );
            return _sample->LepPt();
        }

        float GetLepEta( int i )
        { 
            _sample->SetIndex( i );
            return _sample->LepEta();
        }

        float LeadingJetEta( int i )
        {
            _sample->SetIndex( i );
            return _sample->JetEta();
        }

        float LeadingJetPt( int i )
        {
            _sample->SetIndex( i );
            return _sample->JetPt();
        }

        /*******************************************************************************
        *   Looping event
        *******************************************************************************/
        void   process( const int&, const int& );
        void   ChangeFile( const std::string& );
        void   GetEntry( const int& );
        int    GetEntries(){ return _sample->GetEntries(); }
        TTree* CloneTree() { return _sample->CloneTree(); }

        /*******************************************************************************
        *   Weight
        *******************************************************************************/
        void   RegisterWeight(){ _sample->RegisterWeight(); }
        void   InitBtagWeight( const std::string&, const std::string& );
        float  GetPUWeight(){ return _sample->GetPUWeight(); }
        float  GetPUWeightUp(){ return _sample->GetPUWeightUp(); }
        float  GetPUWeightDn(){ return _sample->GetPUWeightDn(); }
        float  GenWeight()  { return _sample->GenWeight(); }
        double BtagScaleFactor( BTagEntry::OperatingPoint, const int& );
        double BtagScaleFactorDn( BTagEntry::OperatingPoint, const int& );
        double BtagScaleFactorUp( BTagEntry::OperatingPoint, const int& );
        double GetSF( TH2D*, const int& );
        double GetSFUp( TH2D*, const int& );
        double GetSFDn( TH2D*, const int& );
        TH2D*   GetSFHist( const std::string& );
        double TopPtWeight();
        void   WeightMC( const std::string& );

        /*******************************************************************************
        *   Basic RECO
        *******************************************************************************/
        TLorentzVector         GetLepP4( const int& i )       { return _sample->GetLepP4( i ); }
        TLorentzVector         GetJetP4( const int& i )       { return _sample->GetJetP4( i ); }
        TLorentzVector         GetGenP4( const int& i )       { return _sample->GetMCP4( i ); }
        int                    GetPdgID( const int& i ) { return _sample->GetPdgID( i ); }
        int                    GetGenJetIdx( const int& i )      { return _sample->MCTruthJet( i ); }
        int                    GetGenLepIdx( const int& i )      { return _sample->MCTruthLep( i ); }
        float                  nVtx()                         { return _sample->nVtx(); }
        float                  GetIsoLepCharge( const int& i ){ return _sample->GetLepCharge( i ); }
        BaseLineMgr::MatchType bbSeparation( const int& hb, const int& lb, const int& lep ){ return _sample->bbSeparation( hb, lb, lep ); }
        void                   DumpEvtInfo(){ return _sample->DumpEvtInfo(); }
        int                    GetGenJet( const int& i ){ return _sample->FindJet( i ); }
        int                    GetGenLepton(){ return _sample->FindLepton(); }
        int                    GetGenHardJet(){ return _sample->FindHardJet(); }

    private:

        BaseLineMgr* _sample;
};

#endif
