#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

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
    CompMgr().GetSample()->RegisterWeight();
    CompMgr().GetSample()->InitBtagWeight("bcheck", CompMgr().GetSingleData<string>( "BtagWeight" ));

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

        lep_tmass = (CompMgr().GetSample()->GetJetP4(lep_b) + CompMgr().GetSample()->GetLepP4(lephandle) ).M();
            
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
        float  puweight  = CompMgr().CheckOption( "pileup" ) ? CompMgr().GetSample()->GetPUWeight() : 1; 
        float  genweight = is_data ? 1 : CompMgr().GetSample()->GenWeight(); 
        double idsf      = is_data ? 1 : GetSF( idsfhist,  lephandle );
        double isosf     = is_data ? 1 : GetSF( isosfhist, lephandle );
        double trgsf     = is_data ? 1 : GetSF( trgsfhist, lephandle );
        double btagSF    = is_data ?   
            1 : CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, had_b) * 
                CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, lep_b) ;

        if(genweight < 0){
            genweight = -1;
        }
        else{
            genweight = 1;
        }

        if( CompMgr().CheckOption("test") ){
            cout<<"pu       "<<puweight<<endl;
            cout<<"gen      "<<genweight<<endl;
            cout<<"btag1    "<<CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, had_b)<<endl;
            cout<<"btag2    "<<CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, lep_b)<<endl;
            cout<<"id       "<<idsf<<endl;
            cout<<"iso      "<<isosf<<endl;
            cout<<"trg      "<<trgsf<<endl;
        }

        double weight = puweight * genweight * btagSF * idsf * isosf * trgsf;

        float nVtx = CompMgr().GetSample()->nVtx();
        CompMgr().GetSample()->Hist( "had_tmass" ) ->Fill( had_tmass, weight );
        CompMgr().GetSample()->Hist( "lep_tmass" ) ->Fill( lep_tmass, weight );
        CompMgr().GetSample()->Hist( "chi2" )  ->Fill( chi2mass, weight );
        CompMgr().GetSample()->Hist( "nVtx" )  ->Fill( nVtx,     weight );

        if(!is_data){
            CompMgr().GetSample()->Hist2D( "chi2_tmass" ) ->Fill( had_tmass, chi2mass );
        }

        /*******************************************************************************
        *  Observable
        *******************************************************************************/
        float          charge  = CompMgr().GetSample()->GetIsoLepCharge(lephandle);
        TLorentzVector isolep  = CompMgr().GetSample()->GetLepP4(lephandle);
        TLorentzVector hardjet = CompMgr().GetSample()->GetJetP4(jethandle[0]);
        TLorentzVector b;
        TLorentzVector bbar;

        if(charge < 0){
            b    = CompMgr().GetSample()->GetJetP4(had_b);
            bbar = CompMgr().GetSample()->GetJetP4(lep_b);
        }
        else{
            b    = CompMgr().GetSample()->GetJetP4(lep_b);
            bbar = CompMgr().GetSample()->GetJetP4(had_b);
        }
        
        //In Lab frame
        double o4 = Obs4(isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge);
        
        //In bbar CM frame (b and b~ will back-to-back, i.e vector(b)=-vector(b~))
        TVector3 bbCM = -( b + bbar ).BoostVector();
        b.Boost(bbCM); 
        isolep.Boost(bbCM); 
        hardjet.Boost(bbCM);
        double o3 = Obs3(isolep.Vect(), hardjet.Vect(), b.Vect(), charge);

        CompMgr().GetSample()->Hist( "Obs3" )->Fill( o3/1000000., weight );
        CompMgr().GetSample()->Hist( "Obs4" )->Fill( o4/1000000., weight );
        


        /*******************************************************************************
        *  bbSeparation
        *******************************************************************************/
        if(!is_data){
            BaseLineMgr::MatchType flag = CompMgr().GetSample()->bbSeparation( had_b, lep_b, lephandle);

            if(flag == BaseLineMgr::Correct ){
                CompMgr().GetSample()->Hist( "tmass_Correct" )->Fill( had_tmass, weight );
                CompMgr().GetSample()->Hist( "chi2_Correct" )-> Fill( chi2mass, weight );
            }
            if(flag == BaseLineMgr::Misid ){
                CompMgr().GetSample()->Hist( "tmass_Misid" )->Fill( had_tmass, weight );
                CompMgr().GetSample()->Hist( "chi2_Misid" )-> Fill( chi2mass, weight );
            }
            if(flag == BaseLineMgr::Mistag ){
                CompMgr().GetSample()->Hist( "tmass_Mistag" )->Fill( had_tmass, weight );
                CompMgr().GetSample()->Hist( "chi2_Mistag" )-> Fill( chi2mass, weight );
            }
            if(flag == BaseLineMgr::Nomatched ){
                CompMgr().GetSample()->Hist( "tmass_Nomatched" )->Fill( had_tmass, weight );
                CompMgr().GetSample()->Hist( "chi2_Nomatched" )-> Fill( chi2mass, weight );
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

extern TH2*
GetSFHist(const string& tag)
{
    string filename  = mgr::GetSingle<string>( "file"  , CompMgr().GetSubTree( tag ) );
    string title     = mgr::GetSingle<string>( "title" , CompMgr().GetSubTree( tag ) );
    
    TFile* f = TFile::Open( filename.c_str() );
    TH2* h  = (TH2*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory(0);
    f->Close();
    
    return h;
}

extern double
GetSF(TH2* h, const int& idx)
{
    return CompMgr().GetSample()->GetSFTH2(h, idx);
}

extern void
AddHist()
{
    /*******************************************************************************
    *  Reco TH1
    *******************************************************************************/
    CompMgr().GetSample()->AddHist( "lep_tmass", "#mub [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "had_tmass", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "chi2",  "#chi^{2}_{min}", "Events", 40, 0, 200 );
    CompMgr().GetSample()->AddHist( "nVtx",  "Number of primary vertex", "Yield", 50, 0, 50 );

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    CompMgr().GetSample()->AddHist2D( "chi2_tmass", "M_{jjb} [GeV]", "#chi^{2}_{min}", 50, 0, 500, 40, 0, 200);

    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    CompMgr().GetSample()->AddHist( "Obs3", "O_{3}", "Events", 200, -10, 10 );
    CompMgr().GetSample()->AddHist( "Obs4", "O_{4}", "Events", 200, -10, 10 );

    /*******************************************************************************
    * bbSeparation
    *******************************************************************************/
    CompMgr().GetSample()->AddHist( "tmass_Correct" , "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "tmass_Misid"   , "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "tmass_Mistag"  , "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "tmass_Nomatched","M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "chi2_Correct" , "#chi^{2}_{min}", "PDF", 200, 0, 200 );
    CompMgr().GetSample()->AddHist( "chi2_Misid"   , "#chi^{2}_{min}", "PDF", 200, 0, 200 );
    CompMgr().GetSample()->AddHist( "chi2_Mistag"  , "#chi^{2}_{min}", "PDF", 200, 0, 200 );
    CompMgr().GetSample()->AddHist( "chi2_Nomatched","#chi^{2}_{min}", "PDF", 200, 0, 200 );
}

extern void 
WeightMC(const string& sample)
{
    double lumi    = CompMgr().GetSingleData<double>( "lumi" );
    double xs      = mgr::GetSingle<double>( "cross_section", CompMgr().GetSubTree( sample ) );
    double gen_num = mgr::GetSingle<double>( "gen_num", CompMgr().GetSubTree( sample ) );

    CompMgr().GetSample()->Scale( (lumi*xs) / gen_num );
}

extern void
StoreCompare()
{
    auto s =  CompMgr().GetSample();
    string filename = CompMgr().GetResultsName( "root", "Hist" );
    
    /*******************************************************************************
    *  Reco TH1
    *******************************************************************************/
    mgr::SaveToROOT( s->Hist("lep_tmass"), filename, "lep_tmass" );
    mgr::SaveToROOT( s->Hist("had_tmass"), filename, "had_tmass" );
    mgr::SaveToROOT( s->Hist("chi2" ), filename, "chi2" );
    mgr::SaveToROOT( s->Hist("nVtx" ), filename, "nVtx" );

    /*******************************************************************************
    *  Reco TH2
    *******************************************************************************/
    mgr::SaveToROOT( s->Hist2D("chi2_tmass" ), filename, "chi2_tmass" );
    
    /*******************************************************************************
    *  Observable
    *******************************************************************************/
    mgr::SaveToROOT( s->Hist("Obs3"), filename, "Obs3");
    mgr::SaveToROOT( s->Hist("Obs4"), filename, "Obs4");

    /*******************************************************************************
    * bbSeparation
    *******************************************************************************/
    mgr::SaveToROOT( s->Hist("tmass_Correct" ), filename, "tmass_Correct" );
    mgr::SaveToROOT( s->Hist("tmass_Misid"   ), filename, "tmass_Misid" );
    mgr::SaveToROOT( s->Hist("tmass_Mistag"  ), filename, "tmass_Mistag" );
    mgr::SaveToROOT( s->Hist("tmass_Nomatched"), filename,"tmass_Nomatched" );
    mgr::SaveToROOT( s->Hist("chi2_Correct" ), filename, "chi2_Correct" );
    mgr::SaveToROOT( s->Hist("chi2_Misid"   ), filename, "chi2_Misid" );
    mgr::SaveToROOT( s->Hist("chi2_Mistag"  ), filename, "chi2_Mistag" );
    mgr::SaveToROOT( s->Hist("chi2_Nomatched"), filename,"chi2_Nomatched" );
}
