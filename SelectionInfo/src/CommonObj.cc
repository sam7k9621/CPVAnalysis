#include "Selection/SelectionInfo/interface/SelectionInfo.h"

using namespace std;
using namespace sel;
/*******************************************************************************
*   Global variable
*******************************************************************************/

SelectionMgr smgr;

/*******************************************************************************
*   Global function
*******************************************************************************/

extern void process(int total, int progress){
    
    printf("[%d|%d]\r",total,progress);
    fflush(stdout);

}
