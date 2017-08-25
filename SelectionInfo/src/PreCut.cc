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
    string src   = SelMgr().GetOption<string>("source");
    bool is_data =  strncmp(src.c_str(), "run", 3 ) ? 0 : 1 ;

    vector<string> sourcelst = GetList<string>("path", SelMgr().GetSubTree(src));
    vector<int>    hlt       = GetList<int>   ("HLT" , SelMgr().GetSubTree(src));       

    //Adding files
    TChain* ch = new TChain("bprimeKit/root");
    for(const auto& i : sourcelst)
        ch->Add( i.c_str() );
    SelMgr().SetRoot(ch);

    mgr::CheckPath( GetResultsName("root", "precut") );
    TFile* newfile = new TFile( GetResultsName("root", "precut").c_str(), "recreate");
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
       
        if (!preMuon())
            continue;

        newtree->Fill();
    }
    cout<<endl;
    newtree->AutoSave();
    delete ch;
    delete newfile;
}

