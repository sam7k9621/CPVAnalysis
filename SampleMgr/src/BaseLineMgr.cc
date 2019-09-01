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
    ReadConfig();
    _calib  = NULL;
}

BaseLineMgr::~BaseLineMgr()
{
    delete _calib;
}

void 
BaseLineMgr::AddVal( const string& PSet, const string& obj )
{
    _valmap.erase( PSet + "_" + obj );// deleting existing instance if already exist
    _valmap[ PSet + "_" + obj ] = GetParam<double>( PSet, obj );
}

double
BaseLineMgr::GetVal( const string& PSet, const std::string& obj )
{
    try{
        return _valmap.at( PSet + "_" + obj );
    }
    catch( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << endl;
        return INT_MIN;
    } 
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
           Ndof() >   GetVal( "PV", "Ndof" ) &&
           AbsZ() <   GetVal( "PV", "AbsZ" ) &&
           VtxRho() < GetVal( "PV", "VtxRho" )
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
        JetNHF() <           GetVal( "Jet", "NHF" ) &&
        JetNEF() <           GetVal( "Jet", "NEF" ) &&
        JetNConstituents() > GetVal( "Jet", "NConstituents" ) &&

        JetAbsEta() <=       GetVal( "Jet", "AbsEta" ) &&
        JetCHF() >           GetVal( "Jet", "CHF" ) &&
        JetNCH() >           GetVal( "Jet", "NCH" ) && 
        JetCEF() <           GetVal( "Jet", "CEF" )
    ;
}

bool
BaseLineMgr::PassJetKinematic()
{
    return JetPt() >     GetVal( "Jet", "Pt" ) &&
           JetAbsEta() < GetVal( "Jet", "AbsEta" )
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
           JetPt() >     GetVal( "Jet", "Pre_Pt" ) &&
           JetAbsEta() < GetVal( "Jet", "Pre_AbsEta" )
    ;
}

bool
BaseLineMgr::PassCSVM()
{
    return JetCSV() > GetVal( "Jet", "CSVM" );
}

bool 
BaseLineMgr::PassCSVL()
{
    return JetCSV() > GetVal( "Jet", "CSVL" );
}

bool 
BaseLineMgr::PassDeepCSVM()
{
    return JetDeepCSV() > GetVal( "Jet", "DeepCSVM" );
}

bool 
BaseLineMgr::PassDeepCSVL()
{
    return JetDeepCSV() > GetVal( "Jet", "DeepCSVL" );
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
    return LepPt() >     GetVal( "Mu", "LoosePt" ) &&
           LepAbsEta() < GetVal( "Mu", "LooseAbsEta" )
    ;
}

bool
BaseLineMgr::PassMuLooseISO()
{
    return RelIsoR04() < GetVal( "Mu", "LooseISO" );
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
           MuGlobalNormalizedChi2() <     GetVal( "Mu", "GlobalNormalizedChi2" ) &&
           MuNMuonhits() >                GetVal( "Mu", "MuNMuonhits" ) &&
           MuNMatchedStations() >         GetVal( "Mu", "MuNMatchedStations" ) &&
           AbsMuInnerTrackDxy_PV() <      GetVal( "Mu", "AbsMuInnerTrackDxy_PV" ) &&
           AbsMuInnerTrackDz() <          GetVal( "Mu", "AbsMuInnerTrackDz" ) &&
           MuNPixelLayers() >             GetVal( "Mu", "MuNPixelLayers" ) &&
           MuNTrackLayersWMeasurement() > GetVal( "Mu", "MuNTrackLayersWMeasurement" )
    ;
}

bool
BaseLineMgr::PassMuTightKinematic()
{
    return LepPt() >     GetVal( "Mu", "TightPt" ) &&
           LepAbsEta() < GetVal( "Mu", "TightAbsEta" )
    ;
}

bool
BaseLineMgr::PassMuTightISO()
{
    return RelIsoR04() < GetVal( "Mu", "TightISO" );
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
        ElsigmaIetaIeta() <           GetVal( "El", tag + pos + "ElsigmaIetaIeta" ) &&
        fabs( EldEtaInSeed() ) <      GetVal( "El", tag + pos + "EldEtaInSeed" ) &&
        fabs( EldPhiIn() ) <          GetVal( "El", tag + pos + "EldPhiIn" ) &&
        ElGsfEleHadronicOverEMCut() < GetVal( "El", tag  + pos + "HE_c0" ) + GetVal( "El", tag + pos + "HE_cE" ) / EnergySC() + GetVal( "El", tag + pos + "HE_cR" ) * EvtRho() / EnergySC() &&
        fabs( GsfEleEInverseMinusPInverseCut() ) < GetVal( "El", tag + pos + "GsfEleEInverseMinusPInverseCut" ) &&
        ElNumberOfExpectedInnerHits() <=           GetVal( "El", tag + pos + "ElNumberOfExpectedInnerHits" ) &&
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
        ElsigmaIetaIeta() <           GetVal( "El", tag + pos + "ElsigmaIetaIeta" ) &&
        fabs( EldEtaInSeed() ) <      GetVal( "El", tag + pos + "EldEtaInSeed" ) &&
        fabs( EldPhiIn() ) <          GetVal( "El", tag + pos + "EldPhiIn" ) &&
        ElGsfEleHadronicOverEMCut() < GetVal( "El", tag  + pos + "HE_c0" ) + GetVal( "El", tag + pos + "HE_cE" ) / EnergySC() + GetVal( "El", tag + pos + "HE_cR" ) * EvtRho() / EnergySC() &&
        fabs( GsfEleEInverseMinusPInverseCut() ) < GetVal( "El", tag + pos + "GsfEleEInverseMinusPInverseCut" ) &&
        ElNumberOfExpectedInnerHits() <=           GetVal( "El", tag + pos + "ElNumberOfExpectedInnerHits" ) &&
        !ElhasConv()
    ;
}

bool
BaseLineMgr::PassElLooseKinematic()
{
    return LepPt() >     GetVal( "El", "LoosePt" ) &&
           LepAbsEta() < GetVal( "El", "LooseAbsEta" ) &&
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
    return LepPt() >     GetVal( "El", "TightPt" ) &&
           LepAbsEta() < GetVal( "El", "TightAbsEta" ) &&
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

    return ElPFISO() < GetVal( "El", tag + pos + "ISOc0") + GetVal( "El", tag + pos + "ISOcpt" ) / LepPt();
}
