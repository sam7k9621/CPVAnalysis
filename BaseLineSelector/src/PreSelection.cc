#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"
#include "ManagerUtils/SysUtils/interface/PathUtils/CommonPath.hpp"
#include "TFile.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

/*******************************************************************************
*   Global function
*******************************************************************************/
extern Selector&
PreMgr( const string& subdir, const string& json )
{
    static Selector mgr( subdir, json );
    return mgr;
}

/*******************************************************************************
*   PreCut
*******************************************************************************/
extern bool
SetPUWeight( float& weight, const vector<double>& puweight )
{
    int pv = PreMgr().nPU();
    if( pv > 74 )
        return false;
    else
        weight = puweight[ pv ];
   
    return true;
}

extern void
MakePreCut()
{
    // Build new file
    TFile* newfile = TFile::Open( ( PreMgr().GetResultsName( "root", "PreCut" ) ).c_str(), "recreate" );
    
    string sample = PreMgr().GetOption<string>( "sample" );
    string source = PreMgr().GetSubSingleData<string>( sample, "path" );
    bool is_data  = sample.find( "Run" ) != std::string::npos ? 1 : 0;
    
    TChain* ch    = new TChain( "bprimeKit/root" );
    ch->Add( source.c_str() );
    PreMgr().AddSample( ch );

    TTree* newtree = ch->CloneTree( 0 );

    // Running over golden_json
    checkEvtTool checkEvt;
    if( is_data ){
        checkEvt.addJson( PreMgr().GetSingleData<string>( "lumimask" ) );
        checkEvt.makeJsonMap();
    }
    
    // Reading PUWeight file
    string line;
    vector<double> puweight;
    std::ifstream fin( PreMgr().GetSingleData<string>( "puweight" ) );
    while( std::getline( fin, line ) ){
        puweight.push_back( stod( line ) );
    }
    float weight;
    newtree->Branch( "PUWeight",        &weight,        "PUWeight/F" );

    // Prepare Datacard
    int entries   = 0;
    int positive  = 0;
    int negative  = 0;

    // Looping events
    int events = PreMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        PreMgr().process( events, i );

        // pile-up reweighted
        // abandom events with >74 vertex
        if( !is_data ){
            if( 
                !SetPUWeight( weight, puweight ) 
                ){
                continue;
            }    
        }
        else{
            weight    = 1;
        }

        // datacard
        double gen = PreMgr().GenWeight() > 0 ? 1.0 : -1.0 ;
        if( gen > 0 )
            positive++;
        else
            negative++;
        
        entries++;

        // Lumimask
        if( is_data ){
            if( !PreMgr().IsGoodEvt( checkEvt ) ){
                continue;
            }
        }

        // Pass vertex 
        if( !PreMgr().PassVertex() ){
            continue;
        }

        // Preselection :
        // Jet : at least four jets
        // Lep : at least one lepton
        if( !PreMgr().PreSelection() ){
            continue;
        }

        newtree->Fill();
    }

    if( !is_data ){
        ofstream output;
        output.open( PreMgr().GetResultsName( "txt", "Datacard" ) );
        output<<PreMgr().GetOption<string>( "sample" )<<endl;
        output<<"Number of events = "<<entries<<endl;
        output<<"Sum of Positive weights = "<<positive<<endl;
        output<<"Sum of Negative weights = "<<negative<<endl;
        output<<"Effective number of events = "<< std::fixed << std::setprecision(2) << positive - negative <<endl;
        output.close();
    }
    
    cout << endl;
    newtree->AutoSave();
    delete ch;
    delete newfile;
}
