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
    cout << ">>Preparing Fullcut" << endl;
    StoreCompare();
    if( CompMgr().CheckOption( "dist" ) ){
        DrawbbSeparate();
    }
   
    //PlotSample();
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

        //Register puweight from the tree
        float PUWeight;
        CompMgr().RegisterWeight(ch, &PUWeight);

        // Looping events
        cout << ">>Processing " << sample << endl;
        int events = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

        for( int i = 0; i < events; i++ ){
            ch->GetEntry( i );
            CompMgr().process( events, i );

            vector<int> muidx;// store one isolated tight muon
            vector<int> jetidx;// store every jets
            vector<int> bjetidx;// store two bjets

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
            int had_b       = get<2>( tup );

            if( CompMgr().CheckOption( "chi2" ) ){
                if( chi2mass > CompMgr().GetOption<double>( "chi2" ) ){
                    continue;
                }
            }

            CompMgr().GetSample()->Hist( "tmass" )->Fill( seltmass );
            CompMgr().GetSample()->Hist( "chi2" )->Fill( chi2mass );

            /*******************************************************************************
            *  bbSeparation
            *******************************************************************************/
            
            if( CompMgr().CheckOption( "dist" ) ){
                if( !CompMgr().GetSample()->FillbHandle() ){
                    continue;
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
            }
        }

        cout << ">>Finishing " << sample << endl;

        delete ch;
    }
}

extern void
AddHist()
{
    CompMgr().GetSample()->AddHist( "tmass", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "chi2", "#chi^{2}_{min}", "Events", 40, 0, 200 );
    
    CompMgr().GetSample()->AddHist( "correct", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "fakeb", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "mistag", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "swap", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "other", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "mismatched", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
}

extern void
StoreCompare()
{
    Samplelst smplst = CompMgr().GetSamplelst();

    for(auto s : smplst){
        mgr::SaveToROOT( s->Hist("tmass"), CompMgr().GetResultsName( "root", s->GetTag() ), "tmass" );
    }
}
