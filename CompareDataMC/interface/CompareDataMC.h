#include "CPVAnalysis/BaseLineSelector/interface/Selector.h"

/*******************************************************************************
*  FullCut
*******************************************************************************/
extern Selector& CompMgr( const std::string& = "", const std::string& = "" );
extern void MakeFullCut();
extern void FullCut();
extern void StoreCompare();
extern void AddHist();
extern void WeightMC(const string&);

/*******************************************************************************
*  PlotCompare
*******************************************************************************/
extern Selector& PlotMgr( const std::string& = "", const std::string& = "" );
extern void PlotCompare();
extern void MergeMC();
extern vector<TH1D*> ExtractMC(const string&);
extern TH1D* ExtractData(const string&); 
extern double GetErrSum(TH1D*);

extern void PlotMass(vector<TH1D*>, TH1D*);
