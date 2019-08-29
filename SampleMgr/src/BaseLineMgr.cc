#include "CPVAnalysis/SampleMgr/interface/BaseLineMgr.h"
#include <climits>
#include <iostream>
#include <random>
#include <string>
using namespace std;
using namespace mgr;

/*******************************************************************************
*   Class initialization
*******************************************************************************/
BaseLineMgr::BaseLineMgr( const string& sample ) :
    Hist2DMgr( sample ),
    HistMgr( sample )
{
    _calib  = NULL;
}

BaseLineMgr::~BaseLineMgr()
{
    delete _calib;
}

/*******************************************************************************
*   bbSeparation
*******************************************************************************/
BaseLineMgr::MatchType
BaseLineMgr::bbSeparation( const int& had_b, const int& lep_b, const int& lep )
{
    // hadronic b charge equals to muon, and vice versa
    int genbidx    = FindJet( 5 );
    int genbbaridx = FindJet( -5 );

    if( genbidx == -1 || genbbaridx == -1 ){
        return Nomatched;
    }

    float charge = GetLepCharge( lep );

    int recobidx    = charge > 0 ? MCTruthJet( lep_b ) : MCTruthJet( had_b );
    int recobbaridx = charge > 0 ? MCTruthJet( had_b ) : MCTruthJet( lep_b );

    if( recobidx == -1 || recobbaridx == -1 ){
        return Nomatched;
    }

    if(
        genbidx == recobidx &&
        genbbaridx == recobbaridx
        )
    {
        return Correct;
    }
    else if(
        genbidx == recobbaridx &&
        genbbaridx == recobidx
        )
    {
        return Misid;
    }
    else{
        return Mistag;
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
           VtxRho() < 2
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
BaseLineMgr::IsWellMatched( const double& res )
{
    // To avoid pile-up jet
    // (compare genjet and jet)
    double deta = JetEta() - GenJetEta();
    double dphi = Phi_mpi_pi( (double)( JetPhi() - GenJetPhi() ) );
    double delR = TMath::Sqrt( deta * deta + dphi * dphi );

    if( delR >= 0.4 / 2. ){
        return false;
    }
    if( fabs( JetPt() - GenJetPt() ) >= 3 * res * JetPt() ){
        return false;
    }

    return true;
}

double
BaseLineMgr::MakeScaled( const double& ressf )
{
    const double newpt = std::max( 0.0, GenJetPt() + ressf * ( JetPt() - GenJetPt() ) );
    const double scale = newpt / JetPt();

    return scale;
}

double
BaseLineMgr::MakeSmeared( const double& ressf, const double& res )
{
    // Getting normal
    const double width = ressf > 1 ? sqrt( ressf * ressf - 1 ) * res : 0;

    // Generating random number
    // https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_25/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h
    std::normal_distribution<> myrand( 0, width );
    double scale = 1. + myrand( _generator );
    if( scale <= 0 || ::isnan( scale ) ){
        return 1;
    }
    else{
        return scale;
    }
}

void
BaseLineMgr::JECDn()
{
    int js = Jsize();

    for( int i = 0; i < js; i++ ){
        SetIndex( i );
        TLorentzVector jetp4 = GetJetP4( i );
        double unc = JesUnc();
        jetp4 *= ( 1 - unc );
        SetJetP4( jetp4 );
    }
}

void
BaseLineMgr::JECUp()
{
    int js = Jsize();

    for( int i = 0; i < js; i++ ){
        SetIndex( i );
        TLorentzVector jetp4 = GetJetP4( i );
        double unc = JesUnc();
        jetp4 *= ( 1 + unc );
        SetJetP4( jetp4 );
    }
}

void
BaseLineMgr::JERCorr()
{
    for( int i = 0; i < Jsize(); i++ ){
        SetIndex( i );

        double ressf = JERScale();
        double res   = JERPt();
        double scale = 1.;

        if( IsWellMatched( res ) ){
            scale = MakeScaled( ressf );
        }
        else{
            scale = MakeSmeared( ressf, res );
        }
        TLorentzVector jetp4 = GetJetP4( i );
        jetp4 *= scale;

        SetJetP4( jetp4 );
    }
}

void
BaseLineMgr::JERCorrDn()
{
    for( int i = 0; i < Jsize(); i++ ){
        SetIndex( i );

        double ressf = JERScale_dn();
        double res   = JERPt();
        double scale = 1;
        if( IsWellMatched( res ) ){
            scale = MakeScaled( ressf );
        }
        else{
            scale = MakeSmeared( ressf, res );
        }
        TLorentzVector jetp4 = GetJetP4( i );
        jetp4 *= scale;
        SetJetP4( jetp4 );
    }
}

void
BaseLineMgr::JERCorrUp()
{
    for( int i = 0; i < Jsize(); i++ ){
        SetIndex( i );
        
        double ressf = JERScale_up();
        double res   = JERPt();
        double scale = 1;
        if( IsWellMatched( res ) ){
            scale = MakeScaled( ressf );
        }
        else{
            scale = MakeSmeared( ressf, res );
        }
        TLorentzVector jetp4 = GetJetP4( i );
        jetp4 *= scale;
        SetJetP4( jetp4 );
    }
}

bool
BaseLineMgr::PassJetLooseID()
{
    return
        // Loose ID
        JetNHF() < 0.99 &&
        JetNEF() < 0.99 &&
        JetNConstituents() > 1 &&

        JetAbsEta() < 2.4 &&
        JetCHF() > 0 &&
        JetNCH() > 0 &&
        JetCEF() < 0.99
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
BaseLineMgr::IsPreSelJet()
{
    return PassJetLooseID() &&
           JetAbsEta() < 2.4 &&
           JetPt() > 20
    ;
}

bool
BaseLineMgr::PassCSVM()
{
    return JetCSV() > 0.8838; //old
}

bool 
BaseLineMgr::PassCSVL()
{
    return JetCSV() > 0.5803; //old
}

bool 
BaseLineMgr::PassDeepCSVM()
{
    return JetDeepCSV() > 0.6321;
}

bool 
BaseLineMgr::PassDeepCSVL()
{
    return JetDeepCSV() > 0.2217;
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
    // removing PF isolation cut
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
           LepAbsEta() < 2.4
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

bool 
BaseLineMgr::IsInvTightMu()
{
    return PassMuTightID() &&
           PassMuTightKinematic() &&
           !PassMuTightISO()
    ;
}

bool
BaseLineMgr::IsPreTightMu()
{
    return PassMuTightID() &&
           PassMuTightKinematic()
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
BaseLineMgr::PassElLooseKinematic()
{
    return LepPt() > 15 &&
           LepAbsEta() < 2.4 &&
           !( LepAbsEta() > 1.44 && LepAbsEta() < 1.57 )
    ;
}

bool
BaseLineMgr::PassElLooseID()
{
    return ElIDLoose();
}

bool
BaseLineMgr::PassElCRLooseID()
{
    return ElIDCRLoose();
}

bool
BaseLineMgr::IsLooseEl()
{
    return PassElLooseID() &&
           PassElLooseKinematic()
    ;
}

bool
BaseLineMgr::IsCRLooseEl()
{
    return PassElCRLooseID() &&
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
    return LepPt() > 38 &&
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

bool
BaseLineMgr::IsInvTightEl()
{
    return PassImpactParameter() &&
           ElIDCRTight() &&
           PassElTightKinematic() &&
           !PassElTightISO()
    ;
}

bool
BaseLineMgr::IsPreTightEl()
{
    return PassImpactParameter() &&
           ElIDCRTight() &&
           PassElTightKinematic()
    ;
}

bool 
BaseLineMgr::PassElTightISO()
{
    if( LepAbsEta() <= 1.479 ){
        return ElPFISO() < 0.0287 + 0.506 / LepPt();
    }
    else{  //absEta > 1.479
        return ElPFISO() < 0.0445 + 0.963 / LepPt();
    }
}
