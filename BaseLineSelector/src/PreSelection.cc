#include "CPVAnalysis/BaseLineSelector/interface/PreSelection.h"
#include "ManagerUtils/SysUtils/interface/PathUtils/CommonPath.hpp"
#include "TFile.h"

#include <iostream>

using namespace std;

/*******************************************************************************
*   Global function
*******************************************************************************/
extern Selector&
PreMgr( const string& subdir )
{
    static Selector mgr( subdir );
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
    vector<int> hlt = mgr::GetList<int>( "HLT", PreMgr().GetSubTree( src ) );
    bool is_data    = strncmp( src.c_str(), "run", 3 ) ? 0 : 1;

    // Make new root file out of old one
    mgr::CheckPath( PreMgr().GetResultsName( "root", "precut" ) );
    TFile* newfile = new TFile( ( PreMgr().GetResultsName( "root", "precut" ) ).c_str(), "recreate" );
    ch->GetEntry( 0 );
    TTree* newtree = (TTree*)ch->GetTree()->CloneTree( 0 );

    // Running over golden_json
    checkEvtTool checkEvt;
    checkEvt.addJson( PreMgr().GetSingleData<string>( "lumimask" ) );
    checkEvt.makeJsonMap();

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

        newtree->Fill();
    }

    cout << endl;
    newtree->AutoSave();
    newfile->Close();
    delete newfile;
    delete ch;
}