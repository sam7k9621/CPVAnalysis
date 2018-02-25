#ifndef COMPAREDATAMC_H
#define COMPAREDATAMC_H

#include "CPVAnalysis/CompareDataMC/interface/Histor.h"

/*******************************************************************************
*  MakeHist
*******************************************************************************/
extern Histor& CompMgr( const std::string& = "", const std::string& = "" );
extern void MakeHist();
extern void StoreCompare();
extern void AddHist();
extern void WeightMC(const std::string&);
extern double GetSF(TH2*, const int&);
extern TH2* GetSFHist(const std::string&);

/*******************************************************************************
*  Observable
*******************************************************************************/
extern double Obs3( TVector3, TVector3, TVector3, float );
extern double Obs4( TVector3, TVector3, TVector3, TVector3, float );

/*******************************************************************************
*  PlotCompare
*******************************************************************************/
extern Histor& PlotMgr( const std::string& = "", const std::string& = "" );
extern void MakePlotCompare();
extern void MergeMC();
extern void CleanMC();
extern std::vector<TH1D*> ExtractMC(const std::string&);
extern std::vector<TH2D*> ExtractMC2D(const std::string&);
extern TH1D* ExtractData(const std::string&); 
extern double GetErrSum(TH1D*);

extern std::string GetName(TH1D*);
extern void PlotCompare(vector<TH1D*>, TH1D*, const std::string&);
extern TH1D* SumHist(std::vector<TH1D*>);
extern TH2D* SumHist2D(std::vector<TH2D*>);
extern void PlotMC(std::vector<TH1D*>, const std::string&);
extern void PlotPDF(
        std::vector<TH1D*>,
        std::vector<TH1D*>,
        std::vector<TH1D*>,
        std::vector<TH1D*>
        );
extern void PlotIntegral(
        TH1D*,
        TH1D*,
        TH1D*,
        TH1D*
        );
extern void Plot2D(std::vector<TH2D*>);

#endif
