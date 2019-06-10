#ifndef BASELINEMGR_H
#define BASELINEMGR_H

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"

#include "ManagerUtils/HistMgr/interface/Hist2DMgr.h"
#include "ManagerUtils/HistMgr/interface/HistMgr.h"

#include "CPVAnalysis/SampleMgr/interface/checkEvtTool.h"
#include "CPVAnalysis/SampleMgr/interface/SampleMgr.h"

#include "TChain.h"
#include "TH2.h"
#include "TTree.h"

#include <string>
#include <tuple>
#include <vector>

class BaseLineMgr : public mgr::Hist2DMgr,
                    public mgr::HistMgr,
                    public mgr::SampleMgr {
    public:

        /*******************************************************************************
        *   Class initialization
        *******************************************************************************/
        BaseLineMgr( const std::string& = "" );
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
        bool     IsWellMatched( const double& );
        double   MakeScaled( const double& );
        double   MakeSmeared( const double&, const double& );
        void     JECUp();
        void     JECDn();
        void     JERCorr();
        void     JERCorrUp();
        void     JERCorrDn();

        bool PassJetLooseID();
        bool PassJetKinematic();
        bool IsSelJet();
        bool IsPreSelJet();
        bool PassBJet();
        bool PassCS2BJet();
        bool RejectBJet();

        /*******************************************************************************
        *   Muon selection
        *******************************************************************************/
        bool PassMuLooseID();
        bool PassMuLooseKinematic();
        bool PassMuLooseISO();
        bool IsLooseMu();
        bool IsCRLooseMu();

        bool PassMuTightID();
        bool PassMuTightKinematic();
        bool PassMuTightISO();
        bool IsTightMu();
        bool IsPreTightMu();

        /*******************************************************************************
        *   Electron selection
        *******************************************************************************/
        bool PassImpactParameter();

        bool PassElLooseKinematic();
        bool PassElLooseID();
        bool PassElCRLooseID();
        bool IsLooseEl();
        bool IsCRLooseEl();

        bool PassElTightID();
        bool PassElTightKinematic();
        bool IsTightEl();
        bool IsPreTightEl();

        /*******************************************************************************
        *   Event Weighting
        *******************************************************************************/
        // PU weight
        void  RegisterWeight( TChain* );
        float GetPUWeight()  { return _puweight; }
        float GetPUWeightUp(){ return _puweight_up; }
        float GetPUWeightDn(){ return _puweight_dn; }
        int   nVtx()         { return Vsize(); }
        float Rho()          { return EvtRho(); }

        // B-tagging weight
        void   InitBtagWeight( const std::string&, const std::string& );
        double BtagScaleFactor( BTagEntry::OperatingPoint, const int& );
        double BtagScaleFactorUp( BTagEntry::OperatingPoint, const int& );
        double BtagScaleFactorDn( BTagEntry::OperatingPoint, const int& );

        // ID ISO Trg weight
        double GetSFTH2( TH2D*, const int& );
        double GetSFTH2Up( TH2D*, const int& );
        double GetSFTH2Dn( TH2D*, const int& );

        // JES
        void InitJES();

        // Top Pt Weighting
        double TopPtWeight();

    private:

        float _puweight;
        float _puweight_up;
        float _puweight_dn;
        BTagCalibration* _calib;
        JetCorrectionUncertainty* _jecUnc;
        std::map<BTagEntry::OperatingPoint, BTagCalibrationReader> _reader_map;
};


#endif
