#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include <math.h>
#include "THStack.h"
#include "TRandom3.h"
#include "regex"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp> 

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
    CompMgr().Hist( obs + "_dist" )->Fill( acp / ( 172.5 * 172.5 * 172.5 ), weight );
}

extern void 
FillObservableRatio( const string& obs, const double& mc, const double& reco, const double& weight )
{
    double val = mc * reco > 0 ? 0.5 : -0.5;
    CompMgr().Hist( obs )->Fill( val, weight );
}

extern string
MakeFileName( const string& sample, const string& lepton, const string& year, const string& region, const string& unc )
{
    string reg = ( unc.find( "JE" ) != string::npos || unc.find( "FR" ) != string::npos ) ? "_" + unc : "";
    if( !region.empty() ){
        reg += "_region_" + region;
    }

    if( CompMgr().CheckOption( "topmass" ) ){
        reg += "_topmass_" + CompMgr().GetOption<string>( "topmass" );
    }

    boost::format filename( CompMgr().GetParam<string>( sample, "path" ) );
    filename % year % lepton % "_[0-9]+" % reg;
    return filename.str();
}

extern void
MakeSampleName( string& sample, const string& unc )
{
    if( unc.find( "CP5" ) != string::npos || unc.find( "hdamp" ) != string::npos || unc.find( "mtop" ) != string::npos ){
        sample += "_" + unc; 
    }
}

extern double
SetEventWeight( const string& unc, const string& tag, const double& nom, const double& up, const double& dn )
{
    if( unc.find( tag ) != string::npos ){
        double weight_unc = unc.find( "_up" ) != string::npos ? up : dn;
        return weight_unc;
    }
    else{
        return nom;
    }
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

    MakeSampleName( sample, unc );
    string filename = MakeFileName( sample, lepton, year, region, unc );
    cout << ">> Adding " << filename << endl;

    TChain* ch = new TChain( "root" );
    ch->Add( filename.c_str() );
    CompMgr().AddSample( ch, is_data, sample );
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
    Float_t bc_weight_up;
    Float_t bc_weight_dn;
    Float_t light_weight_up;
    Float_t light_weight_dn;
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
    ch->SetBranchAddress( "bc_weight_up",    &bc_weight_up );
    ch->SetBranchAddress( "bc_weight_dn",    &bc_weight_dn );
    ch->SetBranchAddress( "light_weight_up", &light_weight_up );
    ch->SetBranchAddress( "light_weight_dn", &light_weight_dn );
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
        vector<string> split_result;
        boost::split( split_result, unc, boost::is_any_of("_") ); 
        double shift = unc.find( "resolution" ) != string::npos ? 1 + stod(split_result[1])/100. : 1.;
        if( CompMgr().CheckOption( "chi2" ) ){
            if( ( chi2mass / (shift*shift) ) > CompMgr().GetOption<double>( "chi2" ) ){
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
            // pile-up reweighting
            if( !CompMgr().CheckOption( "wopileup" ) ){
                weight *= SetEventWeight( unc, "pileup", CompMgr().GetPUWeight(), CompMgr().GetPUWeightUp(), CompMgr().GetPUWeightDn() );
            }
            // b-tagging
            if( !CompMgr().CheckOption( "wobtag" ) ){
                if( unc.find( "bctag" ) != string::npos ){ 
                    weight *= SetEventWeight( unc, "bctag", b_weight, bc_weight_up, bc_weight_dn );
                }
                else{
                    weight *= SetEventWeight( unc, "lighttag", b_weight, light_weight_up, light_weight_dn );
                }
            }

            // lepton ID, RECO and ISO re-weighting
            weight *= SetEventWeight( unc, "lepton", CompMgr().GetLepSF( LepID, lep ),   CompMgr().GetLepSFUp( LepID, lep ),   CompMgr().GetLepSFDn( LepID, lep ) );
            weight *= SetEventWeight( unc, "lepton", CompMgr().GetLepSF( LepHLT, lep ),  CompMgr().GetLepSFUp( LepHLT, lep ),  CompMgr().GetLepSFDn( LepHLT, lep ) );
            weight *= SetEventWeight( unc, "lepton", CompMgr().GetLepSF( LepRECO, lep ), CompMgr().GetLepSFUp( LepRECO, lep ), CompMgr().GetLepSFDn( LepRECO, lep ) );

            // PDF unc
            if( unc.find( "PDF" ) != string::npos ){
                double pdf_nom = CompMgr().PDFWeight( 0 );
                double pdf_sig = 0.;
                for( int i = 10; i < 110; i++ ){
                    pdf_sig += pow( CompMgr().PDFWeight( i ) - pdf_nom, 2 ); 
            
                }
                double alpha_sig = pow( ( CompMgr().PDFWeight( 111 ) - CompMgr().PDFWeight( 110 ) ) / 2., 2 );
                double sig = sqrt( pdf_sig + alpha_sig );
                weight *= SetEventWeight( unc, "PDF", pdf_nom, pdf_nom + sig, pdf_nom - sig ); 
            }
            
            // muFmuR unc
            if( unc.find( "muFmuR" ) != string::npos ){
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
                weight *= SetEventWeight( unc, "muFmuR", 1, mu_weight_up, mu_weight_dn );
            }

            // gen-level re-weighting
            if( unc.find( "ISR" ) != string::npos ){
                weight *= SetEventWeight( unc, "ISR", 1, CompMgr().GetISRup(), CompMgr().GetISRdn() );
            }
            else if( unc.find( "FSR" ) != string::npos ){
                weight *= SetEventWeight( unc, "FSR", 1, CompMgr().GetFSRup(), CompMgr().GetFSRdn() );
            }
            else{
                weight *= CompMgr().GenWeight();
            }
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
        double o14 = Obs14( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect() );

        TVector3 bbCM = -( b + bbar ).BoostVector();
        b.Boost( bbCM );
        bbar.Boost( bbCM );
        isolep.Boost( bbCM );
        hardjet.Boost( bbCM );

        double o3 = Obs3( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );

        FillObservable( "Obs3",  o3,  weight );
        FillObservable( "Obs6",  o6,  weight );
        FillObservable( "Obs12", o12, weight );
        FillObservable( "Obs14", o14, weight );
        CompMgr().Hist2D( "Obs3_leptmass" ) ->Fill( lep_tmass, o3  / ( 172.5 * 172.5 * 172.5 ), weight );
        CompMgr().Hist2D( "Obs6_leptmass" ) ->Fill( lep_tmass, o6  / ( 172.5 * 172.5 * 172.5 ), weight );
        CompMgr().Hist2D( "Obs12_leptmass" )->Fill( lep_tmass, o12 / ( 172.5 * 172.5 * 172.5 ), weight );
        CompMgr().Hist2D( "Obs14_leptmass" )->Fill( lep_tmass, o14 / ( 172.5 * 172.5 * 172.5 ), weight );

        /*******************************************************************************
        *  bbSeparation / Optimisation
        *******************************************************************************/
        double jj_mass = ( CompMgr().GetJetP4( jet1 ) + CompMgr().GetJetP4( jet2 ) ).M(); 
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
            CompMgr().Hist2D( "chi2_jjmass" )->Fill( jj_mass, chi2mass, weight );
            
            /*******************************************************************************
            *  Gen Acp
            *******************************************************************************/
            int genbidx    = CompMgr().GetGenJet( 5 );
            int genbbaridx = CompMgr().GetGenJet( -5 );
            int genlepidx  = CompMgr().GetGenLepton();
            int genjetidx  = CompMgr().GetGenHardJet();
            // non-matched gen-level
            if( genbidx == -1 || genbbaridx == -1 || genlepidx == -1 || genjetidx == -1 ){
                continue;
            }
            float mccharge = CompMgr().GetPdgID( genlepidx ) > 0 ? -1. : 1.;
            TLorentzVector mcb = CompMgr().GetGenP4( genbidx );
            TLorentzVector mcbbar = CompMgr().GetGenP4( genbbaridx );
            TLorentzVector mcisolep = CompMgr().GetGenP4( genlepidx );
            TLorentzVector mchardjet = CompMgr().GetGenP4( genjetidx );
            
            // In Lab frame
            double mco14 = Obs14( mcisolep.Vect(), mchardjet.Vect(), mcb.Vect(), mcbbar.Vect() );
            double mco6  = Obs6( mcisolep.Vect(), mchardjet.Vect(), mcb.Vect(), mcbbar.Vect(), mccharge );
            double mco12 = Obs12( mcb.Vect(), mcbbar.Vect() );

            // In bbar CM frame
            TVector3 mcbbCM = -( mcb + mcbbar ).BoostVector();
            mcb.Boost( mcbbCM );
            mcbbar.Boost( mcbbCM );
            mcisolep.Boost( mcbbCM );
            mchardjet.Boost( mcbbCM );

            double mco3 = Obs3( mcisolep.Vect(), mchardjet.Vect(), mcb.Vect(), mcbbar.Vect(), mccharge );

            FillObservable( "GenObs3",  mco3,  weight );
            FillObservable( "GenObs6",  mco6,  weight );
            FillObservable( "GenObs12", mco12, weight );
            FillObservable( "GenObs14", mco14, weight );

            FillObservableRatio( "Ratio_Obs3", mco3, o3, weight );
            FillObservableRatio( "Ratio_Obs6", mco6, o6, weight );
            FillObservableRatio( "Ratio_Obs12", mco12, o12, weight );
            FillObservableRatio( "Ratio_Obs14", mco14, o14, weight );
        }

        /*******************************************************************************
        *  Event filling
        *******************************************************************************/
        CompMgr().Hist( "Syst" )->Fill( 0., weight );
        CompMgr().Hist( "had_tmass" )->Fill( had_tmass, weight );
        CompMgr().Hist( "lep_tmass" )->Fill( lep_tmass, weight );
        CompMgr().Hist( "leptmass" )->Fill( lep_tmass, weight );
        CompMgr().Hist( "jj_tmass" )->Fill( jj_mass, weight ); 
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

        /*******************************************************************************
        * Selected jet info filling
        *******************************************************************************/
        //cout<<CompMgr().GetJetCSV( had_b )<<endl;
        /* if( !is_data ){*/
            //CompMgr().Hist( "bweight" )->Fill( b_weight );
        //}
        sort( jetidx->begin(), jetidx->end() );
        CompMgr().Hist( "DeepCSV" )->Fill( CompMgr().GetJetCSV( had_b ), weight );
        CompMgr().Hist( "DeepCSV" )->Fill( CompMgr().GetJetCSV( lep_b ), weight );
        for( size_t i = 0; i < jetidx->size(); i++ ){
            CompMgr().Hist( "DeepCSV" )->Fill( CompMgr().GetJetCSV( jetidx->at( i ) ), weight );
            //if( !is_data ){
                //CompMgr().Hist( "JetFlavor" )->Fill( CompMgr().GetJetFlavor( fabs( jetidx->at( i ) ) ), weight );
            //}
            //if( i >= 7 ){
                //continue;
            //}
            //string jid = to_string( i );
            //CompMgr().Hist( "LJetPt" + jid )->Fill( CompMgr().GetJetPt( jetidx->at( i ) ), weight );
            //CompMgr().Hist( "LJetEta" + jid )->Fill( CompMgr().GetJetEta( jetidx->at( i ) ), weight );
        }
    }

    cout << endl;

    if( !is_data ){
        CompMgr().WeightMC( sample );
        cout << ">>Weighting " << sample << endl;
    }
    StoreCompare();
    delete ch;
}
