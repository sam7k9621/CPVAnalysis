#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern void
PlotCompare( vector<TH1D*> mclst, TH1D* data, const string& title, const string& entry )
{
    TCanvas* c   = mgr::NewCanvas();
    Color_t x[]  = { kGray + 1, kMagenta + 2, kRed - 7, kOrange + 1, kAzure - 3, kGreen - 6 };
    THStack* bg  = new THStack();
    TLegend* leg = mgr::NewLegend( 0.7, 0.51, 0.83, 0.81 );
    leg->SetLineColor( kWhite );

    for( int i = 0; i < (int)mclst.size(); i++ ){
        mclst[ i ]->SetLineColor( x[ i ] );
        mclst[ i ]->SetFillColor( x[ i ] );
        bg->Add( mclst[ i ] );
        leg->AddEntry( mclst[ i ], GetName( mclst[ i ] ).c_str(), "F" );
    }

    TH1D* bg_sum = SumHist( mclst );

    TPad* top = mgr::NewTopPad();
    top->Draw();
    top->cd();

    bg->Draw( "HIST" );
    data->Draw( "EP same" );
    leg->Draw();

    mgr::SetTopPlotAxis( bg );
    data->SetLineColor( 1 );
    data->SetLineWidth( 1 );
    data->SetMarkerSize( 0.5 );
    data->SetMarkerStyle( 20 );
    leg->AddEntry( data, "Data", "le" );
    
    bg->SetMaximum( mgr::GetYmax( data ) * 1.2 );
    bg->GetHistogram()->GetXaxis()->SetTitle( data->GetXaxis()->GetTitle() );
    bg->GetHistogram()->GetYaxis()->SetTitle( data->GetYaxis()->GetTitle() );
    
    c->cd();

    TPad* bot = mgr::NewBottomPad();
    bot->Draw();
    bot->cd();

    double xmin = data->GetXaxis()->GetXmin();
    double xmax = data->GetXaxis()->GetXmax();

    TLine* line       = new TLine( xmin, 1.0, xmax, 1.0 );
    TLine* upper_line = new TLine( xmin, 1.5, xmax, 1.5 );
    TLine* lower_line = new TLine( xmin, 0.5, xmax, 0.5 );
    TH1D* rel         = mgr::DivideHist( data, bg_sum, 1 );

    rel->Draw( "EP" );
    upper_line->Draw( "same" );
    lower_line->Draw( "same" );
    line->Draw( "same" );

    line->SetLineColor( kRed );
    upper_line->SetLineStyle( 3 );
    lower_line->SetLineStyle( 3 );

    rel->SetMaximum( 1.6 );
    rel->SetMinimum( 0.4 );
    rel->GetYaxis()->SetTitle( "Data/MC" );
    rel->GetXaxis()->SetTitle( data->GetXaxis()->GetTitle() );
    mgr::SetBottomPlotAxis( rel );

    c->cd();

    mgr::DrawCMSLabel( PRELIMINARY );
    mgr::DrawLuminosity( 36814 );
    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_MIN, PLOT_Y_MAX + TEXT_MARGIN / 2, BOTTOM_LEFT)
    .WriteLine( entry );

    top->SetLogy( kTRUE );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Stack_" + title + "_logy" ) );

    top->SetLogy( kFALSE );
    SetYTitle( bg->GetHistogram() );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Stack_" + title ) );
    
    delete leg;
    delete top;
    delete bot;
    delete line;
    delete upper_line;
    delete lower_line;
    delete bg_sum;

    for( auto h : mclst ){
        delete h;
    }

    delete bg;
    delete data;

    delete c;
}

extern void
PlotMC( vector<TH1D*> mclst, const string& title, const string& entry )
{
    TCanvas* c   = mgr::NewCanvas();
    Color_t x[]  = { kGray + 1, kMagenta + 2, kRed - 7, kOrange + 1, kAzure - 3, kGreen - 6 };
    THStack* bg  = new THStack();
    TLegend* leg = mgr::NewLegend( 0.7, 0.51, 0.83, 0.81 );
    leg->SetLineColor( kWhite );

    for( int i = 0; i < (int)mclst.size(); i++ ){
        mclst[ i ]->SetLineColor( x[ i ] );
        mclst[ i ]->SetFillColor( x[ i ] );
        bg->Add( mclst[ i ] );
        leg->AddEntry( mclst[ i ], GetName( mclst[ i ] ).c_str(), "F" );
    }

    TH1D* bg_sum = SumHist( mclst );

    bg->Draw( "HIST" );
    leg->Draw();

    mgr::SetSinglePad( c );
    mgr::SetAxis( bg );
    bg->SetMaximum( mgr::GetYmax(bg_sum) * 1.2 );
    bg->GetHistogram()->GetXaxis()->SetTitle( bg_sum->GetXaxis()->GetTitle() );
    bg->GetHistogram()->GetYaxis()->SetTitle( bg_sum->GetYaxis()->GetTitle() );
    
    mgr::DrawCMSLabel( SIMULATION );
    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_MIN, PLOT_Y_MAX + TEXT_MARGIN / 2, BOTTOM_LEFT)
    .WriteLine( entry );
    
    c->SetLogy( kTRUE );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Simulation_" + title + "_logy" ) );
    
    c->SetLogy( kFALSE );
    SetYTitle( bg->GetHistogram() );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Simulation_" + title ) );

    delete leg;
    delete bg_sum;

    for( auto h : mclst ){
        delete h;
    }

    delete bg;
    delete c;
}

extern void
PlotPDF(
    vector<TH1D*> correctlst,
    vector<TH1D*> misidlst,
    vector<TH1D*> mistaglst,
    vector<TH1D*> nomatchedlst
    )
{
    TH1D* correct   = SumHist( correctlst );
    TH1D* misid     = SumHist( misidlst );
    TH1D* mistag    = SumHist( mistaglst );
    TH1D* nomatched = SumHist( nomatchedlst );

    PlotIntegral( correct, misid, mistag, nomatched );

    TCanvas* c   = mgr::NewCanvas();
    TLegend* leg = mgr::NewLegend( 0.7, 0.57, 0.83, 0.87 );
    leg->SetLineColor( kWhite );

    correct->Draw( "hist" );
    misid->Draw( "hist same" );
    mistag->Draw( "hist same" );
    nomatched->Draw( "hist same" );

    correct->SetLineColor( kAzure - 3 );
    misid->SetLineColor( kGreen - 6 );
    mistag->SetLineColor( kRed - 7 );
    nomatched->SetLineColor( kGray + 1 );

    correct->SetLineWidth( 2 );
    misid->SetLineWidth( 2 );
    mistag->SetLineWidth( 2 );
    nomatched->SetLineWidth( 2 );

    leg->Draw();

    mgr::SetNormToUnity( correct );
    mgr::SetNormToUnity( misid );
    mgr::SetNormToUnity( mistag );
    mgr::SetNormToUnity( nomatched );

    mgr::SetSinglePad( c );
    mgr::SetAxis( correct );
    correct->SetMaximum( 1. );

    leg->AddEntry( correct,   "Correct",       "l" );
    leg->AddEntry( misid,     "Misidentified", "l" );
    leg->AddEntry( mistag,    "Mistag",        "l" );
    leg->AddEntry( nomatched, "No matched",    "l" );

    c->SetLogy( kTRUE );
    mgr::DrawCMSLabel( SIMULATION );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "PDF_matchtype" ) );

    delete leg;

    for( auto h : correctlst ){
        delete h;
    }

    for( auto h : misidlst ){
        delete h;
    }

    for( auto h : mistaglst ){
        delete h;
    }

    for( auto h : nomatchedlst ){
        delete h;
    }

    delete c;
}

void
PlotIntegral( TH1D* correct, TH1D* misid, TH1D* mistag, TH1D* nomatched )
{
    TH1D* cor_eff = new TH1D( "cor_eff", "", 200, 0, 200 );
    TH1D* tag_eff = new TH1D( "tag_eff", "", 200, 0, 200 );
    TH1D* ide_eff = new TH1D( "ide_eff", "", 200, 0, 200 );
    TH1D* nom_eff = new TH1D( "nom_eff", "", 200, 0, 200 );
    TH1D* eve_eff = new TH1D( "eve_eff", "", 200, 0, 200 );

    double total    = correct->Integral() + misid->Integral() + mistag->Integral() + nomatched->Integral();

    for( int i = 1; i <= 200; i++ ){
        double cor = correct->Integral( 1, i );
        double ide = misid->Integral( 1, i );
        double tag = mistag->Integral( 1, i );
        double nom = nomatched->Integral( 1, i );
        double sum = cor + ide + tag + nom;

        cor_eff->SetBinContent( i, cor / sum );
        tag_eff->SetBinContent( i, tag / sum );
        ide_eff->SetBinContent( i, ide / sum );
        nom_eff->SetBinContent( i, nom / sum );

        eve_eff->SetBinContent( i, sum / total );
    }

    TCanvas* c   = mgr::NewCanvas();
    TLegend* leg = mgr::NewLegend( 0.7, 0.57, 0.83, 0.87 );
    leg->SetLineColor( kWhite );
    leg->SetFillColorAlpha( 0, 1 );
    cor_eff->Draw( "hist" );
    tag_eff->Draw( "hist same" );
    ide_eff->Draw( "hist same" );
    nom_eff->Draw( "hist same" );
    eve_eff->Draw( "hist same" );
    leg->Draw();

    cor_eff->SetStats( 0 );
    cor_eff->GetXaxis()->SetTitle( "#chi^{2}_{min} upper cut" );
    cor_eff->GetYaxis()->SetTitle( "Rate" );

    cor_eff->SetLineColor( kAzure - 3 );
    tag_eff->SetLineColor( kGreen - 6 );
    ide_eff->SetLineColor( kRed - 7 );
    nom_eff->SetLineColor( kMagenta + 2 );
    eve_eff->SetLineColor( kOrange + 1 );

    cor_eff->SetLineWidth( 2 );
    tag_eff->SetLineWidth( 2 );
    ide_eff->SetLineWidth( 2 );
    nom_eff->SetLineWidth( 2 );
    eve_eff->SetLineWidth( 2 );


    cor_eff->SetMaximum( 1. );
    cor_eff->SetMinimum( 0. );
    mgr::SetSinglePad( c );
    mgr::SetAxis( cor_eff );

    leg->AddEntry( cor_eff, "Correct",          "l" );
    leg->AddEntry( tag_eff, "Mistag",           "l" );
    leg->AddEntry( ide_eff, "Misidentified",    "l" );
    leg->AddEntry( nom_eff, "No matched",       "l" );
    leg->AddEntry( eve_eff, "Event efficiency", "l" );

    c->SetGrid();
    mgr::DrawCMSLabelOuter( SIMULATION );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Chi2_uppercut" ) );

    delete leg;
    delete cor_eff;
    delete tag_eff;
    delete ide_eff;
    delete nom_eff;
    delete eve_eff;

    delete c;
}

void
Plot2D( vector<TH2D*> mclst )
{
    TH2D* hist = SumHist2D( mclst );
    TCanvas* c = mgr::NewCanvas();

    hist->Draw( "COLZ" );

    mgr::SetAxis( hist );
    mgr::SetSinglePad( c );
    c->SetRightMargin( 1 );
    mgr::DrawCMSLabelOuter( SIMULATION );


    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "2D_chi2_tmass" ) );

    for( auto h : mclst ){
        delete h;
    }

    delete c;
}

void 
GetYield( vector<TH1D*> mclst, TH1D* data )
{
    for( const auto& h : mclst ){
        cout<<h->GetName()<<" "<<h->Integral()<<endl;
    }
    cout<<"data "<<data->Integral()<<endl;
}
