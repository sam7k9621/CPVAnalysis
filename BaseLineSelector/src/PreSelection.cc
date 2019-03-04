#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"
#include "ManagerUtils/SysUtils/interface/PathUtils/CommonPath.hpp"
#include "TFile.h"

#include <math.h>
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
    newtree->SetDirectory( newfile );

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
    vector<double> puweight_up;
    vector<double> puweight_dn;
    std::ifstream fin( "/afs/cern.ch/work/p/pusheng/CMSSW_8_0_32/src/CPVAnalysis/BaseLineSelector/data/pileupweights_69200.csv" );
    while( std::getline( fin, line ) ){
        puweight.push_back( stod( line ) );
    }
    std::ifstream fin_up( "/afs/cern.ch/work/p/pusheng/CMSSW_8_0_32/src/CPVAnalysis/BaseLineSelector/data/pileupweights_72660.csv" );
    while( std::getline( fin_up, line ) ){
        puweight_up.push_back( stod( line ) );
    }
    std::ifstream fin_dn( "/afs/cern.ch/work/p/pusheng/CMSSW_8_0_32/src/CPVAnalysis/BaseLineSelector/data/pileupweights_65740.csv" );
    while( std::getline( fin_dn, line ) ){
        puweight_dn.push_back( stod( line ) );
    }

    // Register new branch
    int   muFR_size;
    float weight;
    float weight_up;
    float weight_dn;
    float pdfweight_up;
    float pdfweight_dn;
    float meps_up;
    float meps_dn;
    float muFmuR[9];
    newtree->Branch( "PUWeight",        &weight,        "PUWeight/F" );
    newtree->Branch( "PUWeight_up",     &weight_up,     "PUWeight_up/F" );
    newtree->Branch( "PUWeight_dn",     &weight_dn,     "PUWeight_dn/F" );
    newtree->Branch( "PDFWeight_up",    &pdfweight_up,  "PDFWeight_up/F" );
    newtree->Branch( "PDFWeight_dn",    &pdfweight_dn,  "PDFWeight_dn/F" );
    newtree->Branch( "ME_PS_up",        &meps_up,       "ME_PS_up/F" );
    newtree->Branch( "ME_PS_dn",        &meps_dn,       "ME_PS_dn/F" );
    newtree->Branch( "muFR_size",       &muFR_size,     "muFR_size/I");
    newtree->Branch( "MuFMuR",          &muFmuR[0],     "MuFMuR[muFR_size]/F" );

    // Prepare Datacard
    int entries   = 0;
    int positive  = 0;
    int negative  = 0;
    double effective    = 0.0;
    double effective_up = 0.0;
    double effective_dn = 0.0;
    double pdf_up = 0.0;
    double pdf_dn = 0.0;
    double MEPS_up = 0.0;
    double MEPS_dn = 0.0;
    double MuFMuR[9] = { 0.0 };

    // Looping events
    int events = PreMgr().CheckOption( "test" ) ? 1000 : PreMgr().GetEntries();

    for( int i = 0; i < events; i++ ){
        PreMgr().GetEntry( i );
        PreMgr().process( events, i );
        muFR_size = 9;
        
        // pile-up reweighted
        // abandom events with >74 vertex
        if( !is_data ){
            if( 
                !SetPUWeight( weight, puweight ) ||
                !SetPUWeight( weight_up, puweight_up ) ||
                !SetPUWeight( weight_dn, puweight_dn ) 
                ){
                continue;
            }
        
            PreMgr().SetPDFUnc( pdfweight_up, pdfweight_dn );
            PreMgr().SetMuFMuRUnc( muFmuR );
            PreMgr().SetME_PSUnc( meps_up, meps_dn );
        }
        else{
            weight    = 1;
            weight_up = 1;
            weight_dn = 1;
        }

        // datacard
        double gen = PreMgr().GenWeight() > 0 ? 1.0 : -1.0 ;
        if( gen > 0 )
            positive++;
        else
            negative++;
        
        entries++;
        effective    += ( gen * weight );
        effective_up += ( gen * weight_up );
        effective_dn += ( gen * weight_dn );
        pdf_up       += ( gen * weight * pdfweight_up );
        pdf_dn       += ( gen * weight * pdfweight_dn );
        MEPS_up      += ( gen * weight * meps_up );
        MEPS_dn      += ( gen * weight * meps_dn );
        
        for( int i = 0; i < 9; i++ ){
            MuFMuR[i] += ( gen * weight * muFmuR[i] );
        }

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
        output<<"Effective number of events after PU    reweighting = "<< std::fixed << std::setprecision(2) <<effective<<endl;
        output<<"Effective number of events after PU_up reweighting = "<< std::fixed << std::setprecision(2) <<effective_up<<endl;
        output<<"Effective number of events after PU_dn reweighting = "<< std::fixed << std::setprecision(2) <<effective_dn<<endl;
        output<<"Effective number of events after PDF_up reweighting = "<< std::fixed << std::setprecision(2) <<pdf_up<<endl;
        output<<"Effective number of events after PDF_dn reweighting = "<< std::fixed << std::setprecision(2) <<pdf_dn<<endl;
        output<<"Effective number of events after ME_PS_up reweighting = "<< std::fixed << std::setprecision(2) <<MEPS_up<<endl;
        output<<"Effective number of events after ME_PS_dn reweighting = "<< std::fixed << std::setprecision(2) <<MEPS_dn<<endl;

        for( int i = 0; i < 9; i++){
            output<<"Effective number of events after MuFMuR_" <<i<<" reweighting = "<< std::fixed << std::setprecision(2) <<MuFMuR[i]<<endl;
        }
        output.close();
    }

    cout << endl;
    newtree->AutoSave();
    delete newfile;
}
