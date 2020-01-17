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
        ( "year,y", opt::value<string>()->required(), "which year" )
        ( "input,i", opt::value<vector<string> >()->multitoken(), "CEDM sample" )
        ( "chi2,u", opt::value<double>(), "chi2 upper cut" )
        ( "invChi2", opt::value<double>(), "inverse chi2 upper cut" )
        ( "region,r", opt::value<string>(), "which region" )
        ( "uncertainty,e", opt::value<string>(), "uncertainty shift" )
        ( "opt,o", opt::value<double>(), "lep_tmass optimization cut" )
        ( "Acp,a", opt::value<int>(), "intrinsic Acp(%)" )
        ( "mixed,m", opt::value<int>(), "shuffle with nearby events" )
        ( "0bjet", "0bjet" )
        ( "wopileup,p", "without pileup running" )
        ( "wobtag,w", "withoutn btag" )
        ( "bbSep,B", "bbSeparation method" )
        
        ( "TLV,L", "Get Acp TLorentzVector list" )
        ( "SIM,S", "check gen-level simulation Acp" )
    ;
    CompMgr( "CompareDataMC", "WeightInfo.py" ).AddOptions( de );
    const int run = CompMgr().ParseOptions( argc, argv );

    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }

    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }

    CompMgr().SetFileName( { "year", "lepton", "sample" } );
    CompMgr().AddCutName( { "test", "chi2", "invChi2", "bbSep", "Acp", "opt", "mixed", "region", "uncertainty", "wopileup", "wobtag","0bjet", "SIM", "TLV" } );

    if( CompMgr().CheckOption( "SIM" ) ){
        CompMgr().InitRoot( "CheckAcp.json" );
        CheckAcp();
    }
    else if( CompMgr().CheckOption( "TLV" ) ){
        StoreTLV();
    }
    else if( CompMgr().CheckOption( "Acp" ) ){
        ReweighAcp();
    }
    else{
        MakeHist();
    }
}
