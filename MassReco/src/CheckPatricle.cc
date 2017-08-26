#include "CPVAnalysis/MassReco/interface/MassReco.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1.h"
#include <algorithm>


using namespace std;
using namespace sel;
using namespace dra;

extern bool checkComponent() {
    int countb = 0;
    int countw = 0;
    int countm = 0;

    for( int i = 0; i < SelMgr().gsize(); i++ ) {
        SelMgr().SetIndex( i );

        if( SelMgr().isXGenParticle( i, 13 ) ) {
            countm++;
            continue;
        }

        if( SelMgr().isXGenParticle( i, 5 ) ) {
            countb++;
            continue;
        }

        if( SelMgr().isXGenParticle( i, 24 ) ) {
            countw++;
        }
    }

    return ( countb >= 2 && countw >= 2 && countm >= 1 );
}

extern void checkParticle() {
    bool is_data = SelMgr().GetOption<string>( "source" ) == "data" ? 1 : 0 ;
    string source = is_data ? "data" : "mc";
    vector<string> sourcelst = GetList<string>( "path", SelMgr().GetSubTree( source ) );
    TChain* ch = new TChain( "root" );

    for( auto& i : sourcelst ) {
        ch->Add( i.c_str() );
    }

    SelMgr().SetRoot( ch );
    int count = 0;

    for( int i = 0; i < ch->GetEntries(); i++ ) {
        ch->GetEntry( i );
        process( ch->GetEntries(), i );

        if( checkComponent() ) {
            count++;
        }
    }

    cout << endl << ch->GetEntries() << endl;
    cout << endl << count << endl;
}
