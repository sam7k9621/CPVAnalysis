#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"
#include "TChain.h"
#include <vector>
/*******************************************************************************
*   Global variables
*******************************************************************************/

extern sel::SelectionMgr smgr;

/*******************************************************************************
*   Global function
*******************************************************************************/
extern void process(int, int);
/**********************
*   PreCut            *
**********************/

extern bool passMuTight();
extern bool preJet();

extern void InitPreCut(TChain*, char*);
extern void MakePreCut(TChain*, char*);

/**********************
*   JetLepCut         *
**********************/

extern bool passElLoose();
extern bool passMuLoose();
extern bool passVertex();
extern bool passJet();
extern bool passBJet();
extern std::vector<int>* getTightMuidx();
extern std::vector<TLorentzVector>* getTightMu(const std::vector<int>&);
extern std::vector<TLorentzVector>* getJet(const std::vector<int>&);

extern void MakeFullCut(TChain*);

