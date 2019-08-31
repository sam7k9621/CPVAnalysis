#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"

using namespace std;

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for Pre-selection" );
    de.add_options()
        ( "sample,s", opt::value<string>()->required(), "which sample" )
        ( "year,y", opt::value<string>()->required(), "which year" )
        ( "count,c", "count events" )
        ( "test,t", "run testing events number" )
    ;
    PreMgr( "BaseLineSelector", "SampleInfo.json" ).AddOptions( de );
    const int run = PreMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    PreMgr().SetFileName( { "year", "sample" } );
    PreMgr().AddCutName( { "test" } );
    MakePreCut();
}
