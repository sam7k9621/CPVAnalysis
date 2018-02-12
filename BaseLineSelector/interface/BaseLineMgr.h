#ifndef BASELINEMGR_H
#define BASELINEMGR_H

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"
#include "ManagerUtils/HistMgr/interface/HistMgr.h"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.h"

#include "CPVAnalysis/BaseLineSelector/interface/checkEvtTool.h"

#include "TChain.h"
#include "TTree.h"
#include "TH2.h"

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

        int nPU(){ return _sample->Evt().nPU[0];}
        int nVtx(){ return _sample->Vsize(); }
        float GenWeight() { return _sample->Gen().Weight; }
        /*******************************************************************************
        *   HLT selection
        *******************************************************************************/
        bool passHLT( const std::vector<int>& );

        /*******************************************************************************
        *   Vertex selection
        *******************************************************************************/

        bool passVertex();
        bool isGoodPVtx();

        /*******************************************************************************
        *   Jet selection
        *******************************************************************************/
        void jetSmeared();
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
        
        /*******************************************************************************
        *   Event Weighting
        *******************************************************************************/
        void RegisterWeight();
        float GetPUWeight(){ return _puweight; }

        void InitBtagWeight( const std::string&, const std::string& );
        double BtagScaleFactor( BTagEntry::OperatingPoint, const int& );

        double GetSFTH2(TH2*, const int&);

    private:

        std::unique_ptr<mgr::SampleMgr> _sample;
        TChain* _ch;
        float  _puweight;
        BTagCalibration* _calib;
        std::map<BTagEntry::OperatingPoint, BTagCalibrationReader> _reader_map;
};


#endif
