#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern void
DrawbbSeparate()
{
    Color_t x[] = { kGray + 1, kMagenta + 2, kRed - 7, kOrange + 1, kAzure - 3, kGreen - 6 };
    vector<TH1*> histlst;

    histlst.push_back( CompMgr().GetSample()->Hist( "mismatched" ) );
    histlst.push_back( CompMgr().GetSample()->Hist( "other" ) );
    histlst.push_back( CompMgr().GetSample()->Hist( "mistag" ) );
    histlst.push_back( CompMgr().GetSample()->Hist( "fakeb" ) );
    histlst.push_back( CompMgr().GetSample()->Hist( "swap" ) );
    histlst.push_back( CompMgr().GetSample()->Hist( "correct" ) );

    for( int i = 0; i < (int)histlst.size(); i++ ){
        histlst[ i ]->SetLineColor( x[ i ] );
        histlst[ i ]->SetFillColor( x[ i ] );
    }

    TCanvas* c  = mgr::NewCanvas();
    THStack* hs = new THStack();

    TLegend* leg = mgr::NewLegend( 0.65, 0.55, 0.75, 0.75 );
    leg->SetLineColor( kWhite );

    for( auto& h : histlst ){
        hs->Add( h );
        leg->AddEntry( h, CompMgr().Discript( h ).c_str(), "l" );
    }

    TH1D* total = CompMgr().GetSample()->Hist( "tmass" );

    TLine* line = new TLine( 172.5, 0, 172.5, mgr::GetYmax( total ) * 1.6 );
    line->SetLineColor( kRed );
    line->SetLineStyle( 8 );

    TPaveText* pt = mgr::NewTextBox( .73, .81, .93, .87 );
    pt->AddText( "Muon Channel" );

    total->Draw();
    hs->Draw( "same" );
    leg->Draw();
    line->Draw();
    pt->Draw();

    mgr::SetSinglePad( c );
    mgr::SetAxis( total );
    total->SetMaximum( mgr::GetYmax( total ) * 1.6 );

    mgr::DrawCMSLabel( PRELIMINARY );
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "bbSeparation" ) );
    cout << endl << endl;
    cout << "tmass events peak  " << mgr::GetYmax( total ) << endl;
    cout << "tmass total events " << total->Integral() << endl;
    cout << "Correct   " << CompMgr().GetSample()->Hist( "correct" )->Integral() << " " << (double)CompMgr().GetSample()->Hist( "correct" )->Integral() / total->Integral() << endl;
    cout << "Fake b    " << CompMgr().GetSample()->Hist( "fakeb" )->Integral() << " " << (double)CompMgr().GetSample()->Hist( "fakeb" )->Integral() / total->Integral() << endl;
    cout << "Mistag    " << CompMgr().GetSample()->Hist( "mistag" )->Integral() << " " << (double)CompMgr().GetSample()->Hist( "mistag" )->Integral() / total->Integral() << endl;
    cout << "Swapped   " << CompMgr().GetSample()->Hist( "swap" )->Integral() << " " << (double)CompMgr().GetSample()->Hist( "swap" )->Integral() / total->Integral() << endl;
    cout << "Other     " << CompMgr().GetSample()->Hist( "other" )->Integral() << " " << (double)CompMgr().GetSample()->Hist( "other" )->Integral() / total->Integral() << endl;
    cout << "Mismatched " << CompMgr().GetSample()->Hist( "mismatched" )->Integral() << " " << (double)CompMgr().GetSample()->Hist( "mismatched" )->Integral() / total->Integral() << endl;
    delete pt;
    delete leg;
    delete line;
    delete hs;
    delete c;
}

extern void
MergeSample( vector<TH1D*>& histlst, TH1D* hist, const string& source )
{
    for( auto h : histlst ){
        if( IsXsample( source, h->GetName() ) ){
            h->Add( hist );
        }
    }
}
extern void
WeightMC( const string& source, TH1D* h )
{
    double lumi    = CompMgr().GetSingleData<double>( "lumi" );
    double xs      = mgr::GetSingle<double>( "cross_section", CompMgr().GetSubTree( source ) );
    double gen_num = mgr::GetSingle<double>( "gen_num", CompMgr().GetSubTree( source ) );

    h->Sumw2();    //apply before weighting, if you need any calcuation of the weighted histo
    h->Scale( (lumi * xs) / gen_num );
}

extern bool
IsXsample( const string& source, const string& tag )
{
    size_t found = source.find( tag );

    if( found != std::string::npos ){
        return true;
    }
    else{
        return false;
    }
}

extern double
GetErrSum(TH1D* hist)
{
    double binerror = 0;
    for( int i = 0; i < hist->GetSize(); ++i ){
        binerror += ( hist->GetBinError( i ) * hist->GetBinError( i ) );
    }

    return sqrt(binerror);
}

extern void
PlotSample()
{
    CompMgr().ChangeFile("DataMCSample.json");
    
    vector<string> samplelst = CompMgr().GetListData<string>( "samplelst" );
    vector<TH1D*>  histlst;
    
    histlst.push_back( new TH1D( "DYJet", "", 50, 0, 500 ) );
    histlst.push_back( new TH1D( "SingleTop", "", 50, 0, 500 ) );
    histlst.push_back( new TH1D( "VV", "", 50, 0, 500 ) );
    histlst.push_back( new TH1D( "WJet", "", 50, 0, 500 ) );
    histlst.push_back( new TH1D( "TTbar", "", 50, 0, 500 ) );
    histlst.push_back( new TH1D( "run", "", 50, 0, 500 ) );

    TH1D* bg = new TH1D( "bg", "", 50, 0, 500 );

    for(const auto s : samplelst){
        
        cout<<">>Processing Sample : "<<s<<endl;

        string file = CompMgr().ResultsDir() / s + "_muon.root";
        
        TFile* f = TFile::Open( file.c_str() );
        TH1D*  h = (TH1D*)( f->Get("tmass")->Clone() );
        h->SetDirectory(0); 
        
        //weight mc
        if( !IsXsample( s, "run" ) ){
            cout<<">>Weighting sample : "<<s<<endl;
            WeightMC( s, h );
        }

        MergeSample( histlst, h, s );
        
        cout<<">>Merging Sample : "<<s<<endl;
        f->Close();
    }

    TCanvas* c   = mgr::NewCanvas();
    Color_t x[]  = { kGray + 1, kMagenta + 2, kRed - 7, kOrange + 1, kAzure - 3, kGreen - 6 };
    THStack* hs  = new THStack();
    TLegend* leg = mgr::NewLegend( 0.65, 0.45, 0.75, 0.75 );
    leg->SetLineColor( kWhite );
    
    for( int i = 0; i < 5; i++ ){
        histlst[ i ]->SetLineColor( x[ i ] );
        histlst[ i ]->SetFillColor( x[ i ] );
        bg->Add( histlst[ i ] );
        hs->Add( histlst[ i ] );
        leg->AddEntry( histlst[ i ], histlst[ i ]->GetName(), "F" );
    }
   
    //expected yield
    cout<<"Data       "<<histlst[5]->Integral()<<endl;
    cout<<"Signal     "<<histlst[4]->Integral()<<" error "<<GetErrSum(histlst[4])<<endl;
    cout<<"background "<<bg        ->Integral()<<" error "<<GetErrSum(bg)<<endl;


    TPad* top = mgr::NewTopPad();
    top->Draw();
    top->cd();

    histlst.back()->Draw("EP");
    hs->Draw("same histo");
    histlst.back()->Draw("EP same");
    leg->Draw();
    
    mgr::SetTopPlotAxis( histlst.back() );
    histlst.back()->SetMaximum( mgr::GetYmax( histlst.back() ) * 1.2 );
    histlst.back()->SetStats( false );
    histlst.back()->SetLineColor(1);
    histlst.back()->SetLineWidth(1);
    histlst.back()->SetMarkerSize(0.5);
    histlst.back()->SetMarkerStyle(20);
    histlst.back()->GetYaxis()->SetTitle("Events");
    leg->AddEntry(histlst.back(), "Data", "le");

    c->cd();

    TPad* bot = mgr::NewBottomPad();
    bot->Draw();
    bot->cd();

    TLine* line     = new TLine( 0, 1, 500, 1 );
    TH1D* rel = mgr::DivideHist( histlst.back(), bg, 1 );

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
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "Stack_mass" ) );
   
    //delete hs;
    delete leg;
    for(auto h : histlst){
        delete h;
    }
    
    delete c;
}
