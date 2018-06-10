#ifndef SELECTION_H
#define SELECTION_H

#include "CPVAnalysis/SampleMgr/interface/Selector.h"

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
extern bool SetPUWeight( float&, const std::vector<double>& );
/*******************************************************************************
*   PreCut
*******************************************************************************/
extern std::string MakeFileName(bool);
extern void MakeFullCut();
extern void FullCut(bool);

extern void MakeCheckSelection();
extern void CheckSelection(bool);

#endif
