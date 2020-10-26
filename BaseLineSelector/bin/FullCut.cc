#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"

using namespace std;

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "sample,s", opt::value<string>()->required(), "which sample" )
        ( "year,y", opt::value<string>()->required(), "which sample" )
        ( "lepton,l", opt::value<string>(), "which lepton" )
        ( "uncertainty,e", opt::value<string>(), "uncertainty" )
        ( "region,r", opt::value<string>(), "which region" )
        ( "topmass,m", opt::value<double>(), "simulated top mass" )
        ( "deviation,d", opt::value<double>(), "simulated width shift" )
        ( "count,c", "count events" )
        ( "test,t", "run testing events number" )
    ;
    FullMgr( "BaseLineSelector", "SampleInfo.py" ).AddOptions( de );
    const int run = FullMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    FullMgr().SetFileName( { "year", "lepton", "sample","uncertainty" } );
    FullMgr().AddCutName( { "test", "region", "topmass", "deviation" } );

    if( FullMgr().CheckOption( "BWEIGHT" ) ){
        CheckBtag();
    }
    else{
        MakeFullCut();
    }
}
