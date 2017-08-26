#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"
#include <climits>
#include <iostream>
using namespace std;
using namespace sel;

/*******************************************************************************
 Vertex selection

 **** vertex information

 https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookPATExampleTopQuarks
*******************************************************************************/
bool SelectionMgr::IsFake() {
    return vtx.isFake[_idx];
}

bool SelectionMgr::IsOfflinePV() {
    return ( vtx.Type[_idx] == 1 );
}

bool SelectionMgr::passNdof() {
    return ( vtx.Ndof[_idx] > 4 );
}

bool SelectionMgr::passAbsZ() {
    return ( fabs( vtx.z[_idx] ) < 24 );
}

bool SelectionMgr::passRho() {
    return ( vtx.Rho[_idx] < 2 );
}
