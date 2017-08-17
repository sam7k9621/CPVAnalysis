#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"
#include "CPVAnalysis/SelectionInfo/interface/SelectionInfo.h"
#include "TH1.h"
#include <tuple>

/*******************************************************************************
*   Global variables
*******************************************************************************/

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

/**********************
*   MassReco    
**********************/

/* Data */
extern bool passChi2Upper(const double&, const double& = 0);
extern bool passFullJet(std::vector<int>&, std::vector<int>&, const int&);
extern bool passFullMuon(std::vector<int>&);
extern std::tuple<double,double,int> 
getChi2Info(const int&, const std::vector<int>&, const std::vector<int>&);

/* MC */
extern bool passMCJet(std::vector<int>&, std::vector<int>&);
extern bool passMCMuon(std::vector<int>&);


extern void Chi2Dist();

/**********************
*   FullCut       
**********************/

extern bool isIsolated(const int&, const int&);
extern bool fillBhandle();
extern void setHist(TH1*, const std::string&, const std::string&);
extern int  bbarSeparation(const int&, const int&, const int&);

extern void MakeFullCut();

/**********************
*   CheckJet        
**********************/

extern bool hasCommonT_lep(const int&, const int&);
extern bool hasCommonT_jet(const int&, const int&);
extern void CheckJet();
extern int  MCTruthCut();

/**********************
*   CheckParticle    
**********************/

extern void checkParticle();
extern bool checkComponent();








