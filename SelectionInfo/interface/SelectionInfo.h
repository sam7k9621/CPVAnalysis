#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"
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
*   FullCut         *
**********************/

extern bool passElLoose();
extern bool passMuLoose();
extern bool passVertex();
extern bool passJet();
extern bool passBJet();

extern bool passFullVertex();
extern bool passFullHLT();
extern bool passFullMuon(std::vector<int>&);
extern bool passFullJet(std::vector<int>&,std::vector<int>&);

extern bool hasMotherT(const int&, const int&, const int&);

extern std::vector<int>* getTightMuidx();
extern std::vector<TLorentzVector>* getTightMu(const std::vector<int>&);
extern std::vector<TLorentzVector>* getJet(const std::vector<int>&);

extern void MakeFullCut();

