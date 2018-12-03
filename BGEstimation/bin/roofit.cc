#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooDataHist.h"
#include "RooPolynomial.h"
#include "RooHistPdf.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
#include "RooAddPdf.h"
#include "TH1.h"
#include "TFile.h"
#include "RooFitResult.h"
#include "RooLinkedList.h"
#include <initializer_list>
#include "RooCurve.h"
#include <string>
#include "TMath.h"
#include "THStack.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"

using namespace std;
using namespace RooFit ;

TObject* GetTObj( RooPlot* frame )
{
    return frame->getObject( frame->numItems() - 1 );
}

template<typename T, typename  ... LS>
void PlotOn( RooPlot* frame, T& obj, const std::string& objname, LS ...  Args) 
{
    vector<RooCmdArg> arglist{ Args ... };
    // Plot pdf on
    RooLinkedList oplist;
    for( const auto& args : arglist ){
        oplist.Add( args.Clone() );
    }

    RooPlot* test = obj.plotOn( frame, oplist );
    if( !test ){
      throw std::invalid_argument(
        "Bad argument list or object, plotting failed" );
    }

    // Rename obj pointer
    TNamed* ptr = dynamic_cast<TNamed*>( GetTObj( frame ) );
    ptr->SetName( objname.c_str() );
}

TObject* GetTObj( RooPlot* frame, const string& objname )
{
    return frame->findObject( objname.c_str() );
}

int
main( int argc, char* argv[] )
{
    
    opt::options_description de( "Command for SelectionCut" );
    de.add_options()
        ( "lepton,l", opt::value<string>()->required(), "which lepton" )
        ( "uncertainty,e", opt::value<string>(), "which lepton" )
    ;
    PlotMgr( "BGEstimation", "Template.json" ).AddOptions( de );
    const int run = PlotMgr().ParseOptions( argc, argv );
    
    if( run == mgr::Parsermgr::HELP_PARSER ){
        return 0;
    }
    
    if( run == mgr::Parsermgr::FAIL_PARSER ){
        return 1;
    }
    
    PlotMgr().SetFileName( { "lepton" } );
    
    string lepton = PlotMgr().GetOption<string>("lepton");
    string mass   = lepton == "mu" ? "M_{#mub} [GeV]" : "M_{eb} [GeV]";
    RooRealVar x("x", mass.c_str(), 0, 500) ;

    // signal template
    string template_sg = PlotMgr().GetSingleData<string>( lepton + "_template_sg" );
    
    if( PlotMgr().CheckOption( "uncertainty" ) ){
        template_sg.insert( template_sg.find_last_of("_"), "_uncertainty_" + PlotMgr().GetOption<string>( "uncertainty") );
    }

    TFile* f1 = TFile::Open( template_sg.c_str() );
    TH1D*  h1 = (TH1D*) ( f1->Get( "lep_tmass" )->Clone() );
    h1->SetDirectory( 0 );

    RooDataHist* dh1 = new RooDataHist("dh1","dh",x,Import(*h1)) ;
    RooHistPdf sg("sg", "sg", x, *dh1);
    
    // background template
    string template_bg = PlotMgr().GetSingleData<string>( lepton + "_template_bg" );
    TFile* f2 = TFile::Open( template_bg.c_str() );
    TH1D*  h2 = (TH1D*) ( f2->Get( "lep_tmass" )->Clone() );
    h2->SetDirectory( 0 );
    
    RooDataHist* dh2 = new RooDataHist("dh2", "dh2", x, Import(*h2)) ;
    RooHistPdf bg("bg", "bg", x, *dh2);

    // Construct composite pdf
    RooRealVar nsg("nsg", "nsg", 120000, 10000., 300000. ) ;
    RooRealVar nbg("nbg", "nbg", 4000,   0.,     40000.) ;
    RooAddPdf model("model", "model", RooArgList( sg, bg ), RooArgList( nsg, nbg ) ) ;

    // constraint template
    string template_cs = PlotMgr().GetSingleData<string>( lepton + "_template_cs" );
    TFile* f3 = TFile::Open( template_cs.c_str() );
    TH1D*  h3 = (TH1D*) ( f3->Get( "lep_tmass" )->Clone() );
    h3->SetDirectory( 0 );
    
    RooDataHist* data = new RooDataHist("data", "data", x, Import(*h3) ) ;

    // fitting
    model.fitTo( *data, Extended(true) );
    RooFitResult* result = model.fitTo( *data, Save(true), Extended(true) );
    cout<<"Result status : "<<result->status()<<endl;
    
    TCanvas* c = mgr::NewCanvas();
    
    h1->Scale( nsg.getValV() / h1->Integral() );
    h2->Scale( nbg.getValV() / h2->Integral() );

    TH1D* h1_up = (TH1D*)h1->Clone();
    TH1D* h2_up = (TH1D*)h2->Clone();

    h1_up->Scale( ( nsg.getError() + nsg.getValV() ) / nsg.getValV() );
    h2_up->Scale( ( nbg.getError() + nbg.getValV() ) / nbg.getValV() );


    THStack* mc = new THStack();
    h1->SetLineColor( kRed - 7 );
    h1->SetFillColor( kRed - 7 );
    h2->SetLineColor( kGreen - 6 );
    h2->SetFillColor( kGreen - 7 );
    mc->Add( h2 );
    mc->Add( h1 );
    
    TLegend* leg = mgr::NewLegend( 0.65, 0.51, 0.8, 0.81 );
    leg->SetLineColor( kWhite );
    leg->AddEntry( h3, "Data", "LE" );
    leg->AddEntry( h1, "Fitted t#bar{t}", "F");
    leg->AddEntry( h2, "Fitted background", "F" );

    TH1D* mc_sum = (TH1D*) h1->Clone();
    mc_sum->Add( h2 );

    TPad* top = mgr::NewTopPad();
    top->Draw();
    top->cd();

    mc->Draw("HIST");
    h3->Draw("EP same");
    leg->Draw();

    mgr::SetTopPlotAxis( mc );
    h3->SetLineColor( 1 );
    h3->SetLineWidth( 1 );
    h3->SetMarkerSize( 0.5 );
    h3->SetMarkerStyle( 20 );
   
    mc->SetMaximum( mgr::GetYmax( h3 ) * 1.5 );
    mc->GetYaxis()->SetTitle( "Events x 10^{3}" );
    mc->GetXaxis()->SetTitle( "" );
    
    c->cd();

    TPad* bot = mgr::NewBottomPad();
    bot->Draw();
    bot->cd();

    double xmin = h3->GetXaxis()->GetXmin();
    double xmax = h3->GetXaxis()->GetXmax();

    TLine* line       = new TLine( xmin, 1.0, xmax, 1.0 );
    TLine* upper_line = new TLine( xmin, 1.5, xmax, 1.5 );
    TLine* lower_line = new TLine( xmin, 0.5, xmax, 0.5 );
    TH1D* rel         = mgr::DivideHist( h3, mc_sum, 1 );

    rel->Draw( "EP" );
    upper_line->Draw( "same" );
    lower_line->Draw( "same" );
    line->Draw( "same" );

    line->SetLineColor( kRed );
    upper_line->SetLineStyle( 3 );
    lower_line->SetLineStyle( 3 );

    rel->SetMaximum( 1.6 );
    rel->SetMinimum( 0.4 );
    rel->GetYaxis()->SetTitle( "Data/Fit" );
    mgr::SetBottomPlotAxis( rel );

    c->cd();

    mgr::DrawCMSLabelOuter( PRELIMINARY );
    int lumi = lepton == "el" ? 35700 : 35900;
    mgr::DrawLuminosity( lumi ); 
    c->SaveAs( (lepton + "fit.pdf" ).c_str() );

 /*   double nsg_val = nsg.getValV();*/
    //double nbg_val = nbg.getValV();
    //double nsg_err = nsg.getError();
    //double nbg_err = nbg.getError();
    //double cov_err = result->correlation("nsg","nbg") * nsg_err * nbg_err ;
    //double sum_err = TMath::Sqrt( nsg_err * nsg_err + nbg_err * nbg_err + 2* cov_err );
    //double fra_err = TMath::Sqrt( nsg_val * nsg_val * nbg_err * nbg_err + nbg_val * nbg_val * nsg_err * nsg_err - 2 * nsg_val * nbg_val * cov_err );

 /*   cout<<"Data events : "  <<h3->Integral()<<endl;*/
    
    //cout<<"Fitted sg events : "<<h1->Integral() <<"  "<<nsg_err<<endl;
    //cout<<"Fitted bg events : "<<h2->Integral() <<"  "<<nbg_err<<endl;
    //cout<<"Fitted events : "<<mc_sum->Integral()<<"  "<<sum_err<<endl;
    //cout<<"Signal fraction : "<<h1->Integral() / mc_sum->Integral()<<"  "<<fra_err / ( mc_sum->Integral() * mc_sum->Integral() )<<endl;
    
    //cout<<"**************************************************************"<<endl;

    double nsg_val = h1->Integral( 1, h1->FindBin( 150 ) - 1 );
    double nbg_val = h2->Integral( 1, h2->FindBin( 150 ) - 1 );
    double nsg_err = h1_up->Integral( 1, h1_up->FindBin( 150 ) - 1 ) -  h1->Integral( 1, h1->FindBin( 150 ) - 1 );
    double nbg_err = h2_up->Integral( 1, h2_up->FindBin( 150 ) - 1 ) -  h2->Integral( 1, h2->FindBin( 150 ) - 1 );
    double cov_err = result->correlation("nsg","nbg") * nsg_err * nbg_err ;
    double sum_err = TMath::Sqrt( nsg_err * nsg_err + nbg_err * nbg_err + 2* cov_err );
    double fra_err = TMath::Sqrt( nsg_val * nsg_val * nbg_err * nbg_err + nbg_val * nbg_val * nsg_err * nsg_err - 2 * nsg_val * nbg_val * cov_err ) / ( ( h1->Integral( 1, h1->FindBin( 150 ) - 1 ) + h2->Integral( 1, h2->FindBin( 150 ) - 1 ) ) * ( h1->Integral( 1, h1->FindBin( 150 ) - 1 ) + h2->Integral( 1, h2->FindBin( 150 ) - 1 ) ) );
   
    cout<< std::fixed<<std::setprecision(5);
    cout<<"Data events : "  <<h3->Integral( 1, h3->FindBin( 150 ) - 1 )<<endl;
    cout<<"Fitted sg events after cut : "<<h1->Integral( 1, h1->FindBin( 150 ) - 1 ) <<"  "<<nsg_err<<endl;
    cout<<"Fitted bg events after cut : "<<h2->Integral( 1, h2->FindBin( 150 ) - 1 ) <<"  "<<nbg_err<<endl;

    cout<<"Fitted events after cut : "   <<h1->Integral( 1, h1->FindBin( 150 ) - 1 ) + h2->Integral( 1, h2->FindBin( 150 ) - 1 )<<"  "<<sum_err<<endl;
    cout<<"Signal fraction after cut : " <<h1->Integral( 1, h1->FindBin( 150 ) - 1 ) / ( h1->Integral( 1, h1->FindBin( 150 ) - 1 ) + h2->Integral( 1, h2->FindBin( 150 ) - 1 ) ) <<"  "<<fra_err<<endl;
    

}
    




//PlotOn( frame, *data, "" );
    //PlotOn( frame, model, "sg", FillStyle(1001), FillColor(kRed + 1),LineColor(kRed + 1), DrawOption("FL") );
    //PlotOn( frame, model, "bg", Components(bg), FillStyle(1001), FillColor(kGreen - 2), LineColor(kGreen - 2), DrawOption("FL") );
    //PlotOn( frame, *data, "data" );

    //leg->AddEntry( GetTObj( frame, "data"), "Data", "EP" );
    //leg->AddEntry( GetTObj( frame, "sg"),   "Fitted t#bar{t}", "F" );
    //leg->AddEntry( GetTObj( frame, "bg"),   "Fitted background", "F" );

