#include "CPVAnalysis/SampleMgr/interface/Histor.h"
#include "TFile.h"
using namespace std;

/*******************************************************************************
*   Initialization
*******************************************************************************/
Histor::Histor( const string& subdir, const string& json ) :
    Pathmgr( "CPVAnalysis", subdir ),
    Readmgr( SettingsDir() / json ),
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
    _sample = new BaseLineMgr( ch, sample );
}

/*******************************************************************************
*   Common
*******************************************************************************/
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

/*******************************************************************************
*   Looping event
*******************************************************************************/
void
Histor::process( const int& total, const int& progress )
{
    if( CheckOption( "count" ) ){
        printf( "[%d|%d]\r", total, progress );
        fflush( stdout );
    }
}

void
Histor::ChangeFile( const string& file )
{
    ChangeJSON( SettingsDir() / file );
}

void
Histor::GetEntry( const int& i )
{
    _sample->GetEntry( i );
}

/*******************************************************************************
*   Weight
*******************************************************************************/
void
Histor::InitBtagWeight( const string& wp, const string& type )
{
    _sample->InitBtagWeight( wp, type );
}

double
Histor::BtagScaleFactor( BTagEntry::OperatingPoint op, const int& idx )
{
    return _sample->BtagScaleFactor( op, idx );
}

double
Histor::GetSF( TH2D* hist, const int& idx )
{
    return _sample->GetSFTH2( hist, idx );
}

TH2D*
Histor::GetSFHist( const string& tag )
{
    string filename = mgr::GetSingle<string>( "file", GetSubTree( tag ) );
    string title    = mgr::GetSingle<string>( "title", GetSubTree( tag ) );

    TFile* f = TFile::Open( filename.c_str() );
    TH2D* h   = (TH2D*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory( 0 );
    f->Close();

    return h;
}

void
Histor::WeightMC( const string& sample )
{
    double lumi    = GetSingleData<double>( "lumi" );
    double xs      = mgr::GetSingle<double>( "cross_section", GetSubTree( sample ) );
    double gen_num = mgr::GetSingle<double>( "gen_num", GetSubTree( sample ) );

    Scale( ( lumi * xs ) / gen_num );
}

/*******************************************************************************
*   Weight
*******************************************************************************/
BaseLineMgr::MatchType
Histor::bbSeparation( const int& hb, const int& lb, const int& lep )
{
    return _sample->bbSeparation( hb, lb, lep );
}
