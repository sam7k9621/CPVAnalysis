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
    int events   = CompMgr().CheckOption( "test" ) ? 10000 : CompMgr().GetEntries();
    bool is_data = ( sample == "Data" ) ? 1 : 0;

    for( int i = 0; i < events; i++ ){
        CompMgr().GetEntry( i );
        CompMgr().process( events, i );

        /*******************************************************************************
        *  Gen-level Acp
        *******************************************************************************/
        if( !is_data ){
            
            int genbidx    = CompMgr().GetGenJet(5);
            int genbbaridx = CompMgr().GetGenJet(-5);
            int genlepidx  = CompMgr().GetGenLepton();
            int genjetidx  = CompMgr().GetGenHardJet();

            int mccharge = CompMgr().GetPdgID( genlepidx ) > 0 ? -1 : 1;
            // non-matched gen-level
            if( genbidx == -1 || genbbaridx == -1 || genlepidx == -1 || genjetidx == -1 )
                continue;

            TLorentzVector mcbjet    = CompMgr().GetGenP4( genbidx );
            TLorentzVector mcbbarjet = CompMgr().GetGenP4( genbbaridx );
            TLorentzVector mcisolep  = CompMgr().GetGenP4( genlepidx );
            TLorentzVector mchardjet = CompMgr().GetGenP4( genjetidx );

            // In Lab frame
            double mco2 = Obs2( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );
            double mco4 = Obs4( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );
            double mco7 = Obs7( mcbjet.Vect(), mcbbarjet.Vect() );

            // In bbar CM frame
            TVector3 mcbbCM = -( mcbjet + mcbbarjet ).BoostVector();
            mcbjet.Boost( mcbbCM );
            mcbbarjet.Boost( mcbbCM );
            mcisolep.Boost( mcbbCM );
            mchardjet.Boost( mcbbCM );
            
            double mco3 = Obs3( mcisolep.Vect(), mchardjet.Vect(), mcbjet.Vect(), mcbbarjet.Vect(), mccharge );

            CompMgr().Hist( "GenObs2" )->Fill( mco2 / 1000000. );
            CompMgr().Hist( "GenObs3" )->Fill( mco3 / 1000000. );
            CompMgr().Hist( "GenObs4" )->Fill( mco4 / 1000000. );
            CompMgr().Hist( "GenObs7" )->Fill( mco7 / 1000000. );
    
            //CompMgr().DumpEvtInfo();
        }
    }

    StoreCompare();

    delete ch;
}
