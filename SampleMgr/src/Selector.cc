#include "CPVAnalysis/SampleMgr/interface/Selector.h"
#include "ManagerUtils/Common/interface/ProgressBar.hpp"
#include <algorithm>
#include <math.h>
#include "TFile.h"
using namespace std;

/*******************************************************************************
*   Initialization
*******************************************************************************/
Selector::Selector( const string& subdir, const string& py ) :
    Pathmgr( "CPVAnalysis", subdir ),
    Readmgr( PythonDir() / py ),
    Parsermgr()
{
    _sample = NULL;
    _eff_b = NULL;
    _eff_c = NULL;
    _eff_l = NULL;
}

Selector::~Selector()
{
    delete _sample;
}

void
Selector::AddSample( TChain* ch, const string& sample )
{
    _sample = new BaseLineMgr( GetOption<string>( "year" ) + "Selection.py", sample );
    _sample->Register( ch );
}

void 
Selector::InitBtagEffPlot( TH2D* b, TH2D* c, TH2D* l )
{
    _eff_b = b;
    _eff_c = c;
    _eff_l = l;
}

/*******************************************************************************
*   Common
*******************************************************************************/
bool
Selector::IsGrid5()
{
    return ( HostName().find( "ntugrid5" ) != string::npos ) || ( HostName().find( "node" ) != string::npos );
}

bool
Selector::IsLxplus()
{
    return HostName().find( "cern" ) != string::npos;
}
    
string
Selector::GetResultsName( const string& type, const string& prefix, string dir )
{
    string ans = OptName();
    dir = IsLxplus() ? "/eos/cms/store/user/pusheng/public" / dir : ResultsDir();
    
    if( prefix == "" ){
        ans.erase( ans.begin() );
    }

    if( type == "" ){
        return  dir / ( prefix + ans );
    }
    else{
        return dir / ( prefix + ans + "." + type );
    }
}

string
Selector::Discript( TH1* h )
{
    string tag = _sample->GetTag();
    regex pattern( tag + "_" );
    return regex_replace( h->GetName(), pattern, "" );
}

bool
Selector::OptionContent( const string& opt, const string& content )
{
    if( CheckOption( opt ) ){
        if( GetOption<string>( opt ) == content ){
            return true;
        }
    }

    return false;
}

double 
Selector::GetZmass( const vector<int>& lepidx )
{
    return ( _sample->GetLepP4( lepidx[0] ) + _sample->GetLepP4( lepidx[1] ) ).M();
}

/*******************************************************************************
*   Weight
*******************************************************************************/
void 
Selector::GetSelJet( vector<int>& jetlst, vector<int>& jetlst_test )
{
    for( int i = 0; i< _sample->Jsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsSelJet() ){
            jetlst.push_back( i );
        }

        if( _sample->IsSelJet_test() ){
            jetlst_test.push_back( i );
        }
    }
}
   
void
Selector::Fill2DBtagEff( TEfficiency* eff_b, TEfficiency* eff_c, TEfficiency* eff_l, const vector<int>& jetlst, const double& csv )
{
    for( auto j : jetlst ){
        _sample->SetIndex( j );
        
        if( fabs( _sample->GenJetFlavor() ) == 5 ){
            eff_b->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetPt(), _sample->JetEta()
                    );
        }
        else if( fabs( _sample->GenJetFlavor() ) == 4 ){
            eff_c->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetPt(), _sample->JetEta()
                    );
        }
        else{
            eff_l->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetPt(), _sample->JetEta()
                    );
        }
    }
}

void
Selector::Fill1DBtagEff_Pt( TEfficiency* eff_b, TEfficiency* eff_c, TEfficiency* eff_l, const vector<int>& jetlst, const double& csv )
{
    for( auto j : jetlst ){
        _sample->SetIndex( j );
        
        if( fabs( _sample->GenJetFlavor() ) == 5 ){
            eff_b->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetPt()
                    );
        }
        else if( fabs( _sample->GenJetFlavor() ) == 4 ){
            eff_c->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetPt()
                    );
        }
        else{
            eff_l->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetPt()
                    );
        }
    }
}

void
Selector::Fill1DBtagEff_Eta( TEfficiency* eff_b, TEfficiency* eff_c, TEfficiency* eff_l, const vector<int>& jetlst, const double& csv )
{
    for( auto j : jetlst ){
        _sample->SetIndex( j );
        
        if( fabs( _sample->GenJetFlavor() ) == 5 ){
            eff_b->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetEta()
                    );
        }
        else if( fabs( _sample->GenJetFlavor() ) == 4 ){
            eff_c->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetEta()
                    );
        }
        else{
            eff_l->Fill( 
                    _sample->JetDeepCSV() >= csv,
                    _sample->JetEta()
                    );
        }
    }
}

double
Selector::GetLepSF( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->LepEta(), _sample->LepPt() );
}

double
Selector::GetLepSFUp( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->LepEta(), _sample->LepPt(), 1 );
}

double
Selector::GetLepSFDn( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->LepEta(), _sample->LepPt(), -1 );
}
double
Selector::GetJetSF( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->JetEta(), _sample->JetPt() );
}

double
Selector::GetJetSFUp( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->JetEta(), _sample->JetPt(), 1 );
}

double
Selector::GetJetSFDn( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->JetEta(), _sample->JetPt(), -1 );
}

TH2D*
Selector::GetSFHist( const string& tag )
{
    string filename = GetParam<string>( tag, "path" );
    string title    = GetParam<string>( tag, "title" );

    TFile* f = TFile::Open( filename.c_str() );
    TH2D* h  = (TH2D*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory( 0 );
    f->Close();

    return h;
}

void
Selector::InitBtagWeight( const string& wp, const string& type )
{
    _sample->InitBtagWeight( wp, type );
}

extern TH2D*
Selector::GetBtagEffPlot( const int& idx )
{
    _sample->SetIndex( idx );
    
    if( fabs( _sample->GenJetFlavor() ) == 5 ){
        return _eff_b;
    }
    else if( fabs( _sample->GenJetFlavor() ) == 4 ){
        return _eff_c;
    }
    else{
        return _eff_l;
    }
}

extern BTagEntry::JetFlavor
Selector::GetBtagFlavor( const int& idx )
{
    _sample->SetIndex( idx );
    
    if( fabs( _sample->GenJetFlavor() ) == 5 ){
        return BTagEntry::FLAV_B;
    }
    else if( fabs( _sample->GenJetFlavor() ) == 4 ){
        return BTagEntry::FLAV_C;
    }
    else{
        return BTagEntry::FLAV_UDSG;
    }
}

extern double 
Selector::GetTaggedEff( const int& jidx, const bool& mc, const string& unc, const BTagEntry::OperatingPoint& wp )
{
    double sf  = mc ? 1 : _sample->BtagScaleFactor( jidx, wp, GetBtagFlavor( jidx ), unc );
    double eps = GetJetSF( GetBtagEffPlot( jidx ), jidx );
    return sf * eps;
}

extern double 
Selector::GetNonTaggedEff( const int& jidx, const bool& mc, const string& unc, const BTagEntry::OperatingPoint& wp )
{
    double sf  = mc ? 1 : _sample->BtagScaleFactor( jidx, wp, GetBtagFlavor( jidx ), unc );
    double eps = GetJetSF( GetBtagEffPlot( jidx ), jidx );
    return ( 1 - sf * eps );
}

double 
Selector::GetBtagWeight( const vector<int>& blst, const vector<int>& jlst, const string& unc )
{
    double prob_m = 1.;
    double prob_d = 1.;

    for( const auto& b : blst ){
        prob_m *= GetTaggedEff( b, true,  unc ); 
        prob_d *= GetTaggedEff( b, false, unc ); 
    }
    for( const auto& j : jlst ){
        prob_m *= GetNonTaggedEff( j, true,  unc ); 
        prob_d *= GetNonTaggedEff( j, false, unc ); 
    }
    
    return prob_d / prob_m;
}

double 
Selector::GetBtagWeight_CR( const vector<int>& blst, const vector<int>& jlst )
{
    double prob_m = 1.;
    double prob_d = 1.;

    for( const auto& b : blst ){
        prob_m *= GetNonTaggedEff( b, true,  "central", BTagEntry::OP_LOOSE ); 
        prob_d *= GetNonTaggedEff( b, false, "central", BTagEntry::OP_LOOSE ); 
    }
    for( const auto& j : jlst ){
        prob_m *= GetNonTaggedEff( j, true,  "central", BTagEntry::OP_LOOSE ); 
        prob_d *= GetNonTaggedEff( j, false, "central", BTagEntry::OP_LOOSE ); 
    }
    
    return prob_d / prob_m;
}

/*******************************************************************************
*   Pre-selection
*******************************************************************************/
bool
Selector::IsGoodEvt( checkEvtTool& evt )
{
    return _sample->IsGoodEvt( evt );
}

void
Selector::JECCorrUp()
{
    _sample->JECUp();
}

void
Selector::JECCorrDn()
{
    _sample->JECDn();
}

void
Selector::JERCorr()
{
    _sample->JERCorr();
}

void
Selector::JERCorrUp()
{
    _sample->JERCorrUp();
}

void
Selector::JERCorrDn()
{
    _sample->JERCorrDn();
}

bool
Selector::PassVertex()
{
    for( int i = 0; i < _sample->Vsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsGoodPVtx() ){
            return true;
        }
    }

    return false;
}

bool
Selector::PassHLT( const vector<int>& hlt )
{
    return _sample->PassHLT( hlt );
}

bool
Selector::PreJet()
{
    int count = 0;

    for( int i = 0; i < _sample->Jsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsPreSelJet() ){
            count++;
        }
    }

    return count >= 4;
}

bool
Selector::PreLep()
{
    int count = 0;

    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsPreTightEl() || _sample->IsPreTightMu() ){
            count++;
        }
    }

    return count >= 1;
}

/*******************************************************************************
*   Pre-selection
*******************************************************************************/
bool
Selector::PassFullLepton( vector<int>& lepidx, const string& lepton )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        // assign Tight lepton
        if( lepton == "el" && _sample->IsTightEl() ){
            lepidx.push_back( i );
            continue;
        }
        else if( lepton == "mu" && _sample->IsTightMu() ){
            lepidx.push_back( i );
            continue;
        }

        // veto Loose lepton
        if( _sample->IsLooseMu() || _sample->IsLooseEl() ){
            return false;
        }
    }

    return lepidx.size() == 1;
}

bool 
Selector::PassFullLepton_CRWJets( vector<int>& lepidx, const string& lepton )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        // assign Tight lepton
        if( lepton == "el" && _sample->IsTightEl() ){
            lepidx.push_back( i );
            continue;
        }
        else if( lepton == "mu" && _sample->IsTightMu() ){
            lepidx.push_back( i );
            continue;
        }

        if( _sample->IsCRLooseMu() || _sample->IsCRLooseEl() ){
            return false;
        }
    }
    return lepidx.size() == 1;
}

bool
Selector::PassFullLepton_CRQCD( vector<int>& lepidx, const string& lepton )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        // assign Tight lepton
        if( lepton == "el" && _sample->IsInvTightEl() ){
            lepidx.push_back( i );
            continue;
        }
        else if( lepton == "mu" && _sample->IsInvTightMu() ){
            lepidx.push_back( i );
            continue;
        }

        // veto Loose lepton
        if( _sample->IsLooseMu() || _sample->IsLooseEl() ){
            return false;
        }
    }

    return lepidx.size() == 1;
}

bool 
Selector::PassFullLepton_CRDYJets( vector<int>& lepidx, const string& lepton )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if( lepton == "el" && _sample->IsTightEl() ){
            lepidx.push_back( i );
            continue;
        }
        else if( lepton == "mu" && _sample->IsTightMu() ){
            lepidx.push_back( i );
            continue;
        }
    }

    return lepidx.size() == 2;
}

bool
Selector::PassFullJet( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    for( int j = 0; j < _sample->Jsize(); j++ ){
        _sample->SetIndex( j );

        // Cleaning against leptons (isolated lepton)
        if( !_sample->IsIsoLepton( lepidx, j ) ){
            continue;
        }

        if( _sample->IsSelJet() ){
            if( _sample->PassDeepCSVM() ){
                bjetidx.push_back( j );
            }
            else{
                jetidx.push_back( j );
            }
        }
    }

    return ( jetidx.size() >= 2 ) && ( bjetidx.size() == 2 );
}

bool 
Selector::PassFullJet_CRDYJets( vector<int>& jetidx )
{
    for( int i = 0; i < _sample->Jsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsSelJet() ){
            jetidx.push_back( i );
        }
    }

    return jetidx.size() == 1;
}

bool
Selector::PassFullJet_CRWJets( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    // list of index, csv value
    vector<tuple<int, float> > jetlst;

    for( int j = 0; j < _sample->Jsize(); j++ ){
        _sample->SetIndex( j );

        // Cleaning against leptons (isolated lepton)
        if( !_sample->IsIsoLepton( lepidx, j ) ){
            continue;
        }

        if( !_sample->IsSelJet() ){
            continue;
        }
        
        // Rejecting events with selected jets loose b-tagged
        if( _sample->PassDeepCSVL() ){
            return false;
        }

        jetlst.push_back( make_tuple( j, _sample->JetDeepCSV() ) );
    }

    if( jetlst.size() < 4 ){
        return false;
    }

    // https://stackoverflow.com/questions/23030267/custom-sorting-a-vector-of-tuples
    std::sort(
        begin( jetlst ),
        end( jetlst ),
        [ ]( const auto& t1, const auto& t2 ){
        return get<1>( t1 ) < get<1>( t2 );
    }
        );

    // define the 2 max CSV value index as bjet
    bjetidx.push_back( get<0>( jetlst.back() ) );
    jetlst.pop_back();
    bjetidx.push_back( get<0>( jetlst.back() ) );
    jetlst.pop_back();

    for( const auto& j : jetlst ){
        jetidx.push_back( get<0>( j ) );
    }
    
    sort(jetidx.begin(), jetidx.end()); 

    return true;
}

bool
Selector::PassFullJet_CRQCD( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    // list of index, csv value
    vector<tuple<int, float> > jetlst;

    int bjet_count = 0;
    for( int j = 0; j < _sample->Jsize(); j++ ){
        _sample->SetIndex( j );
        
        // Cleaning against leptons (isolated lepton)
        if( !_sample->IsIsoLepton( lepidx, j ) ){
            continue;
        }

        if( !_sample->IsSelJet() ){
            continue;
        }
       
        // < 2 bjets in selected jets
        if( _sample->PassDeepCSVL() ){
            bjet_count++;
        }
        jetlst.push_back( make_tuple( j, _sample->JetDeepCSV() ) );
    }

    if( jetlst.size() < 4 || bjet_count >= 2 ){
        return false;
    }

    // https://stackoverflow.com/questions/23030267/custom-sorting-a-vector-of-tuples
    std::sort(
        begin( jetlst ),
        end( jetlst ),
        [ ]( const auto& t1, const auto& t2 ){
        return get<1>( t1 ) < get<1>( t2 );
    }
        );

    // define the 2 max CSV value index as bjet
    bjetidx.push_back( get<0>( jetlst.back() ) );
    jetlst.pop_back();
    bjetidx.push_back( get<0>( jetlst.back() ) );
    jetlst.pop_back();

    for( const auto& j : jetlst ){
        jetidx.push_back( get<0>( j ) );
    }

    sort(jetidx.begin(), jetidx.end()); 

    return true;
}
    
std::tuple<double, double, int, int, int>
Selector::GetChi2Info( const vector<int>& jetidx, const vector<int>& bjetidx )
{
    vector<TLorentzVector> jethandle;

    for( const auto& j : jetidx ){
        jethandle.push_back( _sample->GetJetP4( j ) );
    }

    vector<TLorentzVector> bjethandle;

    for( const auto& b : bjetidx ){
        bjethandle.push_back( _sample->GetJetP4( b ) );
    }

    // Mass constrain method - find hadronic b
    double chi2mass  = INT_MAX;
    double had_tmass = 0;
    int had_b        = -1;
    int jet1         = -1;
    int jet2         = -1;

    for( unsigned int i = 0; i < jethandle.size(); i++ ){
        for( unsigned int j = ( i + 1 ); j < jethandle.size(); j++ ){
            for( unsigned int k = 0; k < bjethandle.size(); k++ ){
                double t_mass = ( jethandle[ i ] + jethandle[ j ] + bjethandle[ k ] ).M();
                double w_mass = ( jethandle[ i ] + jethandle[ j ] ).M();
                double chi_t  = ( t_mass - 172.5 ) / 16.3;
                double chi_w  = ( w_mass - 82.9 ) / 9.5;

                if( ( chi_t * chi_t + chi_w * chi_w ) < chi2mass ){
                    chi2mass  = ( chi_t * chi_t + chi_w * chi_w );
                    had_tmass = t_mass;
                    had_b     = k;
                    jet1      = i;
                    jet2      = j;
                }
            }
        }
    }

    return make_tuple( chi2mass, had_tmass, had_b, jet1, jet2 );
}

double
Selector::GetLeptonicM( const int& lidx, const int& bidx )
{
    TLorentzVector lep  = _sample->GetLepP4( lidx );
    TLorentzVector bjet = _sample->GetJetP4( bidx );

    return ( lep + bjet ).M();
}

void
Selector::LeptonECorr()
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );
        _sample->ElEnergyCorr();
    }
}

/*******************************************************************************
*   Looping event
*******************************************************************************/
void
Selector::process( const int& total, const int& progress )
{
    if( CheckOption( "count" ) ){
        mgr::ProgressBar( progress, total );
    }
}
