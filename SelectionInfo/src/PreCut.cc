#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
#include "CPVAnalysis/SelectionInfo/interface/checkEvtTool.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include "TFile.h"

#include <iostream>

using namespace std;
using namespace sel;
using namespace dra;


extern void MakePreCut(){

    //Initializing
    bool is_data = SelMgr().GetOption<string>("source") == "data" ? 1 : 0 ;
    string source = is_data? "data" : "mc";

    vector<string> sourcelst = GetList<string>("path", SelMgr().GetSubTree(source));
    vector<int>    hlt       = GetList<int>   ("HLT" , SelMgr().GetSubTree(source));       

    //Adding files
    TChain* ch = new TChain("bprimeKit/root");
    for(auto& i : sourcelst){
        ch->Add(i.c_str());
    }
    SelMgr().SetRoot(ch);

    TFile* newfile = new TFile("muon_precut_MC.root","recreate");
    TTree* newtree = ch->CloneTree(0);
    
    //Running over golden_json 
    checkEvtTool checkEvt;
    checkEvt.addJson( SelMgr().GetSingleData<string>("lumimask") );
    checkEvt.makeJsonMap();

    //Looping events
    int events = SelMgr().CheckOption("test") ? 10000 : ch->GetEntries();
    for(int i=0;i<events;i++){
        ch->GetEntry(i);
        process(events,i);

        //Lumimask
        if(is_data){
            if( !checkEvt.isGoodEvt( SelMgr().runNO(),SelMgr().lumiNO() ) ){
                continue;
            }
        }

        //Pass vertex and hlt
        if( !passVertex() || !passHLT(hlt) )
            continue;

        //Jet preselection at least four jet
        if (!preJet())
            continue;

        //Lepton preselection at least one tight muon
        bool hasTightMu = false;
        for(int j=0;j<SelMgr().lsize();j++){
            SelMgr().SetIndex(j);

            if( passMuTight() )
                hasTightMu = true;        
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

