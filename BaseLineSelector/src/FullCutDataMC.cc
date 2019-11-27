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

extern string
MakeFileName( bool is_data )
{
    string pos      = FullMgr().IsGrid5() ? FullMgr().ResultsDir() : "/eos/cms/store/user/pusheng/public/PreCut";
    string year     = FullMgr().GetOption<string>( "year" ); 

    if( is_data ){
        string lepton = FullMgr().GetOption<string>( "lepton" );

        if( lepton == "el" ){
            return pos / ( "PreCut_" + year + "_*E*_" +  FullMgr().GetOption<string>( "sample" ) + ".root" );
            // /eos/cms/store/user/pusheng/files/PreCut_mu_runG_2.root
        }
        else if( lepton == "mu" ){
            return pos / ( "PreCut_" + year + "_*M*_" +  FullMgr().GetOption<string>( "sample" ) + ".root" );
            // /eos/cms/store/user/pusheng/files/PreCut_mu_runG_2.root
        }
        else{
            return "Lepton type " + lepton + "cannot be found";
        }
    }
    else{
        return pos / ( "PreCut_" + year + "_" + FullMgr().GetOption<string>( "sample" ) + ".root" );
    }
}

extern void
MakeFullCut()
{
    // Build new file
    FullMgr().IsGrid5();
    
    FullMgr().InitRoot( "sample" + FullMgr().GetOption<string>( "year" ) );
    //TFile* newfile = TFile::Open( ( FullMgr().GetEOSName( "root", "FullCut", "FullCut" ) ).c_str(), "recreate" );
    TFile* newfile = TFile::Open( ( FullMgr().GetResultsName( "root", "FullCut", "FullCut" ) ).c_str(), "recreate" );

    std::size_t found = FullMgr().GetOption<string>( "sample" ).find( "Run" );
    bool is_data      = found != std::string::npos ? true : false;
    string filename   = MakeFileName( is_data );
    cout << ">> Processing " << filename << endl;

    TChain* ch = new TChain( "root" );
    ch->Add( ( filename ).c_str() );
    FullMgr().AddSample( ch );
    TTree* newtree = ch->CloneTree( 0 );

    // Initialize data
    string lepton   = FullMgr().GetOption<string>( "lepton" );
    vector<int> hlt = FullMgr().GetVParam<int>( "Info", lepton + "_HLT" );
    string region = FullMgr().CheckOption( "region" ) ? "CR" : "SR";
    TH2D* beff = FullMgr().GetSFHist( region + "_eff_b" );
    TH2D* ceff = FullMgr().GetSFHist( region + "_eff_c" );
    TH2D* leff = FullMgr().GetSFHist( region + "_eff_l" );
    FullMgr().InitBtagWeight( "bcheck", FullMgr().GetParam<string>( "BtagWeight", "path" ) );
    FullMgr().InitBtagEffPlot( beff, ceff, leff );
   
    // Register new branch
    Int_t had_b;
    Int_t lep_b;
    Int_t lep;
    Int_t jet1;
    Int_t jet2;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;
    Float_t b_weight;
    Float_t b_weight_up;
    Float_t b_weight_dn;

    newtree->Branch( "had_b",       &had_b,       "had_b/I" );
    newtree->Branch( "lep_b",       &lep_b,       "lep_b/I" );
    newtree->Branch( "jet1",        &jet1,        "jet1/I" );
    newtree->Branch( "jet2",        &jet2,        "jet2/I" );
    newtree->Branch( "lep",         &lep,         "lep/I" );
    newtree->Branch( "chi2mass",    &chi2mass,    "chimass/F" );
    newtree->Branch( "had_tmass",   &had_tmass,   "had_tmass/F" );
    newtree->Branch( "lep_tmass",   &lep_tmass,   "lep_tmass/F" );
    newtree->Branch( "b_weight",    &b_weight,    "b_weight/F" );
    newtree->Branch( "b_weight_up", &b_weight_up, "b_weight_up/F" );
    newtree->Branch( "b_weight_dn", &b_weight_dn, "b_weight_dn/F" );

    // Looping events
    string tag = FullMgr().GetOption<string>( "uncertainty" );
    int events = FullMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        FullMgr().process( events, i );
        vector<int> lepidx;// store one tight lepton
        vector<int> jetidx;// store every jets
        vector<int> bjetidx;// store two bjets

        // JERCorr
        if( !is_data ){
            if( tag == "JER_up" ){
                FullMgr().JERCorrUp();
            }
            else if( tag == "JER_dn" ){
                FullMgr().JERCorrDn();
            }
            else{
                FullMgr().JERCorr();
            }
        }

        // JEC 
        if( !is_data ){
            if( tag == "JEC_up" ){
                FullMgr().JECCorrUp();
            }
            else if( tag == "JEC_dn" ){
                FullMgr().JECCorrDn();
            }
        }
        
        /*******************************************************************************
        *  Baseline selection
        *******************************************************************************/
        if( !FullMgr().PassHLT( hlt ) ){
            continue;
        }

        /*******************************************************************************
        *  Lepton selection
        *******************************************************************************/
        if( FullMgr().OptionContent( "region", "WJets" ) ){
            if( !FullMgr().PassFullLepton_CRWJets( lepidx, lepton ) ){
                continue;
            }
        }
        else if( FullMgr().OptionContent( "region", "QCD" ) ){
            if( !FullMgr().PassFullLepton_CRQCD( lepidx, lepton ) ){
                continue;
            }
        }
        else{ 
            if( !FullMgr().PassFullLepton( lepidx, lepton ) ){
                continue;
            }
        }
        /*******************************************************************************
        *  Jet selection
        *******************************************************************************/
        if( FullMgr().OptionContent( "region", "WJets" ) ){
            if( !FullMgr().PassFullJet_CRWJets( jetidx, bjetidx, lepidx[ 0 ] ) ){
                continue;
            }
           
        }
        else if( FullMgr().OptionContent( "region", "QCD" ) ){
            if( !FullMgr().PassFullJet_CRQCD( jetidx, bjetidx, lepidx[ 0 ] ) ){
                continue;
            }
        }
        else{
            if( !FullMgr().PassFullJet( jetidx, bjetidx, lepidx[ 0 ] ) ){
                continue;
            }
        }
        /*******************************************************************************
        *  b-tagging probability
        *******************************************************************************/
        if( !is_data ){
            if( FullMgr().CheckOption( "region" ) ){
                b_weight    = FullMgr().GetBtagWeight_CR( bjetidx, jetidx );
            }
            else{
                b_weight    = FullMgr().GetBtagWeight( bjetidx, jetidx );
                b_weight_up = FullMgr().GetBtagWeight( bjetidx, jetidx, "up" );
                b_weight_dn = FullMgr().GetBtagWeight( bjetidx, jetidx, "down" );
            }
        }
        /*******************************************************************************
        *  Chi2 sorting
        *******************************************************************************/
        auto tup = FullMgr().GetChi2Info( jetidx, bjetidx );

        /*******************************************************************************
        *  Storing sample
        *******************************************************************************/
        chi2mass  = (Float_t)get<0>( tup );
        had_tmass = (Float_t)get<1>( tup );
        had_b     = get<2>( tup );
        jet1      = get<3>( tup );
        jet2      = get<4>( tup );
        lep_b     = had_b ? 0 : 1;

        had_b = bjetidx[ had_b ];
        lep_b = bjetidx[ lep_b ];
        jet1  = jetidx [ jet1 ];
        jet2  = jetidx [ jet2 ];
        lep   = lepidx[ 0 ]; // choose leading lepton

        lep_tmass = FullMgr().GetLeptonicM( lep, lep_b );

        newtree->Fill();
    }

    cout << endl;
    
    newtree->AutoSave();
    delete ch;
    delete newfile;
}
