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
MakeFileName( const string& sample, const string& lepton, const string& year, const string& region, const string& unc )
{
    string reg = unc.find( "JE" ) != string::npos ? "_" + unc : "";
    if( !region.empty() ){
        reg += "_region_" + region;
    }

    boost::format filename( CompMgr().GetParam<string>( sample, "path" ) );
    filename % year % lepton % "_[0-9]+" % reg;
    return filename.str();
}

extern void
MakeHist()
{
    // Initialize file
    string sample = CompMgr().GetOption<string>( "sample" );
    string lepton = CompMgr().GetOption<string>( "lepton" );
    string region = CompMgr().GetOption<string>( "region" );
    string unc    = CompMgr().GetOption<string>( "uncertainty" );
    string year   = CompMgr().GetOption<string>( "year" );
    bool is_data  = ( sample == "Data" ) ? 1 : 0;
    CompMgr().InitRoot( "sample" + year );

    string filename = MakeFileName( sample, lepton, year, region, unc );
    cout << ">> Adding " << filename << endl;

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
    Int_t Njets;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;
    Float_t b_weight;
    Float_t b_weight_up;
    Float_t b_weight_dn;
    vector<int>* jetidx = 0;

    ch->SetBranchAddress( "jet1",        &jet1 );
    ch->SetBranchAddress( "jet2",        &jet2 );
    ch->SetBranchAddress( "lep",         &lep );
    ch->SetBranchAddress( "had_b",       &had_b );
    ch->SetBranchAddress( "lep_b",       &lep_b );
    ch->SetBranchAddress( "Njets",       &Njets );
    ch->SetBranchAddress( "chi2mass",    &chi2mass );
    ch->SetBranchAddress( "had_tmass",   &had_tmass );
    ch->SetBranchAddress( "lep_tmass",   &lep_tmass );
    ch->SetBranchAddress( "b_weight",    &b_weight );
    ch->SetBranchAddress( "b_weight_up", &b_weight_up );
    ch->SetBranchAddress( "b_weight_dn", &b_weight_dn );
    ch->SetBranchAddress( "jetidx",      &jetidx );

    TH2D* LepID   = CompMgr().GetSFHist( lepton + "ID" );
    TH2D* LepRECO = CompMgr().GetSFHist( lepton + "RECO" );
    TH2D* LepHLT  = CompMgr().GetSFHist( lepton + "HLT" );

    // Looping events
    cout << ">> Processing " << filename << endl;
    int events = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        CompMgr().process( events, i );

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
            string tag = unc;

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
            if( !CompMgr().CheckOption( "wobtag" ) ){
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
            }

            // lepton ID, RECO and ISO re-weighting
            if( tag.find( "lepton" ) != string::npos ){
                if( tag.find( "up" ) != string::npos ){
                    weight *= CompMgr().GetLepSFUp( LepID, lep );
                    weight *= CompMgr().GetLepSFUp( LepRECO, lep );
                    weight *= CompMgr().GetLepSFUp( LepHLT, lep );
                }
                else if( tag.find( "dn" ) != string::npos ){
                    weight *= CompMgr().GetLepSFDn( LepID, lep );
                    weight *= CompMgr().GetLepSFDn( LepRECO, lep );
                    weight *= CompMgr().GetLepSFDn( LepHLT, lep );
                }
            }
            else{
                weight *= CompMgr().GetLepSF( LepID, lep );
                weight *= CompMgr().GetLepSF( LepRECO, lep );
                weight *= CompMgr().GetLepSF( LepHLT, lep );
            }

            // PDF unc
            if( tag.find( "PDF" ) != string::npos ){
                float pdf_weight_up = 1; 
                float pdf_weight_dn = 1; 
                for( int i = 10; i < 112; i++ ){
                    
                    if( CompMgr().PDFWeight( i ) > pdf_weight_up ){
                        pdf_weight_up = CompMgr().PDFWeight( i );
                    }
                    if( CompMgr().PDFWeight( i ) < pdf_weight_dn ){
                        pdf_weight_dn = CompMgr().PDFWeight( i );
                    }
                }
             
                if( tag.find( "up" ) != string::npos ){
                    weight *= pdf_weight_up;
                }
                else if( tag.find( "dn" ) != string::npos ){
                    weight *= pdf_weight_dn;
                }
            }

            // muFmuR unc
            if( tag.find( "muFmuR" ) != string::npos ){
                float mu_weight_up = 1;
                float mu_weight_dn = 1;
                for( int i = 1; i < 9; i++ ){
                    if( i == 5 || i == 7 ){
                        continue;
                    }
                    
                    if( CompMgr().PDFWeight( i ) > mu_weight_up ){
                        mu_weight_up = CompMgr().muFmuRWeight( i );
                    }
                    if( CompMgr().PDFWeight( i ) < mu_weight_dn ){
                        mu_weight_dn = CompMgr().muFmuRWeight( i );
                    }
                }
                if( tag.find( "up" ) != string::npos ){
                    weight *= mu_weight_up;
                }
                else if( tag.find( "dn" ) != string::npos ){
                    weight *= mu_weight_dn;
                }
            }

            // gen-level re-weighting
            weight *= CompMgr().GenWeight();
        }

        /*******************************************************************************
        *  Reco Acp
        *******************************************************************************/
        float charge           = CompMgr().GetIsoLepCharge( lep );
        TLorentzVector isolep  = CompMgr().GetLepP4( lep );
        TLorentzVector hardjet = CompMgr().GetJetP4( jet1 );
        TLorentzVector b       = charge < 0 ? CompMgr().GetJetP4( had_b ) : CompMgr().GetJetP4( lep_b );
        TLorentzVector bbar    = charge < 0 ? CompMgr().GetJetP4( lep_b ) : CompMgr().GetJetP4( had_b );

        double o6  = Obs6( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
        double o12 = Obs12( b.Vect(), bbar.Vect() );
        double o13 = Obs13( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );

        TVector3 bbCM = -( b + bbar ).BoostVector();
        b.Boost( bbCM );
        bbar.Boost( bbCM );
        isolep.Boost( bbCM );
        hardjet.Boost( bbCM );

        double o3 = Obs3( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );

        FillObservable( "Obs3",  o3,  weight );
        FillObservable( "Obs6",  o6,  weight );
        FillObservable( "Obs12", o12, weight );
        FillObservable( "Obs13", o13, weight );

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
        CompMgr().Hist( "Njets" )->Fill( Njets, weight );
        CompMgr().Hist( "chi2" )->Fill( chi2mass, weight );
        //CompMgr().Hist( "nVtx" )->Fill( CompMgr().nVtx(), weight );
        //CompMgr().Hist( "Rho" )->Fill( CompMgr().Rho(), weight );

        int ljet = std::min( std::min( lep_b, had_b ), jet1 );
        CompMgr().Hist( "LJetPt" )  ->Fill( CompMgr().GetJetPt ( ljet ),  weight );
        CompMgr().Hist( "LJetEta" ) ->Fill( CompMgr().GetJetEta( ljet ),  weight );
        CompMgr().Hist( "LJetCSV" ) ->Fill( CompMgr().GetJetCSV( ljet ),  weight );
        CompMgr().Hist( "HBJetPt" ) ->Fill( CompMgr().GetJetPt ( had_b ), weight );
        CompMgr().Hist( "HBJetEta" )->Fill( CompMgr().GetJetEta( had_b ), weight );
        CompMgr().Hist( "LBJetPt" ) ->Fill( CompMgr().GetJetPt ( lep_b ), weight );
        CompMgr().Hist( "LBJetEta" )->Fill( CompMgr().GetJetEta( lep_b ), weight );
        CompMgr().Hist( "LepPt" )   ->Fill( CompMgr().GetLepPt ( lep ),   weight );
        CompMgr().Hist( "LepEta" )  ->Fill( CompMgr().GetLepEta( lep ),   weight );

        if( lepton == "el" ){
            CompMgr().Hist( "LepIso" )->Fill( CompMgr().GetElISO( lep ), weight );
        }
        else if( lepton == "mu" ){
            CompMgr().Hist( "LepIso" )->Fill( CompMgr().GetMuISO( lep ), weight );
        }

        if( !is_data ){
            CompMgr().Hist2D( "chi2_tmass" )->Fill( had_tmass, chi2mass );
        }

        /*******************************************************************************
        * Selected jet info filling
        *******************************************************************************/
       /* if( !is_data ){*/
            //CompMgr().Hist( "bweight" )->Fill( b_weight );
        //}
        //sort( jetidx->begin(), jetidx->end() );
        //CompMgr().Hist( "DeepCSV" )->Fill( CompMgr().GetJetCSV( had_b ), weight );
        //CompMgr().Hist( "DeepCSV" )->Fill( CompMgr().GetJetCSV( lep_b ), weight );

        //for( size_t i = 0; i < jetidx->size(); i++ ){
            //CompMgr().Hist( "DeepCSV" )->Fill( CompMgr().GetJetCSV( jetidx->at( i ) ), weight );
            //if( !is_data ){
                //CompMgr().Hist( "JetFlavor" )->Fill( CompMgr().GetJetFlavor( fabs( jetidx->at( i ) ) ), weight );
            //}
            //if( i >= 7 ){
                //continue;
            //}
            //string jid = to_string( i );
            //CompMgr().Hist( "LJetPt" + jid )->Fill( CompMgr().GetJetPt( jetidx->at( i ) ), weight );
            //CompMgr().Hist( "LJetEta" + jid )->Fill( CompMgr().GetJetEta( jetidx->at( i ) ), weight );
        /*}*/
    }

    cout << endl;

    if( !is_data ){
        CompMgr().WeightMC( sample );
        cout << ">>Weighting " << sample << endl;
    }
    StoreCompare();
    delete ch;
}
