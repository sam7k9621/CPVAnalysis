#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "source,s", opt::value<string>()->required(), "Is data or MC" )
        ( "count,c", "count events" )
        ( "test,t", "run testing events number" )
        ( "chi2,u", opt::value<double>(), "chi2 upper cut" )
        ( "deltaR,r", opt::value<double>(), "deltaR matching upper cut" )
        ( "vertex,v", opt::value<double>(), "vertex per events" )
    ;
    CompMgr( "CompareDataMC" ).AddOptions( de );
    const int run = CompMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    CompMgr().SetFileName( { "lepton", "source" } );
    MakeFullCut();
}