#include "CPVAnalysis/BaseLineSelector/interface/PreSelection.h"
#include "ManagerUtils/SysUtils/interface/PathUtils/CommonPath.hpp"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*******************************************************************************
*   Global function
*******************************************************************************/
extern Selector&
PreMgr( const string& subdir )
{
    static Selector mgr( subdir, "Selection.json" );
    return mgr;
}

/*******************************************************************************
*   PreCut
*******************************************************************************/
extern void
MakePreCut()
{
    // initialize input file
    string src               = PreMgr().GetOption<string>( "source" );
    vector<string> sourcelst = mgr::GetList<string>( "path", PreMgr().GetSubTree( src ) );
    TChain* ch               = new TChain( "bprimeKit/root" );

    for( const auto& i : sourcelst ){
        ch->Add( i.c_str() );
    }

    PreMgr().AddSample( src, ch );

    // Initialize data
    bool is_data    = strncmp( src.c_str(), "run", 3 ) ? 0 : 1;
    vector<int> hlt = is_data ? PreMgr().GetListData<int>("data_HLT") : PreMgr().GetListData<int>("mc_HLT");

    // Make new root file out of old one
    mgr::CheckPath( PreMgr().GetResultsName( "root", "precut" ) );
    TFile* newfile = TFile::Open( ( PreMgr().GetResultsName( "root", "precut" ) ).c_str(), "recreate" );
    ch->GetEntry( 0 );
    TTree* newtree = (TTree*)ch->GetTree()->CloneTree( 0 );

    // Running over golden_json
    checkEvtTool checkEvt;
    checkEvt.addJson( PreMgr().GetSingleData<string>( "lumimask" ) );
    checkEvt.makeJsonMap();

    //Decalre nVtx hist
    TH1D* nVtx        = new TH1D("nVtx",        "", 80, 0, 80);
    TH1D* nVtx_weight = new TH1D("nVtx_weight", "", 80, 0, 80);

    //Reading PUWeight file
    string line;
    vector<double> puweight;
    std::ifstream fin("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/data/pileupweights_69200.csv");
    while( std::getline(fin, line) ){
        puweight.push_back( stod(line) );
    }

    //Adding PUWeight scale branch
    float weight;
    newtree->Branch("PUWeight",&weight);
    
    // Looping for events
    int events = PreMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        PreMgr().process( events, i );

        // Lumimask
        if( is_data ){
            if( !PreMgr().GetSample()->isGoodEvt( checkEvt ) ){
                continue;
            }
        }

        //jet smeared
        if( !is_data ){
            PreMgr().GetSample()->jetSmeared();
        }
        // Pass vertex and hlt
        if( !PreMgr().GetSample()->passVertex() || !PreMgr().GetSample()->passHLT( hlt ) ){
            continue;
        }

        // Jet preselection at least four jet
        if( !PreMgr().GetSample()->preJet() ){
            continue;
        }

        // Lepton preselection at least one tight muon
        if( !PreMgr().GetSample()->preMuon() ){
            continue;
        }
        
        //pile-up reweighted
        if( !is_data ){
            int pv = PreMgr().GetSample()->nPU();
            weight = puweight[pv];
        }
        else{
            weight = 1;
        }

        //Plot nVtx hist
        double gen_weight = is_data ? 1 : PreMgr().GetSample()->GenWeight();
        nVtx       ->Fill( PreMgr().GetSample()->nVtx(), gen_weight );
        nVtx_weight->Fill( PreMgr().GetSample()->nVtx(), gen_weight*weight );

        newtree->Fill();
    }

    nVtx->Write();
    nVtx_weight->Write();
    cout << endl;
    newtree->AutoSave();
    newfile->Close();
    
    delete newfile;
    delete ch;
}
