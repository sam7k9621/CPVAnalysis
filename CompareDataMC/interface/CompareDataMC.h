#include "CPVAnalysis/BaseLineSelector/interface/Selector.h"

/*******************************************************************************
*  Global function
*******************************************************************************/

extern Selector& CompMgr( const std::string& = "" );

/*******************************************************************************
*  MassReco
*******************************************************************************/
extern void MakeFullCut();
extern void FullCut();
extern void AddHist();
extern void StoreCompare();
/*******************************************************************************
*  Charege Pairing
*******************************************************************************/
extern void DrawbbSeparate();

/*******************************************************************************
*  DataMCCompare
*******************************************************************************/
extern void PlotSample();
extern bool IsXsample( const string&, const std::string&);
extern void MergeSample(std::vector<TH1D*>&, TH1D*, const string&);
extern void WeightMC(const string&, TH1D*);
