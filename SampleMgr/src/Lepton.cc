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
        return fabs( _lep.MuInnerTrackDxy[ _idx ] );
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
    // https://github.com/cms-sw/cmssw/blob/CMSSW_9_4_X/RecoEgamma/ElectronIdentification/python/Identification/cutBasedElectronID_Fall17_94X_V2_cff.py
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

    bool
    SampleMgr::ElIDCRTight()
    {
        // removing PF isolation cut
        if( LepAbsEta() <= 1.479 ){
            return
                ElsigmaIetaIeta() < 0.0104 &&
                fabs( EldEtaInSeed() ) < 0.00255 &&
                fabs( EldPhiIn() ) < 0.022 &&
                ElGsfEleHadronicOverEMCut() < 0.026 + 1.15 / EnergySC() + 0.0324 * EvtRho() / EnergySC() &&
                fabs( GsfEleEInverseMinusPInverseCut() ) < 0.159 &&
                ElNumberOfExpectedInnerHits() <= 1 &&
                !ElhasConv()
            ;
        }
        else{// absEta > 1.479
            return
                ElsigmaIetaIeta() < 0.0353 &&
                fabs( EldEtaInSeed() ) < 0.00501 &&
                fabs( EldPhiIn() ) < 0.0236 &&
                ElGsfEleHadronicOverEMCut() < 0.0188 + 2.06 / EnergySC() + 0.183 * EvtRho() / EnergySC() &&
                fabs( GsfEleEInverseMinusPInverseCut() ) < 0.0197 &&
                ElNumberOfExpectedInnerHits() <= 1 &&
                !ElhasConv()
            ;
        }
    }

    bool
    SampleMgr::ElIDCRLoose()
    {
        // removing PF isolation cut
        if( LepAbsEta() <= 1.479 ){
            return
                ElsigmaIetaIeta() < 0.0112 &&
                fabs( EldEtaInSeed() ) < 0.00377 &&
                fabs( EldPhiIn() ) < 0.0884 &&
                ElGsfEleHadronicOverEMCut() < 0.005 + 1.16 / EnergySC() + 0.0324 * EvtRho() / EnergySC() &&
                fabs( GsfEleEInverseMinusPInverseCut() ) < 0.193 &&
                ElNumberOfExpectedInnerHits() <= 1 &&
                !ElhasConv()
            ;
        }
        else{// absEta > 1.479
            return
                ElsigmaIetaIeta() < 0.0425 &&
                fabs( EldEtaInSeed() ) < 0.00674 &&
                fabs( EldPhiIn() ) < 0.169 &&
                ElGsfEleHadronicOverEMCut() < 0.0441 + 2.54 / EnergySC() + 0.183 * EvtRho() / EnergySC() &&
                fabs( GsfEleEInverseMinusPInverseCut() ) < 0.111 &&
                ElNumberOfExpectedInnerHits() <= 1 &&
                !ElhasConv()
            ;
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
        return fabs( _lep.ElTrackDxy[ _idx ] );
    }

    float
    SampleMgr::ElsigmaIetaIeta()
    {
        return _lep.SigmaIetaIeta[ _idx ];
    }

    float
    SampleMgr::EldEtaInSeed()
    {
        return _lep.EldeltaPhiSeed[ _idx ];
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
        const float eSCoverP            = _lep.ElEoverP[ _idx ];
        return std::fabs( 1.0 - eSCoverP ) * ecal_energy_inverse;
    }

    float
    SampleMgr::ElNumberOfExpectedInnerHits()
    {
        return _lep.NumberOfExpectedInnerHits[ _idx ];
    }

    float
    SampleMgr::EnergySC()
    {
        return _lep.EnergySC[ _idx ];
    }

    bool
    SampleMgr::ElhasConv()
    {
        return _lep.ElhasConv[ _idx ];
    }

    float
    SampleMgr::ElPFISO()
    {
        return _lep.PFIsoRhoCorrR03[ _idx ] / _lep.Pt[ _idx ];
    }

    void
    SampleMgr::ElEnergyCorr()
    {
        for( int i = 0; i < Lsize(); i++ ){
            if( _lep.LeptonType[ _idx ] == 11 ){
                SetIndex( i );
                TLorentzVector lepp4 = GetLepP4( i );
                lepp4 *= _lep.ElEnergyCorrFactor[ _idx ];
                SetLepP4( lepp4 );
            }
        }
    }

    void 
    SampleMgr::SetLepP4( const TLorentzVector& tl )
    {
        _lep.Pt[ _idx ]  = tl.Pt();
        _lep.Px[ _idx ]  = tl.Px();
        _lep.Py[ _idx ]  = tl.Py();
        _lep.Pz[ _idx ]  = tl.Pz();
        _lep.Energy[ _idx ]  = tl.Energy();
    }
}
