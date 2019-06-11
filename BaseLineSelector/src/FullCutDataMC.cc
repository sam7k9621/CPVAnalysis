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
    string pos      = "/eos/cms/store/user/pusheng/2017/";
    string filename = "";

    if( is_data ){
        filename = pos / ( "PreCut_" + FullMgr().GetOption<string>( "lepton" ) + "_" +  FullMgr().GetOption<string>( "sample" ) + ".root" );
    }
    // /eos/cms/store/user/pusheng/files/PreCut_mu_runG_2.root
    else{
        filename = pos / ( "PreCut_" + FullMgr().GetOption<string>( "sample" ) + ".root" );
    }
    // /eos/cms/store/user/pusheng/files/PreCut_TTbar_7.root

    return filename;
}

extern void
MakeFullCut()
{
    // Build new file
    TFile* newfile = TFile::Open( ( FullMgr().GetResultsName( "root", "FullCut" ) ).c_str(), "recreate" );

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
    vector<int> hlt = FullMgr().GetListData<int>( lepton + "_HLT" );

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
    int events = FullMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
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
            cout << endl << "[Warning] Should have assigned lepton type" << endl;
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
