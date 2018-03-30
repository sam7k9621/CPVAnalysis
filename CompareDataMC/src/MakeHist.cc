#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Histor&
CompMgr( const string& subdir, const string& json )
{
    static Histor mgr( subdir, json );
    return mgr;
}

extern vector<TH2D*>
GetLepSFHist( const string& lepton )
{
    vector<string> weightlst = CompMgr().GetListData<string>( lepton + "weight" );
    vector<TH2D*>  lst;
    for( const auto& w : weightlst ){
        lst.push_back( CompMgr().GetSFHist( w ) );
    }

    return lst;
}

extern double
GetLepSF(const vector<TH2D*>& weightlst, const int& lephandle)
{
    double weight = 1;
    for(const auto& w : weightlst){
        weight *= CompMgr().GetSF( w, lephandle );
    }

    return weight;
}

extern void
MakeHist()
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
    
    vector<TH2D*> lepweightlst = GetLepSFHist( lepton );
    CompMgr().RegisterWeight();
    CompMgr().InitBtagWeight( "bcheck", CompMgr().GetSingleData<string>( "BtagWeight" ) );

    // Register reco sample
    Int_t jethandle[ 256 ];
    Int_t jet_size;
    Int_t had_b;
    Int_t lep_b;
    Int_t lephandle;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;

    ch->SetBranchAddress( "jethandle", jethandle );
    ch->SetBranchAddress( "jet_size",  &jet_size );
    ch->SetBranchAddress( "had_b",     &had_b );
    ch->SetBranchAddress( "lep_b",     &lep_b );
    ch->SetBranchAddress( "lephandle", &lephandle );
    ch->SetBranchAddress( "chi2mass",  &chi2mass );
    ch->SetBranchAddress( "seltmass",  &had_tmass );

    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 1000 : CompMgr().GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;

    for( int i = 0; i < events; i++ ){
        CompMgr().GetEntry( i );
        CompMgr().process( events, i );

        lep_tmass = ( CompMgr().GetJetP4( lep_b ) + CompMgr().GetLepP4( lephandle ) ).M();

        /*******************************************************************************
        * Chi2 minimum upper limit
        *******************************************************************************/
        if( CompMgr().CheckOption( "chi2" ) ){
            if( chi2mass > CompMgr().GetOption<double>( "chi2" ) ){
                continue;
            }
        }

        /*******************************************************************************
        *  Event weighting & filling
        *******************************************************************************/
        float puweight  = CompMgr().CheckOption( "pileup" ) ? CompMgr().GetPUWeight() : 1;
        float genweight = is_data ? 1 : CompMgr().GenWeight();
        double lepsf    = is_data ? 1 : GetLepSF( lepweightlst, lephandle );
        double btagSF   = is_data && !CompMgr().CheckOption( "region" ) ?
                          1 : CompMgr().BtagScaleFactor( BTagEntry::OP_MEDIUM, had_b ) *
                          CompMgr().BtagScaleFactor( BTagEntry::OP_MEDIUM, lep_b );

        if( genweight < 0 ){
            genweight = -1;
        }
        else{
            genweight = 1;
        }

        double weight = puweight * genweight * btagSF * lepsf;

        float nVtx = CompMgr().nVtx();
                          CompMgr().Hist( "had_tmass" )->Fill( had_tmass, weight );
                          CompMgr().Hist( "lep_tmass" )->Fill( lep_tmass, weight );
                          CompMgr().Hist( "chi2" )->Fill( chi2mass, weight );
                          CompMgr().Hist( "nVtx" )->Fill( nVtx, weight );

        if( !is_data ){
            CompMgr().Hist2D( "chi2_tmass" )->Fill( had_tmass, chi2mass );
        }

        /*******************************************************************************
        *  Observable
        *******************************************************************************/
        float charge           = CompMgr().GetIsoLepCharge( lephandle );
        TLorentzVector isolep  = CompMgr().GetLepP4( lephandle );
        TLorentzVector hardjet = CompMgr().GetJetP4( jethandle[ 0 ] );
        TLorentzVector b;
        TLorentzVector bbar;

        if( charge < 0 ){
            b    = CompMgr().GetJetP4( had_b );
            bbar = CompMgr().GetJetP4( lep_b );
        }
        else{
            b    = CompMgr().GetJetP4( lep_b );
            bbar = CompMgr().GetJetP4( had_b );
        }

        // In Lab frame
        double o4 = Obs4( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );

        // In bbar CM frame (b and b~ will back-to-back, i.e vector(b)=-vector(b~))
        TVector3 bbCM = -( b + bbar ).BoostVector();
        b.Boost( bbCM );
        isolep.Boost( bbCM );
        hardjet.Boost( bbCM );
        double o3 = Obs3( isolep.Vect(), hardjet.Vect(), b.Vect(), charge );

        CompMgr().Hist( "Obs3" )->Fill( o3 / 1000000., weight );
        CompMgr().Hist( "Obs4" )->Fill( o4 / 1000000., weight );

        /*******************************************************************************
        *  bbSeparation
        *******************************************************************************/
        if( !is_data ){
            BaseLineMgr::MatchType flag = CompMgr().bbSeparation( had_b, lep_b, lephandle );

            if( flag == BaseLineMgr::Correct ){
                CompMgr().Hist( "tmass_Correct" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Correct" )->Fill( chi2mass, weight );
            }
            if( flag == BaseLineMgr::Misid ){
                CompMgr().Hist( "tmass_Misid" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Misid" )->Fill( chi2mass, weight );
            }
            if( flag == BaseLineMgr::Mistag ){
                CompMgr().Hist( "tmass_Mistag" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Mistag" )->Fill( chi2mass, weight );
            }
            if( flag == BaseLineMgr::Nomatched ){
                CompMgr().Hist( "tmass_Nomatched" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Nomatched" )->Fill( chi2mass, weight );
            }
        }
    }

    if( !is_data ){
        CompMgr().WeightMC( sample );
        cout << ">>Weighting " << sample << endl;
    }
    StoreCompare();

    delete ch;
}
