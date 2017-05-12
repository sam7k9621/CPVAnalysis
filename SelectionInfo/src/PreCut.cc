#include "Selection/SelectionInfo/interface/SelectionInfo.h"
#include "TFile.h"

#include <iostream>

using namespace std;
using namespace sel;

extern bool passMuLoose(){
    
    return(
            smgr.passMuPt(10) *
            smgr.passMuEta(2.5) *
            smgr.passMuRelIsoR04(0.2) *
            (smgr.isGlobalMuon() || smgr.isTrackerMuon()) 
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
    

    for(int i=0;i<ch->GetEntries();i++){
        ch->GetEntry(i);

        process(ch->GetEntries(),i);


        //jet preselection
        if (!preJet())
            continue;
    

        //lepton preselection
        bool hasLooseMu = false;
        for(int j=0;j<smgr.lsize();j++){
            smgr.SetIndex(j);

            if ( smgr.lep_type() == 13 ){
                if( passMuLoose() )
                    hasLooseMu = true;
            }
        }

        if(!hasLooseMu)
            continue;

        newtree->Fill();
    }
    cout<<endl;
    newtree->AutoSave();
    delete ch;
    delete newfile;
    
}

