#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Histor&
PlotMgr( const string& subdir, const string& json )
{
    static Histor mgr( subdir, json );
    return mgr;
}

extern void
MakePlotCompare()
{
    string lepton          = PlotMgr().GetOption<string>( "lepton" );
    string entry           = PlotMgr().GetSingleData<string>( lepton + "entry" );
    vector<string> histlst = {
        "lep_tmass",
        "had_tmass",
        "nVtx",
        "chi2",
        "LJetPt",
        "LJetEta",
        "LBJetPt",
        "LBJetEta",
        "HBJetPt",
        "HBJetEta",
        "LepPt",
        "LepEta",
        "Rho"
    };

    /*******************************************************************************
    *  Stack Plot
    *******************************************************************************/
    for( const auto& title : histlst ){
        PlotCompare( ExtractMC( title ), ExtractData( title ), title, entry );
    }

    GetYield( ExtractMC( "lep_tmass" ), ExtractData( "lep_tmass" ) );

    /*******************************************************************************
    *  CR Sample Check
    *******************************************************************************/
    // TestBGLike( ExtractMC( "lep_tmass" ), ExtractData( "lep_tmass" ), ExtractData( "lep_tmass", "CS2" ) );

    /*******************************************************************************
    * b-tagging Efficiency
    *******************************************************************************/
    /*    PlotPDF(*/
    // ExtractMC( "chi2_Correct" ),
    // ExtractMC( "chi2_Misid" ),
    // ExtractMC( "chi2_Mistag" )
    // );

    /*Plot2D( ExtractMC2D( "chi2_tmass" ) );*/
    // PlotOpt( ExtractMC( "Cor_leptmass" ), ExtractMC( "Misid_leptmass" ), ExtractMC( "Mistag_leptmass" ) );

    /*******************************************************************************
    * Observerable Dist.
    *******************************************************************************/
    // vector<string> mclst = { "Obs2", "Obs3", "Obs4", "Obs7" };
    // vector<string> remclst = { "GenObs2", "GenObs3", "GenObs4", "GenObs7" };

    // GetAcp( mclst );
    // GetSubtractAcp( mclst );
}
