#include "CPVAnalysis/SampleMgr/interface/Histor.h"
#include "ManagerUtils/Common/interface/ProgressBar.hpp"
#include "TFile.h"
using namespace std;

/*******************************************************************************
*   Initialization
*******************************************************************************/
Histor::Histor( const string& subdir, const string& py ) :
    Pathmgr( "CPVAnalysis", subdir ),
    Readmgr( PythonDir() / py ),
    Parsermgr()
{
    _sample = NULL;
}

Histor::~Histor()
{
    delete _sample;
}

void
Histor::AddSample( const string& sample, TChain* ch )
{
    _sample = new BaseLineMgr( GetOption<string>( "year") + "Selection.py", sample );
    _sample->Register( ch );
}

/*******************************************************************************
*   Common
*******************************************************************************/
void
Histor::InitBtagWeight( const string& wp, const string& type )
{
    _sample->InitBtagWeight( wp, type );
}

BTagEntry::JetFlavor
Histor::GetBtagFlavor( const int& idx )
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

double 
Histor::GetBtagSF( const int& jidx )
{
    return _sample->BtagScaleFactor( jidx, BTagEntry::OP_LOOSE, GetBtagFlavor( jidx ), "central" );
}

bool 
Histor::PassHLT( const vector<int>& hlt )
{
    return _sample->PassHLT( hlt );
}
   
bool 
Histor::PassISOLepton( const int& jid, const int& lid )
{
    return _sample->IsIsoLepton( lid, jid );
}

bool 
Histor::HasLooseB( const initializer_list<int>& list )
{
    for( const auto& l : list ){
        _sample->SetIndex( l );
        if( _sample->PassDeepCSVL() ){
            return true;
        }
    }

    return false;
}

string
Histor::GetResultsName( const string& type, const string& prefix )
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
Histor::Discript( TH1* h )
{
    string tag = _sample->GetTag();
    regex pattern( tag + "_" );
    return regex_replace( h->GetName(), pattern, "" );
}

void
Histor::AddHist(
    const string& label,
    const string& xtitle,
    const string& ytitle,
    const int     bin_size,
    const double  x_lower,
    const double  x_upper
    )
{
    _sample->AddHist( label, xtitle, ytitle, bin_size, x_lower, x_upper );
}

void
Histor::AddHist2D(
    const std::string& label,
    const std::string& xtitle,
    const std::string& ytitle,
    const unsigned     xbin,
    const double       xmin,
    const double       xmax,
    const unsigned     ybin,
    const double       ymin,
    const double       ymax
    )
{
    _sample->AddHist2D( label, xtitle, ytitle, xbin, xmin, xmax, ybin, ymin, ymax );
}

float 
Histor::GetMuISO( const int& i )
{
    _sample->SetIndex( i );
    return _sample->RelIsoR04();
}

float 
Histor::GetElISO( const int& i )
{
    _sample->SetIndex( i );
    return _sample->ElPFISO();
}

/*******************************************************************************
*   Looping event
*******************************************************************************/
void
Histor::process( const int& total, const int& progress )
{
    if( CheckOption( "count" ) ){
        mgr::ProgressBar( progress, total );
    }
}

/*******************************************************************************
*   Weight
*******************************************************************************/
double
Histor::GetLepSF( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->LepEta(), _sample->LepPt() );
}

double
Histor::GetLepSFUp( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->LepEta(), _sample->LepPt(), 1 );
}

double
Histor::GetLepSFDn( TH2D* hist, const int& idx )
{
    _sample->SetIndex( idx );
    return _sample->GetSFTH2( hist, _sample->LepEta(), _sample->LepPt(), -1 );
}

TH2D*
Histor::GetSFHist( const string& tag )
{
    string filename = GetParam<string>( tag, "path" );
    string title    = GetParam<string>( tag, "title" );

    TFile* f = TFile::Open( filename.c_str() );
    TH2D* h  = (TH2D*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory( 0 );
    f->Close();

    return h;
}

double
Histor::TopPtWeight()
{
    return _sample->TopPtWeight();
}

float 
Histor::PDFWeight( const int& idx )
{
    return _sample->PDFWeight( idx );
}

float 
Histor::muFmuRWeight( const int& idx )
{
    return _sample->muFmuRWeight( idx );
}

void
Histor::WeightMC( const string& sample )
{
    double lumi    = GetParam<double>( "Info", "lumi" );
    double gen_num = GetParam<double>( sample, "evt" );
    double xs      = GetParam<double>( sample, "xsec" );

    Scale( ( lumi * xs ) / gen_num );
}
