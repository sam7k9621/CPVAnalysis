#ifndef BASELINEMGR_H
#define BASELINEMGR_H

#include "ManagerUtils/HistMgr/interface/HistMgr.h"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.h"

#include "CPVAnalysis/BaseLineSelector/interface/checkEvtTool.h"

#include "TChain.h"
#include "TTree.h"

#include <string>
#include <tuple>
#include <vector>

class BaseLineMgr : public mgr::HistMgr {
    public:

        /*******************************************************************************
        *   Class initialization
        *******************************************************************************/
        BaseLineMgr( const std::string&, TChain* );
        virtual ~BaseLineMgr();

        BaseLineMgr( const BaseLineMgr& )            = delete;
        BaseLineMgr& operator=( const BaseLineMgr& ) = delete;

        /*******************************************************************************
        *   Common
        *******************************************************************************/
        std::tuple<double, double, int> GetChi2Info( const std::vector<int>&, const std::vector<int>& );
        bool                            isGoodEvt( checkEvtTool& evt ){ return evt.isGoodEvt( _sample->RunNo(), _sample->LumiNo() ); }

        bool FillbHandle(){ return _sample->MCTruthBJet(); }
        int  bbSeparation( const int&, const int&, const int& );

        enum MatchType
        {
            Correct = 1 << 0,
            Fakeb   = 1 << 1,
            Mistag  = 1 << 2,
            Swap    = 1 << 3,
            Other   = 1 << 4,
            None    = 1 << 5,

            // 1  Correct: correct
            // 2  Fakeb  : not b
            // 4  Mistag : b not from t
            // 8  Swap   : b from t but charge mistag(swapped)
            // 16 Other  : (2 & 4) and only one mistag
            // 32 None   : not matched
        };

        /*******************************************************************************
        *   HLT selection
        *******************************************************************************/
        bool passHLT( const std::vector<int>& );

        /*******************************************************************************
        *   Vertex selection
        *******************************************************************************/
        int pvNumber(){ return _sample->Vsize(); }

        bool passVertex();
        bool isGoodPVtx();

        /*******************************************************************************
        *   Jet selection
        *******************************************************************************/
        void jetSmeared( TTree* );
        bool passJet();
        bool passBJet();
        bool passFullJet( std::vector<int>&, std::vector<int>&, int& );

        /*******************************************************************************
        *   Muon selection
        *******************************************************************************/
        bool passMuLoose();
        bool passMuTight();
        bool passFullMuon( std::vector<int>& );

        /*******************************************************************************
        *   Electron selection
        *******************************************************************************/
        bool passElLoose();

        /*******************************************************************************
        *   Pre-selection
        *******************************************************************************/
        bool preJet();
        bool preMuon();

        /*******************************************************************************
        *   Event looping
        *******************************************************************************/
        void GetEntry(const int&);
        int  GetEntries(){ return _ch->GetEntries(); }
    private:

        std::unique_ptr<mgr::SampleMgr> _sample;
        TChain* _ch;
};


#endif
