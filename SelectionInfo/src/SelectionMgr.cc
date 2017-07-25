#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"

#include "TLorentzVector.h"
#include <TMath.h>
#include <iostream>

using namespace std;
using namespace sel;
using namespace dra;
/*******************************************************************************
*  Class initialization 
*******************************************************************************/

SelectionMgr::SelectionMgr(const string& subdir):
    Pathmgr("CPVAnalysis", subdir),
    Readmgr(SettingsDir() / "Selection.json"),
    Parsermgr()
{
    Rvalue = 0;
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
    gen.Register(ch);
}

string SelectionMgr::GetFileName(const string& prefix, const string& type){
    string ans = ""; 
    for( auto& name : GetNamelist() ){
        ans += ( "_" + OptName(name) );
    }
    if(prefix == ""){
        ans.erase(ans.begin());
    }
    return ResultsDir() / ( prefix+ans+"."+type );
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

int SelectionMgr::gsize(){
    return gen.Size;
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

double SelectionMgr:: Phi_mpi_pi(double x){
    
    Double_t const  kPI        = TMath::Pi();
    Double_t const  kTWOPI     = 2.*kPI;

    while (x >= kTWOPI) x -= kTWOPI;
    while (x <     0.)  x += kTWOPI;

    return x;
}


/*******************************************************************************
*   MC Truth
*******************************************************************************/

void SelectionMgr::RvalueUP(const double& up){
    Rvalue += up;
}

int SelectionMgr::matchGenlevel(const float& eta, const float& phi){

    for(int i=0;i<gsize();i++) {
       
        double deta = eta - gen.Eta[i];
        double dphi = Phi_mpi_pi( (double)(phi - gen.Phi[i]) );
        double deltaR = TMath::Sqrt( deta*deta+dphi*dphi );

        //prevent from shower particle
        if(deltaR < Rvalue){
            return i;
        }
    }
    return -999;
}

int SelectionMgr::getGenParticle(const int& idx){

    int pdgid   = lep.GenPdgID[idx];
    float eta   = lep.GenEta  [idx];
    float phi   = lep.GenPhi  [idx];

    if(pdgid == 0)
        return -999;

    return matchGenlevel(eta,phi);
}


int SelectionMgr::getGenParton(const int& idx){

    int pdgid   = jet.GenPdgID[idx];
    float eta   = jet.GenEta  [idx];
    float phi   = jet.GenPhi  [idx];
   
    if(pdgid == 0)
        return -999;
    
    return matchGenlevel(eta,phi);

}

int SelectionMgr::getDirectMother(int idx){

    if( gen.Mo1[idx] < 0 )
        return -999;

    while( gen.Mo1PdgID[idx] == gen.PdgID[idx] ){
        idx = gen.Mo1[idx];
    }
    
    return gen.Mo1[idx];

}

int SelectionMgr::getDirectMotherPdgID(const int& idx){
    
    return gen.PdgID[ getDirectMother(idx) ];
}

bool SelectionMgr::isXGenParticle(const int& idx, const int& pdgid){

    return ( fabs(gen.PdgID[idx]) == pdgid );
}

int SelectionMgr::getPdgID(const int& idx){
    return gen.PdgID[idx];
}


bool SelectionMgr::isCommonMo(const int& idx1, const int& idx2, const int& pdgid){
   

    if ( getDirectMother(idx1) != getDirectMother(idx2) )
        return false;

    if ( fabs(getDirectMotherPdgID(idx1)) != pdgid )
        return false;

    return true;

}

bool SelectionMgr::checkPartonTopo(){

    int count_b = 0;
    int count_j = 0;

    for(int i=0;i<gsize();i++){
        
        if(
                fabs( gen.PdgID[i] ) == 5 &&
                fabs( getDirectMotherPdgID(i) ) == 6
          )
        {
            count_b++;
        }

        if(
                fabs( getDirectMotherPdgID(i) ) == 24 &&
                fabs( getDirectMotherPdgID( getDirectMother(i) ) ) == 6
          )
        {
            count_j++;
        }
    }

    return (count_b>=2 && count_j>=2);

}


















