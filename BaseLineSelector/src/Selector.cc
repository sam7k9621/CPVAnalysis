#include "CPVAnalysis/BaseLineSelector/interface/Selector.h"

using namespace std;

/*******************************************************************************
*   Initialization
*******************************************************************************/
Selector::Selector( const string& subdir ) :
    Pathmgr( "CPVAnalysis", subdir ),
    Readmgr( SettingsDir() / "Selection.json" ),
    Parsermgr()
{
}

Selector::~Selector()
{
}

/*******************************************************************************
*   Common
*******************************************************************************/
void
Selector::process( const int& total, const int& progress )
{
    if( CheckOption( "count" ) ){
        printf( "[%d|%d]\r", total, progress );
        fflush( stdout );
    }
}

string
Selector::GetResultsName( const string& type, const string& prefix )
{
    string ans = OptName();

    if( prefix == "" ){
        ans.erase( ans.begin() );
    }

    return ResultsDir() / ( prefix + ans + "." + type );
}

void
Selector::AddSample( const string& sample, TChain* ch )
{
    _samplelst.emplace_back( new BaseLineMgr( sample, ch ) );
}

string
Selector::Discript(TH1* h)
{
    string tag = GetSample()->GetTag();
    regex pattern( tag + "_" );
    return regex_replace( h->GetName(), pattern, "" );
}