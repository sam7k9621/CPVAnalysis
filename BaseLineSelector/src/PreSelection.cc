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
extern void
MakePreCut()
{
    // initialize input file
    string sample            = PreMgr().GetOption<string>( "sample" );
    vector<string> sourcelst = PreMgr().GetSubListData<string>( sample, "path" );
    bool is_data             = sample.find( "run" ) != std::string::npos ? 1 : 0;
    TChain* ch               = new TChain( "bprimeKit/root" );

    for( const auto& i : sourcelst ){
        ch->Add( i.c_str() );
    }

    PreMgr().AddSample( ch );
    PreCut( is_data );
    delete ch;
}

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
PreCut( bool is_data )
{
    // Build new file
    TFile* newfile = TFile::Open( ( PreMgr().GetResultsName( "root", "PreCut" ) ).c_str(), "recreate" );
    TTree* newtree = PreMgr().CloneTree();

    // Running over golden_json
    checkEvtTool checkEvt;
    if( is_data ){
        string sample = PreMgr().GetOption<string>( "sample" ); 
        size_t found = sample.find( "el" );
        string lepton = found != std::string::npos ? "el" : "mu";
        checkEvt.addJson( PreMgr().GetSingleData<string>( lepton + "_lumimask" ) );
        cout<< PreMgr().GetSingleData<string>( lepton + "_lumimask" ) <<endl;
        checkEvt.makeJsonMap();
    }
    
    // Reading PUWeight file
    string line;
    vector<double> puweight;
    std::ifstream fin( "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/data/pileupweights_69200.csv" );
    while( std::getline( fin, line ) ){
        puweight.push_back( stod( line ) );
    }

    // Register new branch
    float weight;
    newtree->Branch( "PUWeight", &weight, "PUWeight/F" );

    // Prepare Datacard
    int entries   = 0;
    int positive  = 0;
    int negative  = 0;
    double effective = 0.0;

    // Looping events
    int events = PreMgr().CheckOption( "test" ) ? 10000 : PreMgr().GetEntries();

    for( int i = 0; i < events; i++ ){
        PreMgr().GetEntry( i );
        PreMgr().process( events, i );
        
        // pile-up reweighted
        if( !is_data ){
            if( !SetPUWeight( weight, puweight ) ){
                continue;
            }
        }
        else{
            weight = 1;
        }

        // datacard
        double gen = PreMgr().GenWeight() > 0 ? 1.0 : -1.0 ;
        if( gen > 0 )
            positive++;
        else
            negative++;
        
        entries++;
        effective += ( gen * weight );


        // Lumimask
        if( is_data ){
            if( !PreMgr().IsGoodEvt( checkEvt ) ){
                continue;
            }
        }

        // JERCorr
        if( !is_data ){
            PreMgr().JERCorr();
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
        output<<"Effective number of events after PU reweighting = "<< std::fixed << std::setprecision(2) <<effective<<endl;
        output.close();
    }

    cout << endl;
    newtree->AutoSave();
    newfile->Close();
    delete newfile;
}
