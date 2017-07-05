#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1.h"
#include <algorithm>

using namespace std;
using namespace sel;
using namespace dra;

extern bool passVertex(){

    return(
            !(smgr.IsFake()) * 
            smgr.passNdof() * 
            smgr.passAbsZ() *
            smgr.passRho() 
          );
}

extern bool passMuLoose(){
    
    return(
            smgr.passMuPt(10) *
            smgr.passMuEta(2.5) *
            smgr.passMuRelIsoR04(0.2) *
            (smgr.isGlobalMuon() || smgr.isTrackerMuon()) 
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

extern bool passFullVertex(){

    bool passV = false;
    for(int i=0;i<smgr.vsize();i++){
        smgr.SetIndex(i);
        
        if(passVertex()){
            passV = true;
            break;
        }
    }

    return passV;
        
}

extern bool passFullHLT(){

    string source = smgr.GetOption<string>("source");
    vector<int> hlt = GetList<int>("HLT" ,smgr.GetSubTree(source));
    return smgr.passHLT(hlt);

}

extern bool passFullMuon(vector<int>& muidx){
    
    bool passL = true;
    bool hasLooseEl = false;
    for(int i=0;i<smgr.lsize();i++){
        smgr.SetIndex(i);

            if(smgr.lep_type() == 13){
                if(passMuLoose() && !passMuTight()){
                     passL=false;
                     break;
                }
                if(passMuTight()){
                    muidx.push_back(i);
                }
            }

            else if(smgr.lep_type() == 11){
                if( passElLoose() ){
                    hasLooseEl = true;
                    break;
                }
            }
    }

    return !(!passL || muidx.size() == 0 || hasLooseEl);
    
}

extern bool passFullJet(vector<int>& jetidx, vector<int>& bjetidx){

        int countJ  = 0;
        int countJB = 0;
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

        return !(countJ < 4 || countJB!=2);

}

extern bool hasMotherT(const int& jet1idx, const int& jet2idx, const int& bjetidx){

    int ptonidx1 = smgr.getPartonID(jet1idx);
    int ptonidx2 = smgr.getPartonID(jet2idx);
    int ptonidx3 = smgr.getPartonID(bjetidx);

    if(ptonidx1 < 0 || ptonidx2 < 0 || ptonidx3 < 0){
        return false;
    }


    if( !smgr.isCommonMo( ptonidx1, ptonidx2, 24 ) )
        return false;


    if( !smgr.isCommonMo( smgr.getMoID(ptonidx1), ptonidx3, 6  ) )
        return false;


return true;
}

extern void MakeFullCut(){
 
    bool is_data = smgr.GetOption<string>("source") == "data" ? 1 : 0 ;
    string source = is_data? "data" : "mc";
    vector<string> sourcelst = GetList<string>("path", smgr.GetSubTree(source));

    TChain* ch = new TChain("root");
    for(auto& i : sourcelst){
        ch->Add(i.c_str());
    }

    smgr.SetRoot(ch);

    TH1F* wmass = new TH1F("wmass","wmass",200,0,200);
    TH1F* tmass = new TH1F("tmass","tmass",50,0,500);
    TH1F* check_tmass = new TH1F("check_tmass","check_tmass",50,0,500);
    
    for(int i=0;i<ch->GetEntries();i++){
        ch->GetEntry(i);

        process(ch->GetEntries(),i);
 
        //vertex selection
        if(!passFullVertex())
            continue;

        //pass HLT
        if (!passFullHLT())
            continue;
        
        // muon selection
        // store every tight muon
        vector<int> muidx;
        if (!passFullMuon(muidx))
            continue;
        
        // jet selection
        // store every jet (including 2 bjets)
        vector<int> jetidx;
        vector<int> bjetidx;
        if (!passFullJet(jetidx,bjetidx))
            continue;

        //cleanign bjet in jetidx
        for(const auto& b : bjetidx){
            jetidx.erase(std::remove(jetidx.begin(), jetidx.end(), b), jetidx.end());
        }

        vector<TLorentzVector>* muonhandle = getTightMu(muidx);
        vector<TLorentzVector>* jethandle  = getJet(jetidx);
        vector<TLorentzVector>* bjethandle = getJet(bjetidx);
        
        // cleaning against leptons
        TLorentzVector lephandle;
        int countMu=0;
        bool passDeltaR;
        for(const auto& _mu : *muonhandle){
            
            passDeltaR=true;

            for(const auto& _jet : *jethandle ){
                if(_jet.DeltaR(_mu) < 0.5){
                    passDeltaR = false;
                    break;
                }
            } 
            if(passDeltaR){
                countMu++;
                lephandle=_mu; //check if the sorting is correct
            }
        }

        if(countMu != 1)
            continue;

        // mass constrain method       
        double chi2mass = 50;
        double seltmass = -1;
        double selwmass = -1;
        int    bjetindex = -1;
        int    jet1index = -1;
        int    jet2index = -1;

        
        for(unsigned int i=0;i<jethandle->size();i++){
            for(unsigned int j=(i+1);j<jethandle->size();j++){
                for(unsigned int k=0;k<bjethandle->size();k++){
                    
                    double t_mass = ((*jethandle)[i]+(*jethandle)[j]+(*bjethandle)[k]).M();
                    double w_mass = ((*jethandle)[i]+(*jethandle)[j]).M();
                    double chi_t  = (t_mass-172.5)/16.3;
                    double chi_w  = (w_mass-82.9 )/9.5;

                    if( (chi_t*chi_t + chi_w*chi_w) <= 40 ){
                        
                        if( (chi_t*chi_t + chi_w*chi_w) < chi2mass){
                            chi2mass = (chi_t*chi_t + chi_w*chi_w);
                            seltmass = t_mass;
                            selwmass = w_mass;

                            bjetindex = k;
                            jet1index = i;
                            jet2index = j;
                        }

                    }
                }
            }
        }
        if(chi2mass == 50)
            continue;


        if(!is_data)
            if( !hasMotherT(jetidx[jet1index], jetidx[jet2index], bjetidx[bjetindex]) )
                continue;

        tmass->Fill(seltmass);
        wmass->Fill(selwmass);

        //finding MET component
        for(int i=0;i<2;i++){
        
            if(i==bjetindex)
                continue;

            double ans = (lephandle+(*bjethandle)[i]+smgr.getMET( lephandle )).M();

            if(ans < 0)
                continue;

            check_tmass->Fill(ans);

        }        
        delete muonhandle;
        delete jethandle;
    }


    
    tmass->SetStats(false);
    wmass->SetStats(false);
    check_tmass->SetStats(false);

    tmass->SetTitle("");
    tmass->GetXaxis()->SetTitle("m_{jjb} [GeV]");
    tmass->GetYaxis()->SetTitle("x10^{3} Events");
    
    wmass->SetTitle("");
    wmass->GetXaxis()->SetTitle("m_{W} [GeV]");
    wmass->GetYaxis()->SetTitle("x10^{3} Events");
    
    check_tmass->SetTitle("");
    check_tmass->GetXaxis()->SetTitle("m_{lb} [GeV]");
    check_tmass->GetYaxis()->SetTitle("x10^{3} Events");

    TCanvas* c = mgr::NewCanvas();
    tmass->Draw();
    
    mgr::SetSinglePad(c);
    mgr::SetAxis(tmass);
    tmass->SetMaximum( mgr::GetYmax( tmass ) * 1.6 );
    mgr::DrawCMSLabelOuter(PRELIMINARY);
    mgr::DrawLuminosity(36811);

    TPaveText *pt = mgr::NewTextBox(350,mgr::GetYmax( tmass ) * 1.4,480,mgr::GetYmax( tmass ) * 1.55);
    pt->AddText("Muon Channel");
    pt->Draw();

    TLine* line = new TLine(172.5,0,172.5,mgr::GetYmax(tmass)*1.6);
    line->SetLineColor(kRed);
    line->SetLineStyle(8);
    line->Draw();

    TLegend* leg = mgr::NewLegend(0.65,0.5,0.75,0.7);
    leg->SetLineColor(kWhite);
    leg->AddEntry("tmass","t#bar{t}+jets", "l");
    leg->Draw();
    
    c->SaveAs("tmass.pdf");
   
    cout<<endl<<mgr::GetYmax(tmass)<<endl;

    delete pt;
    delete line;
    delete leg;
    delete c;
    
/*************************************/

    TCanvas* cc = mgr::NewCanvas();
    check_tmass->Draw();
    
    mgr::SetSinglePad(cc);
    mgr::SetAxis(check_tmass);
    check_tmass->SetMaximum( mgr::GetYmax( check_tmass ) * 1.6 );
    mgr::DrawCMSLabelOuter(PRELIMINARY);
    mgr::DrawLuminosity(36811);

    TPaveText *ppt = mgr::NewTextBox(350,mgr::GetYmax( check_tmass ) * 1.4,480,mgr::GetYmax( check_tmass ) * 1.55);
    ppt->AddText("Muon Channel");
    ppt->Draw();

    TLine* lline = new TLine(172.5,0,172.5,mgr::GetYmax(check_tmass)*1.6);
    lline->SetLineColor(kRed);
    lline->SetLineStyle(8);
    lline->Draw();

    TLegend* lleg = mgr::NewLegend(0.65,0.5,0.75,0.7);
    lleg->SetLineColor(kWhite);
    lleg->AddEntry("chekc_tmass","t#bar{t}+jets", "l");
    lleg->Draw();
    
    cc->SaveAs("check_tmass.pdf");
   
    cout<<endl<<mgr::GetYmax(check_tmass)<<endl;

    delete ppt;
    delete lline;
    delete lleg;
    delete cc;

/*************************************/

    TCanvas* c1 = mgr::NewCanvas();
    wmass->Draw();
    mgr::SetSinglePad(c1);
    mgr::SetAxis(wmass);
    wmass->SetMaximum( mgr::GetYmax( wmass  ) * 1.6 );

    TPaveText *pt1 = mgr::NewTextBox(140,mgr::GetYmax( wmass ) * 1.4,190,mgr::GetYmax( wmass ) * 1.55);
    pt1->AddText("Muon Channel");
    pt1->Draw();
    
    TLine* line1 = new TLine(82.9,0,82.9,mgr::GetYmax(wmass)*1.6);
    line1->SetLineColor(kRed);
    line1->SetLineStyle(8);
    line1->Draw();

    mgr::DrawCMSLabelOuter(PRELIMINARY);
    mgr::DrawLuminosity(36811);

    c1->SaveAs("wmass.pdf");

    delete pt1;
    delete c1;
    delete wmass;
    delete tmass;
    delete check_tmass;
    
}
