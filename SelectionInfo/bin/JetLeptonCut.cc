#include "TChain.h"
#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"

int main(){
    TChain* chain = new TChain("root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_b.root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_c.root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_d.root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_e.root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_f.root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_g.root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_h1.root");
    chain->Add("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/muon_precut_h2.root");

    MakeFullCut(chain);
}
