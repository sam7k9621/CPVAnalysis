#ifndef SELECTION_H
#define SELECTION_H

#include "CPVAnalysis/SampleMgr/interface/Selector.h"
#include "CPVAnalysis/SampleMgr/interface/Histor.h"

/*******************************************************************************
*   Global function
*******************************************************************************/
extern Selector& PreMgr( const std::string&  = "", const string& = "" );
extern Selector& FullMgr( const std::string& = "", const string& = "" );

/*******************************************************************************
*   PreCut
*******************************************************************************/
extern void Discard( TChain* );
extern void MakePreCut();
extern void MakeDataCard();
extern void MakeBtagEff();

/*******************************************************************************
*   FullCut
*******************************************************************************/
extern std::string MakeFileName( bool );
extern void        MakeFullCut();

/*******************************************************************************
*  CheckBtag
*******************************************************************************/
extern std::string MakeFileName( bool );
extern void CheckBtag();
extern Histor& CompMgr( const std::string& = "", const std::string& = "" );
extern void AddHist();
extern void StoreCompare();

#endif
