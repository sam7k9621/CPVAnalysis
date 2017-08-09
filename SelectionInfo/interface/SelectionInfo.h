#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"

/*******************************************************************************
*   Global variables
*******************************************************************************/


/*******************************************************************************
*   Global function
*******************************************************************************/

extern sel::SelectionMgr& PreMgr();

extern void process(int, int);

/**********************
*   PreCut            *
**********************/

extern bool preMuon();
extern bool preJet();

extern void InitPreCut(TChain*, char*);
extern void MakePreCut(TChain*, char*);
