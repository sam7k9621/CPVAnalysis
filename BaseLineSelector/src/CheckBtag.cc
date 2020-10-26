#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;

extern Histor&
CompMgr( const string& subdir, const string& json )
{
    static Histor mgr( subdir, json );
    return mgr;
}

extern void
CheckBtag()
{
    // Build new file
    string year   = PreMgr().GetOption<string>( "year" );
    string sample = PreMgr().GetOption<string>( "sample" );
    string lepton = PreMgr().GetOption<string>( "lepton" );
    bool is_data  = sample.find( "Run" ) != string::npos;
    CompMgr( "CompareDataMC", "WeightInfo.py" );
    CompMgr().InitRoot( "sample" + year );
    PreMgr().InitRoot( "sample" + year );

    string filename = PreMgr().GetParam<string>( sample, "path" );
    cout << ">> Processing " << filename << endl;
    TChain* ch = new TChain( "bprimeKit/root" );
    ch->Add( ( filename ).c_str() );
    PreMgr().AddSample( ch, is_data, sample );
    CompMgr().AddSample( sample );
    AddHist();

    vector<string> subsample;
    boost::split( subsample, sample, boost::is_any_of( "_" ) );
    subsample = vector<string>( subsample.begin(), subsample.end() - 1 );
    sample    = boost::algorithm::join( subsample, "_" );
    if( sample == "TTToSemiLeptonic" ){
        sample = "ttbar";
    }

    // Initialize data
    string line;
    vector<double> puweight;
    std::ifstream fin( PreMgr().GetParam<string>( "Info", "puweight" ) );

    while( std::getline( fin, line ) ){
        puweight.push_back( stod( line ) );
    }

    vector<int> hlt = PreMgr().GetVParam<int>( "Info", lepton + "_HLT" );
    vector<int> lepidx;// store two tight lepton
    vector<int> jetidx;

    // Running over golden_json
    checkEvtTool checkEvt;
    if( is_data ){
        checkEvt.addJson( PreMgr().GetParam<string>( "Info", "lumimask" ) );
        checkEvt.makeJsonMap();
    }

    // Looping events
    string tag = PreMgr().GetOption<string>( "uncertainty" );
    int events = PreMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        PreMgr().process( events, i );
        lepidx.clear();
        jetidx.clear();

        /*******************************************************************************
        *  Energy correction
        *******************************************************************************/
        if( !is_data ){
            PreMgr().JERCorr();
        }

        /*******************************************************************************
        *  Baseline selection
        *******************************************************************************/
        if( !PreMgr().PassHLT( hlt ) ){
            continue;
        }

        // Pass vertex
        if( !PreMgr().PassVertex() ){
            continue;
        }

        // Lumimask
        if( is_data ){
            if( !PreMgr().IsGoodEvt( checkEvt ) ){
                continue;
            }
        }

        /*******************************************************************************
        *  Jet selection
        *******************************************************************************/
        if( !PreMgr().PassFullJet_CRDYJets( jetidx ) ){
            continue;
        }

        /*******************************************************************************
        *  Lepton selection
        *******************************************************************************/
        if( !PreMgr().PassFullLepton_CRDYJets( lepidx, lepton ) ){
            continue;
        }

        double zmass = PreMgr().GetZmass( lepidx );
        if( zmass > 120 || zmass < 60 ){
            continue;
        }

        /*******************************************************************************
        *  Event weighting
        *******************************************************************************/
        double weight = 1.0;
        if( !is_data ){
            int pv = PreMgr().nPU();
            if( pv < 0 || pv >= (int)puweight.size() ){
                continue;
            }

            weight *= puweight[ pv ];
            weight *= PreMgr().GenWeight();
        }

        /*******************************************************************************
        * Selected jet info filling
        *******************************************************************************/
        CompMgr().Hist( "Zmass" )->Fill( zmass, weight );
        CompMgr().Hist( "LeadingLepPt" )->Fill( PreMgr().GetLepPt( jetidx.front() ), weight );
        CompMgr().Hist( "LeadingLepEta" )->Fill( PreMgr().GetLepEta( jetidx.front() ), weight );
        CompMgr().Hist( "LeadingJetDeepCSV" )->Fill( PreMgr().GetJetCSV( jetidx.front() ), weight );
        CompMgr().Hist( "LeadingJetPt" )->Fill( PreMgr().GetJetPt( jetidx.front() ), weight );
        CompMgr().Hist( "LeadingJetEta" )->Fill( PreMgr().GetJetEta( jetidx.front() ), weight );
    }

    cout << endl;
    /*  if( !is_data ){*/
    // CompMgr().WeightMC( sample );
    // cout<<">>Weighting "<<sample<<endl;
    /*}*/
    StoreCompare();
    delete ch;
}

extern void
AddHist()
{
    CompMgr().AddHist( "Zmass", "Z mass", "Events", 50, 40, 140 );
    CompMgr().AddHist( "LeadingLepPt", "Leading lep P_{T}", "Events", 30, 0, 150 );
    CompMgr().AddHist( "LeadingLepEta", "Leading lep Eta", "Events", 40, -3, 5 );
    CompMgr().AddHist( "LeadingJetDeepCSV", "Leading jet DeepCSV", "Events", 30, 0, 0.15 );
    CompMgr().AddHist( "LeadingJetPt", "Leading jet P_{T}", "Events", 30, 0, 150 );
    CompMgr().AddHist( "LeadingJetEta", "Leading jet Eta", "Events", 40, -3, 5 );
}

extern void
StoreCompare()
{
    string filename = PreMgr().GetResultsName( "root", "CheckBWeight" );
    mgr::SaveToROOT( CompMgr().Hist( "Zmass" ),             filename, "Zmass" );
    mgr::SaveToROOT( CompMgr().Hist( "LeadingLepPt" ),      filename, "LeadingLepPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LeadingLepEta" ),     filename, "LeadingLepEta" );
    mgr::SaveToROOT( CompMgr().Hist( "LeadingJetDeepCSV" ), filename, "LeadingJetDeepCSV" );
    mgr::SaveToROOT( CompMgr().Hist( "LeadingJetPt" ),      filename, "LeadingJetPt" );
    mgr::SaveToROOT( CompMgr().Hist( "LeadingJetEta" ),     filename, "LeadingJetEta" );
}
