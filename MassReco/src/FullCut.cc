#include "CPVAnalysis/MassReco/interface/MassReco.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TChain.h"
#include "TCanvas.h"
#include "THStack.h"
#include <algorithm>

using namespace std;
using namespace sel;
using namespace dra;

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

extern bool passFullHLT(const vector<int>& hlt){

    return smgr.passHLT(hlt);
}

extern bool passFullMuon(vector<int>& muidx){
    
    for(int i=0;i<smgr.lsize();i++){
        smgr.SetIndex(i);
            
        if(smgr.lep_type() == 13){
            
            if(passMuTight()){
                muidx.push_back(i);
                continue;
            }
            
            if(passMuLoose()){
                return false;
            }
        }
         
        else if(smgr.lep_type() == 11){

            if( passElLoose() ){
                return false;
            }
        }
    }

    return muidx.size() > 0;
}

extern bool passFullJet(vector<int>& jetidx, vector<int>& bjetidx, const int& muidx){

        for(int j=0;j<smgr.jsize();j++){
            smgr.SetIndex(j);

            //Cleaning against leptons (isolated lepton)
            if( !isIsolated(muidx,j) )
                continue;

            int mask = 0x01;

            if(passJet()){
                mask <<= 1;
            }
            
            if(passBJet()){
                mask <<= 2;
            }
    
            if(mask & 0x02){
                jetidx.push_back(j);
            }

            if(mask & 0x08){
                bjetidx.push_back(j);
            }
        }

        return ( jetidx.size() >= 2 && bjetidx.size() == 2 );
}

extern bool fillBhandle(){
    smgr.cleanHandle();
    return smgr.checkPartonTopo();
}

extern bool isIsolated(const int& muidx, const int& jetidx){

    return smgr.isIsoLepton(muidx,jetidx);
}

extern void setHist(TH1* hist, const string& xtitle, const string& ytitle){

    hist->SetStats(false);
    hist->SetTitle("");
    hist->GetXaxis()->SetTitle(xtitle.c_str());
    hist->GetYaxis()->SetTitle(ytitle.c_str());
}

extern int bbarSeparation(const int& had_b, const int& lep_b, const int& muidx){

    int charge = smgr.getMuonCharge(muidx);
    
    // hadronic b charge equals to muon
    int flag1 = smgr.bbarDeltaR(had_b,charge);

    // leptonic b charge is opposite to muon
    int flag2 = smgr.bbarDeltaR(lep_b,charge*(-1));
  
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
    bool is_data = smgr.GetOption<string>("source") == "data" ? 1 : 0 ;
    string source = is_data? "data" : "mc";

    vector<string> sourcelst = GetList<string>("path", smgr.GetSubTree(source));
    vector<int>    hlt       = GetList<int>   ("HLT" , smgr.GetSubTree(source));


    TH1F* tmass = new TH1F("tmass","tmass",50,0,500);
    TH1F* case1 = new TH1F("case1","correct",50,0,500);
    TH1F* case2 = new TH1F("case2","fake b",50,0,500);
    TH1F* case3 = new TH1F("case3","mistag",50,0,500);
    TH1F* case4 = new TH1F("case4","swap",50,0,500);
    TH1F* case5 = new TH1F("case5","other",50,0,500);
    TH1F* case6 = new TH1F("case6","not matched",50,0,500);

    TH1F* teff   = new TH1F("teff","teff",8,0,8);
    THStack* hs  = new THStack("hs","");

    //Adding files
    TChain* ch = new TChain("root");
    for(auto& i : sourcelst){
        ch->Add(i.c_str());
    }
    smgr.SetRoot(ch);

    //Looping events
    int events = smgr.CheckOption("test") ? 10000 : ch->GetEntries();
    for(int i=0;i<events;i++){
        ch->GetEntry(i);
        
        if(smgr.CheckOption("count"))
            process(events,i);

        teff->Fill(0.5);
       
        /*****************************/
        
        //if(!preJet())
            //continue;

        //teff->Fill(1.5);

        //bool hasTightMu = false;
        //for(int j=0;j<smgr.lsize();j++){
            //smgr.SetIndex(j);

            //if ( smgr.lep_type() == 13 ){
                //if( passMuTight() )
                    //hasTightMu = true;
            //}
        //}


        //if(!hasTightMu)
            //continue;

        /*teff->Fill(2.5);*/
     
        /*****************************/


        if(!fillBhandle())
            continue;

        teff->Fill(3.5);

        vector<int>    muidx;      //store one isolated tight muon
        vector<int>    jetidx;     //store every jets
        vector<int>    bjetidx;    //store two bjets

        if( !passFullVertex() || !passFullHLT(hlt) )
            continue;
        
        teff->Fill(4.5);

        if( !passFullMuon(muidx) )
            continue;

        teff->Fill(5.5);
        
        if( !passFullJet(jetidx,bjetidx,muidx[0]) )
            continue;

        teff->Fill(6.5);

        //Get TLorentzVector from index
        TLorentzVector         muonhandle = smgr.getLorentzLep(muidx[0]);
        vector<TLorentzVector> jethandle  = smgr.getLorentzJet(jetidx);
        vector<TLorentzVector> bjethandle = smgr.getLorentzJet(bjetidx);
        
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

                    if( (chi_t*chi_t + chi_w*chi_w) < chi2mass ){

                        chi2mass = (chi_t*chi_t + chi_w*chi_w);
                        seltmass = t_mass;
                        had_b     = k;
                    }
                }
            }
        }

        if(chi2mass == 999)
            continue;

        teff->Fill(7.5);

        tmass->Fill(seltmass);
       
        int lep_b = had_b ? 0 : 1;
        int flag = bbarSeparation(bjetidx[had_b],bjetidx[lep_b],muidx[0]);       

        if(flag == Correct)
            case1->Fill(seltmass);
        
        else if(flag == Fakeb)
            case2->Fill(seltmass);

        else if(flag == Mistag)
            case3->Fill(seltmass);
        
        else if(flag == Swap)
            case4->Fill(seltmass);

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
    
    c->SaveAs("tmass.pdf");
   
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
/*************************************/

   setHist(teff, "", "Events x10^{6}"); 

   c = mgr::NewCanvas();
   
   teff->Draw();

   teff->GetXaxis()->SetBinLabel(1,"Total");
   teff->GetXaxis()->SetBinLabel(2,">= 4 jets");
   teff->GetXaxis()->SetBinLabel(3,">= 1 tight muon");
   teff->GetXaxis()->SetBinLabel(4,"mc truth");
   teff->GetXaxis()->SetBinLabel(5,"vertex && HLT");
   teff->GetXaxis()->SetBinLabel(6,"muon cut");
   teff->GetXaxis()->SetBinLabel(7,"jet  cut");
   teff->GetXaxis()->SetBinLabel(8,"chi2 cut");
   
   mgr::SetSinglePad(c);
   mgr::SetAxis(teff);
   teff->SetMaximum( mgr::GetYmax( teff ) * 1.6 );
   mgr::DrawCMSLabelOuter(PRELIMINARY);
   
   leg = mgr::NewLegend(0.65,0.5,0.75,0.7);
   leg->SetLineColor(kWhite);
   leg->AddEntry("teff","Event", "l");
   leg->Draw();
   
   c->SaveAs("EffTable.pdf");

   delete teff;
   delete c;
   delete leg;


/*    if (smgr.CheckOption("check")){*/
        //TCanvas* cc = mgr::NewCanvas();
        //check_tmass->Draw();
        
        //mgr::SetSinglePad(cc);
        //mgr::SetAxis(check_tmass);
        //check_tmass->SetMaximum( mgr::GetYmax( check_tmass ) * 1.6 );
        //mgr::DrawCMSLabelOuter(PRELIMINARY);
        //mgr::DrawLuminosity(36811);

        //TPaveText *ppt = mgr::NewTextBox(350,mgr::GetYmax( check_tmass ) * 1.4,480,mgr::GetYmax( check_tmass ) * 1.55);
        //ppt->AddText("Muon Channel");
        //ppt->Draw();

        //TLine* lline = new TLine(172.5,0,172.5,mgr::GetYmax(check_tmass)*1.6);
        //lline->SetLineColor(kRed);
        //lline->SetLineStyle(8);
        //lline->Draw();

        //TLegend* lleg = mgr::NewLegend(0.65,0.5,0.75,0.7);
        //lleg->SetLineColor(kWhite);
        //lleg->AddEntry("chekc_tmass","t#bar{t}+jets", "l");
        //lleg->Draw();
        
        //cc->SaveAs("check_tmass.pdf");
   
        //cout<<endl<<mgr::GetYmax(check_tmass)<<endl;

        //delete ppt;
        //delete lline;
        //delete lleg;
        /*delete cc;*/
    //}
/*************************************/

/*    TCanvas* c1 = mgr::NewCanvas();*/
    //wmass->Draw();
    //mgr::SetSinglePad(c1);
    //mgr::SetAxis(wmass);
    //wmass->SetMaximum( mgr::GetYmax( wmass  ) * 1.6 );

    //TPaveText *pt1 = mgr::NewTextBox(140,mgr::GetYmax( wmass ) * 1.4,190,mgr::GetYmax( wmass ) * 1.55);
    //pt1->AddText("Muon Channel");
    //pt1->Draw();
    
    //TLine* line1 = new TLine(82.9,0,82.9,mgr::GetYmax(wmass)*1.6);
    //line1->SetLineColor(kRed);
    //line1->SetLineStyle(8);
    //line1->Draw();

    //mgr::DrawCMSLabelOuter(PRELIMINARY);
    
    //if(is_data)
        //mgr::DrawLuminosity(36811);

    //c1->SaveAs("wmass.pdf");

    //delete pt1;
    //delete c1;
    //delete wmass;
    //delete tmass;
    /*delete check_tmass;*/
    
}
