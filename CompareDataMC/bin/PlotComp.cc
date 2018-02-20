#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "chi2,u",   opt::value<double>(), "chi2 upper cut" )
        ( "pileup,p", "pile-up reweight" )
    ;
    PlotMgr( "CompareDataMC", "PlotInfo.json" ).AddOptions( de );
    const int run = PlotMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    PlotMgr().SetFileName( { "lepton" } );
    PlotMgr().AddCutName ( {  "pileup", "chi2" } );
    MakePlotCompare();
}
