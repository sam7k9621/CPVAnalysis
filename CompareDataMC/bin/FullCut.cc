#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "sample,s", opt::value<string>()->required(), "which sample" )
        ( "chi2,u",   opt::value<double>(), "chi2 upper cut" )
        ( "count,c", "count events" )
        ( "test,t", "run testing events number" )
    ;
    CompMgr( "CompareDataMC", "SampleInfo.json" ).AddOptions( de );
    const int run = CompMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    CompMgr().SetFileName( { "lepton", "sample" } );
    CompMgr().AddCutName ( { "chi2", "test" } );
    MakeFullCut();
}
