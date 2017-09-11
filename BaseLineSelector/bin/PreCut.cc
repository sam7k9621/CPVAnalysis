#include "CPVAnalysis/BaseLineSelector/interface/PreSelection.h"

using namespace std;

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for Pre-selection" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "source,s", opt::value<string>()->required(), "which era of data or mc" )
        ( "count,c", "count events" )
        ( "test,t", "run testing events number" )
    ;
    PreMgr( "BaseLineSelector" ).AddOptions( de );
    const int run = PreMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    PreMgr().SetFileName( { "lepton", "source" } );
    PreMgr().AddCutName( { "test" } );
    MakePreCut();
}
