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
    string lepton = CompMgr().GetOption<string>( "lepton" );
    if( lepton == "mu" ){
        CompMgr().AddHist( "lep_tmass", "M_{#mub} [GeV]", "Events", 50, 0, 500 );
    }
    else if( lepton == "el" ){
        CompMgr().AddHist( "lep_tmass", "M_{eb} [GeV]", "Events", 50, 0, 500 );
    }
    else{
        CompMgr().AddHist( "lep_tmass", "M_{lb} [GeV]", "Events", 50, 0, 500 );
    }
    CompMgr().AddHist( "had_tmass", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().AddHist( "chi2", "#chi^{2}_{min}", "Events", 40, 0, 200 );
    CompMgr().AddHist( "nVtx", "Number of primary vertex", "Yield", 50, 0, 50 );
    CompMgr().AddHist( "Rho", "Rho", "Events", 100, 0, 100 );

    CompMgr().AddHist( "LJetPt", "Leading Jet P_{T}", "Events", 40, 0, 800 );
    CompMgr().AddHist( "LJetEta", "Leading Jet Eta", "Events", 40, -3, 5 );
    CompMgr().AddHist( "HBJetPt", "Hadronic b-jet P_{T}", "Events", 25, 0, 500 );
    CompMgr().AddHist( "LBJetPt", "Leptonic b-jet P_{T}", "Events", 25, 0, 500 );
    CompMgr().AddHist( "HBJetEta", "Hadronic b-jet Eta", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LBJetEta", "Leptonic b-jet Eta", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LepPt", "Lepton P_{T}", "Events", 25, 0, 500 );
    CompMgr().AddHist( "LepEta", "Lepton Eta", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LepIso", "Lepton Iso", "Events", 25, 0, 0.5 );

    CompMgr().AddHist( "LJetPt1", "Leading Jet Pt1", "Events", 1000, 0, 1000 );
    CompMgr().AddHist( "LJetPt2", "Leading Jet Pt2", "Events", 1000, 0, 1000 );
    CompMgr().AddHist( "LJetPt3", "Leading Jet Pt3", "Events", 1000, 0, 1000 );
    CompMgr().AddHist( "LJetPt4", "Leading Jet Pt4", "Events", 1000, 0, 1000 );
    CompMgr().AddHist( "LLepPt1", "Leading Lep Pt1", "Events", 1000, 0, 1000 );
    CompMgr().AddHist( "LLepPt2", "Leading Lep Pt2", "Events", 1000, 0, 1000 );
    CompMgr().AddHist( "LLepPt3", "Leading Lep Pt3", "Events", 1000, 0, 1000 );
    CompMgr().AddHist( "LLepPt4", "Leading Lep Pt4", "Events", 1000, 0, 1000 );

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    CompMgr().AddHist2D( "chi2_tmass", "M_{jjb} [GeV]", "#chi^{2}_{min}", 50, 0, 500, 40, 0, 200 );

    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    CompMgr().AddHist( "Obs3", "O_{3}", "Events", 200, -10, 10 );
    CompMgr().AddHist( "Obs6", "O_{6}", "Events", 200, -10, 10 );
    CompMgr().AddHist( "Obs12", "O_{12}", "Events", 200, -10, 10 );
    CompMgr().AddHist( "Obs13", "O_{13}", "Events", 200, -10, 10 );

    /*******************************************************************************
    *  Intrinsci Acp
    *******************************************************************************/
    CompMgr().AddHist( "GenObs3", "O_{3}", "Events", 200, -10, 10 );
    CompMgr().AddHist( "GenObs6", "O_{6}", "Events", 200, -10, 10 );
    CompMgr().AddHist( "GenObs12", "O_{12}", "Events", 200, -10, 10 );
    CompMgr().AddHist( "GenObs13", "O_{13}", "Events", 200, -10, 10 );

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

    /*******************************************************************************
    * Optimisation
    *******************************************************************************/
    if( lepton == "mu" ){
        CompMgr().AddHist( "Cor_leptmass", "M_{#mub} [GeV]", "Events", 50, 0, 500 );
        CompMgr().AddHist( "Misid_leptmass", "M_{#mub} [GeV]", "Events", 50, 0, 500 );
        CompMgr().AddHist( "Mistag_leptmass", "M_{#mub} [GeV]", "Events", 50, 0, 500 );
    }
    else if( lepton == "el" ){
        CompMgr().AddHist( "Cor_leptmass", "M_{eb} [GeV]", "Events", 50, 0, 500 );
        CompMgr().AddHist( "Misid_leptmass", "M_{eb} [GeV]", "Events", 50, 0, 500 );
        CompMgr().AddHist( "Mistag_leptmass", "M_{eb} [GeV]", "Events", 50, 0, 500 );
    }
    else{
        CompMgr().AddHist( "Cor_leptmass", "M_{lb} [GeV]", "Events", 50, 0, 500 );
        CompMgr().AddHist( "Misid_leptmass", "M_{lb} [GeV]", "Events", 50, 0, 500 );
        CompMgr().AddHist( "Mistag_leptmass", "M_{lb} [GeV]", "Events", 50, 0, 500 );
    }
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
    mgr::SaveToROOT( CompMgr().Hist( "Rho" ),             filename, "Rho" );

    mgr::SaveToROOT( CompMgr().Hist( "LJetPt" ),          filename, "LJetPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta" ),         filename, "LJetEta" );
    mgr::SaveToROOT( CompMgr().Hist( "LBJetPt" ),         filename, "LBJetPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LBJetEta" ),        filename, "LBJetEta" );
    mgr::SaveToROOT( CompMgr().Hist( "HBJetPt" ),         filename, "HBJetPt" );
    mgr::SaveToROOT( CompMgr().Hist( "HBJetEta" ),        filename, "HBJetEta" );
    mgr::SaveToROOT( CompMgr().Hist( "LepPt" ),           filename, "LepPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LepEta" ),          filename, "LepEta" );
    mgr::SaveToROOT( CompMgr().Hist( "LepIso" ),          filename, "LepIso" );

    mgr::SaveToROOT( CompMgr().Hist( "LJetPt1" ),         filename, "LJetPt1" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt2" ),         filename, "LJetPt2" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt3" ),         filename, "LJetPt3" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt4" ),         filename, "LJetPt4" );
    mgr::SaveToROOT( CompMgr().Hist( "LLepPt1" ),         filename, "LLepPt1" );
    mgr::SaveToROOT( CompMgr().Hist( "LLepPt2" ),         filename, "LLepPt2" );
    mgr::SaveToROOT( CompMgr().Hist( "LLepPt3" ),         filename, "LLepPt3" );
    mgr::SaveToROOT( CompMgr().Hist( "LLepPt4" ),         filename, "LLepPt4" );

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist2D( "chi2_tmass" ),    filename, "chi2_tmass" );

    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "Obs3" ),            filename, "Obs3" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs6" ),            filename, "Obs6" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs12" ),           filename, "Obs12" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs13" ),           filename, "Obs13" );

    /*******************************************************************************
    *  Intrinsci Acp
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "GenObs3" ),         filename, "GenObs3" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs6" ),         filename, "GenObs6" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs12" ),        filename, "GenObs12" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs13" ),        filename, "GenObs13" );

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

    /*******************************************************************************
    * Optimisation
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "Cor_leptmass" ),    filename, "Cor_leptmass" );
    mgr::SaveToROOT( CompMgr().Hist( "Misid_leptmass" ),  filename, "Misid_leptmass" );
    mgr::SaveToROOT( CompMgr().Hist( "Mistag_leptmass" ), filename, "Mistag_leptmass" );
}
