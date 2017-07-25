#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1.h"
#include <algorithm>


using namespace std;
using namespace sel;
using namespace dra;

extern bool passMCMuon(vector<int>& mu){
  
    //muon : muon, mother = w, grandmother = t


    int lepidx  = 0;
    int mlepidx = 0;
    int glepidx = 0;


    for(int i=0;i<smgr.lsize();i++){
        smgr.SetIndex(i);

        lepidx = smgr.getGenParticle(i);
       

        if(lepidx  > 0)
            mlepidx = smgr.getDirectMother(lepidx);
        else
            continue ;
        
        if(mlepidx > 0) 
            glepidx = smgr.getDirectMother(mlepidx);
        else
            continue ;



        if (    
                smgr.isXGenParticle(lepidx, 13) &&  
                smgr.isXGenParticle(mlepidx,24) &&
                smgr.isXGenParticle(glepidx,6)
           )
            
            mu.push_back(i);
    
    }

    return mu.size() >= 1 ;
}

extern bool passMCJet(vector<int>& jet, vector<int>& bjet){
    

    //bjet : parton = b, mother = t
    //jet : mother = w, grandmother = t
    
    int jetidx  = 0;
    int mjetidx = 0;
    int gjetidx = 0;

    for(int i=0;i<smgr.jsize();i++){
        smgr.SetIndex(i);

        jetidx = smgr.getGenParton(i);
        
        if(jetidx >= 0)
            mjetidx = smgr.getDirectMother(jetidx);
        else
            continue ;

        if(mjetidx >= 0) 
            gjetidx = smgr.getDirectMother(mjetidx) ;
        else
            continue ;
        
        if (
                smgr.isXGenParticle(jetidx, 5) &&
                smgr.isXGenParticle(mjetidx,6) 
           )
        {    
            bjet.push_back(i);
            continue;
        }

        if(gjetidx < 0)
            continue;

        if (
                smgr.isXGenParticle(mjetidx, 24) &&
                smgr.isXGenParticle(gjetidx, 6)
           )
        {
            jet.push_back(i);
        }



    }

    return ( bjet.size() >= 2 && jet.size() >= 2 ) ;
        

}

extern bool hasCommonT_lep(const int& lep, const int& bjet){
    
   int lepidx  = smgr.getGenParticle(lep);
   int bjetidx = smgr.getGenParton  (bjet);

   if(lepidx < 0 || bjetidx < 0)
       return false;

   return smgr.isCommonMo( smgr.getDirectMother(lepidx), bjetidx, 6 );
}

extern bool hasCommonT_jet(const int& jet, const int& bjet){
    
   int jetidx  = smgr.getGenParton(jet);
   int bjetidx = smgr.getGenParton(bjet);

   if(jetidx < 0 || bjetidx < 0)
       return false;
   
   return smgr.isCommonMo( smgr.getDirectMother(jetidx), bjetidx, 6 );
}

extern int MCTruthCut(){

    bool is_data = smgr.GetOption<string>("source") == "data" ? 1 : 0 ;
    string source = is_data? "data" : "mc";
    vector<string> sourcelst = GetList<string>("path", smgr.GetSubTree(source));
    TChain* ch = new TChain("root");
    for(auto& i : sourcelst){
        ch->Add(i.c_str());
    }
    smgr.SetRoot(ch);

//    TH1F* tmass = new TH1F("tmass","tmass",50,0,500);

    int count = 0;
    int muon_count = 0;

    for(int i=0;i<ch->GetEntries();i++){
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

 /*       if( !smgr.checkPartonTopo() )*/
            /*continue;*/

/*        vector<TLorentzVector>* jethandle  = getJet(jetidx);*/
        //vector<TLorentzVector>* bjethandle = getJet(bjetidx);

        //double t_mass = 0;


        //if( hasCommonT_lep(muidx[0], bjetidx[0]) && hasCommonT(jetidx[0], bjetidx[1]) ){
            
            //t_mass = ((*jethandle)[0]+(*jethandle)[1]+(*bjethandle)[1]).M();
            //tmass->Fill(t_mass);
        //}

        //else if ( hasCommonT_lep(muidx[0], bjetidx[1]) && hasCommonT(jetidx[0], bjetidx[0]) ){
            
            //t_mass = ((*jethandle)[0]+(*jethandle)[1]+(*bjethandle)[0]).M();
            //tmass->Fill(t_mass);
        //}
        count++;

    }
   
    cout<<endl<<muon_count<<endl;

    return count;
   /* int total = tmass->Integral();*/
    //delete ch;
    //delete tmass;
    /*return total;*/
}

extern void CheckJet(){
  

    TH1F* teff = new TH1F("tmass","tmass",200,0,0.2);

      for(int i=1;i<=200;i++){

        cout<<endl<<i<< " th"<<endl;
        int count = MCTruthCut();
        double eff = (double) count/3000000;
        teff -> SetBinContent(i,eff);
        
        smgr.RvalueUP(0.001);
        
    }

    
    teff->SetStats(false);
    teff->SetTitle("");
    teff->GetXaxis()->SetTitle("delR");
    teff->GetYaxis()->SetTitle("Eff");

    TCanvas* c = mgr::NewCanvas();
    teff->Draw();

    mgr::SetSinglePad(c);
    mgr::SetAxis(teff);
    teff->SetMaximum( 0.5 );
    mgr::DrawCMSLabelOuter(PRELIMINARY);
    
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
