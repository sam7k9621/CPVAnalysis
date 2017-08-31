#include "CPVAnalysis/MassReco/interface/MassReco.h"

using namespace std;
using namespace sel;
using namespace mgr;


int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "source,s", opt::value<string>()->required(), "Is data or MC" )
        ( "check,c", opt::value<string>(), "nope" )
    ;
    SelMgr( "MassReco" ).AddOptions( de );
    const int run = SelMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    SelMgr().SetFileName( { "lepton", "source" } );
    string source   = SelMgr().GetOption<string>( "source" );
    vector<int> hlt = GetList<int>( "HLT", SelMgr().GetSubTree( source ) );

    for( auto c : hlt ){
        cout << c << endl;
    }
}
