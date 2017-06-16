#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"

#include "TLorentzVector.h"
#include <TMath.h>
#include <iostream>

using namespace std;
using namespace sel;

/*******************************************************************************
*  Class initialization 
*******************************************************************************/

SelectionMgr::SelectionMgr(){
}

SelectionMgr::~SelectionMgr(){

}

void SelectionMgr::SetIndex(int i){
    idx = i;
}

void SelectionMgr::SetRoot(TChain* ch){
    vtx.Register(ch);
    lep.Register(ch);
    evt.Register(ch);
    jet.Register(ch);
}



/*******************************************************************************
*   Common calculation
*******************************************************************************/

bool SelectionMgr::passHLT(vector<int> hlt){
    for(auto h : hlt){
        if(int(evt.TrgBook[h]) == 1)
            return true;
    }

    return false;
}


double SelectionMgr::RelIsoR04(){

    double rel = (lep.ChargedHadronIsoR04[idx] + max(float (0.), lep.NeutralHadronIsoR04[idx] + lep.PhotonIsoR04[idx] - float (0.5) * lep.sumPUPtR04[idx])) / lep.Pt[idx];
    return rel;
}

int SelectionMgr::vsize(){
    return vtx.Size;
}

int SelectionMgr::lsize(){
    return lep.Size;
}

int SelectionMgr::jsize(){
    return jet.Size;
}

int SelectionMgr::lep_type(){
    return lep.LeptonType[idx];
}

int SelectionMgr::runNO(){
    return evt.RunNo;
}

int SelectionMgr::lumiNO(){
    return evt.LumiNo;
}

TLorentzVector SelectionMgr::getLorentzVector(string type){

    TLorentzVector tl;
    if(type == "lep"){
        tl.SetPxPyPzE(
                lep.Px[idx],
                lep.Py[idx],
                lep.Pz[idx],
                lep.Energy[idx]
                );
    }


    else if(type == "jet"){
        tl.SetPxPyPzE(
                jet.Px[idx],
                jet.Py[idx],
                jet.Pz[idx],
                jet.Energy[idx]
            );
    }

    return tl;
}

TLorentzVector SelectionMgr::getMET(const TLorentzVector lep){
   

    double px = lep.Px()+evt.PFMETx;
    double py = lep.Py()+evt.PFMETy;
    double e  = lep.E() +evt.PFMET;


    double pz2 = e*e -(82.9*82.9) - px*px - py*py;
   
    double pz;
    
    if(pz2 < 0){
        pz = 1000000;
    }
    else{
        pz  = -1*(sqrt(pz2)-lep.Pz());
    }

    TLorentzVector tl;
    tl.SetPxPyPzE(
            evt.PFMETx,
            evt.PFMETy,
            pz,
            evt.PFMET
            );

    return tl;
}
























