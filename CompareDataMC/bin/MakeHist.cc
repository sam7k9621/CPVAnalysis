#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"

int
main( int argc, char* argv[] )
{
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "count,c", "count events" )
        ( "test,t", "run testing events number" )
        
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "sample,s", opt::value<string>()->required(), "which sample" )
        ( "input,i", opt::value<vector<string> >()->multitoken(), "CEDM sample" )
        ( "chi2,u", opt::value<double>(), "chi2 upper cut" )
        ( "invChi2,iu", opt::value<double>(), "inverse chi2 upper cut" )
        ( "region,r", opt::value<string>(), "which region" )
        ( "uncertainty,e", opt::value<string>(), "uncertainty shift" )
        ( "opt,o", opt::value<double>(), "lep_tmass optimization cut" )
        ( "HLT", "high level trigger" )
        ( "ISO", "isolated lepton" )
        ( "0bjet", "0bjet" )
        ( "pileup,p", "test pileup running" )
        ( "mixed,x", "randomly choose event" )
        ( "bbSep,B", "bbSeparation method" )
        
        ( "Acp,A", opt::value<double>(), "intrinsci acp(%)" )
        ( "SIM,S", "check gen-level simulation Acp" )
        ( "CEDM,C", "make CEDM model plot" )
    ;
    CompMgr( "CompareDataMC", "WeightInfo.json" ).AddOptions( de );
    const int run = CompMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    CompMgr().SetFileName( { "lepton", "sample" } );
    CompMgr().AddCutName( { "test", "chi2", "invChi2", "bbSep", "SIM", "CEDM", "Acp", "opt", "mixed", "region", "uncertainty", "pileup", "HLT", "0bjet", "ISO" } );

    if( CompMgr().CheckOption( "SIM" ) ){
        CompMgr().ChangeFile( "CheckAcp.json" );
        CheckAcp();
    }
    else if( CompMgr().CheckOption( "CEDM" ) ){
        CompMgr().ChangeFile( "CEDM.json" );
        CEDMPlot();
    }
    else if( CompMgr().CheckOption( "Acp" ) ){
        AddAcp();
        // ReweighAcp();
    }
    else{
        MakeHist();
    }
}
