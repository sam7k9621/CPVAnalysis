#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern void
CheckAcp()
{
    // Initialize file
    string sample            = CompMgr().GetOption<string>( "sample" );
    vector<string> samplelst = CompMgr().GetSubListData<string>( sample, "path" );
    TChain* ch               = new TChain( "bprimeKit/root" );

    for( const auto& s : samplelst ){
        ch->Add( s.c_str() );
    }

    CompMgr().AddSample( sample, ch );
    AddHist();

    // Looping events
    int events   = CompMgr().CheckOption( "test" ) ? 10000 : ch->GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;

    for( int i = 0; i < events; i++ ){
        ch->GetEntry( i );
        CompMgr().process( events, i );

        /*******************************************************************************
        *  Gen-level Acp
        *******************************************************************************/
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

            CompMgr().Hist( "GenObs12" )->Fill( mco12 / 1000000. );
            CompMgr().Hist( "GenObs6" )->Fill( mco6 / 1000000. );
            CompMgr().Hist( "GenObs13" )->Fill( mco13 / 1000000. );
            CompMgr().Hist( "GenObs3" )->Fill( mco3 / 1000000. );

            // CompMgr().DumpEvtInfo();
        }
    }

    StoreCompare();

    delete ch;
}
