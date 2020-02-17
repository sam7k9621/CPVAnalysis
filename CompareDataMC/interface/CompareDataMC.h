#ifndef COMPAREDATAMC_H
#define COMPAREDATAMC_H

#include "CPVAnalysis/SampleMgr/interface/Histor.h"
#include <boost/python.hpp>
#include <deque>
#include <map>

/*******************************************************************************
*  bbSeparation
*******************************************************************************/
extern void bbSeparation();

/*******************************************************************************
*  Add Acp
*******************************************************************************/
extern void ReweighAcp();
extern void FillWeighObservable( const string&, const double&, const bool& = false );
extern void FillMoreObservable( const string&, const double&, const bool& = false );
extern void SetTLV( TLorentzVector*, const TLorentzVector& );
extern void StoreTLV();

/*******************************************************************************
*  Check Acp
*******************************************************************************/
extern void CheckAcp();

/*******************************************************************************
*  MakeHist
*******************************************************************************/
extern Histor& CompMgr( const std::string& = "", const std::string& = "" );
extern std::string MakeFileName( const std::string&, const std::string&, const std::string&, const std::string& ="", const std::string& =""); 
extern void FillObservable( const std::string&, const double&, const double& =1. );
extern void    MakeHist();


/*******************************************************************************
*  AddHist
*******************************************************************************/
extern void StoreCompare();
extern void AddHist();

/*******************************************************************************
*  Observable
*******************************************************************************/
extern double Obs13( TVector3, TVector3, TVector3, TVector3, float );
extern double Obs6( TVector3, TVector3, TVector3, TVector3, float );
extern double Obs3( TVector3, TVector3, TVector3, TVector3, float );
extern double Obs12( TVector3, TVector3 );
#endif
