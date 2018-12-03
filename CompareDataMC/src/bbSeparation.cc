#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include <cstdlib>
#include <ctime>

using namespace std;

extern void
bbSeparation()
{
    // Initialize file
    string lepton            = CompMgr().GetOption<string>("lepton");
    string sample            = CompMgr().GetOption<string>( "sample" );
    vector<string> samplelst = CompMgr().GetSubListData<string>( sample, lepton + "path" );
    TChain* ch               = new TChain( "root" );

    for( const auto& s : samplelst ){
        ch->Add( s.c_str() );
    }

    CompMgr().AddSample( sample, ch );
    AddHist();
    
    // Register reco sample
    Int_t jet1;
    Int_t jet2;
    Int_t had_b;
    Int_t lep_b;
    Int_t lep;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;

    ch->SetBranchAddress( "jet1", &jet1 );
    ch->SetBranchAddress( "jet2", &jet2 );
    ch->SetBranchAddress( "had_b", &had_b );
    ch->SetBranchAddress( "lep_b", &lep_b );
    ch->SetBranchAddress( "lep", &lep );
    ch->SetBranchAddress( "chi2mass", &chi2mass );
    ch->SetBranchAddress( "had_tmass", &had_tmass );
    ch->SetBranchAddress( "lep_tmass", &lep_tmass );

    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 10000 : CompMgr().GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;

    for( int i = 0; i < events; i++ ){
        CompMgr().GetEntry( i );
        CompMgr().process( events, i );


        /*******************************************************************************
        * Chi2 minimum upper limit
        *******************************************************************************/
        if( CompMgr().CheckOption( "chi2" ) ){
            if( chi2mass > CompMgr().GetOption<double>( "chi2" ) ){
                continue;
            }
        }
        /*******************************************************************************
        *  bbSeparation / Optimisation
        *******************************************************************************/
        if( !is_data ){
            BaseLineMgr::MatchType flag = CompMgr().bbSeparation( had_b, lep_b, lep );

            if( flag == BaseLineMgr::Correct ){
                CompMgr().Hist( "tmass_Correct" )->Fill( had_tmass );
                CompMgr().Hist( "chi2_Correct" )->Fill( chi2mass );
                CompMgr().Hist( "Cor_leptmass" )->Fill( lep_tmass );
            }
            else if( flag == BaseLineMgr::Misid ){
                CompMgr().Hist( "tmass_Misid" )->Fill( had_tmass );
                CompMgr().Hist( "chi2_Misid" )->Fill( chi2mass );
                CompMgr().Hist( "Misid_leptmass" )->Fill( lep_tmass );
            }
            else if( flag == BaseLineMgr::Mistag ){
                CompMgr().Hist( "tmass_Mistag" )->Fill( had_tmass );
                CompMgr().Hist( "chi2_Mistag" )->Fill( chi2mass );
                CompMgr().Hist( "Mistag_leptmass" )->Fill( lep_tmass );
            }
            else{
                continue;
            }

            CompMgr().Hist2D( "chi2_tmass" )->Fill( had_tmass, chi2mass );
        } 
    }
    
    StoreCompare();
    delete ch;

}
