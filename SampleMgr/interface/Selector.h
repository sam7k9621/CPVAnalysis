#ifndef SELECTOR_H
#define SELECTOR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"

#include "CPVAnalysis/SampleMgr/interface/BaseLineMgr.h"

#include "TEfficiency.h"
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

        void AddSample( TChain*, const bool&, const std::string& );
        void InitBtagEffPlot( TH2D*, TH2D*, TH2D* );

        /*******************************************************************************
        *   Common
        *******************************************************************************/
        bool        IsGrid5();
        bool        IsLxplus();
        std::string GetResultsName( const std::string&, const std::string&, const bool& = false );
        std::string Discript( TH1* );
        bool        OptionContent( const std::string&, const std::string& );
        double      GetZmass( const std::vector<int>& );

        float GetJetPt( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->JetPt();
        }

        float GetJetEta( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->JetEta();
        }

        float GetJetCSV( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->JetDeepCSV();
        }

        bool IsSelJet( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->IsSelJet();
        }

        bool PassDeepCSVM( const int& i )
        {
            _sample->SetIndex( i );
            return _sample->PassDeepCSVM();
        }

        int Jsize()
        {
            return _sample->Jsize();
        }

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

        /*******************************************************************************
        *   Weight
        *******************************************************************************/
        void                 GetSelJet( std::vector<int>&, std::vector<int>& );
        void                 Fill2DBtagEff( TEfficiency*, TEfficiency*, TEfficiency*, const std::vector<int>&, const double& );
        void                 Fill1DBtagEff_Pt( TEfficiency*, TEfficiency*, TEfficiency*, const std::vector<int>&, const double& );
        void                 Fill1DBtagEff_Eta( TEfficiency*, TEfficiency*, TEfficiency*, const std::vector<int>&, const double& );
        double               GetLepSF( TH2D*, const int& );
        double               GetLepSFUp( TH2D*, const int& );
        double               GetLepSFDn( TH2D*, const int& );
        double               GetJetSF( TH2D*, const int& );
        double               GetJetSFUp( TH2D*, const int& );
        double               GetJetSFDn( TH2D*, const int& );
        float                GenWeight()                 { return _sample->GenWeight(); }
        float                GetPUWeight()               { return _sample->GetPUWeight(); }
        float                GetPUWeightUp()             { return _sample->GetPUWeightUp(); }
        float                GetPUWeightDn()             { return _sample->GetPUWeightDn(); }
        void                 RegisterWeight( TChain* ch ){ _sample->RegisterWeight( ch ); }
        TH2D*                GetSFHist( const std::string& );
        void                 InitBtagWeight( const std::string&, const std::string& );
        TH2D*                GetBtagEffPlot( const int& );
        BTagEntry::JetFlavor GetBtagFlavor( const int& );
        double               GetTaggedEff( const int&, const bool&, const std::string&, const BTagEntry::OperatingPoint&    = BTagEntry::OP_MEDIUM );
        double               GetNonTaggedEff( const int&, const bool&, const std::string&, const BTagEntry::OperatingPoint& = BTagEntry::OP_MEDIUM );
        double               GetBtagWeight( const std::vector<int>&, const std::vector<int>&, const std::string&            = "central" );
        double               GetBtagWeight_CR_0b( const std::vector<int>& );
        double               GetBtagWeight_CR_1b( const std::vector<int>&, const std::vector<int>& );

        /*******************************************************************************
        *   Pre-selection
        *******************************************************************************/
        bool IsGoodEvt( checkEvtTool& );
        void JECUpdate(); 
        void JECUncUpdate(); 
        void JECUncSrcUpdate(); 
        void JERUpdate();

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
        bool PassFullLepton_CRDYJets( std::vector<int>&, const std::string& );
        bool PassFullLepton_CRQCD( std::vector<int>&, const std::string& );
        bool PassFullLepton_CRWJets( std::vector<int>&, const std::string& );

        bool                                      PassFullJet( std::vector<int>&, std::vector<int>&, const int& );
        bool                                      PassFullJet_CR_1b( std::vector<int>&, std::vector<int>&, const int& );
        bool                                      PassFullJet_CR_0b( std::vector<int>&, const int& );
        bool                                      PassFullJet_CRDYJets( std::vector<int>& );
        std::tuple<double, double, int, int, int> GetChi2Info( const std::vector<int>&, const std::vector<int>& );
        std::tuple<double, double, int, int, int> GetChi2Info_CR_0b( std::vector<int>&, std::vector<int>&, const int& );
        double                                    GetLeptonicM( const int&, const int& );

        /*******************************************************************************
        *   Looping event
        *******************************************************************************/
        void         process( const int&, const int& );
        BaseLineMgr* GetSample(){ return _sample; }

    private:

        BaseLineMgr* _sample;
        TH2D* _eff_b;
        TH2D* _eff_c;
        TH2D* _eff_l;
};

#endif
