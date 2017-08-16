#include "TChain.h"
#include "CPVAnalysis/MassReco/interface/MassReco.h"

int main(int argc, char* argv[]){
    
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "source,s", opt::value<string>()->required(), "Is data or MC" )
        ( "count,c" , "count events" )
        ( "test,t"  , "run testing events number")
        ( "chi2,u"   , opt::value<double>(),"chi2 upper cut")
        ( "deltaR,r", opt::value<double>(),"deltaR matching upper cut")
    ;

    SelMgr("MassReco").AddOptions( de );
    const int run = SelMgr().ParseOptions( argc, argv );
    if( run == dra::Parsermgr::HELP_PARSER  ){ return 0; }
    if( run == dra::Parsermgr::FAIL_PARSER ){ return 1; }

    SelMgr().SetFileName( {"lepton","source"} );
    SelMgr().AddCutName( {"chi2","deltaR"} );



    MakeFullCut();
    //CheckJet();
    //EffTable();
    //checkParticle();
}
