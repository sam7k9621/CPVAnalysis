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
    CompMgr().AddSample( sample, ch );
    
    FullCut();
}

extern void
FullCut()
{
    //Build new file
    TFile* newfile = TFile::Open( (CompMgr().GetResultsName( "root", "FullCut") ).c_str(), "recreate" );
    TTree* newtree = CompMgr().CloneTree(); 
    
    //Register new branch
    Int_t jet_size;
    Int_t jethandle[256];
    Int_t had_b;
    Int_t lep_b;
    Int_t lephandle;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;
   
    newtree->Branch("jet_size",  &jet_size,  "jet_size/I");
    newtree->Branch("jethandle",  jethandle, "jethandle[jet_size]/I");
    newtree->Branch("had_b",     &had_b,     "had_b/I");
    newtree->Branch("lep_b",     &lep_b,     "lep_b/I");
    newtree->Branch("lephandle", &lephandle, "lephandle/I");
    newtree->Branch("chi2mass",  &chi2mass,  "chimass/F");
    newtree->Branch("seltmass",  &had_tmass, "had_tmass/F");
    newtree->Branch("lep_tmass",  &lep_tmass, "lep_tmass/F");
    
    // Looping events
    int events = CompMgr().CheckOption( "test" ) ? 10000 : CompMgr().GetEntries();

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
        *  Chi2 sorting
        *******************************************************************************/
        auto tup = CompMgr().GetSample()->GetChi2Info( jetidx, bjetidx );

        /*******************************************************************************
        *  Storing sample
        *******************************************************************************/
        chi2mass  = (Float_t)get<0>( tup );
        
        had_b     = get<2>( tup );
        lep_b     = had_b ? 0 : 1;
        had_b     = bjetidx[ had_b ];
        lep_b     = bjetidx[ lep_b ];
        
        lephandle = muidx[0];
        
        had_tmass = (Float_t)get<1>( tup );
        lep_tmass = CompMgr().GetSample()->GetLeptonicM(lephandle, lep_b); 

        jet_size  = (int) jetidx.size();
       
        for(int i=0; i<jet_size; i++){
            jethandle[i] = jetidx[i];
        }

        newtree->Fill();
    }

    cout<<endl;
    newtree->AutoSave();
    newfile->Close();
    delete newfile;
}
