#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"
#include <vector>
#include "TH1.h"
/*******************************************************************************
*   Global variables
*******************************************************************************/

extern sel::SelectionMgr smgr;

enum MatchType{

    Correct = 1 << 0,
    Fakeb   = 1 << 1, 
    Mistag  = 1 << 2, 
    Swap    = 1 << 3, 
    Other   = 1 << 4, 
    None    = 1 << 5,
    
    // 1  Correct: correct 
    // 2  Fakeb  : not b 
    // 4  Mistag : b not from t
    // 8  Swap   : b from t but charge mistag(swapped)
    // 16 Other  : (2 & 4) and only one mistag
    // 32 None   : not matched
};

/*******************************************************************************
*   Global function
*******************************************************************************/

extern void process(int, int);

/**********************
*   BaseLineSelect    *
**********************/

extern bool passElLoose();
extern bool passMuLoose();
extern bool passMuTight();
extern bool passVertex();
extern bool passJet();
extern bool passBJet();

/**********************
*   FullCut         *
**********************/

extern bool passFullJet(std::vector<int>&, std::vector<int>&, const int&);
extern bool passFullVertex();
extern bool passFullHLT(const std::vector<int>&);
extern bool passFullMuon(std::vector<int>&);

extern bool isIsolated(const int&, const int&);
extern bool fillBhandle();
extern void setHist(TH1*, const std::string&, const std::string&);
extern int  bbarSeparation(const int&, const int&, const int&);

extern void MakeFullCut();

/**********************
*   CheckJet          *
**********************/

extern bool passMCJet(std::vector<int>&, std::vector<int>&);
extern bool passMCMuon(std::vector<int>&);
extern bool hasCommonT_lep(const int&, const int&);
extern bool hasCommonT_jet(const int&, const int&);
extern void CheckJet();
extern int  MCTruthCut();

/**********************
*   CheckParticle          *
**********************/

extern void checkParticle();
extern bool checkComponent();







