#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"


using namespace std;

extern Histor&
CompMgr( const string& subdir, const string& json )
{
    static Histor mgr( subdir, json );
    return mgr;
}

extern TH2D*
GetLepSF( const string& tag )
{
    return CompMgr().GetSFHist( CompMgr().GetOption<string>( "lepton" ) + tag );
}

extern void
MakeHist()
{
    // Initialize file
    string lepton            = CompMgr().GetOption<string>("lepton");
    string sample            = CompMgr().GetOption<string>( "sample" );
    vector<string> samplelst = CompMgr().GetSubListData<string>( sample, lepton + "path" );
    TChain* ch               = new TChain( "root" );
    
    for( const auto& s : samplelst ){
        ch->Add( s.c_str() );
    }

    CompMgr().AddSample( sample, ch );
    AddHist();
    
    CompMgr().RegisterWeight();
    CompMgr().InitBtagWeight( "bcheck", CompMgr().GetSingleData<string>( "BtagWeight" ) );

    // Register reco sample
    Int_t jet1;
    Int_t jet2;
    Int_t had_b;
    Int_t lep_b;
    Int_t lep;
    Float_t chi2mass;
    Float_t had_tmass;
    Float_t lep_tmass;

    ch->SetBranchAddress( "jet1", &jet1 );
    ch->SetBranchAddress( "jet2", &jet2 );
    ch->SetBranchAddress( "lep", &lep );
    ch->SetBranchAddress( "had_b", &had_b );
    ch->SetBranchAddress( "lep_b", &lep_b );
    ch->SetBranchAddress( "chi2mass",  &chi2mass );
    ch->SetBranchAddress( "had_tmass",  &had_tmass );
    ch->SetBranchAddress( "lep_tmass",  &lep_tmass );
    
    map<int, TLorentzVector> reco_b;
    map<int, TLorentzVector> reco_bbar;
    map<int, TLorentzVector> reco_lep;
    map<int, TLorentzVector> reco_jet;
    map<int, int>            reco_charge;

    TH2D* LepID   = GetLepSF( "ID" ); 
    TH2D* LepRECO = GetLepSF( "RECO" ); 
    TH2D* LepHLT  = GetLepSF( "Trg" ); 

    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 10000 : CompMgr().GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;

    for( int i = 0; i < events; i++ ){
        CompMgr().GetEntry( i );
        CompMgr().process( events, i );

        /*******************************************************************************
        * Chi2 minimum upper limit
        *******************************************************************************/
        if( CompMgr().CheckOption( "chi2" ) ){
            if( chi2mass < CompMgr().GetOption<double>( "chi2" ) ){
                continue;
            }
        }

        /*******************************************************************************
        * Leptonic tmass optimization cut
        *******************************************************************************/
        if( CompMgr().CheckOption( "Opt" ) ){
            if( lep_tmass > CompMgr().GetOption<double>( "Opt" ) ){
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
            
            // lepton ID, RECO and ISO re-weighting
            if( tag.find( "lepton" ) != string::npos ){
                if( tag.find( "up" ) != string::npos ){
                    weight *= CompMgr().GetSFUp( LepID, lep );
                    weight *= CompMgr().GetSFUp( LepRECO, lep );
                    weight *= CompMgr().GetSFUp( LepHLT, lep );
                }
                else if( tag.find( "dn" ) != string::npos ){
                    weight *= CompMgr().GetSFDn( LepID, lep );
                    weight *= CompMgr().GetSFDn( LepRECO, lep );
                    weight *= CompMgr().GetSFDn( LepHLT, lep );
                }
            }
            else{
                weight *= CompMgr().GetSF( LepID, lep );
                weight *= CompMgr().GetSF( LepRECO, lep );    
                weight *= CompMgr().GetSF( LepHLT, lep );    
            }
            
            // b-tagging re-weighting
            if( tag.find( "b-tag" ) != string::npos ){
                if( tag.find( "up" ) != string::npos ){
                    weight *= CompMgr().BtagScaleFactorUp( BTagEntry::OP_MEDIUM, had_b ) * CompMgr().BtagScaleFactorUp( BTagEntry::OP_MEDIUM, lep_b );
                }
                else if( tag.find( "dn" ) != string::npos ){
                    weight *= CompMgr().BtagScaleFactorDn( BTagEntry::OP_MEDIUM, had_b ) * CompMgr().BtagScaleFactorDn( BTagEntry::OP_MEDIUM, lep_b );
                }
            } 
            else{
                // avoid CR sample
                if( !CompMgr().CheckOption( "region" ) ){
                    weight *= CompMgr().BtagScaleFactor( BTagEntry::OP_MEDIUM, had_b ) * CompMgr().BtagScaleFactor( BTagEntry::OP_MEDIUM, lep_b );
                }
                else if ( CompMgr().GetOption<string>( "region") == "CSVL" ){
                    weight *= CompMgr().BtagScaleFactor( BTagEntry::OP_LOOSE, had_b ) * CompMgr().BtagScaleFactor( BTagEntry::OP_LOOSE, lep_b );
                }
            }
            
            if( sample == "TTbar" && tag.find( "TopPt_dn" ) == string::npos ){
                weight *= CompMgr().TopPtWeight();
            }

        }
        
        // gen-level re-weighting
        weight *=  CompMgr().GenWeight() < 0 ? -1 : 1;
        
        /*******************************************************************************
        *  Event filling
        *******************************************************************************/
        float nVtx = CompMgr().nVtx();

        CompMgr().Hist( "had_tmass" )->Fill( had_tmass, weight );
        CompMgr().Hist( "lep_tmass" )->Fill( lep_tmass, weight );
        CompMgr().Hist( "chi2" )->Fill( chi2mass, weight );
        CompMgr().Hist( "nVtx" )->Fill( nVtx, weight );

        CompMgr().Hist( "LJetPt"    )->Fill( CompMgr().LeadingJetPt ( jet1 ),  weight );
        CompMgr().Hist( "LJetEta"   )->Fill( CompMgr().LeadingJetEta( jet2 ), weight );
        CompMgr().Hist( "HBJetPt"   )->Fill( CompMgr().LeadingJetPt ( had_b ), weight );
        CompMgr().Hist( "HBJetEta"  )->Fill( CompMgr().LeadingJetEta( had_b ), weight );
        CompMgr().Hist( "LBJetPt"   )->Fill( CompMgr().LeadingJetPt ( lep_b ), weight );
        CompMgr().Hist( "LBJetEta"  )->Fill( CompMgr().LeadingJetEta( lep_b ), weight );
        CompMgr().Hist( "LepPt"     )->Fill( CompMgr().GetLepPt( lep ),  weight  );
        CompMgr().Hist( "LepEta"    )->Fill( CompMgr().GetLepEta( lep ), weight  );

        if( !is_data ){
            CompMgr().Hist2D( "chi2_tmass" )->Fill( had_tmass, chi2mass );
        }
        
        /*******************************************************************************
        *  Reco Acp
        *******************************************************************************/
        float charge           = CompMgr().GetIsoLepCharge( lep );
        TLorentzVector isolep  = CompMgr().GetLepP4( lep );
        TLorentzVector hardjet = CompMgr().GetJetP4( jet1 );
        TLorentzVector b       = charge < 0 ? CompMgr().GetJetP4( had_b ) : CompMgr().GetJetP4( lep_b );
        TLorentzVector bbar    = charge < 0 ? CompMgr().GetJetP4( lep_b ) : CompMgr().GetJetP4( had_b );

        // Fill in list
        reco_b   [ i ]    = b;
        reco_bbar[ i ]    = bbar;
        reco_lep [ i ]    = isolep ;
        reco_jet [ i ]    = hardjet ;
        reco_charge [ i ] = charge;
    }

    FillObserable( "Obs2", vector<int>(), reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );  
    FillObserable( "Obs3", vector<int>(), reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );  
    FillObserable( "Obs4", vector<int>(), reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );  
    FillObserable( "Obs7", vector<int>(), reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );  
    
    if( !is_data ){
        CompMgr().WeightMC( sample );
        cout << ">>Weighting " << sample << endl;
    }
    StoreCompare();

    delete ch;
}

extern void
CheckHist() 
{
    // Initialize file
    string lepton            = CompMgr().GetOption<string>("lepton");
    string sample            = CompMgr().GetOption<string>( "sample" );
    vector<string> samplelst = CompMgr().GetSubListData<string>( sample, lepton + "path" );
    TChain* ch               = new TChain( "root" );
    
    Float_t LJetPt1;
    Float_t LJetPt2;
    Float_t LJetPt3;
    Float_t LJetPt4;
    Float_t LLepPt1;
    Float_t LLepPt2;
    Float_t LLepPt3;
    Float_t LLepPt4;

    ch->SetBranchAddress( "LJetPt1", &LJetPt1 );
    ch->SetBranchAddress( "LJetPt2", &LJetPt2 );
    ch->SetBranchAddress( "LJetPt3", &LJetPt3 );
    ch->SetBranchAddress( "LJetPt4", &LJetPt4 );
    ch->SetBranchAddress( "LLepPt1", &LLepPt1 );
    ch->SetBranchAddress( "LLepPt2", &LLepPt2 );
    ch->SetBranchAddress( "LLepPt3", &LLepPt3 );
    ch->SetBranchAddress( "LLepPt4", &LLepPt4 );
    
    for( const auto& s : samplelst ){
        ch->Add( s.c_str() );
    }
    CompMgr().AddSample( sample, ch );
    AddHist();
    
    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 1000 : CompMgr().GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;
    
    for( int i = 0; i < events; i++ ){
        CompMgr().GetEntry( i );
        CompMgr().process( events, i );
    
        float weight = is_data ? 1 : CompMgr().GenWeight();
        weight = weight < 0 ? -1 : 1 ;
    
        CompMgr().Hist( "LJetPt1" )->Fill( LJetPt1, weight );
        CompMgr().Hist( "LJetPt2" )->Fill( LJetPt2, weight );
        CompMgr().Hist( "LJetPt3" )->Fill( LJetPt3, weight );
        CompMgr().Hist( "LJetPt4" )->Fill( LJetPt4, weight );
        CompMgr().Hist( "LLepPt1" )->Fill( LLepPt1, weight );
        CompMgr().Hist( "LLepPt2" )->Fill( LLepPt2, weight );
        CompMgr().Hist( "LLepPt3" )->Fill( LLepPt3, weight );
        CompMgr().Hist( "LLepPt4" )->Fill( LLepPt4, weight );
    }
   
    if( !is_data ){
    CompMgr().WeightMC( sample );
    cout << ">>Weighting " << sample << endl;
    }
    
    StoreCompare();
    delete ch;
}
