#include "CPVAnalysis/BaseLineSelector/interface/Selector.h"

/*******************************************************************************
*  FullCut
*******************************************************************************/
extern Selector& CompMgr( const std::string& = "", const std::string& = "" );
extern void MakeFullCut();
extern void FullCut();
extern void StoreCompare();
extern void AddHist();
extern void WeightMC(const std::string&);
extern double GetSF(TH2*, const int&);
extern TH2* GetSFHist(const std::string&);

extern void PlotPUCompare(const std::string&);
/*******************************************************************************
*  PlotCompare
*******************************************************************************/
extern Selector& PlotMgr( const std::string& = "", const std::string& = "" );
extern void MakePlotCompare();
extern void MergeMC();
extern void CleanMC();
extern std::vector<TH1D*> ExtractMC(const std::string&);
extern TH1D* ExtractData(const std::string&); 
extern double GetErrSum(TH1D*);

extern std::string GetName(TH1D*);
extern void PlotCompare(vector<TH1D*>, TH1D*, const std::string&, bool = false);

