#ifndef BASELINEMGR_H
#define BASELINEMGR_H

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"

#include "ManagerUtils/HistMgr/interface/Hist2DMgr.h"
#include "ManagerUtils/HistMgr/interface/HistMgr.h"

#include "CPVAnalysis/BaseLineSelector/interface/checkEvtTool.h"
#include "CPVAnalysis/SampleMgr/interface/SampleMgr.h"

#include "TChain.h"
#include "TH2.h"
#include "TTree.h"

#include <string>
#include <tuple>
#include <vector>

class BaseLineMgr : public mgr::SampleMgr,
                    public mgr::HistMgr,
                    public mgr::Hist2DMgr {
    public:

        /*******************************************************************************
        *   Class initialization
        *******************************************************************************/
        BaseLineMgr( TChain*, const std::string& = "" );
        ~BaseLineMgr();

        BaseLineMgr( const BaseLineMgr& )            = delete;
        BaseLineMgr& operator=( const BaseLineMgr& ) = delete;

        /*******************************************************************************
        *   bbSeparation
        *******************************************************************************/
        enum MatchType
        {
            Correct   = 1,
            Misid     = 2,
            Mistag    = 3,
            Nomatched = 4
                        // Correct: correct
                        // Misid  : both are b, but wrong charge
                        // Mistag : not b
        };

        BaseLineMgr::MatchType bbSeparation( const int&, const int&, const int& );

        /*******************************************************************************
        *   Basic RECO
        *******************************************************************************/
        bool IsGoodEvt( checkEvtTool& );

        /*******************************************************************************
        *   Vertex & HLT selection
        *******************************************************************************/
        bool IsGoodPVtx();

        /*******************************************************************************
        *   Jet selection
        *******************************************************************************/
        // Jet energy resolution correction
        unsigned bitconv( const float& x );
        bool     IsWellMatched();
        double   MakeScaled();
        double   MakeSmeared();
        void     JERCorr();

        bool PassJetLooseID();
        bool PassJetKinematic();
        bool IsSelJet();
        bool PassBJet();

        /*******************************************************************************
        *   Muon selection
        *******************************************************************************/
        bool PassMuLooseID();
        bool PassMuLooseKinematic();
        bool PassMuLooseISO();
        bool IsLooseMu();

        bool PassMuTightID();
        bool PassMuTightKinematic();
        bool PassMuTightISO();
        bool IsTightMu();

        /*******************************************************************************
        *   Electron selection
        *******************************************************************************/
        bool PassImpactParameter();

        bool PassElLooseID();
        bool PassElLooseKinematic();
        bool IsLooseEl();

        bool PassElTightID();
        bool PassElTightKinematic();
        bool IsTightEl();

        /*******************************************************************************
        *   Event looping
        *******************************************************************************/
        void   GetEntry( const int& );
        int    GetEntries(){ return _ch->GetEntries(); }
        TTree* CloneTree();

        /*******************************************************************************
        *   Event Weighting
        *******************************************************************************/
        // PU weight
        void  RegisterWeight();
        float GetPUWeight(){ return _puweight; }
        int   nVtx()       { return Vsize(); }

        // B-tagging weight
        void   InitBtagWeight( const std::string&, const std::string& );
        double BtagScaleFactor( BTagEntry::OperatingPoint, const int& );

        // ID ISO Trg weight
        double GetSFTH2( TH2*, const int& );

    private:

        TChain* _ch;

        float _puweight;
        BTagCalibration* _calib;
        std::map<BTagEntry::OperatingPoint, BTagCalibrationReader> _reader_map;
};


#endif
