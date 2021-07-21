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
        ( "mixed,m", opt::value<int>(), "shuffle with nearby events" )
        ( "topmass,p", opt::value<string>(), "top mass in chi2" )
        ( "WHF,w", opt::value<int>(), "raise WHF fraction" )
        ( "wopileup", "without pileup running" )
        ( "wobtag", "withoutn btag" )
        ( "bias,b", opt::value<double>(), "intrinsic detector bias" )
        ( "bbSep,B", "bbSeparation method" )

        ( "Acp,a", opt::value<int>(), "intrinsic Acp(%)" )
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
    CompMgr().AddCutName( { "test", "chi2", "invChi2", "bbSep", "Acp", "opt", "mixed", "region", "topmass", "uncertainty", "wopileup", "wobtag", "0bjet", "SIM", "TLV", "WHF", "bias" } );

    if( CompMgr().CheckOption( "SIM" ) ){
        CompMgr().InitRoot( "CheckAcp.json" );
        CheckAcp();
    }
    else if( CompMgr().CheckOption( "TLV" ) ){
        StoreTLV();
    }
    //else if( CompMgr().CheckOption( "Acp" ) ){
        //ReweighAcp();
    //}
    else{
        MakeHist();
    }
}
