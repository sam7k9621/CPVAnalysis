#include <algorithm>
#include <math.h>
#include "CPVAnalysis/SampleMgr/interface/Selector.h"

using namespace std;

/*******************************************************************************
*   Initialization
*******************************************************************************/
Selector::Selector( const string& subdir, const string& json ) :
    Pathmgr( "CPVAnalysis", subdir ),
    Readmgr( SettingsDir() / json ),
    Parsermgr()
{
    _sample = NULL;
}

Selector::~Selector()
{
    delete _sample;
}

void
Selector::AddSample( TChain* ch )
{
    _sample = new BaseLineMgr();
    _sample->Register( ch );
}

/*******************************************************************************
*   Common
*******************************************************************************/
string
Selector::GetResultsName( const string& type, const string& prefix )
{
    string ans = OptName();

    if( prefix == "" ){
        ans.erase( ans.begin() );
    }

    if( type == "" ){
        return ResultsDir() / ( prefix + ans );
    }
    else{
        return ResultsDir() / ( prefix + ans + "." + type );
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
        if( GetOption<string>( opt ) == content )
            return true;
    }

    return false;
}


  void Selector::SetME_PSUnc( float& up, float& dn )
 {
     up = _sample->LHESystematicWeights( 245 );
     dn = _sample->LHESystematicWeights( 236 );
 }

  void Selector::SetMuFMuRUnc( float* unclst )
 {
     //https://twiki.cern.ch/twiki/bin/view/CMS/LHEReaderCMSSW
     //http://www.hep.wisc.edu/~nsmith/llvvAnalysis/lheWeights_Signal.txt
     for( int i = 0; i < 9; i++){
         unclst[i] = _sample->LHESystematicWeights( i );
     }
 }

 void Selector::SetPDFUnc( float& up, float& dn )
{
    // https://indico.cern.ch/event/494682/contributions/1172505/attachments/1223578/1800218/mcaod-Feb15-2016.pdf
    up = 0;
    dn = 0;
    int count_up = 0;
    int count_dn = 0;

     // 9-108 PDF set
    for(int i =9; i<109; i++){ 
        float unc = _sample->LHESystematicWeights( i );
        if( unc >= 1 ){
            up += ( unc * unc );
            count_up++;
        }
        else{
            dn += ( unc * unc );
            count_dn++;
        }
    }

     up = fabs( 1 - sqrt( up / count_up ) );
    dn = fabs( 1 - sqrt( dn / count_dn ) );

     // 109 as_0117
    // 110 as_0119
    for( int i = 109; i<111; i++){
        float unc = fabs( 1 - _sample->LHESystematicWeights( i ) );
        if( unc >= 0 ){
            up += ( unc * unc );
        }
        else{
            dn += ( unc * unc );
        }
    }

     up = 1 + up;
    dn = 1 - dn;
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
    return _sample->Jsize() >= 4;
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
Selector::PassFullEl( vector<int>& lepidx )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsTightEl() ){
            lepidx.push_back( i );
            continue;
        }

        if( _sample->IsLooseMu() || _sample->IsLooseEl() ){
            return false;
        }
    }

    return lepidx.size() == 1;
}

bool
Selector::PassFullCREl( vector<int>& lepidx )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->IsTightEl() ){
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
Selector::PassFullMu( vector<int>& lepidx )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if(  _sample->IsTightMu() ){
            lepidx.push_back( i );
            continue;
        }

        if( _sample->IsLooseMu() || _sample->IsLooseEl() ){
            return false;
        }
    }

    return lepidx.size() == 1;
}

bool
Selector::PassFullCRMu( vector<int>& lepidx )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if(  _sample->IsTightMu() ){
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
Selector::PassFullJet( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    for( int j = 0; j < _sample->Jsize(); j++ ){
        _sample->SetIndex( j );

        // Cleaning against leptons (isolated lepton)
        if( !_sample->IsIsoLepton( lepidx, j ) ){
            continue;
        }

        int mask = 0x01;

        if( _sample->IsSelJet() ){
            mask <<= 1;
        }

        if( _sample->PassBJet() ){
            mask <<= 2;
        }

        if( mask & 0x02 ){
            jetidx.push_back( j );
        }
        else if( mask & 0x08 ){
            bjetidx.push_back( j );
        }
    }

    return jetidx.size() >= 2 && bjetidx.size() == 2;
}

bool
Selector::PassFullCRJet( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    // list of index, csv value
    vector< tuple<int, float> > jetlst;

    for( int j = 0; j < _sample->Jsize(); j++ ){
        _sample->SetIndex( j );

        // Rejecting events containing any b-tagged jet
        if( !_sample->RejectBJet() ){
            return false;
        }

        // Cleaning against leptons (isolated lepton)
        if( !_sample->IsIsoLepton( lepidx, j ) ){
            continue;
        }

        if( !_sample->IsSelJet() ){
            continue;
        }

        jetlst.push_back( make_tuple( j, _sample->JetCSV() ) );    
    }
    
    if( jetlst.size() < 4 ){
        return false;
    }

    //https://stackoverflow.com/questions/23030267/custom-sorting-a-vector-of-tuples
    std::sort( 
        begin(jetlst),
        end(jetlst),
        [](  const auto& t1, const auto& t2 ){
            return get<1>(t1) < get<1>(t2);
        }
    );

    //define the 2 max CSV value index as bjet
    bjetidx.push_back( get<0>( jetlst.back() ) );
    jetlst.pop_back();
    bjetidx.push_back( get<0>( jetlst.back() ) );
    jetlst.pop_back();

    for(const auto& j : jetlst ){
        jetidx.push_back( get<0>( j ) );
    }

    return true;
}

bool
Selector::PassFullCR2Jet( vector<int>& jetidx, vector<int>& bjetidx, const int& lepidx )
{
    for( int j = 0; j < _sample->Jsize(); j++ ){
        _sample->SetIndex( j );

        // Cleaning against leptons (isolated lepton)
        if( !_sample->IsIsoLepton( lepidx, j ) ){
            continue;
        }

        int mask = 0x01;

        if( _sample->IsSelJet() ){
            mask <<= 1;
        }

        if( _sample->PassCS2BJet() ){
            mask <<= 2;
        }

        if( mask & 0x02 ){
            jetidx.push_back( j );
        }
        else if( mask & 0x08 ){
            bjetidx.push_back( j );
        }
    }

    return jetidx.size() >= 2 && bjetidx.size() == 2;
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
    int    had_b     = -1;
    int    jet1      = -1;
    int    jet2      = -1;

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
    for( int i=0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );
        cout<<"before "<<_sample->LepPt()<<endl;
        _sample->ElEnergyCorr();
        cout<<"after  "<<_sample->LepPt()<<endl;
    }
}

/*******************************************************************************
*   Looping event
*******************************************************************************/
void
Selector::process( const int& total, const int& progress )
{
    if( CheckOption( "count" ) ){
        printf( "[%d|%d]\r", total, progress );
        fflush( stdout );
    }
}

void
Selector::ChangeFile( const string& file )
{
    ChangeJSON( SettingsDir() / file );
}
