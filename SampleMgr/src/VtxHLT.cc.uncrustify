#include "CPVAnalysis/SampleMgr/interface/SampleMgr.h"
#include <climits>

using namespace std;

namespace mgr{
    /*******************************************************************************
    *   Vertex selection
    *   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookPATExampleTopQuarks
    *******************************************************************************/
    bool
    SampleMgr::IsFake()
    {
        return _vtx.isFake[ _idx ];
    }

    float
    SampleMgr::Ndof()
    {
        return _vtx.Ndof[ _idx ];
    }

    float
    SampleMgr::AbsZ()
    {
        return fabs( _vtx.z[ _idx ] );
    }

    float
    SampleMgr::VtxRho()
    {
        return _vtx.Rho[ _idx ];
    }

    /*******************************************************************************
    *   HLT selection
    *******************************************************************************/
    bool
    SampleMgr::PassHLT( const vector<int>& hlt )
    {
        for( const auto h : hlt ){
            if( int(_trg.TrgBook[ h ]) == 1 ){
                return true;
            }
        }

        return false;
    }
}
