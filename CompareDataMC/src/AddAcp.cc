#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include <cstdlib>
#include <ctime>

using namespace std;

extern void
ReweighAcp()
{
    // Initialize file
    string lepton            = CompMgr().GetOption<string>( "lepton" );
    string sample            = CompMgr().GetOption<string>( "sample" );
    vector<string> samplelst = CompMgr().GetSubListData<string>( sample, lepton + "path" );
    TChain* ch               = new TChain( "root" );

    for( const auto& s : samplelst ){
        ch->Add( s.c_str() );
    }

    CompMgr().AddSample( sample, ch );
    AddHist();

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

    // Looping events
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

        /*******************************************************************************
        * Leptonic tmass optimization cut
        *******************************************************************************/
        if( CompMgr().CheckOption( "Opt" ) ){
            if( lep_tmass > CompMgr().GetOption<double>( "Opt" ) ){
                continue;
            }
        }

        /*******************************************************************************
        *  Gen-level Acp
        *******************************************************************************/
        int genbidx    = CompMgr().GetGenJet( 5 );
        int genbbaridx = CompMgr().GetGenJet( -5 );
        int genlepidx  = CompMgr().GetGenLepton();
        int genjetidx  = CompMgr().GetGenHardJet();

        int mccharge = CompMgr().GetPdgID( genlepidx ) > 0 ? -1 : 1;
        // non-matched gen-level
        if( genbidx == -1 || genbbaridx == -1 || genlepidx == -1 || genjetidx == -1 ){
            continue;
        }

        TLorentzVector mcbjet    = CompMgr().GetGenP4( genbidx );
        TLorentzVector mcbbarjet = CompMgr().GetGenP4( genbbaridx );
        TLorentzVector mcisolep  = CompMgr().GetGenP4( genlepidx );
        TLorentzVector mchardjet = CompMgr().GetGenP4( genjetidx );

        // In Lab frame
        double mco13 = Obs13( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );
        double mco6  = Obs6( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );
        double mco12 = Obs12( mcbjet.Vect(), mcbbarjet.Vect() );

        // CompMgr().DumpEvtInfo();
        // In bbar CM frame
        TVector3 mcbbCM = -( mcbjet + mcbbarjet ).BoostVector();
        mcbjet.Boost( mcbbCM );
        mcbbarjet.Boost( mcbbCM );
        mcisolep.Boost( mcbbCM );
        mchardjet.Boost( mcbbCM );

        double mco3 = Obs3( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );

        bool acp13 = mco13 > 0;
        bool acp6  = mco6 > 0;
        bool acp12 = mco12 > 0;
        bool acp3  = mco3 > 0;

        /*******************************************************************************
        *  Reco Acp
        *******************************************************************************/
        float charge           = CompMgr().GetIsoLepCharge( lep );
        TLorentzVector isolep  = CompMgr().GetLepP4( lep );
        TLorentzVector hardjet = CompMgr().GetJetP4( jet1 );
        TLorentzVector b       = charge < 0 ? CompMgr().GetJetP4( had_b ) : CompMgr().GetJetP4( lep_b );
        TLorentzVector bbar    = charge < 0 ? CompMgr().GetJetP4( lep_b ) : CompMgr().GetJetP4( had_b );
        // In Lab frame
        double o13 = Obs13( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
        double o6  = Obs6( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
        double o12 = Obs12( b.Vect(), bbar.Vect() );

        // CompMgr().DumpEvtInfo();
        // In bbar CM frame
        TVector3 bbCM = -( b + bbar ).BoostVector();
        b.Boost( bbCM );
        bbar.Boost( bbCM );
        isolep.Boost( bbCM );
        hardjet.Boost( bbCM );

        double o3 = Obs3( isolep.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );

        /*******************************************************************************
        *  Fake Acp
        *******************************************************************************/
        FillWeighObservable( "Obs13",    o13,   acp13 );
        FillWeighObservable( "Obs6",     o6,    acp6 );
        FillWeighObservable( "Obs12",    o12,   acp12 );
        FillWeighObservable( "Obs3",     o3,    acp3 );
        FillWeighObservable( "GenObs13", mco13, acp13 );
        FillWeighObservable( "GenObs6",  mco6,  acp6 );
        FillWeighObservable( "GenObs12", mco12, acp12 );
        FillWeighObservable( "GenObs3",  mco3,  acp3 );
    }

    StoreCompare();
    delete ch;
}

extern void
FillWeighObservable( const string& obs, const double& acp, const bool& flag )
{
    double weight = CompMgr().GetOption<double>( "Acp" ) / 100;
    weight = flag ? 1 + weight : 1 - weight;

    CompMgr().Hist( obs )->Fill( acp / 1000000., weight );
}

extern void
AddAcp()
{
    // Initialize file
    string lepton            = CompMgr().GetOption<string>( "lepton" );
    string sample            = CompMgr().GetOption<string>( "sample" );
    vector<string> samplelst = CompMgr().GetSubListData<string>( sample, lepton + "path" );
    TChain* ch               = new TChain( "root" );
    srand( 100 );

    for( const auto& s : samplelst ){
        ch->Add( s.c_str() );
    }

    CompMgr().AddSample( sample, ch );
    AddHist();

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

    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;

    map<int, TLorentzVector> reco_b;
    map<int, TLorentzVector> reco_bbar;
    map<int, TLorentzVector> reco_lep;
    map<int, TLorentzVector> reco_jet;
    map<int, int> reco_charge;

    vector<int> acp13idx;
    vector<int> acp6idx;
    vector<int> acp12idx;
    vector<int> acp3idx;

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
        if( CompMgr().CheckOption( "Opt" ) ){
            if( lep_tmass > CompMgr().GetOption<double>( "Opt" ) ){
                continue;
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

        // Fill in list
        reco_b   [ i ]    = b;
        reco_bbar[ i ]    = bbar;
        reco_lep [ i ]    = isolep;
        reco_jet [ i ]    = hardjet;
        reco_charge [ i ] = charge;

        /*******************************************************************************
        *  Gen-level Acp
        *******************************************************************************/
        bool acp13 = false;
        bool acp6  = false;
        bool acp12 = false;
        bool acp3  = false;

        if( !is_data ){
            int genbidx    = CompMgr().GetGenJet( 5 );
            int genbbaridx = CompMgr().GetGenJet( -5 );
            int genlepidx  = CompMgr().GetGenLepton();
            int genjetidx  = CompMgr().GetGenHardJet();

            int mccharge = CompMgr().GetPdgID( genlepidx ) > 0 ? -1 : 1;
            // non-matched gen-level
            if( genbidx == -1 || genbbaridx == -1 || genlepidx == -1 || genjetidx == -1 ){
                continue;
            }

            TLorentzVector mcbjet    = CompMgr().GetGenP4( genbidx );
            TLorentzVector mcbbarjet = CompMgr().GetGenP4( genbbaridx );
            TLorentzVector mcisolep  = CompMgr().GetGenP4( genlepidx );
            TLorentzVector mchardjet = CompMgr().GetGenP4( genjetidx );

            // In Lab frame
            double mco13 = Obs13( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );
            double mco6  = Obs6( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );
            double mco12 = Obs12( mcbjet.Vect(), mcbbarjet.Vect() );

            // In bbar CM frame
            TVector3 mcbbCM = -( mcbjet + mcbbarjet ).BoostVector();
            mcbjet.Boost( mcbbCM );
            mcbbarjet.Boost( mcbbCM );
            mcisolep.Boost( mcbbCM );
            mchardjet.Boost( mcbbCM );

            double mco3 = Obs3( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );

            CompMgr().Hist( "GenObs13" )->Fill( mco13 / 1000000. );
            CompMgr().Hist( "GenObs6" )->Fill( mco6 / 1000000. );
            CompMgr().Hist( "GenObs12" )->Fill( mco12 / 1000000. );
            CompMgr().Hist( "GenObs3" )->Fill( mco3 / 1000000. );

            if( mco13 > 0 ){
                acp13 = true;
            }
            if( mco6 > 0 ){
                acp6 = true;
            }
            if( mco12 > 0 ){
                acp12 = true;
            }
            if( mco3 > 0 ){
                acp3 = true;
            }
        }

        /*******************************************************************************
        *  Fake Acp
        *******************************************************************************/
        int x   = rand() % ( 99 - 0 + 1 ) + 0;
        int acp = CompMgr().GetOption<double>( "Acp" );

        if( x < acp ){
            if( acp13 ){
                acp13idx.push_back( i );
            }
            if( acp6 ){
                acp6idx.push_back( i );
            }
            if( acp12 ){
                acp12idx.push_back( i );
            }
            if( acp3 ){
                acp3idx.push_back( i );
            }
        }
    }

    FillObservable( "Obs3",  acp3idx,  reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );
    FillObservable( "Obs6",  acp6idx,  reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );
    FillObservable( "Obs12", acp12idx, reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );
    FillObservable( "Obs13", acp13idx, reco_b, reco_bbar, reco_lep, reco_jet, reco_charge );

    StoreCompare();
    delete ch;
}

constexpr unsigned int
StoI( const char* str, const int& h )
{
    return !str[ h ] ? 5381 : ( StoI( str, h + 1 ) * 33 ) ^ str[ h ];
}

void
FillObservable( const string& obs, const vector<int>& idx, const map<int, TLorentzVector>& b, const map<int, TLorentzVector>& bbar, const map<int, TLorentzVector>& lep, const map<int, TLorentzVector>& jet, const map<int, int>& charge )
{
    deque<TLorentzVector> copy_b    = CopyEvent( idx, b );
    deque<TLorentzVector> copy_bbar = CopyEvent( idx, bbar );
    deque<TLorentzVector> copy_lep  = CopyEvent( idx, lep );
    deque<TLorentzVector> copy_jet  = CopyEvent( idx, jet );
    deque<int> copy_charge          = CopyEvent( idx, charge );

    if( CompMgr().CheckOption( "mixed" ) ){
        ShuffleEvent( copy_b );
        ShuffleEvent( copy_jet );
        // std::random_shuffle( copy_bbar.begin(), copy_bbar.end() );
    }

    for( unsigned i = 0; i < copy_b.size(); i++ ){
        double oval = 0.;

        switch( StoI( obs.c_str() ) ){
        case StoI( "Obs13" ):
            oval = Obs13( copy_lep[ i ].Vect(), copy_jet[ i ].Vect(), copy_b[ i ].Vect(), copy_bbar[ i ].Vect(), copy_charge[ i ] );
            break;

        case StoI( "Obs6" ):
            oval = Obs6( copy_lep[ i ].Vect(), copy_jet[ i ].Vect(), copy_b[ i ].Vect(), copy_bbar[ i ].Vect(), copy_charge[ i ] );
            break;

        case StoI( "Obs12" ):
            oval = Obs12( copy_b[ i ].Vect(), copy_bbar[ i ].Vect() );
            break;

        // https://stackoverflow.com/questions/5685471/error-jump-to-case-label
        case StoI( "Obs3" ):
            // In bbar CM frame (b and b~ will back-to-back, i.e vector(b)=-vector(b~))
            TVector3 bbCM = -( copy_b[ i ] + copy_bbar[ i ] ).BoostVector();
            copy_b[ i ].Boost( bbCM );
            copy_bbar[ i ].Boost( bbCM );
            copy_lep[ i ].Boost( bbCM );
            copy_jet[ i ].Boost( bbCM );

            oval = Obs3( copy_lep[ i ].Vect(), copy_jet[ i ].Vect(), copy_b[ i ].Vect(), copy_bbar[ i ].Vect(), copy_charge[ i ] );
            break;
        }

        CompMgr().Hist( obs )->Fill( oval / 1000000. );
    }
}

deque<TLorentzVector>
CopyEvent( const vector<int>& idx, const map<int, TLorentzVector>& lst )
{
    // Copy map element to vector
    deque<TLorentzVector> temp;

    for( auto l : lst ){
        temp.push_back( l.second );
    }

    // Copy events with positive Acp
    for( const auto& i : idx ){
        temp.push_back( lst.at( i ) );
    }

    return temp;
}

deque<int>
CopyEvent( const vector<int>& idx, const map<int, int>& lst )
{
    // Copy map element to vector
    deque<int> temp;

    for( auto l : lst ){
        temp.push_back( l.second );
    }

    // Copy events with poisitive Acp
    for( const auto& i : idx ){
        temp.push_back( lst.at( i ) );
    }

    return temp;
}

void
ShuffleEvent( deque<TLorentzVector>& lst )
{
    TLorentzVector temp = lst.front();
    lst.pop_front();
    lst.push_back( temp );
}
