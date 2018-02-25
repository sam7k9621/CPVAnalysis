#include "CPVAnalysis/SampleMgr/interface/SampleMgr.h"
#include <climits>

using namespace std;

namespace mgr{
    int
    SampleMgr::GetPartonID( const int& idx )
    {
        return _gen.PdgID[ MCTruthJet( idx ) ];
    }

    int
    SampleMgr::MCTruthJet( const int& idx )
    {
        // Using GenParton because GenParton represents the quark level
        // (to identify b quark)
        return MatchGenlevel( _jet.GenEta[ idx ], _jet.GenPhi[ idx ] );
    }

    int
    SampleMgr::MatchGenlevel( const float& eta, const float& phi )
    {
        double deltaR = INT_MAX;
        int idx       = -1;

        for( int i = 0; i < Gsize(); i++ ){
            double deta = eta - _gen.Eta[ i ];
            double dphi = Phi_mpi_pi( (double)( phi - _gen.Phi[ i ] ) );
            double delR = TMath::Sqrt( deta * deta + dphi * dphi );

            if( delR < deltaR && delR < 0.4 ){
                deltaR = delR;
                idx    = i;
            }
        }

        return idx;
    }
}
