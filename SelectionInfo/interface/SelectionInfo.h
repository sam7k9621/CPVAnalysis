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

extern bool passFullVertex();
extern bool passFullHLT();
extern bool passFullMuon(std::vector<int>&);
extern bool passFullJet(std::vector<int>&,std::vector<int>&);
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


extern std::vector<int>* getTightMuidx();
extern std::vector<TLorentzVector>* getTightMu(const std::vector<int>&);
extern std::vector<TLorentzVector>* getJet(const std::vector<int>&);

extern void MakeFullCut();

/**********************
*   CheckJet          *
**********************/
extern bool passMCJet(std::vector<int>&, std::vector<int>&);
extern bool passMCMuon(std::vector<int>&);
extern bool hasCommonT_lep(const int&, const int&);
extern bool hasCommonT_jet(const int&, const int&);
extern void CheckJet();
extern int   MCTruthCut();

/**********************
*   CheckJet          *
**********************/

extern void checkParticle();
extern bool checkComponent();




















