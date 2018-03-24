#ifndef SELECTOR_H
#define SELECTOR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"

#include "CPVAnalysis/BaseLineSelector/interface/BaseLineMgr.h"

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
        std::string Discript( TH1* );

        /*******************************************************************************
        *   Pre-selection
        *******************************************************************************/
        bool IsGoodEvt( checkEvtTool& );
        void JERCorr();
        bool PassVertex();
        bool PassHLT( const std::vector<int>& );
        bool PreJet();
        bool PreLep();
        bool PreSelection();
        int  nPU(){ return _sample->nPU(); }

        /*******************************************************************************
        *   Full-selection
        *******************************************************************************/
        bool                            PassFullEl( std::vector<int>& );
        bool                            PassFullMu( std::vector<int>& );
        bool                            PassFullJet( std::vector<int>&, std::vector<int>&, const int& );
        std::tuple<double, double, int> GetChi2Info( const std::vector<int>&, const std::vector<int>& );
        double                          GetLeptonicM( const int&, const int& );

        /*******************************************************************************
        *   Looping event
        *******************************************************************************/
        void   process( const int&, const int& );
        void   ChangeFile( const std::string& );
        void   GetEntry( const int& );
        int    GetEntries(){ return _sample->GetEntries(); }
        TTree* CloneTree() { return _sample->CloneTree(); }

    private:

        BaseLineMgr* _sample;
};

#endif
