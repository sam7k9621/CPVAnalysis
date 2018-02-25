#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern void
AddHist()
{
    /*******************************************************************************
    *  Reco TH1
    *******************************************************************************/
    CompMgr().AddHist( "lep_tmass", "#mub [GeV]", "Events", 50, 0, 500 );
    CompMgr().AddHist( "had_tmass", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().AddHist( "chi2", "#chi^{2}_{min}", "Events", 40, 0, 200 );
    CompMgr().AddHist( "nVtx", "Number of primary vertex", "Yield", 50, 0, 50 );

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    CompMgr().AddHist2D( "chi2_tmass", "M_{jjb} [GeV]", "#chi^{2}_{min}", 50, 0, 500, 40, 0, 200 );

    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    CompMgr().AddHist( "Obs3", "O_{3}", "Events", 200, -10, 10 );
    CompMgr().AddHist( "Obs4", "O_{4}", "Events", 200, -10, 10 );

    /*******************************************************************************
    * bbSeparation
    *******************************************************************************/
    CompMgr().AddHist( "tmass_Correct", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().AddHist( "tmass_Misid", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().AddHist( "tmass_Mistag", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().AddHist( "tmass_Nomatched", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().AddHist( "chi2_Correct", "#chi^{2}_{min}", "PDF", 200, 0, 200 );
    CompMgr().AddHist( "chi2_Misid", "#chi^{2}_{min}", "PDF", 200, 0, 200 );
    CompMgr().AddHist( "chi2_Mistag", "#chi^{2}_{min}", "PDF", 200, 0, 200 );
    CompMgr().AddHist( "chi2_Nomatched", "#chi^{2}_{min}", "PDF", 200, 0, 200 );
}

extern void
StoreCompare()
{
    string filename = CompMgr().GetResultsName( "root", "Hist" );

    /*******************************************************************************
    *  Reco TH1
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "lep_tmass" ),       filename, "lep_tmass" );
    mgr::SaveToROOT( CompMgr().Hist( "had_tmass" ),       filename, "had_tmass" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2" ),            filename, "chi2" );
    mgr::SaveToROOT( CompMgr().Hist( "nVtx" ),            filename, "nVtx" );

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist2D( "chi2_tmass" ),    filename, "chi2_tmass" );

    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "Obs3" ),            filename, "Obs3" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs4" ),            filename, "Obs4" );

    /*******************************************************************************
    * bbSeparation
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Correct" ),   filename, "tmass_Correct" );
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Misid" ),     filename, "tmass_Misid" );
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Mistag" ),    filename, "tmass_Mistag" );
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Nomatched" ), filename, "tmass_Nomatched" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Correct" ),    filename, "chi2_Correct" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Misid" ),      filename, "chi2_Misid" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Mistag" ),     filename, "chi2_Mistag" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Nomatched" ),  filename, "chi2_Nomatched" );
}
