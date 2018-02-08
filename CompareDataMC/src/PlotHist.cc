#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern string
GetName(TH1D* h)
{
    string name  = h->GetName();
    size_t found = name.find("_");
    if (found!=std::string::npos)
        return name.substr(0, found);
    else
        return name;
}

extern void
PlotMass(vector<TH1D*> mclst, TH1D* data)
{
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
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Stack_mass" ) );
   
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
