#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TCanvas.h"
#include "TH1.h"
#include <algorithm>

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


extern void MakeFullCut(TChain* ch){
  

    smgr.SetRoot(ch);

    TH1F* wmass = new TH1F("wmass","wmass",200,0,200);
    TH1F* tmass = new TH1F("tmass","tmass",50,0,500);
    TH1F* check_tmass = new TH1F("check_tmass","check_tmass",50,0,500);

    
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
        //4809
        //2748,2749,4174,4809
        vector<int> hlt = {2748,2749,4174,4809};
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

        // cleaning bjet in jethandle
        for(const auto& b : *bjethandle){
           jethandle->erase(std::remove(jethandle->begin(), jethandle->end(), b), jethandle->end());
       }

        // mass constrain method       
        vector<double> chi2mass;
        vector<double> seltmass;
        vector<double> selwmass;

        vector<int>    bjetindex;
        
        for(unsigned int i=0;i<jethandle->size();i++){
            for(unsigned int j=(i+1);j<jethandle->size();j++){
                for(unsigned int k=0;k<bjethandle->size();k++){
                    
                    double t_mass = ((*jethandle)[i]+(*jethandle)[j]+(*bjethandle)[k]).M();
                    double w_mass = ((*jethandle)[i]+(*jethandle)[j]).M();
                    double chi_t  = (t_mass-172.5)/16.3;
                    double chi_w  = (w_mass-82.9 )/9.5;

                    if( (chi_t*chi_t + chi_w*chi_w) <= 40 ){
                        chi2mass.push_back( (chi_t*chi_t + chi_w*chi_w) );
                        seltmass.push_back( t_mass );
                        selwmass.push_back( w_mass );

                        bjetindex.push_back( k );
                    }
                }
            }
        }
       
        if(chi2mass.empty())
            continue;

        int min_pos = distance(chi2mass.begin(),min_element(chi2mass.begin(),chi2mass.end()));

        tmass->Fill(seltmass[min_pos]);
        wmass->Fill(selwmass[min_pos]);

        for(int i=0;i<2;i++){
           

            if(i==bjetindex[min_pos])
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
