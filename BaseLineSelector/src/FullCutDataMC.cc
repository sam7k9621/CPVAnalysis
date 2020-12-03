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
    string pos  = FullMgr().IsLxplus() ? "/eos/cms/store/user/pusheng/public/PreCut" : FullMgr().ResultsDir();
    string year = FullMgr().GetOption<string>( "year" );

    if( is_data ){
        string lepton = FullMgr().GetOption<string>( "lepton" );

        if( lepton == "el" ){
            return pos / ( "PreCut_" + year + "_*E*_" + FullMgr().GetOption<string>( "sample" ) + ".root" );
            // /eos/cms/store/user/pusheng/files/PreCut_mu_runG_2.root
        }
        else if( lepton == "mu" ){
            return pos / ( "PreCut_" + year + "_*M*_" + FullMgr().GetOption<string>( "sample" ) + ".root" );
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
    string year = FullMgr().GetOption<string>( "year" );
    FullMgr().InitRoot( "sample" + year );
    TFile* newfile = TFile::Open( ( FullMgr().GetResultsName( "root", "FullCut", FullMgr().CheckOption( "uncertainty" ) ) ).c_str(), "recreate" );

    string sample =  FullMgr().GetOption<string>( "sample" );
    bool is_data  = sample.find( "Run" ) != std::string::npos ? 1 : 0;
    string filename   = MakeFileName( is_data );
    cout << ">> Adding " << filename << endl;

    TChain* ch = new TChain( "root" );
    ch->Add( ( filename ).c_str() );
    FullMgr().AddSample( ch, is_data, sample );
    
    // Discard useless branches
    TTree* newtree = ch->CloneTree( 0 );
    Discard_FullCut( newtree );
    if( FullMgr().OptionContent( "sample", "TT" ) && !FullMgr().CheckOption( "region" ) ){
        newtree->SetBranchStatus( "GenInfo.*",    1 );
        newtree->SetBranchStatus( "GenInfo.PDF*", 0 );
    }

    // Initialize data
    string lepton   = FullMgr().GetOption<string>( "lepton" );
    vector<int> hlt = FullMgr().GetVParam<int>( "Info", lepton + "_HLT" );
    string region   = FullMgr().OptionContent( "region", "0b" ) ? "CR" : "SR";

    TH2D* beff      = FullMgr().GetSFHist( region + "_eff_b" );
    TH2D* ceff      = FullMgr().GetSFHist( region + "_eff_c" );
    TH2D* leff      = FullMgr().GetSFHist( region + "_eff_l" );
    FullMgr().InitBtagWeight( "bcheck", FullMgr().GetParam<string>( "BtagWeight", "path" ) );
    FullMgr().InitBtagEffPlot( beff, ceff, leff );

    // Register new branch
    Int_t had_b;
    Int_t lep_b;
    Int_t lep;
    Int_t jet1;
    Int_t jet2;
    Int_t Njets;
    Int_t isWHF;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;
    Float_t b_weight;
    Float_t bc_weight_up;
    Float_t bc_weight_dn;
    Float_t light_weight_up;
    Float_t light_weight_dn;
    vector<int> lepidx;// store one tight lepton
    vector<int> jetidx;// store every jets
    vector<int> bjetidx;// store two bjets

    newtree->Branch( "had_b",       &had_b,       "had_b/I" );
    newtree->Branch( "lep_b",       &lep_b,       "lep_b/I" );
    newtree->Branch( "lep",         &lep,         "lep/I" );
    newtree->Branch( "jet1",        &jet1,        "jet1/I" );
    newtree->Branch( "jet2",        &jet2,        "jet2/I" );
    newtree->Branch( "Njets",       &Njets,       "Njets/I" );
    newtree->Branch( "isWHF",       &isWHF,       "isWHF/I" );
    newtree->Branch( "chi2mass",    &chi2mass,    "chimass/F" );
    newtree->Branch( "had_tmass",   &had_tmass,   "had_tmass/F" );
    newtree->Branch( "lep_tmass",   &lep_tmass,   "lep_tmass/F" );
    newtree->Branch( "b_weight",    &b_weight,    "b_weight/F" );
    newtree->Branch( "bc_weight_up", &bc_weight_up, "bc_weight_up/F" );
    newtree->Branch( "bc_weight_dn", &bc_weight_dn, "bc_weight_dn/F" );
    newtree->Branch( "light_weight_up", &light_weight_up, "light_weight_up/F" );
    newtree->Branch( "light_weight_dn", &light_weight_dn, "light_weight_dn/F" );
    newtree->Branch( "jetidx",      &jetidx );

    // Looping events
    cout << ">> Processing " << filename << endl;
    string tag = FullMgr().GetOption<string>( "uncertainty" );
    int events = FullMgr().CheckOption( "test" ) ? 100 : ch->GetEntries();

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        FullMgr().process( events, i );

        lepidx.clear();
        jetidx.clear();
        bjetidx.clear();

        // Determine WHF or not 
        isWHF = sample.find("WJets") != std::string::npos ? (int)FullMgr().IsWHF() : 0;

        // JEC JER update JEC central value didn't change
        if( is_data ){
            FullMgr().JECUpdate();
        }
        else{
            FullMgr().JECUncUpdate();
            if( FullMgr().OptionContent( "uncertainty", "FR" ) ) FullMgr().JECUncSrcUpdate();
        }
        FullMgr().JERUpdate();

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

        if( !is_data ){
            if( tag == "FR_up" ){
                FullMgr().JECCorrUp();
            }
            else if( tag == "FR_dn" ){
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
            if ( FullMgr().OptionContent( "region", "worelax" ) ){
                if( !FullMgr().PassFullLepton( lepidx, lepton ) ){
                    continue;
                }
            }
            else{
                if( !FullMgr().PassFullLepton_CRWJets( lepidx, lepton ) ){
                    continue;
                }
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
        if( FullMgr().OptionContent( "region", "1b" ) ){
            if( !FullMgr().PassFullJet_CR_1b( jetidx, bjetidx, lepidx[ 0 ] ) ){
                continue;
            }
        } 
        else if( FullMgr().OptionContent( "region", "0b" ) ){
            if( !FullMgr().PassFullJet_CR_0b( jetidx, lepidx[ 0 ] ) ){
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
            if( FullMgr().OptionContent( "region", "1b" ) ){
                b_weight = FullMgr().GetBtagWeight_CR_1b( bjetidx, jetidx );
            }
            else if( FullMgr().OptionContent( "region", "0b" ) ){
                b_weight = FullMgr().GetBtagWeight_CR_0b( jetidx );
            }
            else{
                b_weight        = FullMgr().GetBtagWeight( bjetidx, jetidx );
                bc_weight_up    = FullMgr().GetBtagWeight( bjetidx, jetidx, "bc_up" );
                bc_weight_dn    = FullMgr().GetBtagWeight( bjetidx, jetidx, "bc_down" );
                light_weight_up = FullMgr().GetBtagWeight( bjetidx, jetidx, "light_up" );
                light_weight_dn = FullMgr().GetBtagWeight( bjetidx, jetidx, "light_down" );
            }
        }

        /*******************************************************************************
        *  Chi2 sorting
        *******************************************************************************/
        std::tuple<double, double, int, int, int> tup;
        if( FullMgr().OptionContent( "region", "0b" ) ){
            tup = FullMgr().GetChi2Info_CR_0b( jetidx,  bjetidx, lepidx[0] );
        }
        else{
            tup = FullMgr().GetChi2Info( jetidx, bjetidx );
        }

        /*******************************************************************************
        *  Storing sample
        *******************************************************************************/
        chi2mass  = (Float_t)get<0>( tup );
        had_tmass = (Float_t)get<1>( tup );
        had_b     = bjetidx[ get<2>( tup ) ];
        lep_b     = bjetidx[ get<2>( tup ) ? 0 : 1 ];
        jet1      = get<3>( tup );
        jet2      = get<4>( tup );
        lep       = lepidx[ 0 ]; // choose leading lepton

        Njets = jetidx.size() + bjetidx.size();

        lep_tmass = FullMgr().GetLeptonicM( lep, lep_b );
        newtree->Fill();
    }

    cout << endl;
    newtree->AutoSave();
    delete ch;
    delete newfile;
    cout<<">> Storing "<<FullMgr().GetResultsName( "root", "FullCut", "FullCut" )<<endl;
}
