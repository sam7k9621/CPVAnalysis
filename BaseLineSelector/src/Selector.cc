#include "CPVAnalysis/BaseLineSelector/interface/Selector.h"

using namespace std;

/*******************************************************************************
*   Initialization
*******************************************************************************/
Selector::Selector( const string& subdir, const string& json ) :
    Pathmgr( "CPVAnalysis", subdir ),
    Readmgr( SettingsDir() / json ),
    Parsermgr()
{
}

Selector::~Selector()
{
}

void 
Selector::RegisterWeight(TTree* root, float* weight)
{
    root->SetBranchAddress( "PUWeight", weight );
}

void
Selector::AddSample( const string& sample, TChain* ch )
{
    _sample = new BaseLineMgr( sample, ch ) ;
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

    return ResultsDir() / ( prefix + ans + "." + type );
}

string
Selector::Discript( TH1* h )
{
    string tag = GetSample()->GetTag();
    regex pattern( tag + "_" );
    return regex_replace( h->GetName(), pattern, "" );
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

void
Selector::ChangeFile(const string& file)
{
    ChangeJSON( SettingsDir() / file );
}

void 
Selector::GetEntry(const int& i)
{
    GetSample()->GetEntry(i);
}
