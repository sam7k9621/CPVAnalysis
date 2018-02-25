#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"
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
    vector<string> sourcelst = mgr::GetList<string>( "path", PreMgr().GetSubTree( sample ) );
    bool is_data             = sample.find("run") != std::string::npos? 1 : 0;
    TChain* ch               = new TChain( "bprimeKit/root" );

    for( const auto& i : sourcelst ){
        ch->Add( i.c_str() );
    }
   
    PreMgr().AddSample( ch );
    PreCut(is_data);
    
    delete ch;
}

extern void
PreCut(bool is_data)
{
    //Build new file
    TFile* newfile = TFile::Open( ( PreMgr().GetResultsName( "root", "precut" ) ).c_str(), "recreate" );
    TTree* newtree = PreMgr().CloneTree();

    // Initialize data
    vector<int> hlt = is_data ? PreMgr().GetListData<int>("data_HLT") : PreMgr().GetListData<int>("mc_HLT");
        // Running over golden_json
    checkEvtTool checkEvt;
    checkEvt.addJson( PreMgr().GetSingleData<string>( "lumimask" ) );
    checkEvt.makeJsonMap();
        //Reading PUWeight file
    string line;
    vector<double> puweight;
    std::ifstream fin("/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/data/pileupweights_69200.csv");
    while( std::getline(fin, line) ){
        puweight.push_back( stod(line) );
    }

    //Register new branch
    float weight;
    newtree->Branch("PUWeight",&weight, "PUWeight/F");
    
    // Looping events
    int events = PreMgr().CheckOption( "test" ) ? 10000 : PreMgr().GetEntries();

    for( int i = 0; i < events; i++ ){
        PreMgr().GetEntry( i );
        PreMgr().process( events, i );

        //Lumimask
        if( is_data ){
            if( !PreMgr().IsGoodEvt( checkEvt ) ){
                continue;
            }
        }

        //JERCorr 
        if( !is_data ){
            PreMgr().JERCorr();
        }
        
        //Pass vertex and hlt
        if( !PreMgr().PassVertex() || !PreMgr().PassHLT( hlt ) ){
            continue;
        }

        //Preselection :
        //Jet: at least four jets
        //Lepton: at least one tight muon
        if( !PreMgr().PreSelection() ){
            continue;
        }
        
        //pile-up reweighted
        if( !is_data ){
            int pv = PreMgr().nPU();
            weight = puweight[pv];
        }
        else{
            weight = 1;
        }

        newtree->Fill();
    }

    cout<<endl;
    newtree->AutoSave();
    newfile->Close(); 
    delete newfile;
}
