#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"

using namespace std;

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for Pre-selection" );
    de.add_options()
        ( "sample,s", opt::value<string>()->required(), "which sample" )
        ( "year,y", opt::value<string>()->required(), "which year" )
        ( "lepton,l", opt::value<string>(), "which lepton" )
        ( "CSV", opt::value<double>(), "discriminator" )
        ( "count,c", "count events" )
        ( "datacard,d", "only produce datacard" )
        ( "test,t", "run testing events number" )
        ( "BWEIGHT,W", "check b-tag weight" )
    ;
    PreMgr( "BaseLineSelector", "SampleInfo.py" ).AddOptions( de );
    const int run = PreMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    PreMgr().SetFileName( { "year", "lepton", "sample" } );
    PreMgr().AddCutName( { "test", "CSV" } );
    if( PreMgr().CheckOption( "datacard" ) ){
        MakeDataCard();
    }
    else if( PreMgr().CheckOption( "CSV" ) ){
        MakeBtagEff();
    }
    else if( PreMgr().CheckOption( "BWEIGHT" ) ){
        CheckBtag();
    }
    else{
        MakePreCut();
    }
}
