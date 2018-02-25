#include "CPVAnalysis/SampleMgr/interface/SampleMgr.h"
using namespace std;

namespace mgr{

    /*******************************************************************************
    *  Lep common info
    *******************************************************************************/
    float
    SampleMgr::LepPt()
    {
        return _lep.Pt[ _idx ];
    }

    float
    SampleMgr::LepEta()
    {
        return _lep.Eta[ _idx ] ;
    }
    
    float
    SampleMgr::LepAbsEta()
    {
        return fabs( _lep.Eta[ _idx ] );
    }

    float
    SampleMgr::GetLepCharge( const int& idx )
    {
        return _lep.Charge[ idx ];
    }

    TLorentzVector
    SampleMgr::GetLepP4(const int& i)
    {
        return TLorentzVector( _lep.Px[ i ], _lep.Py[ i ], _lep.Pz[ i ], _lep.Energy[ i ] );
    }
    /*******************************************************************************
    *  Muon selection info
    *  https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonIdRun2#Tight_Muon
    *  http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_8_0_11/doc/html/de/d37/DataFormats_2MuonReco_2interface_2Muon_8h_source.html
    *******************************************************************************/

    float
    SampleMgr::AbsMuInnerTrackDxy_PV()
    {
        return fabs( _lep.MuInnerTrackDxy_PV[ _idx ] );
    }

    float
    SampleMgr::AbsMuInnerTrackDz()
    {
        return fabs( _lep.MuInnerTrackDz[ _idx ] );
    }

    // number of muon hits
    int
    SampleMgr::MuNMuonhits()
    {
        return _lep.MuNMuonhits[ _idx ];
    }

    // number of matched muon stations
    int
    SampleMgr::MuNMatchedStations()
    {
        return _lep.MuNMatchedStations[ _idx ];
    }

    // global normalized chi^2 / dof
    float
    SampleMgr::MuGlobalNormalizedChi2()
    {
        return _lep.MuGlobalNormalizedChi2[ _idx ];
    }

    // number of track layers with measurement
    int
    SampleMgr::MuNTrackLayersWMeasurement()
    {
        return _lep.MuNTrackLayersWMeasurement[ _idx ];
    }

    int
    SampleMgr::MuNPixelLayers()
    {
        return _lep.MuNPixelLayers[ _idx ];
    }

    bool
    SampleMgr::IsPFMuon()
    {
        return _lep.isPFMuon[ _idx ];
    }

    bool
    SampleMgr::IsGlobalMuon()
    {
        return ( _lep.MuType[ _idx ] ) & 0x02;
    }

    bool
    SampleMgr::IsTrackerMuon()
    {
        return ( _lep.MuType[ _idx ] ) & 0x04;
    }
    /*******************************************************************************
    *   Electron selection
    *   https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Cut_Based_Electron_ID_for_Run_2
    *   https://github.com/ntuhep/bprimeKit/blob/master/src/LeptonNtuplizer_Electron.cc#L101
    *******************************************************************************/
    bool
    SampleMgr::ElIDLoose()
    {
        return _lep.EgammaCutBasedEleIdLOOSE[ _idx ];
    }

    bool
    SampleMgr::ElIDTight()
    {
        return _lep.EgammaCutBasedEleIdTIGHT[ _idx ];
    }

    float 
    SampleMgr::ElAbsTrackDz()
    {
        return fabs( _lep.ElTrackDz[ _idx ] );
    }

    float 
    SampleMgr::ElAbsTrackDxy_PV()
    {
        return fabs( _lep.ElTrackDxy_PV[ _idx ] );
    }
}
