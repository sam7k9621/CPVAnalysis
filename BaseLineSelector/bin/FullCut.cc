#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"

using namespace std;

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
    ;
    FullMgr( "BaseLineSelector", "SampleInfo.json" ).AddOptions( de );
    const int run = FullMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    //sample should always be the last term for data sorting
    FullMgr().SetFileName( { "lepton", "sample" } ); 
    FullMgr().AddCutName ( { "test", "pileup" } );
    
    MakeFullCut();
}
