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

void 
Selector::RegisterWeight(TTree* root, float* weight)
{
    root->SetBranchAddress( "PUWeight", weight );
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
Selector::ChangeFile(const string& file)
{
    ChangeJSON( SettingsDir() / file );
}

void
Selector::AddSample( const string& sample, TChain* ch )
{
    _samplelst.emplace_back( new BaseLineMgr( sample, ch ) );
}

vector<TH1*>
Selector::GetHistlst(const std::string& name)
{
    vector<TH1*> lst;
    for( auto s : _samplelst){
        lst.push_back( s->Hist(name) );
    }

    return lst;
}

string
Selector::Discript( TH1* h )
{
    string tag = GetSample()->GetTag();
    regex pattern( tag + "_" );
    return regex_replace( h->GetName(), pattern, "" );
}
