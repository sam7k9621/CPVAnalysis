#ifndef SELECTION_H
#define SELECTION_H

#include "CPVAnalysis/BaseLineSelector/interface/Selector.h"

/*******************************************************************************
*   Global function
*******************************************************************************/
extern Selector& PreMgr( const std::string& = "", const string& = "" );
extern Selector& FullMgr( const std::string& = "", const string& = "" );

/*******************************************************************************
*   PreCut
*******************************************************************************/
extern void MakePreCut();
extern void PreCut( bool );

/*******************************************************************************
*   PreCut
*******************************************************************************/
extern void MakeFullCut();
extern void FullCut();

#endif
