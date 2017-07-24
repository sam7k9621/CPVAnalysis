#include "TChain.h"
#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"

int main(int argc, char* argv[]){
    
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "source,s", opt::value<string>()->required(), "Is data or MC" )
        ( "check,c", opt::value<string>(), "nope" )
    ;

    smgr.AddOptions( de );
    const int run = smgr.ParseOptions( argc, argv );
    if( run == dra::Parsermgr::HELP_PARSER  ){ return 0; }
    if( run == dra::Parsermgr::FAIL_PARSER ){ return 1; }

//    MakeFullCut();
    CheckJet();

    //checkParticle();
}
