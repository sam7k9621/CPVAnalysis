#include "CPVAnalysis/MassReco/interface/MassReco.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TCanvas.h"
#include "TChain.h"
#include <algorithm>

using namespace std;
using namespace sel;
using namespace mgr;

extern void
Chi2Dist()
{
    bool is_data             = SelMgr().GetOption<string>( "source" ) == "data" ? 1 : 0;
    string source            = is_data ? "data" : "mc";
    vector<string> sourcelst = GetList<string>( "path", SelMgr().GetSubTree( source ) );
    TH1F* tmass              = new TH1F( "tmass", "tmass", 50, 0, 500 );
    TH1F* case1              = new TH1F( "case1", "correct", 50, 0, 500 );
    TH1F* case2              = new TH1F( "case2", "fake b", 50, 0, 500 );
    TH1F* case4              = new TH1F( "case4", "swap", 50, 0, 500 );
    TH1F* chi2_correct       = new TH1F( "chi2_correct", "correct", 200, 0, 200 );
    TH1F* chi2_swap          = new TH1F( "chi2_swap", "swapped", 200, 0, 200 );
    TH1F* chi2_fakeb         = new TH1F( "chi2_fakeb", "fake b", 200, 0, 200 );
    TH1F* chi2_total         = new TH1F( "chi2_total", "total", 200, 0, 200 );
    // Adding files
    TChain* ch = new TChain( SelMgr().GetSingleData<string>( "tree" ).c_str() );

    for( auto& i : sourcelst ){
        ch->Add( i.c_str() );
    }

    SelMgr().SetRoot( ch );
    // Looping events
    int events = SelMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

    for( int upper = 1; upper <= 200; upper++ ){
        for( int i = 0; i < events; i++ ){
            ch->GetEntry( i );
            process( events, i );

            if( !fillBhandle() ){
                continue;
            }

            vector<int> muidx;// store one isolated tight muon
            vector<int> jetidx;// store every jets
            vector<int> bjetidx;// store two bjets

            /*******************************************************************************
            *  Baseline selection
            *******************************************************************************/

            if( !passFullMuon( muidx ) ){
                continue;
            }

            if( !passFullJet( jetidx, bjetidx, muidx[ 0 ] ) ){
                continue;
            }

            /*******************************************************************************
            *  High-level cut
            *******************************************************************************/
            double chi2mass;
            double seltmass;
            int had_b;
            tie( chi2mass, seltmass, had_b ) = getChi2Info( muidx[ 0 ], jetidx, bjetidx );

            if( !passChi2Upper( chi2mass, upper ) ){
                continue;
            }

            tmass->Fill( seltmass );
            int lep_b = had_b ? 0 : 1;
            int flag  = bbarSeparation( bjetidx[ had_b ], bjetidx[ lep_b ], muidx[ 0 ] );

            if( flag == Correct ){
                case1->Fill( seltmass );
            }
            else if( flag == Fakeb ){
                case2->Fill( seltmass );
            }
            else if( flag == Swap ){
                case4->Fill( seltmass );
            }
        }

        int total = tmass->Integral();
        chi2_correct->SetBinContent( upper, (double)case1->Integral() / total );
        chi2_swap->SetBinContent( upper, (double)case4->Integral() / total );
        chi2_fakeb->SetBinContent( upper, (double)case2->Integral() / total );
        chi2_total->SetBinContent( upper, (double)total / 312431.0 );
    }

    chi2_correct->SetLineColor( kGreen - 6 );
    chi2_swap->SetLineColor( kAzure - 3 );
    chi2_fakeb->SetLineColor( kOrange + 1 );
    chi2_total->SetLineColor( kRed - 7 );
    chi2_correct->SetLineWidth( 2 );
    chi2_swap->SetLineWidth( 2 );
    chi2_fakeb->SetLineWidth( 2 );
    chi2_total->SetLineWidth( 2 );
    setHist( chi2_total, "#chi^{2}_{min} upper cut", "Rate" );
    TCanvas* c = mgr::NewCanvas();
    chi2_total->Draw();
    chi2_swap->Draw( "same" );
    chi2_fakeb->Draw( "same" );
    chi2_correct->Draw( "same" );
    mgr::SetSinglePad( c );
    mgr::SetAxis( chi2_total );
    chi2_total->SetMaximum( 1.1 );
    mgr::DrawCMSLabelOuter( PRELIMINARY );
    TLine* line = new TLine( 0, 1, 200, 1 );
    line->SetLineColor( kRed );
    line->SetLineStyle( 8 );
    line->Draw();
    TLegend* leg = mgr::NewLegend( 0.65, 0.65, 0.75, 0.8 );
    leg->SetLineColor( kWhite );
    leg->AddEntry( "chi2_total",   "Event efficiency", "l" );
    leg->AddEntry( "chi2_swap",    "Swap",             "l" );
    leg->AddEntry( "chi2_fakeb",   "Fake b",           "l" );
    leg->AddEntry( "chi2_correct", "Correct",          "l" );
    leg->Draw();
    mgr::SaveToPDF( c, GetResultsName( "pdf", "UpperCut" ) );
    delete c;
    delete leg;
    delete chi2_correct;
    delete chi2_fakeb;
    delete chi2_swap;
    delete chi2_total;
}
