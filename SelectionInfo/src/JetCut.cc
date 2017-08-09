#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"

using namespace std;
using namespace sel;


/*******************************************************************************
 * Jet selection
*******************************************************************************/

bool SelectionMgr::passJetPt(double pt ){
     return (jet.Pt[_idx] > 30);
}

bool SelectionMgr::passJetEta(double eta){
    return ( fabs(jet.Eta[_idx]) < 2.4 );
}

bool SelectionMgr::JetNConstituents(){
    return ( jet.NConstituents[_idx] > 1 );
}

bool SelectionMgr::JetNEF(){
    return ( jet.NEF[_idx] <= 0.99 );
}

bool SelectionMgr::JetNHF(){
    return ( jet.NHF[_idx] <= 0.99 );
}

bool SelectionMgr::JetCHF(){
    return ( jet.CHF[_idx] > 0 );
}

bool SelectionMgr::JetNCH(){
    return ( jet.NCH[_idx] > 0 );
}

bool SelectionMgr::JetCEF(){
    return ( jet.CEF[_idx] <= 0.99 );
}

//b-tagging 
bool SelectionMgr::JetCSVM(){
    return ( jet.pfCombinedInclusiveSecondaryVertexV2BJetTags[_idx] > 0.8484 );
}

