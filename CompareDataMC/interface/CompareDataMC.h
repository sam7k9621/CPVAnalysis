#ifndef COMPAREDATAMC_H
#define COMPAREDATAMC_H

#include "CPVAnalysis/SampleMgr/interface/Histor.h"
#include <map>
#include <deque>
/*******************************************************************************
*  bbSeparation
*******************************************************************************/
extern void bbSeparation();

/*******************************************************************************
*  Add Acp
*******************************************************************************/
extern void ReweighAcp();
extern void FillWeighObservable( const string&, const double&, const bool& );

extern void AddAcp();
extern void FillObserable( 
        const std::string&,
        const std::vector<int>&,
        const std::map<int, TLorentzVector>&,
        const std::map<int, TLorentzVector>&,
        const std::map<int, TLorentzVector>&,
        const std::map<int, TLorentzVector>&,
        const std::map<int, int>&
        );

extern constexpr unsigned int StoI(const char*, const int& = 0);
extern std::deque<TLorentzVector> CopyEvent( const std::vector<int>&, const std::map<int, TLorentzVector>& ); 
extern std::deque<int> CopyEvent( const std::vector<int>&, const std::map<int, int>& ); 
extern void ShuffleEvent( std::deque<TLorentzVector>& );
/*******************************************************************************
*  Check Acp
*******************************************************************************/
extern void CheckAcp();

/*******************************************************************************
*  CEDMPlot
*******************************************************************************/
struct Particle 
{
    float* px;
    float* py;
    float* pz;
    float* energy;
};

extern double ErrorEstimate( const double&, const double& );
extern void CEDMPlot();
extern int FindMC( const int&, const int*, const int& );
extern int FindParticle( const int&, const int*, const std::vector<int>& );
extern TLorentzVector GetLorentzVector( const Particle&, const int& );
extern bool PtSelect( const TLorentzVector&, const double& );
extern bool EtaSelect( const TLorentzVector&, const double& );
extern double DeltaR( const TLorentzVector&, const TLorentzVector& );
extern int GetIndex( const int&, const int*, const std::vector<int>& );

/*******************************************************************************
*  MakeHist
*******************************************************************************/
extern Histor& CompMgr( const std::string& = "", const std::string& = "" );
extern TH2D*  GetLepSF(const std::string&);
extern void    MakeHist();
extern void    CheckHist();

/*******************************************************************************
*  AddHist
*******************************************************************************/
extern void    StoreCompare();
extern void    AddHist();

/*******************************************************************************
*  Observable
*******************************************************************************/
extern double Obs2( TVector3, TVector3, TVector3, TVector3 );
extern double Obs3( TVector3, TVector3, TVector3, TVector3, float );
extern double Obs4( TVector3, TVector3, TVector3, TVector3, float );
extern double Obs7( TVector3, TVector3 );

/*******************************************************************************
*  PlotComparison
*******************************************************************************/
extern Histor&            PlotMgr( const std::string& = "", const std::string& = "" );
extern void               MakePlotCompare();

/*******************************************************************************
*  PlotTool
*******************************************************************************/
extern std::vector<TH1D*> ExtractMC( const std::string& );
extern std::vector<TH2D*> ExtractMC2D( const std::string& );
extern TH1D*              ExtractData( const std::string&, std::string="" );
extern std::string        GetFilename();
extern double             GetErrSum( TH1D* );
extern void        SetYTitle( TH1* ); 
extern std::string GetName( TH1D* );
extern TH1D*       SumHist( std::vector<TH1D*> );
extern TH2D*       SumHist2D( std::vector<TH2D*> );

/*******************************************************************************
*  PlotHist
*******************************************************************************/
extern void        PlotCompare( vector<TH1D*>, TH1D*, const std::string&, const std::string& );
extern void        PlotMC( std::vector<TH1D*>, const std::string&, const std::string& );
extern void        PlotPDF(
    std::vector<TH1D*>,
    std::vector<TH1D*>,
    std::vector<TH1D*>
    );
extern void PlotIntegral(
    TH1D*,
    TH1D*,
    TH1D*
    );
extern void Plot2D( std::vector<TH2D*> );
extern void GetYield( std::vector<TH1D*>, TH1D*);
extern void TestBGLike( std::vector<TH1D*>, TH1D*, TH1D* ); 

extern void PlotOpt( std::vector<TH1D*>, std::vector<TH1D*>, std::vector<TH1D*> );
extern TH1D* EffHist( TH1D*, TH1D* );
extern void GetAcp( const std::vector<std::string>& );

extern double GetNorm();
extern void GetSubtractAcp( const std::vector<std::string>& );

#endif
