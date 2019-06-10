#include "CPVAnalysis/SampleMgr/interface/SampleMgr.h"
#include <boost/format.hpp>
#include <climits>
#include <iostream>
#include <list>
using namespace std;

namespace mgr{
    int
    SampleMgr::GetPdgID( const int& idx )
    {
        return _gen.PdgID[ idx ];
    }

    int
    SampleMgr::GetGenJetID( const int& idx )
    {
        int i = MCTruthJet( idx );

        if( i < 0 ){
            return 0;// PDG id 0
        }
        else{
            return _gen.PdgID[ i ];
        }
    }

    int
    SampleMgr::MCTruthJet( const int& idx )
    {
        // Using GenParton because GenParton represents the quark level
        // (to identify b quark)
        return MatchGenlevel( _jet.GenEta[ idx ], _jet.GenPhi[ idx ] );
    }

    int
    SampleMgr::MCTruthLep( const int& idx )
    {
        return MatchGenlevel( _lep.GenEta[ idx ], _lep.GenPhi[ idx ] );
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

        if( idx > 0 ){
            idx = AvoidDuplicate( idx );
        }
        return idx;
    }

    TLorentzVector
    SampleMgr::GetMCP4( const int& i )
    {
        TLorentzVector tl;
        tl.SetPtEtaPhiM( _gen.Pt[ i ], _gen.Eta[ i ], _gen.Phi[ i ], _gen.Mass[ i ] );
        return tl;
    }

    int
    SampleMgr::FindJet( const int& x )
    {
        int idx = -1;

        for( int i = 0; i < Gsize(); i++ ){
            if( _gen.PdgID[ i ] == x ){
                if( fabs( _gen.Mo1PdgID[ i ] ) == 6 || fabs( _gen.Mo2PdgID[ i ] ) == 6 ){
                    idx = i;
                    break;
                }
            }
        }

        if( idx > 0 ){
            idx = AvoidDuplicate( idx );
        }
        return idx;
    }

    int
    SampleMgr::FindLepton()
    {
        int idx = -1;

        for( int i = 0; i < Gsize(); i++ ){
            int id = abs( _gen.PdgID[ i ] );

            // Lepton
            if( id == 13 || id == 11 ){
                int moid1 = abs( _gen.PdgID[ GetDirectMo1( i ) ] );
                int moid2 = abs( _gen.PdgID[ GetDirectMo2( i ) ] );

                // Comes from W/Z
                if( moid1 == 23 || moid1 == 24 || moid2 == 23 || moid2 == 24 ){
                    idx = i;
                    break;
                }
            }
        }

        if( idx > 0 ){
            idx = AvoidDuplicate( idx );
        }
        return idx;
    }

    int
    SampleMgr::FindHardJet()
    {
        vector<tuple<int, float> > jetlst;
        list<int> targetlst = { 1, 2, 3, 4 };
        int idx             = -1;

        for( int i = 0; i < Gsize(); i++ ){
            int id = abs( _gen.PdgID[ i ] );

            // Jet
            for( const auto t : targetlst ){
                if( id == t ){
                    int moid1 = abs( _gen.PdgID[ GetDirectMo1( i ) ] );
                    int moid2 = abs( _gen.PdgID[ GetDirectMo2( i ) ] );

                    // Comes from W
                    if( moid1 == 24 || moid2 == 24 ){
                        idx = i;
                        idx = AvoidDuplicate( idx );
                        jetlst.push_back( make_tuple( idx, _gen.Pt[ idx ] ) );
                        targetlst.remove( t );
                        break;
                    }
                }
            }
        }

        if( jetlst.empty() ){
            return -1;
        }

        // sorting by Pt (s->l)
        std::sort(
            begin( jetlst ),
            end( jetlst ),
            [ ]( const auto& t1, const auto& t2 ){
            return get<1>( t1 ) < get<1>( t2 );
        }
            );

        // return highest pt jet
        return get<0>( jetlst.back() );
    }

    void
    SampleMgr::DumpEvtInfo()
    {
        for( int i = 0; i < Gsize(); i++ ){
            int id = abs( _gen.PdgID[ i ] );

            if( id == 6 || id == 5 || id == 24 || id == 1 || id == 2 || id == 3 || id == 4 || id == 11 || id == 13 ){
                boost::format fmt( "[%-2i]%-4i | [%-2i]%-4i [%-2i]%-4i" );
                fmt % i % _gen.PdgID[ i ];
                fmt % _gen.Da1[ i ] % _gen.Da1PdgID[ i ];
                fmt % _gen.Da2[ i ] % _gen.Da2PdgID[ i ];
                cout << fmt << endl;
            }
        }
    }

    int
    SampleMgr::AvoidDuplicate( const int& idx )
    {
        int id = _gen.PdgID[ idx ];
        int i  = idx;

        while( _gen.Mo1PdgID[ i ] == id || _gen.Mo2PdgID[ i ] == id ){
            i = _gen.Mo1PdgID[ i ] == id ? _gen.Mo1[ i ] : _gen.Mo2[ i ];
        }

        return i;
    }

    int
    SampleMgr::GetDirectMo1( const int& idx )
    {
        int i = idx;

        while( true ){
            if( _gen.Mo1PdgID[ i ] == _gen.PdgID[ idx ] ){
                i = _gen.Mo1[ i ];
            }
            else{
                return _gen.Mo1[ i ];
            }
        }
    }

    int
    SampleMgr::GetDirectMo2( const int& idx )
    {
        int i = idx;

        while( true ){
            if( _gen.Mo1PdgID[ i ] == _gen.PdgID[ idx ] ){
                i = _gen.Mo1[ i ];
            }
            else{
                return _gen.Mo2[ i ];
            }
        }
    }
}
