#include "Selection/SelectionInfo/interface/SelectionMgr.h"
#include <climits>
#include <iostream>
using namespace std;
using namespace sel;

/*******************************************************************************

 Vertex selection

 **** vertex information

 https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookPATExampleTopQuarks

*******************************************************************************/
bool SelectionMgr::IsFake(){

    return vtx.isFake[idx];
}

bool SelectionMgr::IsOfflinePV(){
    return (vtx.Type[idx] == 1);
}

bool SelectionMgr::passNdof(){
    return (vtx.Ndof[idx]>4 );
}

bool SelectionMgr::passAbsZ(){
    return (fabs(vtx.z[idx]) < 24);
}

bool SelectionMgr::passRho(){
    return (vtx.Rho[idx] < 2);
}

     

