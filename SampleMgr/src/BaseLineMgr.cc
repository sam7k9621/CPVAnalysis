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
BaseLineMgr::BaseLineMgr( const string& py, const string& sample ) :
    Pathmgr( "CPVAnalysis", "SampleMgr" ),
    Readmgr( PythonDir() / py ),
    Hist2DMgr( sample ),
    HistMgr( sample )
{
    InitRoot( "process" );
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
           Ndof() >   GetParam<double>( "PV", "Ndof" ) &&
           AbsZ() <   GetParam<double>( "PV", "AbsZ" ) &&
           VtxRho() < GetParam<double>( "PV", "VtxRho" )
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
        JetNHF() <           GetParam<double>( "Jet", "NHF" ) &&
        JetNEF() <           GetParam<double>( "Jet", "NEF" ) &&
        JetNConstituents() > GetParam<double>( "Jet", "NConstituents" ) &&

        JetAbsEta() <=       GetParam<double>( "Jet", "AbsEta" ) &&
        JetCHF() >           GetParam<double>( "Jet", "CHF" ) &&
        JetNCH() >           GetParam<double>( "Jet", "NCH" ) && 
        JetCEF() <           GetParam<double>( "Jet", "CEF" )
    ;
}

bool
BaseLineMgr::PassJetKinematic()
{
    return JetPt() >     GetParam<double>( "Jet", "Pt" ) &&
           JetAbsEta() < GetParam<double>( "Jet", "AbsEta" )
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
           JetPt() >     GetParam<double>( "Jet", "Pre_Pt" ) &&
           JetAbsEta() < GetParam<double>( "Jet", "Pre_AbsEta" )
    ;
}

bool
BaseLineMgr::PassCSVM()
{
    return JetCSV() > GetParam<double>( "Jet", "CSVM" );
}

bool 
BaseLineMgr::PassCSVL()
{
    return JetCSV() > GetParam<double>( "Jet", "CSVL" );
}

bool 
BaseLineMgr::PassDeepCSVM()
{
    return JetDeepCSV() > GetParam<double>( "Jet", "DeepCSVM" );
}

bool 
BaseLineMgr::PassDeepCSVL()
{
    return JetDeepCSV() > GetParam<double>( "Jet", "DeepCSVL" );
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
    return LepPt() >     GetParam<double>( "Mu", "LoosePt" ) &&
           LepAbsEta() < GetParam<double>( "Mu", "LooseAbsEta" )
    ;
}

bool
BaseLineMgr::PassMuLooseISO()
{
    return RelIsoR04() < GetParam<double>( "Mu", "LooseISO" );
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
           MuGlobalNormalizedChi2() <     GetParam<double>( "Mu", "GlobalNormalizedChi2" ) &&
           MuNMuonhits() >                GetParam<double>( "Mu", "MuNMuonhits" ) &&
           MuNMatchedStations() >         GetParam<double>( "Mu", "MuNMatchedStations" ) &&
           AbsMuInnerTrackDxy_PV() <      GetParam<double>( "Mu", "AbsMuInnerTrackDxy_PV" ) &&
           AbsMuInnerTrackDz() <          GetParam<double>( "Mu", "AbsMuInnerTrackDz" ) &&
           MuNPixelLayers() >             GetParam<double>( "Mu", "MuNPixelLayers" ) &&
           MuNTrackLayersWMeasurement() > GetParam<double>( "Mu", "MuNTrackLayersWMeasurement" )
    ;
}

bool
BaseLineMgr::PassMuTightKinematic()
{
    return LepPt() >     GetParam<double>( "Mu", "TightPt" ) &&
           LepAbsEta() < GetParam<double>( "Mu", "TightAbsEta" )
    ;
}

bool
BaseLineMgr::PassMuTightISO()
{
    return RelIsoR04() < GetParam<double>( "Mu", "TightISO" );
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
BaseLineMgr::ElIDCRTight()
{
    string tag = "Tight_";
    string pos = LepAbsEta() <= 1.479 ? "Bar_" : "End_";

    // removing PF isolation cut
    return
        ElsigmaIetaIeta() <           GetParam<double>( "El", tag + pos + "ElsigmaIetaIeta" ) &&
        fabs( EldEtaInSeed() ) <      GetParam<double>( "El", tag + pos + "EldEtaInSeed" ) &&
        fabs( EldPhiIn() ) <          GetParam<double>( "El", tag + pos + "EldPhiIn" ) &&
        ElGsfEleHadronicOverEMCut() < GetParam<double>( "El", tag  + pos + "HE_c0" ) + GetParam<double>( "El", tag + pos + "HE_cE" ) / EnergySC() + GetParam<double>( "El", tag + pos + "HE_cR" ) * EvtRho() / EnergySC() &&
        fabs( GsfEleEInverseMinusPInverseCut() ) < GetParam<double>( "El", tag + pos + "GsfEleEInverseMinusPInverseCut" ) &&
        ElNumberOfExpectedInnerHits() <=           GetParam<double>( "El", tag + pos + "ElNumberOfExpectedInnerHits" ) &&
        !ElhasConv()
    ;
}
    
bool
BaseLineMgr::ElIDCRLoose()
{
    string tag = "Loose_";
    string pos = LepAbsEta() <= 1.479 ? "Bar_" : "End_";

    // removing PF isolation cut
    return
        ElsigmaIetaIeta() <           GetParam<double>( "El", tag + pos + "ElsigmaIetaIeta" ) &&
        fabs( EldEtaInSeed() ) <      GetParam<double>( "El", tag + pos + "EldEtaInSeed" ) &&
        fabs( EldPhiIn() ) <          GetParam<double>( "El", tag + pos + "EldPhiIn" ) &&
        ElGsfEleHadronicOverEMCut() < GetParam<double>( "El", tag  + pos + "HE_c0" ) + GetParam<double>( "El", tag + pos + "HE_cE" ) / EnergySC() + GetParam<double>( "El", tag + pos + "HE_cR" ) * EvtRho() / EnergySC() &&
        fabs( GsfEleEInverseMinusPInverseCut() ) < GetParam<double>( "El", tag + pos + "GsfEleEInverseMinusPInverseCut" ) &&
        ElNumberOfExpectedInnerHits() <=           GetParam<double>( "El", tag + pos + "ElNumberOfExpectedInnerHits" ) &&
        !ElhasConv()
    ;
}

bool
BaseLineMgr::PassElLooseKinematic()
{
    return LepPt() >     GetParam<double>( "El", "LoosePt" ) &&
           LepAbsEta() < GetParam<double>( "El", "LooseAbsEta" ) &&
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
    return LepPt() >     GetParam<double>( "El", "TightPt" ) &&
           LepAbsEta() < GetParam<double>( "El", "TightAbsEta" ) &&
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
    string tag = "Tight_";
    string pos = LepAbsEta() <= 1.479 ? "Bar_" : "End_";

    return ElPFISO() < GetParam<double>( "El", tag + pos + "ISOc0") + GetParam<double>( "El", tag + pos + "ISOcpt" ) / LepPt();
}
