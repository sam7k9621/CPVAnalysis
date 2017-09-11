#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Selector&
CompMgr( const string& subdir )
{
    static Selector mgr( subdir );
    return mgr;
}

extern void
MakeFullCut()
{
    FullCut();
    cout << ">>Preparing Drawing" << endl;
    //DrawCompare();
    DrawbbSeparate();
}

extern void
FullCut()
{
    vector<string> samplelst = CompMgr().GetListData<string>( "samplelst" );

    for( const auto& sample : samplelst ){
        vector<string> sourcelst = mgr::GetList<string>( "path", CompMgr().GetSubTree( sample ) );

        cout << ">>Adding " << sample << endl;
        TChain* ch = new TChain( "root" );

        for( const auto& i : sourcelst ){
            ch->Add( i.c_str() );
        }

        CompMgr().AddSample( sample, ch );
        AddHist();

        // Looping events
        cout << ">>Processing " << sample << endl;
        int events = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

        for( int i = 0; i < events; i++ ){
            ch->GetEntry( i );
            CompMgr().process( events, i );

            vector<int> muidx;// store one isolated tight muon
            vector<int> jetidx;// store every jets
            vector<int> bjetidx;// store two bjets

            if( !CompMgr().GetSample()->FillbHandle() )
                continue;

            /*******************************************************************************
            *  Baseline selection
            *******************************************************************************/

            if( !CompMgr().GetSample()->passFullMuon( muidx ) ){
                continue;
            }

            if( !CompMgr().GetSample()->passFullJet( jetidx, bjetidx, muidx[ 0 ] ) ){
                continue;
            }

            /*******************************************************************************
            *  High-level cut
            *******************************************************************************/

            auto tup = CompMgr().GetSample()->GetChi2Info( jetidx, bjetidx );

            double chi2mass = get<0>( tup );
            double seltmass = get<1>( tup );
            int    had_b    = get<2>( tup );

            if( CompMgr().CheckOption( "chi2" ) ){
                if( chi2mass > CompMgr().GetOption<double>( "chi2" ) ){
                    continue;
                }
            }

            int lep_b = had_b ? 0 : 1;
            int flag  = CompMgr().GetSample()->bbSeparation( bjetidx[ had_b ], bjetidx[ lep_b ], muidx[ 0 ] );

            if( flag == BaseLineMgr::Correct ){
                CompMgr().GetSample()->Hist( "correct" )->Fill( seltmass );
            }
            else if( flag == BaseLineMgr::Fakeb ){
                CompMgr().GetSample()->Hist( "fakeb" )->Fill( seltmass );
            }
            else if( flag == BaseLineMgr::Mistag ){
                CompMgr().GetSample()->Hist( "mistag" )->Fill( seltmass );
            }
            else if( flag == BaseLineMgr::Swap ){
                CompMgr().GetSample()->Hist( "swap" )->Fill( seltmass );
            }
            else if( flag == BaseLineMgr::Other ){
                CompMgr().GetSample()->Hist( "other" )->Fill( seltmass );
            }
            else{
                CompMgr().GetSample()->Hist( "mismatched" )->Fill( seltmass );
            }            

            CompMgr().GetSample()->Hist( "tmass" )->Fill( seltmass );
            CompMgr().GetSample()->Hist( "chi2" )->Fill( chi2mass );
        }

        cout << ">>Finishing " << sample << endl;

        delete ch;
    }
}

extern void
AddHist()
{
    CompMgr().GetSample()->AddHist( "tmass",     "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "chi2",      "#chi^{2}_{min}","Events", 40, 0, 200 );
    CompMgr().GetSample()->AddHist( "correct",   "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "fakeb",     "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "mistag",    "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "swap",      "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "other",     "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "mismatched","M_{jjb} [GeV]", "Events", 50, 0, 500 );
}

extern void
DrawbbSeparate()
{

    Color_t x[] = { kGray + 1, kMagenta + 2, kRed - 7, kOrange + 1, kAzure - 3, kGreen - 6};
    vector<TH1*> histlst;

    histlst.push_back(CompMgr().GetSample()->Hist("mismatched"));
    histlst.push_back(CompMgr().GetSample()->Hist("other"));
    histlst.push_back(CompMgr().GetSample()->Hist("mistag"));
    histlst.push_back(CompMgr().GetSample()->Hist("fakeb"));
    histlst.push_back(CompMgr().GetSample()->Hist("swap"));
    histlst.push_back(CompMgr().GetSample()->Hist("correct"));

    for(int i=0; i<(int)histlst.size(); i++){
        histlst[i]->SetLineColor(x[i]);
        histlst[i]->SetFillColor(x[i]);
    }

    TCanvas* c  = mgr::NewCanvas();
    THStack* hs = new THStack();

    TLegend* leg = mgr::NewLegend( 0.65, 0.55, 0.75, 0.75 );
    leg->SetLineColor( kWhite );
    for( auto& h : histlst ){
        hs->Add( h );
        leg->AddEntry( h, CompMgr().Discript(h).c_str(), "l" );
    }

    TH1D* total = CompMgr().GetSample()->Hist( "tmass" );

    TLine* line = new TLine( 172.5, 0, 172.5, mgr::GetYmax( total ) * 1.6 );
    line->SetLineColor( kRed );
    line->SetLineStyle( 8 );

    TPaveText* pt = mgr::NewTextBox( .73, .81, .93, .87 );
    pt->AddText( "Muon Channel" );

    total->Draw();
    hs->Draw("same");
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
    cout << "Correct   " << CompMgr().GetSample()->Hist("correct")->Integral()     << " " << (double)CompMgr().GetSample()->Hist("correct")->Integral()   / total->Integral() << endl;
    cout << "Fake b    " << CompMgr().GetSample()->Hist("fakeb")->Integral()       << " " << (double)CompMgr().GetSample()->Hist("fakeb")->Integral()     / total->Integral() << endl;
    cout << "Mistag    " << CompMgr().GetSample()->Hist("mistag")->Integral()      << " " << (double)CompMgr().GetSample()->Hist("mistag")->Integral()    / total->Integral() << endl;
    cout << "Swapped   " << CompMgr().GetSample()->Hist("swap")->Integral()        << " " << (double)CompMgr().GetSample()->Hist("swap")->Integral()      / total->Integral() << endl;
    cout << "Other     " << CompMgr().GetSample()->Hist("other")->Integral()       << " " << (double)CompMgr().GetSample()->Hist("other")->Integral()     / total->Integral() << endl;
    cout << "Mismatched " << CompMgr().GetSample()->Hist("mismatched")->Integral() << " "<< (double)CompMgr().GetSample()->Hist("mismatched")->Integral() / total->Integral() << endl;
    delete pt;
    delete leg;
    delete line;
    delete hs;
    delete c;
}

extern void
DrawCompare()
{
    Color_t x[] = { kGreen - 6, kAzure - 3, kOrange + 1, kRed - 7, kMagenta + 2, kGray + 1 };
    vector<TH1*> histlst;

    for( int i = 0; i < (int)CompMgr().GetSamplelst().size(); i++ ){
        CompMgr().GetSamplelst()[ i ]->SetColor( x[ i ] );
        histlst.push_back( CompMgr().GetSamplelst()[ i ]->Hist( "tmass" ) );
    }

    TCanvas* c  = mgr::NewCanvas();
    THStack* hs = new THStack();

    for( auto& h : histlst ){
        hs->Add( h );
    }

    hs->Draw();

    mgr::SetSinglePad( c );
    mgr::SetAxis( hs );
    hs->SetMaximum( mgr::GetYmax( histlst ) * 1.6 );
    hs->GetXaxis()->SetTitle( "M_{jjb} [GeV]" );
    hs->GetYaxis()->SetTitle( "Events" );

    mgr::DrawCMSLabelOuter( PRELIMINARY );
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "tmass" ) );
    cout << endl << "peak  " << mgr::GetYmax( histlst ) << endl;
    cout << endl << "total " << histlst[ 0 ]->Integral() << endl;
    delete hs;
    delete c;
}
