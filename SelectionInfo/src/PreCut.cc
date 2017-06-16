#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
#include "CPVAnalysis/SelectionInfo/interface/checkEvtTool.h"
#include "TFile.h"

#include <iostream>

using namespace std;
using namespace sel;


bool passMuTight(){

    return(
            smgr.passMuPt(26) *
            smgr.passMuEta(2.1) *
            smgr.passMuRelIsoR04(0.12) *
            smgr.MuInnerTrackDxy_PV() *
            smgr.MuInnerTrackDz() *
            smgr.MuNMuonhits() *
            smgr.MuNMatchedStations() *
            smgr.MuGlobalNormalizedChi2() *
            smgr.MuNTrackLayersWMeasurement() *
            smgr.MuNPixelLayers() *
            smgr.isGlobalMuon()
          );
}

extern bool preJet(){
    return (smgr.jsize() >= 4);
}


extern void InitPreCut(TChain* ch, char* name){
    smgr.SetRoot(ch);
    MakePreCut(ch, name);
}

extern void MakePreCut(TChain* ch, char* name){

    TFile* newfile = new TFile(name,"recreate");
    TTree* newtree = ch->CloneTree(0);
    
    checkEvtTool checkEvt;
    checkEvt.addJson("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/SelectionInfo/test/Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON.txt");
    checkEvt.makeJsonMap();


    for(int i=0;i<ch->GetEntries();i++){
        ch->GetEntry(i);

        process(ch->GetEntries(),i);


        bool mcflag = true;
        if(!mcflag){
            //lumimask
            if( !checkEvt.isGoodEvt( smgr.runNO(),smgr.lumiNO() ) ){
                continue;
            }
        }
        //jet preselection at least four jet
        if (!preJet())
            continue;
    

        //lepton preselection at least one tight muon
        bool hasTightMu = false;
        for(int j=0;j<smgr.lsize();j++){
            smgr.SetIndex(j);

            if ( smgr.lep_type() == 13 ){
                if( passMuTight() )
                    hasTightMu = true;
            }
        }

        if(!hasTightMu)
            continue;

        newtree->Fill();
    }
    cout<<endl;
    newtree->AutoSave();
    delete ch;
    delete newfile;
    
}

