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
#include "RooRandom.h"

#include <string>

#include "ManagerUtils/PlotUtils/interface/Common.hpp"

using namespace std;
using namespace RooFit ;

void GaussFit( vector<double> pull, const string& name )
{
    RooRealVar p    ( "p", "Pull Dist.", -4., 4. );
    RooRealVar mu   ( "mu", "mean value", 0., -3., 3. );
    RooRealVar sigma( "sigma", "sigma", 1., -3., 3. );

    RooGaussian gaus( "gaus", "gauss", p, mu, sigma );
    RooDataSet  pullset( "pullset", "pullset", RooArgSet( p ) );
    for( const auto& pu : pull ){
        p.setVal( pu );
        pullset.add( RooArgSet( p ) );
    }
    
    gaus.fitTo( pullset );
    
    RooPlot* frame = p.frame();
    pullset.plotOn( frame );
    gaus.plotOn( frame );

    TCanvas* c = mgr::NewCanvas();
    frame->Draw() ;
    mgr::DrawCMSLabelOuter( PRELIMINARY );
    mgr::DrawLuminosity( 35900 ); 

    TPaveText* pt = mgr::NewTextBox( .55, .80, .92, .90 );
    pt->AddText( ( "mean  : " + to_string( mu.getVal() )    + " #pm " + to_string( mu.getError()    ) ).c_str() );
    pt->AddText( ( "sigma : " + to_string( sigma.getVal() ) + " #pm " + to_string( sigma.getError() ) ).c_str() );
    pt->Draw();

    frame->SetTitle( "" );
    mgr::SetSinglePad( c );
    mgr::SetAxis( frame );

    c->SaveAs( name.c_str() );
}

int
main( )
{
    string lepton = "mu";
    string mass   = "M_{#mub} [GeV]";

    RooRealVar x("x", mass.c_str(), 0, 500) ;

    // signal template
    TFile* f1 = TFile::Open( ( "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/CompareDataMC/results/HistMerge_"+lepton+"_TTbar_chi2_20_pileup_SR.root" ).c_str() );
    TH1D*  h1 = (TH1D*) ( f1->Get( "lep_tmass" )->Clone() );
    h1->SetDirectory( 0 );

    RooDataHist* dh1 = new RooDataHist("dh1","dh",x,Import(*h1)) ;
    RooHistPdf sg("sg", "sg", x, *dh1);
    
    // background template
    TFile* f2 = TFile::Open( ( "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/CompareDataMC/results/HistMerge_"+lepton+"_Data_chi2_20_pileup_CS.root" ).c_str() );
    TH1D*  h2 = (TH1D*) ( f2->Get( "lep_tmass" )->Clone() );
    h2->SetDirectory( 0 );
    
    RooDataHist* dh2 = new RooDataHist("dh2", "dh2", x, Import(*h2)) ;
    RooHistPdf bg("bg", "bg", x, *dh2);

    // Construct composite pdf
    RooRealVar nsg("nsg", "nsg", 120000, 10000., 300000. ) ;
    RooRealVar nbg("nbg", "nbg", 4000,   0.,     40000.) ;
    RooAddPdf model("model", "model", RooArgList( sg, bg ), RooArgList( nsg, nbg ) ) ;

    // Pull
    int Nevt = 2000;

    // Method of Expected yield 
    vector<double> pull;
    const double ns = h1->Integral();
    const double nb = 9700.7866;

    // Method of mean value
    vector<double> pull_mean;
    vector<double> pull_sigm;
    double sum_mean = 0;

    for( int i=0; i<Nevt; i++ ){
        double nbevt = RooRandom::randomGenerator()->Poisson(nb) ;
        double nsevt = RooRandom::randomGenerator()->Poisson(ns) ;
        RooDataSet* bgdata = bg.generate( x, nbevt );
        RooDataSet* sgdata = sg.generate( x, nsevt );

        bgdata->append( *sgdata );
        model.fitTo( *bgdata, NumCPU(6) );
        
        // Method 1
        pull.push_back( ( nsg.getVal() - ns ) / nsg.getError() );

        // Method 2 
        pull_mean.push_back( nsg.getVal() );
        pull_sigm.push_back( nsg.getError() );
        sum_mean += nsg.getVal();
    }
    
    // Method 1
    GaussFit( pull, lepton + "_pull_Method1.pdf" );

     //Method 2
    vector<double> pull2;
    for( int i=0; i<Nevt; i++ ){
        pull2.push_back( ( pull_mean[i] - sum_mean / Nevt ) / pull_sigm[i] );
    }
    GaussFit( pull2, lepton + "_pull_Method2.pdf" );

}
