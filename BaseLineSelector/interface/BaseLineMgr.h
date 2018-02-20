#ifndef BASELINEMGR_H
#define BASELINEMGR_H

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"
#include "ManagerUtils/HistMgr/interface/HistMgr.h"
#include "ManagerUtils/HistMgr/interface/Hist2DMgr.h"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.h"

#include "CPVAnalysis/BaseLineSelector/interface/checkEvtTool.h"

#include "TChain.h"
#include "TTree.h"
#include "TH2.h"

#include <string>
#include <tuple>
#include <vector>

class BaseLineMgr : public mgr::HistMgr,
                    public mgr::Hist2DMgr {
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
        enum MatchType
        {
            Correct   = 1,
            Misid     = 2,
            Mistag    = 3,
            Nomatched = 4
            //Correct: correct
            //Misid  : both are b, but wrong charge
            //Mistag : not b
        };
       
        TLorentzVector GetLepP4(const int&);
        TLorentzVector GetJetP4(const int&);
        float GetIsoLepCharge(const int& idx);

        std::tuple<double, double, int> GetChi2Info( const std::vector<int>&, const std::vector<int>& );
        bool                            isGoodEvt( checkEvtTool& evt ){ return evt.isGoodEvt( _sample->RunNo(), _sample->LumiNo() ); }

        BaseLineMgr::MatchType  bbSeparation( const int&, const int&, const int& );
        double GetLeptonicM(const int&, const int&);

        /*******************************************************************************
        *   Vertex & HLT selection
        *******************************************************************************/
        bool passHLT( const std::vector<int>& );
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
        TTree* CloneTree();
        
        /*******************************************************************************
        *   Event Weighting
        *******************************************************************************/
        //PU weight
        void RegisterWeight();
        float GetPUWeight(){ return _puweight; }
        int nPU(){ return _sample->Evt().nPU[0];}
        int nVtx(){ return _sample->Vsize(); }
        float GenWeight() { return _sample->Gen().Weight; }

        //B-tagging weight
        void InitBtagWeight( const std::string&, const std::string& );
        double BtagScaleFactor( BTagEntry::OperatingPoint, const int& );

        //ID ISO Trg weight
        double GetSFTH2(TH2*, const int&);

    private:

        std::unique_ptr<mgr::SampleMgr> _sample;
        TChain* _ch;

        float  _puweight;
        BTagCalibration* _calib;
        std::map<BTagEntry::OperatingPoint, BTagCalibrationReader> _reader_map;
};


#endif
