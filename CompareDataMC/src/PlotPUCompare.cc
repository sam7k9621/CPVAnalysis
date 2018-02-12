#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;
  
extern void
PlotPUCompare(const string& title)
{

    vector<string> mcsamplelst ={
            "DYJet_HT-100to200",
            "DYJet_HT-200to400",
            "DYJet_HT-400to600",
            "DYJet_HT-600to800",
            "DYJet_HT-800to1200",
            "DYJet_HT-1200to2500",
            "DYJet_HT-2500toInf",
            "SingleTop_s-ch",
            "SingleTop_t-ch",
            "SingleTop_t-ch_anti",
            "SingleTop_tW",
            "SingleTop_tW_anti",
            "TTbar",
            "VV_WW",
            "VV_WZ",
            "VV_ZZ",
            "WJet_Pt-100to250",
            "WJet_Pt-250to400",
            "WJet_Pt-400to600",
            "WJet_Pt-600toInf"   
                                };
    
    vector<TH1D*>  mclst;
    string file = "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/precut_muon_";

    for(const auto s : mcsamplelst){
        TFile* f = TFile::Open( (file + s + ".root").c_str() );
        TH1D* h  = (TH1D*)( f->Get( title.c_str()) )->Clone();
        h->SetDirectory(0);

        double lumi    = CompMgr().GetSingleData<double>( "lumi" );
        double xs      = mgr::GetSingle<double>( "cross_section", CompMgr().GetSubTree( s ) );
        double gen_num = mgr::GetSingle<double>( "gen_num", CompMgr().GetSubTree( s ) );
        h->Scale( (lumi*xs) / gen_num );    
        mclst.push_back(h);
        f->Close();
    }

    TFile* f = TFile::Open( (file + "data.root").c_str() );
    TH1D*  data  = (TH1D*)( f->Get( title.c_str()) )->Clone();
    data->SetDirectory(0);
    f->Close();

    TCanvas* c   = mgr::NewCanvas();
    Color_t x[]  = { kGray + 1, kMagenta + 2, kRed - 7, kOrange + 1, kAzure - 3, kGreen - 6 };
    THStack* bg  = new THStack();
    TLegend* leg = mgr::NewLegend( 0.65, 0.45, 0.75, 0.75 );
    leg->SetLineColor( kWhite );

    for( int i = 0; i<(int)mclst.size(); i++ ){
        mclst[ i ]->SetLineColor( x[ i ] );
        mclst[ i ]->SetFillColor( x[ i ] );
        bg->Add( mclst[ i ] );
        leg->AddEntry( mclst[ i ], GetName( mclst[ i ] ).c_str(), "F" );
    }

    TH1D* bg_sum = (TH1D*)mclst[0]->Clone("bg_sum");
    for(int i=1; i<(int)mclst.size(); i++){
        bg_sum->Add(mclst[i]);
    }

    TPad* top = mgr::NewTopPad();
    top->Draw();
    top->cd();

    data->Draw("EP");
    bg->Draw("same histo");
    data->Draw("EP same");
    leg->Draw();
    
    mgr::SetTopPlotAxis( data );
    data->SetMaximum( mgr::GetYmax( data ) * 1.2 );
    data->SetStats( false );
    data->SetLineColor(1);
    data->SetLineWidth(1);
    data->SetMarkerSize(0.5);
    data->SetMarkerStyle(20);
    data->GetYaxis()->SetTitle("Events");
    leg->AddEntry(data, "Data", "le");

    c->cd();

    TPad* bot = mgr::NewBottomPad();
    bot->Draw();
    bot->cd();

    TLine* line     = new TLine( 0, 1, 500, 1 );
    TH1D* rel = mgr::DivideHist( data, bg_sum, 1 );

    rel->Draw("EP");
    line->Draw("same");

    line->SetLineColor(kRed);
    rel->SetMaximum( 1.6 );
    rel->SetMinimum( 0.4 );
    rel->GetYaxis()->SetTitle("Data/MC");
    rel->GetXaxis()->SetTitle("M_{jjb} [GeV]");
    mgr::SetBottomPlotAxis( rel );

    c->cd();

    mgr::DrawCMSLabel( PRELIMINARY );
    mgr::DrawLuminosity(36773);
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "Stack_" + title ) );
   
    delete leg;
    delete top;
    delete bot;
    delete line;
    delete bg_sum;
    for(auto h : mclst){
        delete h;
    }
    delete bg;
    delete data;
    
    delete c;
}
