#include "CPVAnalysis/BaseLineSelector/interface/BaseLineMgr.h"
#include <climits>
#include <iostream>
#include <string>
using namespace std;
using namespace mgr;

/*******************************************************************************
*   Class initialization
*******************************************************************************/
BaseLineMgr::BaseLineMgr( TChain* ch, const string& sample ) :
    SampleMgr( ch ),
    HistMgr( sample ),
    Hist2DMgr( sample ),
    _ch( ch )
{
    _calib = NULL;
}

BaseLineMgr::~BaseLineMgr()
{
    delete _calib;
}

/*******************************************************************************
*   bbSeparation
*******************************************************************************/
BaseLineMgr::MatchType
BaseLineMgr::bbSeparation( const int& had_b, const int& lep_b, const int& lepidx )
{
    // hadronic b charge equals to muon, and vice versa
    float charge = GetLepCharge( lepidx );
    float had_id = GetPartonID( had_b );
    float lep_id = GetPartonID( lep_b );

    if( had_id == 0 || lep_id == 0 ){
        return Nomatched;
    }

    if( fabs( had_id ) != 5 || fabs( lep_id ) != 5 ){
        return Mistag;
    }

    if( ( had_id * charge ) > 0 || ( lep_id * charge ) < 0 ){
        return Misid;
    }
    else{
        return Correct;
    }
}

/*******************************************************************************
*   Basic RECO
*******************************************************************************/
bool
BaseLineMgr::IsGoodEvt( checkEvtTool& evt )
{
    return evt.isGoodEvt( RunNo(), LumiNo() );
}

/*******************************************************************************
*   Vertex & HLT selection
*******************************************************************************/
bool
BaseLineMgr::IsGoodPVtx()
{
    return !( IsFake() ) &&
           Ndof() > 4 &&
           AbsZ() < 24 &&
           Rho() < 2
    ;
}

/*******************************************************************************
*   Jet selection
*******************************************************************************/
unsigned
BaseLineMgr::bitconv( const float& x )
{
    const void* temp = &x;
    return *( (const unsigned*)( temp ) );
}

bool
BaseLineMgr::IsWellMatched()
{
    // To avoid pile-up jet
    // (compare genjet and jet)
    const double res = JERPt();
    double deta      = JetEta() - GenJetEta();
    double dphi      = Phi_mpi_pi( (double)( JetPhi() - GenJetPhi() ) );
    double delR      = TMath::Sqrt( deta * deta + dphi * dphi );

    if( delR >= 0.4 / 2. ){
        return false;
    }
    if( fabs( JetPt() - GenJetPt() ) >= 3 * res ){
        return false;
    }

    return true;
}

double
BaseLineMgr::MakeScaled()
{
    const double resscale = JERScale();
    const double newpt    = std::max( 0.0, GenJetPt() + resscale * ( JetPt() - GenJetPt() ) );
    const double scale    = newpt / JetPt();

    return scale;
}

double
BaseLineMgr::MakeSmeared()
{
    // Getting normal
    const double res   = JERPt();
    const double ressf = JERScale();
    const double width = ressf > 1 ? sqrt( ressf * ressf - 1 ) * res : 0;

    // Generating random number
    // https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_25/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h
    std::default_random_engine gen( bitconv( JetPhi() ) );
    std::normal_distribution<double> myrand( JetPt(), width );
    const double newpt = myrand( gen );

    // Anonymouns namespace require (bug in gcc530)
    const double scale = newpt / JetPt();

    if( scale <= 0 || ::isnan( scale ) ){
        return 1;
    }
    else{
        return scale;
    }
}

void
BaseLineMgr::JERCorr()
{
    int js = Jsize();

    for( int i = 0; i < js; i++ ){
        SetIndex( i );

        double scale = 1;
        if( IsWellMatched() ){
            scale = MakeScaled();
        }
        else{
            scale = MakeSmeared();
        }
        TLorentzVector jetp4 = GetJetP4( i );
        jetp4 *= scale;
        SetJetPtEta( jetp4.Pt(), jetp4.Eta() );
    }
}

bool
BaseLineMgr::PassJetLooseID()
{
    return
        // Loose ID
        JetNHF() <= 0.99 &&
        JetNEF() <= 0.99 &&
        JetNConstituents() > 1 &&

        JetAbsEta() < 2.4 &&
        JetCHF() > 0 &&
        JetNCH() > 0 &&
        JetCEF() <= 0.99
    ;
}

bool
BaseLineMgr::PassJetKinematic()
{
    return JetPt() > 30 &&
           JetAbsEta() < 2.4
    ;
}

bool
BaseLineMgr::IsSelJet()
{
    return PassJetLooseID() &&
           PassJetKinematic()
    ;
}

bool
BaseLineMgr::PassBJet()
{
    return JetCSV() > 0.8484;
}

bool 
BaseLineMgr::RejectBJet()
{
    return JetCSV() < 0.5426;
}

/*******************************************************************************
*   Muon selection
*******************************************************************************/

bool
BaseLineMgr::PassMuLooseID()
{
    return IsPFMuon() &&
           ( IsGlobalMuon() || IsTrackerMuon() )
    ;
}

bool
BaseLineMgr::PassMuLooseKinematic()
{
    return LepPt() > 15 &&
           LepAbsEta() < 2.4
    ;
}

bool
BaseLineMgr::PassMuLooseISO()
{
    return RelIsoR04() < 0.25;
}

bool
BaseLineMgr::IsLooseMu()
{
    return PassMuLooseID() &&
           PassMuLooseKinematic() &&
           PassMuLooseISO()
    ;
}

bool
BaseLineMgr::IsCRLooseMu()
{
    //removing PF isolation cut
    return PassMuLooseID() &&
           PassMuLooseKinematic() 
    ;
}

bool
BaseLineMgr::PassMuTightID()
{
    return IsGlobalMuon() &&
           IsPFMuon() &&
           MuGlobalNormalizedChi2() < 10 &&
           MuNMuonhits() > 0 &&
           MuNMatchedStations() > 1 &&
           AbsMuInnerTrackDxy_PV() < 0.2 &&
           AbsMuInnerTrackDz() < 0.5 &&
           MuNPixelLayers() > 0 &&
           MuNTrackLayersWMeasurement() > 5
    ;
}

bool
BaseLineMgr::PassMuTightKinematic()
{
    return LepPt() > 30 &&
           LepAbsEta() < 2.1
    ;
}

bool
BaseLineMgr::PassMuTightISO()
{
    return RelIsoR04() < 0.15;
}

bool
BaseLineMgr::IsTightMu()
{
    return PassMuTightID() &&
           PassMuTightKinematic() &&
           PassMuTightISO()
    ;
}

/*******************************************************************************
*   Electron selection
*******************************************************************************/
bool
BaseLineMgr::PassImpactParameter()
{
    if( LepAbsEta() < 1.45 ){
        if( ElAbsTrackDxy_PV() > 0.05 ){
            return false;
        }
        if( ElAbsTrackDz() > 0.10 ){
            return false;
        }
    }
    else{
        if( ElAbsTrackDxy_PV() > 0.10 ){
            return false;
        }
        if( ElAbsTrackDz() > 0.20 ){
            return false;
        }
    }

    return true;
}

bool
BaseLineMgr::PassElLooseID()
{
    return ElIDLoose();
}

bool
BaseLineMgr::PassElCRLooseID()
{
    //removing PF isolation cut
    if( LepAbsEta() <= 1.479 ){
        return (
        ElsigmaIetaIeta() < 0.011 &&
        abs( EldEtaInSeed() ) < 0.00477 &&
        abs( EldPhiIn() ) < 0.222 &&
        ElGsfEleHadronicOverEMCut() < 0.298 &&
        GsfEleEInverseMinusPInverseCut() < 0.241 &&
        ElNumberOfExpectedInnerHits() <= 1 &&
        ElhasConv() 
        );
    }
    else{  //absEta > 1.479
        return (
        ElsigmaIetaIeta() < 0.0314 &&
        abs( EldEtaInSeed() ) < 0.00868 &&
        abs( EldPhiIn() ) < 0.213 &&
        ElGsfEleHadronicOverEMCut() < 0.101 &&
        GsfEleEInverseMinusPInverseCut() < 0.14 &&
        ElNumberOfExpectedInnerHits() <= 1 &&
        ElhasConv() 
        );
    }
}

bool
BaseLineMgr::PassElLooseKinematic()
{
    return LepPt() > 15 &&
           LepAbsEta() < 2.4 &&
           !( LepAbsEta() > 1.44 && LepAbsEta() < 1.57 )
    ;
}

bool
BaseLineMgr::IsLooseEl()
{
    return PassImpactParameter() &&
           PassElLooseID() &&
           PassElLooseKinematic()
    ;
}

bool
BaseLineMgr::IsCRLooseEl()
{
    return PassImpactParameter() &&
           PassElCRLooseID() &&
           PassElLooseKinematic()
    ;
}

bool
BaseLineMgr::PassElTightID()
{
    return ElIDTight();
}

bool
BaseLineMgr::PassElTightKinematic()
{
    return LepPt() > 30 &&
           LepAbsEta() < 2.1 &&
           !( LepAbsEta() > 1.44 && LepAbsEta() < 1.57 )
    ;
}

bool
BaseLineMgr::IsTightEl()
{
    return PassImpactParameter() &&
           PassElTightID() &&
           PassElTightKinematic()
    ;
}

/*******************************************************************************
*   Event looping
*******************************************************************************/
void
BaseLineMgr::GetEntry( const int& i )
{
    _ch->GetEntry( i );
}

TTree*
BaseLineMgr::CloneTree()
{
    _ch->GetEntry( 0 );
    return (TTree*)_ch->GetTree()->CloneTree( 0 );
}
