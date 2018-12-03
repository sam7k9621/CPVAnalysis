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
MakeFileName(bool is_data)
{
    string pos = FullMgr().ResultsDir();
    string filename = "";

    if( is_data ){
        filename = pos / ( "PreCut_" + FullMgr().GetOption<string>("lepton") + "_run*.root" );
        // /wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/PreCut_mu_run*.root
    }
    else{
        filename = pos / ( "PreCut_" + FullMgr().GetOption<string>("sample") + "_[0-9]+.root" );
        // /wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/results/PreCut_TTbar_[0-9]+.root
    }

    return filename;
}
extern void
MakeFullCut()
{
    bool is_data = FullMgr().GetOption<string>("sample") == "Data" ? true : false;
    string filename = MakeFileName( is_data );

    TChain* ch = new TChain( "root" );
    ch->Add( ( filename ).c_str() );
    FullMgr().AddSample( ch );
    FullCut( is_data );

    delete ch;
}

extern void
MakeCheckSelection()
{
    bool is_data = FullMgr().GetOption<string>("sample") == "Data" ? true : false;
    string filename = MakeFileName( is_data );

    TChain* ch = new TChain( "root" );
    ch->Add( ( filename ).c_str() );
    FullMgr().AddSample( ch );
    CheckSelection( is_data );

    delete ch;
}

extern void
CheckSelection( bool is_data )
{
    // Build new file
    TFile* newfile = TFile::Open( ( FullMgr().GetResultsName( "root", "FullCut" ) ).c_str(), "recreate" );
    TTree* newtree = new TTree("root", "root");
    
    // Initialize data
    string lepton = FullMgr().GetOption<string>("lepton");
    vector<int> hlt = is_data ? FullMgr().GetListData<int>( lepton + "_data_HLT" ) : FullMgr().GetListData<int>( lepton + "_mc_HLT" );
    FullMgr().RegisterWeight();
    // Register new branch
    Float_t LJetPt1;
    Float_t LJetPt2;
    Float_t LJetPt3;
    Float_t LJetPt4;
    Float_t LLepPt1;
    Float_t LLepPt2;
    Float_t LLepPt3;
    Float_t LLepPt4;

    Float_t puweight;

    newtree->Branch( "LJetPt1", &LJetPt1, "LJetPt1/F" );
    newtree->Branch( "LJetPt2", &LJetPt2, "LJetPt2/F" );
    newtree->Branch( "LJetPt3", &LJetPt3, "LJetPt3/F" );
    newtree->Branch( "LJetPt4", &LJetPt4, "LJetPt4/F" );
    newtree->Branch( "LLepPt1", &LLepPt1, "LLepPt1/F" );
    newtree->Branch( "LLepPt2", &LLepPt2, "LLepPt2/F" );
    newtree->Branch( "LLepPt3", &LLepPt3, "LLepPt3/F" );
    newtree->Branch( "LLepPt4", &LLepPt4, "LLepPt4/F" );

    newtree->Branch( "PUWeight", &puweight, "PUWeight/F");
    // Looping events
    int events = FullMgr().CheckOption( "test" ) ? 10000 : FullMgr().GetEntries();

    for( int i = 0; i < events; i++ ){
    
        LJetPt1 = -999;
        LJetPt2 = -999;
        LJetPt3 = -999;
        LJetPt4 = -999;
        LLepPt1 = -999;
        LLepPt2 = -999;
        LLepPt3 = -999;
        LLepPt4 = -999;
        
        FullMgr().GetEntry( i );
        FullMgr().process( events, i );

        vector<int> lepidx;// store one tight lepton
        vector<int> jetidx;// store every jets
        vector<int> bjetidx;// store two bjets

        puweight = FullMgr().GetPUWeight();

        LJetPt1 = FullMgr().LJetPt();
        LLepPt1 = FullMgr().LLepPt();
        /*******************************************************************************
        *  Baseline selection
        *******************************************************************************/
    
        if( !FullMgr().PassHLT( hlt ) ){
            newtree->Fill();
            continue;
        }
   
        LJetPt2 = FullMgr().LJetPt();
        LLepPt2 = FullMgr().LLepPt();
        /*******************************************************************************
        *  Lepton selection
        *******************************************************************************/
        if( FullMgr().GetOption<string>( "lepton" ) == "el" ){
            if( !FullMgr().PassFullEl( lepidx ) ){
                newtree->Fill();
                continue;
            }
        }

        else if( FullMgr().GetOption<string>( "lepton" ) == "mu" ) {
            if( !FullMgr().PassFullMu( lepidx ) ){
                newtree->Fill();
                continue;
            }
        }

        else{
            cout<<"[Warning] Should have assigned lepton type"<<endl;
            newtree->Fill();
            return;
        }

        LJetPt3 = FullMgr().LJetPt();
        LLepPt3 = FullMgr().LLepPt();
        /*******************************************************************************
        *  Jet selection
        *******************************************************************************/
        if( !FullMgr().PassFullCRJet( jetidx, bjetidx, lepidx[ 0 ] ) ){
            newtree->Fill();
            continue;
        }

        LJetPt4 = FullMgr().LJetPt();
        LLepPt4 = FullMgr().LLepPt();

        newtree->Fill();
    }

    cout << endl;
    newtree->Write();
    newfile->Close();
    delete newfile;
}

extern void
FullCut( bool is_data )
{
    // Build new file
    TFile* newfile = TFile::Open( ( FullMgr().GetResultsName( "root", "FullCut" ) ).c_str(), "recreate" );
    TTree* newtree = FullMgr().CloneTree();
    
    // Initialize data
    string lepton = FullMgr().GetOption<string>("lepton");
    vector<int> hlt = is_data ? FullMgr().GetListData<int>( lepton + "_data_HLT" ) : FullMgr().GetListData<int>( lepton + "_mc_HLT" );
    
    //Reading JES txt file
    FullMgr().InitJES();

    // Register new branch
    Int_t had_b;
    Int_t lep_b;
    Int_t lep;
    Int_t jet1;
    Int_t jet2; 
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;

    newtree->Branch( "had_b",     &had_b,     "had_b/I" );
    newtree->Branch( "lep_b",     &lep_b,     "lep_b/I" );
    newtree->Branch( "jet1",      &jet1,      "jet1/I" );
    newtree->Branch( "jet2",      &jet2,      "jet2/I" );
    newtree->Branch( "lep",       &lep,       "lep/I" );
    newtree->Branch( "chi2mass",  &chi2mass,  "chimass/F" );
    newtree->Branch( "had_tmass", &had_tmass, "had_tmass/F" );
    newtree->Branch( "lep_tmass", &lep_tmass, "lep_tmass/F" );

    // Looping events
    int events = FullMgr().CheckOption( "test" ) ? 10000 : FullMgr().GetEntries();

    for( int i = 0; i < events; i++ ){
        FullMgr().GetEntry( i );
        FullMgr().process( events, i );

        vector<int> lepidx;// store one tight lepton
        vector<int> jetidx;// store every jets
        vector<int> bjetidx;// store two bjets

        // JERCorr
        if( !is_data ){
            if( FullMgr().GetOption<string>( "uncertainty" ) == "JER_up" ){
                FullMgr().JERCorrUp();
            }
            else if( FullMgr().GetOption<string>( "uncertainty" ) == "JER_dn" ){
                FullMgr().JERCorrDn();
            }
            else{
                FullMgr().JERCorr();
            }
        }
        
        // JECCorr
        if( !is_data ){
            if( FullMgr().GetOption<string>( "uncertainty" ) == "JEC_up" ){
                FullMgr().JECCorrUp();
            }
            else if( FullMgr().GetOption<string>( "uncertainty" ) == "JEC_dn" ){
                FullMgr().JECCorrDn();
            }
            else{
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
        if( FullMgr().OptionContent( "lepton", "el" ) ){
            if( FullMgr().OptionContent( "region", "0bjet" ) ){
                if( !FullMgr().PassFullCREl( lepidx ) ){
                    continue;
                }
            }
            else{
                if( !FullMgr().PassFullEl( lepidx ) ){
                    continue;
                }
            }
        }

        else if( FullMgr().OptionContent( "lepton", "mu" ) ){
            if( FullMgr().OptionContent( "region", "0bjet" ) ){
                if( !FullMgr().PassFullCRMu( lepidx ) ){
                    continue;
                }
            }
            else{
                if( !FullMgr().PassFullMu( lepidx ) ){
                    continue;
                }
            }
        }

        else{
            cout<<endl<<"[Warning] Should have assigned lepton type"<<endl;
            return;
        }

        /*******************************************************************************
        *  Jet selection
        *******************************************************************************/
        if( FullMgr().OptionContent( "region", "0bjet" ) ){
            if( !FullMgr().PassFullCRJet( jetidx, bjetidx, lepidx[ 0 ] ) ){
                continue;
            }
        }
        else if( FullMgr().OptionContent( "region", "CSVL" ) ){
            if( !FullMgr().PassFullCR2Jet( jetidx, bjetidx, lepidx[ 0 ] ) ){
                continue;
            }
        }
        else{
            if( !FullMgr().PassFullJet( jetidx, bjetidx, lepidx[ 0 ] ) ){
                continue;
            }
        }

        /*******************************************************************************
        *  Chi2 sorting
        *******************************************************************************/
        auto tup = FullMgr().GetChi2Info( jetidx, bjetidx );

        /*******************************************************************************
        *  Storing sample
        *******************************************************************************/
        chi2mass = (Float_t)get<0>( tup );
        had_tmass = (Float_t)get<1>( tup );
        had_b = get<2>( tup );
        jet1  = get<3>( tup );
        jet2  = get<4>( tup );
        lep_b = had_b ? 0 : 1;

        had_b = bjetidx[ had_b ];
        lep_b = bjetidx[ lep_b ];
        jet1  = jetidx [ jet1  ];
        jet2  = jetidx [ jet2  ];
        lep   = lepidx[ 0 ]; // choose leading lepton

        lep_tmass = FullMgr().GetLeptonicM( lep, lep_b );

        newtree->Fill();
    }

    cout << endl;
    newtree->AutoSave();
    newfile->Close();
    delete newfile;
}
