#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
#include "TGraphErrors.h"
#include "boost/format.hpp"
#include <iomanip>

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
    
    bg->SetMaximum( mgr::GetYmax( data ) * 1.5 );
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

    string lepton = PlotMgr().GetOption<string>("lepton");
    int lumi = lepton == "el" ? 35700 : 35900;
    mgr::DrawCMSLabel( PRELIMINARY );
    mgr::DrawLuminosity( lumi );
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
    bg->SetMaximum( mgr::GetYmax(bg_sum) * 1.5 );
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
    vector<TH1D*> mistaglst
    )
{
    TH1D* correct   = SumHist( correctlst );
    TH1D* misid     = SumHist( misidlst );
    TH1D* mistag    = SumHist( mistaglst );

    PlotIntegral( correct, misid, mistag );

    TCanvas* c   = mgr::NewCanvas();
    TLegend* leg = mgr::NewLegend( 0.7, 0.65, 0.83, 0.87 );
    leg->SetLineColor( kWhite );

    correct->Draw( "hist" );
    misid->Draw( "hist same" );
    mistag->Draw( "hist same" );

    correct->SetLineColor( kAzure - 3 );
    misid->SetLineColor( kGreen - 6 );
    mistag->SetLineColor( kRed - 7 );

    correct->SetLineWidth( 2 );
    misid->SetLineWidth( 2 );
    mistag->SetLineWidth( 2 );

    leg->Draw();


    mgr::SetNormToUnity( correct );
    mgr::SetNormToUnity( misid );
    mgr::SetNormToUnity( mistag );

    mgr::SetSinglePad( c );
    mgr::SetAxis( correct );
    correct->SetMaximum( 1. );

    leg->AddEntry( correct,   "Correct",       "l" );
    leg->AddEntry( misid,     "b-Misidentified", "l" );
    leg->AddEntry( mistag,    "Mistag",        "l" );

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

    delete c;
}

extern void
PlotIntegral( TH1D* correct, TH1D* misid, TH1D* mistag )
{
    TH1D* cor_eff = new TH1D( "cor_eff", "", 200, 0, 200 );
    TH1D* tag_eff = new TH1D( "tag_eff", "", 200, 0, 200 );
    TH1D* ide_eff = new TH1D( "ide_eff", "", 200, 0, 200 );
    TH1D* eve_eff = new TH1D( "eve_eff", "", 200, 0, 200 );

    double total = correct->Integral() + misid->Integral() + mistag->Integral();
    cout<<"**********************************************"<<endl;
    cout<< "correct "<<(double) correct->Integral() / total <<endl;
    cout<< "misid   "<<(double) misid->Integral() / total <<endl;
    cout<< "mistag  "<<(double) mistag->Integral() / total <<endl;


    for( int i = 1; i <= 200; i++ ){
        double cor = correct->Integral( 1, i );
        double ide = misid->Integral( 1, i );
        double tag = mistag->Integral( 1, i );
        double sum = cor + ide + tag;

        cor_eff->SetBinContent( i, cor / sum );
        tag_eff->SetBinContent( i, tag / sum );
        ide_eff->SetBinContent( i, ide / sum );

        eve_eff->SetBinContent( i, sum / total );
    }

    TCanvas* c   = mgr::NewCanvas();
    TLegend* leg = mgr::NewLegend( 0.7, 0.57, 0.92, 0.87 );
    leg->SetTextSize( 16 );
    leg->SetLineColor( kWhite );
    leg->SetFillColorAlpha( 0, 1 );
    cor_eff->Draw( "hist" );
    tag_eff->Draw( "hist same" );
    ide_eff->Draw( "hist same" );
    eve_eff->Draw( "hist same" );
    leg->Draw();

    cor_eff->SetStats( 0 );
    cor_eff->GetXaxis()->SetTitle( "#chi^{2}_{min} upper cut" );
    cor_eff->GetYaxis()->SetTitle( "Rate" );

    cor_eff->SetLineColor( kAzure - 3 );
    tag_eff->SetLineColor( kGreen - 6 );
    ide_eff->SetLineColor( kMagenta + 2 );
    eve_eff->SetLineColor( kOrange + 1 );

    cor_eff->SetLineWidth( 2 );
    tag_eff->SetLineWidth( 2 );
    ide_eff->SetLineWidth( 2 );
    eve_eff->SetLineWidth( 2 );

    TLine* line = new TLine( 20, 0, 20, 1 );
    line->Draw("same");
    line->SetLineColor( kRed - 7 );
    line->SetLineWidth( 2 );

    cor_eff->SetMaximum( 1. );
    cor_eff->SetMinimum( 0. );
    mgr::SetSinglePad( c );
    mgr::SetAxis( cor_eff );

    leg->AddEntry( cor_eff, "Correct",          "l" );
    leg->AddEntry( tag_eff, "Mistag",           "l" );
    leg->AddEntry( ide_eff, "b-Misidentified",    "l" );
    leg->AddEntry( eve_eff, "Event efficiency", "l" );

    c->SetGrid();
    mgr::DrawCMSLabelOuter( SIMULATION );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Chi2_uppercut" ) );

    delete leg;
    delete cor_eff;
    delete tag_eff;
    delete ide_eff;
    delete eve_eff;

    delete c;
}

extern void
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

extern void 
GetYield( vector<TH1D*> mclst, TH1D* data )
{
    ofstream output;
    output.open( PlotMgr().GetResultsName( "txt", "Yield" ) );

    output << std::fixed<<std::setprecision(5);
    for( const auto& h : mclst ){
        output<<h->GetName()<<" "<<h->Integral()<<endl;
    }
    output<<"data "<<data->Integral()<<endl;
}

extern void 
TestBGLike( vector<TH1D*> mclst, TH1D* data, TH1D* data2 )
{
    //without signal MC
    TH1D* sumMC = SumHist( mclst );
    
    TCanvas* c   = mgr::NewCanvas();
    TLegend* leg = mgr::NewLegend( 0.68, 0.57, 0.8, 0.87 );
    leg->SetLineColor( kWhite );

    sumMC->Draw( "hist e" );
    data2->Draw( "ep same" );
    data ->Draw( "ep same" );
    leg->Draw();

    data  ->SetLineColor( kAzure - 3 );
    sumMC ->SetLineColor( kRed - 7 );
    data2 ->SetLineColor( kGreen - 6 );
    data  ->SetLineWidth( 2 );
    sumMC ->SetLineWidth( 2 );
    data2 ->SetLineWidth( 2 );
  
    sumMC->SetMaximum( mgr::GetYmax( sumMC) * 1.5 );
    sumMC->GetYaxis()->SetTitle( "PDF" );

    leg->SetHeader( ( PlotMgr().GetOption<string>("lepton")+"-channel" ).c_str() );
    leg->AddEntry( data , ( PlotMgr().GetOption<string>("data") + " data ( #chi^{2} < 20 )" ).c_str(), "lep" );
    leg->AddEntry( data2, ( PlotMgr().GetOption<string>("data") + " data ( #chi^{2} > 20 )" ).c_str(), "lep" );
    leg->AddEntry( sumMC, ( PlotMgr().GetOption<string>("mc")   + " bkg. MC" ).c_str(), "lep" );

    mgr::SetNormToUnity( data );
    mgr::SetNormToUnity( data2 );
    mgr::SetNormToUnity( sumMC );

    mgr::SetSinglePad( c );
    mgr::SetAxis( sumMC );

    mgr::DrawCMSLabelOuter( PRELIMINARY );
    string lepton = PlotMgr().GetOption<string>("lepton");
    int lumi = lepton == "el" ? 35700 : 35900;
    mgr::DrawLuminosity( lumi );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "CompareCS" ) );
    
}

extern void
PlotOpt( vector<TH1D*> cor, vector<TH1D*> misid, vector<TH1D*> mistag )
{
    TH1D* sumCor = SumHist( cor );
    TH1D* sumMisid = SumHist( misid );
    TH1D* sumMistag = SumHist( mistag );

    TCanvas* c = mgr::NewCanvas();
    TLegend* leg = mgr::NewLegend( 0.68, 0.57, 0.8, 0.87 );
    leg->SetLineColor( kWhite );

    sumCor->Draw("hist");
    sumMisid->Draw("hist same");
    sumMistag->Draw("hist same");
    leg->Draw();

    sumCor   ->SetLineColor( kAzure - 3 );
    sumMisid ->SetLineColor( kGreen - 6 );
    sumMistag->SetLineColor( kMagenta + 2 );
    sumCor   ->SetLineWidth( 2 );
    sumMisid ->SetLineWidth( 2 );
    sumMistag->SetLineWidth( 2 );

    mgr::SetSinglePad( c );
    mgr::SetAxis( sumCor );
    sumCor->SetMaximum( mgr::GetYmax( sumCor ) * 1.1 ); 
    sumCor->GetYaxis()->SetTitle( "Events x 10^{3}" );
    sumCor->GetXaxis()->SetTitle( "M_{lb} [GeV]" );

    leg->AddEntry( sumCor,    "Correct",   "l" );
    leg->AddEntry( sumMisid,  "Mistidentified", "l" );
    leg->AddEntry( sumMistag, "Mistag", "l" );

    mgr::DrawCMSLabelOuter( SIMULATION );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Optimisation" ) );

    sumMistag->Add( sumMisid );
    TH1D* eff_cor = EffHist( sumCor, sumMistag );

    eff_cor->Draw( "hist e" );
    eff_cor->SetLineColor( kAzure - 3 );
    eff_cor->SetLineWidth( 2 );
    eff_cor->GetYaxis()->SetTitle( "Correct b-tagged rate" );
    eff_cor->GetXaxis()->SetTitle( "M_{lb} [GeV]" );
    eff_cor->SetMaximum( 1 );

    TLegend* leg2 = mgr::NewLegend( 0.68, 0.57, 0.8, 0.87 );
    leg2->Draw();
    leg2->SetLineColor( kWhite );
    leg2->AddEntry( eff_cor, "Correct type", "l" );

    TLine* line = new TLine( 150, 0, 150, 1 );
    line->Draw("same");
    line->SetLineColor( kRed - 7 );
    line->SetLineWidth( 2 );
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( eff_cor );
    mgr::DrawCMSLabelOuter( SIMULATION );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Correct_eff" ) );

    delete line;
    delete leg;
    delete c;

}

extern TH1D* 
EffHist( TH1D* num, TH1D* den )
{
    TH1D* numm = (TH1D*)( num->Clone() );
    TH1D* denn = (TH1D*)( den->Clone() );
    denn->Add( numm );
    numm->Divide( denn );
    
    for( int i = 0; i < numm->GetNcells(); ++i ){
        
        double N = denn->GetBinContent( i );
        double p = numm->GetBinContent( i );

        if( N == 0 ){
            numm->SetBinContent( i, 0 );
            continue;
        }

        numm->SetBinError( i, TMath::Sqrt( p * ( 1-p ) / N ) );
    }
    
    return numm;
}

extern void
GetAcp( const vector<string>& mclst )
{
    TH1D* hist = new TH1D( "hist", "", 4, 0, 4 );
    TH1D* err1 = new TH1D( "err1", "", 4, 0, 4 );
    TH1D* err2 = new TH1D( "err2", "", 4, 0, 4 );
   
    string lepton = PlotMgr().GetOption<string>("lepton");
    lepton =  lepton == "co" ? "el+#mu" : lepton;
   
    vector<string> obsentry = {
        str( boost::format("#bf{O_{3}^{%1%}}") % lepton ),
        str( boost::format("#bf{O_{6}^{%1%}}") % lepton ),
        str( boost::format("#bf{O_{12}^{%1%}}") % lepton ),
        str( boost::format("#bf{O_{13}^{%1%}}") % lepton )
    };
    
    for( int i = 0; i < 4; i++ ){
        
        TH1D* h = ExtractData( mclst[i] );
        //TH1D* h = SumHist( ExtractMC( mclst[i] ) );
        double nm  = h->Integral( 0, h->FindBin(0) - 1 );
        double np  = h->Integral( h->FindBin(0), 201 );
        double Acp = ( np - nm ) / ( np + nm );

        double err_sq = 4. * np * nm / TMath::Power( (np + nm), 3 );
        double err    = TMath::Sqrt( err_sq );
 
        hist->SetBinContent( i + 1, Acp * 100 );
        hist->SetBinError  ( i + 1, err * 100 );
        err1->SetBinContent( i + 1, Acp * 100 );
        err1->SetBinError  ( i + 1, err * 100 );
        err2->SetBinContent( i + 1, Acp * 100 );
        err2->SetBinError  ( i + 1, err * 200 );
        
        cout<<"idx "<<i<<endl;
        cout<<"Acp "<<Acp * 100 << "  "<<err * 100<<endl;

        hist->GetXaxis()->SetBinLabel( i + 1, obsentry[i].c_str() );
    }

    TCanvas* c = mgr::NewCanvas();
    hist->Draw();
    err2->Draw("E2 same");
    err1->Draw("E2 same");
    hist->Draw("EP same");
    
    err2->SetFillColor( kYellow );
    err1->SetFillColor( kGreen );

    TLine* line = new TLine( 0, 0, 4, 0);
    line->Draw( "same" );
    line->SetLineColor( kRed );
    line->SetLineWidth( 2 );
  
    TLegend* leg = mgr::NewLegend( 0.65, 0.7, 0.8, 0.87 );
    leg->Draw();
    leg->AddEntry( hist, "Nominal value", "LEP" );
    leg->AddEntry( err1, "1#sigma stat. error", "F" );
    leg->AddEntry( err2, "2#sigma stat. error", "F" );

    hist->SetMaximum( ceil( mgr::GetYmax( hist ) ) );
    hist->SetMinimum( -1 * ceil( mgr::GetYmax( hist ) ) );
    //hist->SetMaximum( 0.4 );
    //hist->SetMinimum( -0.4 );
    hist->GetYaxis()->SetTitle("A'_{cp}[%]");
    hist->GetXaxis()->SetLabelSize( 19 );
    hist->SetStats(0);
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( hist );
    int lumi = lepton == "el" ? 35700 : 35900;
    mgr::DrawCMSLabel( PRELIMINARY );
    //mgr::DrawCMSLabel( SIMULATION );
    mgr::DrawLuminosity( lumi );
    mgr::SaveToPDF( c, PlotMgr().GetResultsName( "pdf", "Simulation_Acp" ) );
}

extern double
GetNorm()
{
    TH1D*  h      = ExtractData( "lep_tmass", "CS" );
    double expected = h->Integral( 1, h->FindBin(150) - 1 );
    double fitted   = PlotMgr().GetOption<double>( "fitted" );
    return fitted / expected;
}

extern void
GetSubtractAcp( const vector<string>& mclst )
{
    for( int i = 0; i < 4; i++ ){
        TH1D*  data    = ExtractData( mclst[i] );
        double data_nm = data->Integral( 0, data->FindBin(0) - 1 );
        double data_np = data->Integral( data->FindBin(0), 201 );
        
        TH1D*  bg      = ExtractData( mclst[i], "CS" );
        double bg_nm   = bg->Integral( 0, bg->FindBin(0) - 1 );
        double bg_np   = bg->Integral( bg->FindBin(0), 201 );        
    
        double norm = GetNorm();
        bg_nm *= norm;
        bg_np *= norm;

        cout<<"data_nm "<<data_nm<<endl;
        cout<<"data_np "<<data_np<<endl;
        cout<<"bg_nm "<<bg_nm<<endl;
        cout<<"bg_np "<<bg_np<<endl;

        data_nm -= bg_nm;
        data_np -= bg_np;
        
        double Acp = 100 * ( data_np - data_nm ) / ( data_np + data_nm );
        cout<<"idx "<<i<<endl;
        cout<<"Acp "<<Acp<<endl;
    }
}
