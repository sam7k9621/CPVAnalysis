#ifndef SELECTOR_H
#define SELECTOR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"

#include "CPVAnalysis/SampleMgr/interface/BaseLineMgr.h"

#include "TH1D.h"

class Selector : public mgr::Pathmgr,
                 public mgr::Readmgr,
                 public mgr::Parsermgr {
    public:

        /*******************************************************************************
        *   Initialization
        *******************************************************************************/
        Selector( const std::string&, const std::string& = "" );
        ~Selector();

        Selector( const Selector& )            = delete;
        Selector& operator=( const Selector& ) = delete;

        void AddSample( TChain* );

        /*******************************************************************************
        *   Common
        *******************************************************************************/
        std::string GetResultsName( const std::string&, const std::string& );
        std::string GetEOSName( const std::string&, const std::string&, const std::string& );
        std::string Discript( TH1* );
        bool        OptionContent( const std::string&, const std::string& );

        float LJetPt()
        {
            _sample->SetIndex( 0 );
            return _sample->JetPt();
        }

        float LLepPt()
        {
            _sample->SetIndex( 0 );
            return _sample->LepPt();
        }

        float GenWeight()                 { return _sample->GenWeight(); }
        float GetPUWeight()               { return _sample->GetPUWeight(); }
        float GetPUWeightUp()             { return _sample->GetPUWeightUp(); }
        float GetPUWeightDn()             { return _sample->GetPUWeightDn(); }
        void  RegisterWeight( TChain* ch ){ _sample->RegisterWeight( ch ); }
        void  InitJES()                   { _sample->InitJES(); }
        void  SetPDFUnc( float&, float& );
        void  SetMuFMuRUnc( float* );
        void  SetME_PSUnc( float&, float& );

        /*******************************************************************************
        *   Pre-selection
        *******************************************************************************/
        bool IsGoodEvt( checkEvtTool& );
        void JECCorrUp();
        void JECCorrDn();
        void JERCorr();
        void JERCorrUp();
        void JERCorrDn();
        bool PassVertex();
        bool PassHLT( const std::vector<int>& );
        bool PreJet();
        bool PreLep();
        void LeptonECorr();
        int  nPU(){ return _sample->nPU(); }

        /*******************************************************************************
        *   Full-selection
        *******************************************************************************/
        bool PassFullLepton( std::vector<int>&, const std::string& );
        bool PassFullLepton_CRQCD( std::vector<int>&, const std::string& );
        bool PassFullLepton_CRWJets( std::vector<int>&, const std::string& );

        bool                                      PassFullJet( std::vector<int>&, std::vector<int>&, const int& );
        bool                                      PassFullJet_CRQCD( std::vector<int>&, std::vector<int>&, const int& );
        bool                                      PassFullJet_CRWJets( std::vector<int>&, std::vector<int>&, const int& );
        std::tuple<double, double, int, int, int> GetChi2Info( const std::vector<int>&, const std::vector<int>& );
        double                                    GetLeptonicM( const int&, const int& );

        /*******************************************************************************
        *   Looping event
        *******************************************************************************/
        void process( const int&, const int& );
        void ChangeFile( const std::string& );

    private:

        BaseLineMgr* _sample;
};

#endif
