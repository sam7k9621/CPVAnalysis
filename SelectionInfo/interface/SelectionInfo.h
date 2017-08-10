#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"
#include <string>
#include <vector>

/*******************************************************************************
*   Global function
*******************************************************************************/

extern sel::SelectionMgr& SelMgr(const std::string& = "");
extern void process(int, int);

/*******************************************************************************
*   SelectionInfo
*******************************************************************************/

/* HLT */
extern bool passHLT(const std::vector<int>&);

/* Vertex */
extern bool passVertex();
extern bool passGoodPVtx();

/* Jet */
extern bool preJet();
extern bool passJet();
extern bool passBJet();

/* Muon */
extern bool passMuLoose();
extern bool passMuTight();

/* Electron */
extern bool passElLoose();
extern bool passVertex();

/**********************
*   PreCut            *
**********************/

extern void MakePreCut();
