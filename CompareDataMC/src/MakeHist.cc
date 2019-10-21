#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
#include <boost/format.hpp>

using namespace std;

extern Histor&
CompMgr( const string& subdir, const string& json )
{
    static Histor mgr( subdir, json );
    return mgr;
}

extern void 
FillObservable( const string& obs, const double& acp, const double& weight )
{
    double val = acp > 0 ? 0.5 : -0.5;
    CompMgr().Hist( obs )->Fill( val, weight );
}

extern string 
MakeFileName( const string& sample, const string& lepton, const string& year, const string& region )
{
    string reg     = region.empty() ? "" : "_region_" + region;

    boost::format filename( CompMgr().GetParam<string>( sample, "path" ) );
    filename % year % lepton % "_[0-9]+" % reg;
    return filename.str();
}

extern void
MakeHist()
{
    // Initialize file
    string sample   = CompMgr().GetOption<string>( "sample" );
    string lepton   = CompMgr().GetOption<string>( "lepton" );
    string region   = CompMgr().GetOption<string>( "region" ); 
    string year     = CompMgr().GetOption<string>( "year" ); 
    bool is_data    = ( sample == "Data" ) ? 1 : 0;
    CompMgr().InitRoot( "sample" + year );
    
    string filename = MakeFileName( sample, lepton, year, region );
    cout << ">> Processing " << filename << endl;
    
    TChain* ch = new TChain( "root" );
    ch->Add( filename.c_str() );
    CompMgr().AddSample( sample, ch );
    AddHist();
    CompMgr().RegisterWeight( ch );

    // Register reco sample
    Int_t jet1;
    Int_t jet2;
    Int_t had_b;
    Int_t lep_b;
    Int_t lep;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;
    Float_t b_weight;
    Float_t b_weight_up;
    Float_t b_weight_dn;

    ch->SetBranchAddress( "jet1",      &jet1 );
    ch->SetBranchAddress( "jet2",      &jet2 );
    ch->SetBranchAddress( "lep",       &lep );
    ch->SetBranchAddress( "had_b",     &had_b );
    ch->SetBranchAddress( "lep_b",     &lep_b );
    ch->SetBranchAddress( "chi2mass",  &chi2mass );
    ch->SetBranchAddress( "had_tmass", &had_tmass );
    ch->SetBranchAddress( "lep_tmass", &lep_tmass );
    ch->SetBranchAddress( "b_weight", &b_weight );
    ch->SetBranchAddress( "b_weight_up", &b_weight_up );
    ch->SetBranchAddress( "b_weight_dn", &b_weight_dn );

    TH2D* LepID   = CompMgr().GetSFHist( lepton + "ID" );
    TH2D* LepRECO = CompMgr().GetSFHist( lepton + "RECO" );
    //TH2D* LepHLT  = CompMgr().GetSFHist( lepton + "HLT" );

    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        CompMgr().process( events, i );
        
        if( CompMgr().CheckOption( "0bjet" ) && CompMgr().HasLooseB( { had_b, lep_b } ) ){
            continue;
        }

        /*******************************************************************************
        * Chi2 minimum upper limit
        *******************************************************************************/
        if( CompMgr().CheckOption( "chi2" ) ){
            if( chi2mass > CompMgr().GetOption<double>( "chi2" ) ){
                continue;
            }
        }
        else if( CompMgr().CheckOption( "invChi2" ) ){
            if( chi2mass <= CompMgr().GetOption<double>( "invChi2" ) ){
                continue;
            }
        }
        
        /*******************************************************************************
        * Leptonic tmass optimization cut
        *******************************************************************************/
        if( CompMgr().CheckOption( "opt" ) ){
            if( lep_tmass > CompMgr().GetOption<double>( "opt" ) ){
                continue;
            }
        }

        /*******************************************************************************
        *  Event weighting
        *******************************************************************************/
        double weight = 1.0;
        if( !is_data ){
            string tag = CompMgr().CheckOption( "uncertainty" ) ? CompMgr().GetOption<string>( "uncertainty" ) : "";

            // pile-up reweighting
            if( !CompMgr().CheckOption( "wopileup" ) ){
                if( tag.find( "pileup" ) != string::npos ){
                    if( tag.find( "pileup_up" ) != string::npos ){
                        weight *= CompMgr().GetPUWeightUp();
                    }
                    else if( tag.find( "pileup_dn" ) != string::npos ){
                        weight *= CompMgr().GetPUWeightDn();
                    }
                }
                else{
                    weight *= CompMgr().GetPUWeight();
                }
            }

            // b-tagging 
            if( tag.find( "btag" ) != string::npos ){
                if( tag.find( "up" ) != string::npos ){
                    weight *= b_weight_up;
                }
                else if( tag.find( "dn" ) != string::npos ){
                    weight *= b_weight_dn;
                }
            }
            else{
                weight *= b_weight;
            }
            
            // lepton ID, RECO and ISO re-weighting
            if( tag.find( "lepton" ) != string::npos ){
                if( tag.find( "up" ) != string::npos ){
                    weight *= CompMgr().GetLepSFUp( LepID, lep );
                    weight *= CompMgr().GetLepSFUp( LepRECO, lep );
                    //weight *= CompMgr().GetLepSFUp( LepHLT, lep );
                }
                else if( tag.find( "dn" ) != string::npos ){
                    weight *= CompMgr().GetLepSFDn( LepID, lep );
                    weight *= CompMgr().GetLepSFDn( LepRECO, lep );
                    //weight *= CompMgr().GetLepSFDn( LepHLT, lep );
                }
            }
            else{
                weight *= CompMgr().GetLepSF( LepID, lep );
                weight *= CompMgr().GetLepSF( LepRECO, lep );
                //weight *= CompMgr().GetLepSF( LepHLT, lep );
            }

            // PDF unc 
            if( tag.find( "PDF" ) != string::npos ){
                int pdfidx = std::stoi( tag.substr( tag.find( "_" ) + 1 ) );
                weight *= CompMgr().PDFWeight( pdfidx );
            }
            
            // muFmuR unc 
            if( tag.find( "muFmuR" ) != string::npos ){
                int pdfidx = std::stoi( tag.substr( tag.find( "_" ) + 1 ) );
                weight *= CompMgr().muFmuRWeight( pdfidx );
            }

            // gen-level re-weighting
            weight *= CompMgr().GenWeight();
        }
       
        /*******************************************************************************
        *  bbSeparation / Optimisation
        *******************************************************************************/
        if( !is_data && CompMgr().CheckOption( "bbSep" ) ){
            BaseLineMgr::MatchType flag = CompMgr().bbSeparation( had_b, lep_b, lep );

            if( flag == BaseLineMgr::Correct ){
                CompMgr().Hist( "tmass_Correct" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Correct" )->Fill( chi2mass, weight );
                CompMgr().Hist( "Cor_leptmass" )->Fill( lep_tmass, weight );
            }
            else if( flag == BaseLineMgr::Misid ){
                CompMgr().Hist( "tmass_Misid" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Misid" )->Fill( chi2mass, weight );
                CompMgr().Hist( "Misid_leptmass" )->Fill( lep_tmass, weight );
            }
            else if( flag == BaseLineMgr::Mistag ){
                CompMgr().Hist( "tmass_Mistag" )->Fill( had_tmass, weight );
                CompMgr().Hist( "chi2_Mistag" )->Fill( chi2mass, weight );
                CompMgr().Hist( "Mistag_leptmass" )->Fill( lep_tmass, weight );
            }
            else{
                continue;
            }

            CompMgr().Hist2D( "chi2_tmass" )->Fill( had_tmass, chi2mass, weight );
        }

        /*******************************************************************************
        *  Event filling
        *******************************************************************************/
        CompMgr().Hist( "had_tmass" )->Fill( had_tmass, weight );
        CompMgr().Hist( "lep_tmass" )->Fill( lep_tmass, weight );
        CompMgr().Hist( "chi2" )->Fill( chi2mass, weight );
        CompMgr().Hist( "nVtx" )->Fill( CompMgr().nVtx(), weight );
        CompMgr().Hist( "Rho" )->Fill( CompMgr().Rho(), weight );


        CompMgr().Hist( "LJetPt" )->Fill( CompMgr().LeadingJetPt( jet1 ), weight );
        CompMgr().Hist( "LJetEta" )->Fill( CompMgr().LeadingJetEta( jet2 ), weight );
        CompMgr().Hist( "HBJetPt" )->Fill( CompMgr().LeadingJetPt( had_b ), weight );
        CompMgr().Hist( "HBJetEta" )->Fill( CompMgr().LeadingJetEta( had_b ), weight );
        CompMgr().Hist( "LBJetPt" )->Fill( CompMgr().LeadingJetPt( lep_b ), weight );
        CompMgr().Hist( "LBJetEta" )->Fill( CompMgr().LeadingJetEta( lep_b ), weight );
        CompMgr().Hist( "LepPt" )->Fill( CompMgr().GetLepPt( lep ), weight );
        CompMgr().Hist( "LepEta" )->Fill( CompMgr().GetLepEta( lep ), weight );
        
        if( lepton == "el" ){
            CompMgr().Hist( "LepIso" )->Fill( CompMgr().GetElISO( lep ), weight );
        }
        else if( lepton == "mu" ){
            CompMgr().Hist( "LepIso" )->Fill( CompMgr().GetMuISO( lep ), weight );
        }

        if( !is_data ){
            CompMgr().Hist2D( "chi2_tmass" )->Fill( had_tmass, chi2mass );
        }

    }
    cout<<endl;
    
    if( !is_data ){
        CompMgr().WeightMC( sample );
        cout << ">>Weighting " << sample << endl;
    }
    StoreCompare();

    delete ch;
}
