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
        return _lep.Eta[ _idx ];
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
    SampleMgr::GetLepP4( const int& i )
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
        //removing PF isolation cut
        if( LepAbsEta() <= 1.479 ){
            return (
            ElsigmaIetaIeta() < 0.011 &&
            fabs( EldEtaInSeed() ) < 0.00477 &&
            fabs( EldPhiIn() ) < 0.222 &&
            ElGsfEleHadronicOverEMCut() < 0.298 &&
            ElPFISO() < 0.0994 &&
            fabs( GsfEleEInverseMinusPInverseCut() ) < 0.241 &&
            ElNumberOfExpectedInnerHits() <= 1 &&
            !ElhasConv() 
            );
        }
        else{  //absEta > 1.479
            return (
            ElsigmaIetaIeta() < 0.0314 &&
            fabs( EldEtaInSeed() ) < 0.00868 &&
            fabs( EldPhiIn() ) < 0.213 &&
            ElGsfEleHadronicOverEMCut() < 0.101 &&
            ElPFISO() < 0.107 &&
            fabs( GsfEleEInverseMinusPInverseCut() ) < 0.14 &&
            ElNumberOfExpectedInnerHits() <= 1 &&
            !ElhasConv() 
            );
        }
    }

    bool
    SampleMgr::ElIDTight()
    {
        //removing PF isolation cut
        if( LepAbsEta() <= 1.479 ){
            return (
            ElsigmaIetaIeta() < 0.00998 &&
            fabs( EldEtaInSeed() ) < 0.00308 &&
            fabs( EldPhiIn() ) < 0.0816 &&
            ElGsfEleHadronicOverEMCut() < 0.0414 &&
            ElPFISO() < 0.0588 &&
            fabs( GsfEleEInverseMinusPInverseCut() ) < 0.0129 &&
            ElNumberOfExpectedInnerHits() <= 1 &&
            !ElhasConv() 
            );
        }
        else{  //absEta > 1.479
            return (
            ElsigmaIetaIeta() < 0.0292 &&
            fabs( EldEtaInSeed() ) < 0.00605 &&
            fabs( EldPhiIn() ) < 0.0394 &&
            ElGsfEleHadronicOverEMCut() < 0.0641 &&
            ElPFISO() < 0.0571 &&
            fabs( GsfEleEInverseMinusPInverseCut() ) < 0.0129 &&
            ElNumberOfExpectedInnerHits() <= 1 &&
            !ElhasConv() 
            );
        }
    }

    bool 
    SampleMgr::ElIDCRLoose()
    {
        //removing PF isolation cut
        if( LepAbsEta() <= 1.479 ){
            return (
            ElsigmaIetaIeta() < 0.011 &&
            fabs( EldEtaInSeed() ) < 0.00477 &&
            fabs( EldPhiIn() ) < 0.222 &&
            ElGsfEleHadronicOverEMCut() < 0.298 &&
            fabs( GsfEleEInverseMinusPInverseCut() ) < 0.241 &&
            ElNumberOfExpectedInnerHits() <= 1 &&
            !ElhasConv() 
            );
        }
        else{  //absEta > 1.479
            return (
            ElsigmaIetaIeta() < 0.0314 &&
            fabs( EldEtaInSeed() ) < 0.00868 &&
            fabs( EldPhiIn() ) < 0.213 &&
            ElGsfEleHadronicOverEMCut() < 0.101 &&
            fabs( GsfEleEInverseMinusPInverseCut() ) < 0.14 &&
            ElNumberOfExpectedInnerHits() <= 1 &&
            !ElhasConv() 
            );
        }
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

    float 
    SampleMgr::ElsigmaIetaIeta()
    {
        return _lep.ElsigmaIetaIeta[ _idx ];
    }

    float
    SampleMgr::EldEtaInSeed()
    {
        return _lep.EldeltaEta[ _idx ] - LepEta() + _lep.caloEta[ _idx ];
    }

    float 
    SampleMgr::EldPhiIn()
    {
        return _lep.EldeltaPhi[ _idx ];
    }

    float 
    SampleMgr::ElGsfEleHadronicOverEMCut()
    {
        return _lep.ElHadoverEm[ _idx ];
    }

    float
    SampleMgr::GsfEleEInverseMinusPInverseCut()
    {
        const float ecal_energy_inverse = 1.0 / _lep.ElEcalE[ _idx ];
        const float eSCoverP = _lep.ElEoverP[ _idx ];
        return std::fabs(1.0 - eSCoverP)*ecal_energy_inverse;
    }

    float
    SampleMgr::ElNumberOfExpectedInnerHits()
    {
        return _lep.NumberOfExpectedInnerHits[ _idx ];
    }

    bool
    SampleMgr::ElhasConv()
    {
        return _lep.ElhasConv[ _idx ];
    }

    float
    SampleMgr::ElPFISO()
    {
        return _lep.IsoRhoCorrR03[ _idx ] / _lep.Pt[ _idx ];
    }
}
