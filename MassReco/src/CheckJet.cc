#include "CPVAnalysis/MassReco/interface/MassReco.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1.h"
#include <algorithm>


using namespace std;
using namespace sel;
using namespace dra;

extern bool hasCommonT_jet(const int& bjetidx, const int& jetidx){
   
   return SelMgr().isCommonMo( bjetidx, SelMgr().getDirectMother(jetidx), 6 );
}

extern int MCTruthCut(){

    bool is_data = SelMgr().GetOption<string>("source") == "data" ? 1 : 0 ;
    string source = is_data? "data" : "mc";
    vector<string> sourcelst = GetList<string>("path", SelMgr().GetSubTree(source));
    TChain* ch = new TChain("root");
    for(auto& i : sourcelst){
        ch->Add(i.c_str());
    }
    SelMgr().SetRoot(ch);

//    TH1F* tmass = new TH1F("tmass","tmass",50,0,500);

    int count = 0;
    int muon_count = 0;

    for(int i=0;i<ch->GetEntries();i++){
    //for(int i=0;i<10;i++){
        ch->GetEntry(i);
        process(ch->GetEntries(),i);

       
        vector<int> muidx;
        vector<int> jetidx;
        vector<int> bjetidx;
       
        if( !passMCMuon(muidx) ) 
            continue;
    
        muon_count++;

        if( !passMCJet(jetidx,bjetidx) )
            continue;

        bool passbmu = false;
        bool passb2j = false;

        for(const auto& bidx : bjetidx){
            for(const auto& midx : muidx){
                
                if( hasCommonT_lep(bidx, midx) ){
                    passbmu = true;
                    break;
                }
            }
        }

        if(!passbmu)
            continue;

        int count_jidx = 0;
        for(const auto& bidx : bjetidx){
            for(const auto& jidx : jetidx){

                if(hasCommonT_jet(bidx,jidx)){
                    count_jidx++;
                }
            }

            if(count_jidx >= 2){
                passb2j = true;
                break;
            }
        }

        if(!passb2j)
            continue;

        count++;

    }
  
    cout<<endl<<muon_count<<endl;
    cout<<count<<endl;

    return count;
}

extern void CheckJet(){

    TH1F* teff = new TH1F("tmass","tmass",50,0,0.15);

      
    for(int i=1;i<=50;i++){

        cout<<endl<<i<< " th"<<endl;
        SelMgr().getRvalue();

        if(i==1){
            teff -> SetBinContent(i,0);
            SelMgr().RvalueUP(0.003);
            continue;
        }

        int count = MCTruthCut();
        double eff = (double) count/5000000;
        cout<<"efficiency "<<eff<<endl;
        teff -> SetBinContent(i,eff);
        SelMgr().RvalueUP(0.003);
        
        
    }

    teff->SetStats(false);
    teff->SetTitle("");
    teff->GetXaxis()->SetTitle("delR");
    teff->GetYaxis()->SetTitle("Eff");

    TCanvas* c = mgr::NewCanvas();
    teff->Draw();

    mgr::SetSinglePad(c);
    mgr::SetAxis(teff);
    teff->SetMaximum( 1.1 );
    mgr::DrawCMSLabelOuter(PRELIMINARY);
    
    TLine* line = new TLine(0,1,0.15,1);
    line->SetLineColor(kRed);
    line->SetLineStyle(8);
    line->Draw();
    
    TLegend* leg = mgr::NewLegend(0.65,0.5,0.75,0.7);
    leg->SetLineColor(kWhite);
    leg->AddEntry("teff","efficiency", "l");
    leg->Draw();
    
    c->SaveAs("delR_eff.pdf");
/*
    tmass->SetStats(false);
    tmass->SetTitle("");
    tmass->GetXaxis()->SetTitle("m_{jjb} [GeV]");
    tmass->GetYaxis()->SetTitle("x10^{3} Events");

    TCanvas* c = mgr::NewCanvas();
    tmass->Draw();

    mgr::SetSinglePad(c);
    mgr::SetAxis(tmass);
    tmass->SetMaximum( mgr::GetYmax( tmass ) * 1.6 );
    mgr::DrawCMSLabelOuter(PRELIMINARY);

    if(is_data)
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
    
    c->SaveAs("MCTruth_tmass_passSelection.pdf");
  
    cout<<endl<<mgr::GetYmax(tmass)<<endl;
    cout<<endl<<tmass->Integral()<<endl;

    delete pt;
    delete line;
    delete leg;
    delete c;
    
    chi2->SetStats(false);
    chi2->SetTitle("");
    chi2->GetXaxis()->SetTitle("chi2");
    chi2->GetYaxis()->SetTitle("x10^{3} Events");

    TCanvas* cc = mgr::NewCanvas();
    chi2->Draw();

    mgr::SetSinglePad(cc);
    mgr::SetAxis(chi2);
    chi2->SetMaximum( mgr::GetYmax( chi2 ) * 1.6 );
    mgr::DrawCMSLabelOuter(PRELIMINARY);

    if(is_data)
        mgr::DrawLuminosity(36811);

    TPaveText *ppt = mgr::NewTextBox(350,mgr::GetYmax( chi2 ) * 1.4,480,mgr::GetYmax( chi2 ) * 1.55);
    ppt->AddText("Muon Channel");
    ppt->Draw();

    TLegend* lleg = mgr::NewLegend(0.65,0.5,0.75,0.7);
    lleg->SetLineColor(kWhite);
    lleg->AddEntry("chi2","chi2", "l");
    lleg->Draw();
    
    cc->SaveAs("MCTruth_chi2_passSelection.pdf");
   
    cout<<endl<<mgr::GetYmax(chi2)<<endl;

    delete ppt;
    delete lleg;
    delete cc;
 */   
}
