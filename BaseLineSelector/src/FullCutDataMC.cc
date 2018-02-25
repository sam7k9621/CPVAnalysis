#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Selector&
FullMgr( const string& subdir, const string& json )
{
    static Selector mgr( subdir, json );
    return mgr;
}

extern void
MakeFullCut()
{
    // Setting filename ..._[0-9].root
    string str      = "_[0-9]";
    string filename = FullMgr().GetResultsName( "root", "Precut" );
    filename.insert( filename.length() - 5, str );

    TChain* ch = new TChain( "root" );
    ch->Add( ( filename ).c_str() );
    FullMgr().AddSample( ch );
    FullCut();

    delete ch;
}

extern void
FullCut()
{
    // Build new file
    TFile* newfile = TFile::Open( ( FullMgr().GetResultsName( "root", "FullCut" ) ).c_str(), "recreate" );
    TTree* newtree = FullMgr().CloneTree();

    // Register new branch
    Int_t jet_size;
    Int_t jethandle[ 256 ];
    Int_t had_b;
    Int_t lep_b;
    Int_t lephandle;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;

    newtree->Branch( "jet_size",  &jet_size,  "jet_size/I" );
    newtree->Branch( "jethandle", jethandle,  "jethandle[jet_size]/I" );
    newtree->Branch( "had_b",     &had_b,     "had_b/I" );
    newtree->Branch( "lep_b",     &lep_b,     "lep_b/I" );
    newtree->Branch( "lephandle", &lephandle, "lephandle/I" );
    newtree->Branch( "chi2mass",  &chi2mass,  "chimass/F" );
    newtree->Branch( "seltmass",  &had_tmass, "had_tmass/F" );
    newtree->Branch( "lep_tmass", &lep_tmass, "lep_tmass/F" );

    // Looping events
    int events = FullMgr().CheckOption( "test" ) ? 10000 : FullMgr().GetEntries();

    for( int i = 0; i < events; i++ ){
        FullMgr().GetEntry( i );
        FullMgr().process( events, i );

        vector<int> lepidx;// store one tight lepton
        vector<int> jetidx;// store every jets
        vector<int> bjetidx;// store two bjets

        /*******************************************************************************
        *  Baseline selection
        *******************************************************************************/
        if( !FullMgr().PassFullLep( lepidx ) ){
            continue;
        }

        if( !FullMgr().PassFullJet( jetidx, bjetidx, lepidx[ 0 ] ) ){
            continue;
        }

        /*******************************************************************************
        *  Chi2 sorting
        *******************************************************************************/
        auto tup = FullMgr().GetChi2Info( jetidx, bjetidx );

        /*******************************************************************************
        *  Storing sample
        *******************************************************************************/
        chi2mass = (Float_t)get<0>( tup );

        had_b = get<2>( tup );
        lep_b = had_b ? 0 : 1;
        had_b = bjetidx[ had_b ];
        lep_b = bjetidx[ lep_b ];

        lephandle = lepidx[ 0 ];

        had_tmass = (Float_t)get<1>( tup );
        lep_tmass = FullMgr().GetLeptonicM( lephandle, lep_b );

        jet_size = (int)jetidx.size();

        for( int i = 0; i < jet_size; i++ ){
            jethandle[ i ] = jetidx[ i ];
        }

        newtree->Fill();
    }

    cout << endl;
    newtree->AutoSave();
    newfile->Close();
    delete newfile;
}
