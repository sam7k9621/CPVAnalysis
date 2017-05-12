#include "Selection/SelectionInfo/interface/SelectionInfo.h"
#include "TCanvas.h"
#include "TH1.h"

using namespace std;
using namespace sel;

extern bool passVertex(){

    return(
            !(smgr.IsFake()) * 
            smgr.passNdof() * 
            smgr.passAbsZ() *
            smgr.passRho() 
          );
}

extern bool passElLoose(){
    
    return(
            smgr.passElPt(20) *
            smgr.passElEta(2.5) *
            smgr.passElLoose()
          );
}

extern bool passJet(){

    return(
            smgr.passJetPt(30) *
            smgr.passJetEta(2.4) *
            smgr.JetNConstituents() *
            smgr.JetNEF() *
            smgr.JetNHF() *
            smgr.JetCHF() *
            smgr.JetNCH() *
            smgr.JetCEF() 
          );
}

extern bool passBJet(){

    return(
            smgr.JetCSVM()
          );
}

extern vector<TLorentzVector>* getTightMu(const vector<int>& idx){
    
    vector<TLorentzVector>* v = new vector<TLorentzVector>;
    for(auto i : idx){
        smgr.SetIndex(i);
        v->push_back(smgr.getLorentzVector("lep"));
    }

    return v;
}

extern vector<TLorentzVector>* getJet(const vector<int>& idx){

    vector<TLorentzVector>* v = new vector<TLorentzVector>;
    for(auto i : idx){
        smgr.SetIndex(i);
        v->push_back(smgr.getLorentzVector("jet"));
    }

    return v;
}

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

extern void MakeFullCut(TChain* ch){
  

    smgr.SetRoot(ch);

    TH1F* wmass = new TH1F("wmass","wmass",80,40,120);
    TH1F* tmass = new TH1F("tmass","tmass",500,0,500);

    
    for(int i=0;i<ch->GetEntries();i++){
        ch->GetEntry(i);

        process(ch->GetEntries(),i);

         
        //vertex selection
        bool passV = false;
        for(int j=0;j<smgr.vsize();j++){
            smgr.SetIndex(j);

            if(passVertex()){
                passV=true;
                break;
            }
        }

        if(!passV)
            continue;


        //pass HLT
        vector<int> hlt = {4809};
        if (!smgr.passHLT(hlt))
            continue;
        
        // muon selection
        // store every tight muon
        bool passL = true;
        bool hasLooseEl = false;
        vector<int> muidx;
        for(int j=0;j<smgr.lsize();j++){
            smgr.SetIndex(j);
            
            if(smgr.lep_type() == 13){
                if(passMuLoose() && !passMuTight()){
                     passL=false;
                     break;
                }

                if(passMuTight()){
                    muidx.push_back(j);
                }
        
            }

            else if(smgr.lep_type() == 11){
                if( passElLoose() ){
                    hasLooseEl = true;
                    break;
            
                }
            }
        }
        if(!passL || muidx.size() == 0 || hasLooseEl)
            continue;
        
        // jet selection
        // store every jet (including 2 bjets)
        int countJ  = 0;
        int countJB = 0;
        vector<int> jetidx;
        vector<int> bjetidx;
        for(int j=0;j<smgr.jsize();j++){
            smgr.SetIndex(j);

            if(passJet()){
                countJ++;
                jetidx.push_back(j);
                
                if(passBJet()){
                    countJB++;
                    bjetidx.push_back(j);
                }
            }
        }

        if(countJ < 4 || countJB!=2)
            continue;


        // cleaning against leptons
        vector<TLorentzVector>* muonhandle = getTightMu(muidx);
        vector<TLorentzVector>* jethandle  = getJet(jetidx); 
        int countMu=0;
        for(const auto& _mu : *muonhandle){
            
            bool passDeltaR = true;
            
            for(const auto& _jet : *jethandle ){
                if(_jet.DeltaR(_mu) < 0.5)
                    passDeltaR = false;
                    break;
            } 

            if(passDeltaR)
                countMu++;
        }

        if(countMu != 1)
            continue;



        //still need tbranch to store ttb mass and chi2 calc
//        vector<int> seljetidx = smgr.getSelJet(jetidx,bjetidx);
        vector<int> seljetidx;
        for(const auto& i : jetidx){
            if(i != bjetidx[0] && i != bjetidx[1])
                seljetidx.push_back(i);

            if(seljetidx.size() == 2)
                break;
        }



        vector<TLorentzVector> seljet;
        for(const auto& sj : seljetidx){
            smgr.SetIndex(sj);
            seljet.push_back( smgr.getLorentzVector("jet") );
        }
        
        double _wmass = (seljet[0]+seljet[1]).M();
        wmass->Fill(_wmass);

        smgr.SetIndex(bjetidx[0]);
        TLorentzVector btagjet = smgr.getLorentzVector("jet");
   
        double _tmass = (seljet[0]+seljet[1]+btagjet).M() ;

        tmass->Fill(_tmass);

        delete muonhandle;
        delete jethandle;
    }
        
    TCanvas* c = new TCanvas();
    wmass->Draw();
    c->SaveAs("wmass.pdf");

    tmass->Draw();
    c->SaveAs("tmass.pdf");

    delete c;
    delete wmass;
    delete tmass;
}
