#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "mc,m",   opt::value<string>()->required(), "which mc" )
        ( "data,d", opt::value<string>()->required(), "which data" )
        ( "chi2,u", opt::value<double>(), "chi2 upper cut" )
        ( "fitted,f", opt::value<double>(), "background fitted event #" )
        ( "region,r", opt::value<string>(), "CS or SR" )
        ( "uncertainty,e", opt::value<string>(), "systemaitc uncertainty" )
        ( "Acp,A", opt::value<double>(), "intrinsic Acp" )
        ( "SIM,S", "check simulation Acp")
        ( "Opt,o",  opt::value<double>(), "lep_tmass optimization cut" )
        ( "bbSep,B", "bbseparation method")
        ( "mixed,x", "randomly choose event" )
        ( "pileup,p", "randomly choose event" )
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
    PlotMgr().AddCutName( { "chi2", "bbSep", "uncertainty", "Acp", "SIM", "Opt", "mixed", "region", "pileup" } );
    MakePlotCompare();
}
