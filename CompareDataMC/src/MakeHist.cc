#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Histor&
CompMgr( const string& subdir, const string& json )
{
    static Histor mgr(subdir, json);
    return mgr;
}

extern void
MakeHist()
{
    //Initialize file
    string sample = CompMgr().GetOption<string>( "sample" );
    TChain* ch = new TChain( "root" );
    ch->Add( (CompMgr().GetResultsName( "root", "FullCut") ).c_str() );
    CompMgr().AddSample( sample, ch );
    AddHist();
   
    //Initialize weighting factor
    TH2* isosfhist = GetSFHist("isosf") ;
    TH2* idsfhist  = GetSFHist("idsf") ;
    TH2* trgsfhist = GetSFHist("trgsf") ;
    CompMgr().RegisterWeight();
    CompMgr().InitBtagWeight("bcheck", CompMgr().GetSingleData<string>( "BtagWeight" ));

    //Register reco sample
    Int_t jethandle[256];
    Int_t jet_size;
    Int_t had_b;
    Int_t lep_b;
    Int_t lephandle;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;

    ch->SetBranchAddress("jethandle",  jethandle);
    ch->SetBranchAddress("jet_size",  &jet_size);
    ch->SetBranchAddress("had_b",     &had_b);
    ch->SetBranchAddress("lep_b",     &lep_b);
    ch->SetBranchAddress("lephandle", &lephandle);
    ch->SetBranchAddress("chi2mass",  &chi2mass);
    ch->SetBranchAddress("seltmass",  &had_tmass);
    //ch->SetBranchAddress("lep_tmass",  &lep_tmass);

    // Looping events
    int events = CompMgr().CheckOption( "test" ) ? 1000 : CompMgr().GetEntries();
    bool is_data = (sample == "Data") ? 1 : 0;
    for( int i = 0; i < events; i++ ){
        CompMgr().GetEntry( i );
        CompMgr().process( events, i );

        lep_tmass = (CompMgr().GetJetP4(lep_b) + CompMgr().GetLepP4(lephandle) ).M();
            
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
        float  puweight  = CompMgr().CheckOption( "pileup" ) ? CompMgr().GetPUWeight() : 1; 
        float  genweight = is_data ? 1 : CompMgr().GenWeight(); 
        double idsf      = is_data ? 1 : GetSF( idsfhist,  lephandle );
        double isosf     = is_data ? 1 : GetSF( isosfhist, lephandle );
        double trgsf     = is_data ? 1 : GetSF( trgsfhist, lephandle );
        double btagSF    = is_data ?   
            1 : CompMgr().BtagScaleFactor(BTagEntry::OP_MEDIUM, had_b) * 
                CompMgr().BtagScaleFactor(BTagEntry::OP_MEDIUM, lep_b) ;

        if(genweight < 0){
            genweight = -1;
        }
        else{
            genweight = 1;
        }

        double weight = puweight * genweight * btagSF * idsf * isosf * trgsf;

        float nVtx = CompMgr().nVtx();
        CompMgr().Hist( "had_tmass" ) ->Fill( had_tmass, weight );
        CompMgr().Hist( "lep_tmass" ) ->Fill( lep_tmass, weight );
        CompMgr().Hist( "chi2" )  ->Fill( chi2mass, weight );
        CompMgr().Hist( "nVtx" )  ->Fill( nVtx,     weight );

        if(!is_data){
            CompMgr().Hist2D( "chi2_tmass" ) ->Fill( had_tmass, chi2mass );
        }

        /*******************************************************************************
        *  Observable
        *******************************************************************************/
        float          charge  = CompMgr().GetIsoLepCharge(lephandle);
        TLorentzVector isolep  = CompMgr().GetLepP4(lephandle);
        TLorentzVector hardjet = CompMgr().GetJetP4(jethandle[0]);
        TLorentzVector b;
        TLorentzVector bbar;

        if(charge < 0){
            b    = CompMgr().GetJetP4(had_b);
            bbar = CompMgr().GetJetP4(lep_b);
        }
        else{
            b    = CompMgr().GetJetP4(lep_b);
            bbar = CompMgr().GetJetP4(had_b);
        }
        
        //In Lab frame
        double o4 = Obs4(isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge);
        
        //In bbar CM frame (b and b~ will back-to-back, i.e vector(b)=-vector(b~))
        TVector3 bbCM = -( b + bbar ).BoostVector();
        b.Boost(bbCM); 
        isolep.Boost(bbCM); 
        hardjet.Boost(bbCM);
        double o3 = Obs3(isolep.Vect(), hardjet.Vect(), b.Vect(), charge);

        CompMgr().Hist( "Obs3" )->Fill( o3/1000000., weight );
        CompMgr().Hist( "Obs4" )->Fill( o4/1000000., weight );
        


        /*******************************************************************************
        *  bbSeparation
        *******************************************************************************/
        if(!is_data){
            BaseLineMgr::MatchType flag = CompMgr().bbSeparation( had_b, lep_b, lephandle);

            if(flag == BaseLineMgr::Correct ){
                CompMgr().Hist( "tmass_Correct" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Correct" )-> Fill( chi2mass, weight );
            }
            if(flag == BaseLineMgr::Misid ){
                CompMgr().Hist( "tmass_Misid" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Misid" )-> Fill( chi2mass, weight );
            }
            if(flag == BaseLineMgr::Mistag ){
                CompMgr().Hist( "tmass_Mistag" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Mistag" )-> Fill( chi2mass, weight );
            }
            if(flag == BaseLineMgr::Nomatched ){
                CompMgr().Hist( "tmass_Nomatched" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Nomatched" )-> Fill( chi2mass, weight );
            }
        }
    }
    
    if( !is_data ){
        WeightMC(sample);
        cout << ">>Weighting " << sample << endl;
    }
    CompMgr().AddCutName (  {"chi2"} );
    StoreCompare();
}

