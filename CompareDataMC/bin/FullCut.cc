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
        ( "pileup,p", "pile-up reweight" )
        ( "test,t", "run testing events number" )
        ( "makehist,m", "make hist" )
    ;
    CompMgr( "CompareDataMC", "SampleInfo.json" ).AddOptions( de );
    const int run = CompMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    //sample should always be the last term for data sorting
    CompMgr().SetFileName( { "lepton", "sample" } ); 
    CompMgr().AddCutName ( { "test", "pileup" } );
    
    if(!CompMgr().CheckOption("makehist"))
        MakeFullCut();
    else
        MakeHist();
}
