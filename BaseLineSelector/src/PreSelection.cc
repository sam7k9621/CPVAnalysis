#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"
#include "ManagerUtils/SysUtils/interface/PathUtils/CommonPath.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TFile.h"
#include "TEfficiency.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

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
*   B-tag efficiency
*******************************************************************************/
extern void 
MakeBtagEff()
{
    // Build new file
    string year = PreMgr().GetOption<string>( "year" );
    double csv  = PreMgr().GetOption<double>( "CSV"  );
    PreMgr().InitRoot( "sample" + year );
   
    TChain* ch = new TChain( "bprimeKit/root" );
    string sample = PreMgr().GetOption<string>( "sample" );
    string source = PreMgr().GetParam<string>( sample, "path" );
    ch->Add( source.c_str() );
    PreMgr().AddSample( ch );
    
    // Looping events
    int events = PreMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    vector<double> xlst = {0., 20., 30., 50., 70., 100., 140., 200., 300., 600., 1000.};
    vector<double> ylst = {-2.4, -2.0, -1.6, -1.2, -0.8, -0.4, 0., 0.4, 0.8, 1.2, 1.6, 2.0, 2.4};

    TEfficiency* eff_b = new TEfficiency( "eff_b", "eff_b", xlst.size()-1, &(xlst[0]), ylst.size()-1, &(ylst[0]) );
    TEfficiency* eff_c = new TEfficiency( "eff_c", "eff_c", xlst.size()-1, &(xlst[0]), ylst.size()-1, &(ylst[0]) );
    TEfficiency* eff_l = new TEfficiency( "eff_l", "eff_l", xlst.size()-1, &(xlst[0]), ylst.size()-1, &(ylst[0]) );
    
    TEfficiency* eff_b_test = new TEfficiency( "eff_b_test", "eff_b_test", xlst.size()-1, &(xlst[0]), ylst.size()-1, &(ylst[0]) );
    TEfficiency* eff_c_test = new TEfficiency( "eff_c_test", "eff_c_test", xlst.size()-1, &(xlst[0]), ylst.size()-1, &(ylst[0]) );
    TEfficiency* eff_l_test = new TEfficiency( "eff_l_test", "eff_l_test", xlst.size()-1, &(xlst[0]), ylst.size()-1, &(ylst[0]) );

    vector<int> jetlst;
    vector<int> jetlst_test;
    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        jetlst.clear();
        jetlst_test.clear();

        PreMgr().process( events, i );
           
        PreMgr().GetSelJet( jetlst, jetlst_test );

        if( jetlst.empty() && jetlst_test.empty() ){
            continue;
        }

        PreMgr().FillBtagEff( eff_b, eff_c, eff_l, jetlst, csv );
        PreMgr().FillBtagEff( eff_b_test, eff_c_test, eff_l_test, jetlst_test, csv );
    }

    string filename = PreMgr().GetResultsName( "root", "BtagEff" );
    mgr::SaveToROOT( eff_b, filename, "eff_b" ); 
    mgr::SaveToROOT( eff_c, filename, "eff_c" ); 
    mgr::SaveToROOT( eff_l, filename, "eff_l" ); 
    mgr::SaveToROOT( eff_b_test, filename, "eff_b_test" ); 
    mgr::SaveToROOT( eff_c_test, filename, "eff_c_test" ); 
    mgr::SaveToROOT( eff_l_test, filename, "eff_l_test" ); 
}

/*******************************************************************************
*   PreCut
*******************************************************************************/
extern void
MakePreCut()
{
    // Build new file
    string year = PreMgr().GetOption<string>( "year" );
    PreMgr().InitRoot( "sample" + year );
    TFile* newfile = TFile::Open( ( PreMgr().GetResultsName( "root", "PreCut" ) ).c_str(), "recreate" );

    string sample = PreMgr().GetOption<string>( "sample" );
    string source = PreMgr().GetParam<string>( sample, "path" );
    bool is_data  = sample.find( "Run" ) != std::string::npos ? 1 : 0;

    TChain* ch = new TChain( "bprimeKit/root" );
    ch->Add( source.c_str() );
    PreMgr().AddSample( ch );

    // Discard useless branches
    Discard( ch );
    TTree* newtree = ch->CloneTree( 0 );

    // Running over golden_json
    checkEvtTool checkEvt;
    if( is_data ){
        checkEvt.addJson( PreMgr().GetParam<string>( "Info", "lumimask" ) );
        checkEvt.makeJsonMap();
    }

    // Reading PUWeight file
    string line;
    vector<double> puweight;
    std::ifstream fin( PreMgr().GetParam<string>( "Info", "puweight" ) );
    vector<double> puweight_up;
    std::ifstream fin_up( PreMgr().GetParam<string>( "Info", "puweight_up" ) );
    vector<double> puweight_dn;
    std::ifstream fin_dn( PreMgr().GetParam<string>( "Info", "puweight_dn" ) );

    while( std::getline( fin, line ) ){
        puweight.push_back( stod( line ) );
    }

    while( std::getline( fin_up, line ) ){
        puweight_up.push_back( stod( line ) );
    }

    while( std::getline( fin_dn, line ) ){
        puweight_dn.push_back( stod( line ) );
    }

    float weight;
    newtree->Branch( "PUWeight", &weight, "PUWeight/F" );
    float weight_up;
    newtree->Branch( "PUWeight_up", &weight_up, "PUWeight_up/F" );
    float weight_dn;
    newtree->Branch( "PUWeight_dn", &weight_dn, "PUWeight_dn/F" );

    // Prepare Datacard
    int entries  = 0;
    double positive = 0.;
    double negative = 0.;

    // Looping events
    int events = PreMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        PreMgr().process( events, i );

        // pile-up reweighted
        // abandom events with larger than 99 vertex
        weight = 1;
        weight_up = 1;
        weight_dn = 1;
       
        if( !is_data ){
            int pv = PreMgr().nPU();
            if( pv < 0 || pv >= (int)puweight.size() ){
                continue;
            }
        
            weight = puweight[ pv ];
            weight_up = puweight_up[ pv ];
            weight_dn = puweight_dn[ pv ];
        }

        // datacard
        double gen = PreMgr().GenWeight();
        positive += std::max( 0., gen );                                                
        negative -= std::min( 0., gen );                                                
        entries++;

        // Lumimask
        if( is_data ){
            if( !PreMgr().IsGoodEvt( checkEvt ) ){
                continue;
            }
        }
        
        if( !is_data && year != "18" ){
            PreMgr().LeptonECorr();
        }

        // Pass vertex
        if( !PreMgr().PassVertex() ){
            continue;
        }
        
        // Preselection :
        // Jet : at least four jets
        // Lep : at least one lepton
        if( !PreMgr().PreJet() ){
            continue;
        }
        
        if( !PreMgr().PreLep() ){
            continue;
        }
        newtree->Fill();
    }

    if( !is_data ){
        ofstream output;
        output.open( PreMgr().GetResultsName( "txt", "Datacard" ) );
        output << PreMgr().GetOption<string>( "sample" ) << endl;
        output << "Number of events = " << entries << endl;
        output << "Sum of Positive weights = " << positive << endl;
        output << "Sum of Negative weights = " << negative << endl;
        output << "Effective number of events = " << std::fixed << std::setprecision( 2 ) << positive - negative << endl;
        output.close();
    }

    cout << endl;
    newtree->AutoSave();
    delete ch;
    delete newfile;
}

extern void
MakeDataCard()
{
    string sample = PreMgr().GetOption<string>( "sample" );
    string source = PreMgr().GetParam<string>( sample, "path" );

    TChain* ch = new TChain( "bprimeKit/root" );
    ch->Add( source.c_str() );
    PreMgr().AddSample( ch );

    // Prepare Datacard
    int entries  = 0;
    double positive = 0.;
    double negative = 0.;

    // Looping events
    int events = PreMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        PreMgr().process( events, i );

        int pv = PreMgr().nPU();
        if( pv < 0 ){
            continue;
        }

        // datacard
        double gen = PreMgr().GenWeight();
        positive += std::max( 0., gen );
        negative -= std::min( 0., gen );
        entries++;
    }

    ofstream output;
    output.open( PreMgr().GetResultsName( "txt", "Datacard" ) );
    output << PreMgr().GetOption<string>( "sample" ) << endl;
    output << "Number of events = " << entries << endl;
    output << "Sum of Positive weights = " << positive << endl;
    output << "Sum of Negative weights = " << negative << endl;
    output << "Effective number of events = " << std::fixed << std::setprecision( 2 ) << positive - negative << endl;
    output.close();

    delete ch;
}
