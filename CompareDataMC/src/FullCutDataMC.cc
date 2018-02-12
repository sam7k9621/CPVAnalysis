#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Selector&
CompMgr( const string& subdir, const string& json )
{
    static Selector mgr( subdir, json );
    return mgr;
}

extern void
MakeFullCut()
{
    //Process sample
    string sample = CompMgr().GetOption<string>( "sample" ); 
    vector<string> sourcelst = mgr::GetList<string>( "path", CompMgr().GetSubTree( sample ) );
    TChain* ch = new TChain( "root" );
    for( const auto& i : sourcelst ){
        ch->Add( i.c_str() );
    }
    CompMgr().AddSample( CompMgr().GetOption<string>( "sample" ), ch );
    AddHist();
   
    //Register puweight from the tree
    CompMgr().GetSample()->RegisterWeight();
    CompMgr().GetSample()->InitBtagWeight("bcheck", CompMgr().GetSingleData<string>( "BtagWeight" ));

    cout << ">>Processing " << sample << endl;
    FullCut();
    cout << ">>Finishing " << sample << endl;
    
    if( sample != "Data"){
        WeightMC(sample);
        cout << ">>Weighting " << sample << endl;
    }
    
    StoreCompare();
    cout << ">>Storing " << sample << endl;
}

extern void
FullCut()
{
    // Looping events
    int events = CompMgr().CheckOption( "test" ) ? 10000 : CompMgr().GetEntries();
    bool is_data = CompMgr().GetOption<string>( "sample" ) == "Data" ? 1 : 0;

    TH2* isosfhist = GetSFHist("isosf") ;
    TH2* idsfhist  = GetSFHist("idsf") ;
    TH2* trgsfhist = GetSFHist("trgsf") ;

    for( int i = 0; i < events; i++ ){
        CompMgr().GetEntry( i );
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

        /*******************************************************************************
        *  Event weighting & filling
        *******************************************************************************/
        float  puweight  = CompMgr().CheckOption( "pileup" ) ? CompMgr().GetSample()->GetPUWeight() : 1; 
        float  genweight = is_data ? 1 : CompMgr().GetSample()->GenWeight(); 
        double idsf      = is_data ? 1 : GetSF( idsfhist,  muidx[0] );
        double isosf     = is_data ? 1 : GetSF( isosfhist, muidx[0] );
        double trgsf     = is_data ? 1 : GetSF( trgsfhist, muidx[0] );
        double btagSF    = is_data ?   
            1 : CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, bjetidx[0]) * 
                CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, bjetidx[1]) ;

        if(genweight < 0){
            genweight = -1;
        }
        else{
            genweight = 1;
        }


        if( CompMgr().CheckOption("test") ){
            cout<<"pu       "<<puweight<<endl;
            cout<<"gen      "<<genweight<<endl;
            cout<<"btag1    "<<CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, bjetidx[0])<<endl;
            cout<<"btag2    "<<CompMgr().GetSample()->BtagScaleFactor(BTagEntry::OP_MEDIUM, bjetidx[1])<<endl;
            cout<<"id       "<<idsf<<endl;
            cout<<"iso      "<<isosf<<endl;
            cout<<"trg      "<<trgsf<<endl;
        }

        double weight = puweight * genweight * btagSF * idsf * isosf * trgsf;

        float nVtx = CompMgr().GetSample()->nVtx();
        CompMgr().GetSample()->Hist( "tmass" ) ->Fill( seltmass, weight );
        CompMgr().GetSample()->Hist( "chi2" )  ->Fill( chi2mass, weight );
        CompMgr().GetSample()->Hist( "nVtx" )  ->Fill( nVtx,     weight );
        
        /*******************************************************************************
        *  bbSeparation
        *******************************************************************************/
        if( !CompMgr().GetSample()->FillbHandle() ){
            continue;
        }

        int lep_b = had_b ? 0 : 1;
        int flag  = CompMgr().GetSample()->bbSeparation( bjetidx[ had_b ], bjetidx[ lep_b ], muidx[ 0 ] );

        if( flag == BaseLineMgr::Correct ){
            CompMgr().GetSample()->Hist( "correct" )->Fill( seltmass, weight );
        }
        else if( flag == BaseLineMgr::Fakeb ){
            CompMgr().GetSample()->Hist( "fakeb" )->Fill( seltmass, weight );
        }
        else if( flag == BaseLineMgr::Mistag ){
            CompMgr().GetSample()->Hist( "mistag" )->Fill( seltmass, weight );
        }
        else if( flag == BaseLineMgr::Swap ){
            CompMgr().GetSample()->Hist( "swap" )->Fill( seltmass, weight );
        }
        else if( flag == BaseLineMgr::Other ){
            CompMgr().GetSample()->Hist( "other" )->Fill( seltmass, weight );
        }
        else{
            CompMgr().GetSample()->Hist( "mismatched" )->Fill( seltmass, weight );
        }
    }
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
    CompMgr().GetSample()->AddHist( "tmass", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "chi2",  "#chi^{2}_{min}", "Events", 40, 0, 200 );
    CompMgr().GetSample()->AddHist( "nVtx",  "Number of primary vertex", "Yield", 80, 0, 80 );

    CompMgr().GetSample()->AddHist( "correct",    "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "fakeb",      "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "mistag",     "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "swap",       "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "other",      "M_{jjb} [GeV]", "Events", 50, 0, 500 );
    CompMgr().GetSample()->AddHist( "mismatched", "M_{jjb} [GeV]", "Events", 50, 0, 500 );
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

    mgr::SaveToROOT( s->Hist("tmass"), CompMgr().GetResultsName( "root", "FullCut" ), "tmass" );
    mgr::SaveToROOT( s->Hist("chi2" ), CompMgr().GetResultsName( "root", "FullCut" ), "chi2" );
    mgr::SaveToROOT( s->Hist("nVtx" ), CompMgr().GetResultsName( "root", "FullCut" ), "nVtx" );

    mgr::SaveToROOT( s->Hist("correct"   ), CompMgr().GetResultsName( "root", "FullCut" ), "correct" );
    mgr::SaveToROOT( s->Hist("fakeb"     ), CompMgr().GetResultsName( "root", "FullCut" ), "fakeb" );
    mgr::SaveToROOT( s->Hist("mistag"    ), CompMgr().GetResultsName( "root", "FullCut" ), "mistag" );
    mgr::SaveToROOT( s->Hist("swap"      ), CompMgr().GetResultsName( "root", "FullCut" ), "swap" );
    mgr::SaveToROOT( s->Hist("other"     ), CompMgr().GetResultsName( "root", "FullCut" ), "other" );
    mgr::SaveToROOT( s->Hist("mismatched"), CompMgr().GetResultsName( "root", "FullCut" ), "mismatched" );
}
