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
    CompMgr().AddHist( "Syst", "", "", 1, 0, 1 );
    if( lepton == "mu" ){
        CompMgr().AddHist( "lep_tmass", "M_{#mub} [GeV]", "Events", 60, 0, 300 );
        CompMgr().AddHist( "leptmass", "M_{#mub} [GeV]", "Events", 50, 0, 500 );
    }
    else if( lepton == "el" ){
        CompMgr().AddHist( "lep_tmass", "M_{eb} [GeV]", "Events", 60, 0, 300 );
        CompMgr().AddHist( "leptmass", "M_{eb} [GeV]", "Events", 50, 0, 500 );
    }
    CompMgr().AddHist( "had_tmass", "M_{jjb} [GeV]", "Events", 50, 50, 300 );
    CompMgr().AddHist( "jj_tmass", "M_{jj} [GeV]", "Events", 60, 0, 300 );
    CompMgr().AddHist( "chi2", "#chi^{2}_{min}", "Events", 40, 0, 200 );
    CompMgr().AddHist( "nVtx", "Number of primary vertex", "Yield", 50, 0, 50 );
    CompMgr().AddHist( "Rho", "Rho", "Events", 100, 0, 100 );

    CompMgr().AddHist( "LJetCSV",  "LJetCSV", "Events", 50, 0, 1 );
    CompMgr().AddHist( "LepIso",   "Lepton Iso", "Events", 50, 0, 0.5 );
    CompMgr().AddHist( "LJetPt",   "Leading Jet P_{T}", "Events",    40, 0, 200 );
    CompMgr().AddHist( "HBJetPt",  "Hadronic b-jet P_{T}", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LBJetPt",  "Leptonic b-jet P_{T}", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LepPt",    "Lepton P_{T}", "Events",         40, 0, 200 );
    CompMgr().AddHist( "LJetEta",  "Leading Jet Eta", "Events",    40, -3, 5 );
    CompMgr().AddHist( "HBJetEta", "Hadronic b-jet Eta", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LBJetEta", "Leptonic b-jet Eta", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LepEta",   "Lepton Eta", "Events",         40, -3, 5 );

    CompMgr().AddHist( "Njets", "N_{jets}", "Events", 10, 0, 10 );
    CompMgr().AddHist( "DeepCSV", "DeepCSV", "Events", 110, -2, 0.2 );
    CompMgr().AddHist( "bweight", "bweight", "Events", 50, 0.2, 1.2 );
    CompMgr().AddHist( "JetFlavor", "JetFlavor", "Events", 30, 0, 30 );
    CompMgr().AddHist( "isWHF", "isWHF", "Events", 2, 0, 2 );

    CompMgr().AddHist( "LJetPt0", "Leading Jet Pt0", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LJetPt1", "Leading Jet Pt1", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LJetPt2", "Leading Jet Pt2", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LJetPt3", "Leading Jet Pt3", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LJetPt4", "Leading Jet Pt4", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LJetPt5", "Leading Jet Pt5", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LJetPt6", "Leading Jet Pt6", "Events", 40, 0, 200 );
    CompMgr().AddHist( "LJetEta0", "Leading Jet Eta0", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LJetEta1", "Leading Jet Eta1", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LJetEta2", "Leading Jet Eta2", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LJetEta3", "Leading Jet Eta3", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LJetEta4", "Leading Jet Eta4", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LJetEta5", "Leading Jet Eta5", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LJetEta6", "Leading Jet Eta6", "Events", 40, -3, 5 );

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    CompMgr().AddHist2D( "chi2_tmass",   "M_{jjb} [GeV]", "#chi^{2}_{min}", 50, 50, 300, 40, 0, 200 );
    CompMgr().AddHist2D( "chi2_jjmass",  "M_{jj} [GeV]", "#chi^{2}_{min}", 60, 0, 300, 40, 0, 200 );
    CompMgr().AddHist2D( "Obs3_leptmass",  "M_{lb} [GeV]", "O3",  50, 0, 500, 50, -1, 1 );
    CompMgr().AddHist2D( "Obs6_leptmass",  "M_{lb} [GeV]", "O6",  50, 0, 500, 50, -1, 1 );
    CompMgr().AddHist2D( "Obs12_leptmass", "M_{lb} [GeV]", "O12", 50, 0, 500, 50, -1, 1 );
    CompMgr().AddHist2D( "Obs14_leptmass", "M_{lb} [GeV]", "O14", 50, 0, 500, 50, -1, 1 );
    
    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    CompMgr().AddHist( "Obs3_dist",  "O_{3}",  "Events", 50, -1, 1 );
    CompMgr().AddHist( "Obs6_dist",  "O_{6}",  "Events", 50, -1, 1 );
    CompMgr().AddHist( "Obs12_dist", "O_{12}", "Events", 50, -1, 1 );
    CompMgr().AddHist( "Obs14_dist", "O_{14}", "Events", 50, -1, 1 );
    
    CompMgr().AddHist( "Obs3", "O_{3}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "Obs6", "O_{6}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "Obs12", "O_{12}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "Obs14", "O_{14}", "Events", 2, -1, 1 );

    CompMgr().AddHist( "weighted_Obs3", "O_{3}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "weighted_Obs6", "O_{6}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "weighted_Obs12", "O_{12}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "weighted_Obs14", "O_{14}", "Events", 2, -1, 1 );

    CompMgr().AddHist( "Ratio_Obs3", "O_{3}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "Ratio_Obs6", "O_{6}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "Ratio_Obs12", "O_{12}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "Ratio_Obs14", "O_{14}", "Events", 2, -1, 1 );
    
    /*******************************************************************************
    *  Intrinsic Acp
    *******************************************************************************/
    CompMgr().AddHist( "GenObs3", "O_{3}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "GenObs6", "O_{6}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "GenObs12", "O_{12}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "GenObs14", "O_{14}", "Events", 2, -1, 1 );
    
    CompMgr().AddHist( "GenObs3_dist",  "O_{3}",  "Events", 50, -1, 1 );
    CompMgr().AddHist( "GenObs6_dist",  "O_{6}",  "Events", 50, -1, 1 );
    CompMgr().AddHist( "GenObs12_dist", "O_{12}", "Events", 50, -1, 1 );
    CompMgr().AddHist( "GenObs14_dist", "O_{14}", "Events", 50, -1, 1 );

    CompMgr().AddHist( "weighted_GenObs3", "O_{3}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "weighted_GenObs6", "O_{6}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "weighted_GenObs12", "O_{12}", "Events", 2, -1, 1 );
    CompMgr().AddHist( "weighted_GenObs14", "O_{14}", "Events", 2, -1, 1 );

    /*******************************************************************************
    * Lepton source
    *******************************************************************************/
    CompMgr().AddHist( "lepton_src", "PDG ID", "Events",   50, 0, 50 );
    
    /*******************************************************************************
    * bbSeparation
    *******************************************************************************/
    CompMgr().AddHist( "tmass_Correct", "M_{jjb} [GeV]", "Events",   50, 50, 300 );
    CompMgr().AddHist( "tmass_Misid", "M_{jjb} [GeV]", "Events",     50, 50, 300 );
    CompMgr().AddHist( "tmass_Mistag", "M_{jjb} [GeV]", "Events",    50, 50, 300 );
    CompMgr().AddHist( "tmass_Nomatched", "M_{jjb} [GeV]", "Events", 50, 50, 300 );
    CompMgr().AddHist( "chi2_Correct", "#chi^{2}_{min}", "PDF",      200, 0, 200 );
    CompMgr().AddHist( "chi2_Misid", "#chi^{2}_{min}", "PDF",        200, 0, 200 );
    CompMgr().AddHist( "chi2_Mistag", "#chi^{2}_{min}", "PDF",       200, 0, 200 );
    CompMgr().AddHist( "chi2_Nomatched", "#chi^{2}_{min}", "PDF",    200, 0, 200 );

    /*******************************************************************************
    * Optimisation
    *******************************************************************************/
    if( lepton == "mu" ){
        CompMgr().AddHist( "Cor_leptmass", "M_{#mub} [GeV]", "Events",    60, 0, 300 );
        CompMgr().AddHist( "Misid_leptmass", "M_{#mub} [GeV]", "Events",  60, 0, 300 );
        CompMgr().AddHist( "Mistag_leptmass", "M_{#mub} [GeV]", "Events", 60, 0, 300 );
    }
    else if( lepton == "el" ){
        CompMgr().AddHist( "Cor_leptmass", "M_{eb} [GeV]", "Events",    60, 0, 300 );
        CompMgr().AddHist( "Misid_leptmass", "M_{eb} [GeV]", "Events",  60, 0, 300 );
        CompMgr().AddHist( "Mistag_leptmass", "M_{eb} [GeV]", "Events", 60, 0, 300 );
    }
    else{
        CompMgr().AddHist( "Cor_leptmass", "M_{lb} [GeV]", "Events",    60, 0, 300 );
        CompMgr().AddHist( "Misid_leptmass", "M_{lb} [GeV]", "Events",  60, 0, 300 );
        CompMgr().AddHist( "Mistag_leptmass", "M_{lb} [GeV]", "Events", 60, 0, 300 );
    }
}

extern void
StoreCompare()
{
    string filename = CompMgr().GetResultsName( "root", "Hist" );
    cout<< ">>Storing "<< filename <<endl;
    /*******************************************************************************
    *  Reco TH1
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "Syst" ), filename, "Syst" );
    mgr::SaveToROOT( CompMgr().Hist( "lep_tmass" ), filename, "lep_tmass" );
    mgr::SaveToROOT( CompMgr().Hist( "leptmass" ), filename, "leptmass" );
    mgr::SaveToROOT( CompMgr().Hist( "had_tmass" ), filename, "had_tmass" );
    mgr::SaveToROOT( CompMgr().Hist( "jj_tmass" ),  filename, "jj_tmass" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2" ),      filename, "chi2" );
    mgr::SaveToROOT( CompMgr().Hist( "nVtx" ),      filename, "nVtx" );
    mgr::SaveToROOT( CompMgr().Hist( "Rho" ),       filename, "Rho" );

    mgr::SaveToROOT( CompMgr().Hist( "LJetPt" ),    filename, "LJetPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta" ),   filename, "LJetEta" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetCSV" ),   filename, "LJetCSV" );
    mgr::SaveToROOT( CompMgr().Hist( "LBJetPt" ),   filename, "LBJetPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LBJetEta" ),  filename, "LBJetEta" );
    mgr::SaveToROOT( CompMgr().Hist( "HBJetPt" ),   filename, "HBJetPt" );
    mgr::SaveToROOT( CompMgr().Hist( "HBJetEta" ),  filename, "HBJetEta" );
    mgr::SaveToROOT( CompMgr().Hist( "LepPt" ),     filename, "LepPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LepEta" ),    filename, "LepEta" );
    mgr::SaveToROOT( CompMgr().Hist( "LepIso" ),    filename, "LepIso" );

    mgr::SaveToROOT( CompMgr().Hist( "Njets" ),     filename, "Njets" );
    mgr::SaveToROOT( CompMgr().Hist( "DeepCSV" ),   filename, "DeepCSV" );
    mgr::SaveToROOT( CompMgr().Hist( "bweight" ),   filename, "bweight" );
    mgr::SaveToROOT( CompMgr().Hist( "JetFlavor" ), filename, "JetFlavor" );
    mgr::SaveToROOT( CompMgr().Hist( "isWHF"     ), filename, "isWHF" );

    mgr::SaveToROOT( CompMgr().Hist( "LJetPt0" ),   filename, "LJetPt0" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt1" ),   filename, "LJetPt1" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt2" ),   filename, "LJetPt2" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt3" ),   filename, "LJetPt3" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt4" ),   filename, "LJetPt4" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt5" ),   filename, "LJetPt5" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetPt6" ),   filename, "LJetPt6" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta0" ),  filename, "LJetEta0" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta1" ),  filename, "LJetEta1" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta2" ),  filename, "LJetEta2" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta3" ),  filename, "LJetEta3" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta4" ),  filename, "LJetEta4" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta5" ),  filename, "LJetEta5" );
    mgr::SaveToROOT( CompMgr().Hist( "LJetEta6" ),  filename, "LJetEta6" );
    // mgr::SaveToROOT( CompMgr().Hist( "LLepPt1" ),         filename, "LLepPt1" );
    // mgr::SaveToROOT( CompMgr().Hist( "LLepPt2" ),         filename, "LLepPt2" );
    // mgr::SaveToROOT( CompMgr().Hist( "LLepPt3" ),         filename, "LLepPt3" );
    /*mgr::SaveToROOT( CompMgr().Hist( "LLepPt4" ),         filename, "LLepPt4" );*/

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist2D( "chi2_tmass"   ),    filename, "chi2_tmass" );
    mgr::SaveToROOT( CompMgr().Hist2D( "chi2_jjmass"  ),    filename, "chi2_jjmass" );
    mgr::SaveToROOT( CompMgr().Hist2D( "Obs3_leptmass"  ),    filename, "Obs3_leptmass" );
    mgr::SaveToROOT( CompMgr().Hist2D( "Obs6_leptmass"  ),    filename, "Obs6_leptmass" );
    mgr::SaveToROOT( CompMgr().Hist2D( "Obs12_leptmass" ),    filename, "Obs12_leptmass" );
    mgr::SaveToROOT( CompMgr().Hist2D( "Obs14_leptmass" ),    filename, "Obs14_leptmass" );

    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "Obs3_dist" ),         filename, "Obs3_dist" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs6_dist" ),         filename, "Obs6_dist" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs12_dist" ),        filename, "Obs12_dist" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs14_dist" ),        filename, "Obs14_dist" );
    
    mgr::SaveToROOT( CompMgr().Hist( "Obs3" ),              filename, "Obs3" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs6" ),              filename, "Obs6" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs12" ),             filename, "Obs12" );
    mgr::SaveToROOT( CompMgr().Hist( "Obs14" ),             filename, "Obs14" );

    mgr::SaveToROOT( CompMgr().Hist( "weighted_Obs3" ),     filename, "weighted_Obs3" );
    mgr::SaveToROOT( CompMgr().Hist( "weighted_Obs6" ),     filename, "weighted_Obs6" );
    mgr::SaveToROOT( CompMgr().Hist( "weighted_Obs12" ),    filename, "weighted_Obs12" );
    mgr::SaveToROOT( CompMgr().Hist( "weighted_Obs14" ),    filename, "weighted_Obs14" );
    
    mgr::SaveToROOT( CompMgr().Hist( "Ratio_Obs3" ),              filename, "Ratio_Obs3" );
    mgr::SaveToROOT( CompMgr().Hist( "Ratio_Obs6" ),              filename, "Ratio_Obs6" );
    mgr::SaveToROOT( CompMgr().Hist( "Ratio_Obs12" ),             filename, "Ratio_Obs12" );
    mgr::SaveToROOT( CompMgr().Hist( "Ratio_Obs14" ),             filename, "Ratio_Obs14" );

    /*******************************************************************************
    *  Intrinsci Acp
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "GenObs3" ),           filename, "GenObs3" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs6" ),           filename, "GenObs6" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs12" ),          filename, "GenObs12" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs14" ),          filename, "GenObs14" );
    
    mgr::SaveToROOT( CompMgr().Hist( "GenObs3_dist" ),      filename, "GenObs3_dist" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs6_dist" ),      filename, "GenObs6_dist" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs12_dist" ),     filename, "GenObs12_dist" );
    mgr::SaveToROOT( CompMgr().Hist( "GenObs14_dist" ),     filename, "GenObs14_dist" );

    mgr::SaveToROOT( CompMgr().Hist( "weighted_GenObs3" ),  filename, "weighted_GenObs3" );
    mgr::SaveToROOT( CompMgr().Hist( "weighted_GenObs6" ),  filename, "weighted_GenObs6" );
    mgr::SaveToROOT( CompMgr().Hist( "weighted_GenObs12" ), filename, "weighted_GenObs12" );
    mgr::SaveToROOT( CompMgr().Hist( "weighted_GenObs14" ), filename, "weighted_GenObs14" );
    
    /*******************************************************************************
    * Lepton source
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "lepton_src" ),     filename, "lepton_src" );

    /*******************************************************************************
    * bbSeparation
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Correct" ),     filename, "tmass_Correct" );
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Misid" ),       filename, "tmass_Misid" );
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Mistag" ),      filename, "tmass_Mistag" );
    mgr::SaveToROOT( CompMgr().Hist( "tmass_Nomatched" ),   filename, "tmass_Nomatched" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Correct" ),      filename, "chi2_Correct" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Misid" ),        filename, "chi2_Misid" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Mistag" ),       filename, "chi2_Mistag" );
    mgr::SaveToROOT( CompMgr().Hist( "chi2_Nomatched" ),    filename, "chi2_Nomatched" );

    /*******************************************************************************
    * Optimisation
    *******************************************************************************/
    mgr::SaveToROOT( CompMgr().Hist( "Cor_leptmass" ),      filename, "Cor_leptmass" );
    mgr::SaveToROOT( CompMgr().Hist( "Misid_leptmass" ),    filename, "Misid_leptmass" );
    mgr::SaveToROOT( CompMgr().Hist( "Mistag_leptmass" ),   filename, "Mistag_leptmass" );
}
