#ifndef SELECTOR_H
#define SELECTOR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"

#include "CPVAnalysis/BaseLineSelector/interface/BaseLineMgr.h"

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
        float  GenWeight()  { return _sample->GenWeight(); }
        double BtagScaleFactor( BTagEntry::OperatingPoint, const int& );
        double GetSF( TH2D*, const int& );
        TH2D*   GetSFHist( const std::string& );
        void   WeightMC( const std::string& );

        /*******************************************************************************
        *   Basic RECO
        *******************************************************************************/
        TLorentzVector         GetLepP4( const int& i )       { return _sample->GetLepP4( i ); }
        TLorentzVector         GetJetP4( const int& i )       { return _sample->GetJetP4( i ); }
        float                  nVtx()                         { return _sample->nVtx(); }
        float                  GetIsoLepCharge( const int& i ){ return _sample->GetLepCharge( i ); }
        BaseLineMgr::MatchType bbSeparation( const int&, const int&, const int& );

    private:

        BaseLineMgr* _sample;
};

#endif
