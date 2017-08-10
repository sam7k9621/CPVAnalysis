#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
using namespace sel;

/*******************************************************************************
*   Global variable
*******************************************************************************/


/*******************************************************************************
*   Global function
*******************************************************************************/

extern SelectionMgr& PreMgr(){

    static SelectionMgr premgr("SelectionInfo");
    return premgr;
}

extern SelectionMgr& SelMgr(){

    static SelectionMgr selmgr("MassReco");
    return selmgr;
}



extern void process(int total, int progress){
    
    printf("[%d|%d]\r",total,progress);
    fflush(stdout);
}

