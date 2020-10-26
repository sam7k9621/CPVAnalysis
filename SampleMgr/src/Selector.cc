#include "CPVAnalysis/SampleMgr/interface/Selector.h"
#include "ManagerUtils/Common/interface/ProgressBar.hpp"
#include "TFile.h"
#include <algorithm>
#include <math.h>
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
    _eff_b  = NULL;
    _eff_c  = NULL;
    _eff_l  = NULL;
}

Selector::~Selector()
{
    delete _sample;
}

void
Selector::AddSample( TChain* ch, const bool& is_data, const string& sample )
{
    string readfile = is_data ? GetOption<string>( "year" ) + "DataSelection.py" : GetOption<string>( "year" ) + "MCSelection.py";
    _sample = new BaseLineMgr( readfile, sample );
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
Selector::GetResultsName( const string& type, const string& prefix, const bool& eos )
{
    string ans = OptName();
    string dir = IsLxplus() ? "/eos/cms/store/user/pusheng/public" / prefix : ResultsDir();

    if( prefix == "" ){
        ans.erase( ans.begin() );
    }

    if( type == "" ){
        return dir / ( prefix + ans );
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
        return GetOption<string>( opt ).find( content ) != std::string::npos;
    }

    return false;
}

double
Selector::GetZmass( const vector<int>& lepidx )
{
    return ( _sample->GetLepP4( lepidx[ 0 ] ) + _sample->GetLepP4( lepidx[ 1 ] ) ).M();
}

/*******************************************************************************
*   Weight
*******************************************************************************/
void
Selector::GetSelJet( vector<int>& jetlst, vector<int>& jetlst_test )
{
    for( int i = 0; i < _sample->Jsize(); i++ ){
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
    string variation = "central";
    if( GetBtagFlavor( jidx ) == BTagEntry::FLAV_UDSG && unc.find( "light" ) != string::npos ){
        variation = unc.substr( unc.find( "_" ) + 1 );
    }
    else if( GetBtagFlavor( jidx ) != BTagEntry::FLAV_UDSG && unc.find( "bc" ) != string::npos ){
        variation = unc.substr( unc.find( "_" ) + 1 );
    }
    double sf  = mc ? 1 : _sample->BtagScaleFactor( jidx, wp, GetBtagFlavor( jidx ), variation );
    double eps = GetJetSF( GetBtagEffPlot( jidx ), jidx );
    return sf * eps;
}

extern double
Selector::GetNonTaggedEff( const int& jidx, const bool& mc, const string& unc, const BTagEntry::OperatingPoint& wp )
{
    string variation = "central";
    if( GetBtagFlavor( jidx ) == BTagEntry::FLAV_UDSG && unc.find( "light" ) != string::npos ){
        variation = unc.substr( unc.find( "_" ) + 1 );
    }
    else if( GetBtagFlavor( jidx ) != BTagEntry::FLAV_UDSG && unc.find( "bc" ) != string::npos ){
        variation = unc.substr( unc.find( "_" ) + 1 );
    }
    double sf  = mc ? 1 : _sample->BtagScaleFactor( jidx, wp, GetBtagFlavor( jidx ), variation );
    double eps = GetJetSF( GetBtagEffPlot( jidx ), jidx );
    return 1 - sf * eps;
}

double
Selector::GetBtagWeight( const vector<int>& blst, const vector<int>& jlst, const string& unc )
{
    double prob_m = 1.;
    double prob_d = 1.;

    for( const auto& b : blst ){
        prob_m *= GetTaggedEff( b, true, unc );
        prob_d *= GetTaggedEff( b, false, unc );
    }

    for( const auto& j : jlst ){
        prob_m *= GetNonTaggedEff( j, true, unc );
        prob_d *= GetNonTaggedEff( j, false, unc );
    }

    return prob_d / prob_m;
}

double
Selector::GetBtagWeight_CR_0b( const vector<int>& jlst )
{
    double prob_m = 1.;
    double prob_d = 1.;

    for( const auto& j : jlst ){
        prob_m *= GetNonTaggedEff( j, true, "central", BTagEntry::OP_LOOSE );
        prob_d *= GetNonTaggedEff( j, false, "central", BTagEntry::OP_LOOSE );
    }

    return prob_d / prob_m;
}

double
Selector::GetBtagWeight_CR_1b( const vector<int>& blst, const vector<int>& jlst )
{
    double prob_m = 1.;
    double prob_d = 1.;

    prob_m *= GetTaggedEff( blst[0], true,  "central" );
    prob_d *= GetTaggedEff( blst[0], false, "central" );
    prob_m *= GetNonTaggedEff( blst[1], true,  "central" );
    prob_d *= GetNonTaggedEff( blst[1], false, "central" );

    for( const auto& j : jlst ){
        prob_m *= GetNonTaggedEff( j, true,  "central" );
        prob_d *= GetNonTaggedEff( j, false, "central" );
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
Selector::JECUpdate()
{
    _sample->JECUpdate();
}

void 
Selector::JECUncUpdate()
{
    _sample->JECUncUpdate();
}

void 
Selector::JECUncSrcUpdate()
{
    _sample->JECUncSrcUpdate();
}

void 
Selector::JERUpdate()
{
    _sample->JERUpdate();
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
Selector::PassFullJet_CR_1b( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    // list of index, csv value
    vector<tuple<int, float> > jetlst;
    
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
                jetlst.push_back( make_tuple( j, _sample->JetDeepCSV() ) );
            }
        }
    }

    if( jetlst.size() < 3 || bjetidx.size() != 1 ){
        return false;
    }

    std::sort( 
            begin( jetlst ), end( jetlst ), [ ]( const auto& t1, const auto& t2 ){
                return get<1>( t1 ) < get<1>( t2 );
            }
            );
    bjetidx.push_back( get<0>( jetlst.back() ) );
    jetlst.pop_back();

    for( const auto& j : jetlst ){
        jetidx.push_back( get<0>( j ) );
    }

    sort( jetidx.begin(), jetidx.end() );
    return true;
}

bool
Selector::PassFullJet_CR_0b( vector<int>& jetidx, const int& lepidx )
{
    for( int i = 0; i < _sample->Jsize(); i++ ){
        _sample->SetIndex( i );
        
        if( !_sample->IsIsoLepton( lepidx, i ) ){
            continue;
        }
        
        if( !_sample->IsSelJet() ){
            continue;
        }
        
        if( _sample->PassDeepCSVL() ){
            return false;
        }
        jetidx.push_back( i );
    }
    
    return jetidx.size() >= 4;
}
bool
Selector::PassFullJet_CRDYJets( vector<int>& jetidx )
{
    for( int i = 0; i < _sample->Jsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsSelJet_CRDYJets() ){
            jetidx.push_back( i );
        }
    }

    return jetidx.size() == 1;
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
    double topmass   = CheckOption( "topmass" ) ? GetOption<double>( "topmass" ) : 172.5;
    double width_shift = CheckOption( "deviation" ) ? GetOption<double>( "deviation" ) / 100 : 0;
    double wmass     = 82.9;
    double chi2mass  = INT_MAX;
    double had_tmass = 0;
    int had_b_idx    = -1;
    int jet1         = -1;
    int jet2         = -1;

    for( unsigned int i = 0; i < jethandle.size(); i++ ){
        for( unsigned int j = ( i + 1 ); j < jethandle.size(); j++ ){
            for( unsigned int k = 0; k < bjethandle.size(); k++ ){
                double t_mass = ( jethandle[ i ] + jethandle[ j ] + bjethandle[ k ] ).M();
                double w_mass = ( jethandle[ i ] + jethandle[ j ] ).M();
                double chi_t  = ( t_mass - topmass ) / ( 16.3 * ( 1 + width_shift ) );
                double chi_w  = ( w_mass - wmass ) / ( 9.5 * ( 1 + width_shift ) );

                if( ( chi_t * chi_t + chi_w * chi_w ) < chi2mass ){
                    chi2mass  = ( chi_t * chi_t + chi_w * chi_w );
                    had_tmass = t_mass;
                    had_b_idx = k;
                    jet1      = jetidx[i];
                    jet2      = jetidx[j];
                }
            }
        }
    }

    return make_tuple( chi2mass, had_tmass, had_b_idx, jet1, jet2 );
}

std::tuple<double, double, int, int, int>
Selector::GetChi2Info_CR_0b( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    vector<TLorentzVector> jethandle;
    for( const auto& j : jetidx ){
        jethandle.push_back( _sample->GetJetP4( j ) );
    }

    double chi2mass  = INT_MAX;
    double had_tmass = 0;
    int had_b        = -1;
    int jet1         = -1;
    int jet2         = -1;

    int count = 0;
    for( unsigned int i = 0; i < jethandle.size(); i++ ){
        for( unsigned int j = 0; j < jethandle.size(); j++ ){
            if( i == j ){
                continue;
            }
            
            for( unsigned int k = ( j + 1 ); k < jethandle.size(); k++ ){
                if( i == k ){
                    continue;
                }
                count++;
                double t_mass = ( jethandle[i] + jethandle[j] + jethandle[k] ).M();
                double w_mass = ( jethandle[j] + jethandle[k] ).M();

                double chi_t = ( t_mass - 172.5 ) / 16.3;
                double chi_w = ( w_mass - 82.9 ) / 9.5;

                if( ( chi_t * chi_t + chi_w * chi_w ) < chi2mass ){
                    chi2mass  = ( chi_t * chi_t + chi_w * chi_w );
                    had_tmass = t_mass;
                    had_b = jetidx[i];
                    jet1  = jetidx[j];
                    jet2  = jetidx[k];
                }

            }
        }
    }

    double deltaR = INT_MAX;
    int lep_b = -1;
    for( const auto& j : jetidx ){
        if( j == jet1 || j == jet2 || j == had_b ){
            continue;
        }
        
        if( _sample->Lep_Jet_DeltaR( lepidx, j ) < deltaR ){
            lep_b = j;
        }
    }
    bjetidx.push_back( had_b );
    bjetidx.push_back( lep_b );

    jetidx.erase(std::remove(jetidx.begin(), jetidx.end(), had_b), jetidx.end()); 
    jetidx.erase(std::remove(jetidx.begin(), jetidx.end(), lep_b), jetidx.end()); 

    return make_tuple( chi2mass, had_tmass, 0, jet1, jet2  );

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
