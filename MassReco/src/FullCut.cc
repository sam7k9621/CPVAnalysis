#include "CPVAnalysis/MassReco/interface/MassReco.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TChain.h"
#include "TCanvas.h"
#include "THStack.h"
#include <algorithm>

using namespace std;
using namespace sel;
using namespace dra;

extern bool fillBhandle(){
    
    SelMgr().cleanHandle();
    return SelMgr().checkPartonTopo();
}

extern bool isIsolated(const int& muidx, const int& jetidx){

    return SelMgr().isIsoLepton(muidx,jetidx);
}

extern void setHist(TH1* hist, const string& xtitle, const string& ytitle){

    hist->SetStats(false);
    hist->SetTitle("");
    hist->GetXaxis()->SetTitle(xtitle.c_str());
    hist->GetYaxis()->SetTitle(ytitle.c_str());
}

extern int bbarSeparation(const int& had_b, const int& lep_b, const int& muidx){

    int charge = SelMgr().getMuonCharge(muidx);
    
    // hadronic b charge equals to muon
    int flag1 = SelMgr().bbarDeltaR(had_b,charge);

    // leptonic b charge is opposite to muon
    int flag2 = SelMgr().bbarDeltaR(lep_b,charge*(-1));
  
    // flag info : interface/SelectionInfo.h 

    if( (flag1 | flag2) & None ) 
        return None;
    
    else if( (flag1 | flag2) == (Fakeb | Mistag) )
        return Other;

    else if( (flag1 | flag2) & Fakeb )
        return Fakeb;

    else if( (flag1 | flag2) & Mistag )
        return Mistag;

    else if( (flag1 | flag2) == Correct )
        return Correct;

    else if( (flag1 | flag2) == Swap )
        return Swap;

    else 
        return Other;  

}

extern void MakeFullCut(){

    //Initializing
    bool is_data = SelMgr().GetOption<string>("source") == "data" ? 1 : 0 ;
    string source = is_data? "data" : "mc";

    vector<string> sourcelst = GetList<string>("path", SelMgr().GetSubTree(source));

    TH1F* tmass = new TH1F("tmass","tmass"      ,50,0,500);
    TH1F* case1 = new TH1F("case1","correct"    ,50,0,500);
    TH1F* case2 = new TH1F("case2","fake b"     ,50,0,500);
    TH1F* case3 = new TH1F("case3","mistag"     ,50,0,500);
    TH1F* case4 = new TH1F("case4","swap"       ,50,0,500);
    TH1F* case5 = new TH1F("case5","other"      ,50,0,500);
    TH1F* case6 = new TH1F("case6","not matched",50,0,500);

    THStack* hs  = new THStack("hs","");

    TH1F* chi2_correct = new TH1F("chi2_correct","correct" ,40,0,200);
    TH1F* chi2_swap    = new TH1F("chi2_swap"   ,"swapped" ,40,0,200);
    TH1F* chi2_fakeb   = new TH1F("chi2_fakeb"  ,"fake b"  ,40,0,200);

    

    //Adding files
    TChain* ch = new TChain( SelMgr().GetSingleData<string>( "tree" ).c_str() );
    for(auto& i : sourcelst){
        ch->Add(i.c_str());
    }
    SelMgr().SetRoot(ch);

    //Looping events
    int events = SelMgr().CheckOption("test") ? 10000 : ch->GetEntries();
    for(int i=0;i<events;i++){
        ch->GetEntry(i);
        process(events,i);

        if(!fillBhandle())
            continue;

        vector<int>    muidx;      //store one isolated tight muon
        vector<int>    jetidx;     //store every jets
        vector<int>    bjetidx;    //store two bjets

        if( !passFullMuon(muidx) )
            continue;

        if( !passFullJet(jetidx,bjetidx,muidx[0]) )
            continue;

        //Get TLorentzVector from index
        TLorentzVector         muonhandle = SelMgr().getLorentzLep(muidx[0]);
        vector<TLorentzVector> jethandle  = SelMgr().getLorentzJet(jetidx);
        vector<TLorentzVector> bjethandle = SelMgr().getLorentzJet(bjetidx);
        
        //Mass constrain method   (bbar separtion)
        double chi2mass = 999;
        double seltmass = 0;
        int had_b = -1;

        for(unsigned int i=0;i<jethandle.size();i++){
            for(unsigned int j=(i+1);j<jethandle.size();j++){
                for(unsigned int k=0;k<bjethandle.size();k++){
    
                    double t_mass = (jethandle[i]+jethandle[j]+bjethandle[k]).M();
                    double w_mass = (jethandle[i]+jethandle[j]).M();
                    double chi_t  = (t_mass-172.5)/16.3;
                    double chi_w  = (w_mass-82.9 )/9.5;

                    if( (chi_t*chi_t + chi_w*chi_w) < chi2mass && (chi_t*chi_t + chi_w*chi_w) < 40 ){

                        chi2mass = (chi_t*chi_t + chi_w*chi_w);
                        seltmass = t_mass;
                        had_b     = k;
                    }
                }
            }
        }

        if(chi2mass == 999)
            continue;

        tmass->Fill(seltmass);
       
        int lep_b = had_b ? 0 : 1;
        int flag = bbarSeparation(bjetidx[had_b],bjetidx[lep_b],muidx[0]);       

        if(flag == Correct){
            case1->Fill(seltmass);
            chi2_correct->Fill(chi2mass);
            //csvm_correct->Fill( SelMgr().getCSV() ) ;
        }

        else if(flag == Fakeb){
            case2->Fill(seltmass);
            chi2_fakeb->Fill(chi2mass);
            //csvm_fakeb->Fill( SelMgr().getCSV() );
        }
        else if(flag == Mistag)
            case3->Fill(seltmass);

        else if(flag == Swap){
            case4->Fill(seltmass);
            chi2_swap->Fill(chi2mass);
            //csvm_swap->Fill( SelMgr().getCSV() );
        }

        else if(flag == Other)
            case5->Fill(seltmass);
        
        else if(flag == None)
            case6->Fill(seltmass);
    }

    case1->SetLineColor(kGreen-6);
    case2->SetLineColor(kAzure-3);
    case3->SetLineColor(kOrange+1);
    case4->SetLineColor(kRed-7);
    case5->SetLineColor(kMagenta+2);
    case6->SetLineColor(kGray+1);
    
    case1->SetFillColor(kGreen-6);
    case2->SetFillColor(kAzure-3);
    case3->SetFillColor(kOrange+1);
    case4->SetFillColor(kRed-7);
    case5->SetFillColor(kMagenta+2);
    case6->SetFillColor(kGray+1);
    
    hs->Add(case6);
    hs->Add(case5);
    hs->Add(case4);
    hs->Add(case3);
    hs->Add(case2);
    hs->Add(case1);

    setHist(tmass, "m_{jjb} [GeV]", "Events x10^{3}");

    TCanvas* c = mgr::NewCanvas();
    tmass->Draw();
    hs->Draw("same");

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

    TLegend* leg = mgr::NewLegend(0.65,0.4,0.75,0.7);
    leg->SetLineColor(kWhite);
    leg->AddEntry("case1","Correct","f");
    leg->AddEntry("case2","Fake b","f");
    leg->AddEntry("case3","Mistag","f");
    leg->AddEntry("case4","Charge swapped","f");
    leg->AddEntry("case5","Other","f");
    leg->AddEntry("case6","Gen level not matched","f");
    leg->Draw();
    
    c->SaveAs("tmass_chi40.pdf");
   
    cout<<endl<<endl;

    cout<<"tmass events peak  "<<mgr::GetYmax(tmass)<<endl;
    cout<<"tmass total events "<<tmass->Integral()<<endl;
    cout<<"Correct   "<<case1->Integral()<<" "<<(double)case1->Integral()/tmass->Integral()<<endl;
    cout<<"Fake b    "<<case2->Integral()<<" "<<(double)case2->Integral()/tmass->Integral()<<endl;
    cout<<"Mistag    "<<case3->Integral()<<" "<<(double)case3->Integral()/tmass->Integral()<<endl;
    cout<<"Swapped   "<<case4->Integral()<<" "<<(double)case4->Integral()/tmass->Integral()<<endl;
    cout<<"Other     "<<case5->Integral()<<" "<<(double)case5->Integral()/tmass->Integral()<<endl;
    cout<<"Unmatched "<<case6->Integral()<<" "<<(double)case6->Integral()/tmass->Integral()<<endl;

    delete pt;
    delete line;
    delete leg;
    delete c;
    delete tmass;
    delete case1;
    delete case2;
    delete case3;
    delete case4;
    delete case5;
    delete case6;
    delete hs;

    
    chi2_correct->SetLineColor(kGreen-6);
    chi2_fakeb  ->SetLineColor(kAzure-3);
    chi2_swap   ->SetLineColor(kRed-7);

    chi2_correct->SetLineWidth(2);
    chi2_fakeb  ->SetLineWidth(2);
    chi2_swap   ->SetLineWidth(2);
    
    setHist(chi2_correct, "#chi^{2}_{min}", "Event");

    c = mgr::NewCanvas();
    c->SetLogy( kTRUE );
    chi2_correct->Draw();
    chi2_fakeb  ->Draw("same");
    chi2_swap   ->Draw("same");
    
    mgr::SetSinglePad(c);
    mgr::SetAxis(chi2_correct);
    chi2_correct->SetMaximum( mgr::GetYmax( chi2_correct ) * 1.6 );
    chi2_correct->SetMinimum( 0.1 );
    mgr::DrawCMSLabelOuter(PRELIMINARY);

    leg = mgr::NewLegend(0.65,0.65,0.75,0.8);
    leg->SetLineColor(kWhite);
    leg->AddEntry("chi2_correct","Correct","l");
    leg->AddEntry("chi2_fakeb"  ,"Fake b","l");
    leg->AddEntry("chi2_swap"   ,"Charge swapped","l");
    leg->Draw();
    
    c->SaveAs("chi2_dist_chi40.pdf");

    delete c;
    delete leg;
    delete chi2_correct;
    delete chi2_fakeb;
    delete chi2_swap;
    
}
