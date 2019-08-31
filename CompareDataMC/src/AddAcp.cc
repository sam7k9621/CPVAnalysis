#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include <boost/format.hpp>
#include <cstdlib>
#include <ctime>

using namespace std;

extern void
ReweighAcp()
{
    // Initialize file
    boost::format fmt( "/afs/cern.ch/work/p/pusheng/CMSSW_9_4_13/src/CPVAnalysis/CompareDataMC/results/Hist_%1%_%2%_chi2_20_opt_150_TLV.root" );
    string lepton  = CompMgr().GetOption<string>( "lepton" );
    string sample  = CompMgr().GetOption<string>( "sample" );
    bool   is_data = ( sample == "Data" ) ? 1 : 0;
    fmt % lepton % sample;
    srand( 100 );
    
    TChain* ch  = new TChain( "root" ); 
    ch->Add( fmt.str().c_str() );

    CompMgr().AddSample( sample, ch );
    AddHist();
    
    // Register TLV
    Float_t charge;
    TLorentzVector* b = 0;
    TLorentzVector* bbar = 0;
    TLorentzVector* isolep = 0;
    TLorentzVector* hardjet = 0;
    ch->SetBranchAddress( "charge",     &charge );
    ch->SetBranchAddress( "b",          &b );
    ch->SetBranchAddress( "bbar",       &bbar );
    ch->SetBranchAddress( "isolep",     &isolep );
    ch->SetBranchAddress( "hardjet",    &hardjet );

    Float_t mccharge;
    TLorentzVector* mcb = 0;
    TLorentzVector* mcbbar = 0;
    TLorentzVector* mcisolep = 0;
    TLorentzVector* mchardjet = 0;
    ch->SetBranchAddress( "mccharge",     &mccharge );
    ch->SetBranchAddress( "mcb",          &mcb);
    ch->SetBranchAddress( "mcbbar",       &mcbbar );
    ch->SetBranchAddress( "mcisolep",     &mcisolep );
    ch->SetBranchAddress( "mchardjet",    &mchardjet );
    
    // Shuffle list
    vector< TLorentzVector > blst;
    vector< TLorentzVector > jlst; 

    // Looping events
    int events = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        blst.push_back( *b );
        jlst.push_back( *hardjet );
    }
   
    // Looping events
    bool acp13 = false;
    bool acp6  = false;
    bool acp12 = false;
    bool acp3  = false;
    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        CompMgr().process( events, i );

        /*******************************************************************************
        *  Gen-level Acp
        *******************************************************************************/
        if( !is_data ){
            // In Lab frame
            double mco13 = Obs13( mcisolep->Vect(), mchardjet->Vect(), mcb->Vect(), mcbbar->Vect(), mccharge );
            double mco6  = Obs6 ( mcisolep->Vect(), mchardjet->Vect(), mcb->Vect(), mcbbar->Vect(), mccharge );
            double mco12 = Obs12( mcb->Vect(), mcbbar->Vect() );
            
            // In bbar CM frame
            TVector3 mcbbCM = -( *mcb + *mcbbar ).BoostVector();
            mcb      ->Boost( mcbbCM );
            mcbbar   ->Boost( mcbbCM );
            mcisolep ->Boost( mcbbCM );
            mchardjet->Boost( mcbbCM );

            double mco3 = Obs3( mcisolep->Vect(), mchardjet->Vect(), mcb->Vect(), mcbbar->Vect(), mccharge );

            FillWeighObservable( "GenObs13", mco13 );
            FillWeighObservable( "GenObs6",  mco6  );
            FillWeighObservable( "GenObs12", mco12 );
            FillWeighObservable( "GenObs3",  mco3  );
            
            acp13 = mco13 > 0;
            acp6  = mco6 > 0;
            acp12 = mco12 > 0;
            acp3  = mco3 > 0;
        }
        /*******************************************************************************
        *  Reco Acp
        *******************************************************************************/
        int idx = i;
        if( CompMgr().CheckOption( "mixed" ) ){
            int mix = CompMgr().GetOption<int>( "mixed" );
            idx = mix > 0 ? ( i + mix ) % events :  ( i + mix + events ) % events;
        }

        // In Lab frame
        double o13 = Obs13( isolep->Vect(), jlst[ idx ].Vect(), blst[ idx ].Vect(), bbar->Vect(), charge );
        double o6  = Obs6 ( isolep->Vect(), jlst[ idx ].Vect(), blst[ idx ].Vect(), bbar->Vect(), charge );
        double o12 = Obs12( blst[ idx ].Vect(), bbar->Vect() );

        // In bbar CM frame
        TVector3 bbCM = -( blst[ idx ] + *bbar ).BoostVector();
        blst[ idx ].Boost( bbCM );
        bbar       ->Boost( bbCM );
        isolep     ->Boost( bbCM );
        jlst[ idx ].Boost( bbCM );

        double o3 = Obs3( isolep->Vect(), jlst[ idx ].Vect(), blst[ idx ].Vect(), bbar->Vect(), charge );

        /*******************************************************************************
        *  Fake Acp
        *******************************************************************************/
        FillMoreObservable( "Obs13",    o13,   acp13 );
        FillMoreObservable( "Obs6",     o6,    acp6 );
        FillMoreObservable( "Obs12",    o12,   acp12 );
        FillMoreObservable( "Obs3",     o3,    acp3 );
    }
    cout<<endl;
    StoreCompare();
    delete ch;
}

extern void 
SetTLV( TLorentzVector* t1, const TLorentzVector& t2 )
{
    t1->SetPxPyPzE( t2.Px(), t2.Py(), t2.Pz(), t2.E() );
}

extern void 
FillMoreObservable( const string& obs, const double& acp, const bool& flag )
{
    double val = acp > 0 ? 0.5 : -0.5;
    CompMgr().Hist( obs )->Fill( val );
   
    if( flag ){
        int x   = rand() % ( 99 - 0 + 1 ) + 0;
        int weight = CompMgr().GetOption<int>( "Acp" );
        if( x < weight ){
            CompMgr().Hist( obs )->Fill( val );
        }
    }
}

extern void
FillWeighObservable( const string& obs, const double& acp, const bool& flag )
{
    double weight = CompMgr().GetOption<int>( "Acp" ) / 100.;
    double val    = acp > 0 ? 0.5 : -0.5;
    weight = flag ? 1 + weight : 1 - weight;
    CompMgr().Hist( obs )->Fill( val, weight );
}

extern void
StoreTLV()
{
    string filename = CompMgr().GetResultsName( "root", "Hist" );
    TFile* myfile = TFile::Open( filename.c_str(), "RECREATE" );
    TTree *tree = new TTree( "root", "root" );
    
    // Initialize file
    string lepton            = CompMgr().GetOption<string>( "lepton" );
    string sample            = CompMgr().GetOption<string>( "sample" );
    vector<string> samplelst = CompMgr().GetVParam<string>( sample, lepton + "path" );
    TChain* ch               = new TChain( "root" );
    for( const auto& s : samplelst ){
        ch->Add( s.c_str() );
        cout<<"> Processing " + s <<endl;
    }
    CompMgr().AddSample( sample, ch );
    
    // Register reco sample
    Int_t jet1;
    Int_t jet2;
    Int_t had_b;
    Int_t lep_b;
    Int_t lep;
    Float_t chi2mass;
    Float_t lep_tmass;

    ch->SetBranchAddress( "jet1",      &jet1 );
    ch->SetBranchAddress( "jet2",      &jet2 );
    ch->SetBranchAddress( "had_b",     &had_b );
    ch->SetBranchAddress( "lep_b",     &lep_b );
    ch->SetBranchAddress( "lep",       &lep );
    ch->SetBranchAddress( "chi2mass",  &chi2mass );
    ch->SetBranchAddress( "lep_tmass", &lep_tmass );

    // Register tree branch
    float charge          ; 
    TLorentzVector* b         = new TLorentzVector(0.,0.,0.,0.); 
    TLorentzVector* bbar      = new TLorentzVector(0.,0.,0.,0.); 
    TLorentzVector* isolep    = new TLorentzVector(0.,0.,0.,0.); 
    TLorentzVector* hardjet   = new TLorentzVector(0.,0.,0.,0.); 
    float mccharge;
    TLorentzVector* mcb       = new TLorentzVector(0.,0.,0.,0.); 
    TLorentzVector* mcbbar    = new TLorentzVector(0.,0.,0.,0.); 
    TLorentzVector* mcisolep  = new TLorentzVector(0.,0.,0.,0.); 
    TLorentzVector* mchardjet = new TLorentzVector(0.,0.,0.,0.);
  
    tree->Branch( "charge", &charge, "charge/F" );
    tree->Branch( "b",          "TLorentzVector", &b );
    tree->Branch( "bbar",       "TLorentzVector", &bbar );
    tree->Branch( "isolep",     "TLorentzVector", &isolep );
    tree->Branch( "hardjet",    "TLorentzVector", &hardjet );
    tree->Branch( "mccharge", &mccharge, "mccharge/F" );
    tree->Branch( "mcb",        "TLorentzVector", &mcb );
    tree->Branch( "mcbbar",     "TLorentzVector", &mcbbar );
    tree->Branch( "mcisolep",   "TLorentzVector", &mcisolep );
    tree->Branch( "mchardjet",  "TLorentzVector", &mchardjet );

    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;

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
        
        /*******************************************************************************
        * Leptonic tmass optimization cut
        *******************************************************************************/
        if( CompMgr().CheckOption( "opt" ) ){
            if( lep_tmass > CompMgr().GetOption<double>( "opt" ) ){
                continue;
            }
        }

        /*******************************************************************************
        *  Gen-level Acp
        *******************************************************************************/
        if( !is_data ){
            int genbidx    = CompMgr().GetGenJet( 5 );
            int genbbaridx = CompMgr().GetGenJet( -5 );
            int genlepidx  = CompMgr().GetGenLepton();
            int genjetidx  = CompMgr().GetGenHardJet();

            // non-matched gen-level
            if( genbidx == -1 || genbbaridx == -1 || genlepidx == -1 || genjetidx == -1 ){
                continue;
            }
            
            mccharge  = CompMgr().GetPdgID( genlepidx ) > 0 ? -1. : 1.;
            SetTLV( mcb,        CompMgr().GetGenP4( genbidx ) );
            SetTLV( mcbbar,     CompMgr().GetGenP4( genbbaridx ) );
            SetTLV( mcisolep,   CompMgr().GetGenP4( genlepidx ) );
            SetTLV( mchardjet,  CompMgr().GetGenP4( genjetidx ) );
        }
        /*******************************************************************************
        *  Reco Acp
        *******************************************************************************/
        charge  = CompMgr().GetIsoLepCharge( lep );
        SetTLV( isolep, CompMgr().GetLepP4( lep ) );
        SetTLV( hardjet, CompMgr().GetJetP4( jet1 ) );
        charge < 0 ? SetTLV( b, CompMgr().GetJetP4( had_b ) ) : SetTLV( b, CompMgr().GetJetP4( lep_b ) );
        charge < 0 ? SetTLV( bbar, CompMgr().GetJetP4( lep_b ) ) : SetTLV( bbar, CompMgr().GetJetP4( had_b ) );
        //blst.push_back( b );
        //jlst.push_back( hardjet );

        tree->Fill();
    }
    cout<<endl; 

    tree->Write();
    delete myfile;
    delete ch;
    delete b        ;     
    delete bbar     ;     
    delete isolep   ;     
    delete hardjet  ;     
    delete mcb      ;     
    delete mcbbar   ;     
    delete mcisolep ;     
    delete mchardjet;    
}
